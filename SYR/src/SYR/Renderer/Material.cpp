#include "syrpch.h"
#include "Material.h"

#include "SYR/FileIO/FileHandler.h"

namespace SYR {

	MaterialLibrary::MaterialLibrary() {
		m_Materials.push_back(
			{ "DEFAULT", {0.1f, 0.1f, 0.1f}, {1, 0, 1}, {0, 0, 0}, 32 }
		);

		m_Materials.push_back(
			{ "None", {0, 0, 0}, {1, 0.5f, 1}, {1, 1, 0}, 32 }
		);
	}

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

		Material material;

		const char* delimiter = "\n";
		size_t delimLength = strlen(delimiter);

		size_t pos = 0;
		size_t prev = 0;

		while ((pos = MTLContent.find(delimiter, prev)) != std::string::npos) {

			std::string line = MTLContent.substr(prev, pos - prev);

			size_t begin = line.find(' ');
			std::string token = line.substr(0, begin++);

			if (token == "newmtl") {
				material.name = line.substr(token.length() + 1);

				if (exists(material.name)) {
					SYR_CORE_WARN("A material already exists with name: \"{0}\"! The material was not loaded", material.name);
					return;
				}

			} else if (token == "Ka") {//AMBIENT
				material.ambient = {
										(float)atof(line.substr(begin, line.find(' ', begin)).c_str()),
										(float)atof(line.substr(line.find(' ', begin), line.find_last_of(' ')).c_str()),
										(float)atof(line.substr(line.find_last_of(' ')).c_str())
				};

			} else if (token == "Kd") {//DIFFUSE
				material.diffuse = {
										(float)atof(line.substr(begin, line.find(' ', begin)).c_str()),
										(float)atof(line.substr(line.find(' ', begin), line.find_last_of(' ')).c_str()),
										(float)atof(line.substr(line.find_last_of(' ')).c_str())
				};

			} else if (token == "Ks") {//SPECULAR
				material.specular = {
										(float)atof(line.substr(begin, line.find(' ', begin)).c_str()),
										(float)atof(line.substr(line.find(' ', begin), line.find_last_of(' ')).c_str()),
										(float)atof(line.substr(line.find_last_of(' ')).c_str())
				};

			} else if (token == "Ns") {//SPECULAR HIGHLIGHTS (shine)
				material.shine = (float)atof(line.substr(begin).c_str());
			}

			prev = pos + 1;
		}

		m_Materials.push_back(material);
	}

	uint32_t MaterialLibrary::getIDOfMaterial(const std::string& materialName) {
		int i = 1;

		for (std::vector<Material>::iterator it = m_Materials.begin(); it != m_Materials.end(); ++it) {
			if (it->name == materialName) {
				return i;
			}

			i++;
		}

		SYR_CORE_WARN("The specified material was not found! Returning default material ID (0)");

		return 0;
	}

	bool MaterialLibrary::exists(const std::string& materialName) {
		for (std::vector<Material>::iterator it = m_Materials.begin(); it != m_Materials.end(); ++it) {
			if (it->name == materialName) {
				return true;
			}
		}

		return false;
	}
}