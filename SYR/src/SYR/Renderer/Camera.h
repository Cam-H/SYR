#pragma once

#include <glm/glm.hpp>

namespace SYR {

	class Camera {
	public:

		virtual void setProjection(float left, float right, float bottom, float top) = 0;

		~Camera() = default;

		const glm::vec3& getPosition() const { return m_Position; }
		void setPosition(const glm::vec3& position) { m_Position = position; recalculateViewMatrix(); }
		void offset(const glm::vec3& offset) { m_Position += offset; recalculateViewMatrix(); }

		//float getRotation() const { return m_Rotation; }
		//void setRotation(float rotation) { m_Rotation = rotation; recalculateViewMatrix(); }

		const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& getViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& getViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	protected:
		virtual void recalculateViewMatrix() = 0;
	protected:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		//float m_Rotation = 0.0f;//Z rotation
	};

}
