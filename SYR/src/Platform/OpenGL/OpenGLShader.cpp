#include "syrpch.h"

#include "OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace SYR {

	static GLenum shaderTypeFromString(const std::string& type) {

		if (type == "vertex") 
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") 
			return GL_FRAGMENT_SHADER;

		SYR_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath) {
		std::string shaderSource = readFile(filepath);
		auto shaderSources = preprocess(shaderSource);

		compile(shaderSources);

		//Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;

		m_Name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource) : m_Name(name) {

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSource;
		sources[GL_FRAGMENT_SHADER] = fragmentSource;

		compile(sources);
	}

	std::string OpenGLShader::readFile(const std::string& filepath) {
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);

		if (in) {
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else {
			SYR_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::preprocess(const std::string& source) {
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);

		while (pos != std::string::npos) {
			size_t eol = source.find_first_of("\r\n", pos);
			SYR_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			SYR_CORE_ASSERT(shaderTypeFromString(type), "Invalid shader type specified!");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[shaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shaderSources) {

		GLuint program = glCreateProgram();

		SYR_CORE_ASSERT(shaderSources <= 2, "Only two shaders are supported currently!");
		std::array<GLenum, 2> glShaderIDs;

		int glShaderIDIndex = 0;

		m_RendererID = -1;

		for (auto& kv : shaderSources) {
			GLenum shaderType = kv.first;
			const std::string& shaderSource = kv.second;

			GLuint shader = glCreateShader(shaderType);

			const GLchar* source = shaderSource.c_str();
			glShaderSource(shader, 1, &source, 0);

			glCompileShader(shader);

			GLint status = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

			if (status == GL_FALSE) {
				int maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				SYR_CORE_ERROR("{0} shader compilation failure!", shaderType);
				SYR_CORE_ERROR("{0}", infoLog.data());

				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE) {
			int maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			for (auto id : glShaderIDs) {
				glDeleteShader(id);
			}

			SYR_CORE_ERROR("Shader program link failure!");
			SYR_CORE_ERROR("{0}", infoLog.data());

			return;
		}

		for (auto id : glShaderIDs) {
			glDetachShader(program, id);
		}

		m_RendererID = program;
	}

	OpenGLShader::~OpenGLShader(){
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::bind() const {
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::unbind() const {
		glUseProgram(0);
	}

	void OpenGLShader::setInt(const std::string& name, const int value) {
		uploadUniformInt(name, value);
	}

	void OpenGLShader::setIntArray(const std::string& name, int* values, uint32_t count) {
		uploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::setFloat(const std::string& name, float value) {
		uploadUniformFloat(name, value);
	}

	void OpenGLShader::setFloat2(const std::string& name, const glm::vec2& value) {
		uploadUniformFloat2(name, value);
	}

	void OpenGLShader::setFloat3(const std::string& name, const glm::vec3& value) {
		uploadUniformFloat3(name, value);
	}
	
	void OpenGLShader::setFloat4(const std::string& name, const glm::vec4& value) {
		uploadUniformFloat4(name, value);
	}

	void OpenGLShader::setMat3(const std::string& name, const glm::mat3& value) {
		uploadUniformMat3(name, value);
	}
	
	void OpenGLShader::setMat4(const std::string& name, const glm::mat4& value) {
		uploadUniformMat4(name, value);
	}

	void OpenGLShader::uploadUniformInt(const std::string& name, int value) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::uploadUniformIntArray(const std::string& name, int* values, uint32_t count) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::uploadUniformFloat(const std::string& name, float value) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::uploadUniformFloat2(const std::string& name, const glm::vec2& values) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, values.x, values.y);
	}

	void OpenGLShader::uploadUniformFloat3(const std::string& name, const glm::vec3& values) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, values.x, values.y, values.z);
	}

	void OpenGLShader::uploadUniformFloat4(const std::string& name, const glm::vec4& values) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::uploadUniformMat3(const std::string& name, const glm::mat3& matrix) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::uploadUniformMat4(const std::string& name, const glm::mat4& matrix) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}