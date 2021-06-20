#pragma once

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "SYR/Renderer/VertexArray.h"
#include "Hitbox2D.h"
#include <entt.hpp>

namespace SYR {

	enum class Status {
		VISIBLE = 0, HIDDEN, GONE
	};

	struct TagComponent {
		std::string tag;
		std::string id;

		entt::entity parent = entt::null;

		Status status = Status::VISIBLE;

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
		HORIZONTAL, VERTICAL,
		OUTER_LEFT, INNER_LEFT, INNER_RIGHT, OUTER_RIGHT,
		OUTER_TOP, INNER_TOP, INNER_BOTTOM, OUTER_BOTTOM
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
		
		glm::vec4 baseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 highlightColor{ 0.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 selectColor{ 0.0f, 1.0f, 1.0f, 1.0f };

		UiComponent() = default;
		UiComponent(const UiComponent&) = default;
	};

	struct TextComponent {

		std::string text;
		std::string placeholderText;

		std::string characterSetName;
		glm::vec4 textColor{ 0.0f, 0.0f, 0.0f, 1.0f };

		Alignment alignment = Alignment::CENTER;
		bool hypertextEnabled = false;

		uint16_t characterLimit = 256;
		bool editable = false;

		TextComponent() = default;
		TextComponent(const TextComponent&) = default;
		TextComponent(std::string text, std::string characterSetName, glm::vec4 textColor = { 0.0f, 0.0f, 0.0f, 1.0f }) : text(text), characterSetName(characterSetName), textColor(textColor) {}
	};

	enum class Layout {
		FLOAT = 0, LINEAR, GRID
	};

	struct LayoutComponent {

		Layout layout;
		Alignment alignment;

		std::vector<entt::entity> entities;
		entt::entity selectedEntity;

		float margins;
		float spacing;

		bool forceResize = true;


		int cols;
		int rows;

		float contentWidth = -1.0f;
		float contentHeight = -1.0f;

		bool restrictRenderingToBounds = false;

		bool square = false;

		bool scrollable = false;
		glm::vec2 contentLimits{ 0, 0 };
		float offset = 0;

		bool wrap = false;

		LayoutComponent() = default;
		LayoutComponent(const LayoutComponent&) = default;
		LayoutComponent(Layout layout, Alignment alignment, std::vector<entt::entity> entities) {
			this->layout = layout;
			this->alignment = Alignment::VERTICAL;

			this->entities = entities;

			margins = spacing = 0;

			switch (alignment) {
			case Alignment::FLOAT: case Alignment::CENTER:
				SYR_CORE_WARN("Invalid layout alignment: \"{0}\". Defaulting to a vertical alignment", alignment);
				break;
			case Alignment::OUTER_LEFT: case Alignment::INNER_LEFT: case Alignment::INNER_RIGHT: case Alignment::OUTER_RIGHT:
				SYR_CORE_WARN("Using layout alignment \"{0}\" defaults to a horizontal alignment");
				[[fallthrough]];
			case Alignment::HORIZONTAL:
				this->alignment = Alignment::HORIZONTAL;
				break;
			case Alignment::OUTER_TOP: case Alignment::INNER_TOP: case Alignment::INNER_BOTTOM: case Alignment::OUTER_BOTTOM:
				SYR_CORE_WARN("Using layout alignment \"{0}\" defaults to a vertical alignment");
				[[fallthrough]];
			case Alignment::VERTICAL:
				this->alignment = Alignment::VERTICAL;
				break;
			}
		}

		void setScrollable(bool scrollable) {
			this->scrollable = scrollable;
		}

		void setMargins(float margins) {
			this->margins = margins;
		}

		void setSpacing(float spacing) {
			this->spacing = spacing;
		}
	};

	struct IOListenerComponent {

		const glm::vec2* vertices;
		uint16_t vertexCount;

		bool keyListener = true;
		bool mouseListener = true;

		bool interactable = true;
		
		bool hovered = false;
		bool selected = false;
		bool checked = false;

		IOListenerComponent() = default;
		IOListenerComponent(const IOListenerComponent&) = default;
		IOListenerComponent(const glm::vec2 vertices[], uint16_t vertexCount) : vertices(vertices), vertexCount(vertexCount) {}

		inline glm::vec2* getTransformedVertices(glm::mat4& transform) {
			glm::vec2* transformedVertices = new glm::vec2[vertexCount];

			for (int i = 0; i < vertexCount; i++) {
				glm::vec2 baseVertex = vertices[i];
				glm::vec4 vertex = { baseVertex.x, baseVertex.y, 1.0f, 1.0f };
				glm::vec3 pos = transform * vertex;

				transformedVertices[i] = { pos.x, pos.y };
			}

			return transformedVertices;
		}

	};
}