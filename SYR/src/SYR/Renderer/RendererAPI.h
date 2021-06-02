#pragma once

#include <glm/glm.hpp>
#include "VertexArray.h"

namespace SYR {

	class RendererAPI {
	public:
		enum class API {
			NONE = 0, OPENGL
		};
		
		enum class STENCIL {
			KEEP, ZERO, REPLACE, INCR, INCR_WRAP, DECR, DECR_WRAP, INVERT,
			ALWAYS, NEVER, LESS, LEQUAL, EQUAL, GEQUAL, GREATER, NOTEQUAL
		};

	public:
		virtual void init() = 0;

		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void clear() = 0;

		virtual void drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void outlineIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		virtual void drawLines(const Ref<VertexArray>& vertexArray, float lineWidth, uint32_t indexCount = 0) = 0;

		virtual void disableDepthBuffer() = 0;
		virtual void enableDepthBuffer() = 0;

		//Stencil Buffer Management
		virtual void disableStencilBuffer() = 0;
		virtual void enableStencilBuffer() = 0;

		virtual void setStencilOperation(STENCIL sfail, STENCIL dpfail, STENCIL dppass) = 0;
		virtual void setStencilFunction(STENCIL action, uint8_t reference, uint8_t mask) = 0;
		virtual void setStencilMask(uint8_t mask) = 0;

		inline static API getAPI() { return s_API; }
	private:
		static API s_API;
	};

}
