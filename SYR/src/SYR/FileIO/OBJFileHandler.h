#pragma once

#include "SYR/Renderer/VertexArray.h"

namespace SYR {

	class OBJFileHandler {
	public:

		static Ref<VertexArray> parseOBJFile(const std::string& content);
	private:

		//static void handleFace(std::string& faceContent, std::vector<glm::vec3> normals, std::vector<uint32_t>* indices, std::vector<glm::vec3>* vertexNormals);
	};

}
