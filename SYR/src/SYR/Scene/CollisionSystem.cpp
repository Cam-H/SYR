#include "syrpch.h"
#include "CollisionSystem.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <glm/gtx/projection.hpp>

#include <glm/gtx/matrix_decompose.hpp>

#include "SYR/Renderer/Renderer2D.h"

#define PI 3.14159265358979323846

namespace SYR {

	void checkCollisions(entt::registry& registry) {
		auto view = registry.view<TransformComponent, Hitbox2DComponent>();
		
		for (const entt::entity e1 : view) {
			for (const entt::entity e2 : view) {
				if (e1 <= e2) {
					continue;
				}

				view.get<Hitbox2DComponent>(e1).hitbox.checkCollisions(view.get<Hitbox2DComponent>(e2).hitbox);

				/*
				glm::vec2 overlap = CollisionSystem::getOverlap(view.get<Hitbox2DComponent>(e1), view.get<Hitbox2DComponent>(e2));

				if (overlap.x != 0 || overlap.y != 0) {
					view.get<Hitbox2DComponent>(e1).addCollider(e2, overlap);
					view.get<Hitbox2DComponent>(e2).addCollider(e1, overlap);
				}
				*/
			}
			//view.get<Hitbox2DComponent>(e).addCollider(e);
		}
	}

	void handleCollisions(entt::registry& registry) {
		auto view = registry.view<TransformComponent, Hitbox2DComponent>();

		for (const entt::entity e : view) {
			/*
			entt::entity collider = view.get<Hitbox2DComponent>(e).getCollider();

			if (collider != entt::null) {
				glm::vec2 overlap = view.get<Hitbox2DComponent>(e).getOverlap();
				//view.get<TransformComponent>(e).offset(glm::vec3(-overlap.x, -overlap.y, 0.0f));
				view.get<TransformComponent>(collider).offset(glm::vec3(overlap.x, overlap.y, 0.0f));

				SYR_CORE_INFO("{0} {1}", overlap.x, overlap.y);

				view.get<Hitbox2DComponent>(e).removeCollider();
				view.get<Hitbox2DComponent>(collider).removeCollider();
			}
			*/

			view.get<Hitbox2DComponent>(e).removeAllColliders();

			//view.get<Hitbox2DComponent>(e).addCollider(entt::null);
		}
	}

	const glm::vec4* CollisionSystem::getEdges(glm::vec2 center, const glm::vec2* vertices, uint16_t vertexCount) {
		glm::vec4* edges = new glm::vec4[vertexCount];

		for (int i = 0; i < vertexCount; i++) {
			edges[i] = {center.x, center.y, vertices[i].x, vertices[i].y};
		}

		return edges;
	}

	const glm::vec4* CollisionSystem::getEdges(const glm::vec2* vertices, uint16_t vertexCount) {
		glm::vec4* edges = new glm::vec4[vertexCount];

		for (uint16_t i = 0; i < vertexCount - 1; i++) {
			glm::vec2 normal = { -(vertices[i + 1].y - vertices[i].y), (vertices[i + 1].x - vertices[i].x) };
			edges[i] = glm::vec4(vertices[i].x, vertices[i].y, vertices[i].x + normal.x, vertices[i].y + normal.y);
		}

		edges[vertexCount - 1] = glm::vec4(vertices[0].x, vertices[0].y, vertices[0].x - (vertices[0].y - vertices[vertexCount - 1].y), vertices[0].y + (vertices[0].x - vertices[vertexCount - 1].x));


		return edges;
	}

	const glm::vec4* CollisionSystem::getEdges(Hitbox2D hitbox) {
		return getEdges(hitbox.getVertices(), hitbox.getVertexCount());
	}

