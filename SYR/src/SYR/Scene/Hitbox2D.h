#pragma once

#include <glm/glm.hpp>
#include <entt.hpp>

#include <vector>

namespace SYR {

	class Hitbox2D {
	public:
		Hitbox2D() {
			SYR_CORE_WARN("Empty Hitbox2D declaration!");

			init();
		}

		Hitbox2D(const glm::vec2 vertices[], uint16_t vertexCount) : m_Vertices(vertices), m_VertexCount(vertexCount) {
			m_TransformedVertices = new glm::vec2[vertexCount];
			applyNewTransform(glm::mat4(1.0f));

			m_TransformedRadius = m_Radius = 5;//Hitbox is treated as a circle if vertexCount = 1

			init();
		}

		Hitbox2D(glm::vec2 position, float radius) : m_Position(position), m_Radius(radius) {
			m_Vertices = m_TransformedVertices = nullptr;
			m_VertexCount = 1;

			m_TransformedRadius = radius;

			init();
		}

		Hitbox2D(std::vector<Hitbox2D> hitboxes) : m_Hitboxes(hitboxes) {
			init();

			m_VertexCount = 1;
			m_HitboxCount = hitboxes.size();

			m_TransformedPosition = m_Position = getCenter();

			glm::vec2 extremity = getExtremity(m_Position);
			m_TransformedRadius = m_Radius = glm::length(glm::vec2(m_Position.x - extremity.x, m_Position.y - extremity.y));
		}

		void applyNewTransform(glm::mat4& transform);

		bool checkCollisions(Hitbox2D collider);

		void setCollider(entt::entity collider);
		void removeCollider();
		void removeAllColliders();

		void setActive(bool active);
		void setHidden(bool hidden);

		const glm::vec2* getVertices() {
			return m_TransformedVertices;
		}

		uint32_t getVertexCount() {
			return m_VertexCount;
		}

		const glm::vec2& getPosition() {
			return m_TransformedPosition;
		}

		float getRadius() {
			return m_TransformedRadius;
		}

		entt::entity getCollider() { return m_Collider; }

		glm::vec2 getOverlap() { return m_Overlap; }

		glm::vec4 getColor() {
			return m_Active ? (!m_Colliding ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)) : glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
		}

		void render();

	private:
		void init();

		glm::vec2 getCenter();
		glm::vec2 getExtremity(const glm::vec2 center);
	private:
		std::vector<Hitbox2D> m_Hitboxes;
		size_t m_HitboxCount;

		const glm::vec2* m_Vertices;
		glm::vec2* m_TransformedVertices;

		uint16_t m_VertexCount;

		glm::vec2 m_Position;
		glm::vec2 m_TransformedPosition;

		float m_Radius;
		float m_TransformedRadius;

		bool m_Colliding;
		entt::entity m_Collider;
		glm::vec2 m_Overlap;

		bool m_Active;
		bool m_Hidden;

		//friend class CollisionSystem;
	};

	

}