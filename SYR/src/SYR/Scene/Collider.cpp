#include "syrpch.h"
#include "Collider.h"

#include "SYR/FileIO/FileHandler.h"
#include "SYR/FileIO/OBJFileHandler.h"

#include "SYR/Renderer/Renderer3D.h"

#include <glm/gtx/projection.hpp>

namespace SYR {

	Collider::Collider(ColliderPhysicsData collider, uint32_t ID, const std::string& name) : m_ColliderPhysicsData(collider), m_ID(ID), m_Name(name) {}

	ColliderPhysicsData Collider::create(const std::string& filepath) {

		switch (FileHandler::getFileFormat(filepath)) {
		case FileFormat::OBJ:
		{
			std::string data = FileHandler::readFile(filepath);

			std::vector<glm::vec3> vertices;
			std::vector<glm::vec3> normals;
			std::vector<std::vector<uint32_t>> indices;

			OBJFileHandler::parseOBJFile(data, &vertices, &normals, &indices);

			//Remove all collinear normals -> Limits the amount of calculations needed later
			for (uint16_t i = 0; i < normals.size(); i++) {
				for (uint16_t j = i + 1; j < normals.size(); j++) {
					if (glm::length(normals.at(i)) * glm::length(normals.at(j)) - abs(glm::dot(normals.at(i), normals.at(j))) <= 0.0001f) {

						//Remove the duplicate normal
						normals.at(j) = normals.at(normals.size() - 1);
						normals.pop_back();
					}
				}
			}

			//TODO
			//Calculate mass and moment of inertia, bounding radius

			glm::vec3 center{};
			float boundingRadius = 0;

			uint16_t indexCount = 0;
			

			//Count the number of indices that are needed
			for (std::vector<std::vector<uint32_t>>::iterator it = indices.begin(); it != indices.end(); ++it) {
				indexCount += it->end() - it->begin();
			}

			uint32_t* indicesArr = new uint32_t[2 * indexCount];
			indexCount = 0;

			for (std::vector<std::vector<uint32_t>>::iterator it = indices.begin(); it != indices.end(); ++it) {
				for (std::vector<uint32_t>::iterator element = it->begin(); element != it->end(); ++element) {
					indicesArr[indexCount] = *element;
					indicesArr[indexCount + 1] = (element + 1 == it->end()) ? *(it->begin()) : *(element + 1);

					indexCount += 2;
				}
			}

			ColliderPhysicsData collider =
			{
				vertices.size(),
				new glm::vec3[vertices.size()],
				normals.size(),
				new glm::vec3[normals.size()],
				center,
				boundingRadius,
				indexCount,
				indicesArr
				
			};

			//Fill
			for (uint32_t i = 0; i < collider.vertexCount; i++) {
				collider.vertices[i] = vertices.at(i);
			}

			for (uint32_t i = 0; i < collider.normalCount; i++) {
				collider.normals[i] = normals.at(i);
			}

			return collider;
		}
		break;
		case FileFormat::BIN: //return std::make_shared<>(filepath);
		default:
			SYR_CORE_WARN("Can not load hitbox from {0}, invalid file format", filepath);
		}

		return {};
	}

	Ref<Collider> Collider::create(const std::string& filepath, const std::string& name) {
		ColliderPhysicsData collider = create(filepath);

		return std::make_shared<Collider>(collider, 0, name);
	}

	void Collider::render(const glm::mat4& transform) {
		for (uint32_t i = 0; i < m_ColliderPhysicsData.indexCount; i+=2) {
			Renderer3D::drawLine(m_ColliderPhysicsData.vertices[m_ColliderPhysicsData.indices[i]], m_ColliderPhysicsData.vertices[m_ColliderPhysicsData.indices[i + 1]], { 1, 1, 0, 1 }, transform);
		}
	}


	Ref<Collider> ColliderLibrary::load(const std::string& filepath, const std::string& name) {
		auto collider = Collider::create(filepath);
		m_Colliders.push_back(std::make_shared<Collider>(collider, m_Colliders.size(), name));
		return m_Colliders.at(m_Colliders.size() - 1);
	}

	bool ColliderLibrary::exists(const std::string& name) {
		for (std::vector<std::string>::iterator it = m_ColliderNames.begin(); it != m_ColliderNames.end(); ++it) {
			if (name == *it) {
				return true;
			}
		}

		return false;
	}

	bool ColliderLibrary::exists(const uint32_t ID) {
		return ID < m_Colliders.size();
	}

	Ref<Collider> ColliderLibrary::get(const std::string& name) {
		SYR_CORE_ASSERT(exists(name), "Collider [Name: {0}] not found!", name);

		for (std::vector<std::string>::iterator it = m_ColliderNames.begin(); it != m_ColliderNames.end(); ++it) {
			if (name == *it) {
				return get((uint32_t)(it - m_ColliderNames.begin()));
			}
		}

		return nullptr;
	}

	Ref<Collider> ColliderLibrary::get(uint32_t ID) {
		SYR_CORE_ASSERT(exists(ID), "Collider [ID: {0}] not found!", ID);
		return m_Colliders.at(ID);
	}
}