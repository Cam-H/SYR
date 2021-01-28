#include "syrpch.h"
#include "UiSystem.h"

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "SYR/Renderer/Renderer.h"
#include "SYR/Renderer/Renderer2D.h"


namespace SYR {

	void positionComponents(entt::registry& registry) {
		auto view = registry.view<TransformComponent, LayoutComponent>();

		for (const entt::entity e : view) {
			glm::mat4& transform = view.get<TransformComponent>(e).transform;
			LayoutComponent& layout = view.get<LayoutComponent>(e);

			std::vector<entt::entity> activeElements = UiSystem::getActiveEntities(registry, layout.entities);

			int elementCount = activeElements.size();

			glm::vec3 scale;
			glm::vec3 translation;
			glm::decompose(transform, scale, glm::quat(), translation, glm::vec3(), glm::vec4());

			float xMargins = scale.x * layout.margins;
			float yMargins = scale.y * layout.margins;

			float spacing = scale.y * layout.spacing;

			float x = translation.x + layout.offset.x;
			float y = translation.y + layout.offset.y;
			float z = translation.z + 0.001f;

			float width = 0, height = 0;
			float xOffset = 0, yOffset = 0;

			switch (layout.layout) {
			case Layout::FLOAT:
				for (std::vector<entt::entity>::iterator it = activeElements.begin(); it != activeElements.end(); ++it) {
					glm::vec3 cTranslation;
					glm::decompose(registry.get<TransformComponent>(*it).transform, glm::vec3(), glm::quat(), cTranslation, glm::vec3(), glm::vec4());
					registry.get<TransformComponent>(*it).offset({ 0, 0, z - cTranslation.z });
				}
				continue;
				break;
			case Layout::LINEAR_VERTICAL:
				width = scale.x - xMargins * 2;
				height = (scale.y - yMargins * 2 - spacing * (elementCount - 1)) / elementCount;
				y += -scale.y / 2 + height / 2 + yMargins;
				yOffset = height + spacing;
				break;
			case Layout::LINEAR_HORIZONTAL:
				width = (scale.x - xMargins * 2 - spacing * (elementCount - 1)) / elementCount;
				height = scale.y - yMargins * 2;
				x += -scale.x / 2 + width / 2 + xMargins;
				xOffset = width + spacing;
				break;
			}

			int i = activeElements.size() - 1;
			for (std::vector<entt::entity>::iterator it = activeElements.begin(); it != activeElements.end(); ++it) {
				Entity::getComponent<TransformComponent>(registry, *it).transform = glm::translate(glm::mat4(1.0f), { x + xOffset * i, y + yOffset * i, z }) * glm::scale(glm::mat4(1.0f), { width, height, 1.0f });
				i--;
			}
		}
		
		auto uiView = registry.view<UiComponent>();
		for (const entt::entity e : uiView) {
			UiComponent& ui = uiView.get<UiComponent>(e);
			glm::mat4& transform = registry.get<TransformComponent>(e).transform;

			if (ui.anchorHandle != entt::null) {
				glm::mat4& anchor = registry.view<TransformComponent>().get(ui.anchorHandle).transform;

				glm::vec3 scale;
				glm::vec3 translation;
				glm::decompose(anchor, scale, glm::quat(), translation, glm::vec3(), glm::vec4());

				glm::vec3 scale2;
				glm::vec3 cTranslation;
				glm::decompose(transform, scale2, glm::quat(), cTranslation, glm::vec3(), glm::vec4());

				float xOffset = 0, yOffset = 0;
				float xMod = 0, yMod = 0;

				switch (ui.horizontalAlignment) {
				case UiAlignment::CENTER:
					break;
				case UiAlignment::OUTER_LEFT:
					xOffset = -(scale.x / 2 + scale2.x / 2);
					break;
				case UiAlignment::INNER_LEFT:
					xOffset = -scale.x / 2 + scale2.x / 2;
					break;
				case UiAlignment::INNER_RIGHT:
					xOffset = scale.x / 2 - scale2.x / 2;
					break;
				case UiAlignment::OUTER_RIGHT:
					xOffset = scale.x / 2 + scale2.x / 2;
					break;
				case UiAlignment::FLOAT:
					xMod = 1;
					break;
				default:
					SYR_CORE_WARN("Unrecognized UI Component Alignment: {0}", ui.horizontalAlignment);
				}

				switch (ui.verticalAlignment) {
				case UiAlignment::CENTER:
					break;
				case UiAlignment::OUTER_TOP:
					yOffset = scale.y / 2 + scale2.y / 2;
					break;
				case UiAlignment::INNER_TOP:
					yOffset = scale.y / 2 - scale2.y / 2;
					break;
				case UiAlignment::INNER_BOTTOM:
					yOffset = -scale.y / 2 + scale2.y / 2;
					break;
				case UiAlignment::OUTER_BOTTOM:
					yOffset = -(scale.y / 2 + scale2.y / 2);
					break;
				case UiAlignment::FLOAT:
					yMod = 1;
					break;
				default:
					SYR_CORE_WARN("Unrecognized UI Component Alignment: {0}", ui.verticalAlignment);

				}

				transform = glm::translate(glm::mat4(1.0f), { cTranslation.x * xMod + (translation.x + xOffset) * !xMod, cTranslation.y * yMod + (translation.y + yOffset) * !yMod, cTranslation.z }) * glm::scale(glm::mat4(1.0f), scale2);
			}
		}
	}

