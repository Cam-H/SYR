#include "syrpch.h"
#include "FileHandler.h"

#include "OBJFileHandler.h"

#include <sys/stat.h>

namespace SYR {

	bool FileHandler::fileExists(const std::string& filepath) {
		struct stat buffer;

		return stat(filepath.c_str(), &buffer) == 0;
	}

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

	void FileHandler::removeBOM(std::string& data) {//TODO
		//0x00, 0x00, 0xfe, 0xff --The file is almost certainly UTF - 32BE
		//0xff, 0xfe, 0x00, 0x00 --The file is almost certainly UTF - 32LE
		//0xfe, 0xff, XX, XX     -- The file is almost certainly UTF - 16BE
		//0xff, 0xfe, XX, XX(but not 00, 00) --The file is almost certainly UTF - 16LE
		//0xef, 0xbb, 0xbf, XX   -- The file is almost certainly UTF - 8 With a BOM
		
		uint8_t bom[] = {(uint8_t)data.at(0), (uint8_t)data.at(1), (uint8_t)data.at(2), (uint8_t)data.at(3) };

		uint8_t utf8BOM[] = { 0xEF, 0xBB, 0xBF };

		if (bom[0] == utf8BOM[0] && bom[1] == utf8BOM[1] && bom[2] == utf8BOM[2]) {
			data = data.substr(3);//Remove the BOM from the data
		}
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

		removeBOM(result);

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

	void FileHandler::writeFile(const std::string& filepath, std::vector<std::string> content) {
		std::string data = *content.begin();

		for (std::vector<std::string>::iterator it = content.begin() + 1; it != content.end(); ++it) {
			data += "\n" + *it;
		}

		writeFile(filepath, data);
	}

	void FileHandler::writeFile(const std::string& filepath, std::string content) {
		std::ofstream out;
		out.open(filepath, std::ios::out);

		//Byte Order Mark (BOM)
		unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
		out.write((char*)bom, sizeof(bom));
		
		out << content;

		out.close();
	}

	Ref<VertexArray> FileHandler::loadMesh(const std::string& filepath, boolean smoothed) {

		FileFormat format = getFileFormat(filepath);

		std::string content = readFile(filepath, true);

		switch (format) {
		case FileFormat::OBJ:
			return smoothed ? OBJFileHandler::loadSmoothMesh(content) : OBJFileHandler::loadMesh(content);
		default:
			SYR_CORE_WARN("Could not load mesh!");
			return VertexArray::create();
		}

		return VertexArray::create();
	}

}