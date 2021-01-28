#pragma once

#include <entt.hpp>
#include <glm/glm.hpp>

#include "Components.h"

#include "SYR/Core/Timestep.h"

#include "Hitbox2D.h"

namespace SYR {

	void checkCollisions(entt::registry& registry);

	void handleCollisions(entt::registry& registry);

	class CollisionSystem {
	public:
	private:
		static bool areParallel(const glm::vec4& a, const glm::vec4& b) { return abs((a.z - a.x) * (b.w - b.y) - (b.z - b.x) * (a.w - a.y)) < 0.001f; }

		static const glm::vec4* getEdges(Hitbox2D hitbox);
		static const glm::vec4* getEdges(const glm::vec2* vertices, uint16_t vertexCount);
		static const glm::vec4* getEdges(glm::vec2 center, const glm::vec2* vertices, uint16_t vertexCount);

		static const glm::vec4* getUniqueAxes(const glm::vec4* aAxes, uint16_t aCount, const glm::vec4* bAxes, uint16_t bCount, uint16_t& uniqueAxisCount);

		static glm::vec4 getShadow(glm::vec4 surface, const glm::vec2* vertices, uint16_t vertexCount);
		static glm::vec4 getShadow(glm::vec4 surface, const glm::vec2 position, float radius);

		static const glm::vec2* getOutline(Hitbox2D hitbox);

	public:
		static glm::vec2 getOverlap(Hitbox2D a, Hitbox2D b);
		static glm::vec2 getOverlap(const glm::vec2& c1, float r1, const glm::vec2& c2, float r2);

		//static glm::vec2 getOverlap(Hitbox2DComponent a, Hitbox2DComponent b);
		static glm::vec2 getOverlap(const glm::vec4& aProj, const glm::vec4& bProj);
	};	

	void movement(entt::registry& registry, Timestep ts);
	void render(entt::registry& registry);

}
