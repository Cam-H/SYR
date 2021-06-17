#include "syrpch.h"
#include "Renderer2D.h"

#include <iostream>
#include <fstream>

#include "RenderCommand.h"

#include "VertexArray.h"
#include "Shader.h"

#include <glm/gtc/matrix_transform.hpp>


#define PI 3.14159265358979323846

namespace SYR {

	struct QuadVertex {
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texcoord;
		float texIndex;
	};

	struct LineVertex {
		glm::vec3 position;
		glm::vec4 color;
	};

	struct Renderer2DData {

		//QUADS
		static const uint32_t maxQuads = 3000;
		static const uint32_t maxQuadVertices = maxQuads * 4;
		static const uint32_t maxQuadIndices = maxQuads * 6;

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;

		glm::vec4 quadVertexPositions[4];

		//LINES
		static const uint32_t maxLines = 10000;
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
		Ref<Shader> basicShader;
		Ref<Shader> lineShader;

		//TEXTURES & COLOR
		static const uint32_t maxTextureSlots = 32;//TODO determine for system (render capabilities)

		std::array<Ref<Texture2D>, maxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1;//0 = white texture

		Ref<Texture2D> whiteTexture;
		glm::vec4 tintColor;

		//RENDERER STATISTICS & DEBUG
		Renderer2D::Statistics stats;
	};

	static Renderer2DData* s_Data;

	void Renderer2D::init() {

		s_Data = new Renderer2DData();

		//Quad Vertex Array setup
		
		s_Data->quadVertexArray = VertexArray::create();

		s_Data->quadVertexBuffer = VertexBuffer::create(s_Data->maxQuadVertices * sizeof(QuadVertex));

		s_Data->quadVertexBuffer->setLayout({ {ShaderDataType::FLOAT3, "position"},
						{ShaderDataType::FLOAT4, "color"},
						{ShaderDataType::FLOAT2, "texcoords"},
						{ShaderDataType::FLOAT, "texIndex"}});

		s_Data->quadVertexArray->addVertexBuffer(s_Data->quadVertexBuffer);

		s_Data->quadVertexBufferBase = new QuadVertex[s_Data->maxQuadVertices];
		s_Data->quadVertexBufferPtr = s_Data->quadVertexBufferBase;

		uint32_t* quadIndices = new uint32_t[s_Data->maxQuadIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data->maxQuadIndices; i += 6) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIndexBuffer = IndexBuffer::create(quadIndices, s_Data->maxQuadIndices);
		s_Data->quadVertexArray->setIndexBuffer(quadIndexBuffer);

		delete[] quadIndices;

		/*
		s_Data->quadVertexPositions[1] = { 1.0f,  1.0f, 0.0f, 1.0f };
		s_Data->quadVertexPositions[2] = { 1.0f, -1.0f, 0.0f, 1.0f };
		s_Data->quadVertexPositions[3] = { -1.0f, -1.0f, 0.0f, 1.0f };
		s_Data->quadVertexPositions[0] = { -1.0f,  1.0f, 0.0f, 1.0f };
		*/

		s_Data->quadVertexPositions[1] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data->quadVertexPositions[2] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->quadVertexPositions[3] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->quadVertexPositions[0] = { -0.5f,  0.5f, 0.0f, 1.0f };
		
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
		
		//Texture setup

		int32_t samplers[s_Data->maxTextureSlots];
		for (uint32_t i = 0; i < s_Data->maxTextureSlots; i++) {
			samplers[i] = i;
		}

		s_Data->whiteTexture = Texture2D::create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

		s_Data->tintColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		s_Data->basicShader = Shader::create("assets/shaders/Basic.glsl");

		s_Data->basicShader->bind();
		s_Data->basicShader->setIntArray("u_Textures", samplers, s_Data->maxTextureSlots);

		s_Data->lineShader = Shader::create("assets/shaders/Line.glsl");

		s_Data->textureSlots[0] = s_Data->whiteTexture;
	}

	void Renderer2D::shutdown() {
		delete s_Data;
	}

