#pragma once

#include <glm/glm.hpp>
#include <map>

namespace SYR {

	struct Material {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		float shine;
	};

	class MaterialLibrary {
	public:
		MaterialLibrary();

		void loadMaterials(const std::string& materialFile);

		void add(Material material);

		bool exists(const std::string& name);

		Ref<Material> get(const std::string& name);
	private:
		void loadMTLMaterial(const std::string& MTLContent);
	private:
		std::unordered_map<std::string, Ref<Material>> m_Materials;
	};
}