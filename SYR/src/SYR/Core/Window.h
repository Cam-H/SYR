#pragma once

#include "syrpch.h"

#include "SYR/Core/Core.h"
#include "SYR/Events/Event.h"
#include "SYR/Core/MouseCodes.h"

namespace SYR {

	struct WindowProps {
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "SYR Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
			: Title(title), Width(width), Height(height) {}
	};

	//Interface representing a desktop system based Window
	class SYR_API Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void onUpdate() = 0;

		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;

		//Window attributes
		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		virtual void showCursor(uint32_t cursor = SYR_ARROW_CURSOR) = 0;
		virtual void hideCursor() = 0;

		virtual void* getNativeWindow() const = 0;

		static Window* create(const WindowProps& props = WindowProps());
	};
}