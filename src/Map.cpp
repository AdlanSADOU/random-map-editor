#include "Map.hpp"

bool **Map::create(sf::Vector2u windowSize, sf::Vector2i size, int birthLimit,
                   int deathLimit, int steps, int chanceToStartAlive)
{
    this->_chanceToStartAlive = chanceToStartAlive;
    this->_size = size;
    this->_birthLimit = birthLimit;
    this->_deathLimit = deathLimit;
    this->_steps = steps;

    // TODO: hardcoded spritesheet paths
    floorsTileset.loadFromFile("./res/sprites/floors.png");
    wallsTileset.loadFromFile("./res/sprites/walls.png");

    this->_map = new bool *[this->_size.x];
    for (int i = 0; i < this->_size.x; ++i)
        this->_map[i] = new bool[this->_size.y];

    this->_mapTmp = new bool *[this->_size.x];
    for (int i = 0; i < this->_size.x; ++i)
        _mapTmp[i] = new bool[this->_size.y];

    renderTexture.create(windowSize.x * 2, windowSize.y * 2);
    return generate();
}

// TODO: add an additional layer?
bool **Map::generate()
{

    for (int x = 0; x < this->_size.x; x++)
        for (int y = 0; y < this->_size.y; y++) {
            if (x > 2 && y > 2 && this->_size.x > x + 3 && this->_size.y > y + 2) {
                this->_map[x][y] = false;
                this->_mapTmp[x][y] = false;
            }
        }

    for (int x = 0; x < this->_size.x; x++)
        for (int y = 0; y < this->_size.y; y++)
            if (x > 2 && y > 2 && this->_size.x > x + 3 && this->_size.y > y + 2) {
                if ((rand() % 100) < this->_chanceToStartAlive)
                    this->_map[x][y] = true;
            }
    int i = 0;
    while ((i++) < this->_steps) {
        for (int x = 0; x < this->_size.x; x++) {
            for (int y = 0; y < this->_size.y; y++) {
                // Count neighbours
                int count = countNeighbours(x, y);
                // The new value is based on our simulation rules
                // First, if a cell is alive but has too few neighbours, kill it.
                if (this->_map[x][y] && x > 0) {
                    if (count < this->_deathLimit)
                        _mapTmp[x][y] = false;
                    else
                        _mapTmp[x][y] = true;
                } // Otherwise, if the cell is dead now, check if it has the
                  // right number of neighbours to be 'born'
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
    this->printMap();
    puts("\n");
    return this->_map;
}

void Map::update()
{
}

void Map::render()
{
    if (mapTiles.size() > 0) {
        mapTiles.clear();
        wallTiles.clear();
    }

    typedef Tile::Type tType;

#define WALL(x, y) (m[x][y])
#define GROUND(x, y) (!m[x][y])

#define GROUND_TOP(x, y) (GROUND(x, y) && !GROUND(x, y - 1))

#define WALL_TOP(x, y) (WALL(x, y) && m[x][y - 1])
#define WALL_TOP(x, y) (WALL(x, y) && m[x][y - 1])

    auto m = this->_map;
    for (size_t x = 0; x < this->_size.x; x++) {
        if (x >= this->_size.x - 1 || x < 1)
            continue;

        for (size_t y = 0; y < this->_size.y; y++) {
            if (y < 1 || y >= this->_size.y)
                continue;

            bool ground = !m[x][y];
            bool outer = m[x][y];

            bool top = (ground && m[x][y - 1]);
            bool left = (ground && m[x - 1][y]);
            bool right = (ground && m[x + 1][y]);
            bool bottom = (ground && m[x][y + 1]);

            bool corner_top_left = ((top && left) && (!bottom && !right));
            bool corner_top_right = ((top && right) && (!bottom && !left));
            bool corner_bottom_left = ((bottom && left) && (!top && !right));
            bool corner_bottom_right = ((bottom && right) && (!top && !left));

            bool corners_top_left_right = (top && left && right && !bottom);
            bool corners_left_top_bottom = (left && top && bottom && !right);
            bool corners_right_top_bottom = (right && top && bottom && !left);
            bool corners_bottom_left_right = (bottom && left && right && !top);
            bool allCorners = (left && top && bottom && right);

            bool wall_top = (outer && !m[x][y - 1]);
            bool wall_left = (outer && !m[x - 1][y]);
            bool wall_right = (outer && !m[x + 1][y]);
            bool wall_bottom = (outer && !m[x][y + 1]);

            bool wall_top_left = (wall_top && wall_left && !wall_right && !wall_bottom);
            bool wall_top_right = (wall_top && wall_right && !wall_left && !wall_bottom);
            bool wall_bottom_left = (wall_bottom && wall_left && !wall_top && !wall_right);
            bool wall_bottom_right = (wall_bottom && wall_right && !wall_left && !wall_top);

            bool wall_top_bottom = (outer && (WALL(x - 1, y) && WALL(x + 1, y)) && (!WALL(x, y + 1) && !WALL(x, y - 1)));
            bool wall_left_right = (left && right);

            bool walls_top_left_right = (wall_top && wall_left && wall_right && !wall_bottom);
            bool walls_left_top_bottom = (wall_left && wall_top && wall_bottom && !wall_right);
            bool walls_right_top_bottom = (wall_right && wall_top && wall_bottom && !wall_left);
            bool walls_bottom_left_right = (wall_bottom && wall_left && wall_right && !wall_top);
            bool wall_loop = (wall_left && wall_top && wall_bottom && wall_right);

            // TODO: we should rethink these corner cases, we have poor naming convention right now
            // bool walls_top_left_corner_tl = (!wall_top && !wall_left && wall_bottom && wall_right) && (GROUND(x - 1, y - 1) && !GROUND(x - 1, y + 1));
            // bool walls_top_left_bottom_corner_tl_bl = (!wall_top && !wall_left && !wall_bottom && wall_right) && (GROUND(x - 1, y - 1) && GROUND(x - 1, y + 1) && !GROUND(x + 1, y - 1));
            // bool walls_top_left_bottom_right_corner_tl_bl_tr = (!wall_top && !wall_left && !wall_bottom && !wall_right) && (GROUND(x - 1, y - 1) && GROUND(x - 1, y + 1) && GROUND(x + 1, y - 1));
            // bool walls_top_left_bottom_right_corner_tl_bl_tr_br = (!wall_top && !wall_left && !wall_bottom && !wall_right) && (GROUND(x - 1, y - 1) && GROUND(x - 1, y + 1) && GROUND(x + 1, y - 1));
            // // bool walls_left_corner_tr = (!wall_left && (GROUND(x, y-1)  && GROUND(x, y+1) && !GROUND(x)))
            // bool walls_top_left_right_corner_tl_tr = (!wall_top && !wall_left && !wall_right && (GROUND(x-1, y-1) && GROUND(x+1, y-1)));

            sf::Vector2f pos = {(x * 32.f), (y * 32.f)};

            if (ground && !top && !left && !right && !bottom) //ok
                PushTile(pos, Tile::Type::GROUND);
            else if (top && !left && !right && !bottom) //ok
                PushTile(pos, Tile::Type::TOP);
            else if (left && !right && !top && !bottom) //ok
                PushTile(pos, Tile::Type::LEFT);
            else if (bottom && !top && !left && !right) //ok
                PushTile(pos, Tile::Type::BOTTOM);
            else if (right && !left && !top && !bottom) //ok
                PushTile(pos, Tile::Type::RIGHT);

            else if (corner_top_left) //ok
                PushTile(pos, Tile::Type::TOP_LEFT);
            else if (corner_top_right) //ok
                PushTile(pos, Tile::Type::TOP_RIGHT);
            else if (corner_bottom_left) //ok
                PushTile(pos, Tile::Type::BOTTOM_LEFT);
            else if (corner_bottom_right) //ok
                PushTile(pos, Tile::Type::BOTTOM_RIGHT);

            else if (left && right && (!top && !bottom)) //ok
                PushTile(pos, Tile::Type::LEFT_RIGHT);
            else if (top && bottom && (!left && !right)) //ok
                PushTile(pos, Tile::Type::TOP_BOTTOM);

            else if (corners_top_left_right) //ok
                PushTile(pos, Tile::Type::TOP_LEFT_RIGHT);
            else if (corners_left_top_bottom) //ok
                PushTile(pos, Tile::Type::LEFT_TOP_BOTTOM);
            else if (corners_bottom_left_right) //ok
                PushTile(pos, Tile::Type::BOTTOM_LEFT_RIGHT);
            else if (corners_right_top_bottom) //ok
                PushTile(pos, Tile::Type::RIGHT_TOP_BOTTOM);

            // WALLS
            else if (wall_top && !wall_left && !wall_right && !wall_bottom) //ok
                PushTile(pos, Tile::Type::WALL_TOP);
            else if (wall_left && !wall_top && !wall_bottom && !wall_right) //ok
                PushTile(pos, Tile::Type::WALL_LEFT);
            else if (wall_bottom && !wall_left && !wall_right && !wall_top) //ok
                PushTile(pos, Tile::Type::WALL_BOTTOM);
            else if (wall_right && !wall_top && !wall_bottom && !wall_left) //ok
                PushTile(pos, Tile::Type::WALL_RIGHT);

            else if (wall_top_bottom) //ok
                PushTile(pos, Tile::Type::WALL_TOP_BOTTOM);
            else if (wall_top_left) //ok
                PushTile(pos, Tile::Type::WALL_TOP_LEFT);

            else if (wall_top_right) //ok
                PushTile(pos, Tile::Type::WALL_TOP_RIGHT);
            else if (wall_bottom_left) //ok
                PushTile(pos, Tile::Type::WALL_BOTTOM_LEFT);
            else if (wall_bottom_right) //ok
                PushTile(pos, Tile::Type::WALL_BOTTOM_RIGHT);

            else if (walls_top_left_right)
                PushTile(pos, Tile::Type::WALL_TOP_LEFT_RIGHT);
            else if (walls_left_top_bottom)
                PushTile(pos, Tile::Type::WALL_LEFT_TOP_BOTTOM);
            else if (walls_bottom_left_right)
                PushTile(pos, Tile::Type::WALL_BOTTOM_LEFT_RIGHT);
            else if (walls_right_top_bottom)
                PushTile(pos, Tile::Type::WALL_RIGHT_TOP_BOTTOM);

            else if (walls_top_left_corner_tl)
                PushTile(pos, Tile::Type::WALLS_TOP_LEFT_CORNER_TL);
            else if (walls_top_left_bottom_corner_tl_bl)
                PushTile(pos, Tile::Type::WALLS_TOP_LEFT_BOTTOM_CORNER_TL_BL);
            else if (walls_top_left_bottom_right_corner_tl_bl_tr)
                PushTile(pos, Tile::Type::WALLS_TOP_LEFT_BOTTOM_RIGHT_CORNER_TL_BL_TR);
            else if (walls_top_left_bottom_right_corner_tl_bl_tr_br)
                PushTile(pos, Tile::Type::WALLS_TOP_LEFT_BOTTOM_RIGHT_CORNER_TL_BL_TR_BR);
            else if (walls_top_left_right_corner_tl_tr)
                PushTile(pos, Tile::Type::WALLS_TOP_LEFT_RIGHT_CORNER_TL_TR);

            // else if (WALL(x + 1, y) && WALL(x, y + 1) && !WALL(x + 1, y + 1) && !WALL(x - 1, y))
            //     PushTile(pos, Tile::Type::WALL_BOTTOM_RIGHT_CORNER);
            // else if (WALL(x - 1, y) && WALL(x, y + 1) && !WALL(x - 1, y + 1) && !WALL(x, y - 1))
            //     PushTile(pos, Tile::Type::WALL_BOTTOM_LEFT_CORNER);

            // else if (wall_loop)
            //     PushTile(pos, Tile::Type::WALL_LOOP);
            // END WALLs

            // else if (topWall && !leftWall && !rightWall)
            //     PushTile(pos, Tile::Type::WALL_TOP);
            // else if (leftWall && !wall_top_left && !bottomLeftWall && !topWall)
            //     PushTile(pos, Tile::Type::WALL_LEFT);
            // else if (bottomWall && !bottomLeftWall && !bottomRightWall)
            //     PushTile(pos, Tile::Type::WALL_BOTTOM);
            // else if (rightWall && !topRightWall && !bottomRightWall)
            //     PushTile(pos, Tile::Type::WALL_RIGHT);

            // else if (wall_top_bottom && !leftWall && !rightWall)
            //     PushTile(pos, Tile::Type::WALL_TOP_BOTTOM);
            // else if (wall_left_right && !topWall && !leftWall)
            //     PushTile(pos, Tile::Type::WALL_LEFT_RIGHT);

            // else if (wall_top_left && !topRightWall)
            //     PushTile(pos, Tile::Type::WALL_TOP_LEFT);
            // else if (topRightWall && !wall_top_left)
            //     PushTile(pos, Tile::Type::WALL_TOP_RIGHT);
            // else if (bottomLeftWall && !bottomRightWall)
            //     PushTile(pos, Tile::Type::WALL_BOTTOM_LEFT);
            // else if (bottomRightWall && !bottomLeftWall && !walls_right_top_bottom)
            //     PushTile(pos, Tile::Type::WALL_BOTTOM_RIGHT);

            // if (ground && bottom)
            //     PushTile({(x * 32.f), ((y + 1) * 32.f)}, Tile::Type::OUTER);

            // if (ground && top) {
            //     PushTile({((x)*32.f), ((y - 1) * 32.f)}, Tile::Type::WALL);
            // }
            // if (ground && left) {
            //     PushTile({((x - 1) * 32.f), (y * 32.f)}, Tile::Type::WALL);
            // }
            // if (ground && bottom) {
            //     PushTile({((x)*32.f), ((y + 1) * 32.f)}, Tile::Type::WALL);
            // }
            // if (ground && right) {
            //     PushTile({((x + 1) * 32.f), (y * 32.f)}, Tile::Type::WALL);
            // }

            // if (leftWall) {
            //     PushTile({((x - 1) * 32.f), (y * 32.f)}, Tile::Type::WALL);
            // }
            // if (bottomWall) {
            //     PushTile({((x)*32.f), ((y + 1) * 32.f)}, Tile::Type::WALL);
            // }
            // if (rightWall) {
            //     PushTile({((x + 1) * 32.f), (y * 32.f)}, Tile::Type::WALL);
            // }

            // TODO: I don't know if we need to push these, we'll see eventually
            // else if (outer)
            //     PushTile(pos, Tile::Type::WALL);
        }

        this->redraw();
    }
}

void Map::redraw()
{
    renderTexture.clear(sf::Color(32, 32, 32));

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

void Map::printMap()
{
    for (int i = 0; i < this->_size.x; i++) {
        for (int j = 0; j < this->_size.y; j++) {
            std::cout << this->_map[i][j];
        }
        std::cout << std::endl;
    }
}

void Map::PushTile(sf::Vector2f position, Tile::Type type)
{
    /**
    * TODO: the map tile size should be handled somehow
    * or make the world only produce 32x32 sprite sheets
    */

    // TODO: hardcoded tile size
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
    case Tile::Type::OUTER:
        tile.sprite.setTextureRect({w * 0, h * 0, w, h});
        break;
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

    case Tile::Type::LEFT_RIGHT:
        tile.sprite.setTextureRect({w * 2, h * 1, w, h});
        break;
    case Tile::Type::TOP_BOTTOM:
        tile.sprite.setTextureRect({w * 0, h * 2, w, h});
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
        tile.sprite.setTextureRect({w * 2, h * 5, w, h});
        break;
    case Tile::Type::WALL_LEFT:
        tile.sprite.setTextureRect({w * 4, h * 4, w, h});
        break;
    case Tile::Type::WALL_BOTTOM:
        tile.sprite.setTextureRect({w * 4, h * 1, w, h});
        break;
    case Tile::Type::WALL_RIGHT:
        tile.sprite.setTextureRect({w * 4, h * 3, w, h});
        break;
    case Tile::Type::WALL_TOP_LEFT:
        tile.sprite.setTextureRect({w * 2, h * 4, w, h});
        break;
    case Tile::Type::WALL_TOP_RIGHT:
        tile.sprite.setTextureRect({w * 2, h * 3, w, h});
        break;
    case Tile::Type::WALL_BOTTOM_LEFT:
        tile.sprite.setTextureRect({w * 7, h * 0, w, h});
        break;
    case Tile::Type::WALL_BOTTOM_RIGHT:
        tile.sprite.setTextureRect({w * 4, h * 0, w, h});
        break;

    case Tile::Type::WALL_TOP_LEFT_RIGHT:
        tile.sprite.setTextureRect({w * 5, h * 1, w, h});
        break;
    case Tile::Type::WALL_LEFT_TOP_BOTTOM:
        tile.sprite.setTextureRect({w * 5, h * 0, w, h});
        break;
    case Tile::Type::WALL_BOTTOM_LEFT_RIGHT:
        tile.sprite.setTextureRect({w * 1, h * 0, w, h});
        break;
    case Tile::Type::WALL_RIGHT_TOP_BOTTOM:
        tile.sprite.setTextureRect({w * 2, h * 0, w, h});
        break;

    case Tile::Type::WALLS_TOP_LEFT_CORNER_TL:
        tile.sprite.setTextureRect({w * 5, h * 5, w, h});
        break;
    case Tile::Type::WALLS_TOP_LEFT_BOTTOM_CORNER_TL_BL:
        tile.sprite.setTextureRect({w * 0, h * 5, w, h});
        break;
    case Tile::Type::WALLS_TOP_LEFT_BOTTOM_RIGHT_CORNER_TL_BL_TR:
        tile.sprite.setTextureRect({w * 6, h * 4, w, h});
        break;
    case Tile::Type::WALLS_TOP_LEFT_BOTTOM_RIGHT_CORNER_TL_BL_TR_BR:
        tile.sprite.setTextureRect({w * 6, h * 2, w, h});
        break;
    case Tile::Type::WALLS_TOP_LEFT_RIGHT_CORNER_TL_TR:
        tile.sprite.setTextureRect({w * 3, h * 5, w, h});
        break;

    case Tile::Type::WALL_TOP_BOTTOM:
        tile.sprite.setTextureRect({w * 0, h * 1, w, h});
        break;
    case Tile::Type::WALL_LEFT_RIGHT:
        tile.sprite.setTextureRect({w * 6, h * 1, w, h});
        break;

    case Tile::Type::WALL:
        tile.sprite.setTextureRect({w * 0, h * 0, w, h});
        tile.sprite.setColor(sf::Color(40, 0, 0));
        this->wallTiles.push_back(tile.sprite.getGlobalBounds());
        isWall = true;
        break;

    default:
        tile.sprite.setTextureRect({w * 0, h * 0, w, h});
        break;
    }

    // if (!isWall)
    mapTiles.push_back(tile);
    isWall = false;
}

int Map::countNeighbours(int x, int y)
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