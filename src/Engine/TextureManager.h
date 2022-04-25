#pragma once

class TextureManager
{
public:
	static void add(std::string filename);
	static Texture2D* get(std::string filename);
};