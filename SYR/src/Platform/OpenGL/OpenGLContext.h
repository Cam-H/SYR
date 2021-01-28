#pragma once

#include "SYR/Renderer/GraphicsContext.h"
#include <GLFW/glfw3.h>

struct GLFWwindow;

namespace SYR {
	
	class OpenGLContext : public GraphicsContext {
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void init() override;
		virtual void swapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;

	};


}
