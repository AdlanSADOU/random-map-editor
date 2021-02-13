/**
 * This prototype will have the following features:
 * - Animation class
 * - Character animation speed will be adjusted with characters run speed
 * - inventory system
 * - equipment system
 * - Clothing system: characters equipment will be visible on him
**/

#pragma once

#include <iostream>

#include "Character.hpp"
#include "SFML/Graphics.hpp"
#include "Utils.hpp"

static float deltaTime;
static float fps;
static float speed = 0.1f;
static float xAxis = 0, yAxis = 0;
static float zoom = 1;
static zz::Controls controls;
static sf::RenderWindow window;
static Character skeleton;
static Map map;

void create();
void run();

void onKeyUp(sf::Keyboard::Key key);
void onKeyHeld(sf::Keyboard::Key key);
