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
		m_CharacterSetLibrary = createRef<CharacterSetLibrary>();
	}

	void Renderer::onWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::setViewport(0, 0, width, height);
	}

	void Renderer::beginScene(Camera& camera){
		m_SceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
	}
	
	void Renderer::endScene() {}

	void Renderer::submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform) {
		shader->bind();

		shader->setMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		shader->setMat4("u_Transform", transform);

		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}

}