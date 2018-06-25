#include "fbx.hpp"


#if defined(TOOLS_BUILD)    // If we have FBX IMPORTING ENABLED, make sure to include the library and header files.   
    
#include <fbxsdk.h>   
#pragma comment(lib, "libfbxsdk-md.lib")


#include "Engine\Core\ErrorWarningAssert.hpp"
#include <sstream>
#include "Engine\Core\Console.hpp"
#include <fbxsdk\core\math\fbxvector4.h>
#include "Engine\Renderer\MeshBuilder.hpp"
#include <fbxsdk\core\arch\fbxtypes.h>
#include <fbxsdk\scene\geometry\fbxskeleton.h>
#include <fbxsdk\core\fbxdatatypes.h>
#include "Engine\Math\MathUtils.hpp"
#include <algorithm>

FbxManager* FBXUtils::s_manager = nullptr;
FbxIOSettings* FBXUtils::s_ios = nullptr;
FbxImporter* FBXUtils::s_importer = nullptr;






void FBXUtils::Setup()
{
	s_manager = FbxManager::Create();
	// Create the IO settings object.
	s_ios = FbxIOSettings::Create(s_manager, IOSROOT);
	s_manager->SetIOSettings(s_ios);

	// Create an importer using the SDK manager.
	s_importer = FbxImporter::Create(s_manager, "");

}

void FBXUtils::Destroy()
{
	FBX_SAFE_DESTROY(s_importer);
	FBX_SAFE_DESTROY(s_ios);
	FBX_SAFE_DESTROY(s_manager);
}

FbxScene* FBXUtils::LoadScene(const std::string & filePath)
{
	// Use the first argument as the filename for the importer.
	Setup();

	bool result = s_importer->Initialize(filePath.c_str(), -1, s_manager->GetIOSettings());

	FbxScene *scene = FbxScene::Create(s_manager, "");

	// Import into the scene
	result = s_importer->Import(scene);
	s_importer->Destroy();
	s_importer = nullptr;

	if (result) {
		return scene;
	}

	FBX_SAFE_DESTROY(s_importer);
	FBX_SAFE_DESTROY(s_ios);
	FBX_SAFE_DESTROY(s_manager);

	return nullptr;

}

void FBXUtils::UnloadScene(FbxScene * scene)
{
	if (nullptr == scene) {
		return;
	}

	FbxManager *manager = scene->GetFbxManager();
	FbxIOSettings *io_settings = manager->GetIOSettings();

	FBX_SAFE_DESTROY(scene);
	FBX_SAFE_DESTROY(io_settings);
	FBX_SAFE_DESTROY(manager);
}

void FBXUtils::ListScene(FbxScene * scene)
{
	if (!scene)
		return;

	// Print the node
	FbxNode *root = scene->GetRootNode();
	PrintNode(root, 0);
}

void FBXUtils::PrintNode(FbxNode * node, int depth)
{
	if (!node)
		return;

	// Print the node's attributes.
	std::stringstream ss;
	for (int depthCount = 0; depthCount < depth; depthCount++)
		ss << ' ';
	ss << "FBX Node \"" << node->GetName() << "\", Depth " << depth;
	Console::Log(ss.str());
	for (int i = 0; i < node->GetNodeAttributeCount(); i++) {
		PrintAttribute(node->GetNodeAttributeByIndex(i), depth);
	}

	// Print the nodes children
	for (int32_t i = 0; i < node->GetChildCount(); ++i) {
		PrintNode(node->GetChild(i), depth + 1);
	}
}

void FBXUtils::PrintAttribute(FbxNodeAttribute * attribute, int depth)
{
	if (nullptr == attribute)
		return;

	FbxNodeAttribute::EType type = attribute->GetAttributeType();

	std::string typeName = GetAttributeTypeName(type);
	char const* attrName = attribute->GetName();

	std::stringstream ss;
	for (int depthCount = 0; depthCount < depth; depthCount++)
		ss << ' ';
	ss << "FBX type = \'" << typeName << "\", name = \"" << attrName << "\"";
	Console::Log(ss.str());
}

