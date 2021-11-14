#pragma once

#include <SYR.h>

class CollisionSandbox : public SYR::Layer{
public:
	CollisionSandbox();
	virtual ~CollisionSandbox() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(SYR::Timestep ts) override;
	void onEvent(SYR::Event & e) override;
private:

	SYR::Ref<SYR::Shader> m_MeshShader;

	SYR::Ref<SYR::CharacterSet> m_CharacterSet;

	SYR::Ref<SYR::Scene> m_ActiveScene;

	SYR::CameraController m_CameraController;
	SYR::CameraController m_PCameraController;

	SYR::Ref<SYR::ColliderLibrary> m_CL;

};