#include "syrpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace SYR {
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}