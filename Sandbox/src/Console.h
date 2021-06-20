#pragma once

#include <SYR.h>

class Console : public SYR::Layer {
public:
	Console();
	virtual ~Console() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(SYR::Timestep ts) override;
	void onEvent(SYR::Event& e) override;
private:

	SYR::Ref<SYR::Scene> m_ActiveScene;
	SYR::CameraController m_CameraController;

};

