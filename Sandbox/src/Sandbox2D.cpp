#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <SYR/Core/Application.h>


Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(SYR::CameraType::ORTHOGRAPHIC, SYR::CameraControlType::LOCKED_CAMERA, 1280.0f / 720.0f), m_PCameraController(SYR::CameraType::PERSPECTIVE, SYR::CameraControlType::LOCKED_CAMERA, 1280.0f / 720.0f) {
	//SYR::PerspectiveCamera pcam = SYR::PerspectiveCamera(1280.0f / 720.0f);
}

void Sandbox2D::onAttach() {
	m_Texture = SYR::Texture2D::create("assets/textures/Checkerboard.png");
	m_SpriteSheet = SYR::Texture2D::create("assets/textures/RPGpack_sheet.png");
	m_SubTexture = SYR::SubTexture2D::createFromIndex(m_SpriteSheet, glm::vec2(0, 4), glm::vec2(64, 64), glm::vec2(2, 3));

	std::vector<uint32_t> codes(0);

	/*
	std::string tempp = SYR::readFile("assets/2.txt");

	const char* delimiter = "\n";
	size_t delimLength = strlen(delimiter);

	size_t pos = 0;
	size_t prev = 0;



	while ((pos = tempp.find(delimiter, prev)) != std::string::npos) {

		std::string line = tempp.substr(prev, pos - prev);
		int i = line.find(' ');
		int j = line.find_last_of(' ');

		std::string kanji = line.substr(0, i);
		std::string hiragana = line.substr(i + 1, j - i - 1);
		std::string eng = line.substr(j + 1, line.length() - j);


		SYR_CORE_ERROR("-> {0} {1} {2}: ", 0x4E0B, eng, kanji);
		for (int i = 0; i < kanji.length(); i++) {
			SYR_CORE_INFO("{0}", (uint16_t)kanji.at(i));
		}

		prev = pos + 1;
	}

	SYR_CORE_WARN("{0}", tempp);
	SYR_CORE_INFO("{0} {1}", '上', (uint32_t)'上');

	*/

	struct Card {
		std::vector<uint32_t> symbols;
		std::vector<uint32_t> translation;
		std::vector<uint32_t> source;
	};

	std::vector byteData = SYR::FileHandler::parseUTF8File("assets/2.txt");

	int channel = 0;

	std::vector<Card> cards(0);
	Card card{};

	for (std::vector<uint32_t>::iterator it = byteData.begin(); it != byteData.end(); ++it) {

		switch (channel) {
		case 0:
			if (*it == 32) {
				channel = 1;
			}
			else {
				card.symbols.push_back(*it);
			}

			break;
		case 1:
			if (*it == 32) {
				channel = 2;
			}
			else {
				card.translation.push_back(*it);
			}

			break;
		case 2:
			if (*it == 13) {
				it++;

				channel = 0;

				cards.push_back(card);
				card = {};
			} else {
				card.source.push_back(*it);
			}

			break;
		}
	}

	SYR_CORE_TRACE(cards.size());

	for (std::vector<Card>::iterator it = cards.begin(); it != cards.end(); ++it) {
		for (std::vector<uint32_t>::iterator n = it->symbols.begin(); n != it->symbols.end(); ++n) {
			//codes.push_back(*n);
		}
	}

	SYR::CharacterSet::addCharactersToSet(SYR::CharacterCollection::ASCII, &codes);
	//SYR::CharacterSet::addCharactersToSet(SYR::CharacterCollection::HIRAGANA, &codes);
	//SYR::CharacterSet::addCharactersToSet(SYR::CharacterCollection::KATAKANA, &codes);

	//m_CharacterSet = SYR::CharacterSet::create("assets/fonts/kochi-mincho.ttf", SYR::A::NONE);
	m_CharacterSet = SYR::CharacterSet::create("assets/fonts/arial.ttf", codes, 24);

	SYR::FrameBufferSpecification spec;
	spec.width = 1280;
	spec.height = 720;

	m_FB = SYR::FrameBuffer::create(spec);

	m_ActiveScene = SYR::createRef<SYR::Scene>();
	m_ActiveScene->prepare();

	
	//m_CameraController = *m_ActiveScene->getUiCamera();
	//m_CameraController.setZoomLevel(2.0f);
	//m_ActiveScene->getUiCamera()->setZoomLevel(2);

	/*
	SYR::Entity temp = m_ActiveScene->createEntity();
	temp.getComponent<SYR::TransformComponent>().offset(glm::vec3(-1.2f, 0.5f, 0.0f));

	glm::vec2* body = new glm::vec2[4];

	body = new glm::vec2[4];
	body[0] = glm::vec2(0.5f, 0.0f);
	body[1] = glm::vec2(1.0f, 0.5f);
	body[2] = glm::vec2(0.5f, 1.0f);
	body[3] = glm::vec2(0.0f, 0.5f);

	//temp.addComponent<SYR::Hitbox2DComponent>(SYR::Hitbox2D(body, 4));

	entity = m_ActiveScene->createEntity();
	entity.addComponent<SYR::VelocityComponent>(glm::vec3(0.1f, 0.0f, 0.0f));

	body = new glm::vec2[4];
	body[0] = glm::vec2(0.0f, 0.0f);
	body[1] = glm::vec2(3.2f, 0.0f);
	body[2] = glm::vec2(1.0f, 1.2f);
	body[3] = glm::vec2(0.0f, 1.0f);

	entity.addComponent<SYR::Hitbox2DComponent>(SYR::Hitbox2D(body, 4));
	
	body = new glm::vec2[4];
	body[0] = glm::vec2(4.5f, 0.0f);
	body[1] = glm::vec2(5.0f, 0.5f);
	body[2] = glm::vec2(4.5f, 1.0f);
	body[3] = glm::vec2(4.0f, 0.5f);

	SYR::Hitbox2D a = SYR::Hitbox2D(body, 4);

	body = new glm::vec2[4];
	body[0] = glm::vec2(0.5f, 0.0f);
	body[1] = glm::vec2(1.0f, 0.5f);
	body[2] = glm::vec2(0.5f, 1.9f);
	body[3] = glm::vec2(0.0f, 0.5f);

	SYR::Hitbox2D b = SYR::Hitbox2D(body, 4);

	SYR::Hitbox2D hitboxes = SYR::Hitbox2D(std::vector<SYR::Hitbox2D> {a, b});

	temp = m_ActiveScene->createEntity();
	temp.addComponent<SYR::Hitbox2DComponent>(hitboxes);
	temp.getComponent<SYR::TransformComponent>().offset(glm::vec3(0.0f, 3.0f, 0.0f));
	*/

	SYR::Renderer::getShaderLibrary()->load("MeshShader", "assets/shaders/B3D.glsl");
	SYR::Renderer::getShaderLibrary()->load("GrayShader", "assets/shaders/Gray.glsl");
	SYR::Renderer::getShaderLibrary()->load("OutlineShader", "assets/shaders/Highlight.glsl");

	//m_MeshShader = SYR::Shader::create("assets/shaders/B3D.glsl");
	SYR::Entity e = m_ActiveScene->createEntity();
	e.getComponent<SYR::TagComponent>().id = "SPH1";
	e.getComponent<SYR::TransformComponent>().offset({ -10, 5, -5 });
	e.addComponent<SYR::MeshComponent>(SYR::FileHandler::loadMesh("assets/meshes/Screen.obj"));

	e = m_ActiveScene->createEntity();
	e.getComponent<SYR::TagComponent>().id = "TORUS";
	e.getComponent<SYR::TransformComponent>().offset({ 0, -5, 0 });
	e.addComponent<SYR::VelocityComponent>(glm::vec3(0, 0, 0), glm::vec3(glm::radians(50.0f), glm::radians(90.0f), 0));
	e.addComponent<SYR::MeshComponent>(SYR::FileHandler::loadMesh("assets/meshes/Torus.obj"));
	e.addComponent<SYR::OutlineComponent>(SYR::FileHandler::loadMesh("assets/meshes/Torus.obj", true));


	e = m_ActiveScene->createEntity();
	e.getComponent<SYR::TagComponent>().id = "SPH2";
	e.getComponent<SYR::TransformComponent>().offset({ 0, 0, 10 });
	e.addComponent<SYR::VelocityComponent>(glm::vec3(0, 0, 0), glm::vec3(glm::radians(50.0f), glm::radians(90.0f), 0));
	e.addComponent<SYR::MeshComponent>(SYR::FileHandler::loadMesh("assets/meshes/Screen.obj"));

	e = m_ActiveScene->createEntity();
	e.getComponent<SYR::TagComponent>().id = "SWORD";
	e.addComponent<SYR::MeshComponent>(SYR::FileHandler::loadMesh("assets/meshes/LeafSword.obj"));
	e.addComponent<SYR::OutlineComponent>(SYR::FileHandler::loadMesh("assets/meshes/LeafSword.obj", true));
	e.getComponent<SYR::TransformComponent>().offset({ 15, 8, 0 });
	e.addComponent<SYR::VelocityComponent>(glm::vec3(0, 0, 0), glm::vec3(0, glm::radians(90.0f), 0));

	e = m_ActiveScene->createEntity();
	e.getComponent<SYR::TagComponent>().id = "FLOOR";
	e.getComponent<SYR::TransformComponent>().offset({ 0, -5, 0 });
	e.addComponent<SYR::MeshComponent>(SYR::FileHandler::loadMesh("assets/meshes/Floor.obj"));

	/*************************************************LIGHTS*************************************************/
	
	e = m_ActiveScene->createEntity();
	e.getComponent<SYR::TagComponent>().tag = "Test Light";
	e.getComponent<SYR::TagComponent>().id = "TL";
	e.addComponent<SYR::LightComponent>(glm::vec3(0, 1, 1), glm::vec3(15, 2, 15), 0.03f, 0.001f);

	e = m_ActiveScene->createEntity();
	e.getComponent<SYR::TagComponent>().id = "TL2";
	e.addComponent<SYR::LightComponent>(glm::vec3(1, 0.7f, 0), glm::vec3(0, 10, 0), glm::vec3(0, -1, 0.2f), 30, 30.5f);

	e = m_ActiveScene->createEntity();
	e.getComponent<SYR::TagComponent>().id = "TL3";
	e.addComponent<SYR::LightComponent>(glm::vec3(0, 0, 1.0f), glm::vec3(0, 10, 0), glm::vec3(0, -1, 0.2f), 15, 16);

	e = m_ActiveScene->createEntity();
	e.getComponent<SYR::TagComponent>().id = "TL4";
	e.addComponent<SYR::LightComponent>(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, -1, 0));


	SYR::Scene::loadUi(m_ActiveScene, "assets/ui/TestUI1.xml");
	//SYR::UiSystem::loadPredefinedUi(m_ActiveScene, "assets/ui/TestUI1.xml");

}

