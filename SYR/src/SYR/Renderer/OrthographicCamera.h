#pragma once

#include <glm/glm.hpp>

#include "SYR/Renderer/Camera.h"

namespace SYR {

	class OrthographicCamera : public Camera {
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void setProjection(float left, float right, float bottom, float top);

		float getRotation() const { return m_Rotation; }
		void setRotation(float rotation) { m_Rotation = rotation; recalculateViewMatrix(); }

	private:
		void recalculateViewMatrix();
	private:
		float m_Rotation = 0.0f;//Z rotation
	};

}
