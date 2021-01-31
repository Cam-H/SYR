#include "syrpch.h"
#include "OBJFileHandler.h"

#include <glm/glm.hpp>

namespace SYR {

	Ref<VertexArray> OBJFileHandler::parseOBJFile(const std::string& content) {
		const char* delimiter = "\n";
		size_t delimLength = strlen(delimiter);

		const char* vertexToken = "v";
		const char* faceToken = "f";

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<uint32_t> indices;

		std::unordered_map<std::string, uint32_t> vertexKeys;
		
		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		size_t pos = 0;
		size_t prev = 0;

		while ((pos = content.find(delimiter, prev)) != std::string::npos) {

			std::string line = content.substr(prev, pos - prev);

			size_t begin = line.find(' ');
			std::string token = line.substr(0, begin++);

			if (token == "v") {
				float x = (float)atof(line.substr(begin, line.find(' ', begin)).c_str());
				float y = (float)atof(line.substr(line.find(' ', begin), line.find_last_of(' ')).c_str());
				float z = (float)atof(line.substr(line.find_last_of(' ')).c_str());

				vertices.push_back(glm::vec3(x, y, z));

			} else if (token == "vn") {
				float x = (float)atof(line.substr(begin, line.find(' ', begin)).c_str());
				float y = (float)atof(line.substr(line.find(' ', begin), line.find_last_of(' ')).c_str());
				float z = (float)atof(line.substr(line.find_last_of(' ')).c_str());

				normals.push_back(glm::vec3(x, y, z));

			} else if (token == "f") {

				std::string faceContent = line.substr(begin);
				std::string vertices[3];
				vertices[0] = faceContent.substr(0, faceContent.find(' ')).c_str();
				vertices[1] = faceContent.substr(faceContent.find(' ') + 1, faceContent.find_last_of(' ') - faceContent.find(' ') - 1).c_str();
				vertices[2] = faceContent.substr(faceContent.find_last_of(' ') + 1).c_str();

				for (int i = 0; i < 3; i++) {
					std::unordered_map<std::string, uint32_t>::iterator vertexIndex = vertexKeys.find(vertices[i]);

					if (vertexIndex == vertexKeys.end()) {
						indices.push_back(vertexCount);
						vertexKeys.insert(std::pair<std::string, uint32_t>(vertices[i], vertexCount++));
					} else {
						indices.push_back(vertexIndex->second);
					}

					indexCount++;
				}
			}

			prev = pos + 1;
		}

		struct MeshVertex {
			glm::vec3 position;
			glm::vec3 normal;
		};

		MeshVertex* vertexBufferBase = new MeshVertex[vertexKeys.size()];

		for (std::unordered_map<std::string, uint32_t>::iterator it = vertexKeys.begin(); it != vertexKeys.end(); ++it) {
			uint32_t index = (uint32_t)atof(it->first.substr(0, it->first.find('/')).c_str()) - 1;
			uint32_t normalIndex = (uint32_t)atof(it->first.substr(it->first.find_last_of('/') + 1).c_str()) - 1;

			if (it->first.find("//") == std::string::npos) {//Vertex-Texture-Normal
				
			}

			(vertexBufferBase + it->second)->position = vertices.at(index);
			(vertexBufferBase + it->second)->normal = normals.at(normalIndex);
		}

		Ref<VertexArray> mesh = VertexArray::create();

		Ref<VertexBuffer> vb = VertexBuffer::create(vertexCount * sizeof(MeshVertex));
		vb->setLayout({ {ShaderDataType::FLOAT3, "position"}, {ShaderDataType::FLOAT3, "normal"} });
		vb->setData(vertexBufferBase, vertexCount * sizeof(MeshVertex));

		mesh->addVertexBuffer(vb);

		uint32_t* vertexIndices = new uint32_t[indexCount];

		for (uint32_t i = 0; i < indexCount; i++) {
			vertexIndices[i] = indices.at(i);
		}

		Ref<IndexBuffer> ib = IndexBuffer::create(vertexIndices, indexCount);
		mesh->setIndexBuffer(ib);

		return mesh;
	}


}