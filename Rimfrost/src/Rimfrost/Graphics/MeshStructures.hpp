#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include "Material.hpp"

enum class TextureTypes
{
	NONE = 0,
	DIFFUSE = 1,
	SPECULAR = 2,
	NORMAL = 4,


};

inline TextureTypes operator &(TextureTypes l, TextureTypes r)
{
	return (TextureTypes)((int)l & (int)r);
}
inline TextureTypes operator |(TextureTypes l, TextureTypes r)
{
	return (TextureTypes)((int)l | (int)r);
}



struct EngineMeshSubset
{
	std::string name;
	TextureTypes texTypes;
	Engine1::Material material;

	unsigned int vertexCount;
	unsigned int vertexStart;

	unsigned int indexStart;
	unsigned int indexCount;

	std::string filePath;
	std::string diffuseFileName;
	float color[3];		// used if diffuseFilePath == ""

	std::string specularFileName;
	std::string normalFileName;

	//std::string PSFilePath;
	//std::string VSFilePath;
};

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 normal;
};

struct VertexTBN
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 biTangent;

};
struct SubMeshTree
{
	std::vector<SubMeshTree> nodes;
	std::vector<EngineMeshSubset> subMeshes;
	std::vector<size_t> subMeshesIndex;
};

class AssimpLoader;
struct EngineMeshData
{
	friend AssimpLoader;

private:
	std::vector<unsigned int> indices;			// 32 bit!
	std::vector<Vertex> vertices;
	std::vector<VertexTBN> verticesTBN;
	bool hasNormalMap = false;

public:
	std::vector<EngineMeshSubset> subMeshesVector;
	SubMeshTree subsetsInfo;

	bool hasNormalMaps() const
	{
		return this->hasNormalMap;
	}

	float* getVertextBuffer() const
	{
		if (hasNormalMap)
			return (float*)this->verticesTBN.data();
		else
			return (float*)this->vertices.data();
	}
	size_t getVertexSize() const
	{
		if (hasNormalMap)
			return sizeof(VertexTBN);
		else
			return sizeof(Vertex);
	}
	size_t getVertexCount() const
	{
		if (hasNormalMap)
			return verticesTBN.size();
		else
			return vertices.size();
	}
	size_t getIndicesCount() const
	{
		return indices.size();
	}
	const unsigned int* getIndicesData() const
	{
		return indices.data();
	}
};