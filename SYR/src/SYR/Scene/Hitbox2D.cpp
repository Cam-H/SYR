#include "syrpch.h"
#include "Hitbox2D.h"

#include "SYR/Renderer/Renderer2D.h"

#include "SYR/Scene/CollisionSystem.h"

namespace SYR {

	void Hitbox2D::init() {
		m_Colliding = false;
		m_Collider = entt::null;
		m_Overlap = { 0.0f, 0.0f };

		m_Position = { 0.0f, 0.0f };
		m_HitboxCount = 0;

		m_Active = true;
		m_Hidden = false;
	}

	void Hitbox2D::applyNewTransform(glm::mat4& transform) {
		if (m_VertexCount > 1) {
			for (int i = 0; i < m_VertexCount; i++) {
				glm::vec4 vertex = { m_Vertices[i].x, m_Vertices[i].y, 1.0f, 1.0f };
				glm::vec3 pos = transform * vertex;

				m_TransformedVertices[i] = { pos.x, pos.y };
			}
		}else {//TODO also transform radius
			glm::vec4 vertex = { m_Position.x, m_Position.y, 1.0f, 1.0f };
			glm::vec3 pos = transform * vertex;

			m_TransformedPosition = { pos.x, pos.y};
		}

		for (std::vector<Hitbox2D>::iterator it = m_Hitboxes.begin(); it != m_Hitboxes.end(); ++it) {
			(*it).applyNewTransform(transform);
		}
	}

	bool Hitbox2D::checkCollisions(Hitbox2D collider) {

		//Only check active hitboxes
		if (!m_Active) {
			return false;
		}

		glm::vec2 overlap = CollisionSystem::getOverlap((*this), collider);

		if (overlap.x != 0 || overlap.y != 0) {
			//TODO break collider hitboxes down to run collisions

			m_Overlap = overlap;
			m_Colliding = true;

			collider.m_Overlap = overlap;
			collider.m_Colliding = true;

			for (std::vector<Hitbox2D>::iterator it = m_Hitboxes.begin(); it != m_Hitboxes.end(); ++it) {
				(*it).checkCollisions(collider);
			}

			return true;
		}

		return false;
	}

	void Hitbox2D::setCollider(entt::entity collider) {
		m_Collider = collider;
	}

	void Hitbox2D::removeCollider() {
		m_Colliding = false;
		m_Collider = entt::null;
		m_Overlap = { 0.0f, 0.0f };
	}

	void Hitbox2D::removeAllColliders() {

		for (std::vector<Hitbox2D>::iterator it = m_Hitboxes.begin(); it != m_Hitboxes.end(); ++it) {
			(*it).removeAllColliders();
		}

		m_Colliding = false;
		m_Collider = entt::null;
		m_Overlap = { 0.0f, 0.0f };
	}

	void Hitbox2D::setActive(bool active) {
		if (!active) {
			for (std::vector<Hitbox2D>::iterator it = m_Hitboxes.begin(); it != m_Hitboxes.end(); ++it) {
				(*it).setActive(false);
			}
		}

		m_Active = active;
	}

	void Hitbox2D::setHidden(bool hidden) {
		if (hidden) {
			for (std::vector<Hitbox2D>::iterator it = m_Hitboxes.begin(); it != m_Hitboxes.end(); ++it) {
				(*it).setHidden(true);
			}
		}

		m_Hidden = hidden;
	}

	glm::vec2 Hitbox2D::getCenter() {
		glm::vec2 sum(0.0f);
		uint16_t vertexCount = this->m_VertexCount;

		if (vertexCount == 1) {
			sum.x += m_Position.x;
			sum.y += m_Position.y;
		} else {
			for (int i = 0; i < vertexCount; i++) {
				sum.x += m_Vertices[i].x;
				sum.y += m_Vertices[i].y;
			}
		}

		for (std::vector<Hitbox2D>::iterator it = m_Hitboxes.begin(); it != m_Hitboxes.end(); ++it) {
			glm::vec2 center = (*it).getCenter();

			sum.x += center.x * (*it).getVertexCount();
			sum.y += center.y * (*it).getVertexCount();

			vertexCount += (*it).getVertexCount();
		}

		return { sum.x / vertexCount, sum.y / vertexCount };
	}

	glm::vec2 Hitbox2D::getExtremity(const glm::vec2 center) {

		glm::vec2 extremity;
		float distance;

		if (m_VertexCount == 1) {
			extremity = m_Position;
			distance = pow(center.x - extremity.x, 2) + pow(center.y - extremity.y, 2);
		}else {
			extremity = m_Position;
			distance = pow(center.x - extremity.x, 2) + pow(center.y - extremity.y, 2);

			for (int i = 1; i < m_VertexCount; i++) {
				float temp = pow(center.x - m_Vertices[i].x, 2) + pow(center.y - m_Vertices[i].y, 2);

				if (temp > distance) {
					extremity = m_Vertices[i];
					distance = temp;
				}
			}
		}

		for (std::vector<Hitbox2D>::iterator it = m_Hitboxes.begin(); it != m_Hitboxes.end(); ++it) {
			glm::vec2 subExtreme = (*it).getExtremity(center);

			float temp = pow(center.x - subExtreme.x, 2) + pow(center.y - subExtreme.y, 2);

			if (temp > distance) {
				extremity = subExtreme;
				distance = temp;
			}
		}

		return extremity;
	}

	void Hitbox2D::render() {

		//Skip rendering all sub-hitboxes if the parent is hidden
		if (m_Hidden) {
			return;
		}

		for (std::vector<Hitbox2D>::iterator it = m_Hitboxes.begin(); it != m_Hitboxes.end(); ++it) {
			(*it).render();
		}

		if (m_VertexCount > 1) {
			Renderer2D::drawLines(m_TransformedVertices, m_VertexCount, getColor());
		}else {
			Renderer2D::drawCircle(m_TransformedPosition, m_TransformedRadius, getColor());
		}

	}
}