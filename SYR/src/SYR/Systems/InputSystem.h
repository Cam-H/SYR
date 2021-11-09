#pragma once

#include <entt.hpp>
#include <glm/glm.hpp>

#include <queue>

#include "SYR/Events/ApplicationEvent.h"
#include "SYR/Events/MouseEvent.h"
#include "SYR/Events/KeyEvent.h"

#include "SYR/Scene/Components.h"

namespace SYR {

	class InputSystem {
	public:

		static void processUserInputs(entt::registry& registry, const glm::mat4 viewProjection);

		static void setInputAreaDimensions(float width, float height) {
			s_WindowWidth = width;
			s_WindowHeight = height;
		}

		static void onEvent(Event& e);

		static bool isHovered(entt::registry& registry, entt::entity entity, glm::vec2 pointer);
		static entt::entity getHoveredEntity(entt::registry& registry);

	private:

		enum class InputType {
			MOUSE_MOVE = 0, MOUSE_PRESS, MOUSE_RELEASE, MOUSE_SCROLL, KEY_PRESS, KEY_RELEASE
		};

		typedef struct InputSet {

			InputType inputType;

			float mx;
			float my;

			int value;
			uint8_t repeats;
			uint8_t mods;


		} InputSet;

		static void addKey(TextComponent& tc, int key, uint8_t mods);
		
		static bool onWindowResize(WindowResizeEvent& e);

		static bool onMouseMoved(MouseMovedEvent& e);
		static bool onMouseScrolled(MouseScrolledEvent& e);
		static bool onMouseButtonPressed(MouseButtonPressedEvent& e);
		static bool onMouseButtonReleased(MouseButtonReleasedEvent& e);

		static bool onKeyPressedEvent(KeyPressedEvent& e);
		static bool onKeyReleasedEvent(KeyReleasedEvent& e);

	private:
		inline static float s_WindowWidth = 1280, s_WindowHeight = 720;
		
		inline static float s_Mx = 0, s_My = 0;

		inline static std::queue<InputSet> s_InputQueue;

		inline static glm::vec2 s_NavigationHeading { 0, 0 };
	};

}
