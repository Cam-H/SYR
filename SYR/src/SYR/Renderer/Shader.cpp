#include "syrpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace SYR {

	Ref<Shader> Shader::create(const std::string& filepath) {

		switch (Renderer::getAPI()) {
			case RendererAPI::API::NONE:     SYR_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
			case RendererAPI::API::OPENGL:   return std::make_shared<OpenGLShader>(filepath);
		}

		SYR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource) {

		switch (Renderer::getAPI()) {
			case RendererAPI::API::NONE:     SYR_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported!"); return nullptr;
			case RendererAPI::API::OPENGL:   return std::make_shared<OpenGLShader>(name, vertexSource, fragmentSource);
		}

		SYR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	void ShaderLibrary::add(const Ref<Shader>& shader) {
		auto& name = shader->getName();
		add(name, shader);
	}

	void ShaderLibrary::add(const std::string& name, const Ref<Shader>& shader) {
		SYR_CORE_ASSERT(!exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	Ref<Shader> ShaderLibrary::load(const std::string& filepath) {
		auto shader = Shader::create(filepath);
		add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::load(const std::string& name, const std::string& filepath) {
		auto shader = Shader::create(filepath);
		add(name, shader);
		return shader;
	}

	bool ShaderLibrary::exists(const std::string& name) {
		return m_Shaders.find(name) != m_Shaders.end();
	}

	Ref<Shader> ShaderLibrary::get(const std::string& name) {
		SYR_CORE_ASSERT(exists(name), "Shader {0} not found!", name);
		return m_Shaders[name];
	}

}