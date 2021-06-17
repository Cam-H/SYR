#include "syrpch.h"
#include "InputSystem.h"
#include "UiSystem.h"

#include "SYR/Core/Input.h"

#include "SYR/Scene/Components.h"
#include "SYR/Scene/CollisionSystem.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "SYR/Renderer/Renderer2D.h"


namespace SYR {

	void InputSystem::processUserInputs(entt::registry& registry, const glm::mat4 viewProjection) {
		auto view = registry.view<TransformComponent, IOListenerComponent>();

		glm::vec4 nav = { 0, 0, 0, 0 };
		bool runKeyMovement = false;

		//Skip input processing if there are no new inputs
		while (!s_InputQueue.empty()) {
			InputSet set = s_InputQueue.front();
			s_InputQueue.pop();

			//x2 as that is applied by quadVertexBase in Renderer2D
			float rx = (set.mx - s_WindowWidth / 2) / s_WindowWidth * 2.0f;
			float ry = -(set.my - s_WindowHeight / 2) / s_WindowHeight * 2.0f;

			float aspectRatio = s_WindowWidth / s_WindowHeight;

			glm::vec2 mouse = { rx * aspectRatio, ry };

			float modifier = set.inputType == InputType::KEY_RELEASE ? -0.1f : (set.inputType == InputType::KEY_PRESS && set.repeats == 0 ? 0.1f : 0);

			switch (set.value) {
			case SYR_KEY_UP:
				s_NavigationHeading.y += modifier;;
				break;
			case SYR_KEY_LEFT:
				s_NavigationHeading.x -= modifier;
				break;
			case SYR_KEY_DOWN:
				s_NavigationHeading.y -= modifier;
				break;
			case SYR_KEY_RIGHT:
				s_NavigationHeading.x += modifier;
				break;
			}
			

			for (const entt::entity e : view) {
				if (registry.has<UiComponent>(e)) {
					//continue;
				}

				switch (set.inputType) {
				case InputType::KEY_PRESS: 
					if (set.value == SYR_KEY_ENTER) {
						for (const entt::entity e : view) {
							if (view.get<IOListenerComponent>(e).hovered) {
								view.get<IOListenerComponent>(e).selected = true;
								break;
							}
						}
					}

					runKeyMovement = true;
					break;
				case InputType::KEY_RELEASE:
					if (set.value == SYR_KEY_ENTER) {
						for (const entt::entity e : view) {
							if (view.get<IOListenerComponent>(e).selected) {
								view.get<IOListenerComponent>(e).checked = !view.get<IOListenerComponent>(e).checked;
								view.get<IOListenerComponent>(e).selected = false;
								break;
							}
						}
					}
					runKeyMovement = true;
					break;
				case InputType::MOUSE_PRESS:
					view.get<IOListenerComponent>(e).selected = (set.value == 0 && view.get<IOListenerComponent>(e).hovered);
					break;
				case InputType::MOUSE_RELEASE:
					if (set.value == 0 && view.get<IOListenerComponent>(e).selected) {
						if (view.get<IOListenerComponent>(e).hovered) {
							view.get<IOListenerComponent>(e).checked = !view.get<IOListenerComponent>(e).checked;
						}

						view.get<IOListenerComponent>(e).selected = false;
					}
					break;
				case InputType::MOUSE_SCROLL:
					if (view.get<IOListenerComponent>(e).hovered && registry.has<LayoutComponent>(e)) {//Try scrolling the layout if it is hovered
						LayoutComponent& layout = registry.get<LayoutComponent>(e);

						if (layout.scrollable) {
							float offset = layout.offset + set.value / 10.0f;

							if (layout.wrap) {
								//TODO
							} else {
								offset = std::min(1.0f, std::max(0.0f, offset));
							}

							layout.offset = offset;
							//SYR_CORE_INFO("{0} {1}", set.value, layout.offset);
						}
					}
					[[fallthrough]];//Mouse selection should be updated when scrolling
				case InputType::MOUSE_MOVE:
					entt::entity parent = registry.get<TagComponent>(e).parent;
					
					//Only hover an entity if the parent is also hovered so as to avoid interacting with potentially hidden (via stencil) elements
					view.get<IOListenerComponent>(e).hovered = isHovered(registry, e, mouse) && (parent != entt::null ? isHovered(registry, parent, mouse) : true);

#ifdef _DEBUG
					glm::vec4 color = view.get<IOListenerComponent>(e).hovered ? glm::vec4(1.0f, 0.7f, 1.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

					//Renderer2D::drawCircle(mouse, 0.002f, color);

					//Renderer2D::drawLine(transformedVertices[0], transformedVertices[1], color);
					//Renderer2D::drawLine(transformedVertices[1], transformedVertices[2], color);
					//Renderer2D::drawLine(transformedVertices[2], transformedVertices[3], color);
					//Renderer2D::drawLine(transformedVertices[3], transformedVertices[0], color);

					//Renderer2D::drawLine({ nav.x, nav.y }, { nav.z, nav.w }, color);

#endif
					break;
				}
			}
		}

		/*************************Process any keyboard based ui navigation********************************/

		if (glm::length(s_NavigationHeading) > 0 && runKeyMovement) {
			entt::entity sourceEntity = entt::null;

			for (const entt::entity e : view) {
				if (view.get<IOListenerComponent>(e).hovered && view.get<IOListenerComponent>(e).interactable) {
					glm::mat4& transform = view.get<TransformComponent>(e).transform;

					glm::vec3 translation;
					glm::decompose(transform, glm::vec3(), glm::quat(), translation, glm::vec3(), glm::vec4());

					sourceEntity = e;
					nav.x += translation.x;
					nav.y += translation.y;

					break;
				}
			}

			if (sourceEntity != entt::null) {
				nav.z = nav.x + s_NavigationHeading.x;
				nav.w = nav.y + s_NavigationHeading.y;

				//Renderer2D::drawLine({ nav.x, nav.y }, { nav.z, nav.w }, { 0.0f, 0.0f, 1.0f, 1.0f });

				entt::entity nearestEntity = entt::null;
				float tMin = -1;

				//Identify the nearest valid target entity in the specified direction
				for (const entt::entity e : view) {
					if (e == sourceEntity || !view.get<IOListenerComponent>(e).keyListener) {
						continue;
					}

					glm::mat4& transform = view.get<TransformComponent>(e).transform;

					glm::vec2* transformedVertices = new glm::vec2[view.get<IOListenerComponent>(e).vertexCount];
					for (int i = 0; i < view.get<IOListenerComponent>(e).vertexCount; i++) {
						glm::vec2 baseVertex = view.get<IOListenerComponent>(e).vertices[i];
						glm::vec4 vertex = { baseVertex.x, baseVertex.y, 1.0f, 1.0f };
						glm::vec3 pos = transform * vertex;

						transformedVertices[i] = { pos.x, pos.y };
					}

					float temp = CollisionSystem::getCollision(transformedVertices, view.get<IOListenerComponent>(e).vertexCount, nav);
					if (temp > 0 && (tMin < 0 || temp < tMin)) {
						nearestEntity = e;
						tMin = temp;
					}
				}

				//Target the entity, if one was identified
				if (nearestEntity != entt::null) {
					registry.get<IOListenerComponent>(nearestEntity).hovered = true;
					registry.get<IOListenerComponent>(sourceEntity).hovered = false;

					LayoutComponent& layout = registry.get<LayoutComponent>(registry.get<TagComponent>(nearestEntity).parent);

					if (layout.scrollable) {
						glm::mat4& lTransform = registry.get<TransformComponent>(registry.get<TagComponent>(nearestEntity).parent).transform;

						glm::vec3 lScale;
						glm::vec3 lTranslation;
						glm::decompose(lTransform, lScale, glm::quat(), lTranslation, glm::vec3(), glm::vec4());


						glm::mat4& eTransform = registry.get<TransformComponent>(nearestEntity).transform;

						glm::vec3 eScale;
						glm::vec3 eTranslation;
						glm::decompose(eTransform, eScale, glm::quat(), eTranslation, glm::vec3(), glm::vec4());

						if (layout.contentLimits.x > 0) {
							float delta = lTranslation.x - eTranslation.x;

							if (abs(delta) > (lScale.x - eScale.x) / 2 - lScale.x * layout.margins) {
								layout.offset -= (delta - ((delta > 0) * lScale.x - eScale.x) / 2 + lScale.x * layout.margins) / layout.contentLimits.x;
							}
						}
						else {
							float delta = lTranslation.y - eTranslation.y;

							if (abs(delta) > (lScale.y - eScale.y) / 2 - lScale.y * layout.margins) {
								layout.offset += (delta - ((delta > 0) * lScale.y - eScale.y) / 2 + lScale.y * layout.margins) / layout.contentLimits.y;
							}
						}
					}

					//Renderer2D::drawCircle({ (nav.z - nav.x) * tMin + nav.x, (nav.w - nav.y) * tMin + nav.y }, 0.02f, { 0.5f, 0.0f, 1.0f, 1.0f });
				}
			} else {
				//Hover over the first listener entity
				//view.get<IOListenerComponent>(view.back()).hovered = true;
			}
		}
		
	}

	bool InputSystem::isHovered(entt::registry& registry, entt::entity entity, glm::vec2 pointer) {
		if (registry.has<IOListenerComponent>(entity)) {
			IOListenerComponent& io = registry.get<IOListenerComponent>(entity);
			glm::mat4& transform = registry.get<TransformComponent>(entity).transform;

			glm::vec2* transformedVertices = new glm::vec2[io.vertexCount];
			for (int i = 0; i < io.vertexCount; i++) {
				glm::vec2 baseVertex = io.vertices[i];
				glm::vec4 vertex = { baseVertex.x, baseVertex.y, 1.0f, 1.0f };
				glm::vec3 pos = transform * vertex;

				transformedVertices[i] = { pos.x, pos.y };
			}

			return CollisionSystem::checkCollision(transformedVertices, io.vertexCount, pointer, 0.002f);
		}
		
		return false;
	}

	entt::entity InputSystem::getHoveredEntity(entt::registry& registry) {
		auto view = registry.view<IOListenerComponent>();

		for (const entt::entity e : view) {
			if (view.get<IOListenerComponent>(e).hovered) {
				return e;
			}
		}

		return entt::null;
	}

	void InputSystem::enqueue(InputType inputType, int value) {
		enqueue({ inputType, s_Mx, s_My, value, 0 });
	}

	void InputSystem::enqueue(InputSet inputSet) {
		s_InputQueue.emplace(inputSet);
	}

	void InputSystem::dequeue() {
		s_InputQueue.pop();
	}

	void InputSystem::onEvent(Event& e) {
		EventDispatcher dispatcher(e);

		dispatcher.dispatch<WindowResizeEvent>(std::bind(InputSystem::onWindowResize, std::placeholders::_1));

		dispatcher.dispatch<MouseMovedEvent>(std::bind(InputSystem::onMouseMoved, std::placeholders::_1));
		dispatcher.dispatch<MouseScrolledEvent>(std::bind(InputSystem::onMouseScrolled, std::placeholders::_1));
		dispatcher.dispatch<MouseButtonPressedEvent>(std::bind(InputSystem::onMouseButtonPressed, std::placeholders::_1));
		dispatcher.dispatch<MouseButtonReleasedEvent>(std::bind(InputSystem::onMouseButtonReleased, std::placeholders::_1));

		dispatcher.dispatch<KeyPressedEvent>(std::bind(InputSystem::onKeyPressedEvent, std::placeholders::_1));
		dispatcher.dispatch<KeyReleasedEvent>(std::bind(InputSystem::onKeyReleasedEvent, std::placeholders::_1));
	}

	bool InputSystem::onWindowResize(WindowResizeEvent& e) {
		s_WindowWidth = e.getWidth();
		s_WindowHeight = e.getHeight();
		return false;
	}

	bool InputSystem::onMouseMoved(MouseMovedEvent& e) {
		s_Mx = e.getX();
		s_My = e.getY();

		enqueue(InputType::MOUSE_MOVE, 0);

		return false;
	}

	bool InputSystem::onMouseScrolled(MouseScrolledEvent& e) {
		enqueue(InputType::MOUSE_SCROLL, e.getYOffset());
		//SYR_CORE_INFO("{0} {1}", e.getXOffset(), e.getYOffset());
		return false;
	}

	bool InputSystem::onMouseButtonPressed(MouseButtonPressedEvent& e) {
		enqueue(InputType::MOUSE_PRESS, e.getMouseButton());
		//SYR_CORE_INFO(e.getMouseButton());
		return false;
	}

	bool InputSystem::onMouseButtonReleased(MouseButtonReleasedEvent& e) {
		enqueue(InputType::MOUSE_RELEASE, e.getMouseButton());
		//SYR_CORE_INFO(e.getMouseButton());
		return false;
	}

	bool InputSystem::onKeyPressedEvent(KeyPressedEvent& e) {
		enqueue({ InputType::KEY_PRESS, s_Mx, s_My, e.getKeyCode(), (uint8_t) e.getRepeatCount() });
		//SYR_CORE_INFO("F {0} {1}", e.getKeyCode(), e.getRepeatCount());

		/*
		std::map<int, int>::iterator it = s_InputStack.keyPresses.find(e.getKeyCode());
		if (it == s_InputStack.keyPresses.end()) {
			s_InputStack.keyPresses.insert(std::pair<int, int>(e.getKeyCode(), 1));
		} else {
			it->second += e.getRepeatCount();
		}*/

		return false;
	}

	bool InputSystem::onKeyReleasedEvent(KeyReleasedEvent& e) {
		enqueue(InputType::KEY_RELEASE, e.getKeyCode());
		//SYR_CORE_INFO("KR {0}", e.getKeyCode());
		return false;
	}

}