#include "syrpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace SYR {

	Ref<VertexBuffer> VertexBuffer::create(uint32_t size) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::NONE:     SYR_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
			case RendererAPI::API::OPENGL:   return std::make_shared<OpenGLVertexBuffer>(size);
		}

		SYR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::create(float* vertices, uint32_t size) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::NONE:     SYR_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
			case RendererAPI::API::OPENGL:   return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		}

		SYR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::create(uint32_t size) {
		switch (Renderer::getAPI()) {
		case RendererAPI::API::NONE:     SYR_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
		case RendererAPI::API::OPENGL:   return std::make_shared<OpenGLIndexBuffer>(size);
		}

		SYR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t size) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::NONE:     SYR_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
			case RendererAPI::API::OPENGL:   return std::make_shared<OpenGLIndexBuffer>(indices, size);
		}

		SYR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}