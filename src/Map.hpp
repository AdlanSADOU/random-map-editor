#pragma once
#include <cstdio>
#include <iostream>
#include <vector>

// #include "Character.hpp"
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
            WALL_TOP_LEFT_RIGHT,
            WALL_LEFT_TOP_BOTTOM,
            WALL_BOTTOM_LEFT_RIGHT,
            WALL_RIGHT_TOP_BOTTOM,
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

    bool **create(sf::Vector2u windowSize, sf::Vector2i size, int birthLimit, int deathLimit, int steps, int chanceToStartAlive)
    {
        this->_chanceToStartAlive = chanceToStartAlive;
        this->_size = size;
        this->_birthLimit = birthLimit;
        this->_deathLimit = deathLimit;
        this->_steps = steps;

        floorsTileset.loadFromFile("./res/sprites/floors.png");
        wallsTileset.loadFromFile("./res/sprites/Tileset.png");

        this->_map = new bool *[this->_size.x];
        for (int i = 0; i < this->_size.x; ++i)
            this->_map[i] = new bool[this->_size.y];

        this->_mapTmp = new bool *[this->_size.x];
        for (int i = 0; i < this->_size.x; ++i)
            _mapTmp[i] = new bool[this->_size.y];

        renderTexture.create(windowSize.x, windowSize.y);

        return generate();
    }

    bool **generate()
    {

        for (int x = 0; x < this->_size.x; x++)
            for (int y = 0; y < this->_size.y; y++) {
                this->_map[x][y] = false;
                this->_mapTmp[x][y] = false;
            }

        for (int x = 0; x < this->_size.x; x++)
            for (int y = 0; y < this->_size.y; y++)
                if ((rand() % 100) < this->_chanceToStartAlive)
                    this->_map[x][y] = true;

        int i = 0;
        while ((i++) < this->_steps) {
            for (int x = 0; x < this->_size.x; x++) {
                for (int y = 0; y < this->_size.y; y++) {
                    // Count neighbours
                    int count = countNeighbours(x, y);
                    //The new value is based on our simulation rules
                    //First, if a cell is alive but has too few neighbours, kill it.
                    if (this->_map[x][y]) {
                        if (count < this->_deathLimit)
                            _mapTmp[x][y] = false;
                        else
                            _mapTmp[x][y] = true;
                    } //Otherwise, if the cell is dead now, check if it has the right number of neighbours to be 'born'
                    else {
                        if (count > this->_birthLimit)
                            _mapTmp[x][y] = true;
                        else
                            _mapTmp[x][y] = false;
                    }
                }
            }
            this->_map = _mapTmp;
        }
        return this->_map;
    }

    void update()
    {
    }

    void render()
    {
        if (mapTiles.size() > 0) {
            mapTiles.clear();
            wallTiles.clear();
        }

        auto m = this->_map;
        for (size_t x = 0; x < this->_size.x; x++) {
            for (size_t y = 0; y < this->_size.y; y++) {

                bool topBound = y > 0;
                bool leftBound = x > 0;
                bool bottomBound = y <= this->_size.y;
                bool rightBound = x + 1 < this->_size.x;

                bool ground = !m[x][y];
                bool outer = m[x][y];

                bool top = (topBound && ground && m[x][y - 1]);
                bool left = (leftBound && ground && m[x - 1][y]);
                bool right = (rightBound && ground && m[x + 1][y]);
                bool bottom = (bottomBound && ground && m[x][y + 1]);

                bool topLeftCorner = (top && left);
                bool topRightCorner = (top && right);
                bool bottomLeftCorner = (bottom && left);
                bool bottomRightCorner = (bottom && right);

                bool topLeftRightCorners = (top && left && right);
                bool leftTopBottomCorners = (left && top && bottom);
                bool rightTopBottomCorners = (right && top && bottom);
                bool bottomLeftRightCorners = (bottom && left && right);
                bool allCorners = (left && top && bottom && right);

                bool topWall = (bottomBound && outer && !m[x][y + 1]);
                bool leftWall = (rightBound && outer && !m[x + 1][y]);
                bool rightWall = (leftBound && outer && !m[x - 1][y]);
                bool bottomWall = (topBound && outer && !m[x][y - 1]);

                bool topLeftWall = (topWall && leftWall);
                bool topRightWall = (topWall && rightWall);
                bool bottomLeftWall = (bottomWall && leftWall);
                bool bottomRightWall = (bottomWall && rightWall);

                bool topLeftRightWalls = (topWall && leftWall && rightWall);
                bool leftTopBottomWalls = (leftWall && topWall && bottomWall);
                bool rightTopBottomWalls = (rightWall && topWall && bottomWall);
                bool bottomLeftRightWalls = (bottomWall && leftWall && rightWall);
                bool allWalls = (leftWall && topWall && bottomWall && rightWall);

                sf::Vector2f pos = {(x * 32.f), (y * 32.f)};

                if (ground && !top && !left && !right && !bottom)
                    PushTile(pos, Tile::Type::GROUND);

                if (top && !left && !right)
                    PushTile(pos, Tile::Type::TOP);
                if (left && !topLeftCorner && !bottomLeftCorner)
                    PushTile(pos, Tile::Type::LEFT);
                if (bottom && !bottomLeftCorner && !bottomRightCorner)
                    PushTile(pos, Tile::Type::BOTTOM);
                if (right && !topRightCorner && !bottomRightCorner)
                    PushTile(pos, Tile::Type::RIGHT);

                if (topLeftCorner && !bottomLeftCorner)
                    PushTile(pos, Tile::Type::TOP_LEFT);
                if (topRightCorner && !bottomRightCorner)
                    PushTile(pos, Tile::Type::TOP_RIGHT);
                if (bottomLeftCorner && !bottomRightCorner)
                    PushTile(pos, Tile::Type::BOTTOM_LEFT);
                if (bottomRightCorner && !bottomLeftCorner)
                    PushTile(pos, Tile::Type::BOTTOM_RIGHT);

                if (topLeftRightCorners)
                    PushTile(pos, Tile::Type::TOP_LEFT_RIGHT);
                if (leftTopBottomCorners)
                    PushTile(pos, Tile::Type::LEFT_TOP_BOTTOM);
                if (bottomLeftRightCorners)
                    PushTile(pos, Tile::Type::BOTTOM_LEFT_RIGHT);
                if (rightTopBottomCorners)
                    PushTile(pos, Tile::Type::RIGHT_TOP_BOTTOM);

                //! multiple tiles pushed at the same position (ex: leftWall + topLeftWall ...)
                if (topWall && !leftWall && !rightWall)
                    PushTile(pos, Tile::Type::WALL_TOP);
                if (leftWall && !topLeftWall && !bottomLeftWall && !topWall)
                    PushTile(pos, Tile::Type::WALL_LEFT);
                if (bottomWall && !bottomLeftWall && !bottomRightWall)
                    PushTile(pos, Tile::Type::WALL_BOTTOM);
                if (rightWall && !topRightWall && !bottomRightWall)
                    PushTile(pos, Tile::Type::WALL_RIGHT);

                if (topLeftWall && !topRightWall)
                    PushTile(pos, Tile::Type::WALL_TOP_LEFT);
                if (topRightWall && !topLeftWall)
                    PushTile(pos, Tile::Type::WALL_TOP_RIGHT);
                if (bottomLeftWall && !bottomRightWall)
                    PushTile(pos, Tile::Type::WALL_BOTTOM_LEFT);
                if (bottomRightWall && !bottomLeftWall && !rightTopBottomWalls)
                    PushTile(pos, Tile::Type::WALL_BOTTOM_RIGHT);

                if (topLeftRightWalls)
                    PushTile(pos, Tile::Type::WALL_TOP_LEFT_RIGHT);
                if (leftTopBottomWalls && !bottomLeftWall && !topLeftWall)
                    PushTile(pos, Tile::Type::WALL_LEFT_TOP_BOTTOM);
                if (bottomLeftRightWalls)
                    PushTile(pos, Tile::Type::WALL_BOTTOM_LEFT_RIGHT);
                if (rightTopBottomWalls && !topRightWall)
                    PushTile(pos, Tile::Type::WALL_RIGHT_TOP_BOTTOM);

                // Walls
                // if (ground && bottom)
                //     PushTile({(x * 32.f), ((y + 1) * 32.f)}, Tile::Type::OUTER);

                if (ground && top) {
                    PushTile({((x)*32.f), ((y - 1) * 32.f)}, Tile::Type::WALL);
                }
                if (ground && left) {
                    PushTile({((x - 1) * 32.f), (y * 32.f)}, Tile::Type::WALL);
                }
                if (ground && bottom) {
                    PushTile({((x)*32.f), ((y + 1) * 32.f)}, Tile::Type::WALL);
                }
                if (ground && right) {
                    PushTile({((x + 1) * 32.f), (y * 32.f)}, Tile::Type::WALL);
                }
            }
        }

        this->redraw();
    }

    void redraw()
    {
        renderTexture.clear(sf::Color(0, 0, 0));

        for (size_t i = 0; i < mapTiles.size(); i++) {
            renderTexture.draw(mapTiles[i].sprite);
        }
        // for (size_t i = 0; i < this->wallTiles.size(); i++) {
        //     sf::RectangleShape r = sf::RectangleShape({10,10});
        //     r.setPosition(this->wallTiles[i].getPosition());
        //     renderTexture.draw(r);
        // }
        this->setTexture(renderTexture.getTexture());
        renderTexture.display();
    }
    void printMap()
    {
        for (int i = 0; i < this->_size.x; i++) {
            for (int j = 0; j < this->_size.y; j++) {
                std::cout << this->_map[i][j];
            }
            std::cout << std::endl;
        }
    }

  private:
    void PushTile(sf::Vector2f position, Tile::Type type)
    {
        static int w = 32;
        static int h = 32;

        Tile tile;
        tile.type = type;
        if (type < Tile::Type::OUTER)
            tile.sprite.setTexture(this->floorsTileset);
        else
            tile.sprite.setTexture(this->wallsTileset);

        tile.sprite.setPosition(position);

        bool isWall = false;
        switch (type) {
        // case Tile::Type::OUTER:
        //     tile.sprite.setTextureRect({w * 5, h * 6, w, h});
        //     break;
        case Tile::Type::GROUND:
            tile.sprite.setTextureRect({w * 1, h * 0, w, h});
            break;

        case Tile::Type::TOP:
            tile.sprite.setTextureRect({w * 2, h * 0, w, h});
            break;
        case Tile::Type::LEFT:
            tile.sprite.setTextureRect({w * 3, h * 0, w, h});
            break;
        case Tile::Type::BOTTOM:
            tile.sprite.setTextureRect({w * 4, h * 1, w, h});
            break;
        case Tile::Type::RIGHT:
            tile.sprite.setTextureRect({w * 0, h * 1, w, h});
            break;

        case Tile::Type::TOP_LEFT:
            tile.sprite.setTextureRect({w * 4, h * 0, w, h});
            break;
        case Tile::Type::TOP_RIGHT:
            tile.sprite.setTextureRect({w * 1, h * 1, w, h});
            break;
        case Tile::Type::BOTTOM_LEFT:
            tile.sprite.setTextureRect({w * 1, h * 2, w, h});
            break;
        case Tile::Type::BOTTOM_RIGHT:
            tile.sprite.setTextureRect({w * 3, h * 2, w, h});
            break;

        case Tile::Type::TOP_LEFT_RIGHT:
            tile.sprite.setTextureRect({w * 3, h * 1, w, h});
            break;
        case Tile::Type::LEFT_TOP_BOTTOM:
            tile.sprite.setTextureRect({w * 2, h * 2, w, h});
            break;
        case Tile::Type::BOTTOM_LEFT_RIGHT:
            tile.sprite.setTextureRect({w * 0, h * 3, w, h});
            break;
        case Tile::Type::RIGHT_TOP_BOTTOM:
            tile.sprite.setTextureRect({w * 4, h * 2, w, h});
            break;

        case Tile::Type::WALL_TOP:
            tile.sprite.setTextureRect({w * 0, h * 8, w, h});
            break;
        case Tile::Type::WALL_LEFT:
            tile.sprite.setTextureRect({w * 0, h * 1, w, h});
            break;
        case Tile::Type::WALL_BOTTOM:
            tile.sprite.setTextureRect({w * 2, h * 5, w, h});
            break;
        case Tile::Type::WALL_RIGHT:
            tile.sprite.setTextureRect({w * 4, h * 4, w, h});
            break;
        case Tile::Type::WALL_TOP_LEFT:
            tile.sprite.setTextureRect({w * 0, h * 3, w, h});
            break;
        case Tile::Type::WALL_TOP_RIGHT:
            tile.sprite.setTextureRect({w * 0, h * 4, w, h});
            break;
        case Tile::Type::WALL_BOTTOM_LEFT:
            tile.sprite.setTextureRect({w * 0, h * 6, w, h});
            break;
        case Tile::Type::WALL_BOTTOM_RIGHT:
            tile.sprite.setTextureRect({w * 0, h * 7, w, h});
            break;

        case Tile::Type::WALL_TOP_LEFT_RIGHT:
            tile.sprite.setTextureRect({w * 1, h * 3, w, h});
            break;
        case Tile::Type::WALL_LEFT_TOP_BOTTOM:
            tile.sprite.setTextureRect({w * 1, h * 4, w, h});
            break;
        case Tile::Type::WALL_BOTTOM_LEFT_RIGHT:
            tile.sprite.setTextureRect({w * 1, h * 6, w, h});
            break;
        case Tile::Type::WALL_RIGHT_TOP_BOTTOM:
            tile.sprite.setTextureRect({w * 1, h * 7, w, h});
            break;

        case Tile::Type::WALL:
            tile.sprite.setTextureRect({w * 5, h * 6, w, h});
            // tile.sprite.setColor(sf::Color::Transparent);
            this->wallTiles.push_back(tile.sprite.getGlobalBounds());
            isWall = true;
            break;

        default:
            break;
        }

        if (!isWall)
            mapTiles.push_back(tile);
        isWall = false;
    }

    int countNeighbours(int x, int y)
    {
        int count = 0;
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                int neighbour_x = x + i;
                int neighbour_y = y + j;

                //If we're looking at the middle point
                if (i == 0 && j == 0) {
                } // Do nothing
                //In case the index we're looking at it off the edge of the map
                else if (neighbour_x < 0 ||
                         neighbour_y < 0 ||
                         neighbour_x >= _size.x ||
                         neighbour_y >= _size.y) {
                    ++count;
                }
                //Otherwise, a normal check of the neighbour
                else if (_map[neighbour_x][neighbour_y])
                    ++count;
            }
        }
        return count;
    }
};
