#pragma once

#include "Renderer/OrthographicCamera.h"
#include "Renderer/PerspectiveCamera.h"

#include "SYR/Core/Timestep.h"

#include "SYR/Events/ApplicationEvent.h"
#include "SYR/Events/MouseEvent.h"

#include "SYR/Core/KeyCodes.h"

namespace SYR {

	enum class CameraType {
		ORTHOGRAPHIC = 0, PERSPECTIVE
	};

	enum class CameraControlType {
		LOCKED_CAMERA, FIXED_CAMERA, FREE_CAMERA

		/*
		* LOCKED - No manual translation or rotation
		* FIXED - No manual translation
		* FREE - Enables manual translation and rotation (by hiding / centering mouse)
		*/
	};

	class CameraController {
	public:

		CameraController(CameraType cameraType, float aspectRatio);
		CameraController(CameraType cameraType, CameraControlType cameraControlType, float aspectRatio);


		void setCameraControlType(CameraControlType controlType);
		CameraControlType getCameraControlType() { return m_CameraControlType; }

		float getZoomLevel() const { return m_ZoomLevel; }
		void setZoomLevel(float level) { m_ZoomLevel = level; calculateView(); }

		float getAspectRatio() const { return m_AspectRatio; }
		void setAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; }

		void onUpdate(Timestep ts);
		void onEvent(Event& e);

		Camera& getCamera() { return *m_Camera; }
		const Camera& getCamera() const { return *m_Camera; }

	private:
		void initializeCamera(CameraType cameraType);

		void calculateView();

		bool onMouseMoved(MouseMovedEvent& e);
		bool onMouseScrolled(MouseScrolledEvent& e);
		bool onWindowResized(WindowResizeEvent& e);
	private:

		float m_ZoomLevel = 1.0f;
		float m_AspectRatio;

		float m_CameraTranslationSpeed = 6.0f;
		float m_CameraRotationSpeed = 20.0f;

		Camera* m_Camera;

		CameraType m_CameraType;
		CameraControlType m_CameraControlType;

		float lmx = -1, lmy = -1;
	};

}