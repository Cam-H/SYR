#pragma once

#include <entt.hpp>

#include "Scene.h"

namespace SYR {

	class Entity {
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& addComponent(Args&&... args) {
			SYR_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");
;			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		void removeComponent() {
			SYR_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");

			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		template<typename T>
		bool hasComponent() {
			return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		template<typename T>
		T& getComponent() {
			SYR_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");

			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		static T& getComponent(Scene* scene, entt::entity entityHandle) {
			SYR_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");

			return scene->m_Registry.get<T>(entityHandle);
		}

		template<typename T>
		static T& getComponent(entt::registry& registry, entt::entity entityHandle) {
			SYR_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");

			return registry.get<T>(entityHandle);
		}
		
		operator bool() const { return m_EntityHandle != entt::null; }

		entt::entity getHandle() {
			return m_EntityHandle;
		}

	private:
		static Ref<Entity> create(entt::entity handle, Scene* scene);
		friend class Scene;

		entt::entity m_EntityHandle { entt::null };
		Scene* m_Scene = nullptr;
	};

}