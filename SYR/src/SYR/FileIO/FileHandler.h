#pragma once

#include <fstream>

#include "SYR/Renderer/VertexArray.h"

namespace SYR {

	enum class FileFormat {
		NONE = 0, OBJ, MTL
	};

	

	class FileHandler {
	public:

		static bool fileExists(const std::string& filepath);

		static FileFormat getFileFormat(const std::string& filepath);

		static std::string readFile(const std::string& filepath, bool insertFilePath = false);
		static std::vector<char> readBinaryFile(const std::string& filepath);

		template<class T>
		static std::vector<T> readFile(std::string filepath) {
			std::vector<T> list;

			std::ifstream in(filepath, std::ios::in | std::ios::binary | std::ios::ate);

			float elementCount = (float)in.tellg() / sizeof(T);
			SYR_CORE_ASSERT(elementCount != (int)elementCount, "The input file size is not an exact multiple of the object provided. This may cause unpredictable behavior.");
			in.seekg(0, std::ios::beg);

			//Use a for counter rather than !in.eof() to determine end of file because the final obj was being duplicated when reading
			for (uint32_t i = 0; i < elementCount; i++) {
				T newObj;
				in.read((char*)&newObj, sizeof(T));
				list.push_back(newObj);
			}

			in.close();

			return list;
		}


		static std::vector<uint32_t> parseUTF8File(const std::string& filepath);

		static void writeFile(const std::string& filepath, std::vector<std::string> content);
		static void writeFile(const std::string& filepath, std::string content);

		template<typename T>
		static void writeFile(std::string filepath, std::vector<T> objectData) {
			std::ofstream out(filepath, std::ios::out | std::ios::binary);
			if (!out) {
				SYR_CORE_WARN("Can not open file!");
				return;
			}

			for (typename std::vector<T>::iterator it = objectData.begin(); it != objectData.end(); ++it) {
				out.write((char*)&(*it), sizeof(T));
			}

			out.close();
		}

		static Ref<VertexArray> loadMesh(const std::string& filepath, boolean smoothed = false);

	private:

		static void removeBOM(std::string& data);
	};
}