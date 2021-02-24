#include "syrpch.h"
#include "Renderer.h"
#include "Renderer2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace SYR {

	struct MeshVertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;
	};

	struct RendererData {

	};

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData();

	void Renderer::init() {
		RenderCommand::init();
		Renderer2D::init();

		m_ShaderLibrary = createRef<ShaderLibrary>();
		m_MaterialLibrary = createRef<MaterialLibrary>();
		m_CharacterSetLibrary = createRef<CharacterSetLibrary>();
	}

	void Renderer::onWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::setViewport(0, 0, width, height);
	}

	void Renderer::beginScene(Camera& camera){
		m_SceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
		m_SceneData->CameraPosition = camera.getPosition();
	}
	
	void Renderer::endScene() {}

	void Renderer::submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform) {
		shader->bind();

		shader->setMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		shader->setMat4("u_Transform", transform);
		shader->setMat3("u_NormalMatrix", glm::transpose(glm::inverse(transform)));
		shader->setFloat3("u_ViewPosition", m_SceneData->CameraPosition);

		//shader->setFloat3("u_Materials[1].ambient", { 1.0f, 0.5f, 1 });

		
		for (uint32_t i = 0; i < m_MaterialLibrary->getMaterialCount(); i++) {
			Material mat = m_MaterialLibrary->get(i);

			shader->setFloat3("u_Materials[" + std::to_string(i) + "].ambient", mat.ambient);
			shader->setFloat3("u_Materials[" + std::to_string(i) + "].diffuse", mat.diffuse);
			shader->setFloat3("u_Materials[" + std::to_string(i) + "].specular", mat.specular);
			shader->setFloat("u_Materials[" + std::to_string(i) + "].shine", mat.shine);
		}
		shader->setFloat3("u_AmbientLight", { 0.01f, 0.01f, 0.01f });

		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}

	void Renderer::outline(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform) {
		shader->bind();

		shader->setMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		shader->setMat4("u_Transform", transform);
		//shader->setMat3("u_NormalMatrix", glm::transpose(glm::inverse(transform)));
		//shader->setFloat3("u_ViewPosition", m_SceneData->CameraPosition);


		
		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}

}