#pragma once

#include <glm/glm.hpp>
#include "Texture.h"

namespace SYR {

	class SubTexture2D {
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

		const Ref<Texture2D> getTexture() const { return m_Texture; }
		const glm::vec2* getTexCoords() const { return m_TexCoords; }

		static Ref<SubTexture2D> createFromIndex(const Ref<Texture2D>& texture, const glm::vec2& indices, const glm::vec2& cellSize, const glm::vec2& spriteSize = glm::vec2(1,1));
	private:
		Ref<Texture2D> m_Texture;

		glm::vec2 m_TexCoords[4];
	};

}