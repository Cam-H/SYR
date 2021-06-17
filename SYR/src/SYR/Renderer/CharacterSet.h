#pragma once

#include <glm/glm.hpp>
#include "Texture.h"
#include <map>

namespace SYR {

	struct Character {
		uint32_t width;
		uint32_t height;

		uint32_t key;

		glm::ivec2 bearing;
		glm::ivec2 advance;

		uint8_t* buffer;
	};

	struct CharacterPointer {
		
		CharacterPointer() = default;
		CharacterPointer(uint32_t x, uint32_t y, glm::ivec2 bearing, glm::ivec2 advance, uint32_t width, uint32_t height, uint32_t containerWidth, uint32_t containerHeight) {
			float rx = (float)x / containerWidth, ry = (float)y / containerHeight;
			float rw = (float)width / containerWidth, rh = (float)height / containerHeight;

			texCoords[0] = { rx, ry };
			texCoords[1] = { rx + rw, ry };
			texCoords[2] = { rx + rw, ry + rh };
			texCoords[3] = { rx, ry + rh };

			dimensions = { width, height };
			this->bearing = bearing;
			this->advance = advance;
		}

		glm::vec2 texCoords[4];
		
		glm::ivec2 dimensions;
		glm::ivec2 bearing;
		glm::ivec2 advance;
	};

	enum class CharacterCollection {
		NONE = 0,
		ENGLISH, ASCII,
		JAPANESE, HIRAGANA, KATAKANA
	};

	class CharacterSet {
	public:
		CharacterSet(const std::string& ttfPath, CharacterCollection characterCollection, uint32_t fontSize = 48);
		CharacterSet(const std::string& ttfPath, std::vector<uint32_t> set, uint32_t fontSize = 48);
		CharacterSet(std::vector<char> characterSetByteData);

		static Ref<CharacterSet> create(const std::string& ttfPath, CharacterCollection characterCollection, uint32_t fontSize = 48);
		static Ref<CharacterSet> create(const std::string& ttfPath, std::vector<uint32_t> set, uint32_t fontSize = 48);
		static Ref<CharacterSet> create(std::vector<char> characterSetByteData);

		static CharacterCollection getCollectionByName(const std::string& name);

		static void addCharactersToSet(CharacterCollection collection, std::vector<uint32_t>* set);

		Ref<Texture2D> getCharacterSheet() { return m_CharacterSheet; }
		
		const CharacterPointer getCharacterPointer(uint32_t key) { return m_CharacterList.find(key) == m_CharacterList.end() ? m_CharacterList[63] : m_CharacterList[key]; }

		const int getAscent() { return m_Ascent; }
		const int getLinespacing() { return m_Linespacing; }
	private:
		uint8_t* pack(std::vector<Character> rectangles, uint32_t* width, uint32_t* height);
	private:
		Ref<Texture2D> m_CharacterSheet;
		std::map<uint32_t, CharacterPointer> m_CharacterList;

		int m_Ascent;
		int m_Linespacing;
	};

	class CharacterSetLibrary {
	public:
		CharacterSetLibrary();

		void add(const std::string& name, const Ref<CharacterSet>& characterSet);

		bool exists(const std::string& name);

		Ref<CharacterSet> get(const std::string& name);
	private:
		std::unordered_map<std::string, Ref<CharacterSet>> m_CharacterSets;
	};
}