void Sandbox2D::onDetach() {
	
}

void Sandbox2D::onUpdate(SYR::Timestep ts) {

	//SYR::Timer timer("Sandbox2D::onUpdate");

	//Update scene and do rendering

	m_CameraController.onUpdate(ts);
	m_PCameraController.onUpdate(ts);

	glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };

	//static glm::vec3 pos = { 0.0f, 0.0f, 0.0f };

	if (SYR::Input::isKeyPressed(SYR_KEY_UP)) {
		velocity.y += 0.01;
	}

	if (SYR::Input::isKeyPressed(SYR_KEY_DOWN)) {
		velocity.y -= 0.01;
	}

	if (SYR::Input::isKeyPressed(SYR_KEY_LEFT)) {
		velocity.x -= 0.01;
	}

	if (SYR::Input::isKeyPressed(SYR_KEY_RIGHT)) {
		velocity.x += 0.01;
	}

	if (SYR::Input::isKeyPressed(SYR_KEY_N)) {
		velocity.z += 0.01;
	}

	if (SYR::Input::isKeyPressed(SYR_KEY_M)) {
		velocity.z -= 0.01;
	}


	static bool escPressed = false;

	if (SYR::Input::isKeyPressed(SYR_KEY_ESCAPE)) {

		if (!escPressed) {
			if (m_PCameraController.getCameraControlType() == SYR::CameraControlType::FREE_CAMERA) {
				m_PCameraController.setCameraControlType(SYR::CameraControlType::LOCKED_CAMERA);
				m_ActiveScene->enableUi();
			}
			else {
				m_PCameraController.setCameraControlType(SYR::CameraControlType::FREE_CAMERA);
				m_ActiveScene->disableUi();
			}
		}

		escPressed = true;
	} else {
		escPressed = false;
	}

	static float rotation = 0.0f;
	rotation += ts;

	//m_FB->bind();
	SYR::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	SYR::RenderCommand::clear();

	SYR::Renderer::beginScene(m_PCameraController.getCamera());
	//SYR::Renderer::submit(m_MeshShader, m_TestMesh, glm::translate(glm::mat4(1.0f), pos));

	static float px = 0, py = 0, pz = 0;
	//px += velocity.x;
	//py += velocity.y;
	//pz += velocity.z;
	//SYR_CORE_INFO("{0} {1} {2}", px, py, pz);

	//SYR::Renderer::endScene();

	if (m_ActiveScene->entityIDExists(std::string("testid"))) {
		SYR::Entity ttt = m_ActiveScene->getEntityByID(std::string("testid"));
		//ttt.getComponent<SYR::TransformComponent>().offset(velocity);
		//ttt.getComponent<SYR::TransformComponent>().scale(1 + velocity.z);
	}

	if (m_ActiveScene->entityIDExists(std::string("exit"))) {
		SYR::Entity exitButton = m_ActiveScene->getEntityByID(std::string("exit"));
		if (exitButton.getComponent<SYR::IOListenerComponent>().checked) {
			SYR::Application::get().stop();
		}
	}

	//Spin the spotlights
	static float n = 0;
	if (m_ActiveScene->entityIDExists(std::string("TL2"))) {
		SYR::Entity light = m_ActiveScene->getEntityByID(std::string("TL2"));
		
		light.getComponent<SYR::LightComponent>().direction.x = cos(n) / 1.5f;
		light.getComponent<SYR::LightComponent>().direction.z = sin(n) / 1.5f;
	}
	if (m_ActiveScene->entityIDExists(std::string("TL3"))) {
		SYR::Entity light = m_ActiveScene->getEntityByID(std::string("TL3"));

		light.getComponent<SYR::LightComponent>().direction.x = cos(n * 3) / 1.5f;
		light.getComponent<SYR::LightComponent>().direction.z = sin(n * 3) / 1.5f;
	}
	n += 0.01f;

	/*
	static float t = 0.0f;
	t += 0.001f;

	if (m_ActiveScene->entityIDExists(std::string("exit"))) {
		SYR::Entity sword = m_ActiveScene->getEntityByID(std::string("SWORD"));
		sword.getComponent<SYR::TransformComponent>().scale(2 + cos(t));
	}
	*/
	

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), { px, py, pz });

	SYR::Renderer2D::beginScene(m_CameraController.getCamera());

	m_ActiveScene->onUpdate(ts);
	m_ActiveScene->onDraw();

	float wWidth = SYR::Application::get().getWindow().getWidth();
	float wHeight = SYR::Application::get().getWindow().getHeight();

	float mx = 2 * (SYR::Input::getMouseX() - wWidth / 2) / wWidth;
	float my = -2 * (SYR::Input::getMouseY() - wHeight / 2) / wHeight;

	std::string pointer = "(" + fmt::format("{:.2f}", mx) + ", " + fmt::format("{:.2f}", my) + ") ";

	if (m_PCameraController.getCameraControlType() != SYR::CameraControlType::FREE_CAMERA) {
		SYR::Renderer2D::drawText(m_CharacterSet, SYR::TextAlignment::HORIZONTAL_LEFT, pointer, { mx, my, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	}

	//SYR::Renderer2D::drawQuad(glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), { 0.6f, 0.6f, 1.0f }), glm::vec4(1, 1, 0, 1), m_CharacterSet->getCharacterSheet());
	//SYR::Renderer2D::drawLine(glm::vec2(0.0f, 0.0f), glm::vec2(-1.0f, -1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	
	
	SYR::Renderer2D::endScene();

	//m_FB->unbind();
	//uint32_t texID = m_FB->getColorAttachmentRendererID();

	//auto stats = SYR::Renderer2D::getStats();
	//SYR_CORE_INFO("Renderer2D Stats: {0} {1} {2} {3}", stats.drawCalls, stats.quadCount, stats.getTotalVertexCount(), stats.getTotalIndexCount());
}
void Sandbox2D::onEvent(SYR::Event& e) {
	m_CameraController.onEvent(e);
	m_PCameraController.onEvent(e);
	m_ActiveScene->onEvent(e);
}