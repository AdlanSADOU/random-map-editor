#pragma once
#include <cstdio>
#include <iostream>
#include <vector>

#include "SFML/Graphics.hpp"

class Map : public sf::Sprite
{
  public:
    struct Tile {
        enum Type
        {
            GROUND,
            TOP,
            LEFT,
            BOTTOM,
            RIGHT,
            TOP_LEFT,
            TOP_RIGHT,
            LEFT_RIGHT,
            TOP_BOTTOM,
            BOTTOM_LEFT,
            BOTTOM_RIGHT,
            TOP_LEFT_RIGHT,
            LEFT_TOP_BOTTOM,
            BOTTOM_LEFT_RIGHT,
            RIGHT_TOP_BOTTOM,
            OUTER,
            WALL,
            WALL_TOP,
            WALL_LEFT,
            WALL_BOTTOM,
            WALL_RIGHT,
            WALL_TOP_LEFT,
            WALL_TOP_RIGHT,
            WALL_BOTTOM_LEFT,
            WALL_BOTTOM_RIGHT,
            WALL_TOP_BOTTOM,
            WALL_LEFT_RIGHT,
            WALL_TOP_LEFT_RIGHT,
            WALL_LEFT_TOP_BOTTOM,
            WALL_BOTTOM_LEFT_RIGHT,
            WALL_RIGHT_TOP_BOTTOM,
            WALLS_TOP_LEFT_CORNER_TL,
            WALL_BOTTOM_LEFT_CORNER,
            WALL_TOP_RIGHT_CORNER,
            WALL_TOP_LEFT_CORNER,
            WALL_LOOP
        };

        Type type = Type::OUTER;
        sf::Sprite sprite;
        bool isSelected = false;
    };

    sf::RenderTexture renderTexture;
    std::vector<sf::FloatRect> wallTiles;

    float _chanceToStartAlive = 45;
    sf::Vector2i _size = {40, 22};
    int _birthLimit = 4;
    int _deathLimit = 3;
    int _steps = 3;

    // std::vector<std::vector<bool>> _map;
    bool **_map = nullptr;
    bool **_mapTmp = nullptr;

    std::vector<Tile> mapTiles;
    // sf::Sprite renSprite;
    sf::View renTexView = renderTexture.getView();

    sf::Texture floorsTileset;
    sf::Texture wallsTileset;

  public:
    Map(){};
    ~Map(){};

    bool **create(sf::Vector2u windowSize, sf::Vector2i size, int birthLimit, int deathLimit, int steps, int chanceToStartAlive);
    bool **generate();
    void update();
    void render();
    void redraw();
    void printMap();

  private:
    void PushTile(sf::Vector2f position, Tile::Type type);
    int countNeighbours(int x, int y);
};
