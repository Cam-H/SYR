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
			uint16_t xMod = 1, yMod = 1;

			switch (layout.layout) {
			case Layout::FLOAT:
				for (std::vector<entt::entity>::iterator it = activeElements.begin(); it != activeElements.end(); ++it) {
					glm::vec3 cTranslation;
					glm::decompose(registry.get<TransformComponent>(*it).transform, glm::vec3(), glm::quat(), cTranslation, glm::vec3(), glm::vec4());
					registry.get<TransformComponent>(*it).offset({ 0, 0, z - cTranslation.z });//Manipulate the height of ui components so they do not clip into eachother when rendering
				}
				continue;
				break;
			case Layout::LINEAR:
				if (layout.alignment == Alignment::VERTICAL) {
					width = scale.x - xMargins * 2;
					height = (scale.y - yMargins * 2 - spacing * (elementCount - 1)) / elementCount;
					y += -scale.y / 2 + height / 2 + yMargins;
					yOffset = height + spacing;
				} else if (layout.alignment == Alignment::HORIZONTAL) {
					width = (scale.x - xMargins * 2 - spacing * (elementCount - 1)) / elementCount;
					height = scale.y - yMargins * 2;
					x -= -scale.x / 2 + width / 2 + xMargins;
					xOffset = -(width + spacing);
				}

				xMod = yMod = activeElements.size();
				break;
			case Layout::GRID:

				width = scale.x - xMargins * 2;
				height = scale.y - yMargins * 2;

				if (layout.contentWidth != -1.0f) {
					
				}

				break;
			}

			int i = activeElements.size() - 1;
			for (std::vector<entt::entity>::iterator it = activeElements.begin(); it != activeElements.end(); ++it) {
				Entity::getComponent<TransformComponent>(registry, *it).transform = glm::translate(glm::mat4(1.0f), { x + xOffset * (i % xMod), y + yOffset * (i % yMod), z }) * glm::scale(glm::mat4(1.0f), { width, height, 1.0f });
				i--;
			}
		}
		
		
		auto anchorView = registry.view<AnchorComponent>();
		for (const entt::entity e : anchorView) {
			AnchorComponent& anchor = anchorView.get<AnchorComponent>(e);
			glm::mat4& transform = registry.get<TransformComponent>(e).transform;


			float xOffset = 0, yOffset = 0;
			float xMod = 1, yMod = 1;

			glm::vec3 scale2;
			glm::vec3 cTranslation;
			glm::decompose(transform, scale2, glm::quat(), cTranslation, glm::vec3(), glm::vec4());

			if (anchor.horizontalAnchorHandle != entt::null) {
				glm::mat4& anchorTransform = registry.view<TransformComponent>().get(anchor.horizontalAnchorHandle).transform;

				glm::vec3 scale;
				glm::vec3 translation;
				glm::decompose(anchorTransform, scale, glm::quat(), translation, glm::vec3(), glm::vec4());

				xMod = 0;

				switch (anchor.horizontalAlignment) {
				case Alignment::CENTER:
					break;
				case Alignment::OUTER_LEFT:
					xOffset = -(scale.x / 2 + scale2.x / 2);
					break;
				case Alignment::INNER_LEFT:
					xOffset = -scale.x / 2 + scale2.x / 2;
					break;
				case Alignment::INNER_RIGHT:
					xOffset = scale.x / 2 - scale2.x / 2;
					break;
				case Alignment::OUTER_RIGHT:
					xOffset = scale.x / 2 + scale2.x / 2;
					break;
				case Alignment::FLOAT:
					xMod = 1;
					break;
				default:
					SYR_CORE_WARN("Unrecognized UI Component Alignment: {0}", anchor.horizontalAlignment);
				}

				xOffset += translation.x;
			}

			if (anchor.verticalAnchorHandle != entt::null) {
				glm::mat4& anchorTransform = registry.view<TransformComponent>().get(anchor.verticalAnchorHandle).transform;

				glm::vec3 scale;
				glm::vec3 translation;
				glm::decompose(anchorTransform, scale, glm::quat(), translation, glm::vec3(), glm::vec4());

				yMod = 0;

				switch (anchor.verticalAlignment) {
				case Alignment::CENTER:
					break;
				case Alignment::OUTER_TOP:
					yOffset = scale.y / 2 + scale2.y / 2;
					break;
				case Alignment::INNER_TOP:
					yOffset = scale.y / 2 - scale2.y / 2;
					break;
				case Alignment::INNER_BOTTOM:
					yOffset = -scale.y / 2 + scale2.y / 2;
					break;
				case Alignment::OUTER_BOTTOM:
					yOffset = -(scale.y / 2 + scale2.y / 2);
					break;
				case Alignment::FLOAT:
					yMod = 1;
					break;
				default:
					SYR_CORE_WARN("Unrecognized UI Component Alignment: {0}", anchor.verticalAlignment);

				}

				yOffset += translation.y;
			}

			transform = glm::translate(glm::mat4(1.0f), { cTranslation.x * xMod + xOffset * !xMod, cTranslation.y * yMod + yOffset * !yMod, cTranslation.z }) * glm::scale(glm::mat4(1.0f), scale2);
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

			if (childContainer.size() > 0) {//TODO fill out
				Layout layout = getLayout(header);
				Alignment alignment = getLayoutAlignment(header);
				std::string type = getType(header);

				float margins = getMargins(header);
				float spacing = getSpacing(header);

				float contentWidth = getContentWidth(header);
				float contentHeight = getContentHeight(header);

				if (layout == Layout::GRID) {
					int cols = getCols(header);
					int rows = getRows(header);

					if (cols != -1) {
						contentWidth = (1.0f - 2 * margins - (cols - 1) * spacing) / cols;
					} else if (rows != -1) {
						contentHeight = (1.0f - 2 * margins - (rows - 1) * spacing) / rows;
					}
				}

				entity.addComponent<LayoutComponent>(layout, alignment, childContainer);
				entity.getComponent<LayoutComponent>().setScrollable(type.compare("scroll") == 0);//TODO warning

				entity.getComponent<LayoutComponent>().setMargins(margins);
				entity.getComponent<LayoutComponent>().setSpacing(spacing);

				entity.getComponent<LayoutComponent>().contentWidth = contentWidth;
				entity.getComponent<LayoutComponent>().contentHeight = contentHeight;

				SYR_CORE_ERROR("{0} {1}", entity.getComponent<LayoutComponent>().contentWidth, entity.getComponent<LayoutComponent>().contentHeight);
			}
		}

		std::string id = getID(header);
		std::string text = getText(header);

		glm::vec4 color = getColor(header);
		glm::vec4 highlightColor = getHighlightColor(header);
		glm::vec4 selectColor = getSelectColor(header);

		entity.getComponent<TagComponent>().id = id;

		entity.addComponent<UiComponent>();
		entity.getComponent<UiComponent>().baseColor = color;
		entity.getComponent<UiComponent>().highlightColor = highlightColor;
		entity.getComponent<UiComponent>().selectColor = selectColor;

		if (tag.compare("button") == 0) {//Button
			entity.addComponent<ListenerComponent>();
		}

		if (!text.empty()) {//Label / Text
			entity.addComponent<TextComponent>(text, getFont(header), getTextColor(header));
		}

		/***********************UI ANCHORING****************************/
		entity.addComponent<AnchorComponent>();

		std::string horizontalAnchorID = getHorizontalAnchor(header);
		if (!horizontalAnchorID.empty()) {
			entity.getComponent<AnchorComponent>().setHorizontalAnchor(horizontalAnchorID, getHorizontalAlignment(header));
		}

		std::string verticalAnchorID = getVerticalAnchor(header);
		if (!verticalAnchorID.empty()) {
			entity.getComponent<AnchorComponent>().setVerticalAnchor(verticalAnchorID, getVerticalAlignment(header));
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

	std::string UiSystem::getString(std::string header, std::string attribute) {
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;
			return header.substr(index, header.find('\"', index) - index);
		}

		return std::string();
	}

	float UiSystem::getPreferredWidth(std::string header) {
		std::string widthPref = getString(header, "preferredWidth");

		return widthPref.empty() ? -1.0f : std::stof(widthPref);
	}

	float UiSystem::getPreferredHeight(std::string header) {
		std::string heightPref = getString(header, "preferredHeight");

		return heightPref.empty() ? -1.0f : std::stof(heightPref);
	}

	Layout UiSystem::getLayout(std::string header) {
		std::string attribute = "layout=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;
			std::string layoutType = header.substr(index, header.find('\"', index) - index);

			if (layoutType.compare("Linear") == 0) {
				return Layout::LINEAR;
			}
			else if (layoutType.compare("Grid") == 0) {
				return Layout::GRID;
			}
			else if (layoutType.compare("Float") != 0) {
				SYR_CORE_WARN("Unrecognized layout: {0}. Using float layout by default", layoutType);
			}
		}

		return Layout::FLOAT;
	}

	Alignment UiSystem::getLayoutAlignment(std::string header) {
		std::string attribute = "layoutAlignment=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;
			std::string alignmentType = header.substr(index, header.find('\"', index) - index);
			
			SYR_CORE_INFO("---> {0}", alignmentType);

			if (alignmentType.compare("Float") == 0) {
				return Alignment::FLOAT;
			}
			else if (alignmentType.compare("Center") == 0) {
				return Alignment::CENTER;
			}
			else if (alignmentType.compare("Horizontal") == 0) {
				return Alignment::HORIZONTAL;
			}
			else if (alignmentType.compare("Vertical") == 0) {
				return Alignment::VERTICAL;
			}
			else if (alignmentType.compare("OuterLeft") == 0) {
				return Alignment::OUTER_LEFT;
			}
			else if (alignmentType.compare("InnerLeft") == 0) {
				return Alignment::INNER_LEFT;
			}
			else if (alignmentType.compare("InnerRight") == 0) {
				return Alignment::INNER_RIGHT;
			}
			else if (alignmentType.compare("OuterRight") == 0) {
				return Alignment::OUTER_RIGHT;
			}
			else if (alignmentType.compare("OuterTop") == 0) {
				return Alignment::OUTER_TOP;
			}
			else if (alignmentType.compare("InnerTop") == 0) {
				return Alignment::INNER_TOP;
			}
			else if (alignmentType.compare("InnerBottom") == 0) {
				return Alignment::INNER_BOTTOM;
			}
			else if (alignmentType.compare("OuterBottom") == 0) {
				return Alignment::OUTER_BOTTOM;
			} 
		}

		return Alignment::FLOAT;
	}

	std::string UiSystem::getType(std::string header) {
		return getString(header, "type=");
	}

	float UiSystem::getContentWidth(std::string header) {
		std::string widthPref = getString(header, "contentWidth");

		return widthPref.empty() ? -1.0f : std::stof(widthPref);
	}

	float UiSystem::getContentHeight(std::string header) {
		std::string heightPref = getString(header, "contentHeight");

		return heightPref.empty() ? -1.0f : std::stof(heightPref);
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

	int UiSystem::getCols(std::string header) {
		std::string attribute = "cols=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;

			return std::stoi(header.substr(index, header.find('\"') - index));
		}

		return -1;
	}
	int UiSystem::getRows(std::string header) {
		std::string attribute = "rows=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;

			return std::stoi(header.substr(index, header.find('\"') - index));
		}

		return -1;
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

	std::string UiSystem::getHorizontalAnchor(std::string header) {
		std::string attribute = "xAnchor=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;
			return header.substr(index, header.find('\"', index) - index);
		}

		return std::string();
	}

	Alignment UiSystem::getHorizontalAlignment(std::string header) {
		std::string attribute = "xAlign=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;
			std::string alignmentType = header.substr(index, header.find('\"', index) - index);

			if (alignmentType.compare("CENTER") == 0) {
				return Alignment::CENTER;
			}
			else if (alignmentType.compare("OUTER_LEFT") == 0) {
				return Alignment::OUTER_LEFT;
			}
			else if (alignmentType.compare("INNER_LEFT") == 0) {
				return Alignment::INNER_LEFT;
			}
			else if (alignmentType.compare("OUTER_RIGHT") == 0) {
				return Alignment::OUTER_RIGHT;
			}
			else if (alignmentType.compare("INNER_RIGHT") == 0) {
				return Alignment::INNER_RIGHT;
			}
			else if (alignmentType.compare("FLOAT") != 0) {
				SYR_CORE_WARN("Unrecognized alignment: {0}. Using float alignment by default", alignmentType);
			}
		}

		return Alignment::FLOAT;
	}

	std::string UiSystem::getVerticalAnchor(std::string header) {
		std::string attribute = "yAnchor=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;
			return header.substr(index, header.find('\"', index) - index);
		}

		return std::string();
	}

	Alignment UiSystem::getVerticalAlignment(std::string header) {
		std::string attribute = "yAlign=";
		int index = header.find(attribute, 0);

		if (index != std::string::npos) {
			index += attribute.length() + 1;
			std::string alignmentType = header.substr(index, header.find('\"', index) - index);

			if (alignmentType.compare("CENTER") == 0) {
				return Alignment::CENTER;
			}
			else if (alignmentType.compare("OUTER_TOP") == 0) {
				return Alignment::OUTER_TOP;
			}
			else if (alignmentType.compare("INNER_TOP") == 0) {
				return Alignment::INNER_TOP;
			}
			else if (alignmentType.compare("OUTER_BOTTOM") == 0) {
				return Alignment::OUTER_BOTTOM;
			}
			else if (alignmentType.compare("INNER_BOTTOM") == 0) {
				return Alignment::INNER_BOTTOM;
			}
			else if (alignmentType.compare("FLOAT") != 0) {
				SYR_CORE_WARN("Unrecognized alignment: {0}. Using float alignment by default", alignmentType);
			}
		}

		return Alignment::FLOAT;
	}


	bool UiSystem::containsXMLElements(const std::string& content) {
		return content.find("</") != std::string::npos;
	}
	
	bool UiSystem::rootIsElement(const std::string& content, const std::string& element) {
		size_t pos = content.find_last_of("</") + 1;
		return pos != std::string::npos ? (content.substr(pos, content.length() - pos - 1).compare(element) == 0) : false;
	}

}