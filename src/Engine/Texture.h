#pragma once

class Texture2D
{
public:
	Texture2D(const std::string& path);
	~Texture2D();

	uint32_t getWidth() const { return m_width; }
	uint32_t getHeight() const { return m_height; }
	uint32_t getId() const { return m_id; }
	void bind(uint32_t slot = 0) const;
	std::string getFilePath() { return m_path; }
	void loadFromPath(const std::string& path);

	bool operator==(const Texture2D& other) const { return m_id == other.m_id; }
private:
	std::string m_path;
	uint32_t m_width, m_height;
	uint32_t m_id;
};