std::string FBXUtils::GetAttributeTypeName(FbxNodeAttribute::EType type)
{
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

bool FBXUtils::LoadMesh(const std::string& filePath, MeshBuilder& meshBuilder)
{
	// First - load the scene - code is identical to when you 
	// listed the file
	FbxScene *scene = LoadScene(filePath);
	if (nullptr == scene) {
		return false;
	}

	// Second, we want to convert the basis for consitancy
	ConvertSceneToEngineBasis(scene);

	// Third, our engine only deals with triangles, 
	// but authoring programs tend to deal with a lot of 
	// different surface patch types.  For we triangulate
	// (the process of converting every non-triangle polygon to triangles )
	TriangulateScene(scene);


	// Alright, now we can start importing data, for this we'll need a matrix stack 
	// [well, it helps, you can do this without since we'll mostly be deailing with 
	// global transforms outside of what is called the "geometric transform" which is
	// always local]

	// clear all old data before importing
	//mb->clear();

	// My matrix stack treats the top as being IDENTITY
	MatrixStack mat_stack;

	// Next, get the root node, and start importing
	FbxNode *root = scene->GetRootNode();

	// import data into our builder

	ImportMeshes(meshBuilder, root, mat_stack);

	// Clean up after ourself
	UnloadScene(scene);


	// [HACK] After loading, I flip everything across the X axis
	// for conistancy.  Would love this to be part of ConvertSceneToEngineBasis,
	// but no mattter the transform I sent, it was always flipped on X [at best]
	//
	// Doing this causes it to be match Unity/Unreal/Editor in terms of orientation
	FlipX(meshBuilder);
	return true;
}

bool FBXUtils::LoadMesh(const std::string& filePath, MeshBuilder& meshBuilder, Skeleton* skeleton)
{
	// First - load the scene - code is identical to when you 
	// listed the file
	FbxScene *scene = LoadScene(filePath);
	if (nullptr == scene) {
		return false;
	}

	// Second, we want to convert the basis for consitancy
	ConvertSceneToEngineBasis(scene);

	// Third, our engine only deals with triangles, 
	// but authoring programs tend to deal with a lot of 
	// different surface patch types.  For we triangulate
	// (the process of converting every non-triangle polygon to triangles )
	TriangulateScene(scene);


	// Alright, now we can start importing data, for this we'll need a matrix stack 
	// [well, it helps, you can do this without since we'll mostly be deailing with 
	// global transforms outside of what is called the "geometric transform" which is
	// always local]

	// clear all old data before importing
	//mb->clear();

	// My matrix stack treats the top as being IDENTITY
	MatrixStack mat_stack;

	// Next, get the root node, and start importing
	FbxNode *root = scene->GetRootNode();

	// import data into our builder

	ImportMeshes(meshBuilder, root, mat_stack, skeleton);

	// Clean up after ourself
	UnloadScene(scene);


	// [HACK] After loading, I flip everything across the X axis
	// for conistancy.  Would love this to be part of ConvertSceneToEngineBasis,
	// but no mattter the transform I sent, it was always flipped on X [at best]
	//
	// Doing this causes it to be match Unity/Unreal/Editor in terms of orientation
	FlipX(meshBuilder);
	return true;
}

void FBXUtils::ConvertSceneToEngineBasis(FbxScene * scene)
{
	fbxsdk::FbxAxisSystem local_system(
		(fbxsdk::FbxAxisSystem::EUpVector) fbxsdk::FbxAxisSystem::EUpVector::eYAxis,
		(fbxsdk::FbxAxisSystem::EFrontVector) fbxsdk::FbxAxisSystem::EFrontVector::eParityOdd,  // by default points toward me.
		(fbxsdk::FbxAxisSystem::ECoordSystem) fbxsdk::FbxAxisSystem::ECoordSystem::eRightHanded);

	FbxAxisSystem scene_system = scene->GetGlobalSettings().GetAxisSystem();
	if (scene_system != local_system) {
		local_system.ConvertScene(scene);
	}
}

void FBXUtils::TriangulateScene(FbxScene * scene)
{
	FbxGeometryConverter converter(scene->GetFbxManager());

	// Replace being true means it will destroy the old 
	// representation of the mesh after conversion.  Setting this to 
	// false would leave the old representations in.
	converter.Triangulate(scene, /*replace=*/true);
}

void FBXUtils::ImportMeshes(MeshBuilder& mb, FbxNode * node, MatrixStack & mat_stack, Skeleton* skeleton)
{
	// first, we're traversing the graph, keep track of our current world transform
	// (I will do this by by pushing directly the world transform of this node to my stack)
	// (that is, this transform becomes the new top exactly)
	Matrix4 transform = GetNodeWorldTransform(node);
	mat_stack.PushDirect(transform);

	// First, traverse the scene graph, looking for FbxMesh node attributes.
	int attrib_count = node->GetNodeAttributeCount();
	for (int ai = 0; ai < attrib_count; ++ai) {
		FbxNodeAttribute *attrib = node->GetNodeAttributeByIndex(ai);
		mb.m_name = node->GetName();
		if (attrib->GetAttributeType() == FbxNodeAttribute::eMesh) {
			ImportMesh(mb, mat_stack, (FbxMesh*)attrib, skeleton);
		}
	}

	// Try to load any meshes that are children of this node
	int child_count = node->GetChildCount();
	for (int ci = 0; ci < child_count; ++ci) {
		FbxNode *child = node->GetChild(ci);
		MeshBuilder* newMb = new MeshBuilder();
		ImportMeshes(*newMb, child, mat_stack, skeleton);
		mb.m_children.push_back(newMb);
	}

	// we're done with this node, so its transform off the stack
	mat_stack.Pop();
}

void FBXUtils::ImportMesh(MeshBuilder &out, MatrixStack & mat_stack, FbxMesh * mesh)
{
	// Should have been triangulated before this - sort of a late check 
	// [treat this as an ASSERT_OR_DIE]
	ASSERT_OR_DIE(mesh->IsTriangleMesh(),"Mesh not triangulated!");

	// Geometric Transformations only apply to the current node
	// http://download.autodesk.com/us/fbx/20112/FBX_SDK_HELP/index.html?url=WS1a9193826455f5ff1f92379812724681e696651.htm,topicNumber=d0e7429
	Matrix4 geo_trans = GetGeometricTransform(mesh->GetNode());
	mat_stack.Push(geo_trans);

	// import the mesh data.
	Matrix4 transform = mat_stack.Top();

	// Starting a draw call.
	out.Begin(PRIMITIVE_TRIANGLES, true);

	// Load in the mesh - first, figure out how many polygons there are
	int32_t poly_count = mesh->GetPolygonCount();
	for (int32_t poly_idx = 0; poly_idx < poly_count; ++poly_idx) {

		// For each polygon - get the number of vertices that make it up (should always be 3 for a triangulated mesh)
		int32_t vert_count = mesh->GetPolygonSize(poly_idx);
		ASSERT_OR_DIE(vert_count == 3, "Triangle Meshes should ALWAYS have 3 verts per poly"); // Triangle Meshes should ALWAYS have 3 verts per poly

								 // Finally, import all the data for this vertex (for now, just position)
		for (int32_t vert_idx = 0; vert_idx < vert_count; ++vert_idx) {
			ImportVertex(out, transform, mesh, poly_idx, vert_idx, nullptr);
		}
	}

	out.End();

	// pop the geometric transform
	mat_stack.Pop();
}

bool FBXUtils::ImportMesh(MeshBuilder &out, MatrixStack &mat_stack, FbxMesh *mesh, Skeleton const *skeleton)
{
	// Should have been triangulated before this
	//ASSERT_RETURN_VALUE(mesh->IsTriangleMesh(), false);
	if (!mesh->IsTriangleMesh())
		return false;

	int32_t poly_count = mesh->GetPolygonCount();
	if (poly_count <= 0) {
		return false;
	}

	std::vector<fbx_skin_weight_t>* skin_weights = nullptr;
	if (nullptr != skeleton) {
		skin_weights = new std::vector<fbx_skin_weight_t>();
		CalculateSkinWeights(*skin_weights, mesh, skeleton);
	}

	// Geometric Transformations only apply to the current node
	// http://download.autodesk.com/us/fbx/20112/FBX_SDK_HELP/index.html?url=WS1a9193826455f5ff1f92379812724681e696651.htm,topicNumber=d0e7429
	Matrix4 geo_trans = GetGeometricTransform(mesh->GetNode());
	mat_stack.Push(geo_trans);

	// import the mesh data.
	Matrix4 transform = mat_stack.Top();
	transform = Matrix4::IDENTITY;

	char const *mat_name = mesh->GetElementMaterial()->GetName();// FbxGetMaterialName(mesh);



	out.Begin(mat_name, PRIMITIVE_TRIANGLES, false);



	// Load in the mesh
	for (int32_t poly_idx = 0; poly_idx < poly_count; ++poly_idx) {
		int32_t vert_count = mesh->GetPolygonSize(poly_idx);
		
		ASSERT_OR_DIE(vert_count == 3, "FBXUtils::ImportMesh() : polysize is not 3"); // Triangle Meshes should ALWAYS have 3 verts per poly
		for (int32_t vert_idx = 0; vert_idx < vert_count; ++vert_idx) {
			int32_t controlPointIndex = mesh->GetPolygonVertex(poly_idx, vert_idx);

			if (out.m_useIndex)
			{
				if (out.HasIndex(controlPointIndex))
					out.m_indices.push_back(controlPointIndex);
				else
					ImportVertex(out, transform, mesh, poly_idx, vert_idx, skin_weights);
			}
			else
				ImportVertex(out, transform, mesh, poly_idx, vert_idx, skin_weights);
			
		}
	}

	out.End();

	int blendShapeDeformerCount = mesh->GetDeformerCount(FbxDeformer::eBlendShape);
	if (blendShapeDeformerCount > 0)
	{
		ImportBlendShapes(out, mesh, transform);
	}

	mat_stack.Pop();

	SAFE_DELETE(skin_weights);

	return true;
}

bool FBXUtils::LoadAnimation(const std::string& filePath, Animation& animation, Skeleton* skeleton)
{
	if (!skeleton || skeleton->GetJointCount() <= 0)
		return false;
	// Normal things
	FbxScene *scene = LoadScene(filePath);
	if (nullptr == scene) {
		return false;
	}
	ConvertSceneToEngineBasis(scene);

	// Next get the animation count.
	int anim_count = scene->GetSrcObjectCount<FbxAnimStack>();
	if (anim_count > 0U) {
		// great, we have a motion
		// NOTE:  Multiple motions may be embedded in this file, you could update to extract all 
		FbxAnimStack *anim = scene->GetSrcObject<FbxAnimStack>(0);
		int numAnimLayers = anim->GetMemberCount<FbxAnimLayer>();
		if (numAnimLayers > 0)
		{
			FbxAnimLayer* animLayer = anim->GetMember<FbxAnimLayer>(0);
			
			// Get the start, end, and duration of this animation.
			FbxTime local_start = anim->LocalStart;
			FbxTime local_end = anim->LocalStop;
			float local_start_fl = (float)local_start.GetSecondDouble();
			float local_end_fl = (float)local_end.GetSecondDouble();

			// Okay, so local start will sometimes start BEFORE the first frame - probably to give it something to T
			// pose?  This was adding a jerky frame movement to UnityChan's walk cycle. 
			// Whatever, we'll always start at least 0.
			local_start_fl = MAX(0.0f, local_start_fl);
			float duration_fl = local_end_fl - local_start_fl;

			animation.m_duration = duration_fl;
			animation.m_curves.resize(skeleton->GetJointCount());
			ImportAnimation(animation, skeleton, scene->GetRootNode(), animLayer);
		}
	}

	UnloadScene(scene);

	return true;
}

bool FBXUtils::LoadAnimation(const std::string& filePath, Animation& animation)
{
	// Normal things
	FbxScene *scene = LoadScene(filePath);
	if (nullptr == scene) {
		return false;
	}
	ConvertSceneToEngineBasis(scene);

	// Next get the animation count.
	int anim_count = scene->GetSrcObjectCount<FbxAnimStack>();
	if (anim_count > 0U) {
		// great, we have a motion
		// NOTE:  Multiple motions may be embedded in this file, you could update to extract all 
		FbxAnimStack *anim = scene->GetSrcObject<FbxAnimStack>(0);
		int numAnimLayers = anim->GetMemberCount<FbxAnimLayer>();
		if (numAnimLayers > 0)
		{
			FbxAnimLayer* animLayer = anim->GetMember<FbxAnimLayer>(0);

			// Get the start, end, and duration of this animation.
			FbxTime local_start = anim->LocalStart;
			FbxTime local_end = anim->LocalStop;
			float local_start_fl = (float)local_start.GetSecondDouble();
			float local_end_fl = (float)local_end.GetSecondDouble();

			// Okay, so local start will sometimes start BEFORE the first frame - probably to give it something to T
			// pose?  This was adding a jerky frame movement to UnityChan's walk cycle. 
			// Whatever, we'll always start at least 0.
			local_start_fl = MAX(0.0f, local_start_fl);
			float duration_fl = local_end_fl - local_start_fl;

			animation.m_duration = duration_fl;
			ImportAnimation(animation, scene->GetRootNode(), animLayer);
		}
	}

	UnloadScene(scene);

	return true;
}

bool FBXUtils::LoadBlendShapeAnimation(const std::string& filePath, std::vector<BlendShapeAnimation*>& out_animations, const std::vector<Mesh*>& meshes)
{
	FbxScene *scene = LoadScene(filePath);
	if (nullptr == scene) {
		return false;
	}
	ConvertSceneToEngineBasis(scene);

	// Next get the animation count.
	int anim_count = scene->GetSrcObjectCount<FbxAnimStack>();
	if (anim_count > 0U) {
		// great, we have a motion
		// NOTE:  Multiple motions may be embedded in this file, you could update to extract all 
		FbxAnimStack *anim = scene->GetSrcObject<FbxAnimStack>(0);
		int numAnimLayers = anim->GetMemberCount<FbxAnimLayer>();
		if (numAnimLayers > 0)
		{
			FbxAnimLayer* animLayer = anim->GetMember<FbxAnimLayer>(0);

			// Get the start, end, and duration of this animation.
			FbxTime local_start = anim->LocalStart;
			FbxTime local_end = anim->LocalStop;
			float local_start_fl = (float)local_start.GetSecondDouble();
			float local_end_fl = (float)local_end.GetSecondDouble();

			// Okay, so local start will sometimes start BEFORE the first frame - probably to give it something to T
			// pose?  This was adding a jerky frame movement to UnityChan's walk cycle. 
			// Whatever, we'll always start at least 0.
			local_start_fl = MAX(0.0f, local_start_fl);
			float duration_fl = local_end_fl - local_start_fl;
			for(auto mesh : meshes)
				ImportBlendShapeAnimation(scene->GetRootNode(), animLayer, duration_fl, out_animations, mesh);
			//ImportAnimation(animation, skeleton, scene->GetRootNode(), animLayer);
		}
	}

	UnloadScene(scene);

	return true;
}

bool FBXUtils::ImportBlendShapeAnimation(FbxNode* node, FbxAnimLayer* animLayer, float duration, std::vector<BlendShapeAnimation*>& out_animations, Mesh* mesh)
{
	if (mesh->m_blendShape.m_channels.empty()) // no blendshape for the mesh -> no blendshape anim
		return false;
	//FbxNode* meshNode = node->FindChild(mesh->m_name.c_str(), true);
	std::string nodeName(node->GetName());
	for (auto& blendshapeChannel : mesh->m_blendShape.m_channels)
	{
		std::string blendshapeChannelName = blendshapeChannel.m_name;
		FbxNode* channelNode = node->FindChild(blendshapeChannelName.c_str());
		UNUSED(channelNode);
	}

	
	if (nodeName == mesh->m_name)
	{
		BlendShapeAnimation* animChannel = new BlendShapeAnimation(mesh->m_name, duration);
		ImportBlendShapeAnimChannels(node, animLayer, *animChannel);
		out_animations.push_back(animChannel);
	}
	else
	{
		int childCount = node->GetChildCount();
		for (int childIndex = 0; childIndex < childCount; childIndex++)
			ImportBlendShapeAnimation(node->GetChild(childIndex), animLayer, duration, out_animations, mesh);
	}
	return true;
}

void FBXUtils::FlipX(MeshBuilder& mb)
{
	// LEFT AS AN EXCERCISE (mostly as it depends on how your implementation of MeshBuilder/Mesh)

	// You just want to flip the X coordinate of all vertex
	// data (positions,normals/tangents/bitangents)
	// IMPLEMENT_ME;
	mb.FlipX();
}

bool FBXUtils::LoadSkeleton(Skeleton * skeleton, const std::string & fileName)
{
	// Same as you did for Meshes.
	FbxScene *fbx_scene = LoadScene(fileName);
	if (nullptr == fbx_scene) {
		return false;
	}

	ConvertSceneToEngineBasis(fbx_scene);

	// First, get the bind post for the scene
	skeleton->Clear();


	// Now, traverse the scene, and build the skeleton out with 
	// the bind pose positions
	FbxNode *root = fbx_scene->GetRootNode();

	// Bind pose - the pose which assets are authored for.
	FbxPose *pose = GetBindPose(fbx_scene);

	// Import the skeleton by traversing the scene.
	ImportSkeleton(skeleton, root, nullptr, nullptr, pose);

	// Flip the Axis 
	//skeleton->FlipX();

	UnloadScene(fbx_scene);

	skeleton->CalcJointMap();

	// Success if I've loaded in at least ONE bone/joint.
	return (skeleton->GetJointCount() > 0U);
}

void FBXUtils::CalculateSkinWeights(std::vector<fbx_skin_weight_t> &skin_weights, FbxMesh const *mesh, Skeleton const *skeleton)
{

	if (skeleton == nullptr) {
		skin_weights.clear();
		return;
	}

	// default them all to zero
	unsigned int ctrl_count = (unsigned int)mesh->GetControlPointsCount();
	skin_weights.resize(ctrl_count, fbx_skin_weight_t());
	//skin_weights->set_count(ctrl_count);

	// for each deformer [probably will only be one] - figure out how it affects
	// my vertices
	int deformer_count = mesh->GetDeformerCount(FbxDeformer::eSkin);

	//if no deformer
	if (deformer_count == 0)
	{
		std::string meshNodeName = mesh->GetNode()->GetName();
		FbxNode *parentNode = mesh->GetNode()->GetParent();
		while (parentNode)
		{
			std::string parentNodeName = parentNode->GetName();
			int parnetJointIndex = skeleton->GetJointIndex(parentNodeName);
			if (parnetJointIndex != -1)
			{
				for (auto& skin_weight : skin_weights)
					AddHighestWeight(skin_weight, parnetJointIndex, 1.f);
				break;
			}
			else
				parentNode = parentNode->GetParent();
		}
	}

	for (int didx = 0; didx < deformer_count; ++didx) {
		FbxSkin *skin = (FbxSkin*)mesh->GetDeformer(didx, FbxDeformer::eSkin);
		if (nullptr == skin) {
			continue;
		}

		// cluster is how a single joint affects the mesh, 
		// so go over the vertices it affects and keep track of how much!
		int cluster_count = skin->GetClusterCount();
		for (int cidx = 0; cidx < cluster_count; ++cidx) {
			FbxCluster *cluster = skin->GetCluster(cidx);
			FbxNode const *link_node = cluster->GetLink();

			// Can't use it without a link node [shouldn't happen!]
			if (nullptr == link_node) {
				continue;
			}

			// Find the joint - if this skeleton doesn't have a joint
			// then we skip this cluster!
			int joint_idx = skeleton->GetJointIndex(link_node->GetName());
			if (joint_idx == -1) {
				continue;
			}

			// cluster stores things in an index buffer again,
			// it will store indices to the control points it affects, 
			// as well as how much weight we apply
			int *indices = cluster->GetControlPointIndices();
			int index_count = cluster->GetControlPointIndicesCount();
			double *weights = cluster->GetControlPointWeights();

			for (int i = 0; i < index_count; ++i) {
				int control_idx = indices[i];
				double weight = weights[i];

				fbx_skin_weight_t &skin_weight = skin_weights[control_idx];
				AddHighestWeight(skin_weight, (uint32_t)joint_idx, (float)weight);
			}
		}
	}
}

bool FBXUtils::ImportBlendShapes(MeshBuilder& out, FbxMesh *mesh, const Matrix4& transform)
{
	UNUSED(transform);
	out.m_blendShape.m_name = std::string(mesh->GetName());
	int lBlendShapeDeformerCount = mesh->GetDeformerCount(FbxDeformer::eBlendShape);
	ASSERT_OR_DIE(lBlendShapeDeformerCount == 1, "FBXUtils::ImportBlendShapes() more than 1 deformer!\n");
	for (int lBlendShapeIndex = 0; lBlendShapeIndex < lBlendShapeDeformerCount; ++lBlendShapeIndex)
	{
		FbxBlendShape* lBlendShape = (FbxBlendShape*)mesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);

		int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
		for (int lBlendShapeChannelIndex = 0; lBlendShapeChannelIndex < lBlendShapeChannelCount; ++lBlendShapeChannelIndex)
		{
			FbxBlendShapeChannel* lBlendShapeChannel = lBlendShape->GetBlendShapeChannel(lBlendShapeChannelIndex);
			std::string channelName(lBlendShapeChannel->GetName());
			//float deformPercent = (float)lBlendShapeChannel->DeformPercent.Get();
			int lTargetShapeCount = lBlendShapeChannel->GetTargetShapeCount();
			ASSERT_OR_DIE(lTargetShapeCount == 1, "FBXUtils::GetBlendShapes() : Num of target shapes in each Blend Shape channel is always 1!\n");
			for (int lTargetShapeIndex = 0; lTargetShapeIndex < lTargetShapeCount; ++lTargetShapeIndex)
			{
				FbxShape* lShape = lBlendShapeChannel->GetTargetShape(lTargetShapeIndex);
				std::string shapeName(lShape->GetName());
				int lControlPointsCount = lShape->GetControlPointsCount();
				//FbxVector4* lControlPoints = lShape->GetControlPoints();
				FbxLayerElementArrayTemplate<FbxVector4>* lNormals = NULL;
				bool lStatus = lShape->GetNormals(&lNormals);
				FbxLayerElementArrayTemplate<FbxVector4>* meshNormals = NULL;
				mesh->GetNormals(&meshNormals);
				BlendShapeChannel blendShapeChannel;
				blendShapeChannel.m_name = shapeName;

				int32_t polygonCount = mesh->GetPolygonCount();
				for (int32_t polyIndex = 0; polyIndex < polygonCount; polyIndex++)
				{
					int32_t vertexCount = mesh->GetPolygonSize(polyIndex);
					for (int32_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
					{
						
						//Vertex3 meshVertex = ImportVertex(mesh, transform, polyIndex, vertexIndex);
						Vertex3 &meshVertex = out.m_vertices[polyIndex*vertexCount + vertexIndex];
						int32_t controlPointIndex = mesh->GetPolygonVertex(polyIndex, vertexIndex);
						
						//Vector3 targetPosition = ToVector3(lControlPoints[controlPointIndex]);
						Vector3 targetPosition = ToVector3(
							lShape->GetControlPointAt(controlPointIndex)  
							- mesh->GetControlPointAt(controlPointIndex)
						);
						Vector3 normal = meshVertex.normal;
						if (lStatus && lNormals && lNormals->GetCount() == lControlPointsCount)
						{
							normal = ToVector3(
								lNormals->GetAt(controlPointIndex) 
								- meshNormals->GetAt(controlPointIndex)
							);
						}
						BlendShapeParameter parameter;
						parameter.position = targetPosition;// *transform - meshVertex.position;
						parameter.normal = normal;// *transform - meshVertex.normal;
						//ASSERT_RECOVERABLE(parameter.position != Vector3(), "nn?");
						parameter.position.x *= -1.f;
						parameter.normal.x *= -1.f;

						blendShapeChannel.AddPoint(parameter);
					}
				}
				out.m_blendShape.AddChannel(blendShapeChannel);
			}
		}
	}
	return true;
}

FbxPose* FBXUtils::GetBindPose(FbxScene *scene)
{
	int pose_count = scene->GetPoseCount();
	for (int i = 0; i < pose_count; ++i) {
		FbxPose *pose = scene->GetPose(i);
		if (pose->IsBindPose())
			return pose;
	}
	return nullptr;
}

bool FBXUtils::ImportSkeleton(Skeleton *out, FbxNode *node, FbxSkeleton *rootBone, FbxSkeleton *parentBone, FbxPose *pose)
{

	std::string nodeName = node->GetName();

	for (int i = 0; i < node->GetNodeAttributeCount(); ++i) {

		// If this node is a skeleton node (a joint), 
		// then it should have a skeleton attribute. 
		FbxNodeAttribute *na = node->GetNodeAttributeByIndex(i);
		if (na->GetAttributeType() == FbxNodeAttribute::eSkeleton) {

			// Cast it, and figure out the type of bone. 
			FbxSkeleton *skel = (FbxSkeleton*)na;
			FbxSkeleton::EType type = skel->GetSkeletonType();

			// From FBXSDK Documentation.
			// eRoot,			/*!< First element of a chain. */
			// eLimb,			/*!< Chain element. */
			// eLimbNode,		/*!< Chain element. */
			// eEffector		/*!< Last element of a chain. */

			// If this is a root - we better have not have another root higher 
			// up (for now).  This is just here to catch this happening so
			// I know if I should support it in the future - you could cut this.
			if ((type == FbxSkeleton::eRoot) || (type == FbxSkeleton::eEffector)) {
				// this is a root bone - so can treat it as the root
				// of a skeleton.
				// TODO: no support for nested skeletons yet.
				//ASSERT_OR_DIE(root_bone == nullptr,"FBX::ImportSkeleton() rootBone == nullptr");
				rootBone = skel;
			} // else, it is a limb or limb node, which we treat the same

			  // See if this bone exists int he pose, and if so, 
			  // get its index in the pose.
			int poseNodeIndex = -1;
			if (pose != nullptr) {
				poseNodeIndex = pose->Find(node);
			}

			// Get the global transform for this bone
			Matrix4 boneGlobalTransform = Matrix4::IDENTITY;
			if (false)//(poseNodeIndex != -1) 
			{
				// If it exists in the pose - use the poses transform.
				// Since we're only doing the bind pose now, the pose should ONLY
				// be storing global transforms (as per the documentation).
				//
				// When we update this code to load in animated poses, this will change.
				bool is_local = pose->IsLocalMatrix(poseNodeIndex);
				ASSERT_OR_DIE(false == is_local, "FBX::ImportSkeleton() pose matrix is local"); // for now, should always be global (pose transforms always are)
				//FbxMatrix node_martix(node->GetTransform());
				FbxMatrix fbx_matrix = pose->GetMatrix(poseNodeIndex);
				boneGlobalTransform = ToEngineMatrix(fbx_matrix);
			}
			else {
				// Not in the pose - so we have two options
				if (parentBone == nullptr) {
					// We have no parent [first in the skeleton], so just use 
					// the global transform of the node.
					boneGlobalTransform = ToEngineMatrix(node->EvaluateGlobalTransform());
				}
				else {
					// Otherwise, calculate my world based on my parent transform
					// and my nodes local.
					//
					// The reason behind this is EvaluateGlobalTransform actually
					// gets the animated transform, while getting the Pose transform
					// will get an unanimated transform.  So if we're a child of a transform
					// that was gotten from a pose, I should be evaluating off that.
					//
					// TODO: Evaluate Local from Parent - just in case their is siblings between this
					
					Matrix4 localTransform = ToEngineMatrix(node->EvaluateLocalTransform(), false);
					Matrix4 parentGlobalTransform = out->EvaluateJointTransform(GetBoneName(parentBone));
					boneGlobalTransform = localTransform * parentGlobalTransform;

					//boneGlobalTransform = ToEngineMatrix(node->EvaluateGlobalTransform());
				}
			}

			//Matrix4 geometricMat = ToEngineMatrix(node->GetG());
			Vector3 transformOffset = ToVector3(node->GetGeometricTranslation(FbxNode::eSourcePivot));
			Vector3 rotationOffset = ToVector3(node->GetGeometricRotation(FbxNode::eSourcePivot));
			Vector3 scaleOffset = ToVector3(node->GetGeometricTranslation(FbxNode::eSourcePivot));
			Vector3 rotationPivot = ToVector3(node->GetRotationPivot(FbxNode::eSourcePivot));
			Vector3 scalePivot = ToVector3(node->GetScalingPivot(FbxNode::eSourcePivot));
			Vector3 rotationPost = ToVector3(node->GetPostTargetRotation());
			Vector3 rotationPre = ToVector3(node->GetPreRotation(FbxNode::eSourcePivot));

			// Add a joint.
			out->AddJoint(GetBoneName(skel),
				(parentBone != nullptr) ? GetBoneName(parentBone) : "",
				boneGlobalTransform);

			// set this as the next nodes parent, and continue down the chain
			parentBone = skel;
			break;
		}
	}


	// import the rest
	for (int i = 0; i < node->GetChildCount(); ++i) {
		ImportSkeleton(out, node->GetChild(i), rootBone, parentBone, pose);
	}

	return true;
}

bool FBXUtils::LoadMotion(Motion *motion, Skeleton *skeleton, char const *filename, unsigned int framerate /*= 10*/)
{
	// Normal things
	motion->m_framerate = (float)framerate;
	FbxScene *scene = LoadScene(filename);
	if (nullptr == scene) {
		return false;
	}
	ConvertSceneToEngineBasis(scene);

	// Framerate - how often are we sampling this
	float fr = (float)framerate;


	// Next get the animation count.
	int anim_count = scene->GetSrcObjectCount<FbxAnimStack>();
	if (anim_count > 0U) {
		// great, we have a motion
		// NOTE:  Multiple motions may be embedded in this file, you could update to extract all 
		FbxAnimStack *anim = scene->GetSrcObject<FbxAnimStack>(0);
		ImportMotion(motion, skeleton, scene, anim, fr);
	}

	UnloadScene(scene);

	return (motion->GetDuration() > 0.0f);
}

bool FBXUtils::LoadTimedMotion(Motion *motion, Skeleton *skeleton, char const *filename)
{
	// Normal things
	FbxScene *scene = LoadScene(filename);
	if (nullptr == scene) {
		return false;
	}
	ConvertSceneToEngineBasis(scene);

	// Next get the animation count.
	int anim_count = scene->GetSrcObjectCount<FbxAnimStack>();
	if (anim_count > 0U) {
		// great, we have a motion
		// NOTE:  Multiple motions may be embedded in this file, you could update to extract all 
		FbxAnimStack *anim = scene->GetSrcObject<FbxAnimStack>(0);
		ImportMotionByAnimCurve(motion, skeleton, scene, anim);
	}

	UnloadScene(scene);

	return (motion->GetDuration() > 0.0f);
}

bool FBXUtils::ImportMotion(Motion *motion, Skeleton *skeleton, FbxScene *scene, FbxAnimStack *anim, float framerate)
{
	// First, set the scene to use this animation - helps all the evaluation functions
	scene->SetCurrentAnimationStack(anim);

	// how far do we move through this
	FbxTime advance;
	advance.SetSecondDouble((double)(1.0f / framerate));

	// Get the start, end, and duration of this animation.
	FbxTime local_start = anim->LocalStart;
	FbxTime local_end = anim->LocalStop;
	float local_start_fl = (float)local_start.GetSecondDouble();
	float local_end_fl = (float)local_end.GetSecondDouble();

	// Okay, so local start will sometimes start BEFORE the first frame - probably to give it something to T
	// pose?  This was adding a jerky frame movement to UnityChan's walk cycle. 
	// Whatever, we'll always start at least 0.
	local_start_fl = MAX(0.0f, local_start_fl);
	float duration_fl = local_end_fl - local_start_fl;

	// no duration, no animation!
	if (duration_fl <= 0.0f || framerate <= 0.f) {
		return false;
	}

	// Alright, we have a motion, we start getting data from it
	std::string motion_name = anim->GetName();
	float time_span = duration_fl;
	UNUSED(time_span);

	// set some identifying information
	motion->SetName(motion_name);
	motion->SetDuration(duration_fl);

	// number of frames should encompasses the duration, so say we are at 10hz,
	// but have an animation that is 0.21f seconds.  We need at least...
	//    3 intervals (Ceiling(10 * 0.21) = Ceiling(2.1) = 3)
	//    4 frames (0.0, 0.1, 0.2, 0.3)
	unsigned int frameCount = (unsigned int)Ceiling(duration_fl * framerate) + 1;

	motion->SetFrameCount(frameCount);

	// Now, for each joint in our skeleton
	// not the most efficient way to go about this, but whatever - tool step
	// and it is not slow enough to matter.
	unsigned int jointCount = skeleton->GetJointCount();
	for (unsigned int jointIndex = 0; jointIndex < jointCount; ++jointIndex) {
		//std::string name = skeleton->GetJointName(jointIndex);

		SkeletonJoint* currentJoint = skeleton->GetJoint(jointIndex);
		SkeletonJoint* parentJoint = currentJoint->m_parent;

		std::string name = currentJoint->m_name;

		// get the node associated with this bone
		// I rely on the names to find this - so author of animations
		// should be sure to be using the same rig/rig names.
		// (this is only important for the import process, after that
		// everything is assuming matching indices)
		FbxNode *node = scene->FindNodeByName(name.c_str());
		FbxNode *parent = nullptr;

		// get the parent node
		if (parentJoint) {
			parent = scene->FindNodeByName(parentJoint->m_name.c_str());
		}

		// Now, for this entire animation, evaluate the local transform for this bone at every interval
		// number of frames is 
		FbxTime eval_time = FbxTime(0);
		for (unsigned int frame_idx = 0; frame_idx < frameCount; ++frame_idx, eval_time += advance) {
			// Okay, get the pose we want
			Pose *pose = motion->GetPose(frame_idx);



			/*Matrix4 joint_world = GetNodeWorldTransformAtTime(node, eval_time);
			Matrix4 joint_local = joint_world;
			if (nullptr != parent) 
			{
				Matrix4 parent_world = GetNodeWorldTransformAtTime(parent, eval_time);
				joint_local = joint_world * parent_world.GetInverse();
			}*/

			Matrix4 joint_local;
			if(parent)
				joint_local = GetNodeLocalTransformAtTime(node, eval_time);
			else
				joint_local = GetNodeWorldTransformAtTime(node, eval_time);

			pose->GetTransform(jointIndex) = joint_local;
		}
	}

	return true;
}

bool FBXUtils::ImportMotionByAnimCurve(Motion *motion, Skeleton *skeleton, FbxScene *scene, FbxAnimStack *anim)
{
	// First, set the scene to use this animation - helps all the evaluation functions
	scene->SetCurrentAnimationStack(anim);

	// Get the start, end, and duration of this animation.
	FbxTime local_start = anim->LocalStart;
	FbxTime local_end = anim->LocalStop;
	float local_start_fl = (float)local_start.GetSecondDouble();
	float local_end_fl = (float)local_end.GetSecondDouble();

	// Okay, so local start will sometimes start BEFORE the first frame - probably to give it something to T
	// pose?  This was adding a jerky frame movement to UnityChan's walk cycle. 
	// Whatever, we'll always start at least 0.
	local_start_fl = MAX(0.0f, local_start_fl);
	float duration_fl = local_end_fl - local_start_fl;

	// no duration, no animation!
	if (duration_fl <= 0.0f) {
		return false;
	}

	// Alright, we have a motion, we start getting data from it
	std::string motion_name = anim->GetName();
	//float time_span = duration_fl;

	// set some identifying information
	motion->SetName(motion_name);
	motion->SetDuration(duration_fl);

	int numAnimLayers = anim->GetMemberCount<FbxAnimLayer>();
	FbxAnimLayer* animLayer = nullptr;
	if (numAnimLayers > 0)
	{
		animLayer = anim->GetMember<FbxAnimLayer>(0);
	}

	// Now, for each joint in our skeleton
	// not the most efficient way to go about this, but whatever - tool step
	// and it is not slow enough to matter.
	unsigned int jointCount = skeleton->GetJointCount();

	motion->m_timedPose.resize(jointCount);
	motion->m_isAnimated.resize(jointCount, false);

	for (unsigned int jointIndex = 0; jointIndex < jointCount; ++jointIndex) {
		//std::string name = skeleton->GetJointName(jointIndex);

		SkeletonJoint* currentJoint = skeleton->GetJoint(jointIndex);
		SkeletonJoint* parentJoint = currentJoint->m_parent;

		std::string name = currentJoint->m_name;

		// get the node associated with this bone
		// I rely on the names to find this - so author of animations
		// should be sure to be using the same rig/rig names.
		// (this is only important for the import process, after that
		// everything is assuming matching indices)
		FbxNode *node = scene->FindNodeByName(name.c_str());
		FbxNode *parent = nullptr;

		if (!node) // no animation of the joint
			continue;

		// get the parent node
		if (parentJoint) {
			parent = scene->FindNodeByName(parentJoint->m_name.c_str());
		}

		std::vector<float> keyFrameTimes = GetAnimKeyFrameTimes(node, animLayer);
		if (keyFrameTimes.empty()) // not animated joint
		{
			//motion->SetIsAnimated(jointIndex, false);
			keyFrameTimes.push_back(0.f);
			//keyFrameTimes.push_back(duration_fl);
		}
		else
			motion->SetIsAnimated(jointIndex, true);
		// Now, for this entire animation, evaluate the local transform for this bone at every interval
		// number of frames is 
		FbxTime eval_time = FbxTime(0);

		for (unsigned int timeIndex = 0; timeIndex < keyFrameTimes.size(); ++timeIndex) {
			// Okay, get the pose we want
			std::pair<float, Matrix4>* timedLocalTransform = 
				motion->GetTimedLocalTransformAtJointAndTimeIndex(jointIndex, timeIndex);

			eval_time.SetSecondDouble((double)keyFrameTimes[timeIndex]);

			Matrix4 jointLocalTransform;
			if (parent)
				jointLocalTransform = GetNodeLocalTransformAtTime(node, eval_time);
			else
				jointLocalTransform = GetNodeWorldTransformAtTime(node, eval_time);

			timedLocalTransform->first = keyFrameTimes[timeIndex];
			timedLocalTransform->second = jointLocalTransform;
		}
	}

	return true;
}


Matrix4 FBXUtils::GetGeometricTransform(FbxNode * node)
{
	Matrix4 ret = Matrix4::CreateIdentity();

	if ((node != nullptr) && (node->GetNodeAttribute() != nullptr)) {
		FbxEuler::EOrder order;

		node->GetRotationOrder(FbxNode::eSourcePivot, order);

		FbxVector4 const geo_trans = node->GetGeometricTranslation(FbxNode::eSourcePivot);
		FbxVector4 const geo_rot = node->GetGeometricRotation(FbxNode::eSourcePivot);
		FbxVector4 const geo_scale = node->GetGeometricScaling(FbxNode::eSourcePivot);

		FbxAMatrix geo_mat;
		FbxAMatrix mat_rot;
		mat_rot.SetR(geo_rot, order);
		geo_mat.SetTRS(geo_trans, mat_rot, geo_scale);

		ret = ToEngineMatrix(geo_mat);
	}

	return ret;
}

Matrix4 FBXUtils::ToEngineMatrix(FbxMatrix const &fbx_mat, bool flipX /*= true*/)
{
	

	FbxVector4 i = fbx_mat.GetRow(0);
	FbxVector4 j = fbx_mat.GetRow(1);
	FbxVector4 k = fbx_mat.GetRow(2);
	FbxVector4 t = fbx_mat.GetRow(3);

// 	FbxVector4 i = fbx_mat.GetColumn(0);
// 	FbxVector4 j = fbx_mat.GetColumn(1);
// 	FbxVector4 k = fbx_mat.GetColumn(2);
// 	FbxVector4 t = fbx_mat.GetColumn(3);

	if (flipX)
	{
		Matrix4 flipXmat = Matrix4::CreateScale(Vector3(-1.f, 1.f, 1.f));
		return /*flipXmat * */Matrix4(ToVector4(i), ToVector4(j), ToVector4(k), ToVector4(t)) * flipXmat;
	}
	else
		return Matrix4(ToVector4(i), ToVector4(j), ToVector4(k), ToVector4(t));
}

Matrix4 FBXUtils::GetNodeWorldTransform(FbxNode * node)
{
	FbxAMatrix fbx_mat = node->EvaluateGlobalTransform();
	return ToEngineMatrix(fbx_mat);
}

void FBXUtils::ImportVertex(MeshBuilder &out, Matrix4 const &transform, FbxMesh *mesh, int32_t poly_idx, int32_t vert_idx, std::vector<fbx_skin_weight_t>* skin_weights)
{
	/** NEW BITS **/

	// Will demonstarate with normal, but this process is repeated for
	// uvs, colors, tangents, bitangents (called binormals in the SDK)

	Vertex3 vertex;

	int controlIndex = mesh->GetPolygonVertex(poly_idx, vert_idx);

	Vector3 normal;
	if (GetNormal(normal, transform, mesh, poly_idx, vert_idx))
		vertex.normal = normal;

	Vector2 uv;
	if (GetUV(uv, mesh, poly_idx, vert_idx))
		vertex.texCoords = uv;

	if (skin_weights)
	{
		
		fbx_skin_weight_t &skinWeight = skin_weights->at(controlIndex);
		vertex.boneIndices = skinWeight.indices;
		vertex.boneWeights = skinWeight.weights;
	}

	// If we get a position- push it.
	// This actually should always succeed.
	Vector3 position;
	if (GetPosition(position, transform, mesh, poly_idx, vert_idx))
	{
		vertex.position = position;
		if(out.m_useIndex)
			out.AddIndexedVertex(vertex, controlIndex);
		else
			out.AddVertex(vertex);
	}
	
}

Vertex3 FBXUtils::ImportVertex(FbxMesh *mesh, Matrix4 const &transform, int32_t poly_idx, int32_t vert_idx, std::vector<fbx_skin_weight_t>* skin_weights /*= nullptr*/)
{
	/** NEW BITS **/

	// Will demonstarate with normal, but this process is repeated for
	// uvs, colors, tangents, bitangents (called binormals in the SDK)

	Vertex3 vertex;

	int controlIndex = mesh->GetPolygonVertex(poly_idx, vert_idx);

	Vector3 normal;
	if (GetNormal(normal, transform, mesh, poly_idx, vert_idx))
		vertex.normal = normal;

	Vector2 uv;
	if (GetUV(uv, mesh, poly_idx, vert_idx))
		vertex.texCoords = uv;

	if (skin_weights)
	{

		fbx_skin_weight_t &skinWeight = skin_weights->at(controlIndex);
		vertex.boneIndices = skinWeight.indices;
		vertex.boneWeights = skinWeight.weights;
	}

	// If we get a position- push it.
	// This actually should always succeed.
	Vector3 position;
	if (GetPosition(position, transform, mesh, poly_idx, vert_idx))
	{
		vertex.position = position;
		return vertex;
	}
	return Vertex3();
}

bool FBXUtils::GetNormal(Vector3& out, Matrix4 const & transform, FbxMesh * mesh, int poly_idx, int vert_idx)
{
	// First, we need to get the geometry element we care about...
	// you can this of this as a "layer" of data for a mesh.  In this case
	// is is the normal data.

	// Note:  0 Index - meshes can potentially contain multiple layers
	// of the same type (UVs are the most common exmaple of this).  
	// For Normals, I just use the first one and continue on.
	FbxGeometryElementNormal *element = mesh->GetElementNormal(0);
	/*if (vnormal == nullptr) {
		// no layer - no data
		return false;
	}*/

	if (!element) return false;

	// Next, we need to figure out how to pull the normal for this particular vertex out
	// of this layer. 
	// 
	// So, vnormal is just a packed array of normals, or potentially normals, and
	// and index buffer into those normals.
	//
	// Either way, you can think of vnormal as a packed collection of normals.
	// and we're trying to figure out which element of this array we want (elem_idx)

	// So first, get the element index.
	// (Is the data tied to the control point, or the polygon vertex)
	int elem_idx = 0;
	switch (element->GetMappingMode()) {
	case FbxGeometryElement::eByControlPoint: {
		// Get the normal by control point - most direct way.
		elem_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
	} break; // case: eByControlPoint

	case FbxGeometryElement::eByPolygonVertex: {
		// array is packed by polygon vertex - so each polygon is unique
		// (think sharp edges in the case of normals)
		elem_idx = mesh->GetPolygonVertexIndex(poly_idx) + vert_idx;
	} break; // case: eByPolygonVertex

	default:
		ASSERT_OR_DIE(false,"Fbx::GetNormal() Unknown mapping mode"); // Unknown mapping mode
		return false;
	}

	// next, now that we have the element index, we figure out how this is used.
	// Is this a direct array, or an indexed array.
	// If we got an element index, fetch the data based on how it's referenced
	// (Directly, or by an index into an array);
	switch (element->GetReferenceMode()) {
	case FbxGeometryElement::eDirect: {
		// this is just an array - use elem index as an index into this array
		if (elem_idx < element->GetDirectArray().GetCount()) {
			out = Vector3(ToVector4(element->GetDirectArray().GetAt(elem_idx)));
			out = transform.TransformDirection(out);
			return true;
		}
	} break;

	case FbxGeometryElement::eIndexToDirect: {
		// This is an indexed array, so elem_idx is our offset into the 
		// index buffer.  We use that to get our index into the direct array.
		if (elem_idx < element->GetIndexArray().GetCount()) {
			int index = element->GetIndexArray().GetAt(elem_idx);
			out = Vector3(ToVector4(element->GetDirectArray().GetAt(index)));
			out = transform.TransformDirection(out);
			return true;
		}
	} break;

	default:
		ASSERT_OR_DIE(false,"FBX::GetNormal() Unknown reference type"); // Unknown reference type
		return false;
	}

	return false;
}

bool FBXUtils::GetUV(Vector2& out_uv, FbxMesh * mesh, int32_t poly_idx, int32_t vert_idx)
{
	// First, get the control point index for this poly/vert pair.
	int ctrl_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
	if (ctrl_idx < 0) {
		return false;
	}

	
	int uvCount = mesh->GetTextureUVCount();
	if (uvCount <= 0)
		return false;


	FbxStringList uvNames;
	mesh->GetUVSetNames(uvNames);
	bool unmappedUV;
	FbxVector2 fbx_uv;
	if (mesh->GetPolygonVertexUV(poly_idx, vert_idx, uvNames[0], fbx_uv, unmappedUV))
	{
		out_uv = ToVector2(fbx_uv);
		return true;
	}
	return false;
}

bool FBXUtils::GetPosition(Vector3& out_pos, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx)
{
	// First, get the control point index for this poly/vert pair.
	int ctrl_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
	if (ctrl_idx < 0) {
		return false;
	}

	// Get the position for the control point.
	FbxVector4 fbx_pos = mesh->GetControlPointAt(ctrl_idx);

	// convert to an engine vertex, and save it to the out variable
	Vector3 pos = Vector3(ToVector4(fbx_pos));// .xyz();
	out_pos = pos *transform;

	// return success.
	return true;
}

Vector2 FBXUtils::ToVector2(const FbxVector2 & fbxVector)
{
	return Vector2((float)fbxVector[0], (float)fbxVector[1]);
}

Vector3 FBXUtils::ToVector3(const FbxVector4 & fbxVector)
{
	return Vector3((float)fbxVector[0], (float)fbxVector[1], (float)fbxVector[2]);
}

Vector4 FBXUtils::ToVector4(const FbxVector4& fbxVector)
{
	return Vector4((float)fbxVector[0], (float)fbxVector[1], (float)fbxVector[2], (float)fbxVector[3]);
}

Vector4 FBXUtils::ToVector4(const FbxDouble4 & fbxVector)
{
	return Vector4((float)fbxVector[0], (float)fbxVector[1], (float)fbxVector[2], (float)fbxVector[3]);
}

std::string FBXUtils::GetNodeName(const FbxNode * node)
{
	if (node)
		return node->GetName();
	return std::string();
}

std::string FBXUtils::GetBoneName(const FbxSkeleton *skeleton)
{
	if (!skeleton)
		return "";

	std::string node_name = GetNodeName(skeleton->GetNode());
	if (!node_name.empty())
		return skeleton->GetNode()->GetName();
	else
		return skeleton->GetName();
}

float FBXUtils::GetNativeFramefrate(FbxScene *scene)
{
	// Get the scenes authored framerate
	FbxGlobalSettings &settings = scene->GetGlobalSettings();
	FbxTime::EMode time_mode = settings.GetTimeMode();
	double framerate;
	if (time_mode == FbxTime::eCustom) {
		framerate = settings.GetCustomFrameRate();
	}
	else {
		framerate = FbxTime::GetFrameRate(time_mode);
	}
	return (float)framerate;
}

float FBXUtils::GetNativeFramefrate(FbxScene const *scene)
{
	// Get the scenes authored framerate
	const FbxGlobalSettings &settings = scene->GetGlobalSettings();
	FbxTime::EMode time_mode = settings.GetTimeMode();
	double framerate;
	if (time_mode == FbxTime::eCustom)
		framerate = settings.GetCustomFrameRate();
	else
		framerate = FbxTime::GetFrameRate(time_mode);
	return (float)framerate;
}

Matrix4 FBXUtils::GetNodeWorldTransformAtTime(FbxNode* node, FbxTime evalTime)
{
	if (!node)
		return Matrix4::IDENTITY;
	FbxMatrix mat = node->EvaluateGlobalTransform(evalTime);
	return ToEngineMatrix(mat);
}

Matrix4 FBXUtils::GetNodeLocalTransformAtTime(FbxNode* node, FbxTime evalTime)
{
	if (!node)
		return Matrix4::IDENTITY;
	FbxMatrix mat = node->EvaluateLocalTransform(evalTime);
	return ToEngineMatrix(mat,false);
}

void FBXUtils::AddHighestWeight(fbx_skin_weight_t &skin_weight, uint32_t bone_idx, float weight)
{
	std::pair<unsigned int, float> weightAndIndices[4] =
	{
		std::pair<unsigned int, float>(skin_weight.indices.x,skin_weight.weights.x),
		std::pair<unsigned int, float>(skin_weight.indices.y,skin_weight.weights.y),
		std::pair<unsigned int, float>(skin_weight.indices.z,skin_weight.weights.z),
		std::pair<unsigned int, float>(skin_weight.indices.w,skin_weight.weights.w),
	};
	std::vector<std::pair<unsigned int, float>> list(weightAndIndices, weightAndIndices + 4);
	for (int index = 0; index < 4; index++)
	{
		if (list[index].second < weight)
		{
			list.insert(list.begin() + index, std::pair<unsigned int, float>(bone_idx, weight));
			break;
		}
	}
	skin_weight.indices.x = list[0].first; skin_weight.weights.x = list[0].second;
	skin_weight.indices.y = list[1].first; skin_weight.weights.y = list[1].second;
	skin_weight.indices.z = list[2].first; skin_weight.weights.z = list[2].second;
	skin_weight.indices.w = list[3].first; skin_weight.weights.w = list[3].second;
}

bool FBXUtils::HasSkinWeights(FbxMesh const *mesh)
{
	int deformer_count = mesh->GetDeformerCount(FbxDeformer::eSkin);
	return deformer_count > 0;
}

std::string FBXUtils::GetMaterialName(FbxMesh* const mesh)
{
	return mesh->GetElementMaterial()->GetName();
}

void FBXUtils::ImportAnimation(Animation& animation, Skeleton* skeleton, FbxNode * node, FbxAnimLayer * anim)
{

	std::string nodeName = node->GetName();
	SkeletonJoint* joint = skeleton->GetJoint(nodeName);
	if (joint)
	{
		
		AnimCurve curve = ImportAnimChannels(node, anim);
		curve.m_rotationOrder = ToRotationOrder(node->RotationOrder);
		int jointIndex = skeleton->GetJointIndex(joint);
		animation.m_curves[jointIndex] = curve;
	}
	//node->GetRotationOrder();
	//ERotationOrder lRotationOrder;
	//node->GetRotationOrder(FbxNode::eSourcePivot, lRotationOrder);
	for (int childIndex = 0; childIndex < node->GetChildCount(); childIndex++)
	{
		ImportAnimation(animation, skeleton, node->GetChild(childIndex), anim);
	}
}

void FBXUtils::ImportAnimation(Animation& animation, FbxNode * node, FbxAnimLayer * anim)
{
	std::string nodeName(node->GetName());
	AnimCurve curve = ImportAnimChannels(node, anim);
	curve.m_name = nodeName;
	curve.m_rotationOrder = ToRotationOrder(node->RotationOrder);
	animation.m_curves.push_back(curve);
	//node->GetRotationOrder();
	//ERotationOrder lRotationOrder;
	//node->GetRotationOrder(FbxNode::eSourcePivot, lRotationOrder);
	for (int childIndex = 0; childIndex < node->GetChildCount(); childIndex++)
	{
		ImportAnimation(animation, node->GetChild(childIndex), anim);
	}
}

bool FBXUtils::ImportBlendShapeAnimChannels(FbxNode * node, FbxAnimLayer * anim, BlendShapeAnimation &blendAnimation)
{
	FbxAnimCurve* curve;
	FbxNodeAttribute* lNodeAttribute = node->GetNodeAttribute();
	if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
		lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
		lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
	{
		FbxGeometry* lGeometry = (FbxGeometry*)lNodeAttribute;

		int lBlendShapeDeformerCount = lGeometry->GetDeformerCount(FbxDeformer::eBlendShape);
		for (int lBlendShapeIndex = 0; lBlendShapeIndex < lBlendShapeDeformerCount; ++lBlendShapeIndex)
		{
			FbxBlendShape* lBlendShape = (FbxBlendShape*)lGeometry->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);

			int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
			for (int lChannelIndex = 0; lChannelIndex < lBlendShapeChannelCount; ++lChannelIndex)
			{
				FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
				const char* lChannelName = lChannel->GetName();
				UNUSED(lChannelName);
				curve = lGeometry->GetShapeChannel(lBlendShapeIndex, lChannelIndex, anim, true);
				if (curve)
				{
					BlendShapeAnimationChannel blendChannel;
					int lKeyCount = curve->KeyGetCount();
					for (int lCount = 0; lCount < lKeyCount; lCount++)
					{
						float lKeyValue = static_cast<float>(curve->KeyGetValue(lCount));
						float lKeyTime = static_cast<float>(curve->KeyGetTime(lCount).GetSecondDouble());
						blendChannel.AddKey(lKeyTime, lKeyValue);
					}
					blendAnimation.m_timelines.push_back(blendChannel);
				}

			}
		}
	}
	return true;
}

