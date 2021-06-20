#include "syrpch.h"
#include "Scene.h"

#include "CollisionSystem.h"
#include "SYR/Systems/UiSystem.h"
#include "SYR/Systems/InputSystem.h"
#include "SYR/Systems/CommandSystem.h"

#include "SYR/Renderer/Renderer.h"
#include "SYR/Renderer/Renderer2D.h"

#include "SYR/Core/Input.h"

#include <glm/gtx/matrix_decompose.hpp>


#include "Entity.h"

namespace SYR {

	Scene::Scene() : m_UiCamera(CameraType::ORTHOGRAPHIC, CameraControlType::LOCKED_CAMERA, 1280.0f / 720.0f) {
		//entt::entity = m_Registry.create();

		//auto& transform = m_Registry.insert<TransformComponent>(entity, glm::mat4(1.0f));
		//m_Registry.remove(); m_Registry.clear();

		//m_Registry.has<TransformComponent>(entity);
		//TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

		//m_Registry.view<TransformComponent>()
		//for(auto entity : view){}

		//Adding observer for the addition of a component type
		//m_Registry.on_construct<VelocityComponent>().connect<&test>();

		//entt::collector.update<sprite>().where<position>(entt::exclude<velocity>);
		
		//Entity mainPanel = generateMainPanel();
	}

	Scene::~Scene() {

	}

	void Scene::prepare() {
		
	}

	void Scene::loadUi(Ref<Scene> scene, const std::string& filepath) {
		scene->m_UiEnabled = true;

		scene->m_MainPanel = Entity::create(UiSystem::loadPredefinedUi(scene.get(), filepath).getHandle(), scene.get());
		scene->m_MainPanel->getComponent<TransformComponent>().scale({ 2 * scene->m_AspectRatio, 2 });

		//Link entity IDs to the anchors
		auto anchorView = scene->m_Registry.view<AnchorComponent>();
		for (const entt::entity e : anchorView) {
			anchorView.get(e).horizontalAnchorHandle = scene->getEntityByID(anchorView.get(e).horizontalAnchorHandleID).getHandle();
			anchorView.get(e).verticalAnchorHandle = scene->getEntityByID(anchorView.get(e).verticalAnchorHandleID).getHandle();
		}

		//scene->getEntityByID(std::string("tp2")).getComponent<UiComponent>().setAnchor(scene->m_MainPanel->getHandle(), UiAlignment::INNER_LEFT, UiAlignment::INNER_TOP);
		//scene->getEntityByID(std::string("c")).getComponent<UiComponent>().visibility = UiVisibility::GONE;
	}

	Entity Scene::createEntity(const std::string& name) {
		Entity entity = { m_Registry.create(), this };
		m_Entities.push_back(entity);

		entity.addComponent<TransformComponent>();
		auto& tag = entity.addComponent<TagComponent>();
		tag.tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::onEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowResizeEvent>(SYR_BIND_EVENT_FN(Scene::onWindowResized));

		InputSystem::onEvent(e);
	}

	void Scene::onUpdate(Timestep ts) {
		//auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		//auto&[transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
		//for(auto entity : group) { Renderer2D::drawQuad(transform, sprite.color); }

		movement(m_Registry, ts);
		checkCollisions(m_Registry);


		positionComponents(m_Registry);

		InputSystem::processUserInputs(m_Registry, m_UiCamera.getCamera().getViewProjectionMatrix());

	}

