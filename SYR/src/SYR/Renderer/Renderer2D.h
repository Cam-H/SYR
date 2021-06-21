#pragma once

#include "OrthographicCamera.h"
#include "Texture.h"
#include "SubTexture2D.h"
#include "CharacterSet.h"

#include "SYR/Scene/Components.h"

namespace SYR {

	class Renderer2D {
	public:

		static void init();
		static void shutdown();

		static void beginScene(const Camera& camera);
		static void endScene();

		static void flush();

		static void setTintColor(glm::vec4& tintColor);

		//Lines

		static void drawLine(const glm::vec2& start, const glm::vec2& end);
		static void drawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color);
		static void drawLine(const glm::vec3& start, const glm::vec3& end);
		static void drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);

		static void drawLines(const glm::vec2 vertices[], uint32_t vertexCount);
		static void drawLines(const glm::vec3 vertices[], uint32_t vertexCount);
		static void drawLines(const glm::vec2 vertices[], uint32_t vertexCount, const glm::vec4& color);
		static void drawLines(const glm::vec3 vertices[], uint32_t vertexCount, const glm::vec4& color);

		static void drawLines(const glm::vec2 vertices[], uint32_t vertexCount, const glm::mat4& transform, const glm::vec4& color);
		static void drawLines(const glm::vec3 vertices[], uint32_t vertexCount, const glm::mat4& transform, const glm::vec4& color);

		//Circles

		static void drawCircle(const glm::vec2& position, float radius);
		static void drawCircle(const glm::vec2& position, float radius, const glm::vec4& color, int vertexCount = 100);

		//Quads

		static void drawQuad(const glm::vec2& position, glm::vec2& size, const glm::vec4& color);
		static void drawQuad(const glm::vec3& position, glm::vec2& size, const glm::vec4& color);

		static void drawQuad(const glm::vec2& position, glm::vec2& size, const Ref<Texture2D>& texture);
		static void drawQuad(const glm::vec3& position, glm::vec2& size, const Ref<Texture2D>& texture);

		static void drawQuad(const glm::vec2& position, glm::vec2& size, const glm::vec4& color, const Ref<Texture2D>& texture);
		static void drawQuad(const glm::vec3& position, glm::vec2& size, const glm::vec4& color, const Ref<Texture2D>& texture);

		static void drawQuad(const glm::vec2& position, glm::vec2& size, const Ref<SubTexture2D>& subtexture);
		static void drawQuad(const glm::vec3& position, glm::vec2& size, const Ref<SubTexture2D>& subtexture);

		static void drawQuad(const glm::vec2& position, glm::vec2& size, const glm::vec4& color, const Ref<SubTexture2D>& subtexture);
		static void drawQuad(const glm::vec3& position, glm::vec2& size, const glm::vec4& color, const Ref<SubTexture2D>& subtexture);

		static void drawQuad(const glm::vec2& position, glm::vec2& size, const glm::vec4& color, const Ref<Texture2D>& texture, const glm::vec2 texcoords[]);
		static void drawQuad(const glm::vec3& position, glm::vec2& size, const glm::vec4& color, const Ref<Texture2D>& texture, const glm::vec2 texcoords[]);

		static void drawQuad(const glm::mat4& transform, const glm::vec4& color, const Ref<Texture2D>& texture);
		static void drawQuad(const glm::mat4& transform, const glm::vec4& color, const Ref<SubTexture2D>& subtexture);

		//Rotated draws

		static void drawRotatedQuad(const glm::vec2& position, glm::vec2& size, float rotation, const glm::vec4& color);
		static void drawRotatedQuad(const glm::vec3& position, glm::vec2& size, float rotation, const glm::vec4& color);
																			 
		static void drawRotatedQuad(const glm::vec2& position, glm::vec2& size, float rotation, const Ref<Texture2D>& texture);
		static void drawRotatedQuad(const glm::vec3& position, glm::vec2& size, float rotation, const Ref<Texture2D>& texture);
																			
		static void drawRotatedQuad(const glm::vec2& position, glm::vec2& size, float rotation, const glm::vec4& color, const Ref<Texture2D>& texture);
		static void drawRotatedQuad(const glm::vec3& position, glm::vec2& size, float rotation, const glm::vec4& color, const Ref<Texture2D>& texture);

		static void drawRotatedQuad(const glm::vec2& position, glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture);
		static void drawRotatedQuad(const glm::vec3& position, glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture);

		static void drawRotatedQuad(const glm::vec2& position, glm::vec2& size, float rotation, const glm::vec4& color, const Ref<SubTexture2D>& subtexture);
		static void drawRotatedQuad(const glm::vec3& position, glm::vec2& size, float rotation, const glm::vec4& color, const Ref<SubTexture2D>& subtexture);

		static void drawRotatedQuad(const glm::mat4& transform, const glm::vec4& color, const Ref<Texture2D>& texture);
		static void drawRotatedQuad(const glm::mat4& transform, const glm::vec4& color, const Ref<SubTexture2D>& subtexture);

		static void drawRotatedQuad(const glm::mat4& transform, const glm::vec4& color, const Ref<Texture2D>& texture, const glm::vec2 texcoords[]);

		//Strings
		static void drawText(const Ref<CharacterSet> characterSet, const std::string& text, const glm::vec3& position);
		static void drawText(const Ref<CharacterSet> characterSet, TextAlignment textAlignment, const std::string& text, const glm::vec3& position, const glm::vec4& color, bool positionWithAspectRatio = true);

		static void drawText(const Ref<CharacterSet> characterSet, std::vector<uint32_t> text, const glm::vec3& position);
		static void drawText(const Ref<CharacterSet> characterSet, TextAlignment textAlignment, std::vector<uint32_t> text, const glm::vec3& position, const glm::vec4& color, bool positionWithAspectRatio = true);

		struct Statistics {
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;

			uint32_t getTotalVertexCount() { return quadCount * 4; }
			uint32_t getTotalIndexCount() { return quadCount * 6; }
		};

		static void resetStats();
		static Statistics getStats();
	private:
		static void startNewBatch();

		static void prepareForBatch();
	};

}