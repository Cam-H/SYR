#pragma once

#include <glm/glm.hpp>

namespace SYR {

	//Data relevant to any physics calculations for the collider
	const struct ColliderPhysicsData {
		
		uint16_t vertexCount;
		glm::vec3* vertices;

		uint16_t normalCount;
		glm::vec3* normals;

		glm::vec3 center;//(Relative to vertex points) Geometric center / center of mass (collider assumed to have constant density)

		//Mass & MoI

		float m_BoundingRadius;
	};

	class Collider {
	public:

		Collider(ColliderPhysicsData collider, uint32_t ID, const std::string& name = "");

		const uint32_t getID() { return m_ID; }
		const std::string& getName() { return m_Name; }

		static Ref<Collider> create(const std::string& filepath, const std::string& name);

	private:
		static ColliderPhysicsData create(const std::string& filepath);
	private:

		const uint32_t m_ID;
		const std::string m_Name;

		ColliderPhysicsData m_ColliderPhysicsData;

		friend class ColliderLibrary;

		friend class CollisionSystem;

	};

	class ColliderLibrary {
	public:
		ColliderLibrary();
		
		Ref<Collider> load(const std::string& filepath, const std::string& name = "");

		bool exists(const std::string& name);
		bool exists(const uint32_t ID);

		Ref<Collider> get(const std::string& name);
		Ref<Collider> get(const uint32_t ID);

	private:
		std::vector<Ref<Collider>> m_Colliders;
		std::vector<std::string> m_ColliderNames;
	};
}