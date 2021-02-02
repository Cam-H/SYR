#pragma once

#include "SYR/Renderer/Shader.h"
#include <glm/glm.hpp>

//TODO remove
typedef unsigned int GLenum;

namespace SYR {

	class OpenGLShader : public Shader {
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string&name, const std::string& vertexSource, const std::string& fragmentSource);
		virtual ~OpenGLShader();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void setInt(const std::string& name, int value) override;
		virtual void setIntArray(const std::string& name, int* values, uint32_t count) override;

		virtual void setFloat(const std::string& name, float value) override;
		virtual void setFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void setFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void setFloat4(const std::string& name, const glm::vec4& value) override;

		virtual void setMat3(const std::string& name, const glm::mat3& value) override;
		virtual void setMat4(const std::string& name, const glm::mat4& value) override;

		virtual const std::string& getName() const override { return m_Name; }

		void uploadUniformInt(const std::string& name, int value);
		void uploadUniformIntArray(const std::string& name, int* values, uint32_t count);

		void uploadUniformFloat(const std::string& name, float value);
		void uploadUniformFloat2(const std::string& name, const glm::vec2& values);
		void uploadUniformFloat3(const std::string& name, const glm::vec3& values);
		void uploadUniformFloat4(const std::string& name, const glm::vec4& values);

		void uploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void uploadUniformMat4(const std::string& name, const glm::mat4& matrix);

		uint32_t rid() { return m_RendererID; }
	private:
		std::string readFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> preprocess(const std::string& source);
		void compile(const std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		uint32_t m_RendererID;

		std::string m_Name;
	};
}