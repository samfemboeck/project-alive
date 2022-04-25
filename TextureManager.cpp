#include "bcpch.h"
#include "TextureManager.h"

static std::unordered_map<std::string, Texture2D*> s_textures;

void TextureManager::add(std::string filename)
{
	Texture2D* texture = new Texture2D(filename);
	s_textures.insert({ filename, texture });
}

Texture2D* TextureManager::get(std::string filename)
{
	auto it = s_textures.find(filename);

	if (it != s_textures.end())
		return it->second;
	else
		return nullptr;
}
