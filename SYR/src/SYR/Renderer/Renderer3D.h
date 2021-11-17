#pragma once

#include "PerspectiveCamera.h"

namespace SYR {

	class Renderer3D {
	public:

		static void init();
		static void shutdown();

		static void beginScene(const Camera& camera);
		static void endScene();

		static void flush();

		static void setLineWidth(float lineWidth);

		//Lines
		static void drawLine(const glm::vec3& a, const glm::vec3& b, const glm::vec4& color = { 1, 1, 1, 1 }, const glm::mat4& transform = glm::mat4(1.0f));
		static void drawLines(const glm::vec3 vertices[], uint32_t vertexCount, const glm::vec4& color = { 1, 1, 1, 1 }, const glm::mat4& transform = glm::mat4(1.0f));

		//Spheres
		static void drawSphere(const glm::vec3& position, float radius, const glm::vec4& color = { 1, 1, 1, 1 }, uint16_t sectorCount = 12, uint16_t stackCount = 12);
		static void drawICOSphere(const glm::vec3& position, float radius, const glm::vec4& color = { 1, 1, 1, 1 }, int loopVertexCount = 12);

		//Also Cube Sphere

	private:
		static void startNewBatch();

		static void prepareForBatch();
	private:
		inline static float s_LineWidth = 2.0f;
	};

}