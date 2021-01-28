#include "syrpch.h"
#include "FileHandler.h"

#include <glm/glm.hpp>

namespace SYR {

	std::vector<uint32_t> FileHandler::parseUTF8File(const std::string& filepath) {
		std::vector<char> bytes = readBinaryFile(filepath);

		std::vector<uint32_t> content(0);

		for (std::vector<char>::iterator it = bytes.begin(); it != bytes.end(); ++it) {
			if ((*it & 0b10000000) == 0b00000000) {//1-byte UTF
				content.push_back(*it);
			} else if ((*it & 0b11100000) == 0b11000000) {//2-byte UTF
				content.push_back((0b00011111 & *it) << 6 | (0b00111111 & *(++it)));
			} else if ((*it & 0b11110000) == 0b11100000) {//3-byte UTF
				content.push_back((0b00001111 & *it) << 12 | (0b00111111 & *(++it)) << 6 | (0b00111111 & *(++it)));
			} else if ((*it & 0b11111000) == 0b11110000) {//4-byte UTF
				content.push_back((0b00000111 & *it) << 18 | (0b00111111 & *(++it)) << 12 | (0b00111111 & *(++it)) << 6 | (0b00111111 & *(++it)));
			}
		}

		return content;
	}

	Ref<VertexArray> FileHandler::loadMesh(const std::string& filepath) {

		FileFormat format = getFileFormat(filepath);

		std::string content = readFile(filepath);

		switch (format) {
		case FileFormat::OBJ:
		{
			const char* delimiter = "\n";
			size_t delimLength = strlen(delimiter);
				
			const char* vertexToken = "v";
			const char* faceToken = "f";

			std::vector<glm::vec3> vertices;
			std::vector<uint32_t> indices;

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
					//SYR_CORE_INFO("({0}, {1}, {2})", x, y, z);

					vertexCount++;
				}else if (token == "f") {

					std::string vertex1 = line.substr(begin, line.find(' ', begin)).c_str();
					std::string vertex2 = line.substr(line.find(' ', begin), line.find_last_of(' ')).c_str();
					std::string vertex3 = line.substr(line.find_last_of(' ')).c_str();

					uint32_t i1 = (uint32_t)atof(vertex1.substr(0, vertex1.find('/')).c_str()) - 1;
					uint32_t i2 = (uint32_t)atof(vertex2.substr(0, vertex2.find('/')).c_str()) - 1;
					uint32_t i3 = (uint32_t)atof(vertex3.substr(0, vertex3.find('/')).c_str()) - 1;

					indices.push_back(i1);
					indices.push_back(i2);
					indices.push_back(i3);

					//SYR_CORE_INFO("-> {0} {1} {2}", i1, i2, i3);

					indexCount += 3;
				}

				prev = pos + 1;
			}

			Ref<VertexArray> mesh = VertexArray::create();

			struct MeshVertex {
				glm::vec3 position;
			};

			Ref<VertexBuffer> vb = VertexBuffer::create(vertexCount * sizeof(MeshVertex));

			vb->setLayout({ {SYR::ShaderDataType::FLOAT3, "position"} });

			mesh->addVertexBuffer(vb);

			MeshVertex* vertexBufferBase = new MeshVertex[vertexCount];
			MeshVertex* vertexBufferPtr = vertexBufferBase;

			for (uint32_t i = 0; i < vertexCount; i++) {
				vertexBufferPtr->position = vertices.at(i);
				vertexBufferPtr++;
			}

			vb->setData(vertexBufferBase, vertexCount * sizeof(MeshVertex));

			uint32_t* vertexIndices = new uint32_t[indexCount];
			
			for (uint32_t i = 0; i < indexCount; i++) {
				vertexIndices[i] = indices.at(i);
			}

			SYR::Ref<SYR::IndexBuffer> ib = SYR::IndexBuffer::create(vertexIndices, indexCount);
			mesh->setIndexBuffer(ib);

			SYR_CORE_INFO("{0} {1}", vertexCount, indexCount);

			return mesh;
		}
			break;
		default:
			SYR_CORE_WARN("Could not load mesh!");
			return VertexArray::create();
		}

		return VertexArray::create();
	}

}