	const glm::vec4* CollisionSystem::getUniqueAxes(const glm::vec4* aAxes, uint16_t aCount, const glm::vec4* bAxes, uint16_t bCount, uint16_t& uniqueAxisCount) {

		uniqueAxisCount = 1;
		uint16_t maxAxisCount = aCount + bCount;

		glm::vec4* uniqueAxes = new glm::vec4[maxAxisCount];

		uniqueAxes[0] = aAxes[0];

		for (uint16_t i = 1; i < aCount; i++) {

			for (uint16_t j = 0; j < uniqueAxisCount; j++) {
				if (areParallel(uniqueAxes[j], aAxes[i]) && false) {
					break;
				}

				if (j == uniqueAxisCount - 1) {
					uniqueAxes[uniqueAxisCount] = aAxes[i];

					uniqueAxisCount++;
					break;
				}
			}
		}

		for (uint16_t i = 0; i < bCount; i++) {

			for (uint16_t j = 0; j < uniqueAxisCount; j++) {
				if (areParallel(uniqueAxes[j], bAxes[i]) && false) {
					break;
				}

				if (j == uniqueAxisCount - 1) {
					uniqueAxes[uniqueAxisCount] = bAxes[i];

					uniqueAxisCount++;
					break;
				}
			}
		}

		return uniqueAxes;
	}

