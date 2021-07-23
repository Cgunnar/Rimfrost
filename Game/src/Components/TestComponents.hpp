#pragma once

#include "Rimfrost\EntCom\rfEntity.hpp"
#include "Rimfrost\Math\RimfrostMath.hpp"
#include <random>

struct TestComponent : Rimfrost::Component<TestComponent>
{
	int a = 2;
	int b = 23;
	double y = 5.2;
};

template typename Rimfrost::Component<TestComponent>;

struct TestComponent2 : Rimfrost::Component<TestComponent2>
{
	float arr[12];
};

template typename Rimfrost::Component<TestComponent2>;

struct TransformComponent : Rimfrost::Component<TransformComponent>
{
	Rimfrost::Transform transform;

	operator Rimfrost::Transform& () { return transform; }
	operator const Rimfrost::Transform& () const { return transform; }
};

template typename Rimfrost::Component<TransformComponent>;

struct SphereCollider : Rimfrost::Component<SphereCollider>
{
	Rimfrost::Vector3 center;
	float radius = 1;
};
template typename Rimfrost::Component<SphereCollider>;

struct PointMass : Rimfrost::Component<PointMass>
{
	Rimfrost::Vector3 force = { 0,0,0 };
	Rimfrost::Vector3 velocity = { 0,0,0 };
	float mass = 1.0f;
};
template typename Rimfrost::Component<PointMass>;