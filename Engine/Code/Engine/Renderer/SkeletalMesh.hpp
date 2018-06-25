#pragma once
#include <vector>
#include "Mesh.hpp"
#include "Skeleton.hpp"

class SkeletalMesh
{
	std::vector<Mesh*>* m_meshes;
	std::vector<Material*> m_materials;
	Skeleton* m_skeleton;
	SkeletonInstance* m_skeletonInstance;
};