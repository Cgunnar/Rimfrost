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

struct PointLightComponent : Rimfrost::Component<PointLightComponent>
{
	PointLightComponent() : m_pointLightKey(genRandomID()) {}
	Rimfrost::Vector3 position;
	Rimfrost::Vector3 color;
	float strength;
	bool lit;
	int64_t getID() const { return m_pointLightKey; }
private:
	int64_t m_pointLightKey;
	static int64_t genRandomID()
	{
		std::random_device rdev;
		std::mt19937 gen(rdev());
		std::uniform_int_distribution<int64_t> udis(1, INT64_MAX);
		return udis(gen);
	}
};