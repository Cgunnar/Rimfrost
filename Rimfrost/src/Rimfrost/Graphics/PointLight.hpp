#pragma once

#include <string>
#include <vector>
#include "RimfrostMath.hpp"
namespace Rimfrost
{
	class PointLightContainer;

	class PointLight
	{
		friend PointLightContainer;
	private:

		struct PointLightResource
		{
			Vector4 m_position;
			Vector3 m_color;
			float m_strength;
		} m_PointLightData;

		bool m_lit;
		std::shared_ptr<PointLightContainer> m_repo;
		int m_internalRepoIndex;
		std::string m_name;

		static int s_nameNumber;

	private:
		void generateName();
		void linkToRepo(std::shared_ptr<PointLightContainer> repo);
		void updateRepo();
	public:

		PointLight(const Vector3& position, const Vector3& color, float strength, const std::string& name = "");
		PointLight(const Vector3& position, const Vector3& color, const std::string& name = "");
		PointLight(Vector3 position, const std::string& name = "");
		PointLight() = default;
		~PointLight() = default;



		void setColor(float r, float g, float b);
		void setColor(const Vector3& rgb);
		void setColor(unsigned char r, unsigned char g, unsigned char b);

		void setPosition(const Vector3& position);
		void setPosition(float x, float y, float z);

		void setStrength(float strength);

		void setOnOff(bool on);

		Vector4 getPosition() const noexcept;

		//getters??

	};
}
