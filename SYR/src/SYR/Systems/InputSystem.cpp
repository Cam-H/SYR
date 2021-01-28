#include "syrpch.h"
#include "InputSystem.h"
#include "UiSystem.h"

#include "SYR/Scene/Components.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace SYR {

	void InputSystem::processUserInputs(entt::registry& registry, const glm::mat4 viewProjection) {
		auto view = registry.view<TransformComponent, ListenerComponent>();

		//x2 as that is applied by quadVertexBase in Renderer2D
		float rx = (s_InputStack.mx - s_WindowWidth / 2) / s_WindowWidth * 2.0f;
		float ry = -(s_InputStack.my - s_WindowHeight / 2) / s_WindowHeight * 2.0f;

		float aspectRatio = s_WindowWidth / s_WindowHeight;

		for (const entt::entity e : view) {
			if (registry.has<UiComponent>(e)) {
				//continue;
			}

			
			if (s_InputStack.leftClick) {
				if (view.get<ListenerComponent>(e).hovered) {
					view.get<ListenerComponent>(e).selected = true;
				}
			} else {

				if (registry.has<Hitbox2DComponent>(e)) {

				}

				glm::mat4& transform = view.get<TransformComponent>(e).transform;

				glm::vec3 scale;
				glm::vec3 translation;
				glm::decompose(transform * viewProjection, scale, glm::quat(), translation, glm::vec3(), glm::vec4());

				view.get<ListenerComponent>(e).hovered = abs(rx - translation.x / aspectRatio) < abs(scale.x / 2) && abs(ry - translation.y) < abs(scale.y / 2);

				if (view.get<ListenerComponent>(e).selected) {
					if (view.get<ListenerComponent>(e).hovered) {
						view.get<ListenerComponent>(e).checked = !view.get<ListenerComponent>(e).checked;
					}
					view.get<ListenerComponent>(e).selected = false;
				}


			}
		}

		UiSystem::processUiInputs(registry);

	}

	void InputSystem::onEvent(Event& e) {
		EventDispatcher dispatcher(e);

		dispatcher.dispatch<WindowResizeEvent>(std::bind(InputSystem::onWindowResize, std::placeholders::_1));

		dispatcher.dispatch<MouseMovedEvent>(std::bind(InputSystem::onMouseMoved, std::placeholders::_1));
		dispatcher.dispatch<MouseScrolledEvent>(std::bind(InputSystem::onMouseScrolled, std::placeholders::_1));
		dispatcher.dispatch<MouseButtonPressedEvent>(std::bind(InputSystem::onMouseButtonPressed, std::placeholders::_1));
		dispatcher.dispatch<MouseButtonReleasedEvent>(std::bind(InputSystem::onMouseButtonReleased, std::placeholders::_1));

		dispatcher.dispatch<KeyPressedEvent>(std::bind(InputSystem::onKeyPressedEvent, std::placeholders::_1));
		dispatcher.dispatch<KeyReleasedEvent>(std::bind(InputSystem::onKeyReleasedEvent, std::placeholders::_1));
	}

	bool InputSystem::onWindowResize(WindowResizeEvent& e) {
		s_WindowWidth = e.getWidth();
		s_WindowHeight = e.getHeight();
		return false;
	}

	bool InputSystem::onMouseMoved(MouseMovedEvent& e) {
		s_InputStack.mx = e.getX();
		s_InputStack.my = e.getY();
		return false;
	}

	bool InputSystem::onMouseScrolled(MouseScrolledEvent& e) {
		SYR_CORE_INFO("{0} {1}", e.getXOffset(), e.getYOffset());
		return false;
	}

	bool InputSystem::onMouseButtonPressed(MouseButtonPressedEvent& e) {
		SYR_CORE_INFO(e.getMouseButton());
		s_InputStack.leftClick = e.getMouseButton() == 0;

		switch (e.getMouseButton()) {
		case 0:
			s_InputStack.leftClick = true;
			break;
		}

		return false;
	}

	bool InputSystem::onMouseButtonReleased(MouseButtonReleasedEvent& e) {
		SYR_CORE_INFO(e.getMouseButton());

		switch (e.getMouseButton()) {
		case 0:
			s_InputStack.leftClick = false;
			break;
		}

		return false;
	}

	bool InputSystem::onKeyPressedEvent(KeyPressedEvent& e) {
		SYR_CORE_INFO("F {0} {1}", e.getKeyCode(), e.getRepeatCount());

		std::map<int, int>::iterator it = s_InputStack.keyPresses.find(e.getKeyCode());
		if (it == s_InputStack.keyPresses.end()) {
			s_InputStack.keyPresses.insert(std::pair<int, int>(e.getKeyCode(), 1));
		} else {
			it->second += e.getRepeatCount();
		}

		return false;
	}

	bool InputSystem::onKeyReleasedEvent(KeyReleasedEvent& e) {
		//s_NavigationVector = { 0, 0 };
		SYR_CORE_INFO(e.getKeyCode());
		return false;
	}

}