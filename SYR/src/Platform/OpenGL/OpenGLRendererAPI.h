#pragma once

#include "SYR/Renderer/RendererAPI.h"

namespace SYR {

	class OpenGLRendererAPI : public RendererAPI {
	public:
		virtual void init() override;

		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;

		virtual void drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void outlineIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;

		virtual void drawLines(const Ref<VertexArray>& vertexArray, float lineWidth, uint32_t indexCount) override;

		virtual void disableDepthBuffer() override;
		virtual void enableDepthBuffer() override;

		//Stencil Buffer Management
		virtual void disableStencilBuffer() override;
		virtual void enableStencilBuffer() override;

		virtual void setStencilOperation(STENCIL sfail, STENCIL dpfail, STENCIL dppass) override;
		virtual void setStencilFunction(STENCIL action, uint8_t reference, uint8_t mask) override;
		virtual void setStencilMask(uint8_t mask) override;
	private:


	};

}