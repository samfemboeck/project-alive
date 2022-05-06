#pragma once

typedef unsigned int GLenum;

class Shader
{
public:
	Shader(const std::string& filepath);
	Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	~Shader();
	const std::string& getName() const { return m_name; };

	void bind() const;
	void unbind() const;
	void uploadUniformInt(const std::string& name, int value) const;
	void uploadUniformIntArray(const std::string& name, int* values, uint32_t count);

	void uploadUniformFloat(const std::string& name, float value) const;
	void uploadUniformFloat2(const std::string& name, const glm::vec2& value) const;
	void uploadUniformFloat3(const std::string& name, const glm::vec3& value) const;
	void uploadUniformFloat4(const std::string& name, const glm::vec4& value) const;

	void uploadUniformMat3(const std::string& name, const glm::mat3& matrix) const;
	void uploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;

private:
	std::string m_name;
	std::string readFile(const std::string& filepath);
	std::unordered_map<GLenum, std::string> preProcess(const std::string& source);
	void compile(const std::unordered_map<GLenum, std::string>& shaderSources);

private:
	uint32_t m_rendererId;
};

class ShaderLibrary
{
public:
	static void add(const std::string& name, const std::string& path);
	static Shader* get(const std::string& name);
private:
	static std::unordered_map<std::string, std::unique_ptr<Shader>> s_shaders;
};
