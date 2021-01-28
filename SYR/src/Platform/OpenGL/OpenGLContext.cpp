#include "syrpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>

namespace SYR {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle) {
		SYR_CORE_ASSERT(windowHandle, "Window Handle is null!");
	}

	void OpenGLContext::init() {
		glfwMakeContextCurrent(m_WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		SYR_CORE_ASSERT(status, "Failed to initialize Glad!");

		SYR_CORE_INFO("OpenGL Info:");
		SYR_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		SYR_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		SYR_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

	}

	void OpenGLContext::swapBuffers() {
		glfwSwapBuffers(m_WindowHandle);
	}

}