AnimCurve FBXUtils::ImportAnimChannels(FbxNode * node, FbxAnimLayer * animLayer)
{
	FbxAnimCurve* lAnimCurve = nullptr;
	AnimCurve curve;
	lAnimCurve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve)
	{
		curve.m_keys[AnimCurve::POS_X] = ImportAnimCurveKey(lAnimCurve);
// 		for (auto& key : curve.m_keys[AnimCurve::POS_X])
// 			key.m_value *= -1.f;
	}
	lAnimCurve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (lAnimCurve)
	{
		curve.m_keys[AnimCurve::POS_Y] = ImportAnimCurveKey(lAnimCurve);
	}
	lAnimCurve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (lAnimCurve)
	{
		curve.m_keys[AnimCurve::POS_Z] = ImportAnimCurveKey(lAnimCurve);
	}
	//node->LclRotation.
	lAnimCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve)
	{
		curve.m_keys[AnimCurve::ROTATION_X] = ImportAnimCurveKey(lAnimCurve);
// 		for (auto& key : curve.m_keys[AnimCurve::ROTATION_X])
// 			key.m_value *= -1.f;
	}
	lAnimCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (lAnimCurve)
	{
		curve.m_keys[AnimCurve::ROTATION_Y] = ImportAnimCurveKey(lAnimCurve);
	}
	lAnimCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (lAnimCurve)
	{
		curve.m_keys[AnimCurve::ROTATION_Z] = ImportAnimCurveKey(lAnimCurve);
	}

	lAnimCurve = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve)
	{
		curve.m_keys[AnimCurve::SCALE_X] = ImportAnimCurveKey(lAnimCurve);
	}
	lAnimCurve = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (lAnimCurve)
	{
		curve.m_keys[AnimCurve::SCALE_Y] = ImportAnimCurveKey(lAnimCurve);
	}
	lAnimCurve = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (lAnimCurve)
	{
		curve.m_keys[AnimCurve::SCALE_Z] = ImportAnimCurveKey(lAnimCurve);
	}
	return curve;
}

