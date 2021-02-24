#pragma once

#include "SYR/Renderer/VertexArray.h"
#include <glm/glm.hpp>

namespace SYR {

	class OBJFileHandler {
	public:
		static Ref<VertexArray> loadMesh(const std::string& content);
		static Ref<VertexArray> loadSmoothMesh(const std::string& content);

	private:
		static void parseOBJFile(const std::string& content, std::vector<glm::vec3>* vertices, std::vector<glm::vec3>* normals, std::vector<uint32_t>* indices, std::unordered_map<std::string, uint32_t>* vertexKeys);
	};

}
