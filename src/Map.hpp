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
            GROUND_LOOP,
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
            WALLS_TOP_LEFT_BOTTOM_CORNER_TL_BL,
            WALLS_TOP_LEFT_BOTTOM_RIGHT_CORNER_TL_BL_TR,
            WALLS_TOP_LEFT_BOTTOM_RIGHT_CORNER_TL_BL_TR_BR,
            WALLS_TOP_LEFT_RIGHT_CORNER_TL_TR,
            WALLS_LEFT_CORNER_TR,
            WALL_BOTTOM_LEFT_CORNER,
            WALL_TOP_RIGHT_CORNER,
            WALL_TOP_LEFT_CORNER,
            WALL_LOOP,
            EDGE_LR,
            EDGE_TB,
            EDGE_WT_GL,
            EDGE_WT_GL1,
            EDGE_WT_GL2,
            EDGE_WT_GL3,
            EDGE_WT_GL4,
            EDGE_WT_GL5,
            EDGE_WT_GL6,
            EDGE_WT_GL7,
            EDGE_WT_GL8,
            EDGE_WT_GL9,
            EDGE_WT_GL10,
            EDGE_WT_GL11,
            EDGE_WT_GL12,
            EDGE_WT_GL13,
            EDGE_WT_GL14,
            EDGE_WT_GL15,
            EDGE_WT_GL16,
            EDGE_WT_GL17,
            EDGE_WT_GL18,
            EDGE_WT_GL19,
            EDGE_WT_GL20,
            EDGE_WT_GL21,
            EDGE_WT_GL22,
            EDGE_WT_GL23,
            EDGE_WT_GL24,
            EDGE_WT_GL25,
            EDGE_WT_GL26,
            EDGE_WT_GL27,
            EDGE_WT_GL28,
            EDGE_WT_GL29,
            EDGE_WT_GL30,
            EDGE_WT_GL31,
            EDGE_WT_GL32,
            EDGE_WT_GL33,
            EDGE_WT_GL34,
            EDGE_WT_GL35,
            EDGE_WT_GL36,
            EDGE_ALL,
        };

        Type       type       = Type::OUTER;
        bool       isSelected = false;
        sf::Sprite sprite     = {};
    };

    sf::RenderTexture          renderTexture;
    std::vector<sf::FloatRect> wallTiles;

    sf::Vector2i _size               = {40, 22};
    float        _chanceToStartAlive = 45;
    int          _birthLimit         = 4;
    int          _deathLimit         = 3;
    int          _steps              = 3;

    // std::vector<std::vector<bool>> _map;
    bool **_map    = nullptr;
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
    void   update();
    void   render();
    void   redraw();
    void   printMap();

  private:
    void PushTile(sf::Vector2f position, Tile::Type type);
    int  countNeighbours(int x, int y);
};
