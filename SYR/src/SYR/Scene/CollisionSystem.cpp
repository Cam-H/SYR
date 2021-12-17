#include "syrpch.h"
#include "CollisionSystem.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <glm/gtx/projection.hpp>

#include <glm/gtx/matrix_decompose.hpp>

#include "SYR/Renderer/Renderer2D.h"
#include "SYR/Renderer/Renderer3D.h"


#define PI 3.14159265358979323846

namespace SYR {

	static void getBorders(glm::vec3* vertices, uint16_t* vertexCount) {

		glm::vec3 temp;
		glm::vec3 referenceLine = { 0, 1 , 0 };//Use as the base line for dot product calculations to find exterior edges. Initially vertical to assist in getting first edgeTODO
		uint16_t index = 1;

		//Find an appropriate line to start connecting points (rightmost line). Extreme points necessarily won't be subsumed when creating a convex polygon
		uint16_t extremeVertexIndex = 0;
		for (uint16_t i = 1; i < *vertexCount; i++) {
			if (vertices[extremeVertexIndex].x < vertices[i].x) {
				extremeVertexIndex = i;
			}
		}
		//Swap the starting vertex with the first element in the array
		temp = vertices[0];
		vertices[0] = vertices[extremeVertexIndex]; vertices[extremeVertexIndex] = temp;

		//Order vertices such that a series of lines running from point to subsequent point encloses the maximum amount of space
		for (uint16_t i = 0; i < *vertexCount - 1; i++) {
			index = i + 1;

			glm::vec3 base = vertices[index] - vertices[i];
			float baseLength = glm::length(base);
			base = base / baseLength;//Normalize base

			for (uint16_t j = 0; j < *vertexCount; j++) {//Start from 0 rather than i to allow interior vertices to be cut if necessary
				if (i == j) continue;

				glm::vec3 test = vertices[j] - vertices[i];
				float testLength = glm::length(test);
				test = test / testLength;

				float delta = glm::dot(referenceLine, base) - glm::dot(referenceLine, test);
				if (delta > 0 || (delta > -0.0001f && testLength > baseLength)) {//Swap to new point if the internal angle is greater, or if the distance is longer (if both are collinear)
					index = j;

					base = test;
					baseLength = testLength;
				}
			}

			if (index <= i) {
				*vertexCount = i + 1;
				break;
			}
			else {
				temp = vertices[index];
				vertices[index] = vertices[i + 1];
				vertices[i + 1] = temp;
			}

			referenceLine = vertices[i] - vertices[i + 1];
		}

	}


	bool CollisionSystem::checkCollision(Collider a, glm::mat4& aTransform, Collider b, glm::mat4& bTransform) {
		glm::vec3* vertices = a.m_ColliderPhysicsData.vertices;
		uint16_t vertexCount = a.m_ColliderPhysicsData.vertexCount;

		glm::vec3* normals = a.m_ColliderPhysicsData.normals;
		uint16_t normalCount = a.m_ColliderPhysicsData.normalCount;

		glm::vec3* projectedVertices = new glm::vec3[vertexCount];
		for (uint32_t i = 0; i < vertexCount; i++) {
			glm::vec4 vertex = { vertices[i].x, vertices[i].y, vertices[i].z, 1.0f };
			vertex = aTransform * vertex;

			glm::vec3 plane = { 0, 1, -5 };
			float t = normals[0].x * plane.x - normals[0].x * vertex.x + normals[0].y * plane.y - normals[0].y * vertex.y + normals[0].z * plane.z - normals[0].z * vertex.z;
			t /= pow(glm::length(normals[0]), 2);

			Renderer3D::drawSphere(vertex, 0.05f, { 1, 0, 0, 1 }, 4, 4);
			//projectedVertices[i] = glm::proj(vertices[i], normals[0]);
			projectedVertices[i] = { vertex.x + t * normals[0].x, vertex.y + t * normals[0].y, vertex.z + t * normals[0].z };
			//Renderer3D::drawSphere(projectedVertices[i], 0.05f, { 1, 0, 1, 1 }, 4, 4);
			Renderer3D::drawLine({ 0, 0, 0 }, normals[0], { 0, 0, 1, 1 });
		}

		getBorders(projectedVertices, &vertexCount);

		for (uint32_t i = 0; i < vertexCount; i++) {
			Renderer3D::drawSphere(projectedVertices[i], 0.02f, { 0, 1, 1, 1 }, 4, 4);
		}
		Renderer3D::drawLines(projectedVertices, vertexCount, { 0, 1, 0, 1 });
	}


	bool CollisionSystem::checkCollision(const glm::vec2* vertices, uint16_t vertexCount, const glm::vec2& center, float radius) {

		glm::vec2 minimumDisplacement = { 0.0f, 0.0f };

		const glm::vec4* aEdges = getEdges(vertices, vertexCount);//Get all of the edges of the polygon
		const glm::vec4* bEdges = getEdges(center, vertices, vertexCount);//Get all important edges of the circle (those normal to polygon edges)

		uint16_t uniqueAxisCount;

		const glm::vec4* uniqueAxes = getUniqueAxes(bEdges, vertexCount, aEdges, vertexCount, uniqueAxisCount);

		for (uint16_t i = 0; i < uniqueAxisCount; i++) {

			//Get the shadows of the polygon and circle along the axis
			glm::vec4 aProj = getShadow(uniqueAxes[i], vertices, vertexCount);
			glm::vec4 bProj = getShadow(uniqueAxes[i], center, radius);

			glm::vec2 overlap = getOverlap(aProj, bProj);

			//If there was no overlap along the given axis
			if (overlap.x == 0 && overlap.y == 0) {
				return false;
			}
		}

		return true;//Condition reached only when there is overlap along every axis
	}