std::vector<float> FBXUtils::GetAnimKeyFrameTimes(FbxNode * node, FbxAnimLayer * animLayer)
{
	std::vector<float> keyFrames;
	float minInterval = 1 / 60.f;

	FbxAnimCurve* lAnimCurve = nullptr;
	lAnimCurve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve)
	{
		GetKeyFramesInCurve(lAnimCurve, keyFrames);
	}
	lAnimCurve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (lAnimCurve)
	{
		GetKeyFramesInCurve(lAnimCurve, keyFrames);
	}
	lAnimCurve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (lAnimCurve)
	{
		GetKeyFramesInCurve(lAnimCurve, keyFrames);
	}
	lAnimCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve)
	{
		GetKeyFramesInCurve(lAnimCurve, keyFrames);
	}
	lAnimCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (lAnimCurve)
	{
		GetKeyFramesInCurve(lAnimCurve, keyFrames);
	}
	lAnimCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (lAnimCurve)
	{
		GetKeyFramesInCurve(lAnimCurve, keyFrames);
	}

	lAnimCurve = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve)
	{
		GetKeyFramesInCurve(lAnimCurve, keyFrames);
	}
	lAnimCurve = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (lAnimCurve)
	{
		GetKeyFramesInCurve(lAnimCurve, keyFrames);
	}
	lAnimCurve = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (lAnimCurve)
	{
		GetKeyFramesInCurve(lAnimCurve, keyFrames);
	}

	sort(keyFrames.begin(), keyFrames.end());

	//remove frames that are too close to prev frames
	for (size_t i = 0; i < keyFrames.size(); i++)
	{
		size_t j = i + 1;
		if (j < keyFrames.size() && keyFrames[j] - keyFrames[i] < minInterval)
		{
			keyFrames.erase(keyFrames.begin() + j);
			i--;
		}
	}
		
	return keyFrames;
}

