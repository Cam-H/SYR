#pragma once

#include <fstream>

#include "SYR/Renderer/VertexArray.h"

namespace SYR {

	enum class FileFormat {
		NONE = 0, OBJ, MTL
	};

	

	class FileHandler {
	public:

		static FileFormat getFileFormat(const std::string& filepath);

		static std::string readFile(const std::string& filepath, bool insertFilePath = false);
		static std::vector<char> readBinaryFile(const std::string& filepath);

		static std::vector<uint32_t> parseUTF8File(const std::string& filepath);

		static Ref<VertexArray> loadMesh(const std::string& filepath);
	
	private:
	};
}