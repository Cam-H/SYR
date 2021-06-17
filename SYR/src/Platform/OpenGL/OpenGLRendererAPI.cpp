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

		glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LESS);
		//glDepthMask(GL_FALSE);//Disable writing to the depth buffer
		glEnable(GL_MULTISAMPLE);

		/*FACE CULLING*/
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
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

	void OpenGLRendererAPI::disableStencilBuffer() {
		glDisable(GL_STENCIL_TEST);
	}

	void OpenGLRendererAPI::enableStencilBuffer() {
		glEnable(GL_STENCIL_TEST);
	}

	void OpenGLRendererAPI::disableDepthBuffer() {
		glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::enableDepthBuffer() {
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::setStencilOperation(STENCIL sfail, STENCIL dpfail, STENCIL dppass) {
		uint16_t stencilFail = 0;
		uint16_t depthFail = 0;
		uint16_t pass = 0;

		switch (sfail) {
		case STENCIL::KEEP:
			stencilFail = GL_KEEP;
			break;
		case STENCIL::ZERO:
			stencilFail = GL_ZERO;
			break;
		case STENCIL::REPLACE:
			stencilFail = GL_REPLACE;
			break;
		case STENCIL::INCR:
			stencilFail = GL_INCR;
			break;
		case STENCIL::INCR_WRAP:
			stencilFail = GL_INCR_WRAP;
			break;
		case STENCIL::DECR:
			stencilFail = GL_DECR;
			break;
		case STENCIL::DECR_WRAP:
			stencilFail = GL_DECR_WRAP;
			break;
		case STENCIL::INVERT:
			stencilFail = GL_INVERT;
			break;
		}

		switch (dpfail) {
		case STENCIL::KEEP:
			depthFail = GL_KEEP;
			break;
		case STENCIL::ZERO:
			depthFail = GL_ZERO;
			break;
		case STENCIL::REPLACE:
			depthFail = GL_REPLACE;
			break;
		case STENCIL::INCR:
			depthFail = GL_INCR;
			break;
		case STENCIL::INCR_WRAP:
			depthFail = GL_INCR_WRAP;
			break;
		case STENCIL::DECR:
			depthFail = GL_DECR;
			break;
		case STENCIL::DECR_WRAP:
			depthFail = GL_DECR_WRAP;
			break;
		case STENCIL::INVERT:
			depthFail = GL_INVERT;
			break;
		}

		switch (dppass) {
		case STENCIL::KEEP:
			pass = GL_KEEP;
			break;
		case STENCIL::ZERO:
			pass = GL_ZERO;
			break;
		case STENCIL::REPLACE:
			pass = GL_REPLACE;
			break;
		case STENCIL::INCR:
			pass = GL_INCR;
			glClear(GL_STENCIL_BUFFER_BIT);
			break;
		case STENCIL::INCR_WRAP:
			pass = GL_INCR_WRAP;
			break;
		case STENCIL::DECR:
			pass = GL_DECR;
			break;
		case STENCIL::DECR_WRAP:
			pass = GL_DECR_WRAP;
			break;
		case STENCIL::INVERT:
			pass = GL_INVERT;
			break;
		}

		glStencilOp(stencilFail, depthFail, pass);

	}
	void OpenGLRendererAPI::setStencilFunction(STENCIL action, uint8_t reference, uint8_t mask) {
		uint16_t function = 0;

		switch (action) {
		case STENCIL::ALWAYS:
			function = GL_ALWAYS;
			break;
		case STENCIL::NEVER:
			function = GL_NEVER;
			break;
		case STENCIL::LESS:
			function = GL_LESS;
			break;
		case STENCIL::LEQUAL:
			function = GL_LEQUAL;
			break;
		case STENCIL::EQUAL:
			function = GL_EQUAL;
			break;
		case STENCIL::GEQUAL:
			function = GL_GEQUAL;
			break;
		case STENCIL::GREATER:
			function = GL_GREATER;
			break;
		case STENCIL::NOTEQUAL:
			function = GL_NOTEQUAL;
			break;
		default:
			SYR_CORE_WARN("Unrecognized stencil function type!");
		}

		glStencilFunc(function, reference, mask);
	}

	void OpenGLRendererAPI::setStencilMask(uint8_t mask) {
		glStencilMask(mask);
	}
}