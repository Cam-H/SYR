#include "syrpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace SYR {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type) {
			case SYR::ShaderDataType::FLOAT:    return GL_FLOAT;
			case SYR::ShaderDataType::FLOAT2:   return GL_FLOAT;
			case SYR::ShaderDataType::FLOAT3:   return GL_FLOAT;
			case SYR::ShaderDataType::FLOAT4:   return GL_FLOAT;
			case SYR::ShaderDataType::MAT3:     return GL_FLOAT;
			case SYR::ShaderDataType::MAT4:     return GL_FLOAT;
			case SYR::ShaderDataType::INT:      return GL_INT;
			case SYR::ShaderDataType::INT2:     return GL_INT;
			case SYR::ShaderDataType::INT3:     return GL_INT;
			case SYR::ShaderDataType::INT4:     return GL_INT;
			case SYR::ShaderDataType::BOOL:     return GL_BOOL;
		}

		SYR_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() {
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::bind() const {
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::unbind() const {
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
		SYR_CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex buffer has not layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->getLayout();
		for (const auto& element : layout) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, element.getComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE, layout.getStride(), (const void*)element.Offset);

			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
		glBindVertexArray(m_RendererID);
		indexBuffer->bind();

		m_IndexBuffer = indexBuffer;
	}
}