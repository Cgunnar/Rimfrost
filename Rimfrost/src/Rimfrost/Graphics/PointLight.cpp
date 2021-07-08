#include "rfpch.hpp"
#include "PointLight.hpp"
#include "PointLightContainer.hpp"
#include <assert.h>

using namespace DirectX;
namespace Rimfrost
{
	int PointLight::s_nameNumber = 0;

	void PointLight::generateName()
	{
		this->m_name = "pointLight" + std::to_string(s_nameNumber++);
	}

	void PointLight::linkToRepo(std::shared_ptr<PointLightContainer> repo)
	{
		if (m_repo == nullptr) m_repo = repo;
	}

	void PointLight::updateRepo()
	{
		if (m_repo != nullptr)
		{
			m_repo->replacePointLightResource(m_PointLightData, m_internalRepoIndex);
		}
	}

	PointLight::PointLight(const XMFLOAT3& position, const XMFLOAT3& color, float strength, const std::string& name) : PointLight(position, name)
	{
		m_PointLightData.m_color = color;
		m_PointLightData.m_strength = strength;
	}

	PointLight::PointLight(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& color, const std::string& name) : PointLight(position, name)
	{
		m_PointLightData.m_color = color;
	}

	PointLight::PointLight(XMFLOAT3 position, std::string name)
	{
		m_PointLightData.m_color = XMFLOAT3(1, 1, 1);
		m_PointLightData.m_strength = 1.0f;
		m_PointLightData.m_position = XMFLOAT4(position.x, position.y, position.z, 1.0f);
		m_lit = true;
		m_name = name;
		m_repo = nullptr;
		m_internalRepoIndex = -1;

		if (name == "")
		{
			generateName();
		}
	}

	void PointLight::setColor(float r, float g, float b)
	{
		m_PointLightData.m_color = XMFLOAT3(r, g, b);
		updateRepo();
	}

	void PointLight::setColor(const XMFLOAT3& rgb)
	{
		m_PointLightData.m_color = rgb;
		updateRepo();
	}

	void PointLight::setColor(unsigned char r, unsigned char g, unsigned char b)
	{
		m_PointLightData.m_color = XMFLOAT3(r / 255.0f, g / 255.0f, b / 255.0f);
		updateRepo();
	}

	void PointLight::setPosition(const XMFLOAT3& position)
	{
		m_PointLightData.m_position = XMFLOAT4(position.x, position.y, position.z, 1.0f);
		updateRepo();
	}

	void PointLight::setPosition(float x, float y, float z)
	{
		m_PointLightData.m_position = XMFLOAT4(x, y, z, 1.0f);
		updateRepo();
	}

	void PointLight::setStrength(float strength)
	{
		m_PointLightData.m_strength = strength;
		updateRepo();
	}

	void PointLight::setOnOff(bool on)
	{
		m_lit = on;

		if (m_repo != nullptr)
		{
			m_repo->turnOnOff(on, m_internalRepoIndex);
		}
	}

	XMVECTOR PointLight::getPosition() const noexcept
	{
		return XMLoadFloat4(&m_PointLightData.m_position);
	}
}