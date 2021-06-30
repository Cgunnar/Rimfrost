#pragma once

#include <string>
#include <vector>
namespace Engine1
{
	class PointLightContainer;

	class PointLight
	{
		friend PointLightContainer;
	private:

		struct PointLightResource
		{
			DirectX::XMFLOAT4 m_position;
			DirectX::XMFLOAT3 m_color;
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

		PointLight(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& color, float strength, const std::string& name = "");
		PointLight(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& color, const std::string& name = "");
		PointLight(DirectX::XMFLOAT3 position, std::string name = "");
		~PointLight() = default;



		void setColor(float r, float g, float b);
		void setColor(const DirectX::XMFLOAT3& rgb);
		void setColor(unsigned char r, unsigned char g, unsigned char b);

		void setPosition(const DirectX::XMFLOAT3& position);
		void setPosition(float x, float y, float z);

		void setStrength(float strength);

		void setOnOff(bool on);

		DirectX::XMVECTOR getPosition() const noexcept;

		//getters??

	};
}
