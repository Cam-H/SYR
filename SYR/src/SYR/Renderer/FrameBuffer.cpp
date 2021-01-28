#include "syrpch.h"
#include "FrameBuffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace SYR {

	Ref<FrameBuffer> FrameBuffer::create(const FrameBufferSpecification& spec) {

		switch (Renderer::getAPI()) {
			case RendererAPI::API::NONE: SYR_CORE_ASSERT(false, "RendererAPI::NONE is not supported!"); return nullptr;
			case RendererAPI::API::OPENGL: return createRef<OpenGLFrameBuffer>(spec);
		}

		SYR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}