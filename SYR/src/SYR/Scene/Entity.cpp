#include "syrpch.h"
#include "Entity.h"

namespace SYR {
	Entity::Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Scene(scene) {}

	Ref<Entity> Entity::create(entt::entity handle, Scene* scene) {
		return createRef<Entity>(handle, scene);
	}
}