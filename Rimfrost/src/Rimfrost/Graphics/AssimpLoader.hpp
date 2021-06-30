#pragma once

#pragma warning(push, 0)
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>		// Post processing flags
#pragma warning(pop)
#include "MeshStructures.hpp"
#include <vector>	
#include <string>




class AssimpLoader
{
private:
	std::vector<Vertex> m_vertices;
	std::vector<VertexTBN> m_verticesTBN;
	std::vector<unsigned int> m_indices;
	std::vector<EngineMeshSubset> m_subsets;
	// Keep track of submesh offset in m_vertices and m_indices respectively
	unsigned int m_meshVertexCount;
	unsigned int m_meshIndexCount;

	bool m_hasNormalMap;

	SubMeshTree processNode(aiNode* node, const aiScene* scene, const std::string& path);
	EngineMeshSubset processMesh(aiMesh* mesh, const aiScene* scene, const std::string& path);

public:
	AssimpLoader();
	~AssimpLoader();

	EngineMeshData loadStaticModel(std::string filePath);





};