std::vector<AnimCurveKey> FBXUtils::ImportAnimCurveKey(FbxAnimCurve* curve)
{
	static const char* interpolation[] = { "?", "constant", "linear", "cubic" };
	static const char* constantMode[] = { "?", "Standard", "Next" };
	static const char* cubicMode[] = { "?", "Auto", "Auto break", "Tcb", "User", "Break", "User break" };
	static const char* tangentWVMode[] = { "?", "None", "Right", "Next left" };

	FbxTime   lKeyTime;
	float   lKeyValue;
	//char    lTimeString[256];
	FbxString lOutputString;
	int     lCount;

	int lKeyCount = curve->KeyGetCount();


	std::vector<AnimCurveKey> keysInCurve;

	for (lCount = 0; lCount < lKeyCount; lCount++)
	{
		lKeyValue = static_cast<float>(curve->KeyGetValue(lCount));
		lKeyTime = curve->KeyGetTime(lCount);
		curve->KeyGetInterpolation(lCount);

		AnimCurveKey key;
		key.m_time = static_cast<float>(lKeyTime.GetSecondDouble());
		key.m_value = lKeyValue;
		keysInCurve.push_back(key);

		/*lOutputString = "            Key Time: ";
		lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
		lOutputString += ".... Key Value: ";
		lOutputString += lKeyValue;
		lOutputString += " [ ";
		lOutputString += interpolation[InterpolationFlagToIndex(curve->KeyGetInterpolation(lCount))];
		if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant)
		{
			lOutputString += " | ";
			lOutputString += constantMode[ConstantmodeFlagToIndex(pCurve->KeyGetConstantMode(lCount))];
		}
		else if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic)
		{
			lOutputString += " | ";
			lOutputString += cubicMode[TangentmodeFlagToIndex(pCurve->KeyGetTangentMode(lCount))];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentweightFlagToIndex(pCurve->KeyGet(lCount).GetTangentWeightMode())];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentVelocityFlagToIndex(pCurve->KeyGet(lCount).GetTangentVelocityMode())];
		}
		lOutputString += " ]";
		lOutputString += "\n";
		FBXSDK_printf(lOutputString);*/
	}
	return keysInCurve;
}

