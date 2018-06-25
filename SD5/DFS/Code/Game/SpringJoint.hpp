#pragma once
#include "Engine\Math\Matrix4.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Renderer\Skeleton.hpp"

//Reference http://rocketjump.skr.jp/unity3d/109/
//Steal code from https://github.com/unity3d-jp/unitychan-crs/blob/master/Assets/UnityChan/Scripts/SpringBone.cs

class SpringJoint
{
public:
	//次のボーン
	SkeletonJoint* m_child = nullptr;
	SkeletonJoint* m_owner = nullptr;
	SkeletonInstance* m_instance = nullptr;

	//ボーンの向き
	Vector3 m_boneAxis = Vector3(-1.0f, 0.0f, 0.0f);
	float m_radius = 0.05f;

	//各SpringBoneに設定されているstiffnessForceとdragForceを使用するか？
	bool m_isUseEachBoneForceSettings = false;

	//バネが戻る力
	float m_stiffnessForce = 5.f;

	//力の減衰力
	float m_dragForce = 0.01f;
	Vector3 m_springForce = Vector3(0.0f, -0.0001f, 0.0f);
	bool debug = true;
	//Kobayashi:Thread hold Starting to activate activeRatio
	float m_threshold = 0.01f;
public:
	float m_springLength;
	Matrix4 m_localRotation;
	Matrix4 m_currentRotation;
	Matrix4 m_localTransform;
	Vector3 m_currTipPos;
	Vector3 m_prevTipPos;
	//Kobayashi
	Matrix4 m_org;

	int m_jointIndex;
	int m_childJointIndex;
	int m_parentJointIndex;

public:
	SpringJoint(SkeletonJoint* joint, SkeletonJoint* child, SkeletonInstance* instance);
	void Update();
	void Update(float deltaSeconds);
};