	void Renderer2D::beginScene(const Camera& camera) {
		s_Data->basicShader->bind();
		s_Data->basicShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());

		s_Data->lineShader->bind();
		s_Data->lineShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());

		prepareForBatch();

		resetStats();
	}

	void Renderer2D::endScene() {

		uint32_t dataSize = (uint8_t*)s_Data->quadVertexBufferPtr - (uint8_t*)s_Data->quadVertexBufferBase;
		s_Data->quadVertexBuffer->setData(s_Data->quadVertexBufferBase, dataSize);

		s_Data->lineVertexArray->bind();
		s_Data->lineVertexBuffer->setData(s_Data->lineVertexBufferBase, sizeof(LineVertex) * s_Data->lineVertexCount);
		s_Data->lineIndexBuffer->setData(s_Data->lineIndices, sizeof(uint32_t) * s_Data->lineIndexCount);

		flush();
	}

	void Renderer2D::flush() {

		s_Data->basicShader->bind();
		for (uint32_t i = 0; i < s_Data->textureSlotIndex; i++) {
			s_Data->textureSlots[i]->bind(i);
		}

		RenderCommand::drawIndexed(s_Data->quadVertexArray, s_Data->quadIndexCount);
		s_Data->stats.drawCalls++;


		s_Data->lineShader->bind();
		RenderCommand::drawLines(s_Data->lineVertexArray, 2.0f, s_Data->lineIndexCount);
	}

	void Renderer2D::startNewBatch() {
		endScene();

		prepareForBatch();
	}

	void Renderer2D::prepareForBatch() {
		s_Data->quadIndexCount = 0;
		s_Data->quadVertexBufferPtr = s_Data->quadVertexBufferBase;

		s_Data->lineVertexCount = 0;
		s_Data->lineIndexCount = 0;
		s_Data->lineVertexBufferPtr = s_Data->lineVertexBufferBase;

		s_Data->textureSlotIndex = 1;
	}

	void Renderer2D::setTintColor(glm::vec4& tintColor) {
		s_Data->tintColor = tintColor;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DRAW LINE(S) ////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Renderer2D::drawLine(const glm::vec2& start, const glm::vec2& end) {
		SYR_CORE_PROFILE();

		drawLine(start, end, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	void Renderer2D::drawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color) {
		SYR_CORE_PROFILE();

		drawLine(glm::vec3(start.x, start.y, 1.0f), glm::vec3(end.x, end.y, 1.0f), color);
	}

	void Renderer2D::drawLine(const glm::vec3& start, const glm::vec3& end) {
		SYR_CORE_PROFILE();

		drawLine(start, end, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	void Renderer2D::drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color) {
		SYR_CORE_PROFILE();

		if (s_Data->lineIndexCount >= Renderer2DData::maxLineIndices) {
			startNewBatch();
		}

		s_Data->lineVertexBufferPtr->position = { start.x, start.y, start.z };
		s_Data->lineVertexBufferPtr->color = color * s_Data->tintColor;
		s_Data->lineVertexBufferPtr++;

		s_Data->lineVertexBufferPtr->position = { end.x, end.y, end.z };
		s_Data->lineVertexBufferPtr->color = color * s_Data->tintColor;
		s_Data->lineVertexBufferPtr++;

		s_Data->lineIndices[s_Data->lineIndexCount] = s_Data->lineVertexCount;
		s_Data->lineIndices[s_Data->lineIndexCount + 1] = s_Data->lineVertexCount + 1;

		s_Data->lineVertexCount += 2;
		s_Data->lineIndexCount += 2;
	}

	void Renderer2D::drawLines(const glm::vec2 vertices[], uint32_t vertexCount) {
		SYR_CORE_PROFILE();

		drawLines(vertices, vertexCount, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	void Renderer2D::drawLines(const glm::vec2 vertices[], uint32_t vertexCount, const glm::vec4& color) {
		SYR_CORE_PROFILE();

		if (s_Data->lineIndexCount >= Renderer2DData::maxLineIndices) {
			startNewBatch();
		}

		int closeIndex = s_Data->lineVertexCount;

		for (uint32_t i = 0; i < vertexCount; i++) {
			s_Data->lineVertexBufferPtr->position = { vertices[i].x, vertices[i].y, 1.0f };
			s_Data->lineVertexBufferPtr->color = color * s_Data->tintColor;
			s_Data->lineVertexBufferPtr++;

			s_Data->lineIndices[s_Data->lineIndexCount] = s_Data->lineVertexCount;
			s_Data->lineIndices[s_Data->lineIndexCount + 1] = s_Data->lineVertexCount + 1;

			s_Data->lineVertexCount++;
			s_Data->lineIndexCount += 2;
		}

		//Overwrite last vertex index to point it to the first index to close the loop
		s_Data->lineIndices[s_Data->lineIndexCount - 1] = closeIndex;
	}

	void Renderer2D::drawLines(const glm::vec3 vertices[], uint32_t vertexCount) {
		SYR_CORE_PROFILE();

		drawLines(vertices, vertexCount, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	void Renderer2D::drawLines(const glm::vec3 vertices[], uint32_t vertexCount, const glm::vec4& color) {
		SYR_CORE_PROFILE();

		if (s_Data->lineIndexCount >= Renderer2DData::maxLineIndices) {
			startNewBatch();
		}

		int closeIndex = s_Data->lineVertexCount;

		for (uint32_t i = 0; i < vertexCount; i++) {

			s_Data->lineVertexBufferPtr->position = vertices[i];
			s_Data->lineVertexBufferPtr->color = color * s_Data->tintColor;
			s_Data->lineVertexBufferPtr++;

			s_Data->lineIndices[s_Data->lineIndexCount] = s_Data->lineVertexCount;
			s_Data->lineIndices[s_Data->lineIndexCount + 1] = s_Data->lineVertexCount + 1;

			s_Data->lineVertexCount++;
			s_Data->lineIndexCount += 2;
		}

		//Overwrite last vertex index to point it to the first index to close the loop
		s_Data->lineIndices[s_Data->lineIndexCount - 1] = closeIndex;
	}

	void Renderer2D::drawLines(const glm::vec2 vertices[], uint32_t vertexCount, const glm::mat4& transform, const glm::vec4& color) {
		SYR_CORE_PROFILE();

		glm::vec3* transformedVertices = new glm::vec3[vertexCount];

		for (uint32_t i = 0; i < vertexCount; i++) {
			glm::vec4 vertex = { vertices[i].x, vertices[i].y, 1.0f, 1.0f };
			
			transformedVertices[i] = transform * vertex;
		}

		drawLines(transformedVertices, vertexCount, color);
	}

	void Renderer2D::drawLines(const glm::vec3 vertices[], uint32_t vertexCount, const glm::mat4& transform, const glm::vec4& color) {
		SYR_CORE_PROFILE();

		glm::vec3* transformedVertices = new glm::vec3[vertexCount];

		for (uint32_t i = 0; i < vertexCount; i++) {
			glm::vec4 vertex = { vertices[i].x, vertices[i].y, vertices[i].z, 1.0f };

			transformedVertices[i] = transform * vertex;
		}

		drawLines(transformedVertices, vertexCount, color);
	}

	void Renderer2D::drawCircle(const glm::vec2& position, float radius) {
		drawCircle(position, radius, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	void Renderer2D::drawCircle(const glm::vec2& position, float radius, const glm::vec4& color, int vertexCount) {
		SYR_CORE_PROFILE();

		double delta = PI * 2 / vertexCount;

		if (s_Data->lineIndexCount + vertexCount >= Renderer2DData::maxLineIndices) {
			startNewBatch();
		}

		int closeIndex = s_Data->lineVertexCount;

		glm::vec4 vertexColor = color * s_Data->tintColor;

		for (double theta = 0; theta <= PI * 2; theta += delta) {

			glm::vec3 vertexPosition = { position.x + radius * cos(theta), position.y + radius * sin(theta), 1.0f };

			s_Data->lineVertexBufferPtr->position = vertexPosition;
			s_Data->lineVertexBufferPtr->color = vertexColor;
			s_Data->lineVertexBufferPtr++;

			s_Data->lineIndices[s_Data->lineIndexCount] = s_Data->lineVertexCount;
			s_Data->lineIndices[s_Data->lineIndexCount + 1] = s_Data->lineVertexCount + 1;

			s_Data->lineVertexCount++;
			s_Data->lineIndexCount += 2;
		}

		//Overwrite last vertex index to point it to the first index to close the loop
		s_Data->lineIndices[s_Data->lineIndexCount - 1] = closeIndex;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DRAW QUAD - NO ROTATION /////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Renderer2D::drawQuad(const glm::vec2& position, glm::vec2& size, const glm::vec4& color) {
		SYR_CORE_PROFILE();

		drawQuad({position.x, position.y, 0.0f}, size, color);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, glm::vec2& size, const glm::vec4& color) {
		SYR_CORE_PROFILE();

		drawQuad(position, size, color, s_Data->whiteTexture);
	}

	void Renderer2D::drawQuad(const glm::vec2& position, glm::vec2& size, const Ref<Texture2D>& texture) {
		SYR_CORE_PROFILE();

		drawQuad({ position.x, position.y, 0.0f }, size, texture);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, glm::vec2& size, const Ref<Texture2D>& texture) {
		SYR_CORE_PROFILE();

		drawQuad(position, size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), texture);
	}

	void Renderer2D::drawQuad(const glm::vec2& position, glm::vec2& size, const glm::vec4& color, const Ref<Texture2D>& texture) {
		SYR_CORE_PROFILE();

		drawQuad({ position.x, position.y, 0.0f }, size, color, texture);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, glm::vec2& size, const glm::vec4& color, const Ref<Texture2D>& texture) {
		SYR_CORE_PROFILE();
		
		glm::vec2 texcoords[] = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f}
		};

		drawQuad(position, size, color, texture, texcoords);
	}

	void Renderer2D::drawQuad(const glm::vec2& position, glm::vec2& size, const Ref<SubTexture2D>& subtexture) {
		SYR_CORE_PROFILE();

		drawQuad({ position.x, position.y, 0.0f }, size, subtexture);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, glm::vec2& size, const Ref<SubTexture2D>& subtexture) {
		SYR_CORE_PROFILE();

		drawQuad(position, size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), subtexture);
	}

	void Renderer2D::drawQuad(const glm::vec2& position, glm::vec2& size, const glm::vec4& color, const Ref<SubTexture2D>& subtexture) {
		SYR_CORE_PROFILE();

		drawQuad({ position.x, position.y, 0.0f }, size, color, subtexture);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, glm::vec2& size, const glm::vec4& color, const Ref<SubTexture2D>& subtexture) {
		SYR_CORE_PROFILE();

		drawQuad(position, size, color, subtexture->getTexture(), subtexture->getTexCoords());
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color, const Ref<Texture2D>& texture) {
		SYR_CORE_PROFILE();

		drawRotatedQuad(transform, color, texture);
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color, const Ref<SubTexture2D>& subtexture) {
		SYR_CORE_PROFILE();

		drawRotatedQuad(transform, color, subtexture);
	}

	void Renderer2D::drawQuad(const glm::vec2& position, glm::vec2& size, const glm::vec4& color, const Ref<Texture2D>& texture, const glm::vec2 texcoords[]) {
		SYR_CORE_PROFILE();

		drawQuad({ position.x, position.y, 0.0f }, size, color, texture, texcoords);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, glm::vec2& size, const glm::vec4& color, const Ref<Texture2D>& texture, const glm::vec2 texcoords[]) {

		if (s_Data->quadIndexCount >= Renderer2DData::maxQuadIndices) {
			startNewBatch();
		}

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data->textureSlotIndex; i++) {
			if (*s_Data->textureSlots[i].get() == *texture.get()) {
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f) {
			textureIndex = (float)s_Data->textureSlotIndex;
			s_Data->textureSlots[s_Data->textureSlotIndex] = texture;
			s_Data->textureSlotIndex++;
		}

		s_Data->quadVertexBufferPtr->position = position;
		s_Data->quadVertexBufferPtr->color = color * s_Data->tintColor;
		s_Data->quadVertexBufferPtr->texcoord = texcoords[0];
		s_Data->quadVertexBufferPtr->texIndex = textureIndex;
		s_Data->quadVertexBufferPtr++;

		s_Data->quadVertexBufferPtr->position = { position.x + size.x, position.y, position.z };
		s_Data->quadVertexBufferPtr->color = color * s_Data->tintColor;
		s_Data->quadVertexBufferPtr->texcoord = texcoords[1];
		s_Data->quadVertexBufferPtr->texIndex = textureIndex;
		s_Data->quadVertexBufferPtr++;

		s_Data->quadVertexBufferPtr->position = { position.x + size.x, position.y + size.y, position.z };
		s_Data->quadVertexBufferPtr->color = color * s_Data->tintColor;
		s_Data->quadVertexBufferPtr->texcoord = texcoords[2];
		s_Data->quadVertexBufferPtr->texIndex = textureIndex;
		s_Data->quadVertexBufferPtr++;

		s_Data->quadVertexBufferPtr->position = { position.x, position.y + size.y, position.z };
		s_Data->quadVertexBufferPtr->color = color * s_Data->tintColor;
		s_Data->quadVertexBufferPtr->texcoord = texcoords[3];
		s_Data->quadVertexBufferPtr->texIndex = textureIndex;
		s_Data->quadVertexBufferPtr++;

		s_Data->quadIndexCount += 6;


		s_Data->stats.quadCount++;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DRAW QUAD - ROTATION ////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Renderer2D::drawRotatedQuad(const glm::vec2& position, glm::vec2& size, float rotation, const glm::vec4& color) {
		drawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, glm::vec2& size, float rotation, const glm::vec4& color) {
		drawRotatedQuad(position, size, rotation, color, s_Data->whiteTexture);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec2& position, glm::vec2& size, float rotation, const Ref<Texture2D>& texture) {
		drawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, glm::vec2& size, float rotation, const Ref<Texture2D>& texture) {
		drawRotatedQuad(position, size, rotation, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), texture);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec2& position, glm::vec2& size, float rotation, const glm::vec4& color, const Ref<Texture2D>& texture) {
		drawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color, texture);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, glm::vec2& size, float rotation, const glm::vec4& color, const Ref<Texture2D>& texture) {

		glm::vec2 texcoords[] = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f}
		};

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
	
		drawRotatedQuad(transform, color, texture, texcoords);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec2& position, glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture) {
		drawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, subtexture);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture) {
		drawRotatedQuad(position, size, rotation, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), subtexture);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec2& position, glm::vec2& size, float rotation, const glm::vec4& color, const Ref<SubTexture2D>& subtexture) {
		drawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color, subtexture);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, glm::vec2& size, float rotation, const glm::vec4& color, const Ref<SubTexture2D>& subtexture) {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		drawRotatedQuad(transform, color, subtexture);
	}

	void Renderer2D::drawRotatedQuad(const glm::mat4& transform, const glm::vec4& color, const Ref<Texture2D>& texture) {
		glm::vec2 texcoords[] = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f}
		};

		drawRotatedQuad(transform, color, texture, texcoords);
	}

	void Renderer2D::drawRotatedQuad(const glm::mat4& transform, const glm::vec4& color, const Ref<SubTexture2D>& subtexture) {
		drawRotatedQuad(transform, color, subtexture->getTexture(), subtexture->getTexCoords());
	}

	void Renderer2D::drawRotatedQuad(const glm::mat4& transform, const glm::vec4& color, const Ref<Texture2D>& texture, const glm::vec2 texcoords[]) {
		
		if (s_Data->quadIndexCount >= Renderer2DData::maxQuadIndices) {
			startNewBatch();
		}

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data->textureSlotIndex; i++) {
			if (*s_Data->textureSlots[i].get() == *texture.get()) {
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f) {
			textureIndex = (float)s_Data->textureSlotIndex;
			s_Data->textureSlots[s_Data->textureSlotIndex] = texture;
			s_Data->textureSlotIndex++;
		}

		s_Data->quadVertexBufferPtr->position = transform * s_Data->quadVertexPositions[0];
		s_Data->quadVertexBufferPtr->color = color * s_Data->tintColor;
		s_Data->quadVertexBufferPtr->texcoord = texcoords[0];
		s_Data->quadVertexBufferPtr->texIndex = textureIndex;
		s_Data->quadVertexBufferPtr++;

		s_Data->quadVertexBufferPtr->position = transform * s_Data->quadVertexPositions[1];
		s_Data->quadVertexBufferPtr->color = color * s_Data->tintColor;
		s_Data->quadVertexBufferPtr->texcoord = texcoords[1];
		s_Data->quadVertexBufferPtr->texIndex = textureIndex;
		s_Data->quadVertexBufferPtr++;

		s_Data->quadVertexBufferPtr->position = transform * s_Data->quadVertexPositions[2];
		s_Data->quadVertexBufferPtr->color = color * s_Data->tintColor;
		s_Data->quadVertexBufferPtr->texcoord = texcoords[2];
		s_Data->quadVertexBufferPtr->texIndex = textureIndex;
		s_Data->quadVertexBufferPtr++;

		s_Data->quadVertexBufferPtr->position = transform * s_Data->quadVertexPositions[3];
		s_Data->quadVertexBufferPtr->color = color * s_Data->tintColor;
		s_Data->quadVertexBufferPtr->texcoord = texcoords[3];
		s_Data->quadVertexBufferPtr->texIndex = textureIndex;
		s_Data->quadVertexBufferPtr++;

		s_Data->quadIndexCount += 6;


		s_Data->stats.quadCount++;
	}

	void Renderer2D::drawText(const Ref<CharacterSet> characterSet, const std::string& text, const glm::vec3& position) {
		drawText(characterSet, TextAlignment::HORIZONTAL_CENTER, text, position, { 0.0f, 0.0f, 0.0f, 1.0f });
	}

	void Renderer2D::drawText(const Ref<CharacterSet> characterSet, TextAlignment textAlignment, const std::string& text, const glm::vec3& position, const glm::vec4& color, bool positionWithAspectRatio) {
		glm::ivec2 dimensions = RenderCommand::getViewportDimensions();

		float aspectRatio = (float)dimensions.x / dimensions.y;
		float xScale = 2.0f * aspectRatio / dimensions.x, yScale = 2.0f / dimensions.y;

		float x = 0, y = 0, z = position.z - 0.001f;
		float zOffset = 0.001f / text.length();
		uint8_t xM = 0, yM = 0;

		float rx = position.x * (positionWithAspectRatio ? aspectRatio : 1);

		for (int i = 0; i < text.length(); i++) {
			CharacterPointer cp = characterSet->getCharacterPointer(text.at(i));
			x += cp.advance.x >> 16;
			y += cp.advance.y >> 16;
		}

		x *= xScale;
		y *= yScale;

		switch (textAlignment) {
		case TextAlignment::HORIZONTAL_LEFT:
			x = rx;
			y = position.y;
			xM = 1;
			break;
		case TextAlignment::HORIZONTAL_CENTER:
			x = -x / 2 + rx;
			y = position.y;
			xM = 1;
			break;
		case TextAlignment::HORIZONTAL_RIGHT:
			x = -x + rx;
			y = position.y;
			xM = 1;
			break;
		case TextAlignment::VERTICAL_TOP:
			x = rx;
			y = position.y - y / text.length();
			yM = 1;
			break;
		case TextAlignment::VERTICAL_CENTER:
			x = rx;
			y = y / 2 + position.y - y / text.length();
			yM = 1;
			break;
		case TextAlignment::VERTICAL_BOTTOM:
			x = rx;
			y = y + position.y - y / text.length();
			yM = 1;
			break;
		}

		//drawCircle({rx, position.y}, 0.01f);

		for (int i = 0; i < text.length(); i++) {
			CharacterPointer cp = characterSet->getCharacterPointer(text.at(i));
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), { x + (cp.dimensions.x / 2 + cp.bearing.x) * xM * xScale, y - (cp.dimensions.y / 2 - cp.bearing.y + characterSet->getLinespacing() / 2 * xM) * yScale, z += zOffset })
				* glm::scale(glm::mat4(1.0f), { cp.dimensions.x * xScale, cp.dimensions.y * yScale, 0.0f });

			drawRotatedQuad(transform, color, characterSet->getCharacterSheet(), cp.texCoords);

			x += xM * (cp.advance.x >> 16) * xScale;
			y -= yM * (cp.advance.y >> 16) * yScale;

		}
	}

	void Renderer2D::drawText(const Ref<CharacterSet> characterSet, std::vector<uint32_t> text, const glm::vec3& position) {
		drawText(characterSet, TextAlignment::HORIZONTAL_CENTER, text, position, { 0.0f, 0.0f, 0.0f, 1.0f });
	}

	void Renderer2D::drawText(const Ref<CharacterSet> characterSet, TextAlignment textAlignment, std::vector<uint32_t> text, const glm::vec3& position, const glm::vec4& color, bool positionWithAspectRatio) {
		glm::ivec2 dimensions = RenderCommand::getViewportDimensions();

		float aspectRatio = (float)dimensions.x / dimensions.y;
		float xScale = 2.0f * aspectRatio / dimensions.x, yScale = 2.0f / dimensions.y;

		float x = 0, y = 0;
		uint8_t xM = 0, yM = 0;

		for (std::vector<uint32_t>::iterator it = text.begin(); it != text.end(); ++it) {
			CharacterPointer cp = characterSet->getCharacterPointer(*it);
			x += cp.advance.x >> 16;
			y += cp.advance.y >> 16;
		}

		x *= xScale;
		y *= yScale;

		switch (textAlignment) {
		case TextAlignment::HORIZONTAL_LEFT:
			x = position.x;
			y = position.y;
			xM = 1;
			break;
		case TextAlignment::HORIZONTAL_CENTER:
			x = -x / 2 + position.x;
			y = position.y;
			xM = 1;
			break;
		case TextAlignment::HORIZONTAL_RIGHT:
			x = -x + position.x;
			y = position.y;
			xM = 1;
			break;
		case TextAlignment::VERTICAL_TOP:
			x = position.x;
			y = position.y - y / text.size();
			yM = 1;
			break;
		case TextAlignment::VERTICAL_CENTER:
			x = position.x;
			y = y / 2 + position.y - y / text.size();
			yM = 1;
			break;
		case TextAlignment::VERTICAL_BOTTOM:
			x = position.x;
			y = y + position.y - y / text.size();
			yM = 1;
			break;
		}


		//drawCircle({ position.x, position.y }, 0.01f);

		for (std::vector<uint32_t>::iterator it = text.begin(); it != text.end(); ++it) {
			CharacterPointer cp = characterSet->getCharacterPointer(*it);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), { x + (cp.dimensions.x / 2 + cp.bearing.x) * xM * xScale, y - (cp.dimensions.y / 2 - cp.bearing.y + characterSet->getLinespacing() / 2 * xM) * yScale, position.z })
				* glm::scale(glm::mat4(1.0f), { cp.dimensions.x * xScale, cp.dimensions.y * yScale, 0.0f });

			drawRotatedQuad(transform, color, characterSet->getCharacterSheet(), cp.texCoords);

			x += xM * (cp.advance.x >> 16) * xScale;
			y -= yM * (cp.advance.y >> 16) * yScale;
		}
	}

	void Renderer2D::resetStats() {
		memset(&s_Data->stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::getStats() {
		return s_Data->stats;
	}
}