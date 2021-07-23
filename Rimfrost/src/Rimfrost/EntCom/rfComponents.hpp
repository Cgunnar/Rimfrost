#pragma once

#include "Rimfrost\EntCom\rfEntity.hpp"
#include "Rimfrost\Math\RimfrostMath.hpp"
#include "Rimfrost\Scene\Node.hpp"
#include <random>

namespace Rimfrost
{
	struct NodeComponent : Rimfrost::Component<NodeComponent>
	{
		NodeHandle nodeHandel;
		void destroy()
		{
			Logger::getLogger().debugLog("HEJEHJEHEJ\n");
			Logger::getLogger().debugLog(this->componentName);
		}
	};
	template typename Rimfrost::Component<NodeComponent>;

	struct PointLightComponent : Rimfrost::Component<PointLightComponent>
	{
		PointLightComponent() : m_pointLightKey(genRandomID()) {}
		Rimfrost::Vector3 position;
		Rimfrost::Vector3 color;
		float strength;
		bool lit;
		int64_t getKey() const { return m_pointLightKey; }
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
	template typename Rimfrost::Component<PointLightComponent>;
}