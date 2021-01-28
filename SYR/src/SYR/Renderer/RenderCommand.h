#pragma once

#include "RendererAPI.h"

namespace SYR {

	class RenderCommand {
	public:
		inline static void init() { s_RendererAPI->init(); }

		inline static void setClearColor(const glm::vec4& color) { s_RendererAPI->setClearColor(color); }
		inline static void clear() { s_RendererAPI->clear(); };

		inline static void drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) {
			s_RendererAPI->drawIndexed(vertexArray, indexCount);
		}

		inline static void drawLines(const Ref<VertexArray>& vertexArray, float lineWidth, uint32_t indexCount = 0) {
			s_RendererAPI->drawLines(vertexArray, lineWidth, indexCount);
		}

		inline static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
			s_RendererAPI->setViewport(x, y, width, height);

			viewportDimensions = glm::ivec2(width, height);
		}

		inline static glm::ivec2 getViewportDimensions() { return viewportDimensions; }

	private:
		static RendererAPI* s_RendererAPI;

		inline static glm::ivec2 viewportDimensions;
	};

}