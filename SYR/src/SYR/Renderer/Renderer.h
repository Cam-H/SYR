#pragma once

#include "RenderCommand.h"

#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"

#include "Shader.h"
#include "Material.h"
#include "CharacterSet.h"

namespace SYR {

	class Renderer {
	public:
		static void init();
		static void shutdown();

		static void onWindowResize(uint32_t width, uint32_t height);

		static void beginScene(Camera& camera);
		static void endScene();

		static void flush();

		static void submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void outline(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }

		inline static Ref<ShaderLibrary> getShaderLibrary() { return m_ShaderLibrary; }
		inline static Ref<MaterialLibrary> getMaterialLibrary() { return m_MaterialLibrary; }
		inline static Ref<CharacterSetLibrary> getCharacterSetLibrary() { return m_CharacterSetLibrary; }

	private:
		struct SceneData {
			glm::mat4 ViewProjectionMatrix;
			glm::vec3 CameraPosition;
		};

		static SceneData* m_SceneData;

		inline static Ref<ShaderLibrary> m_ShaderLibrary;
		inline static Ref<MaterialLibrary> m_MaterialLibrary;
		inline static Ref<CharacterSetLibrary> m_CharacterSetLibrary;
	};
}