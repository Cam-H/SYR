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

		virtual void drawLines(const Ref<VertexArray>& vertexArray, float lineWidth, uint32_t indexCount) override;
	};

}