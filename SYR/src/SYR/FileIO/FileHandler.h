#pragma once

#include <fstream>

#include "SYR/Renderer/VertexArray.h"

namespace SYR {

	enum class FileFormat {
		NONE = 0, OBJ, MTL
	};

	static FileFormat getFileFormat(const std::string& filepath) {
		std::string fileFormat = filepath.substr(filepath.find_last_of('.') + 1);

		if (fileFormat == "obj") {
			return FileFormat::OBJ;
		} else if (fileFormat == "mtl") {
			return FileFormat::MTL;
		}

		SYR_CORE_WARN("Unrecognized file format: {0}", fileFormat);

		return FileFormat::NONE;
	}

	static std::string readFile(const std::string& filepath, bool insertFilePath = false) {
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

	static std::vector<char> readBinaryFile(const std::string& filepath) {
		std::ifstream input(filepath, std::ios::binary);

		std::vector<char> bytes(
			(std::istreambuf_iterator<char>(input)),
			(std::istreambuf_iterator<char>()));

		input.close();

		return bytes;
	}

	class FileHandler {
	public:
		static std::vector<uint32_t> parseUTF8File(const std::string& filepath);

		static Ref<VertexArray> loadMesh(const std::string& filepath);
	
	private:
	};
}