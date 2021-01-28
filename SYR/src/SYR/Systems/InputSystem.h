#pragma once

#include <entt.hpp>
#include <glm/glm.hpp>

#include "SYR/Events/ApplicationEvent.h"
#include "SYR/Events/MouseEvent.h"
#include "SYR/Events/KeyEvent.h"

namespace SYR {

	struct InputStack {
		float mx = 0;
		float my = 0;

		bool leftClick = false;

		std::map<int, int> keyPresses;
	};

	class InputSystem {
	public:

		static void processUserInputs(entt::registry& registry, const glm::mat4 viewProjection);

		static void setInputAreaDimensions(float width, float height) {
			s_WindowWidth = width;
			s_WindowHeight = height;
		}

		static void onEvent(Event& e);

		static glm::vec2 getNavigationVector() { return s_NavigationVector; }

	private:

		static bool onWindowResize(WindowResizeEvent& e);

		static bool onMouseMoved(MouseMovedEvent& e);
		static bool onMouseScrolled(MouseScrolledEvent& e);
		static bool onMouseButtonPressed(MouseButtonPressedEvent& e);
		static bool onMouseButtonReleased(MouseButtonReleasedEvent& e);

		static bool onKeyPressedEvent(KeyPressedEvent& e);
		static bool onKeyReleasedEvent(KeyReleasedEvent& e);

	private:
		inline static float s_WindowWidth = 1280, s_WindowHeight = 720;
		
		inline static InputStack s_InputStack;

		inline static glm::vec2 s_NavigationVector{ 0, 0 };
	};

}
