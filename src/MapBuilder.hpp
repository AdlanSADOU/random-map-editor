#pragma once

#include "Map.hpp"
#include "imgui/imgui-SFML.h"
#include "imgui/imgui.h"

static sf::Sprite gridSprite;

void InitMapBuilder(sf::RenderWindow &window, Map &map);
void updateMapBuilder(sf::RenderWindow &window, Map &map);
void renderMapBuilder(sf::RenderWindow &window);
void redrawMapBuilder();

void renderMapBuilderGrid(sf::RenderWindow &window, Map &map);
void splitSpriteSheet();