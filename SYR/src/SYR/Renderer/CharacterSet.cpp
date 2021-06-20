#include "syrpch.h"
#include "CharacterSet.h"

#include <ft2build.h>
#include FT_FREETYPE_H 

namespace SYR {

	CharacterCollection CharacterSet::getCollectionByName(const std::string& name) {
		if (name.compare("ASCII") == 0) {
			return CharacterCollection::ASCII;
		}else if (name.compare("ENGLISH") == 0) {
			return CharacterCollection::ENGLISH;
		}
		else if (name.compare("HIRAGANA") == 0) {
			return CharacterCollection::HIRAGANA;
		}
		else if (name.compare("KATAKANA") == 0) {
			return CharacterCollection::KATAKANA;
		}
		else if (name.compare("JAPANESE") == 0) {
			return CharacterCollection::JAPANESE;
		}
	}


	static void addCharacterRange(FT_Face face, std::vector<Character>* characters, uint32_t minCode, uint32_t maxCode);
	static void addCharacter(FT_Face face, std::vector<Character>* characters, uint32_t code);
	static Character getCharacter(FT_Face face, uint32_t code);

	Ref<CharacterSet> CharacterSet::create(const std::string& ttfPath, CharacterCollection characterCollection, uint32_t fontSize) {
		return createRef<CharacterSet>(ttfPath, characterCollection, fontSize);
	}

	Ref<CharacterSet> CharacterSet::create(const std::string& ttfPath, std::vector<uint32_t> set, uint32_t fontSize) {
		return createRef<CharacterSet>(ttfPath, set, fontSize);
	}

	Ref<CharacterSet> CharacterSet::create(std::vector<char> characterSetByteData) {
		return createRef<CharacterSet>(characterSetByteData);
	}


	CharacterSet::CharacterSet(const std::string& ttfPath, CharacterCollection characterCollection, uint32_t fontSize) {//TODO
		FT_Library  library;
		FT_Face face;

		std::vector<Character> characters(0);
		uint32_t width = 0, height = 0;
		SYR_CORE_WARN("A1");

		SYR_CORE_ERROR(FT_Init_FreeType(&library));
		SYR_CORE_ERROR(FT_New_Face(library, ttfPath.c_str(), 0, &face));

		FT_Set_Pixel_Sizes(face, 0, fontSize);
		FT_Select_Charmap(face, ft_encoding_unicode);

		switch (characterCollection) {
		case CharacterCollection::JAPANESE:
			addCharacterRange(face, &characters, 0x3041, 0x3096);//Hiragana
			addCharacterRange(face, &characters, 0x30A0, 0x30FF);//Katakana
			//addCharacterRange(face, &characters, 0x2E80, 0x2FD5);//Kanji Radicals
			//TODO kanji

			addCharacterRange(face, &characters, 32, 126);//English characters

			break;
		case CharacterCollection::ASCII:
		case CharacterCollection::ENGLISH:
			addCharacterRange(face, &characters, 32, 126);
			break;
		}

		if (characterCollection != CharacterCollection::ASCII && characterCollection != CharacterCollection::ENGLISH)
			addCharacter(face, &characters, 63);//Add "?" character in case user tries to render characters that are not in the set

		m_Ascent = face->ascender >> 6;//TODO manage different ascender conventions
		m_Linespacing = face->height >> 6;

		FT_Done_Face(face);
		FT_Done_FreeType(library);

		uint8_t* set = pack(characters, &width, &height);

		uint8_t* buffer = new uint8_t[width * height * 4];

		uint32_t z = 0;
		for (uint32_t i = 0; i < height; i++) {
			for (uint32_t j = 0; j < width; j++) {
				buffer[z] = 255;
				buffer[z + 1] = 255;
				buffer[z + 2] = 255;
				buffer[z + 3] = *(set + j + width * i);

				z += 4;
			}
		}

		m_CharacterSheet = Texture2D::create(buffer, width, height);
	}

	CharacterSet::CharacterSet(const std::string& ttfPath, std::vector<uint32_t> set, uint32_t fontSize) {
		FT_Library  library;
		FT_Face face;

		std::vector<Character> characters(0);
		uint32_t width = 0, height = 0;

		SYR_CORE_ERROR("{0}", FT_Init_FreeType(&library));
		SYR_CORE_ERROR("{0}", FT_New_Face(library, ttfPath.c_str(), 0, &face));

		FT_Set_Pixel_Sizes(face, 0, fontSize);
		FT_Select_Charmap(face, ft_encoding_unicode);

		set.push_back(63);//Add "?" character in case user tries to render characters that are not in the set

		for (std::vector<uint32_t>::iterator it = set.begin(); it != set.end(); ++it) {
			if (std::find(set.begin(), set.end(), *it) == it) {//Verify this character code is the first instance in the list - prevents duplicates
				addCharacter(face, &characters, *it);
			}

		}

		m_Ascent = face->ascender >> 6;//TODO manage different ascender conventions
		m_Linespacing = face->height >> 6;

		FT_Done_Face(face);
		FT_Done_FreeType(library);

		uint8_t* characterSet = pack(characters, &width, &height);

		uint8_t* buffer = new uint8_t[width * height * 4];

		uint32_t z = 0;
		for (uint32_t i = 0; i < height; i++) {
			for (uint32_t j = 0; j < width; j++) {
				buffer[z] = 255;
				buffer[z + 1] = 255;
				buffer[z + 2] = 255;
				buffer[z + 3] = *(characterSet + j + width * i);

				z += 4;
			}
		}

		m_CharacterSheet = Texture2D::create(buffer, width, height);
	}

