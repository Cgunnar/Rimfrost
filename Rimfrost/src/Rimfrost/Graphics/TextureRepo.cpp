#include "pch.hpp"
#include "TextureRepo.hpp"
#include <ReadImg.hpp>

using namespace std;
namespace Engine1
{
	unordered_map<TextureID, Texture> TextureRepo::s_textures;

	TextureRepo::TextureRepo()
	{

	}

	TextureRepo::~TextureRepo()
	{

	}

	optional<reference_wrapper<Texture>> TextureRepo::getTexture(const TextureID& id)
	{
		auto iter = s_textures.find(id);
		if (iter != s_textures.end())
			return reference_wrapper(iter->second);
		else
			return nullopt;
	}

	TextureID TextureRepo::addTexture(const TextureID& filePath, bool srgb)
	{
		if (s_textures.contains(filePath)) return filePath; //texture exists

		MyImageStruct imgStruct;
		readImage(imgStruct, filePath);
		s_textures.insert_or_assign(filePath, Texture(srgb, imgStruct.imagePtr, imgStruct.width, imgStruct.height, imgStruct.alphaBlending, imgStruct.alphaTesting));
		return filePath;
	}
}