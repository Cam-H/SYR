#include "syrpch.h"
#include "PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace SYR {

    void PerspectiveCamera::rotate(float yaw, float pitch, float roll) {
       
        m_Rotation.x = glm::mod((m_Rotation.x + yaw / 5), 360.0f);
        m_Rotation.y += pitch / 5;
        m_Rotation.z += roll / 5; 

        if (m_Rotation.y > 89.0f)
            m_Rotation.y = 89.0f;
        if (m_Rotation.y < -89.0f)
            m_Rotation.y = -89.0f;

        recalculateViewMatrix();
    }

    void PerspectiveCamera::updateCameraVectors() {

        glm::vec3 front = {
                            cos(glm::radians(m_Rotation.x)) * cos(glm::radians(m_Rotation.y)),
                            sin(glm::radians(m_Rotation.y)),
                            sin(glm::radians(m_Rotation.x)) * cos(glm::radians(m_Rotation.y))
        };

        m_Front = glm::normalize(front);

        // also re-calculate the Right and Up vector
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));

    }

    void PerspectiveCamera::recalculateViewMatrix() {
        updateCameraVectors();

        m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;        
    }

    void PerspectiveCamera::setProjection(float left, float right, float bottom, float top) {
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

}