#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <SYR/Core/Application.h>


Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(SYR::CameraType::ORTHOGRAPHIC, SYR::CameraControlType::LOCKED_CAMERA, 1280.0f / 720.0f), m_PCameraController(SYR::CameraType::PERSPECTIVE, SYR::CameraControlType::FREE_CAMERA, 1280.0f / 720.0f) {
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
	m_CharacterSet = SYR::CharacterSet::create("assets/fonts/kochi-mincho.ttf", codes);

	SYR::FrameBufferSpecification spec;
	spec.width = 1280;
	spec.height = 720;

	m_FB = SYR::FrameBuffer::create(spec);

	m_ActiveScene = SYR::createRef<SYR::Scene>();
	m_ActiveScene->prepare();

	//m_CameraController = *m_ActiveScene->getUiCamera();
	//m_CameraController.setZoomLevel(2.0f);
	//m_ActiveScene->getUiCamera()->setZoomLevel(2);

	glm::vec2* body = new glm::vec2[4];
	/*
	body[0] = glm::vec2(0.0f, 0.0f);
	body[1] = glm::vec2(1.0f, 0.0f);
	body[2] = glm::vec2(1.0f, 1.0f);
	body[3] = glm::vec2(0.0f, 1.5f);
	*/
	

	SYR::Entity temp = m_ActiveScene->createEntity();
	temp.getComponent<SYR::TransformComponent>().offset(glm::vec3(-1.2f, 0.5f, 0.0f));

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

	SYR::Renderer::getShaderLibrary()->load("MeshShader", "assets/shaders/B3D.glsl");
	//m_MeshShader = SYR::Shader::create("assets/shaders/B3D.glsl");

	m_TestMesh2 = SYR::FileHandler::loadMesh("assets/meshes/Screen.obj");

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

	static float rotation = 0.0f;
	rotation += ts;

	//m_FB->bind();
	SYR::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	SYR::RenderCommand::clear();

	SYR::Renderer::beginScene(m_PCameraController.getCamera());
	//SYR::Renderer::submit(m_MeshShader, m_TestMesh, glm::translate(glm::mat4(1.0f), pos));
	SYR::Renderer::submit(SYR::Renderer::getShaderLibrary()->get("MeshShader"), m_TestMesh2, glm::translate(glm::mat4(1.0f), { 5.0f, 1.0f, 1.0f }) * glm::rotate(glm::mat4(1.0f), rotation, { 1.2f, 1.1f, 1.0f }));
	//SYR::Renderer::endScene();

	if (m_ActiveScene->entityIDExists(std::string("testid"))) {
		SYR::Entity ttt = m_ActiveScene->getEntityByID(std::string("testid"));
		ttt.getComponent<SYR::TransformComponent>().offset(velocity);
		ttt.getComponent<SYR::TransformComponent>().scale(1 + velocity.z);
	}

	if (m_ActiveScene->entityIDExists(std::string("exit"))) {
		SYR::Entity exitButton = m_ActiveScene->getEntityByID(std::string("exit"));
		if (exitButton.getComponent<SYR::ListenerComponent>().checked) {
			SYR::Application::get().stop();
		}
	}

	//inline static Application& get() { return *s_Instance; }

	//SYR::Entity::getComponent<SYR::TransformComponent>(m_ActiveScene.get(), ttt).offset(velocity);
	//SYR::Entity::getComponent<SYR::TransformComponent>(m_ActiveScene.get(), ttt).scale(1 + velocity.z);

	//SYR_TRACE("Delta time: {0}s ({1}ms)", ts.getSeconds(), ts.getMilliseconds());


	
	SYR::Renderer2D::beginScene(m_CameraController.getCamera());

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 0.75f, 0.5f, 1.0f }) * glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f, 1.0f });
	//SYR::Renderer2D::drawQuad(glm::translate(glm::mat4(1.0f), { 0.75f, 0.5f, 1.0f }) * glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f, 1.0f }), {1.0f, 1.0f, 1.0f, 1.0f}, m_CharacterSet->getCharacterSheet());

	transform = glm::translate(glm::mat4(1.0f), { -1.0f, 0.0f, 1.0f });

	m_ActiveScene->onUpdate(ts);
	m_ActiveScene->onDraw();


	
	glm::vec2 pos(0.0f, 0.0f);
	glm::vec2 size(1.0f, 1.0f);
	glm::vec4 color(0.8f, 0.2f, 0.3f, 1.0f);

	//SYR::Renderer2D::drawQuad(pos, size, color);



	
	pos = { -1.5f, 0.0f };
	size = { 0.5f, 0.5f };
	color = { 0.2f, 0.5f, 0.2f, 1.0f };

	//SYR::Renderer2D::drawRotatedQuad(pos, size, -rotation, color, m_Texture);

	pos = { 5.0f, 0.0f };
	size = { 7.0f, 5.0f };
	//SYR::Renderer2D::drawRotatedQuad(glm::vec3(pos.x, pos.y, -0.1f), size, rotation, m_Texture);

	int i = 0;
	for (float y = -5.0f; y < 5.0f; y+=0.1f) {
		for (float x = -5.0f; x < 5.0f; x+=0.1f) {
			glm::vec4 c = { (x + 5.0f) / 10, (y + 5.0f) / 10.0f, 0.2f, 0.75f};
			//SYR::Renderer2D::drawQuad(glm::vec2(x, y), glm::vec2(0.09f, 0.09f), c);
		}
	}

	//SYR::Renderer2D::drawQuad(glm::vec3(0, 0, 1.0f), size, m_SpriteSheet);
	//SYR::Renderer2D::drawRotatedQuad(glm::vec3(-1.0f, 0, 0.9f), glm::vec2(1.0f, 2.0f), rotation, m_SubTexture);
	SYR::Renderer2D::drawText(SYR::Renderer::getCharacterSetLibrary()->get("CS-KOCHI-MINCHO-48"), SYR::Renderer2D::TextAlignment::VERTICAL_TOP, "ABCDEF?", { -1.0f, 0.1f, 1.0f }, { 1.0f, 0.4f, 0.0f, 1.0f });
	SYR::Renderer2D::drawText(m_CharacterSet, SYR::Renderer2D::TextAlignment::VERTICAL_CENTER, "3BZPAAMAAPZB3", { -0.6f, -0.5f, 1.0f }, { 1.0f, 0.0f, 1.0f, 1.0f });

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