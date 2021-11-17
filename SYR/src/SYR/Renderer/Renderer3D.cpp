#include "syrpch.h"
#include "Renderer3D.h"

#include "RenderCommand.h"

#include "VertexArray.h"
#include "Shader.h"

#include <glm/gtc/matrix_transform.hpp>


#define PI 3.14159265358979323846

namespace SYR {

	struct LineVertex {
		glm::vec3 position;
		glm::vec4 color;
	};

	struct Renderer3DData {

		//LINES
		static const uint32_t maxLines = 5000;
		static const uint32_t maxLineVertices = maxLines * 2;
		static const uint32_t maxLineIndices = maxLines * 2;

		uint32_t lineVertexCount = 0;
		uint32_t lineIndexCount = 0;
		uint32_t* lineIndices = nullptr;
		LineVertex* lineVertexBufferBase = nullptr;
		LineVertex* lineVertexBufferPtr = nullptr;

		Ref<VertexArray> lineVertexArray;
		Ref<VertexBuffer> lineVertexBuffer;
		Ref<IndexBuffer> lineIndexBuffer;

		//SHADERS
		Ref<Shader> lineShader;
	};

	static Renderer3DData* s_Data;

	void Renderer3D::init() {

		s_Data = new Renderer3DData();

		//Line Vertex Array setup

		s_Data->lineVertexArray = VertexArray::create();

		s_Data->lineVertexBuffer = VertexBuffer::create(s_Data->maxLineVertices * sizeof(LineVertex));

		s_Data->lineVertexBuffer->setLayout({ {ShaderDataType::FLOAT3, "position"},
						{ShaderDataType::FLOAT4, "color"} });

		s_Data->lineVertexArray->addVertexBuffer(s_Data->lineVertexBuffer);

		s_Data->lineVertexBufferBase = new LineVertex[s_Data->maxLineVertices];
		s_Data->lineVertexBufferPtr = s_Data->lineVertexBufferBase;

		s_Data->lineIndices = new uint32_t[s_Data->maxLineIndices];

		for (int i = 0; i < s_Data->maxLineIndices; i++) {
			s_Data->lineIndices[i] = i;
		}

		s_Data->lineIndexBuffer = IndexBuffer::create(s_Data->maxLineIndices);
		s_Data->lineVertexArray->setIndexBuffer(s_Data->lineIndexBuffer);
		
		s_Data->lineShader = Shader::create("assets/shaders/Line.glsl");
	}

	void Renderer3D::shutdown() {
		delete s_Data;
	}

	void Renderer3D::beginScene(const Camera& camera) {
		s_Data->lineShader->bind();
		s_Data->lineShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());

