#include "syrpch.h"
#include "Application.h"

#include "SYR/Renderer/Renderer.h"

#include "SYR/Core/Input.h"
#include "SYR/Core/KeyCodes.h"

#include <GLFW/glfw3.h>

namespace SYR {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application() {

		SYR_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::create());
		m_Window->setEventCallback(BIND_EVENT_FN(onEvent));

		Renderer::init();
		RenderCommand::setViewport(0, 0, m_Window->getWidth(), m_Window->getHeight());
	}

	Application::~Application() {

	}

	void Application::pushLayer(Layer* layer) {
		m_LayerStack.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Layer* layer) {
		m_LayerStack.pushOverlay(layer);
		layer->onAttach();
	}

	void Application::onEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(onWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
			(*--it)->onEvent(e);

			if (e.isHandled()) {
				break;
			}
		}
	}

	void Application::run() {

		while (m_Running) {

			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized) {
				for (Layer* layer : m_LayerStack) {
					layer->onUpdate(timestep);
				}
			}

			m_Window->onUpdate();
		}
	}

	void Application::stop() {
		m_Running = false;
	}

	bool Application::onWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}

	bool Application::onWindowResize(WindowResizeEvent& e) {

		if (e.getWidth() == 0 || e.getHeight() == 0) {
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;

		Renderer::onWindowResize(e.getWidth(), e.getHeight());

		return false;
	}
}