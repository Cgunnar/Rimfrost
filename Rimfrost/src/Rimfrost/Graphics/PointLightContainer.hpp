#pragma once
#include "PointLight.hpp"
#include "StructuredBuffer.hpp"
#include <vector>
#include <memory>
#include <utility>
#include <string>

namespace Rimfrost
{
	class PointLightContainer : public std::enable_shared_from_this<PointLightContainer>
	{
		friend PointLight;
	private:


		bool m_rebuildBuffer;
		bool m_updateBuffer;
		size_t m_latestUsedLightIndex;
		D3D11_SUBRESOURCE_DATA m_subresData;

		std::shared_ptr<StructuredBuffer> m_structuredBuffer;


		std::vector<std::pair<PointLight, int>> m_pointLightVector;
		std::vector<PointLight::PointLightResource> m_pointLightResourceVector;

	public:
		PointLightContainer();
		~PointLightContainer();


		void addPointLight(PointLight& pointLight);

		const std::vector<std::pair<PointLight, int>>& getPointLights() const;


		bool findPointLight(const std::string& name);
		const PointLight& getPointLight(const std::string& name);

		std::shared_ptr<StructuredBuffer> getStructuredLightBuffer();


	private:
		[[nodiscard]] bool createBuffer();
		void updateBuffer();
		void replacePointLightResource(const PointLight::PointLightResource& data, int index);
		void turnOnOff(bool on, int index);
	};
}
