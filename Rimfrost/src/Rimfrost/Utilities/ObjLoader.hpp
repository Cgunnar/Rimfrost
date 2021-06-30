#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

class ModelClass
{

};
class objLoader
{
private:
	
	static void mtlLoad(std::string fileName, ModelClass* model)
	{	
		
		std::string path = "../ObjFiler/" + fileName;
		std::ifstream mtlFile;
		mtlFile.open(path);
		std::string row1;
		std::string line;

		while (std::getline(mtlFile, line))
		{
			if (line[0] == '#' || (line.substr(0, 2) != "Ns" && line.substr(0, 2) != "Ka" && line.substr(0, 2) != "Ks" &&
				line.substr(0, 2) != "Kd" && line.substr(0, 2) != "Ns" && line.substr(0, 6) != "map_Kd"))
				continue;
			if (line.substr(0, 6) == "map_Kd")
			{
				//läs bildens filnamn
				//std::string imagePath = "../ObjFiler/" + line.substr(7);
				//model->texturePath = imagePath;
				//model->textureLoadedFromMTL = true;
			}
			else
			{
				//tack o lov behandlar vi bara Ks och Ns
				if (line.substr(0, 2) == "Ks")
				{
					float ks[3];
					int i = 0;
					line = line.substr(3);
					std::stringstream strStream(line);
					std::string subStr;// en substring här är en komposant av vektorn Kd
					while (std::getline(strStream, subStr, ' ')) //ska bara köras tre gånger
					{
						ks[i++] = std::stof(subStr);
					}
					//model->material.specularColor.x = ks[0];
					//model->material.specularColor.y = ks[1];
					//model->material.specularColor.z = ks[2];
				}
				else if(line.substr(0, 2) == "Ns")
				{
					//model->material.specularColor.w = std::stof(line.substr(3));
				}
			}
		}
		

		mtlFile.close();
	}
public: 
	static unsigned int objLoaderFunction(std::string fileName, float** ptrToFloatPtr, ModelClass* model = nullptr)
	{
		struct FaceData
		{
			struct VertexIndex
			{
				size_t index[3];
			};
			VertexIndex Vertex[3];
		};

		std::ifstream objFile;
		objFile.open(fileName);

		std::string line;
		std::vector<std::string> strVec;
		std::vector<std::string> faceLineVec;
		std::vector<FaceData> faceDataVec;

		std::string mtlLine = "";

		while (std::getline(objFile, line))
		{
			if (line[0] == '#' || (line.substr(0, 2) != "vn" && line.substr(0, 2) != "vt" &&
				line[0] != 'v' && line[0] != 'f' && line.substr(0,6) != "mtllib"))
				continue;

			std::stringstream strStream(line);
			std::string subStr;
			if (line[0] == 'f')
			{
				faceLineVec.push_back(line);

			}
			else if (line.substr(0, 6) == "mtllib")
			{
				mtlLine = line;
				if (model)//börja läs mtl filen
				{
					mtlLine = line.substr(7); //7 är längden på "mtllib " vi vill läsa resten av raden
					mtlLoad(mtlLine, model);
				}
			}
			else
			{
				while (std::getline(strStream, subStr, ' '))
				{
					strVec.push_back(subStr);
				}
			}
		}
		objFile.close();



		std::vector<float> vertexVector;
		std::vector<float> normalVector;
		std::vector<float> uvVector;


		std::string lineCase;
		for (auto s : strVec)
		{
			if (s == "v")
			{
				lineCase = "v";
				continue;
			}
			else if (s == "vn")
			{
				lineCase = "vn";
				continue;
			}
			else if (s == "vt")
			{
				lineCase = "vt";
				continue;
			}

			if (lineCase == "v")
			{
				vertexVector.push_back(std::stof(s));
			}
			if (lineCase == "vn")
			{
				normalVector.push_back(std::stof(s));
			}
			if (lineCase == "vt")
			{
				uvVector.push_back(std::stof(s));
			}

		}

		//faceLine  f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
		for (int i = 0; i < faceLineVec.size(); i++)
		{
			std::string subStr; // v/vt/vn
			std::stringstream strStream(faceLineVec[i]);

			FaceData newFace;

			int vCount = 0;
			std::vector<std::string> faceSubStrVec;
			while (std::getline(strStream, subStr, ' '))
			{
				if (subStr[0] == 'f')
					continue;
				faceSubStrVec.push_back(subStr);

				int iCount = 0;

				std::string indexStr; // v 
				std::stringstream subStrStream(subStr);

				while (std::getline(subStrStream, indexStr, '/'))
				{
					newFace.Vertex[vCount].index[iCount++] = std::stoi(indexStr);
				}
				vCount++;
			}
			faceDataVec.push_back(newFace);
		}

		struct Vertex
		{
			float position[3];
			float normal[3];
			float uv[2];
		};


		std::vector<std::vector<float>>nestedVertexVec;
		for (size_t i = 0; i < vertexVector.size(); i += 3)
		{
			std::vector<float> posVec;
			posVec.push_back(-1*vertexVector[i]);
			posVec.push_back(vertexVector[i + 1]);
			posVec.push_back(vertexVector[i + 2]);
			nestedVertexVec.push_back(posVec);
		}

		std::vector<std::vector<float>>nestedNormalVec;
		for (size_t i = 0; i < normalVector.size(); i += 3)
		{
			std::vector<float>normVec;
			normVec.push_back(-1*normalVector[i]);
			normVec.push_back(normalVector[i + 1]);
			normVec.push_back(normalVector[i + 2]);
			nestedNormalVec.push_back(normVec);
		}

		std::vector<std::vector<float>>nestedUvVec;
		for (size_t i = 0; i < uvVector.size(); i += 2)
		{
			std::vector<float>uvVec;
			uvVec.push_back(uvVector[i]);
			uvVec.push_back(1 - uvVector[i + 1]); //fixar så att v = 1-v
			nestedUvVec.push_back(uvVec);
		}


		size_t vertexCount = faceDataVec.size() * 3;

		Vertex* vertexArray = new Vertex[vertexCount];
		int offset = 0;
		for (const auto& v : faceDataVec)
		{																 //1 här för att flipa winding order
			(vertexArray + offset)->position[0] = nestedVertexVec[v.Vertex[1].index[0] - 1][0];
			(vertexArray + offset)->position[1] = nestedVertexVec[v.Vertex[1].index[0] - 1][1];
			(vertexArray + offset)->position[2] = nestedVertexVec[v.Vertex[1].index[0] - 1][2];

			(vertexArray + offset)->normal[0] = nestedNormalVec[v.Vertex[1].index[2] - 1][0];
			(vertexArray + offset)->normal[1] = nestedNormalVec[v.Vertex[1].index[2] - 1][1];
			(vertexArray + offset)->normal[2] = nestedNormalVec[v.Vertex[1].index[2] - 1][2];

			(vertexArray + offset)->uv[0] = nestedUvVec[v.Vertex[1].index[1] - 1][0];
			(vertexArray + offset)->uv[1] = nestedUvVec[v.Vertex[1].index[1] - 1][1];

			offset++;
																		 //0 här för att flippa winding order
			(vertexArray + offset)->position[0] = nestedVertexVec[v.Vertex[0].index[0] - 1][0];
			(vertexArray + offset)->position[1] = nestedVertexVec[v.Vertex[0].index[0] - 1][1];
			(vertexArray + offset)->position[2] = nestedVertexVec[v.Vertex[0].index[0] - 1][2];

			(vertexArray + offset)->normal[0] = nestedNormalVec[v.Vertex[0].index[2] - 1][0];
			(vertexArray + offset)->normal[1] = nestedNormalVec[v.Vertex[0].index[2] - 1][1];
			(vertexArray + offset)->normal[2] = nestedNormalVec[v.Vertex[0].index[2] - 1][2];

			(vertexArray + offset)->uv[0] = nestedUvVec[v.Vertex[0].index[1] - 1][0];
			(vertexArray + offset)->uv[1] = nestedUvVec[v.Vertex[0].index[1] - 1][1];


			offset++;
			(vertexArray + offset)->position[0] = nestedVertexVec[v.Vertex[2].index[0] - 1][0];
			(vertexArray + offset)->position[1] = nestedVertexVec[v.Vertex[2].index[0] - 1][1];
			(vertexArray + offset)->position[2] = nestedVertexVec[v.Vertex[2].index[0] - 1][2];

			(vertexArray + offset)->normal[0] = nestedNormalVec[v.Vertex[2].index[2] - 1][0];
			(vertexArray + offset)->normal[1] = nestedNormalVec[v.Vertex[2].index[2] - 1][1];
			(vertexArray + offset)->normal[2] = nestedNormalVec[v.Vertex[2].index[2] - 1][2];

			(vertexArray + offset)->uv[0] = nestedUvVec[v.Vertex[2].index[1] - 1][0];
			(vertexArray + offset)->uv[1] = nestedUvVec[v.Vertex[2].index[1] - 1][1];
			offset++;
		}


		*ptrToFloatPtr = (float*)vertexArray;

		return (unsigned int)(vertexCount * sizeof(Vertex));
	}
};
