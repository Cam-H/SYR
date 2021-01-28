#pragma once

#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SYR {

	// Default camera values
	const float YAW = -90.0f;
	const float PITCH = 0.0f;

	class PerspectiveCamera : public Camera {
	public:

		PerspectiveCamera(float yaw, float pitch) : m_Rotation(yaw, pitch, 0.0f) {
			setProjection(0, 0, 0, 0);
			updateCameraVectors();
		}

		PerspectiveCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : m_Front(glm::vec3(0.0f, 0.0f, -1.0f))
		{
			setProjection(0, 0, 0, 0);

			m_Position = position;
			m_WorldUp = up;
			
			m_Rotation = { yaw, pitch, 0.0f };

			updateCameraVectors();
		}

		virtual void setProjection(float left, float right, float bottom, float top) override;

		void rotate(float yaw, float pitch, float roll);
		const glm::vec3& getRotation() { return m_Rotation; }

	private:
		void updateCameraVectors();
		virtual void recalculateViewMatrix() override;
	private:
		glm::vec3 m_Front;
		glm::vec3 m_Up;
		glm::vec3 m_Right;
		glm::vec3 m_WorldUp;

		// euler Angles
		glm::vec3 m_Rotation;

	};
}