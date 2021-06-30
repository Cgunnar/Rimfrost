#include "rfpch.hpp"
#include "Material.hpp"

namespace Engine1
{
	void Material::createConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device)
	{
		materialBuffer res;
		res.diff = m_colorDiffuse;
		res.spec = m_colorSpecular;
		res.emis = m_colorEmissive;
		res.shininess = m_shininess;
		res.opacity = m_opacity;

		D3D11_BUFFER_DESC cBufferDesc = {};
		cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cBufferDesc.CPUAccessFlags = 0;
		cBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		cBufferDesc.StructureByteStride = 0;
		cBufferDesc.MiscFlags = 0;
		cBufferDesc.ByteWidth = sizeof(materialBuffer);

		D3D11_SUBRESOURCE_DATA subres;
		subres.pSysMem = &res;
		HRESULT hr = device->CreateBuffer(&cBufferDesc, &subres, &m_materialBuffer);
		assert(SUCCEEDED(hr));

		m_bindable = true;
	}
	void Material::bind(uint32_t slot, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) const
	{
		assert(m_bindable);
		context->PSSetConstantBuffers(slot, 1, m_materialBuffer.GetAddressOf());
	}
	const MaterialProperties& Material::getType() const noexcept
	{
		return m_materialType;
	}
	void Material::setDiffuseColor(float r, float g, float b) noexcept
	{
		m_colorDiffuse.x = pow(r, 2.2f);
		m_colorDiffuse.y = pow(g, 2.2f);
		m_colorDiffuse.z = pow(b, 2.2f);
		m_materialType = m_materialType | MaterialProperties::DIFFUSE_COLOR;
	}
	void Material::setSpecular(float shininess, float strength, float r, float g, float b) noexcept
	{
		m_shininess = shininess;
		m_shininessStrength = strength;

		m_colorSpecular.x = r;
		m_colorSpecular.y = g;
		m_colorSpecular.z = b;
		m_materialType = m_materialType | MaterialProperties::SPECULAR_COLOR;
	}
	void Material::setDiffuseTexure(const std::string& filePath) noexcept
	{
		m_diffuseMapPath = filePath;
		m_materialType = m_materialType | MaterialProperties::DIFFUSE_MAP;
	}
	void Material::setSpecularTexure(const std::string& filePath) noexcept
	{
		m_specularMapPath = filePath;
		m_materialType = m_materialType | MaterialProperties::SPECULAR_MAP;
	}
	void Material::setNormalTexure(const std::string& filePath) noexcept
	{
		m_normalMapPath = filePath;
		m_materialType = m_materialType | MaterialProperties::NORMAL_MAP;
	}
	void Material::setOpacity(float opacity) noexcept
	{
		m_opacity = opacity;
		if (opacity < 1) m_materialType = m_materialType | MaterialProperties::ALPHA_BLENDING;
	}
	void Material::markForAlphaBlending() noexcept
	{
		m_materialType = m_materialType | MaterialProperties::ALPHA_BLENDING;
	}
	void Material::markForAlphaTesting() noexcept
	{
		m_materialType = m_materialType | MaterialProperties::ALPHA_TESTING;
	}
	void Material::addProperty(MaterialProperties flag) noexcept
	{
		m_materialType = m_materialType | flag;
	}
}