bool FBXUtils::GetKeyFramesInCurve(FbxAnimCurve* curve, std::vector<float>& keyFrames)
{
	FbxTime   lKeyTime;
	int     lCount;
	int lKeyCount = curve->KeyGetCount();

	for (lCount = 0; lCount < lKeyCount; lCount++)
	{
		lKeyTime = curve->KeyGetTime(lCount);
		keyFrames.push_back(static_cast<float>(lKeyTime.GetSecondDouble()));
	}
	return true;
}

AnimCurve::RotationOrder FBXUtils::ToRotationOrder(const FbxEuler::EOrder rotationOrder)
{
	switch (rotationOrder)
	{
	case FbxEuler::eOrderXZY:
		return AnimCurve::ROTATION_ORDER_XZY;
		break;
	case FbxEuler::eOrderYZX:
		return AnimCurve::ROTATION_ORDER_YZX;
		break;
	case FbxEuler::eOrderYXZ:
		return AnimCurve::ROTATION_ORDER_YXZ;
		break;
	case FbxEuler::eOrderZXY:
		return AnimCurve::ROTATION_ORDER_ZXY;
		break;
	case FbxEuler::eOrderZYX:
		return AnimCurve::ROTATION_ORDER_ZYX;
		break;
	case FbxEuler::eOrderSphericXYZ:
		return AnimCurve::ROTATION_ORDER_SPHERIC_XYZ;
		break;
	default:
		return AnimCurve::ROTATION_ORDER_XZY;
	}
}

