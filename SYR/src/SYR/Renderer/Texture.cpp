#include "syrpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace SYR{

	Ref<Texture2D> Texture2D::create(uint32_t width, uint32_t height) {
		switch (Renderer::getAPI()) {
		case RendererAPI::API::NONE:     SYR_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
		case RendererAPI::API::OPENGL:   return createRef<OpenGLTexture2D>(width, height);
		}

		SYR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::create(const std::string& path) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::NONE:     SYR_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
			case RendererAPI::API::OPENGL:   return createRef<OpenGLTexture2D>(path);
		}

		SYR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::create(void* buffer, uint32_t width, uint32_t height) {

		switch (Renderer::getAPI()) {
			case RendererAPI::API::NONE:     SYR_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
			case RendererAPI::API::OPENGL:   return createRef<OpenGLTexture2D>((void*)buffer, width, height);
		}

		SYR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}