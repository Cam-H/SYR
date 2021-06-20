#include "Console.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <SYR/Core/Application.h>
#include "SYR/Renderer/Renderer.h"


Console::Console() : Layer("Console"), m_CameraController(SYR::CameraType::ORTHOGRAPHIC, SYR::CameraControlType::LOCKED_CAMERA, 1280.0f / 720.0f) {
	//SYR::PerspectiveCamera pcam = SYR::PerspectiveCamera(1280.0f / 720.0f);
}

void Console::onAttach() {

	std::vector<uint32_t> codes(0);
	SYR::CharacterSet::addCharactersToSet(SYR::CharacterCollection::ASCII, &codes);

	m_ActiveScene = SYR::createRef<SYR::Scene>();
	m_ActiveScene->prepare();

	SYR::Scene::loadUi(m_ActiveScene, "assets/ui/Console.xml");
	m_ActiveScene->disableUi();

}

void Console::onDetach() {}

void Console::onUpdate(SYR::Timestep ts) {
	//Update scene and do rendering

	m_CameraController.onUpdate(ts);

	static bool tildaPressed = false;
	static bool uiEnabled = false;

	if (SYR::Input::isKeyPressed(SYR_KEY_GRAVE_ACCENT)) {

		if (!tildaPressed) {
			if (uiEnabled) {
				m_ActiveScene->disableUi();
			} else {
				m_ActiveScene->enableUi();
			}

			uiEnabled = !uiEnabled;
		}

		tildaPressed = true;
	}
	else {
		tildaPressed = false;
	}

	//SYR::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	//SYR::RenderCommand::clear();

	SYR::Renderer2D::beginScene(m_CameraController.getCamera());

	m_ActiveScene->onUpdate(ts);
	m_ActiveScene->onDraw();

	SYR::Renderer2D::endScene();

}

void Console::onEvent(SYR::Event& e) {
	m_CameraController.onEvent(e);
	m_ActiveScene->onEvent(e);
}