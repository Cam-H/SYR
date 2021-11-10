#include "syrpch.h"
#include "OBJFileHandler.h"

#include "SYR/Renderer/Renderer.h"

namespace SYR {

	Ref<VertexArray> OBJFileHandler::loadMesh(const std::string& content) {

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<uint32_t> indices;
		std::vector<std::array<uint32_t, 3>> vertexKeys;

		parseOBJFile(content, &vertices, &normals, &indices, &vertexKeys);

		uint32_t indexCount = indices.size();
		uint32_t vertexCount = vertexKeys.size();

		struct MeshVertex {
			glm::vec3 position;
			glm::vec3 normal;
			float material;
		};

		MeshVertex* vertexBufferBase = new MeshVertex[vertexKeys.size()];

		for (std::vector<std::array<uint32_t, 3>>::iterator it = vertexKeys.begin(); it != vertexKeys.end(); ++it) {
			uint32_t offset = it - vertexKeys.begin();
			/*
			uint32_t index = (uint32_t)atof(it->substr(0, it->find('/')).c_str()) - 1;
			uint32_t normalIndex = (uint32_t)atof(it->substr(it->find_last_of('/') + 1, it->find(' ') - it->find_last_of('/') - 1).c_str()) - 1;
			float materialIndex = (float)atof(it->substr(it->find(' ') + 1).c_str()) - 1;
			*/

			//if (it->find("//") == std::string::npos) {//Vertex-Texture-Normal
			//
			//}


			(vertexBufferBase + offset)->position = vertices.at(it->at(0));
			(vertexBufferBase + offset)->normal = normals.at(it->at(1));
			(vertexBufferBase + offset)->material = (float)it->at(2);
			
			//SYR_CORE_WARN("MAT: {0} {1} {2}", it->at(2), (float)it->at(2), (vertexBufferBase + offset)->material);

			//SYR_CORE_INFO("{0} {1} {2}", (vertexBufferBase + offset)->position.x, (vertexBufferBase + offset)->normal.x, it->z);

		}

		Ref<VertexArray> mesh = VertexArray::create();

		Ref<VertexBuffer> vb = VertexBuffer::create(vertexCount * sizeof(MeshVertex));
		vb->setLayout({ {ShaderDataType::FLOAT3, "position"}, {ShaderDataType::FLOAT3, "normal"}, {ShaderDataType::FLOAT, "material"} });
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

	/**
	* NOTE: Face material data is tossed
	*/
	Ref<VertexArray> OBJFileHandler::loadSmoothMesh(const std::string& content) {
		
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<uint32_t> indices;
		std::vector<std::array<uint32_t, 3>> vertexKeys;

		parseOBJFile(content, &vertices, &normals, &indices, &vertexKeys);

		uint32_t indexCount = indices.size();
		uint32_t vertexCount = vertices.size();

		glm::vec3* smoothedVertexNormals = new glm::vec3[vertexCount];
		uint8_t* vertexNormalCount = new uint8_t[vertexCount];

		//Initialize smoothed vertex arrays
		for (uint32_t i = 0; i < vertexCount; i++) {
			smoothedVertexNormals[i] = {0, 0, 0};
			vertexNormalCount[i] = 0;
		}

		//Sum all normals for a given vertex
		for (std::vector<std::array<uint32_t, 3>>::iterator it = vertexKeys.begin(); it != vertexKeys.end(); ++it) {
			smoothedVertexNormals[it->at(0)] += normals.at(it->at(1));
			vertexNormalCount[it->at(0)]++;
		}

		//
		for (uint32_t i = 0; i < vertexCount; i++) {
			smoothedVertexNormals[i] /= vertexNormalCount[i];
		}

		struct MeshVertex {
			glm::vec3 position;
			glm::vec3 normal;
		};

		MeshVertex* vertexBufferBase = new MeshVertex[vertexCount];

		for (uint32_t i = 0; i < vertexCount; i++) {

			(vertexBufferBase + i)->position = vertices.at(i);
			(vertexBufferBase + i)->normal = smoothedVertexNormals[i];

		}

		Ref<VertexArray> mesh = VertexArray::create();

		Ref<VertexBuffer> vb = VertexBuffer::create(vertexCount * sizeof(MeshVertex));
		vb->setLayout({ {ShaderDataType::FLOAT3, "position"}, {ShaderDataType::FLOAT3, "normal"} });
		vb->setData(vertexBufferBase, vertexCount * sizeof(MeshVertex));

		mesh->addVertexBuffer(vb);

		uint32_t* vertexIndices = new uint32_t[indexCount];

		for (uint32_t i = 0; i < indexCount; i++) {
			vertexIndices[i] = vertexKeys.at(indices.at(i))[0];
		}

		Ref<IndexBuffer> ib = IndexBuffer::create(vertexIndices, indexCount);
		mesh->setIndexBuffer(ib);


		return mesh;
	}

	void OBJFileHandler::parseOBJFile(const std::string& content, std::vector<glm::vec3>* vertices, std::vector<glm::vec3>* normals, std::vector<uint32_t>* indices, std::vector<std::array<uint32_t, 3>>* vertexKeys) {
		std::string path = content.substr(0, content.find("\n"));

		const char* delimiter = "\n";
		size_t delimLength = strlen(delimiter);

		const char* vertexToken = "v";
		const char* faceToken = "f";

		uint32_t vertexCount = 0;

		uint32_t materialID = 0;

		size_t pos = 0;
		size_t prev = 0;

		while ((pos = content.find(delimiter, prev)) != std::string::npos) {

			std::string line = content.substr(prev, pos - prev);

			size_t begin = line.find(' ');
			std::string token = line.substr(0, begin++);
			
			if (token == "v") {//Load vertex
				vertices->push_back(splitString3(' ', line.substr(begin, line.length() - line.find_first_of(' '))));
			} else if (token == "vn") {//Load vertex normals
				normals->push_back(splitString3(' ', line.substr(begin, line.length() - line.find_first_of(' '))));
			} else if (token == "mtllib") {//Load specified material
				SYR_CORE_INFO("Loading {0}", (path + line.substr(begin)));
				Renderer::getMaterialLibrary()->loadMaterials(path + line.substr(begin));
			} else if (token == "usemtl") {//Assign specified material to subsequent faces
				materialID = Renderer::getMaterialLibrary()->getIDOfMaterial(line.substr(begin));
				SYR_CORE_INFO("Assign material:  {0} ", materialID);
			} else if (token == "f") {//Load a face

				//Split the face data into the three vertices
				std::string faceContent = line.substr(begin);
				std::string vertices[3];
				vertices[0] = faceContent.substr(0, faceContent.find(' ')).c_str();
				vertices[1] = faceContent.substr(faceContent.find(' ') + 1, faceContent.find_last_of(' ') - faceContent.find(' ') - 1).c_str();
				vertices[2] = faceContent.substr(faceContent.find_last_of(' ') + 1).c_str();

				//Process each vertex separately
				for (uint8_t i = 0; i < 3; i++) {

					uint32_t index = (uint32_t)atof(vertices[i].substr(0, vertices[i].find('/')).c_str()) - 1;
					uint32_t normalIndex = (uint32_t)atof(vertices[i].substr(vertices[i].find_last_of('/') + 1, vertices[i].find(' ') - vertices[i].find_last_of('/') - 1).c_str()) - 1;

					indices->push_back(vertexCount++);//If not registered, add the new combo to the index list

					vertexKeys->push_back(std::array<uint32_t, 3>{index, normalIndex, materialID});

					//TODO remove duplicates to save on memory

					//glm::vec3 eq = glm::equal(glm::vec3(5.0f, 2.0f, 1.0f), glm::vec3(5.0f, 2.0f, 1.0f));
					//SYR_CORE_INFO("{0} {1} {2}", eq.x, eq.y, eq.z);

					/*
					auto it = std::find(vertexKeys->begin(), vertexKeys->end(), vertices[i]);

					//Check if the particular vertex/normal index combination is registered
					if (it == vertexKeys->end()) {
						indices->push_back(vertexCount++);//If not registered, add the new combo to the index list
						vertexKeys->push_back(glm::ivec3(index, normalIndex, materialIndex));
					} else {
						indices->push_back(it - vertexKeys->begin());//If registered, add the existing combo to the index list
					}
					*/
				}
			}

			prev = pos + 1;
		}
	}

	glm::vec2 OBJFileHandler::splitString2(char delim, std::string& line) {//TODO test

		float x = (float)atof(line.substr(0, line.find(delim, 0)).c_str());
		float y = (float)atof(line.substr(line.find_last_of(delim)).c_str());

		return { x, y };
	}

	glm::vec3 OBJFileHandler::splitString3(char delim, std::string& line) {

		float x = (float)atof(line.substr(0, line.find(delim, 0)).c_str());
		float y = (float)atof(line.substr(line.find(delim), line.find_last_of(delim)).c_str());
		float z = (float)atof(line.substr(line.find_last_of(delim)).c_str());

		return { x, y, z };
	}
}