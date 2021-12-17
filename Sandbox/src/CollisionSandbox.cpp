#include "CollisionSandbox.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <SYR/Core/Application.h>


CollisionSandbox::CollisionSandbox() : Layer("CollisionSandbox"), m_CameraController(SYR::CameraType::ORTHOGRAPHIC, SYR::CameraControlType::LOCKED_CAMERA, 1280.0f / 720.0f), m_PCameraController(SYR::CameraType::PERSPECTIVE, SYR::CameraControlType::LOCKED_CAMERA, 1280.0f / 720.0f) {
	//SYR::PerspectiveCamera pcam = SYR::PerspectiveCamera(1280.0f / 720.0f);
}

void CollisionSandbox::onAttach() {

	m_CharacterSet = SYR::CharacterSet::create("assets/fonts/arial.ttf", SYR::CharacterCollection::ASCII, 24);


	m_ActiveScene = SYR::createRef<SYR::Scene>();
	m_ActiveScene->prepare();

	m_PCameraController.setCameraControlType(SYR::CameraControlType::FREE_CAMERA);
	m_ActiveScene->disableUi();

	SYR::Renderer::getShaderLibrary()->load("MeshShader", "assets/shaders/B3D.glsl");
	SYR::Renderer::getShaderLibrary()->load("GrayShader", "assets/shaders/Gray.glsl");
	SYR::Renderer::getShaderLibrary()->load("OutlineShader", "assets/shaders/Highlight.glsl");

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
	e.getComponent<SYR::TagComponent>().id = "FLOOR";
	e.getComponent<SYR::TransformComponent>().offset({ 0, -5, 0 });
	e.addComponent<SYR::MeshComponent>(SYR::FileHandler::loadMesh("assets/meshes/Floor.obj"));

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<uint32_t> indices;
	std::vector<std::array<uint32_t, 3>> vertexKeys;
	
	/*************************************************LIGHTS*************************************************/

	e = m_ActiveScene->createEntity();
	e.getComponent<SYR::TagComponent>().tag = "Test Light";
	e.getComponent<SYR::TagComponent>().id = "TL";
	e.addComponent<SYR::LightComponent>(glm::vec3(0, 1, 1), glm::vec3(15, 2, 15), 0.03f, 0.001f);

	e = m_ActiveScene->createEntity();
	e.getComponent<SYR::TagComponent>().id = "TL4";
	e.addComponent<SYR::LightComponent>(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, -1, 0));

	srand(time(NULL));

	m_PCameraController.setCameraControlType(SYR::CameraControlType::FREE_CAMERA);
	//SYR::Application::get().getWindow().hideCursor();
	
	m_CL = SYR::createRef<SYR::ColliderLibrary>();
	m_Cyl = m_CL->load("assets/meshes/Cylinder.obj");
	m_Cube1 = m_CL->load("assets/meshes/Convex2.obj");
	m_Cube2 = m_CL->load("assets/meshes/Convex2.obj");

}

void CollisionSandbox::onDetach() {

}

/*
static void getBorders(glm::vec2* vertices, uint16_t* vertexCount) {
	
	glm::vec2 temp;
	glm::vec2 referenceLine = { 0, 1 };//Use as the base line for dot product calculations to find exterior edges. Initially vertical to assist in getting first edge
	uint16_t index = 1;

	//Find an appropriate line to start connecting points (rightmost line). Extreme points necessarily won't be subsumed when creating a convex polygon
	uint16_t extremeVertexIndex = 0;
	for (uint16_t i = 1; i < *vertexCount; i++) {
		if (vertices[extremeVertexIndex].x < vertices[i].x) {
			extremeVertexIndex = i;
		}
	}
	//Swap the starting vertex with the first element in the array
	temp = vertices[0];
	vertices[0] = vertices[extremeVertexIndex]; vertices[extremeVertexIndex] = temp;

	//Order vertices such that a series of lines running from point to subsequent point encloses the maximum amount of space
	for (uint16_t i = 0; i < *vertexCount - 1; i++) {
		index = i + 1;
		glm::vec2 base = glm::normalize(vertices[index] - vertices[i]);

		for (uint16_t j = 0; j < *vertexCount; j++) {//Start from 0 rather than i to allow interior vertices to be cut if necessary
			if (i == j) continue;

			glm::vec2 test = glm::normalize(vertices[j] - vertices[i]);

			if (glm::dot(referenceLine, test) < glm::dot(referenceLine, base)) {
				index = j;
				base = glm::normalize(vertices[index] - vertices[i]);
			}
		}

		if (index <= i) {
			*vertexCount = i + 1;
			break;
		} else {
			temp = vertices[index];
			vertices[index] = vertices[i + 1];
			vertices[i + 1] = temp;
		}

		referenceLine = vertices[i] - vertices[i + 1];
	}

}
//*/

