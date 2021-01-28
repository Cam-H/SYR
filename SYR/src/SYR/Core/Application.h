#pragma once

#include "Core.h"
#include "SYR/Core/LayerStack.h"
#include "SYR/Events/Event.h"
#include "SYR/Events/ApplicationEvent.h"

#include "Window.h"

#include "SYR/Renderer/Shader.h"
#include "SYR/Renderer/Buffer.h"
#include "SYR/Renderer/VertexArray.h"

#include "SYR/Core/Timestep.h"

#include "SYR/Renderer/OrthographicCamera.h"

namespace SYR {

	class SYR_API Application{
	public:
		Application();
		virtual ~Application();

		void run();
		void stop();

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);

		inline static Application& get() { return *s_Instance; }
		inline Window& getWindow() { return *m_Window; }
	private:
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResize(WindowResizeEvent& e);
	private:

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		static Application* s_Instance;

		Timestep m_Timestep;
		float m_LastFrameTime = 0.0f;
	};

	//To be defined in CLIENT
	Application* CreateApplication();
}