	glm::vec4 CollisionSystem::getShadow(glm::vec4 surface, const glm::vec2 position, float radius) {

		glm::vec2 axis = { surface.z - surface.x, surface.w - surface.y };
		float scale = glm::length(axis);

		glm::vec2 projection = glm::proj(position, axis);

		glm::vec4 shadow = { projection.x - radius * axis.x / scale, projection.y - radius * axis.y / scale , projection.x + radius * axis.x / scale , projection.y + radius * axis.y / scale };

#ifdef _DEBUG
		Renderer2D::drawLine(glm::vec2(shadow.x, shadow.y), glm::vec2(shadow.z, shadow.w), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
#endif

		return shadow;
	}

	glm::vec4 CollisionSystem::getShadow(glm::vec4 surface, const glm::vec2* vertices, uint16_t vertexCount) {

		glm::vec2 axis = {surface.z - surface.x, surface.w - surface.y};

		glm::vec2* projections = new glm::vec2[vertexCount];

		for (uint16_t i = 0; i < vertexCount; i++) {
			projections[i] = glm::proj(vertices[i], axis);
		}

		glm::vec4 shadow = { projections[0], projections[1] };
		float max = pow(shadow.z - shadow.x, 2) + pow(shadow.w - shadow.y, 2);

		for (uint16_t i = 0; i < vertexCount; i++) {
			for (uint16_t j = i + 1; j < vertexCount; j++) {
				float temp = pow(projections[j].x - projections[i].x, 2) + pow(projections[j].y - projections[i].y, 2);

				if (temp > max) {
					shadow = { projections[i], projections[j] };
					max = temp;
				}
			}
		}

#ifdef _DEBUG
		Renderer2D::drawLine(glm::vec2(shadow.x, shadow.y), glm::vec2(shadow.z, shadow.w), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
#endif

		return shadow;
	}

	const glm::vec2* CollisionSystem::getOutline(Hitbox2D hitbox) {

		float scale = 1.4f;

		glm::vec2 sum = { 0.0f, 0.0f };
		glm::vec2* outlineVertices = new glm::vec2[hitbox.getVertexCount()];

		const glm::vec2* vertices = hitbox.getVertices();

		for (uint16_t i = 0; i < hitbox.getVertexCount(); i++) {
			sum.x -= vertices[i].x * (1 - scale);
			sum.y -= vertices[i].y * (1 - scale);
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { -sum.x / hitbox.getVertexCount(), -sum.y / hitbox.getVertexCount(), 0.0f }) * glm::scale(glm::mat4(1.0f), { scale, scale, 1.0f });


		for (uint16_t i = 0; i < hitbox.getVertexCount(); i++) {
			glm::vec3 vertex = transform * glm::vec4(hitbox.getVertices()[i], 1.0f, 1.0f);

			outlineVertices[i] = { vertex.x, vertex.y };
		}

		return outlineVertices;
	}

	glm::vec2 CollisionSystem::getOverlap(const glm::vec2& c1, float r1, const glm::vec2& c2, float r2) {

		if (c1.x == c2.x && c1.y == c2.y) {
			return glm::vec2(0.0f, 0.0f);
		}

		float dx = c2.x - c1.x;
		float dy = c2.y - c1.y;

		float overlapLength = pow(r2 - r1, 2) - (pow(dx, 2) + pow(dy, 2));

		if (overlapLength <= 0) {
			return glm::vec2(0.0f, 0.0f);
		}

		overlapLength = sqrt(overlapLength);
		double theta = atan(dy / dx) + ((dx < 0) ? PI : 0);

		glm::vec2 overlap = glm::vec2(overlapLength * cos(theta), overlapLength * sin(theta));

#ifdef _DEBUG
		Renderer2D::drawLine(c1, glm::vec2(c1.x + overlap.x, c1.y + overlap.y), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
#endif

		return overlap;
	}


	glm::vec2 CollisionSystem::getOverlap(Hitbox2D a, Hitbox2D b) {

		//Special case where both colliders are circles
		if (a.getVertexCount() == 1 && b.getVertexCount() == 1) {
			return getOverlap(a.getPosition(), a.getRadius(), b.getPosition(), b.getRadius());
		}

		const glm::vec2* aVertices = a.getVertices();
		const glm::vec2* bVertices = b.getVertices();

#ifdef _DEBUG
		//aVertices = getOutline(a);
		//bVertices = getOutline(b);
#endif

		glm::vec2 minimumDisplacement = {0.0f, 0.0f};

		//The edge count for a polygon = number of vertices, for collision detection purposes, circles are treated as having edges normal to all the edges of the collider
		uint16_t aEdgeCount = a.getVertexCount() > 1 ? a.getVertexCount() : b.getVertexCount();
		uint16_t bEdgeCount = b.getVertexCount() > 1 ? b.getVertexCount() : aEdgeCount;

		const glm::vec4* aEdges = a.getVertexCount() > 1 ? getEdges(aVertices, aEdgeCount) : getEdges(a.getPosition(), bVertices, aEdgeCount);
		const glm::vec4* bEdges = b.getVertexCount() > 1 ? getEdges(bVertices, bEdgeCount) : getEdges(b.getPosition(), aVertices, bEdgeCount);

		uint16_t uniqueAxisCount;

		const glm::vec4* uniqueAxes = getUniqueAxes(bEdges, bEdgeCount, aEdges, aEdgeCount, uniqueAxisCount);

		bool noCollision = false;

		for (uint16_t i = 0; i < uniqueAxisCount; i++) {

			glm::vec4 aProj = a.getVertexCount() > 1 ? getShadow(uniqueAxes[i], a.getVertices(), aEdgeCount) : getShadow(uniqueAxes[i], a.getPosition(), a.getRadius());
			glm::vec4 bProj = b.getVertexCount() > 1 ? getShadow(uniqueAxes[i], b.getVertices(), bEdgeCount) : getShadow(uniqueAxes[i], b.getPosition(), b.getRadius());

			glm::vec2 overlap = getOverlap(aProj, bProj);
			
			if (overlap.x != 0 || overlap.y != 0) {

				//If minimumDisplacement is undeclared or greater than the new overlap
				if ((minimumDisplacement.x == 0 && minimumDisplacement.y == 0) || glm::length(overlap) < glm::length(minimumDisplacement)) {
					minimumDisplacement = overlap;
				}
			}else {
#ifdef _DEBUG
				noCollision = true;
#else
				return glm::vec2(0.0f, 0.0f);
#endif
			}
			
		}
		
		return noCollision ? glm::vec2(0.0f) : minimumDisplacement;
	}

	glm::vec2 CollisionSystem::getOverlap(const glm::vec4& aProj, const glm::vec4& bProj) {

		glm::vec2 direction = { aProj.z - aProj.x, aProj.w - aProj.y };

		float k1 = direction.x != 0 ? (bProj.x - aProj.x) / direction.x : (bProj.y - aProj.y) / direction.y;
		float k2 = direction.x != 0 ? (bProj.z - aProj.x) / direction.x : (bProj.w - aProj.y) / direction.y;

		glm::vec4 proj = { bProj.x, bProj.y, bProj.z, bProj.w };

		bool flip = false;

		if (flip = k1 > k2) {
			float temp = k1;
			k1 = k2;
			k2 = temp;
		}

		glm::vec2 tail(0.0f);
		glm::vec2 head(0.0f);

		if (k1 < 0 && k2 > 1) {//aProj enclosed
			
			if (abs(k1) + 1 < k2) {//1->k1 is the shorter distance
				tail = {aProj.z, aProj.w};
				head = flip ? glm::vec2(bProj.z, bProj.w) : glm::vec2(bProj.x, bProj.y);
			} else {//0->k2 is the shorter distance
				tail = { aProj.x, aProj.y };
				head = !flip ? glm::vec2(bProj.z, bProj.w) : glm::vec2(bProj.x, bProj.y);
			}

		} else if (k1 >= 0 && k1 <= 1 && k2 <= 1) {//bProj enclosed

			if (1 - k1 < k2) {//k1->1 is the shorter distance
				tail = flip ? glm::vec2(bProj.z, bProj.w) : glm::vec2(bProj.x, bProj.y);
				head = { aProj.z, aProj.w };
			} else {//k2-<0 is the shorter distance
				tail = !flip ? glm::vec2(bProj.z, bProj.w) : glm::vec2(bProj.x, bProj.y);
				head = { aProj.x, aProj.y };
			}

		} else if (k1 >= 0 && k1 <= 1 && k2 > 1) {//a->bProj overlap
			tail = flip ? glm::vec2(bProj.z, bProj.w) : glm::vec2(bProj.x, bProj.y);
			head = {aProj.z, aProj.w};
		} else if (k1 <= 0 && k2 >= 0 && k2 <= 1) {//b->aProj overlap
			tail = flip ? glm::vec2(bProj.x, bProj.y) : glm::vec2(bProj.z, bProj.w);
			head = { aProj.x, aProj.y};
		}

		glm::vec4 overlap = {tail, head};

#ifdef _DEBUG
		glm::vec2 normal = { overlap.z - overlap.x, overlap.w - overlap.y };
		normal = { -normal.y / glm::length(normal) / 10, normal.x / glm::length(normal) / 10 };

		Renderer2D::drawLine(glm::vec2(overlap.x + normal.x, overlap.y + normal.y), glm::vec2(overlap.z + normal.x, overlap.w + normal.y), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
#endif

		return glm::vec2(overlap.z - overlap.x, overlap.w - overlap.y);
	}

	void movement(entt::registry& registry, Timestep ts) {
		auto view = registry.view<TransformComponent, VelocityComponent>();
		
		for (const entt::entity e : view) {

			glm::vec3& velocity = view.get<VelocityComponent>(e);

			if (velocity.x != 0 || velocity.y != 0 || velocity.z != 0) {
				glm::vec3 displacement = { velocity.x * ts, velocity.y * ts, velocity.z * ts };

				view.get<TransformComponent>(e).offset(displacement);
			}
		}
	}

	void render(entt::registry& registry) {
		auto view = registry.view<TransformComponent, Hitbox2DComponent>();

		for (const entt::entity e : view) {
			glm::mat4& transform = view.get<TransformComponent>(e).transform;
			Hitbox2DComponent& hitbox = view.get<Hitbox2DComponent>(e);
			hitbox.applyNewTransform(transform);
			hitbox.render();
			//Renderer2D::drawLines(hitbox.getVertices(), hitbox.getVertexCount(), hitbox.getColor());
		}
	}
}