#include "syrpch.h"
#include "Collider.h"

#include "SYR/FileIO/FileHandler.h"
#include "SYR/FileIO/OBJFileHandler.h"

namespace SYR {

	Collider::Collider(ColliderPhysicsData collider, uint32_t ID, const std::string& name) : m_ColliderPhysicsData(collider), m_ID(ID), m_Name(name) {}

	ColliderPhysicsData Collider::create(const std::string& filepath) {

		switch (FileHandler::getFileFormat(filepath)) {
		case FileFormat::OBJ:
		{
			std::string data = FileHandler::readFile(filepath);

			std::vector<glm::vec3> vertices;
			std::vector<glm::vec3> normals;
			std::vector<uint32_t> indices;
			std::vector<std::array<uint32_t, 3>> vertexKeys;

			OBJFileHandler::parseOBJFile(data, &vertices, &normals, &indices, &vertexKeys);

			SYR_CORE_ERROR("SIZE: {0} {1} {2}", vertices.size(), vertexKeys.size(), normals.size());

			//TODO
			//Calculate mass and moment of inertia, bounding radius

			glm::vec3 center{};
			float boundingRadius = 0;

			ColliderPhysicsData collider =
			{
				vertices.size(),
				new glm::vec3[vertices.size()],
				normals.size(),
				new glm::vec3[normals.size()],
				center,
				boundingRadius
			};

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