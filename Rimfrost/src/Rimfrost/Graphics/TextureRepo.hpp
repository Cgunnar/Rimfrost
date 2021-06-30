#pragma once
#include "Texture.hpp"
#include <optional>
#include <unordered_map>

namespace Engine1
{
	typedef std::string TextureID;

	class TextureRepo
	{
	public:
		TextureRepo();
		~TextureRepo();

		static std::optional<std::reference_wrapper<Texture>> getTexture(const TextureID& id);
		static TextureID addTexture(const TextureID& filePath, bool srgb);

	private:
		static std::unordered_map<TextureID, Texture> s_textures;

	};
}
