#pragma once
#include <string>

struct MyImageStruct
{
	MyImageStruct() = default;
	/*MyImageStruct(const MyImageStruct& other);
	MyImageStruct& operator=(const MyImageStruct& other);*/
	~MyImageStruct();
	
	int width = 0;
	int height = 0;
	int bpp = 0;
	bool alphaBlending = false;
	bool alphaTesting = false;
	void* imagePtr = nullptr;
};

void readImage(MyImageStruct& pImageStruct, const std::string& fileName);