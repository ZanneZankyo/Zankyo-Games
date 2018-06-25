#pragma once

//#define TOOLS_BUILD

#if defined(TOOLS_BUILD)

#include <fbxsdk.h> 
#include <string>
#include "Engine\Math\Matrix4.hpp"
#include "Engine\Math\MatrixStack.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include <fbxsdk\core\arch\fbxtypes.h>
#include "Engine\Renderer\Skeleton.hpp"
#include "Engine\Math\UIntVector4.hpp"
#include "Engine\Math\Vector4.hpp"
#include "Engine\Renderer\Animation.hpp"
#include "Engine\Renderer\BlendShape.hpp"

struct fbx_skin_weight_t
{
	fbx_skin_weight_t() :
		indices(UIntVector4::ZERO),
		weights(Vector4::ZERO)
	{}

	void reset()
	{
		indices = UIntVector4::ZERO;
		weights = Vector4::ZERO;
	}

	UIntVector4 indices;
	Vector4 weights;
};

struct fbx_blend_shape_t
{
	std::vector<Vector3> position;
	std::vector<Vector3> normal;
	std::vector<Vector3> tangent;
};

class FBXUtils
{
public:
	static void Setup();
	static void Destroy();
	static FbxScene* LoadScene(const std::string& filePath);
	static void UnloadScene(FbxScene* scene);
	static void ListScene(FbxScene* scene);
	static void PrintNode(FbxNode* node, int depth);
	static void PrintAttribute(FbxNodeAttribute* attribute, int depth);
	static std::string GetAttributeTypeName(FbxNodeAttribute::EType type);
	static bool LoadMesh(const std::string& filePath, MeshBuilder& meshBuilder);
	static bool LoadMesh(const std::string& filePath, MeshBuilder& meshBuilder, Skeleton* skeleton);
	static void ConvertSceneToEngineBasis(FbxScene* scene);
	static void TriangulateScene(FbxScene* scene);
	static void ImportMeshes(MeshBuilder &out, FbxNode *node, MatrixStack &mat_stack, Skeleton* skeleton = nullptr);
	static void ImportMesh(MeshBuilder &out, MatrixStack &mat_stack, FbxMesh *mesh);
	static bool ImportMesh(MeshBuilder &out,
		MatrixStack &mat_stack,
		FbxMesh *mesh,
		Skeleton const *skeleton);
	static bool LoadAnimation(const std::string& filePath, Animation& animation);
	static bool LoadAnimation(const std::string& filePath, Animation& animation, Skeleton* skeleton);
	static bool LoadBlendShapeAnimation(const std::string& filePath, std::vector<BlendShapeAnimation*>& out_animations, const std::vector<Mesh*>& meshes);
	static bool ImportBlendShapeAnimation(FbxNode* node, FbxAnimLayer* animLayer, float duration, std::vector<BlendShapeAnimation*>& out_animations, Mesh* mesh);
	static void FlipX(MeshBuilder &mb);
	static bool LoadSkeleton(Skeleton *skeleton, const std::string& fileName);
	static void CalculateSkinWeights(std::vector<fbx_skin_weight_t> &skin_weights,
		FbxMesh const *mesh,
		Skeleton const *skeleton);
	static bool ImportBlendShapes(MeshBuilder& out, FbxMesh *mesh, const Matrix4& transform);
	static FbxPose* GetBindPose(FbxScene *scene);
	static bool ImportSkeleton(Skeleton *out, FbxNode *node, FbxSkeleton *root_bone, FbxSkeleton *parent_bone, FbxPose *pose);
	static bool LoadMotion(Motion *motion, Skeleton *skeleton, char const *filename, unsigned int framerate = 10);
	static bool LoadTimedMotion(Motion *motion, Skeleton *skeleton, char const *filename);
	static bool ImportMotion(Motion *motion, Skeleton *skeleton, FbxScene *scene, FbxAnimStack *anim, float framerate);
	static bool ImportMotionByAnimCurve(Motion *motion, Skeleton *skeleton, FbxScene *scene, FbxAnimStack *anim);
	static Matrix4 GetGeometricTransform(FbxNode *node);
	static Matrix4 ToEngineMatrix(FbxMatrix const &fbx_mat, bool flipX = true);
	static Matrix4 GetNodeWorldTransform(FbxNode *node);
	static void ImportVertex(MeshBuilder &out, Matrix4 const &transform, FbxMesh *mesh, int32_t poly_idx, int32_t vert_idx, std::vector<fbx_skin_weight_t>* skin_weights = nullptr);
	static Vertex3 ImportVertex(FbxMesh *mesh, Matrix4 const &transform, int32_t poly_idx, int32_t vert_idx, std::vector<fbx_skin_weight_t>* skin_weights = nullptr);
	static bool GetNormal(Vector3 &out, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx);
	static bool GetPosition(Vector3 &out_pos, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx);
	static bool GetUV(Vector2& out_uv, FbxMesh * mesh, int32_t poly_idx, int32_t vert_idx);
	static Vector2 ToVector2(const FbxVector2& fbxVector);
	static Vector3 ToVector3(const FbxVector4& fbxVector);
	static Vector4 ToVector4(const FbxVector4& fbxVector);
	static Vector4 ToVector4(const FbxDouble4& fbxVector);
	static std::string GetNodeName(const FbxNode* node);
	static std::string GetBoneName(const FbxSkeleton *skeleton);
	static float GetNativeFramefrate(FbxScene *scene);
	static Matrix4 GetNodeWorldTransformAtTime(FbxNode* node, FbxTime evalTime);
	static Matrix4 GetNodeLocalTransformAtTime(FbxNode* node, FbxTime evalTime);
	static void AddHighestWeight(fbx_skin_weight_t &skin_weight, uint32_t bone_idx, float weight);
	static bool HasSkinWeights(FbxMesh const *mesh);
	static std::string GetMaterialName(FbxMesh* const mesh);
	static float GetNativeFramefrate(FbxScene const *scene);
	static void ImportAnimation(Animation& animation, FbxNode * node, FbxAnimLayer * anim);
	static void ImportAnimation(Animation& animation, Skeleton* skeleton, FbxNode * node, FbxAnimLayer * anim);
	static bool ImportBlendShapeAnimChannels(FbxNode * node, FbxAnimLayer * anim, BlendShapeAnimation &blendAnimation);
	static AnimCurve ImportAnimChannels(FbxNode * node, FbxAnimLayer * anim);
	static std::vector<AnimCurveKey> ImportAnimCurveKey(FbxAnimCurve* curve);
	static std::vector<float> GetAnimKeyFrameTimes(FbxNode * node, FbxAnimLayer * anim);
	static bool GetKeyFramesInCurve(FbxAnimCurve* curve, std::vector<float>& keyFrames);
	static AnimCurve::RotationOrder ToRotationOrder(const FbxEuler::EOrder RotationOrder);
public:
	static FbxManager* s_manager;
	static FbxIOSettings *s_ios;
	static FbxImporter* s_importer;
};

#else

#endif