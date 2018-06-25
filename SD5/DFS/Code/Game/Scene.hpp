#pragma once
#include "Engine\Math\PhysicsDynamicActor.hpp"
#include "Engine\Math\PhysicsScene.hpp"
#include "Engine\RHI\VertexBuffer.hpp"
#include "Engine\Math\PhysicsStaticActor.hpp"
#include <vector>

class Scene
{
public:
	void Start();
	void Update();
	void Render() const;
	void BuildUnityChanJoints();

	PhysicsDynamicActor* m_sphere = nullptr;
	PhysicsStaticActor* m_plane = nullptr;

	std::vector<PhysicsDynamicActor*> m_physicsJoints;
	std::vector<PhysicsSpringJoint*> m_springJoints;
	PhysicsSpringJoint* m_lHairSpringJoint = nullptr;
	PhysicsSpringJoint* m_rHairSpringJoint = nullptr;

	PhysicsScene* m_pxScene = nullptr;

	VertexBuffer* m_sphereVbo;
};
