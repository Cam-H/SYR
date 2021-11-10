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

		//Lines

		static void setLineWidth(float lineWidth);

		static void drawLines(const glm::vec3 vertices[], uint32_t vertexCount);
		static void drawLines(const glm::vec3 vertices[], uint32_t vertexCount, const glm::vec4& color);
		static void drawLines(const glm::vec3 vertices[], uint32_t vertexCount, const glm::mat4& transform, const glm::vec4& color);

		//Spheres
		static void drawSphere(const glm::vec3& position, float radius);
		static void drawSphere(const glm::vec3& position, float radius, const glm::vec4& color, int loopVertexCount = 12);

		static void drawICOSphere(const glm::vec3& position, float radius);
		static void drawICOSphere(const glm::vec3& position, float radius, const glm::vec4& color, int loopVertexCount = 12);

	private:
		static void startNewBatch();

		static void prepareForBatch();
	private:
		inline static float s_LineWidth = 2.0f;
	};

}