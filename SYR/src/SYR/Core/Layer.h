#pragma once

#include "SYR/Core/Core.h"
#include "SYR/Core/Timestep.h"

#include "SYR/Events/Event.h"

#include "SYR/Events/ApplicationEvent.h"
#include "SYR/Events/KeyEvent.h"
#include "SYR/Events/MouseEvent.h"

namespace SYR {

	class SYR_API Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(Timestep ts) {}
		virtual void onEvent(Event& event) {}

		inline const std::string& getName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}

