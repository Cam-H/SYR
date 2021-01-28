#include "syrpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace SYR {

	Ref<VertexArray> VertexArray::create() {
		switch (Renderer::getAPI()) {
		case RendererAPI::API::NONE:     SYR_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
		case RendererAPI::API::OPENGL:   return std::make_shared<OpenGLVertexArray>();
		}

		SYR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}