#include "syrpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

#include "SYR/Renderer/VertexArray.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		SYR_CORE_ERROR("[OpenGLError] ({0}) {1} {2}", error, function, line);
		return 0;
	}

	return 1;
}

namespace SYR {

	void OpenGLRendererAPI::init() {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glEnable(GL_STENCIL_TEST);
		//glStencilMask(0x00);//Disables writing to the stencil buffer
		//glStencilFunc(GL_EQUAL, 1, 0xFF);//Specifies what fragments pass the test
		//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LESS);
		//glDepthMask(GL_FALSE);//Disable writing to the depth buffer
		glEnable(GL_MULTISAMPLE);
		//glDisable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::setClearColor(const glm::vec4& color) {
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRendererAPI::drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) {
		uint32_t count = indexCount != 0 ? indexCount : vertexArray->getIndexBuffer()->getCount();

		vertexArray->bind();

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::outlineIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) {
		uint32_t count = indexCount != 0 ? indexCount : vertexArray->getIndexBuffer()->getCount();

		vertexArray->bind();

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::drawLines(const Ref<VertexArray>& vertexArray, float lineWidth, uint32_t indexCount) {
		uint32_t count = indexCount != 0 ? indexCount : vertexArray->getIndexBuffer()->getCount();

		vertexArray->bind();
		vertexArray->getIndexBuffer()->bind();

		glLineWidth(lineWidth);
		glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, nullptr);
	}
}