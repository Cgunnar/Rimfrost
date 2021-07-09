#pragma once

#include "Rimfrost\EntCom\BaseComponent.hpp"
#include "Rimfrost\Math\RimfrostMath.hpp"


struct TestComponent : Rimfrost::Component<TestComponent>
{
	int a = 2;
	int b = 23;
	double y = 5.2;
};

struct TransformComponent : Rimfrost::Component<TransformComponent>
{
	Rimfrost::Transform transform;

	operator Rimfrost::Transform& () { return transform; }
	operator const Rimfrost::Transform& () const { return transform; }
};

struct PointMass : Rimfrost::Component<PointMass>
{
	Rimfrost::Vector3 force = { 0,0,0 };
	Rimfrost::Vector3 velocity = { 0,0,0 };
	float mass = 1.0f;
};

struct SphereCollider : Rimfrost::Component<SphereCollider>
{
	Rimfrost::Vector3 center;
	float radius = 1;
};