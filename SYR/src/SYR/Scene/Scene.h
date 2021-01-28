#pragma once

#include <entt.hpp>

#include "SYR/Events/ApplicationEvent.h"
#include "SYR/Events/MouseEvent.h"
#include "SYR/Events/KeyEvent.h"

#include "Components.h"
#include "SYR/Core/Timestep.h"

#include "SYR/CameraController.h"


namespace SYR {

	class Entity;

	class Scene {
	public:
		Scene();
		~Scene();

		void prepare();
		static void loadUi(Ref<Scene> scene, const std::string& filepath);

		Entity createEntity(const std::string& name = std::string());

		void onEvent(Event& e);
		void onUpdate(Timestep ts);
		void onDraw();

		bool entityIDExists(std::string& id);
		Entity getEntityByID(std::string& id);

		CameraController* getUiCamera() { return &m_UiCamera; }

	private:
		bool onWindowResized(WindowResizeEvent& e);
	private:
		entt::registry m_Registry;
		std::vector<Entity> m_Entities;

		float m_AspectRatio = 1280.0f / 720.0f;
		Ref<Entity> m_MainPanel;
		TransformComponent* m_Anchor;

		CameraController m_UiCamera;

		friend class Entity;

	};

}