	std::vector<entt::entity> UiSystem::getActiveEntities(entt::registry& registry, std::vector<entt::entity> entities) {
		std::vector<entt::entity> active;

		for (std::vector<entt::entity>::iterator it = entities.begin(); it != entities.end(); ++it) {
			if (!registry.has<UiComponent>(*it) || registry.get<UiComponent>(*it).visibility != UiVisibility::GONE) {
				active.push_back(*it);
			}
		}

		return active;
	}


	//Negates any user inputs applied to non-visible components
	void UiSystem::processUiInputs(entt::registry& registry) {
		auto view = registry.view<LayoutComponent>();

		for (entt::entity e : view) {
			if (registry.has<UiComponent>(e) && registry.get<UiComponent>(e).visibility != UiVisibility::VISIBLE) {
				LayoutComponent& layout = view.get<LayoutComponent>(e);

				for (std::vector<entt::entity>::iterator it = layout.entities.begin(); it != layout.entities.end(); ++it) {
					if (registry.has<ListenerComponent>(*it)) {
						ListenerComponent& listener = registry.get<ListenerComponent>(*it);

						listener.hovered = listener.selected = listener.checked = false;
					}
				}
			}
			
		}

	}


	void renderUi(entt::registry& registry, entt::entity root) {
		static Ref<Texture2D> texture = Texture2D::create("assets/textures/Empty.png");

		UiComponent& ui = registry.get<UiComponent>(root);

		if (ui.visibility == UiVisibility::VISIBLE) {
			glm::mat4& transform = registry.get<TransformComponent>(root).transform;

			if (registry.has<ListenerComponent>(root)) {
				if (registry.get<ListenerComponent>(root).selected || (!registry.get<ListenerComponent>(root).hovered && registry.get<ListenerComponent>(root).checked)) {
					Renderer2D::drawQuad(transform, ui.selectColor, texture);
				} else if (registry.get<ListenerComponent>(root).hovered) {
					if (registry.get<ListenerComponent>(root).checked && false) {
						Renderer2D::drawQuad(transform, { ui.highlightColor.r * ui.selectColor.r, ui.highlightColor.g * ui.selectColor.g , ui.highlightColor.b * ui.selectColor.b, 1.0f }, texture);
					} else {
						Renderer2D::drawQuad(transform, ui.highlightColor, texture);
					}
				} else {
					Renderer2D::drawQuad(transform, ui.baseColor, texture);
				}
			} else {
				Renderer2D::drawQuad(transform, ui.baseColor, texture);
			}

			if (registry.has<TextComponent>(root)) {
				TextComponent& text = registry.get<TextComponent>(root);
				glm::vec3 translation;
				glm::decompose(transform, glm::vec3(), glm::quat(), translation, glm::vec3(), glm::vec4());
				translation.z += 0.001f;

				Renderer2D::drawText(Renderer::getCharacterSetLibrary()->get(text.characterSetName), Renderer2D::TextAlignment::HORIZONTAL_CENTER, text.text, translation, text.textColor);
			}

			if (registry.has<LayoutComponent>(root)) {
				LayoutComponent& layout = registry.get<LayoutComponent>(root);
				for (std::vector<entt::entity>::iterator it = layout.entities.begin(); it != layout.entities.end(); ++it) {
					renderUi(registry, *it);
				}
			}
			
		}
	}

	void renderUiComponent(entt::registry& registry, const entt::entity uiComponent) {
		glm::mat4& transform = registry.get<TransformComponent>(uiComponent).transform;
		

	}