	CharacterSet::CharacterSet(std::vector<char> characterSetByteData) {
		/*
		std::vector<Character> characters(0);
		uint32_t width = 0, height = 0;

		uint8_t* set = pack(characters, &width, &height);

		uint8_t* buffer = new uint8_t[width * height * 4];

		int z = 0;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				buffer[z] = 0;
				buffer[z + 1] = 255;
				buffer[z + 2] = 255;
				buffer[z + 3] = *(set + j + width * i);

				z += 4;
			}
		}

		characterSheet = Texture2D::create(buffer, width, height);
		*/
	}

	static void addCharacterRange(FT_Face face, std::vector<Character>* characters, uint32_t minCode, uint32_t maxCode) {

		for (uint32_t i = minCode; i < maxCode; i++) {
			Character character = getCharacter(face, i);

			uint32_t z = 0;
			for (uint32_t j = 0; j < characters->size(); j++) {
				if (characters->at(j).height < character.height) {
					break;
				}

				z++;
			}
			characters->insert(characters->begin() + z, character);
		}
	}

	static void addCharacter(FT_Face face, std::vector<Character>* characters, uint32_t code) {
		Character character = getCharacter(face, code);

		uint32_t z = 0;
		for (uint32_t j = 0; j < characters->size(); j++) {
			if (characters->at(j).height < character.height) {
				break;
			}

			z++;
		}
		characters->insert(characters->begin() + z, character);
	}

	void CharacterSet::addCharactersToSet(CharacterCollection collection, std::vector<uint32_t>* set) {

		switch (collection) {
		case CharacterCollection::ASCII:
		case CharacterCollection::ENGLISH:
			for (uint32_t i = 32; i <= 126; i++) {
				set->push_back(i);
			}
			break;
		case CharacterCollection::HIRAGANA:
			for (uint32_t i = 0x3041; i <= 0x3096; i++) {
				set->push_back(i);
			}
			break;
		case CharacterCollection::KATAKANA:
			for (uint32_t i = 0x30A0; i <= 0x30FF; i++) {
				set->push_back(i);
			}
			break;
		default:
			SYR_CORE_WARN("Unrecognized collection!");
		}
	}

	static Character getCharacter(FT_Face face, uint32_t code) {
		FT_Load_Glyph(face, FT_Get_Char_Index(face, code), FT_LOAD_RENDER);

		Character character;

		character.key = code;

		character.width = face->glyph->bitmap.width;
		character.height = face->glyph->bitmap.rows;

		character.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		character.advance = glm::ivec2(face->glyph->linearHoriAdvance, face->glyph->linearVertAdvance);

		uint8_t* temp = new uint8_t[character.width * character.height];
		for (uint32_t i = 0; i < character.width * character.height; i++) {
			temp[i] = *(face->glyph->bitmap.buffer + i);
		}
		character.buffer = temp;

		return character;
	}

	uint8_t* CharacterSet::pack(std::vector<Character> characters, uint32_t* width, uint32_t* height) {

		if (characters.empty()) {
			SYR_CORE_WARN("WARNING: Empty character set!");
			return uint8_t();
		}

		int cellSpacing = 2;//Must be an even number

		uint32_t x = 0, y = 0;
		uint32_t area = 0;

		//Calculate area + width for the character sheet
		for (std::vector<Character>::iterator it = characters.begin(); it != characters.end(); ++it) {

			//Width/Height increased to add spacing between cells
			it->width += cellSpacing;
			it->height += cellSpacing;

			area += it->width * it->height;
		}

		*width = sqrt(area);
		*height = 0;
		x = 0;
		y = characters.at(0).height;

		//Calculate necessary height for the character sheet
		for (std::vector<Character>::iterator it = characters.begin(); it != characters.end(); ++it) {
			if (x + it->width > * width) {
				x = 0;
				*height += y;
				y = it->height;
			}

			x += it->width;
		}

		*height += y;

		//Write all the characters to the character sheet buffer
		uint8_t* content = new uint8_t[*width * *height];
		for (uint32_t i = 0; i < *width * *height; i++) {
			content[i] = 0;
		}

		x = y = 0;
		uint32_t offset = characters.at(0).height;
		for (std::vector<Character>::iterator it = characters.begin(); it != characters.end(); ++it) {
			if (x + it->width > *width) {
				x = 0;
				y += offset;
				offset = it->height;
			}

			CharacterPointer cp = CharacterPointer(x + cellSpacing / 2, y + cellSpacing / 2, it->bearing, it->advance, it->width - cellSpacing, it->height - cellSpacing, *width, *height);
			m_CharacterList.insert(std::pair<uint32_t, CharacterPointer>(it->key, cp));
			for (uint32_t i = 0; i < it->height - cellSpacing; i++) {
				for (uint32_t j = 0; j < it->width - cellSpacing; j++) {
					content[x + j + (y + i + 1) * *width + cellSpacing / 2] = *(it->buffer + j + i * (it->width - cellSpacing));
				}
			}

			x += it->width;
		}

		return content;
	}

	CharacterSetLibrary::CharacterSetLibrary() {
		std::vector<uint32_t> codes(0);
		SYR::CharacterSet::addCharactersToSet(SYR::CharacterCollection::ASCII, &codes);

		add("DEFAULT", CharacterSet::create("assets/fonts/arial.ttf", codes, 48));
	}

	void CharacterSetLibrary::add(const std::string& name, const Ref<CharacterSet>& characterSet) {
		SYR_CORE_ASSERT(!exists(name), "Character Set already exists!");
		m_CharacterSets[name] = characterSet;
	}

	bool CharacterSetLibrary::exists(const std::string& name) {
		return m_CharacterSets.find(name) != m_CharacterSets.end();
	}

	Ref<CharacterSet> CharacterSetLibrary::get(const std::string& name) {
		SYR_CORE_ASSERT(exists(name), "Character Set {0} not found!", name);
		return m_CharacterSets[name];
	}
}