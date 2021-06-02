#include "syrpch.h"
#include "CameraController.h"

#include "SYR/Core/Input.h"
#include "SYR/Core/Application.h"

namespace SYR {

	/*
	CameraController::CameraController(Camera* camera, float aspectRatio) : m_Camera(camera), m_AspectRatio(aspectRatio) {
		//SYR_CORE_INFO("Z: {0}", dynamic_cast<PerspectiveCamera*>(camera));
		(*m_Camera).setProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}
	*/

	CameraController::CameraController(CameraType cameraType, float aspectRatio) : m_CameraType(cameraType), m_AspectRatio(aspectRatio) {
		initializeCamera(cameraType);

		setCameraControlType(CameraControlType::LOCKED_CAMERA);
	}

	CameraController::CameraController(CameraType cameraType, CameraControlType cameraControlType, float aspectRatio) : m_CameraType(cameraType), m_CameraControlType(cameraControlType), m_AspectRatio(aspectRatio) {
		//SYR_CORE_INFO("Z: {0}", dynamic_cast<PerspectiveCamera*>(camera));

		initializeCamera(cameraType);
		setCameraControlType(cameraControlType);
	}

	void CameraController::initializeCamera(CameraType cameraType) {
		switch (cameraType) {
		case CameraType::ORTHOGRAPHIC:
			m_Camera = new OrthographicCamera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
			break;
		case CameraType::PERSPECTIVE:
			m_Camera = new PerspectiveCamera();
			break;
		default:
			SYR_CORE_ERROR("Unrecognized camera type!");
			m_Camera = nullptr;
		}

		calculateView();
		//m_Camera->offset({ 0, 0, 0 });//Call a camera function to force it to recalculate the view matrix
	}

	void CameraController::setCameraControlType(CameraControlType controlType) { 
		m_CameraControlType = controlType;

		if (m_CameraControlType == CameraControlType::LOCKED_CAMERA) {//Show mouse in locked view
			Application::get().getWindow().showCursor();
		} else {//Hide mouse in free camera mode
			Application::get().getWindow().hideCursor();
		}
	}


	void CameraController::calculateView() {
		m_Camera->setProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	void CameraController::onUpdate(Timestep ts) {
		
		if (m_CameraControlType == CameraControlType::LOCKED_CAMERA || m_CameraControlType == CameraControlType::FIXED_CAMERA) {
			return;
		}

		glm::vec3 cameraAdvance(0.0f);

		float translation = m_CameraTranslationSpeed * ts;

		if (m_CameraType == CameraType::ORTHOGRAPHIC) {
			if (Input::isKeyPressed(SYR_KEY_W)) {
				cameraAdvance.y += translation;
			}

			if (Input::isKeyPressed(SYR_KEY_S)) {
				cameraAdvance.y -= translation;
			}

			if (Input::isKeyPressed(SYR_KEY_A)) {
				cameraAdvance.x -= translation;
			}

			if (Input::isKeyPressed(SYR_KEY_D)) {
				cameraAdvance.x += translation;
			}
		}else if (m_CameraType == CameraType::PERSPECTIVE) {
			const glm::vec3 rotation = dynamic_cast<PerspectiveCamera*>(m_Camera)->getRotation();

			if (Input::isKeyPressed(SYR_KEY_W)) {
				cameraAdvance.x += translation * cos(glm::radians(rotation.x));
				cameraAdvance.z += translation * sin(glm::radians(rotation.x));

				//cameraAdvance.y += translation * sin(glm::radians(rotation.y));
			}

			if (Input::isKeyPressed(SYR_KEY_S)) {
				cameraAdvance.x -= translation * cos(glm::radians(rotation.x));
				cameraAdvance.z -= translation * sin(glm::radians(rotation.x));

				//cameraAdvance.y -= translation * sin(glm::radians(rotation.y));
			}

			if (Input::isKeyPressed(SYR_KEY_A)) {
				cameraAdvance.x += translation * cos(glm::radians(rotation.x - 90.0f));
				cameraAdvance.z += translation * sin(glm::radians(rotation.x - 90.0f));
			}

			if (Input::isKeyPressed(SYR_KEY_D)) {
				cameraAdvance.x += translation * cos(glm::radians(rotation.x + 90.0f));
				cameraAdvance.z += translation * sin(glm::radians(rotation.x + 90.0f));
			}

			if (Input::isKeyPressed(SYR_KEY_SPACE)) {
				cameraAdvance.y += translation;
			}

			if (Input::isKeyPressed(SYR_KEY_LEFT_SHIFT)) {
				cameraAdvance.y -= translation;
			}

		}

		(*m_Camera).offset(cameraAdvance);
	}

	void CameraController::onEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseMovedEvent>(SYR_BIND_EVENT_FN(CameraController::onMouseMoved));
		dispatcher.dispatch<MouseScrolledEvent>(SYR_BIND_EVENT_FN(CameraController::onMouseScrolled));
		dispatcher.dispatch<WindowResizeEvent>(SYR_BIND_EVENT_FN(CameraController::onWindowResized));
	}

	bool CameraController::onMouseMoved(MouseMovedEvent& e) {
		if (m_CameraControlType != CameraControlType::LOCKED_CAMERA) {
			if (m_CameraType == CameraType::PERSPECTIVE) {
				if (lmx != -1 && lmy != -1) {
					float dx = e.getX() - lmx;
					float dy = e.getY() - lmy;

					dynamic_cast<PerspectiveCamera*>(m_Camera)->rotate(dx, -dy, 0.0f);
				}
			}
		}

		lmx = e.getX();
		lmy = e.getY();

		return false;
	}


	bool CameraController::onMouseScrolled(MouseScrolledEvent& e) {

		if (m_CameraControlType != CameraControlType::LOCKED_CAMERA) {
			m_ZoomLevel -= e.getYOffset() * 0.25f;
			m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);

			calculateView();
		}

		return false;
	}

	bool CameraController::onWindowResized(WindowResizeEvent& e) {
		m_AspectRatio = (float)e.getWidth() / (float)e.getHeight();
		calculateView();

		return false;
	}

}