	void UiSystem::loadUiResources(std::map<std::string, uint32_t>* resources, const std::string& content) {

		std::string header = content.substr(content.find('<', 0), content.find('>', 0) + 1);
		std::string tag = header.substr(1, (header.find('>', 1) < header.find(' ', 1) ? header.find('>', 1) : header.find(' ', 1)) - 1);
		std::string body = content.substr(header.length(), content.find_last_of("</") - header.length() - 1);

		if (tag.compare("res") == 0) {
			std::vector<std::string> children = splitXMLSiblings(body);

			for (std::vector<std::string>::iterator it = children.begin(); it != children.end(); ++it) {
				header = it->substr(it->find('<', 0), it->find('>', 0) + 1);
				tag = header.substr(1, (header.find('>', 1) < header.find(' ', 1) ? header.find('>', 1) : header.find(' ', 1)) - 1);

				std::string id = getID(header);

				if (resources->find(id) != resources->end()) {
					SYR_CORE_WARN("Another resource already exists with ID: {0}. The resource was not loaded!", id);
					continue;
				}

				if (tag.compare("charset") == 0) {
					Renderer::getCharacterSetLibrary()->add(id, CharacterSet::create(getSource(header), CharacterSet::getCollectionByName(getCollection(header)), getSize(header)));
				} else if (tag.compare("animation") == 0) {

				}

				SYR_CORE_INFO("{0}: {1}", tag, header);
			}
		}
	}

	void UiSystem::loadUiElements(Scene* scene, std::vector<entt::entity>* container, const std::string& content) {

		static bool inUI = false;

		std::string header = content.substr(content.find('<', 0), content.find('>', 0) + 1);
		std::string tag = header.substr(1, (header.find('>', 1) < header.find(' ', 1) ? header.find('>', 1) : header.find(' ', 1)) - 1);
		std::string body = content.substr(header.length(), content.find_last_of("</") - header.length() - 1);

		//Skip parent nodes: ui
		if (tag.compare("ui") == 0) {
			inUI = true;

			std::vector<std::string> children = splitXMLSiblings(body);
			for (std::vector<std::string>::iterator it = children.begin(); it != children.end(); ++it) {
				loadUiElements(scene, container, *it);
			}

			inUI = false;
			return;
		}

		if (!inUI)
			return;

		Entity entity = scene->createEntity();
		container->push_back(entity.getHandle());

		std::vector<std::string> children = splitXMLSiblings(body);

		if (children.size() > 0) {
			std::vector<entt::entity> childContainer(0);

			if (tag.compare("panel") == 0) {
				for (std::vector<std::string>::iterator it = children.begin(); it != children.end(); ++it) {
					loadUiElements(scene, &childContainer, *it);
				}
			}
			else {
				SYR_CORE_WARN("Discarding {0} XML Elements. Only panels may contain other elements!", children.size());
			}

			if (childContainer.size() > 0) {
				Layout layout = getLayout(header);

				float margins = getMargins(header);
				float spacing = getSpacing(header);

				SYR_CORE_INFO("{0} {1} {2}", layout, margins, spacing);

				entity.addComponent<LayoutComponent>(layout, childContainer);
				entity.getComponent<LayoutComponent>().setMargins(margins);
				entity.getComponent<LayoutComponent>().setSpacing(spacing);
			}
		}

		std::string id = getID(header);
		std::string text = getText(header);

		glm::vec4 color = getColor(header);
		glm::vec4 highlightColor = getHighlightColor(header);
		glm::vec4 selectColor = getSelectColor(header);

		entity.getComponent<TagComponent>().id = id;

		entity.addComponent<UiComponent>();
		entity.getComponent<UiComponent>().anchorHandle = entt::null;
		entity.getComponent<UiComponent>().baseColor = color;
		entity.getComponent<UiComponent>().highlightColor = highlightColor;
		entity.getComponent<UiComponent>().selectColor = selectColor;

		if (tag.compare("button") == 0) {//Button
			entity.addComponent<ListenerComponent>();
		}

		if (!text.empty()) {//Label / Text
			entity.addComponent<TextComponent>(text, getFont(header), getTextColor(header));
		}
	}

	std::vector<std::string> UiSystem::splitXMLSiblings(const std::string& content) {
		std::vector<std::string> siblings;

		const char* delimiter = "\n";

		size_t pos = 0;

		while ((pos = content.find("<", pos)) != std::string::npos) {
			if (content.at(pos + 1) == '/') { pos++;  continue; }

			std::string tag = content.substr(pos + 1, (content.find('>', pos) < content.find(' ', pos) ? content.find('>', pos) : content.find(' ', pos)) - pos - 1);

			size_t end = pos + 1;
			uint16_t tagCount = 1;

			while (tagCount > 0) {
				end = content.find(tag, end + 1);

				if (end == std::string::npos) {
					SYR_CORE_ERROR("Unable to generate UI due to dangling tag in source! Look for missing closing statement: </{0}>", tag);
					return std::vector<std::string>(0);
				}

				tagCount += content.at(end - 1) == '/' ? -1 : 1;
			}

			std::string body = content.substr(pos, end - pos + tag.length() + 1);
			siblings.push_back(body);

			pos = end;
		}

		return siblings;
	}

