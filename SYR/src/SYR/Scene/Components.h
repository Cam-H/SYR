#pragma once

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Hitbox2D.h"
#include <entt.hpp>

namespace SYR {

	struct TagComponent {
		std::string tag;
		std::string id;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : tag(tag) {}
	};

	struct TransformComponent {
		glm::mat4 transform{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform) : transform(transform) {}

		void offset(glm::vec3 offset) {
			transform = transform * glm::translate(glm::mat4(1.0f), offset);
		}

		void scale(double scalar) {
			scale(glm::vec2(scalar, scalar));
		}

		void scale(glm::vec2 scale) {
			transform = transform * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		}

		void print() {
			SYR_CORE_INFO("{0}", glm::to_string(transform));
		}

		void setTransform(glm::mat4 t) {
			transform = t;
		}

		//operator glm::mat4& () { return transform; }
		//operator const glm::mat4& () { return transform; }
	};

	struct VelocityComponent {
		glm::vec3 velocity {0.0f, 0.0f, 0.0f};

		VelocityComponent() = default;
		VelocityComponent(const VelocityComponent&) = default;
		VelocityComponent(const glm::vec3& velocity) : velocity(velocity) {}

		void setVelocity(glm::vec3& velocity) {
			this->velocity = velocity;
		}

		operator glm::vec3& () { return velocity; }
		operator const glm::vec3& () { return velocity; }
	};

	struct SpriteRendererComponent {
		glm::vec4 color {1.0f, 1.0f, 1.0f, 1.0f};

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color) : color(color) {}

	};

	struct Hitbox2DComponent {
		
		Hitbox2D hitbox;

		Hitbox2DComponent() = default;
		Hitbox2DComponent(const Hitbox2DComponent&) = default;
		Hitbox2DComponent(Hitbox2D hitbox) : hitbox(hitbox) {}

		void applyNewTransform(glm::mat4& transform) {
			hitbox.applyNewTransform(transform);
		}

		void removeCollider() {
			hitbox.removeCollider();
		}

		void removeAllColliders() {
			hitbox.removeAllColliders();
		}

		void setCollider(entt::entity collider) {
			hitbox.setCollider(collider);
		}

		void render() {
			hitbox.render();
		}
	};

	enum class UiAlignment {
		FLOAT = 0,
		CENTER,
		OUTER_LEFT, INNER_LEFT, INNER_RIGHT, OUTER_RIGHT,
		OUTER_TOP, INNER_TOP, INNER_BOTTOM, OUTER_BOTTOM
	};
	enum class UiVisibility {
		VISIBLE = 0, HIDDEN, GONE
	};

	struct UiComponent {

		entt::entity anchorHandle = entt::null;

		UiAlignment horizontalAlignment = UiAlignment::FLOAT;
		UiAlignment verticalAlignment = UiAlignment::FLOAT;

		UiVisibility visibility = UiVisibility::VISIBLE;
		
		glm::vec4 baseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 highlightColor{ 0.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 selectColor{ 0.0f, 1.0f, 1.0f, 1.0f };

		UiComponent() = default;
		UiComponent(const UiComponent&) = default;

		void setAnchor(entt::entity anchorHandle, UiAlignment horizontalAlignment, UiAlignment verticalAlignment) {
			this->anchorHandle = anchorHandle;
			this->horizontalAlignment = horizontalAlignment;
			this->verticalAlignment = verticalAlignment;

		}


		//UiComponent()
	};

	struct TextComponent {

		std::string text;

		std::string characterSetName;
		glm::vec4 textColor{ 0.0f, 0.0f, 0.0f, 1.0f };

		TextComponent() = default;
		TextComponent(const TextComponent&) = default;
		TextComponent(std::string text, std::string characterSetName, glm::vec4 textColor = { 0.0f, 0.0f, 0.0f, 1.0f }) : text(text), characterSetName(characterSetName), textColor(textColor) {}
	};

	enum class Layout {
		FLOAT = 0, LINEAR_VERTICAL, LINEAR_HORIZONTAL, GRID
	};

	struct LayoutComponent {

		Layout layout;
		std::vector<entt::entity> entities;

		float margins;
		float spacing;

		glm::vec2 offset{ 0.0f, 0.0f };

		LayoutComponent() = default;
		LayoutComponent(const LayoutComponent&) = default;
		LayoutComponent(Layout layout, std::vector<entt::entity> entities) : layout(layout), entities(entities), margins(0.0f), spacing(0.0f) {}

		void setMargins(float margins) {
			this->margins = margins;
		}

		void setSpacing(float spacing) {
			this->spacing = spacing;
		}
	};

	struct ListenerComponent {

		bool listening;

		bool hovered;
		bool selected;
		bool checked;

		ListenerComponent() = default;
		ListenerComponent(const ListenerComponent&) = default;

	};
}