//------------------------------------------------------------------------
/*
Skeleton* GetSkinWeights(FbxScene *scene, std::vector<FbxWeightDT> &skin_weights, FbxMesh const *mesh)
{
	Skeleton *skeleton = nullptr;

	// default them all to zero
	size_t ctrl_count = skin_weights->get_count();
	for (size_t i = 0; i < ctrl_count; ++i) {
		skin_weights->push_back(FbxWeightDT());
	}

	int deformer_count = mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int didx = 0; didx < deformer_count; ++didx) {
		FbxSkin *skin = (FbxSkin*)mesh->GetDeformer(didx, FbxDeformer::eSkin);
		if (nullptr == skin) {
			continue;
		}

		int cluster_count = skin->GetClusterCount();
		for (int cidx = 0; cidx < cluster_count; ++cidx) {
			FbxCluster *cluster = skin->GetCluster(cidx);
			FbxNode const *link_node = cluster->GetLink();

			// Can't use it without a link node
			if (nullptr == link_node) {
				continue;
			}

			if (skeleton == nullptr) {
				// Find the skeleton for this mesh
				// if one exists
				skeleton = GetSkeletonForBone(import, link_node->GetName());
				if (skeleton == nullptr) {
					continue;
				}
			}

			// Find the joint
			uint32_t joint_idx = skeleton->find_joint_index(link_node->GetName());
			if (joint_idx == INVALID_JOINT_INDEX) {
				continue;
			}

			// SkeletonOld needs to look this up
			int *indices = cluster->GetControlPointIndices();
			int index_count = cluster->GetControlPointIndicesCount();
			double *weights = cluster->GetControlPointWeights();

			for (int i = 0; i < index_count; ++i) {
				int control_idx = indices[i];
				double weight = weights[i];

				fbx_skin_weight_t *skin_weight = skin_weights->get_pointer(control_idx);
				AddHighestWeight(skin_weight, (uint32_t)joint_idx, (float)weight);
			}
		}
	}

	return skeleton;
}*/


#else     


#endif