	Layout UiSystem::getLayout(std::string header) {
		std::string attribute = "layout=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;
			std::string layoutType = header.substr(index, header.find('\"', index) - index);

			if (layoutType.compare("LinearVertical") == 0) {
				return Layout::LINEAR_VERTICAL;
			}
			else if (layoutType.compare("LinearHorizontal") == 0) {
				return Layout::LINEAR_HORIZONTAL;
			}
			else if (layoutType.compare("Grid") == 0) {

			}
			else if (layoutType.compare("Float") != 0) {
				SYR_CORE_WARN("Unrecognized layout: {0}. Using float layout by default", layoutType);
			}
		}

		return Layout::FLOAT;
	}

	std::string UiSystem::getString(std::string header, std::string attribute) {
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;
			return header.substr(index, header.find('\"', index) - index);
		}

		return std::string();
	}

	std::string UiSystem::getID(std::string header) {
		return getString(header, "id=");
	}

	float UiSystem::getMargins(std::string header) {
		std::string attribute = "margins=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;

			return std::stof(header.substr(index, header.find('\"') - index));
		}

		return 0.1f;
	}
	float UiSystem::getSpacing(std::string header) {
		std::string attribute = "spacing=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;

			return std::stof(header.substr(index, header.find('\"') - index));
		}

		return 0.05f;
	}

	glm::vec4 UiSystem::getColor(std::string header) {
		std::string attribute = "color=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;
			std::string color = header.substr(index, header.find('\"', index) - index);
			return { stoi(color.substr(2, 2), 0, 16) / 255.0f, stoi(color.substr(4, 2), 0, 16) / 255.0f, stoi(color.substr(6, 2), 0, 16) / 255.0f, color.length() == 10 ? stoi(color.substr(8, 2), 0, 16) / 255.0f : 1.0f };
		}

		return { 0.5f, 0.5f, 0.5f, 1.0f };
	}
	glm::vec4 UiSystem::getHighlightColor(std::string header) {
		std::string attribute = "highlight=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;
			std::string color = header.substr(index, header.find('\"', index) - index);
			return { stoi(color.substr(2, 2), 0, 16) / 255.0f, stoi(color.substr(4, 2), 0, 16) / 255.0f, stoi(color.substr(6, 2), 0, 16) / 255.0f, color.length() == 10 ? stoi(color.substr(8, 2), 0, 16) / 255.0f : 1.0f };
		}

		return { 0.0f, 1.0f, 1.0f, 1.0f };
	}
	glm::vec4 UiSystem::getSelectColor(std::string header) {
		std::string attribute = "select=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;
			std::string color = header.substr(index, header.find('\"', index) - index);
			return { stoi(color.substr(2, 2), 0, 16) / 255.0f, stoi(color.substr(4, 2), 0, 16) / 255.0f, stoi(color.substr(6, 2), 0, 16) / 255.0f, color.length() == 10 ? stoi(color.substr(8, 2), 0, 16) / 255.0f : 1.0f };
		}

		return { 0.0f, 0.6f, 0.6f, 1.0f };
	}

	std::string UiSystem::getText(std::string header) {
		return getString(header, "text=");
	}

	std::string UiSystem::getFont(std::string header) {
		std::string font = getString(header, "font=");

		return font.empty() ? "DEFAULT" : font;
	}

	std::string UiSystem::getSource(std::string header) {
		return getString(header, "src=");
	}
	std::string UiSystem::getCollection(std::string header) {
		std::string collection = getString(header, "collection=");

		return collection.empty() ? "ASCII" : collection;
	}

	uint32_t UiSystem::getSize(std::string header) {
		std::string attribute = "size=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;

			return std::stoi(header.substr(index, header.find('\"') - index));
		}

		return 32;
	}

	glm::vec4 UiSystem::getTextColor(std::string header) {
		std::string attribute = "textcolor=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;
			std::string color = header.substr(index, header.find('\"', index) - index);
			return { stoi(color.substr(2, 2), 0, 16) / 255.0f, stoi(color.substr(4, 2), 0, 16) / 255.0f, stoi(color.substr(6, 2), 0, 16) / 255.0f, color.length() == 10 ? stoi(color.substr(8, 2), 0, 16) / 255.0f : 1.0f };
		}

		return { 0.0f, 0.0f, 0.0f, 1.0f };
	}



	bool UiSystem::containsXMLElements(const std::string& content) {
		return content.find("</") != std::string::npos;
	}
	
	bool UiSystem::rootIsElement(const std::string& content, const std::string& element) {
		size_t pos = content.find_last_of("</") + 1;
		return pos != std::string::npos ? (content.substr(pos, content.length() - pos - 1).compare(element) == 0) : false;
	}

}