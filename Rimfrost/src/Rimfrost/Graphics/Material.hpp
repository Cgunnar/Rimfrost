#pragma once
#include "RimfrostMath.hpp"

namespace Rimfrost
{

	enum class MaterialProperties
	{
		NONE = 0,
		DIFFUSE_MAP = 1 << 0,
		SPECULAR_MAP = 1 << 1,
		NORMAL_MAP = 1 << 2,

		DIFFUSE_COLOR = 1 << 3,
		SPECULAR_COLOR = 1 << 4,

		ALPHA_BLENDING = 1 << 5,
		ALPHA_TESTING = 1 << 6,

		IS_EMISSIVE = 1 << 7,

		NO_BACKFACE_CULLING = 1 << 8,
		WIREFRAME = 1 << 9,

	};



	class Material
	{
		

	public:
		Material() = default;
		~Material() = default;

		struct alignas(16) materialBuffer
		{
			Vector3 spec;
			float shininess;
			Vector3 diff;
			float opacity;
			Vector3 emis;
		};

		void createConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device);
		void bind(uint32_t slot, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) const;

		const MaterialProperties& getType() const noexcept;
		void setDiffuseColor(float r, float g, float b) noexcept;
		void setSpecular(float shininess, float strength, float r = 0, float g = 0, float b = 0) noexcept;
		void setDiffuseTexure(const std::string& filePath) noexcept;
		void setSpecularTexure(const std::string& filePath) noexcept;
		void setNormalTexure(const std::string& filePath) noexcept;
		void setOpacity(float opacity) noexcept;

		void markForAlphaBlending() noexcept;
		void markForAlphaTesting() noexcept;

		void addProperty(MaterialProperties flag) noexcept;

	private:

		bool m_bindable = false;
		MaterialProperties m_materialType = MaterialProperties::NONE;

		Vector3 m_colorDiffuse = { 0,0,0 };
		Vector3 m_colorSpecular = { 0,0,0 }; // what does this have to do with the material?, it should be a property of the light source
		Vector3 m_colorEmissive = { 0,0,0 }; //fix later
		Vector3 m_colorTransparent = { 0,0,0 }; //what is this?, is this not the same color as the diffuse, but with alpha blendign?

		float m_opacity = 1; // why do i need this?
		float m_shininess = 0;
		float m_shininessStrength = 1;

		std::string m_diffuseMapPath = "";
		std::string m_specularMapPath = "";
		std::string m_normalMapPath = "";

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_materialBuffer;

	};

	inline MaterialProperties operator &(MaterialProperties l, MaterialProperties r)
	{
		return (MaterialProperties)((int)l & (int)r);
	}
	inline MaterialProperties operator |(MaterialProperties l, MaterialProperties r)
	{
		return (MaterialProperties)((int)l | (int)r);
	}
}
