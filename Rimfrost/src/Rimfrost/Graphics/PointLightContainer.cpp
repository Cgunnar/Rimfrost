#include "rfpch.hpp"
#include "PointLightContainer.hpp"
namespace Rimfrost
{
	PointLightContainer::PointLightContainer()//cosnt ref
	{
		m_latestUsedLightIndex = 0;
		m_rebuildBuffer = true;
		m_updateBuffer = false;
		ZeroMemory(&m_subresData, sizeof(D3D11_SUBRESOURCE_DATA));
	}

	PointLightContainer::~PointLightContainer()
	{

	}

	void PointLightContainer::addPointLight(PointLight& pointLight)
	{
		if (findPointLight(pointLight.m_name)) return; //can't have same


		m_pointLightVector.push_back(std::pair(pointLight, -1)); // hope pair() stores a copy of pointlight
		m_latestUsedLightIndex = m_pointLightVector.size() - 1;
		m_rebuildBuffer = true;
		pointLight.linkToRepo(shared_from_this()); // use non const refrence to link repo
		pointLight.m_internalRepoIndex = static_cast<int>(m_pointLightVector.size()) - 1;
	}

	const std::vector<std::pair<PointLight, int>>& PointLightContainer::getPointLights() const
	{
		return m_pointLightVector;
	}

	bool PointLightContainer::findPointLight(const std::string& name)
	{
		for (int i = 0; i < m_pointLightVector.size(); ++i)
		{
			if (name.compare(m_pointLightVector[i].first.m_name) == 0)
			{
				m_latestUsedLightIndex = i;
				return true;
			}
		}
		return false;

	}

	const PointLight& PointLightContainer::getPointLight(const std::string& name)
	{
		if (name.compare(m_pointLightVector[m_latestUsedLightIndex].first.m_name)) return m_pointLightVector[m_latestUsedLightIndex].first; // fast check, if someone called findPointLight beforehand

		for (int i = 0; i < m_pointLightVector.size(); ++i)
		{
			if (name.compare(m_pointLightVector[i].first.m_name))
			{
				m_latestUsedLightIndex = i;
				return m_pointLightVector[i].first;
			}
		}
		assert(false);
		return m_pointLightVector[0].first; // will never return
	}

	std::shared_ptr<StructuredBuffer> PointLightContainer::getStructuredLightBuffer()
	{
		if (m_rebuildBuffer)
		{
			bool check = createBuffer();
			assert(check);
		}
		else if (m_updateBuffer)
		{
			updateBuffer();
		}

		return m_structuredBuffer;
	}


	bool PointLightContainer::createBuffer()
	{
		m_pointLightResourceVector.clear();
		m_pointLightResourceVector.reserve(m_pointLightVector.size());

		for (int i = 0; i < m_pointLightVector.size(); ++i)
		{
			m_pointLightVector[i].second = -1; // reset index to resourceVector
			if (m_pointLightVector[i].first.m_lit)
			{
				m_pointLightResourceVector.push_back(m_pointLightVector[i].first.m_PointLightData);
				m_pointLightVector[i].second = static_cast<int>(m_pointLightResourceVector.size()) - 1; // save index so we can map into resourceVector later
			}
		}

		//create structerd buffer
		m_subresData.pSysMem = m_pointLightResourceVector.data();
		uint32_t count = static_cast<uint32_t>(m_pointLightResourceVector.size());
		if (count == 0) return false;
		m_structuredBuffer = std::make_shared<StructuredBuffer>(static_cast<uint32_t>(sizeof(PointLight::PointLightResource)), count);
		this->updateBuffer();

		m_rebuildBuffer = false;
		m_updateBuffer = false;

		return true;
	}

	void PointLightContainer::updateBuffer()
	{
		m_structuredBuffer->updateBuffer(m_pointLightResourceVector.data());
		m_updateBuffer = false;
	}

	void PointLightContainer::replacePointLightResource(const PointLight::PointLightResource& data, int index)
	{
		m_pointLightVector[index].first.m_PointLightData = data; // update PointLight
		if (m_pointLightVector[index].second != -1) // update resource vector if it has owns a copy of the resource
		{
			m_pointLightResourceVector[m_pointLightVector[index].second] = data;
			m_updateBuffer = true;
		}
	}

	void PointLightContainer::turnOnOff(bool on, int index)
	{
		if (m_pointLightVector[index].first.m_lit == on) return; // no change, early exit

		m_pointLightVector[index].first.m_lit = on;
		m_rebuildBuffer = true;
	}
}