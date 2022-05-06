#pragma once

enum class ShaderDataType
{
	None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

static uint32_t ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:    return 4;
	case ShaderDataType::Float2:   return 4 * 2;
	case ShaderDataType::Float3:   return 4 * 3;
	case ShaderDataType::Float4:   return 4 * 4;
	case ShaderDataType::Mat3:     return 4 * 3 * 3;
	case ShaderDataType::Mat4:     return 4 * 4 * 4;
	case ShaderDataType::Int:      return 4;
	case ShaderDataType::Int2:     return 4 * 2;
	case ShaderDataType::Int3:     return 4 * 3;
	case ShaderDataType::Int4:     return 4 * 4;
	case ShaderDataType::Bool:     return 1;
	}

	return 0;
}

struct BufferElement
{
	std::string Name;
	ShaderDataType Type;
	uint32_t Size;
	uint32_t Offset;
	bool Normalized;

	BufferElement() {}

	BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
		: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
	{
	}

	uint32_t GetComponentCount() const
	{
		switch (Type)
		{
		case ShaderDataType::Float:   return 1;
		case ShaderDataType::Float2:  return 2;
		case ShaderDataType::Float3:  return 3;
		case ShaderDataType::Float4:  return 4;
		case ShaderDataType::Mat3:    return 3 * 3;
		case ShaderDataType::Mat4:    return 4 * 4;
		case ShaderDataType::Int:     return 1;
		case ShaderDataType::Int2:    return 2;
		case ShaderDataType::Int3:    return 3;
		case ShaderDataType::Int4:    return 4;
		case ShaderDataType::Bool:    return 1;
		}

		return 0;
	}
};

class BufferLayout
{
public:
	BufferLayout() {}

	BufferLayout(const std::initializer_list<BufferElement>& elements)
		: m_elements(elements)
	{
		CalculateOffsetsAndStride();
	}

	inline uint32_t GetStride() const { return m_stride; }
	inline const std::vector<BufferElement>& GetElements() const { return m_elements; }

	std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
	std::vector<BufferElement>::iterator end() { return m_elements.end(); }
	std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }
private:
	void CalculateOffsetsAndStride()
	{
		uint32_t offset = 0;
		m_stride = 0;
		for (auto& element : m_elements)
		{
			element.Offset = offset;
			offset += element.Size;
			m_stride += element.Size;
		}
	}
private:
	std::vector<BufferElement> m_elements;
	uint32_t m_stride = 0;
};

class VertexBuffer
{
public:
	VertexBuffer(uint32_t size);
	VertexBuffer(const void* vertices, uint32_t size);
	virtual ~VertexBuffer();

	virtual void bind() const;
	virtual void unbind() const;

	virtual void setData(const void* data, uint32_t size);

	virtual const BufferLayout& getLayout() const { return m_layout; }
	virtual void setLayout(const BufferLayout& layout) { m_layout = layout; }
private:
	uint32_t m_rendererId;
	BufferLayout m_layout;
};

class IndexBuffer
{
public:
	IndexBuffer(uint32_t* indices, uint32_t count);
	virtual ~IndexBuffer();

	virtual void bind() const;
	virtual void unbind() const;

	virtual uint32_t getCount() const { return m_count; }
private:
	uint32_t m_rendererId;
	uint32_t m_count;
};
