#pragma once

#include <entt.hpp>
#include <glm/glm.hpp>

#include "SYR/FileIO/FileHandler.h"

#include "SYR/Scene/Components.h"
#include "SYR/Scene/Entity.h"

namespace SYR {

	class UiSystem {
	public:
		static Entity loadPredefinedUi(Scene* scene, const std::string& filepath) {
			std::string content = FileHandler::readFile(filepath);

			std::vector<std::string> fields = splitXMLSiblings(content);

			std::map<std::string, uint32_t> resources;

			Entity mainPanel = scene->createEntity();


			for (std::vector<std::string>::iterator it = fields.begin(); it != fields.end(); ++it) {
				if (rootIsElement(*it, "res")) {
					loadUiResources(&resources, *it);
				} else if (rootIsElement(*it, "ui")) {
					std::vector<entt::entity> panels;
					loadUiElements(scene, &panels, *it);

					//entity.getComponent<TransformComponent>().transform = glm::tra
					mainPanel.getComponent<TransformComponent>().offset({ 0.0f, 0.0f, 0.9f });//---------------------------------------------------------

					mainPanel.getComponent<TagComponent>().id = "MAIN PANEL";
					mainPanel.getComponent<TagComponent>().tag = "MAIN PANEL";
					mainPanel.getComponent<TagComponent>().status = Status::VISIBLE;

					mainPanel.addComponent<LayoutComponent>(Layout::FLOAT, Alignment::FLOAT, panels);
					mainPanel.addComponent<UiComponent>();

					mainPanel.getComponent<UiComponent>().baseColor = { 0.3f, 0.3f, 0.3f, 0.0f };
				}
			}

			return mainPanel;
		}

		static Entity createUiElement(Scene* activeScene, Entity anchor, Alignment alignment) {
			Entity entity = activeScene->createEntity();
			entity.addComponent<UiComponent>();
			//entity.getComponent<SYR::UiComponent>().baseColor = glm::vec4(i / 8.0f, 1.0f - i / 8.0f, 0.0f, 1.0f);
			//entity.getComponent<SYR::TransformComponent>().scale(0.25f + i / 80.0f);

			return entity;
		}

		static std::vector<entt::entity> getActiveEntities(entt::registry& registry, std::vector<entt::entity> entities);


	private:

		static void loadUiResources(std::map<std::string, uint32_t>* resources, const std::string& content);

		static void loadUiElements(Scene* scene, std::vector<entt::entity>* container, const std::string& content, entt::entity parent = entt::null);

		static std::vector<std::string> splitXMLSiblings(const std::string& content);

		static std::string getString(std::string header, std::string attribute);
		static float getFloat(std::string header, std::string attribute);

		static bool getBoolean(std::string header, std::string attribute);

		static float getPreferredWidth(std::string header);
		static float getPreferredHeight(std::string header);

		static Layout getLayout(std::string header);
		static Alignment getLayoutAlignment(std::string header);
		static std::string getType(std::string header);

		static float getContentWidth(std::string header);
		static float getContentHeight(std::string header);

		static std::string getID(std::string header);

		static float getMargins(std::string header);
		static float getSpacing(std::string header);

		static int getCols(std::string header);
		static int getRows(std::string header);

		static glm::vec4 getColor(std::string header);
		static glm::vec4 getHighlightColor(std::string header);
		static glm::vec4 getSelectColor(std::string header);

		static std::string getText(std::string header);
		static std::string getFont(std::string header);
		static glm::vec4 getTextColor(std::string header);

		static uint32_t getSize(std::string header);

		static std::string getSource(std::string header);
		static std::string getCollection(std::string header);

		static std::string getHorizontalAnchor(std::string header);
		static Alignment getHorizontalAlignment(std::string header);

		static std::string getVerticalAnchor(std::string header);
		static Alignment getVerticalAlignment(std::string header);

		static bool containsXMLElements(const std::string& content);
		static bool rootIsElement(const std::string& content, const std::string& element);
	};

	
	void positionComponents(entt::registry& registry);
	void renderUi(entt::registry& registry, entt::entity root);
	void renderUi(entt::registry& registry, entt::entity root, uint16_t layer);
	void renderUiComponent(entt::registry& registry, entt::entity uiComponent);


}
