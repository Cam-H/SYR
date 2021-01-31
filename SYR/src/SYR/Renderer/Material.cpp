#include "syrpch.h"
#include "Material.h"

#include "SYR/FileIO/FileHandler.h"

namespace SYR {

	MaterialLibrary::MaterialLibrary(){}

	void MaterialLibrary::loadMaterials(const std::string& materialFile) {

		switch (getFileFormat(materialFile)) {
		case FileFormat::MTL:
			{
			std::string content = readFile(materialFile);

			const char* delimiter = "newmtl";
			size_t delimLength = strlen(delimiter);

			size_t pos = 0;

			while ((pos = content.find(delimiter, pos)) != std::string::npos) {
				loadMTLMaterial(content.substr(pos, content.find(delimiter, pos + 1) - pos));
				pos++;
			}

			}
			break;
		default:
			SYR_CORE_WARN("Unrecognized material file format! No materials were loaded");
		}
	}

	void MaterialLibrary::loadMTLMaterial(const std::string& MTLContent) {

		Ref<Material> material = createRef<Material>();
		std::string materialName = "";

		const char* delimiter = "\n";
		size_t delimLength = strlen(delimiter);

		size_t pos = 0;
		size_t prev = 0;

		while ((pos = MTLContent.find(delimiter, prev)) != std::string::npos) {

			std::string line = MTLContent.substr(prev, pos - prev);

			size_t begin = line.find(' ');
			std::string token = line.substr(0, begin++);

			if (token == "newmtl") {
				materialName = line.substr(token.length() + 1);
			} else if (token == "Ka") {//AMBIENT
				material->ambient = {
										(float)atof(line.substr(begin, line.find(' ', begin)).c_str()),
										(float)atof(line.substr(line.find(' ', begin), line.find_last_of(' ')).c_str()),
										(float)atof(line.substr(line.find_last_of(' ')).c_str())
				};

			} else if (token == "Kd") {//DIFFUSE
				material->diffuse = {
										(float)atof(line.substr(begin, line.find(' ', begin)).c_str()),
										(float)atof(line.substr(line.find(' ', begin), line.find_last_of(' ')).c_str()),
										(float)atof(line.substr(line.find_last_of(' ')).c_str())
				};

			} else if (token == "Ks") {//SPECULAR
				material->specular = {
										(float)atof(line.substr(begin, line.find(' ', begin)).c_str()),
										(float)atof(line.substr(line.find(' ', begin), line.find_last_of(' ')).c_str()),
										(float)atof(line.substr(line.find_last_of(' ')).c_str())
				};

			} else if (token == "Ns") {//SPECULAR HIGHLIGHTS (shine)
				material->shine = (float)atof(line.substr(begin).c_str());
			}

			prev = pos + 1;
		}

		m_Materials.insert(std::pair<std::string, Ref<Material>>(materialName, material));
	}

}