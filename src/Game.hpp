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

struct Game {
    float deltaTime;
    float fps;
    float speed = 0.1f;

    float xAxis = 0, yAxis = 0;

    float zoom = 1;

    zz::Controls controls;
    sf::RenderWindow window;
    Character skeleton;
    Map map;

    void create();
    void run();

  private:
    void onKeyUp(sf::Keyboard::Key key);
    void onKeyHeld(sf::Keyboard::Key key);

}; // namespace Game
