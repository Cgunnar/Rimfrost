#pragma once

#include <DirectXMath.h>
#include "RimfrostMath.hpp"
#include <string>
namespace Rimfrost
{
	typedef size_t SubModelID;
	typedef size_t ModelID;

	typedef size_t NodeID;
	static constexpr NodeID rootNode = -1;
	typedef size_t BufferID;

	enum class ViewType
	{
		RTV = 0,
		SRV,
		DSV,
		UAV
	};

	class View
	{
	public:
		View(BufferID bufferID, ViewType viewType) : m_ID(bufferID), m_name(), m_type(viewType) {}
		View(const std::string& bufferName, ViewType viewType) : m_ID(-1), m_name(bufferName), m_type(viewType) {}

		inline BufferID ID() const { return m_ID; }
		inline const std::string& name() const { return m_name; }
		inline ViewType type() const { return m_type; }

	private:
		BufferID m_ID;
		ViewType m_type;
		std::string m_name;
		View(BufferID bufferID, const std::string& name, ViewType viewType);
	};

	enum class ShaderEnum
	{
		NONE = 0,
		VERTEXSHADER = 1,
		HULLSHADER = 2,
		DOMAINSHADER = 4,
		GEOMETRYSHADER = 8,
		PIXELSHADER = 16,
		COMPUTESHADER = 32
	};
	inline ShaderEnum operator &(ShaderEnum l, ShaderEnum r)
	{
		return (ShaderEnum)((int)l & (int)r);
	}
	inline ShaderEnum operator |(ShaderEnum l, ShaderEnum r)
	{
		return (ShaderEnum)((int)l | (int)r);
	}


	enum class ModelSettings
	{
		NONE = 0,
		TRANSPARENCY_IS_TWOSIDED = 1 << 0,
		WIREFRAME = 1 << 1,

	};
	inline ModelSettings operator &(ModelSettings l, ModelSettings r)
	{
		return (ModelSettings)((int)l & (int)r);
	}
	inline ModelSettings operator |(ModelSettings l, ModelSettings r)
	{
		return (ModelSettings)((int)l | (int)r);
	}


	struct alignas(16) PerFrameData //gpu matrix fix transpose
	{
		Matrix v;
		Matrix p;
		Vector3 pos;
		float padding;
		int32_t mouseX;
		int32_t mouseY;
	};
}