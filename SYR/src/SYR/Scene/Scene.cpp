#include "syrpch.h"
#include "Scene.h"

#include "CollisionSystem.h"
#include "SYR/Systems/UiSystem.h"
#include "SYR/Systems/InputSystem.h"

#include "SYR/Renderer/Renderer2D.h"

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
		scene->m_MainPanel = Entity::create(UiSystem::loadPredefinedUi(scene.get(), filepath).getHandle(), scene.get());
		scene->m_MainPanel->getComponent<TransformComponent>().scale({ 2 * scene->m_AspectRatio, 2 });

		scene->getEntityByID(std::string("tp2")).getComponent<UiComponent>().setAnchor(scene->m_MainPanel->getHandle(), UiAlignment::INNER_LEFT, UiAlignment::INNER_TOP);
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

		render(m_Registry);
		renderUi(m_Registry, m_MainPanel->m_EntityHandle);

		handleCollisions(m_Registry);

	}

	bool Scene::onWindowResized(WindowResizeEvent& e) {
		m_MainPanel->getComponent<TransformComponent>().scale({ ((float)e.getWidth() / (float)e.getHeight()) / m_AspectRatio, 1 });
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