	void Scene::onDraw() {

		//TODO move to a system

		auto view = m_Registry.view<LightComponent>();

		Ref<Shader> shader = Renderer::getShaderLibrary()->get("GrayShader");

		uint32_t i = 0;
		for (const entt::entity e : view) {
			LightComponent& light = view.get<LightComponent>(e);

			shader->bind();
			shader->setFloat3("u_Lights[" + std::to_string(i) + "].color", light.color);

			shader->setFloat3("u_Lights[" + std::to_string(i) + "].position", light.position);
			shader->setFloat3("u_Lights[" + std::to_string(i) + "].direction", glm::length(light.direction) != 0 ? glm::normalize(light.direction) : light.direction);

			shader->setFloat("u_Lights[" + std::to_string(i) + "].linearAttenuation", light.linearAttenuation);
			shader->setFloat("u_Lights[" + std::to_string(i) + "].quadraticAttenuation", light.quadraticAttenuation);

			shader->setFloat("u_Lights[" + std::to_string(i) + "].innerCutoff", light.innerCutoff);
			shader->setFloat("u_Lights[" + std::to_string(i) + "].outerCutoff", light.outerCutoff);

			i++;
		}

		shader->setInt("u_ActiveLightCount", view.size());

		auto meshView = m_Registry.view<MeshComponent>();

		//Set stencil operation
		RenderCommand::setStencilOperation(RendererAPI::STENCIL::KEEP, RendererAPI::STENCIL::REPLACE, RendererAPI::STENCIL::REPLACE);

		RenderCommand::setStencilMask(0x00);

		for (const entt::entity e : meshView) {
			if (!m_Registry.has<OutlineComponent>(e)) {
				Renderer::submit(shader, meshView.get<MeshComponent>(e).mesh, m_Registry.get<TransformComponent>(e).transform);
			}
		}

		for (const entt::entity e : meshView) {
			if (m_Registry.has<OutlineComponent>(e)) {

				//Enable stencil writing
				RenderCommand::setStencilFunction(RendererAPI::STENCIL::ALWAYS, 1, 0xFF);
				RenderCommand::setStencilMask(0xFF);
				RenderCommand::enableDepthBuffer();

				//Render object to be outlined
				Renderer::submit(shader, meshView.get<MeshComponent>(e).mesh, m_Registry.get<TransformComponent>(e).transform);

				//Disable stencil writing and depth testing
				RenderCommand::setStencilFunction(RendererAPI::STENCIL::NOTEQUAL, 1, 0xFF);
				RenderCommand::setStencilMask(0x00);
				RenderCommand::disableDepthBuffer();

				//Render a scaled version of the object, but only the fragments whose stencil values are not equal to 1
				Renderer::outline(Renderer::getShaderLibrary()->get("OutlineShader"), m_Registry.get<OutlineComponent>(e).scaledMesh, m_Registry.get<TransformComponent>(e).transform, 0.1f);

			}
		}

		//Reenable depth testing and restore stencil function
		RenderCommand::setStencilFunction(RendererAPI::STENCIL::ALWAYS, 1, 0xFF);
		RenderCommand::setStencilMask(0xFF);
		RenderCommand::enableDepthBuffer();

		//render(m_Registry);
		if (m_UiEnabled) {
			renderUi(m_Registry, m_MainPanel->m_EntityHandle);
		}

		//handleCollisions(m_Registry);

	}

	bool Scene::onWindowResized(WindowResizeEvent& e) {
		if (m_UiEnabled) {
			m_MainPanel->getComponent<TransformComponent>().scale({ ((float)e.getWidth() / (float)e.getHeight()) / m_AspectRatio, 1 });
		}

		m_AspectRatio = (float)e.getWidth() / (float)e.getHeight();

		return false;
	}

	bool Scene::entityIDExists(std::string& id) {
		for (std::vector<Entity>::iterator it = m_Entities.begin(); it != m_Entities.end(); ++it) {
			if (!it->getComponent<TagComponent>().id.empty() && it->getComponent<TagComponent>().id.compare(id) == 0) {
				return true;
			}
		}

		return false;
	}

	Entity Scene::getEntityByID(std::string& id) {
		for (std::vector<Entity>::iterator it = m_Entities.begin(); it != m_Entities.end(); ++it) {
			if (!it->getComponent<TagComponent>().id.empty() && it->getComponent<TagComponent>().id.compare(id) == 0) {
				return *it;
			}
		}

		SYR_CORE_WARN("No Entity had the ID: \"{0}\"", id);

		return {entt::null, nullptr};
	}
}