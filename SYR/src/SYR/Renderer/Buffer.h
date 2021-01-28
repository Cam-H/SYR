#pragma once

namespace SYR {

	enum class ShaderDataType {
		NONE = 0, FLOAT, FLOAT2, FLOAT3, FLOAT4, MAT3, MAT4, INT, INT2, INT3, INT4, BOOL
	};

	static uint32_t shaderDataTypeSize(ShaderDataType type) {
		switch (type) {
			case ShaderDataType::FLOAT:     return 4;
			case ShaderDataType::FLOAT2:    return 8;
			case ShaderDataType::FLOAT3:    return 12;
			case ShaderDataType::FLOAT4:    return 16;
			case ShaderDataType::MAT3:      return 36;
			case ShaderDataType::MAT4:      return 64;
			case ShaderDataType::INT:       return 4;
			case ShaderDataType::INT2:      return 8;
			case ShaderDataType::INT3:      return 12;
			case ShaderDataType::INT4:      return 16;
			case ShaderDataType::BOOL:      return 1;
		}

		SYR_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement {
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		BufferElement() {}
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false) : Name(name), Type(type), Size(shaderDataTypeSize(type)), Offset(0), Normalized(normalized) {}

		uint32_t getComponentCount() const {
			switch (Type) {
				case ShaderDataType::FLOAT:     return 1;
				case ShaderDataType::FLOAT2:    return 2;
				case ShaderDataType::FLOAT3:    return 3;
				case ShaderDataType::FLOAT4:    return 4;
				case ShaderDataType::MAT3:      return 9;
				case ShaderDataType::MAT4:      return 16;
				case ShaderDataType::INT:       return 1;
				case ShaderDataType::INT2:      return 2;
				case ShaderDataType::INT3:      return 3;
				case ShaderDataType::INT4:      return 4;
				case ShaderDataType::BOOL:      return 1;
			}

			SYR_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout {
	public:

		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferElement>& element) : m_Elements(element) {
			calculateOffsetsAndStride();
		}

		inline uint32_t getStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& getElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void calculateOffsetsAndStride() {
			uint32_t offset = 0;
			m_Stride = 0;

			for (auto& element : m_Elements) {
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer {
	public:
		virtual ~VertexBuffer(){}

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual const BufferLayout& getLayout() const = 0;
		virtual void setLayout(const BufferLayout& layout) = 0;

		virtual void setData(const void* data, uint32_t size) = 0;

		static Ref<VertexBuffer> create(uint32_t size);
		static Ref<VertexBuffer> create(float* vertices, uint32_t size);
	};

	//Only supports 32-bit index buffers currently
	class IndexBuffer {
	public:
		virtual ~IndexBuffer() {}

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual uint32_t getCount() const = 0;

		virtual void setData(const void* data, uint32_t size) = 0;

		static Ref<IndexBuffer> create(uint32_t count);
		static Ref<IndexBuffer> create(uint32_t* indices, uint32_t count);
	};
}