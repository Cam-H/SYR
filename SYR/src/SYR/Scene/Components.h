#pragma once

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "SYR/Renderer/VertexArray.h"
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

		void rotate(float rotation, glm::vec3 axis) {
			transform = transform * glm::rotate(glm::mat4(1.0f), rotation, axis);
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
		glm::vec3 velocity{ 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };

		VelocityComponent() = default;
		VelocityComponent(const VelocityComponent&) = default;
		VelocityComponent(const glm::vec3& velocity) : velocity(velocity) {}
		VelocityComponent(const glm::vec3& velocity, const glm::vec3& rotation) : velocity(velocity), rotation(rotation) {}

		void setVelocity(glm::vec3& velocity) {
			this->velocity = velocity;
		}

		operator glm::vec3& () { return velocity; }
		operator const glm::vec3& () { return velocity; }
	};

	enum class LightType {
		POINT = 0, DIRECTION, SPOT
	};

	struct LightComponent {
		LightType lightType;

		glm::vec3 color;

		glm::vec3 position;
		glm::vec3 direction;

		float linearAttenuation;
		float quadraticAttenuation;

		float innerCutoff;
		float outerCutoff;

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;

		//Direction light definition
		LightComponent(glm::vec3 color, glm::vec3 direction) {
			this->color = color;
			//position = glm::normalize(direction) * std::numeric_limits<float>::max();
			this->direction = glm::normalize(direction);
			position = -direction * 100000.0f;

			linearAttenuation = quadraticAttenuation = 0;
			innerCutoff = outerCutoff = -1;

			lightType = LightType::DIRECTION;
		}

		//Point light definition
		LightComponent(glm::vec3 color, glm::vec3 position, float linearAttenuation, float quadraticAttenuation) : color(color), position(position), direction({ 0.0f, 0.0f, 0.0f }), linearAttenuation(linearAttenuation), quadraticAttenuation(quadraticAttenuation), innerCutoff(-1), outerCutoff(-1), lightType(LightType::POINT) {}

		//Spot light definition
		LightComponent(glm::vec3 color, glm::vec3 position, glm::vec3 direction, float innerAngle, float outerAngle) {
			this->color = color;
			this->position = position;
			this->direction = glm::normalize(direction);

			linearAttenuation = quadraticAttenuation = 0;

			innerCutoff = cos(glm::radians(innerAngle));
			outerCutoff = cos(glm::radians(outerAngle));

			lightType = LightType::SPOT;
		}

		void moveTo(glm::vec3 newPos) {
			position = newPos;
		}

		void setAttenuation(float linearAttenuation, float quadraticAttenuation) {
			this->linearAttenuation = linearAttenuation;
			this->quadraticAttenuation = quadraticAttenuation;
		}

		void setSpotlight(glm::vec3 direction, float innerAngle, float outerAngle) {
			this->direction = direction;

			innerCutoff = cos(glm::radians(innerAngle));
			outerCutoff = cos(glm::radians(outerAngle));
		}
	};

	struct MeshComponent {

		Ref<VertexArray> mesh;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(Ref<VertexArray> mesh) : mesh(mesh) {}
	};

	struct OutlineComponent {

		Ref<VertexArray> scaledMesh;

		OutlineComponent() = default;
		OutlineComponent(const OutlineComponent&) = default;
		OutlineComponent(Ref<VertexArray> scaledMesh) : scaledMesh(scaledMesh) {}
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

	enum class Alignment {
		FLOAT = 0,
		CENTER,
		OUTER_LEFT, INNER_LEFT, INNER_RIGHT, OUTER_RIGHT,
		OUTER_TOP, INNER_TOP, INNER_BOTTOM, OUTER_BOTTOM
	};
	enum class UiVisibility {
		VISIBLE = 0, HIDDEN, GONE
	};

	struct AnchorComponent {

		entt::entity horizontalAnchorHandle = entt::null;
		entt::entity verticalAnchorHandle = entt::null;

		//Necessary to initialize the anchor when loading anchor data from a file since entities are initialized too early 
		std::string horizontalAnchorHandleID;
		std::string verticalAnchorHandleID;

		Alignment horizontalAlignment = Alignment::FLOAT;
		Alignment verticalAlignment = Alignment::FLOAT;
		
		//TODO add anchor offsets

		AnchorComponent() = default;
		AnchorComponent(const AnchorComponent&) = default;

		//AnchorComponent(entt::entity anchorHandle, UiAlignment horizontalAlignment, UiAlignment verticalAlignment) : anchorHandle(anchorHandle), horizontalAlignment(horizontalAlignment), verticalAlignment(verticalAlignment) {}

		void setHorizontalAnchor(entt::entity anchorHandle, Alignment horizontalAlignment) {
			this->horizontalAnchorHandle = anchorHandle;
			this->horizontalAlignment = horizontalAlignment;
		}

		void setHorizontalAnchor(std::string anchorHandleID, Alignment horizontalAlignment) {
			this->horizontalAnchorHandleID = anchorHandleID;
			this->horizontalAlignment = horizontalAlignment;

#ifdef _DEBUG
			SYR_CORE_WARN("Horizontal anchor handle ID set. The entity reference still needs to be created");
#endif
		}

		void setVerticalAnchor(entt::entity anchorHandle, Alignment verticalAlignment) {
			this->verticalAnchorHandle = anchorHandle;
			this->verticalAlignment = verticalAlignment;
		}

		void setVerticalAnchor(std::string anchorHandleID, Alignment verticalAlignment) {
			this->verticalAnchorHandleID = anchorHandleID;
			this->verticalAlignment = verticalAlignment;

#ifdef _DEBUG
			SYR_CORE_WARN("Horizontal anchor handle ID set. The entity reference still needs to be created");
#endif
		}
	};

	struct UiComponent {

		UiVisibility visibility = UiVisibility::VISIBLE;
		
		glm::vec4 baseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 highlightColor{ 0.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 selectColor{ 0.0f, 1.0f, 1.0f, 1.0f };

		UiComponent() = default;
		UiComponent(const UiComponent&) = default;

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