		prepareForBatch();
	}

	void Renderer3D::endScene() {

		s_Data->lineVertexArray->bind();
		s_Data->lineVertexBuffer->setData(s_Data->lineVertexBufferBase, sizeof(LineVertex) * s_Data->lineVertexCount);
		s_Data->lineIndexBuffer->setData(s_Data->lineIndices, sizeof(uint32_t) * s_Data->lineIndexCount);

		flush();
	}

	void Renderer3D::flush() {

		if (s_Data->lineIndexCount > 0) {
			s_Data->lineShader->bind();
			RenderCommand::drawLines(s_Data->lineVertexArray, s_LineWidth, s_Data->lineIndexCount);
		}

	}

	void Renderer3D::startNewBatch() {
		endScene();

		prepareForBatch();
	}

	void Renderer3D::prepareForBatch() {
		s_Data->lineVertexCount = 0;
		s_Data->lineIndexCount = 0;
		s_Data->lineVertexBufferPtr = s_Data->lineVertexBufferBase;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DRAW LINE(S) ////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Renderer3D::setLineWidth(float lineWidth) {
		s_LineWidth = lineWidth;
	}

	void Renderer3D::drawLine(const glm::vec3& a, const glm::vec3& b, const glm::vec4& color, const glm::mat4& transform) {
		SYR_CORE_PROFILE();

		drawLines(new glm::vec3[2]{ a, b }, 2, color, transform);
	}

	void Renderer3D::drawLines(const glm::vec3 vertices[], uint32_t vertexCount, const glm::vec4& color, const glm::mat4& transform) {
		SYR_CORE_PROFILE();

		if (s_Data->lineIndexCount >= Renderer3DData::maxLineIndices) {
			startNewBatch();
		}

		int closeIndex = s_Data->lineVertexCount;

		for (uint32_t i = 0; i < vertexCount; i++) {
			glm::vec4 vertex = { vertices[i].x, vertices[i].y, vertices[i].z, 1.0f };

			s_Data->lineVertexBufferPtr->position = transform * vertex;
			s_Data->lineVertexBufferPtr->color = color;
			s_Data->lineVertexBufferPtr++;

			s_Data->lineIndices[s_Data->lineIndexCount] = s_Data->lineVertexCount;
			s_Data->lineIndices[s_Data->lineIndexCount + 1] = s_Data->lineVertexCount + 1;

			s_Data->lineVertexCount++;
			s_Data->lineIndexCount += 2;
		}

		//Overwrite last vertex index to point it to the first index to close the loop
		s_Data->lineIndices[s_Data->lineIndexCount - 1] = closeIndex;
	}

	void Renderer3D::drawSphere(const glm::vec3& position, float radius, const glm::vec4& color, uint16_t sectorCount, uint16_t stackCount) {
		SYR_CORE_PROFILE();

		double deltaX = PI * 2 / sectorCount;
		double deltaY = PI / (stackCount + 1);

		int closeIndex = s_Data->lineVertexCount;

		glm::vec4 vertexColor = color;

		//TODO limit
		if (s_Data->lineIndexCount + 1 >= Renderer3DData::maxLineIndices) {
			startNewBatch();
		}

		//Generate vertices/indices for HORIZONTAL rings
		for (double phi = -PI / 2 + deltaY; phi <= PI / 2; phi += deltaY) {
			float ringRadius = radius * cos(phi);

			for (double theta = 0; theta < PI * 2; theta += deltaX) {
				glm::vec3 vertexPosition = { position.x + ringRadius * cos(theta), position.y + radius * sin(phi), position.z + ringRadius * sin(theta) };

				s_Data->lineVertexBufferPtr->position = vertexPosition;
				s_Data->lineVertexBufferPtr->color = vertexColor;
				s_Data->lineVertexBufferPtr++;

				s_Data->lineIndices[s_Data->lineIndexCount] = s_Data->lineVertexCount;
				s_Data->lineIndices[s_Data->lineIndexCount + 1] = s_Data->lineVertexCount + 1;

				s_Data->lineVertexCount++;
				s_Data->lineIndexCount += 2;
			}

			s_Data->lineIndices[s_Data->lineIndexCount - 1] = s_Data->lineVertexCount - (PI * 2) / deltaX;

		}
		
		//Generate vertices/indices for vertical loops
		bool switcher = 0;
		for (double theta = 0; theta <= PI * 2; theta += deltaX) {
			for (double phi = -PI / 2; phi < PI / 2; phi += deltaY) {
				glm::vec3 vertexPosition = { position.x + radius * cos(theta) * cos(phi), position.y + radius * sin(phi + switcher * PI), position.z + radius * sin(theta) * cos(phi) };

				s_Data->lineVertexBufferPtr->position = vertexPosition;
				s_Data->lineVertexBufferPtr->color = vertexColor;
				s_Data->lineVertexBufferPtr++;

				s_Data->lineIndices[s_Data->lineIndexCount] = s_Data->lineVertexCount;
				s_Data->lineIndices[s_Data->lineIndexCount + 1] = s_Data->lineVertexCount + 1;

				s_Data->lineVertexCount++;
				s_Data->lineIndexCount += 2;
			}

			switcher = !switcher;
		}

		s_Data->lineIndexCount -= 2;
	}

	void Renderer3D::drawICOSphere(const glm::vec3& position, float radius, const glm::vec4& color, int loopVertexCount) {
		//Sphere Generation Reference: http://www.songho.ca/opengl/gl_sphere.html
	}
}