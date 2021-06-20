#include <SYR.h>

//************* Entry Point *************//
#include "SYR/Core/EntryPoint.h"
//***************************************//

#include <glm/gtc/matrix_transform.hpp>

#include "Platform/OpenGL/OpenGLShader.h"
#include "SYR/Renderer/Shader.h"
#include "SYR/Renderer/Renderer.h"
#include "SYR/Renderer/CharacterSet.h"

#include "Sandbox2D.h"
#include "Console.h"

class ExampleLayer : public SYR::Layer {
public:
	ExampleLayer() : Layer("Example") {

	}

	void onUpdate(SYR::Timestep ts) override {
		SYR_CORE_INFO(".VV");
	}

	void onEvent(SYR::Event& event) override {
		SYR::EventDispatcher dispatcher(event);
		dispatcher.dispatch<SYR::KeyPressedEvent>(SYR_BIND_EVENT_FN(ExampleLayer::onKeyPressedEvent));
	}

	bool onKeyPressedEvent(SYR::KeyPressedEvent& event) {
		return false;
	}

private:
};

class Sandbox : public SYR::Application {
public:
	Sandbox() {
		//pushLayer(new ExampleLayer());

		SYR::Renderer::getCharacterSetLibrary()->add("Arial24", SYR::CharacterSet::create("assets/fonts/arial.ttf", SYR::CharacterCollection::ASCII, 24));
		SYR::Renderer::getCharacterSetLibrary()->add("Arial36", SYR::CharacterSet::create("assets/fonts/arial.ttf", SYR::CharacterCollection::ASCII, 36));

		pushLayer(new Sandbox2D());
		pushOverlay(new Console());
		
		//getWindow().hideCursor();
	}

	~Sandbox() {

	}
};

SYR::Application* SYR::CreateApplication() {
	return new Sandbox();
}