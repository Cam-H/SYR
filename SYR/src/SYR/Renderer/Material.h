#pragma once

#include <glm/glm.hpp>
#include <map>

namespace SYR {

	struct Material {
		std::string name;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		float shine;
	};

	class MaterialLibrary {
	public:
		MaterialLibrary();

		void loadMaterials(const std::string& materialFile);

		//void add(Material material);

		Material get(uint32_t materialID) { return m_Materials.at(materialID); }

		uint32_t getIDOfMaterial(const std::string& materialName);

		uint32_t getMaterialCount() { return m_Materials.size(); }

		bool exists(const std::string& materialName);

		//Ref<Material> get(const std::string& name);
	private:
		void loadMTLMaterial(const std::string& MTLContent);
	private:
		std::vector<Material> m_Materials;
	};
}