	float CollisionSystem::getCollision(const glm::vec2* vertices, uint16_t vertexCount, const glm::vec4& line) {

		if (glm::length(line) == 0) {
			return -1;
		}

		float tMin = getCollision(line, { vertices[0].x, vertices[0].y, vertices[vertexCount - 1].x, vertices[vertexCount - 1].y });
		for (uint16_t i = 0; i < vertexCount - 1; i++) {
			float temp = getCollision(line, { vertices[i].x, vertices[i].y, vertices[i + 1].x, vertices[i + 1].y });

			if (temp > 0 && (tMin < 0 || temp < tMin)) {
				tMin = temp;
			}
		}

		return tMin;
	}

	bool CollisionSystem::checkCollision(const glm::vec2* vertices, uint16_t vertexCount, const glm::vec4& line) {

		if (glm::length(line) == 0) {
			return false;
		}


		float tMin = getCollision(line, { vertices[0].x, vertices[0].y, vertices[vertexCount - 1].x, vertices[vertexCount - 1].y });
		for (uint16_t i = 0; i < vertexCount - 1; i++) {
			float temp = getCollision(line, { vertices[i].x, vertices[i].y, vertices[i + 1].x, vertices[i + 1].y });

			if (temp != -1) {
				return true;
			}
		}

		return tMin >= 0;
	}

	float CollisionSystem::getCollision(const glm::vec4& tLine, const glm::vec4& uLine) {

		if (glm::length(tLine) == 0 || glm::length(uLine) == 0) {
			return false;
		}

		//Renderer2D::drawLine({ uLine.x, uLine.y }, { uLine.z, uLine.w }, { 0.0f, 0.0f, 0.0f, 1.0f });

		glm::vec2 xt = { tLine.z - tLine.x, tLine.x };
		glm::vec2 yt = { tLine.w - tLine.y, tLine.y };

		glm::vec2 xu = { uLine.z - uLine.x , uLine.x };
		glm::vec2 yu = { uLine.w - uLine.y , uLine.y };

		float u = -1;
		float t = -1;

		if (yt.x == 0) {
			if (yu.x == 0) {//A parallel lines case (horizontal)
				if (yt.y == yu.y) {//Check if the lines are collinear
					u = 0;
					t = (xu.y - xt.y + xu.x * u) / xt.x;
				}
			}
			else {
				u = (yt.y - yu.y) / yu.x;
				t = (xu.y - xt.y + xu.x * u) / xt.x;
			}

		} else {//Standard case
			if (yu.x * xt.x / yt.x - xu.x == 0) {//Check for special case
				if (xt.x == 0 && xu.x == 0) {//Both lines are parallel to the vertical
					if (xt.y == xu.y) {//Check if lines are colliner
						u = 0;
						t = (yu.y - yt.y + yu.x * u) / yt.x;
					}
				} else {
					SYR_CORE_ERROR("Unhandled line collision case!");
				}
			}
			else {
				u = (xu.y - xt.y - xt.x * (yu.y - yt.y) / yt.x) / (yu.x * xt.x / yt.x - xu.x);
				t = (yu.y - yt.y + yu.x * u) / yt.x;
			}
		}

		if (0 <= u && u <= 1 && t > 0) {
			//Renderer2D::drawCircle({ xu.x * u + xu.y, yu.x * u + yu.y }, 0.01f, { 1.0f, 0.0f, 0.0f, 1.0f });
			return t;
		}

		return -1;
	}

	bool CollisionSystem::checkCollision(const glm::vec4& tLine, const glm::vec4& uLine) {
		float t = getCollision(tLine, uLine);
		
		return 0 <= t && t<= 1;
	}

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
		//Renderer2D::drawLine(glm::vec2(shadow.x, shadow.y), glm::vec2(shadow.z, shadow.w), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
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
		//Renderer2D::drawLine(glm::vec2(shadow.x, shadow.y), glm::vec2(shadow.z, shadow.w), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
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

		//Renderer2D::drawLine(glm::vec2(overlap.x + normal.x, overlap.y + normal.y), glm::vec2(overlap.z + normal.x, overlap.w + normal.y), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
#endif

		return glm::vec2(overlap.z - overlap.x, overlap.w - overlap.y);
	}

	void movement(entt::registry& registry, Timestep ts) {
		auto view = registry.view<TransformComponent, VelocityComponent>();
		
		for (const entt::entity e : view) {

			VelocityComponent& velocity = view.get<VelocityComponent>(e);

			view.get<TransformComponent>(e).offset(velocity.velocity * ts.getSeconds());
			view.get<TransformComponent>(e).rotate(velocity.rotation.x * ts.getSeconds(), glm::vec3(1, 0, 0));
			view.get<TransformComponent>(e).rotate(velocity.rotation.y * ts.getSeconds(), glm::vec3(0, 1, 0));
			view.get<TransformComponent>(e).rotate(velocity.rotation.z * ts.getSeconds(), glm::vec3(0, 0, 1));

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