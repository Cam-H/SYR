#include "syrpch.h"
#include "SubTexture2D.h"

namespace SYR {

	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max) : m_Texture(texture) {
		m_TexCoords[0] = { min.x, min.y };
		m_TexCoords[1] = { max.x, min.y };
		m_TexCoords[2] = { max.x, max.y };
		m_TexCoords[3] = { min.x, max.y };
	}

	Ref<SubTexture2D> SubTexture2D::createFromIndex(const Ref<Texture2D>& texture, const glm::vec2& indices, const glm::vec2& cellSize, const glm::vec2& spriteSize) {
		glm::vec2 min = { (indices.x * cellSize.x) / texture->getWidth(), (indices.y * cellSize.y) / texture->getHeight() };
		glm::vec2 max = { ((indices.x + spriteSize.x) * cellSize.x) / texture->getWidth(), ((indices.y + spriteSize.y) * cellSize.y) / texture->getHeight() };

		return createRef<SubTexture2D>(texture, min, max);
	}

}