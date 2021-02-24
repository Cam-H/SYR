#include "syrpch.h"
#include "FileHandler.h"

#include "OBJFileHandler.h"

namespace SYR {

	FileFormat FileHandler::getFileFormat(const std::string& filepath) {
		std::string fileFormat = filepath.substr(filepath.find_last_of('.') + 1);

		if (fileFormat == "obj") {
			return FileFormat::OBJ;
		}
		else if (fileFormat == "mtl") {
			return FileFormat::MTL;
		}

		SYR_CORE_WARN("Unrecognized file format: {0}", fileFormat);

		return FileFormat::NONE;
	}

	std::string FileHandler::readFile(const std::string& filepath, bool insertFilePath) {
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);

		if (in) {
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else {
			SYR_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return (insertFilePath ? filepath.substr(0, filepath.find_last_of("\\/") + 1) + "\n" : "") + result;
	}

	std::vector<char> FileHandler::readBinaryFile(const std::string& filepath) {
		std::ifstream input(filepath, std::ios::binary);

		std::vector<char> bytes(
			(std::istreambuf_iterator<char>(input)),
			(std::istreambuf_iterator<char>()));

		input.close();

		return bytes;
	}

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

		std::string content = readFile(filepath, true);

		switch (format) {
		case FileFormat::OBJ:
			return OBJFileHandler::loadMesh(content);
		default:
			SYR_CORE_WARN("Could not load mesh!");
			return VertexArray::create();
		}

		return VertexArray::create();
	}

}