void CollisionSandbox::onUpdate(SYR::Timestep ts) {

	//Update scene and do rendering

	m_CameraController.onUpdate(ts);
	m_PCameraController.onUpdate(ts);

	glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };

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
			}
			else {
				m_PCameraController.setCameraControlType(SYR::CameraControlType::FREE_CAMERA);
			}
		}
	}
	escPressed = SYR::Input::isKeyPressed(SYR_KEY_ESCAPE);

	static float rotation = 0.0f;
	rotation += ts;

	//m_FB->bind();
	SYR::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	SYR::RenderCommand::clear();

	SYR::Renderer::beginScene(m_PCameraController.getCamera());

	static float px = 0, py = 0, pz = 0;
	//px += velocity.x;
	//py += velocity.y;
	//pz += velocity.z;
	//SYR_CORE_INFO("{0} {1} {2}", px, py, pz);

	//SYR::Renderer::endScene();

	SYR::Renderer2D::beginScene(m_CameraController.getCamera());

	m_ActiveScene->onUpdate(ts);
	m_ActiveScene->onDraw();

	static glm::vec4 color = { 0, 1, 0, 1 };

	static int z = -1;
	static uint16_t vertexCount = 9;
	static glm::vec2 vertices[9];
	static glm::vec2 oVertices[9];

	if (z == -1 || (SYR::Input::isKeyPressed(SYR_KEY_SPACE) && z > 6)) {
		//Random reset
		for (uint8_t i = 0; i < sizeof(vertices) / sizeof(glm::vec2); i++) {
			oVertices[i] = vertices[i] = { ((float)(rand() % 100)) / 100, ((float)(rand() % 100)) / 100 };
		}

		vertexCount = 9;

		z = 0;
	}
	z++;


	/*
	getBorders(vertices, &vertexCount);

	for (uint8_t i = 0; i < sizeof(oVertices) / sizeof(glm::vec2); i++) {
		SYR::Renderer2D::drawCircle(oVertices[i], 0.01f, { 1, 1, 1, 1 });
		SYR::Renderer2D::drawText(m_CharacterSet, SYR::TextAlignment::HORIZONTAL_CENTER, std::to_string(i), { vertices[i].x, vertices[i].y - 0.05f, 1 }, { 1,1,1,1 }, false);
	}
	
	for (uint8_t i = 0; i < vertexCount - 1; i++) {
		SYR::Renderer2D::drawLine(vertices[i], vertices[i + 1], color);
	}
	SYR::Renderer2D::drawLine(vertices[vertexCount - 1], vertices[0], color);


	SYR::Renderer2D::endScene();
	//*/

	//SYR::Renderer::endScene();
	//SYR::Renderer::beginScene(m_PCameraController.getCamera());

	SYR::Renderer::endScene();


	glm::vec3 vx[] = { {1, 0, 0}, {1.1, 0.1, 1}, {1.1, 2, 0.2} };
	glm::vec3 vx2[] = { {2, 2, 2}, {5.1, 1.1, 1}, {5.1, 5, 5.2} };

	vertexCount = 3;

	SYR::Renderer3D::beginScene(m_PCameraController.getCamera());
	SYR::Renderer3D::drawLines(vx, vertexCount, { 1, 1, 1, 1 });
	SYR::Renderer3D::drawLines(vx2, vertexCount, { 1, 1, 1, 1 });
	SYR::Renderer3D::drawSphere({ -2, 1, 1 }, 2, { 0, 0, 1, 1 }, 16, 16);
	SYR::Renderer3D::drawSphere({ -2, 1, 1 }, 1, { 0, 1, 0, 1 }, 24);
	SYR::Renderer3D::drawICOSphere({ -2, 5, 1 }, 1, { 0, 1, 0, 1 }, 8);

	//glm::mat4 cylTransform = glm::translate(glm::mat4(1.0f), {5, 1, 1}) * glm::rotate(glm::mat4(1.0f), 3.1415f, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { 1, 1, 1 });
	glm::mat4 cylTransform = glm::translate(glm::mat4(1.0f), {0, 1, -5}) * glm::scale(glm::mat4(1.0f), { 1, 1, 1 });
	m_Cube2->render(cylTransform);

	SYR::Renderer3D::endScene();
}

void CollisionSandbox::onEvent(SYR::Event& e) {
	m_CameraController.onEvent(e);
	m_PCameraController.onEvent(e);
	m_ActiveScene->onEvent(e);
}