#pragma once
#include "SceneGraph.hpp"
namespace Rimfrost
{

	class SceneSerializer
	{
	public:
		static void serialize(const std::string& fileName, const SceneGraph& scene);
		static void serialize(const std::string& fileName, const std::shared_ptr<SceneGraph>& scene);
		static void deSerialize(const std::string& fileName, std::shared_ptr<SceneGraph>& scene);

	private:

	};

}