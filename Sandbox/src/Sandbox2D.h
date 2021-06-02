#pragma once

#include <SYR.h>

class Sandbox2D : public SYR::Layer{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(SYR::Timestep ts) override;
	void onEvent(SYR::Event& e) override;
private:
	//SYR::Ref<SYR::Shader> m_MeshShader;

	SYR::Ref<SYR::Shader> m_MeshShader;

	SYR::Ref<SYR::Texture2D> m_Texture;

	SYR::Ref<SYR::VertexArray> m_SquareVA;

	SYR::Ref<SYR::Texture2D> m_SpriteSheet;
	SYR::Ref<SYR::SubTexture2D> m_SubTexture;
	SYR::Ref<SYR::CharacterSet> m_CharacterSet;

	SYR::Ref<SYR::FrameBuffer> m_FB;

	SYR::Ref<SYR::Scene> m_ActiveScene;
	SYR::Entity entity;

	SYR::CameraController m_CameraController;
	SYR::CameraController m_PCameraController;

};

