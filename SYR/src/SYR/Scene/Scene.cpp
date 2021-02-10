#include "syrpch.h"
#include "Scene.h"

#include "CollisionSystem.h"
#include "SYR/Systems/UiSystem.h"
#include "SYR/Systems/InputSystem.h"

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
		Entity entity = { m_Registry.create(), this };
		m_Entities.push_back(entity);
		
		auto& tag = entity.addComponent<TagComponent>();
		tag.tag = "Test Light";
		tag.id = "TL";

		//entity.addComponent<LightComponent>(glm::vec3(0.8f, 0.8f, 1), glm::vec3(1, 20, 0));
		//entity.addComponent<LightComponent>(glm::vec3(0.8f, 0.8f, 1), glm::vec3(1, 10, 0), 0.03f, 0.0064f);
		entity.addComponent<LightComponent>(glm::vec3(0, 1, 1), glm::vec3(15, 2, 15), 0.03f, 0.001f);

		entity = { m_Registry.create(), this };
		m_Entities.push_back(entity);

		entity.addComponent<TagComponent>().id = "TL2";
		entity.addComponent<LightComponent>(glm::vec3(1, 0.7f, 0), glm::vec3(0, 10, 0), glm::vec3(0, -1, 0.2f), 30, 30.5f);


		entity = { m_Registry.create(), this };
		m_Entities.push_back(entity);

		entity.addComponent<TagComponent>();
		entity.addComponent<LightComponent>(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, -1, 0));
	}

	void Scene::loadUi(Ref<Scene> scene, const std::string& filepath) {
		scene->m_UiEnabled = true;

		scene->m_MainPanel = Entity::create(UiSystem::loadPredefinedUi(scene.get(), filepath).getHandle(), scene.get());
		scene->m_MainPanel->getComponent<TransformComponent>().scale({ 2 * scene->m_AspectRatio, 2 });

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

		static float n = 0;
		m_Registry.get<LightComponent>(getEntityByID(std::string("TL2")).getHandle()).direction.x = cos(n += 0.01f) / 1.5f;
		m_Registry.get<LightComponent>(getEntityByID(std::string("TL2")).getHandle()).direction.z = sin(n += 0.01f) / 1.5f;

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

		for (const entt::entity e : meshView) {
			Renderer::submit(shader, meshView.get<MeshComponent>(e).mesh, m_Registry.get<TransformComponent>(e).transform);
		}

		//render(m_Registry);
		
		if (m_UiEnabled) {
			//renderUi(m_Registry, m_MainPanel->m_EntityHandle);
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

		SYR_CORE_WARN("No Entity had the ID: {0}", id);

		return {entt::null, nullptr};
	}
}