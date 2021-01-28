#pragma once

#include "syrpch.h"
#include "SYR/Core/Input.h"

#include "SYR/Core/Application.h"

#include <GLFW/glfw3.h>


namespace SYR {

	Input* Input::s_Instance = new Input();

	bool Input::isKeyPressedImpl(int keycode) {
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		auto state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::isMouseButtonPressedImpl(int button) {
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		auto state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::getMousePositionImpl() {
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::getMouseXImpl() {
		auto [x, y] = getMousePositionImpl();
		return x;
	}

	float Input::getMouseYImpl() {
		auto [x, y] = getMousePositionImpl();
		return y;
	}
}