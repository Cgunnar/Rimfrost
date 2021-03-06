#include "rfpch.hpp"
#pragma warning(push, 0)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#pragma warning(pop)
#include "ReadImg.hpp"


MyImageStruct::~MyImageStruct()
{
	stbi_image_free(this->imagePtr);
	this->imagePtr = nullptr;
}

void readImage(MyImageStruct& pImageStruct, const std::string& fileName)
{
	unsigned char* img = stbi_load(fileName.c_str(), &pImageStruct.width, &pImageStruct.height, &pImageStruct.bpp, STBI_rgb_alpha);
	if (pImageStruct.bpp == 4)
	{
		int size = pImageStruct.height * pImageStruct.width * 4;
		bool testing = false;
		bool blending = false;
		for (int i = 0; i < size; i += 4)
		{
			if (img[i + 3] == 0)
			{
				pImageStruct.alphaTesting = true;
			}
			else if (0 < img[i + 3] && img[i + 3] < 255)
			{
				pImageStruct.alphaBlending = true;
				break;
			}
		}
		if (pImageStruct.alphaBlending)
		{
			pImageStruct.alphaTesting = false;
		}

	}
	pImageStruct.imagePtr = (void*)img;
}


