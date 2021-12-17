#pragma once

#include "SYR/Renderer/VertexArray.h"
#include <glm/glm.hpp>

namespace SYR {

	class OBJFileHandler {
	public:
		static Ref<VertexArray> loadMesh(const std::string& content);
		static Ref<VertexArray> loadSmoothMesh(const std::string& content);

		static void parseOBJFile(const std::string& content, std::vector<glm::vec3>* vertices, std::vector<glm::vec3>* normals, std::vector<std::vector<uint32_t>>* indices);

	private:
		
		static void parseOBJFile(const std::string& content, std::vector<glm::vec3>* vertices, std::vector<glm::vec3>* normals, std::vector<uint32_t>* indices, std::vector<std::array<uint32_t, 3>>* vertexKeys);

		static glm::vec2 splitString2(char delim, std::string& line);
		static glm::vec3 splitString3(char delim, std::string& line);
	};

}
