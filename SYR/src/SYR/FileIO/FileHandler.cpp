#include "syrpch.h"
#include "FileHandler.h"

#include "OBJFileHandler.h"

namespace SYR {

	std::vector<uint32_t> FileHandler::parseUTF8File(const std::string& filepath) {
		std::vector<char> bytes = readBinaryFile(filepath);

		std::vector<uint32_t> content(0);

		for (std::vector<char>::iterator it = bytes.begin(); it != bytes.end(); ++it) {
			if ((*it & 0b10000000) == 0b00000000) {//1-byte UTF
				content.push_back(*it);
			} else if ((*it & 0b11100000) == 0b11000000) {//2-byte UTF
				content.push_back((0b00011111 & *it) << 6 | (0b00111111 & *(++it)));
			} else if ((*it & 0b11110000) == 0b11100000) {//3-byte UTF
				content.push_back((0b00001111 & *it) << 12 | (0b00111111 & *(++it)) << 6 | (0b00111111 & *(++it)));
			} else if ((*it & 0b11111000) == 0b11110000) {//4-byte UTF
				content.push_back((0b00000111 & *it) << 18 | (0b00111111 & *(++it)) << 12 | (0b00111111 & *(++it)) << 6 | (0b00111111 & *(++it)));
			}
		}

		return content;
	}

	Ref<VertexArray> FileHandler::loadMesh(const std::string& filepath) {

		FileFormat format = getFileFormat(filepath);

		std::string content = readFile(filepath);

		switch (format) {
		case FileFormat::OBJ:
			return OBJFileHandler::parseOBJFile(content);
		default:
			SYR_CORE_WARN("Could not load mesh!");
			return VertexArray::create();
		}

		return VertexArray::create();
	}

}