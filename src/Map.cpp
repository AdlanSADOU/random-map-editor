/**
 * seems like we are done adding all tile types
 * code should be cleaned up
 * ask community if there is a standard way to name those tiles
 */

#include "Map.hpp"

bool **Map::create(sf::Vector2u windowSize, sf::Vector2i size, int birthLimit,
                   int deathLimit, int steps, int chanceToStartAlive)
{
    this->_chanceToStartAlive = chanceToStartAlive;
    this->_size               = size;
    this->_birthLimit         = birthLimit;
    this->_deathLimit         = deathLimit;
    this->_steps              = steps;

    // TODO: hardcoded spritesheet paths
    floorsTileset.loadFromFile("./res/sprites/default.png");
    wallsTileset.loadFromFile("./res/sprites/default_walls.png");

    this->_map = new bool *[this->_size.x];
    for (int i = 0; i < this->_size.x; ++i)
        this->_map[i] = new bool[this->_size.y];

    this->_mapTmp = new bool *[this->_size.x];
    for (int i = 0; i < this->_size.x; ++i)
        _mapTmp[i] = new bool[this->_size.y];

    renderTexture.create(windowSize.x * 4, windowSize.y * 4);
    return generate();
}

// TODO: add an additional layer?
bool **Map::generate()
{
    for (int x = 0; x < this->_size.x; x++)
        for (int y = 0; y < this->_size.y; y++) {
            if (x > 2 && y > 2 && this->_size.x > x + 3 && this->_size.y > y + 2) {
                this->_map[x][y]    = false;
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
    // this->printMap();
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

#define W(x, y) (m[x][y])  // looking for a wall
#define G(x, y) (!m[x][y]) // looking for ground

#define G_TOP(x, y)   (G(x, y - 1))
#define G_LEFT(x, y)  (G(x - 1, y))
#define G_BOT(x, y)   (G(x, y + 1))
#define G_RIGHT(x, y) (G(x + 1, y))

#define W_TOP(x, y)   (W(x, y - 1))
#define W_LEFT(x, y)  (W(x - 1, y))
#define W_BOT(x, y)   (W(x, y + 1))
#define W_RIGHT(x, y) (W(x + 1, y))

#define G_TL_CORNER(x, y) (G(x - 1, y - 1))
#define G_BL_CORNER(x, y) (G(x - 1, y + 1))
#define G_BR_CORNER(x, y) (G(x + 1, y + 1))
#define G_TR_CORNER(x, y) (G(x + 1, y - 1))

    auto m = this->_map;
    for (size_t x = 0; x < this->_size.x; x++) {
        if (x >= this->_size.x - 1 || x < 1)
            continue;

        for (size_t y = 0; y < this->_size.y; y++) {
            if (y < 1 || y >= this->_size.y)
                continue;

            bool ground = !m[x][y];
            bool outer  = m[x][y];
            bool top    = (ground && m[x][y - 1]); // cell is standing on a ground tile and has a wall above it
            bool left   = (ground && m[x - 1][y]);
            bool right  = (ground && m[x + 1][y]);
            bool bottom = (ground && m[x][y + 1]);

            bool corner_top_left     = ((top && left) && (!bottom && !right));
            bool corner_top_right    = ((top && right) && (!bottom && !left));
            bool corner_bottom_left  = ((bottom && left) && (!top && !right));
            bool corner_bottom_right = ((bottom && right) && (!top && !left));

            bool corners_top_left_right    = (top && left && right && !bottom);
            bool corners_left_top_bottom   = (left && top && bottom && !right);
            bool corners_right_top_bottom  = (right && top && bottom && !left);
            bool corners_bottom_left_right = (bottom && left && right && !top);
            bool allCorners                = (left && top && bottom && right);

            bool wall_top    = (outer && !m[x][y - 1]);
            bool wall_left   = (outer && !m[x - 1][y]);
            bool wall_right  = (outer && !m[x + 1][y]);
            bool wall_bottom = (outer && !m[x][y + 1]);

            bool wall_top_left     = (wall_top && wall_left && !wall_right && !wall_bottom);
            bool wall_top_right    = (wall_top && wall_right && !wall_left && !wall_bottom);
            bool wall_bottom_left  = (wall_bottom && wall_left && !wall_top && !wall_right);
            bool wall_bottom_right = (wall_bottom && wall_right && !wall_left && !wall_top);

            bool wall_top_bottom = (outer && (W(x - 1, y) && W(x + 1, y)) && (!W(x, y + 1) && !W(x, y - 1)));
            bool wall_left_right = (left && right);

            bool walls_top_left_right    = (wall_top && wall_left && wall_right && !wall_bottom);
            bool walls_left_top_bottom   = (wall_left && wall_top && wall_bottom && !wall_right);
            bool walls_right_top_bottom  = (wall_right && wall_top && wall_bottom && !wall_left);
            bool walls_bottom_left_right = (wall_bottom && wall_left && wall_right && !wall_top);
            bool wall_loop               = (wall_left && wall_top && wall_bottom && wall_right);

            bool edge_wt_gl = outer &&
                              G_TL_CORNER(x, y) && G_TR_CORNER(x, y) && !G_BR_CORNER(x, y) && G_BL_CORNER(x, y) &&
                              W_TOP(x, y) && W_LEFT(x, y) && W_BOT(x, y) && W_RIGHT(x, y);
            bool edge_wt_gl1 = outer &&
                               G_TL_CORNER(x, y) && !G_TR_CORNER(x, y) && !G_BR_CORNER(x, y) && G_BL_CORNER(x, y) &&
                               W_TOP(x, y) && W_LEFT(x, y) && W_BOT(x, y) && W_RIGHT(x, y);
            bool edge_wt_gl2 = outer &&
                               G_TL_CORNER(x, y) && G_TR_CORNER(x, y) && !G_BR_CORNER(x, y) && !G_BL_CORNER(x, y) &&
                               W_TOP(x, y) && W_LEFT(x, y) && W_BOT(x, y) && W_RIGHT(x, y);
            bool edge_wt_gl3 = outer &&
                               !G_TL_CORNER(x, y) && !G_TR_CORNER(x, y) && G_BR_CORNER(x, y) && G_BL_CORNER(x, y) &&
                               W_TOP(x, y) && W_LEFT(x, y) && W_BOT(x, y) && W_RIGHT(x, y);
            bool edge_wt_gl4 = outer &&
                               !G_TL_CORNER(x, y) && !G_TR_CORNER(x, y) && G_BR_CORNER(x, y) && !G_BL_CORNER(x, y) &&
                               W_TOP(x, y) && W_LEFT(x, y) && W_BOT(x, y) && W_RIGHT(x, y);
            bool edge_wt_gl5 = outer &&
                               !G_TL_CORNER(x, y) && !G_TR_CORNER(x, y) && !G_BR_CORNER(x, y) && G_BL_CORNER(x, y) &&
                               W_TOP(x, y) && W_LEFT(x, y) && W_BOT(x, y) && W_RIGHT(x, y);
            bool edge_wt_gl6 = outer &&
                               G_TL_CORNER(x, y) && !G_TR_CORNER(x, y) && !G_BR_CORNER(x, y) && !G_BL_CORNER(x, y) &&
                               W_TOP(x, y) && W_LEFT(x, y) && W_BOT(x, y) && W_RIGHT(x, y);
            bool edge_wt_gl7 = outer &&
                               !G_TL_CORNER(x, y) && G_TR_CORNER(x, y) && !G_BR_CORNER(x, y) && !G_BL_CORNER(x, y) &&
                               W_TOP(x, y) && W_LEFT(x, y) && W_BOT(x, y) && W_RIGHT(x, y);
            bool edge_wt_gl8 = outer &&
                               !G_TL_CORNER(x, y) &&
                               G_TR_CORNER(x, y) &&
                               //    G_BR_CORNER(x, y) &&
                               //    G_BL_CORNER(x, y) &&

                               W_TOP(x, y) && W_LEFT(x, y) && !W_BOT(x, y) && W_RIGHT(x, y);

            bool edge_wt_gl9 = outer &&
                               !G_TL_CORNER(x, y) && !G_TR_CORNER(x, y) && G_BR_CORNER(x, y) && (!G_BL_CORNER(x, y) || G_BL_CORNER(x, y)) &&
                               W_TOP(x, y) && !W_LEFT(x, y) && W_BOT(x, y) && W_RIGHT(x, y);
            // bool edge_wt_gl10 = outer &&
            //                   !G_TL_CORNER(x, y) && !G_TR_CORNER(x, y) && !G_BR_CORNER(x, y) && !G_BL_CORNER(x, y) &&
            //                   W_TOP(x, y) && W_LEFT(x, y) && W_BOT(x, y) && !W_RIGHT(x, y);
            bool edge_wt_gl11 = outer &&
                                // G_TL_CORNER(x, y) &&
                                !G_TR_CORNER(x, y) &&
                                G_BR_CORNER(x, y) &&
                                // G_BL_CORNER(x, y) &&
                                W_TOP(x, y) && !W_LEFT(x, y) &&
                                W_BOT(x, y) && W_RIGHT(x, y);

            // bool edge_wt_gl12 = outer &&
            //                     // G_TL_CORNER(x, y) &
            //                     !G_TR_CORNER(x, y) &&
            //                     !G_BR_CORNER(x, y) &&
            //                     // !G_BL_CORNER(x, y) &&
            //                     W_TOP(x, y) && !W_LEFT(x, y) && W_BOT(x, y) && W_RIGHT(x, y);

            bool edge_wt_gl13 = outer &&
                                !G_TL_CORNER(x, y) && G_TR_CORNER(x, y) && G_BR_CORNER(x, y) && !G_BL_CORNER(x, y) &&
                                W_TOP(x, y) && W_LEFT(x, y) && W_BOT(x, y) && W_RIGHT(x, y);

            // bool edge_wt_gl14 = outer &&
            //                     G_TL_CORNER(x, y) && G_TR_CORNER(x, y) && G_BR_CORNER(x, y) && !G_BL_CORNER(x, y) &&
            //                     !W_TOP(x, y) && !W_LEFT(x, y) && W_BOT(x, y) && W_RIGHT(x, y);

            bool edge_wt_gl15 = outer &&
                                G_TL_CORNER(x, y) && !G_TR_CORNER(x, y) && G_BR_CORNER(x, y) && !G_BL_CORNER(x, y) &&
                                W_TOP(x, y) && W_LEFT(x, y) && W_BOT(x, y) && W_RIGHT(x, y);

            bool edge_wt_gl16 = outer && // TODO: seems like we got a prob on this one
                                G_TL_CORNER(x, y) &&
                                !G_TR_CORNER(x, y) &&
                                !G_BR_CORNER(x, y) &&
                                G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                !W_BOT(x, y) &&
                                !W_RIGHT(x, y);

            bool edge_wt_gl17 = outer &&
                                // !G_TL_CORNER(x, y) && // *Note: do we really not care?
                                // !G_TR_CORNER(x, y) &&
                                !G_BR_CORNER(x, y) &&
                                G_BL_CORNER(x, y) &&

                                !W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                W_RIGHT(x, y);
            bool edge_wt_gl32 = outer &&
                                // !G_TL_CORNER(x, y) && // *Note: do we really not care?
                                // !G_TR_CORNER(x, y) &&
                                G_BR_CORNER(x, y) &&
                                G_BL_CORNER(x, y) &&

                                !W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                W_RIGHT(x, y);
            bool edge_wt_gl18 = outer &&
                                !G_TL_CORNER(x, y) &&
                                G_TR_CORNER(x, y) &&
                                !G_BR_CORNER(x, y) &&
                                G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                W_RIGHT(x, y);

            bool edge_wt_gl19 = outer &&
                                G_TL_CORNER(x, y) &&
                                // G_TR_CORNER(x, y) &&
                                // !G_BR_CORNER(x, y) &&
                                !G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                !W_RIGHT(x, y);
            bool edge_wt_gl20 = outer &&
                                !G_TL_CORNER(x, y) &&
                                // G_TR_CORNER(x, y) &&
                                // !G_BR_CORNER(x, y) &&
                                G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                !W_RIGHT(x, y);

            bool edge_wt_gl21 = outer &&
                                G_TL_CORNER(x, y) &&
                                // G_TR_CORNER(x, y) &&
                                // !G_BR_CORNER(x, y) &&
                                G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                !W_RIGHT(x, y);
            bool edge_wt_gl22 = outer &&
                                G_TL_CORNER(x, y) &&
                                !G_TR_CORNER(x, y) &&
                                // G_BR_CORNER(x, y) &&
                                //G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                !W_BOT(x, y) &&
                                W_RIGHT(x, y);

            bool edge_wt_gl23 = outer &&
                                !G_TL_CORNER(x, y) &&
                                G_TR_CORNER(x, y) &&
                                // G_BR_CORNER(x, y) &&
                                //G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                !W_BOT(x, y) &&
                                W_RIGHT(x, y);
            bool edge_wt_gl30 = outer &&
                                G_TL_CORNER(x, y) &&
                                !G_TR_CORNER(x, y) &&
                                // G_BR_CORNER(x, y) &&
                                //G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                !W_BOT(x, y) &&
                                W_RIGHT(x, y);

            bool edge_wt_gl24 = outer &&
                                // G_TL_CORNER(x, y) &&
                                G_TR_CORNER(x, y) &&
                                !G_BR_CORNER(x, y) &&
                                //G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                !W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                W_RIGHT(x, y);

            bool edge_wt_gl25 = outer &&
                                // G_TL_CORNER(x, y) &&
                                //G_TR_CORNER(x, y) &&
                                G_BR_CORNER(x, y) &&
                                !G_BL_CORNER(x, y) &&

                                !W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                W_RIGHT(x, y);

            bool edge_wt_gl26 = outer &&
                                // G_TL_CORNER(x, y) &&
                                G_TR_CORNER(x, y) &&
                                G_BR_CORNER(x, y) &&
                                // !G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                !W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                W_RIGHT(x, y);

            bool edge_wt_gl27 = outer &&
                                !G_TL_CORNER(x, y) &&
                                G_TR_CORNER(x, y) &&
                                G_BR_CORNER(x, y) &&
                                G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                W_RIGHT(x, y);

            bool edge_wt_gl28 = outer &&
                                // !G_TL_CORNER(x, y) &&
                                !G_TR_CORNER(x, y) &&
                                G_BR_CORNER(x, y) &&
                                // G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                !W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                W_RIGHT(x, y);
            bool edge_wt_gl29 = outer &&
                                G_TL_CORNER(x, y) &&
                                G_TR_CORNER(x, y) &&
                                G_BR_CORNER(x, y) &&
                                !G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                W_RIGHT(x, y);
            bool edge_wt_gl31 = outer &&
                                G_TL_CORNER(x, y) &&
                                !G_TR_CORNER(x, y) &&
                                G_BR_CORNER(x, y) &&
                                G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                W_RIGHT(x, y);

            bool edge_wt_gl33 = outer &&
                                // G_TL_CORNER(x, y) &&
                                G_TR_CORNER(x, y) &&
                                // G_BR_CORNER(x, y) &&
                                // G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                !W_LEFT(x, y) &&
                                !W_BOT(x, y) &&
                                W_RIGHT(x, y);
            bool edge_wt_gl34 = outer &&
                                G_TL_CORNER(x, y) &&
                                // !G_TR_CORNER(x, y) &&
                                // !G_BR_CORNER(x, y) &&
                                // !G_BL_CORNER(x, y) &&

                                W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                !W_BOT(x, y) &&
                                !W_RIGHT(x, y);
            bool edge_wt_gl35 = outer &&
                                // !G_TL_CORNER(x, y) &&
                                // !G_TR_CORNER(x, y) &&
                                G_BR_CORNER(x, y) &&
                                // !G_BL_CORNER(x, y) &&

                                !W_TOP(x, y) &&
                                !W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                W_RIGHT(x, y);
            bool edge_wt_gl36 = outer &&
                                // !G_TL_CORNER(x, y) &&
                                // !G_TR_CORNER(x, y) &&
                                // !G_BR_CORNER(x, y) &&
                                G_BL_CORNER(x, y) &&

                                !W_TOP(x, y) &&
                                W_LEFT(x, y) &&
                                W_BOT(x, y) &&
                                !W_RIGHT(x, y);
            bool edge_all = outer &&
                            G_TL_CORNER(x, y) &&
                            G_TR_CORNER(x, y) &&
                            G_BR_CORNER(x, y) &&
                            G_BL_CORNER(x, y) &&

                            W_TOP(x, y) &&
                            W_LEFT(x, y) &&
                            W_BOT(x, y) &&
                            W_RIGHT(x, y);

            bool edge_lr = outer &&
                           W_TOP(x, y) &&
                           !W_LEFT(x, y) &&
                           !W_RIGHT(x, y) &&
                           W_BOT(x, y);

            bool edge_tb = outer &&
                           !W_TOP(x, y) && W_LEFT(x, y) &&
                           !W_BOT(x, y) && W_RIGHT(x, y);

            sf::Vector2f pos = {(x * 32.f), (y * 32.f)};

            if (ground && !top && !left && !right && !bottom) //ok
                PushTile(pos, Tile::Type::GROUND);
            if (top && !left && !right && !bottom) //ok
                PushTile(pos, Tile::Type::TOP);
            else if (left && !right && !top && !bottom) //ok
                PushTile(pos, Tile::Type::LEFT);
            else if (bottom && !top && !left && !right) //ok
                PushTile(pos, Tile::Type::BOTTOM);
            else if (right && !left && !top && !bottom) //ok
                PushTile(pos, Tile::Type::RIGHT);

            // TODO: these are not actually wall corners, but inner ground corners, so, we should rename these
            // INNER GROUND CORNERS
            if (corner_top_left) //ok
                PushTile(pos, Tile::Type::TOP_LEFT);
            else if (corner_top_right) //ok
                PushTile(pos, Tile::Type::TOP_RIGHT);
            else if (corner_bottom_left) //ok
                PushTile(pos, Tile::Type::BOTTOM_LEFT);
            else if (corner_bottom_right) //ok
                PushTile(pos, Tile::Type::BOTTOM_RIGHT);

            else if (corners_top_left_right) //ok
                PushTile(pos, Tile::Type::TOP_LEFT_RIGHT);
            else if (corners_left_top_bottom) //ok
                PushTile(pos, Tile::Type::LEFT_TOP_BOTTOM);
            else if (corners_bottom_left_right) //ok
                PushTile(pos, Tile::Type::BOTTOM_LEFT_RIGHT);
            else if (corners_right_top_bottom) //ok
                PushTile(pos, Tile::Type::RIGHT_TOP_BOTTOM);

            else if (left && right && (!top && !bottom)) //ok
                PushTile(pos, Tile::Type::LEFT_RIGHT);
            else if (top && bottom && (!left && !right)) //ok
                PushTile(pos, Tile::Type::TOP_BOTTOM);

            // WALL EDGES
            else if (edge_lr)
                PushTile(pos, Tile::Type::EDGE_LR); // TODO: these are actually top bot / left right walls
            else if (edge_tb)
                PushTile(pos, Tile::Type::EDGE_TB);

            else if (edge_wt_gl)
                PushTile(pos, Tile::Type::EDGE_WT_GL);
            else if (edge_wt_gl1)
                PushTile(pos, Tile::Type::EDGE_WT_GL1);
            else if (edge_wt_gl2)
                PushTile(pos, Tile::Type::EDGE_WT_GL2);
            else if (edge_wt_gl3)
                PushTile(pos, Tile::Type::EDGE_WT_GL3);
            else if (edge_wt_gl4)
                PushTile(pos, Tile::Type::EDGE_WT_GL4);
            else if (edge_wt_gl5)
                PushTile(pos, Tile::Type::EDGE_WT_GL5);
            else if (edge_wt_gl6)
                PushTile(pos, Tile::Type::EDGE_WT_GL6);
            else if (edge_wt_gl7)
                PushTile(pos, Tile::Type::EDGE_WT_GL7);
            else if (edge_wt_gl8)
                PushTile(pos, Tile::Type::EDGE_WT_GL8);
            else if (edge_wt_gl9)
                PushTile(pos, Tile::Type::EDGE_WT_GL9);
            // else if (edge_wt_gl10)
            //     PushTile(pos, Tile::Type::EDGE_WT_GL10);
            else if (edge_wt_gl11)
                PushTile(pos, Tile::Type::EDGE_WT_GL11);
            // else if (edge_wt_gl12)
            //     PushTile(pos, Tile::Type::EDGE_WT_GL12);
            else if (edge_wt_gl13)
                PushTile(pos, Tile::Type::EDGE_WT_GL13);
            // else if (edge_wt_gl14)
            //     PushTile(pos, Tile::Type::EDGE_WT_GL14);
            else if (edge_wt_gl15)
                PushTile(pos, Tile::Type::EDGE_WT_GL15);
            else if (edge_wt_gl16)
                PushTile(pos, Tile::Type::EDGE_WT_GL16);
            else if (edge_wt_gl17)
                PushTile(pos, Tile::Type::EDGE_WT_GL17);
            else if (edge_wt_gl18)
                PushTile(pos, Tile::Type::EDGE_WT_GL18);
            else if (edge_wt_gl19)
                PushTile(pos, Tile::Type::EDGE_WT_GL19);
            else if (edge_wt_gl20)
                PushTile(pos, Tile::Type::EDGE_WT_GL20);
            else if (edge_wt_gl21)
                PushTile(pos, Tile::Type::EDGE_WT_GL21);
            else if (edge_wt_gl22)
                PushTile(pos, Tile::Type::EDGE_WT_GL22);
            else if (edge_wt_gl23)
                PushTile(pos, Tile::Type::EDGE_WT_GL23);
            else if (edge_wt_gl24)
                PushTile(pos, Tile::Type::EDGE_WT_GL24);
            else if (edge_wt_gl25)
                PushTile(pos, Tile::Type::EDGE_WT_GL25);
            else if (edge_wt_gl26)
                PushTile(pos, Tile::Type::EDGE_WT_GL26);
            else if (edge_wt_gl27)
                PushTile(pos, Tile::Type::EDGE_WT_GL27);
            else if (edge_wt_gl28)
                PushTile(pos, Tile::Type::EDGE_WT_GL28);
            else if (edge_wt_gl29)
                PushTile(pos, Tile::Type::EDGE_WT_GL29);
            else if (edge_wt_gl30)
                PushTile(pos, Tile::Type::EDGE_WT_GL30);
            else if (edge_wt_gl31)
                PushTile(pos, Tile::Type::EDGE_WT_GL31);
            else if (edge_wt_gl32)
                PushTile(pos, Tile::Type::EDGE_WT_GL32);

            else if (edge_wt_gl33)
                PushTile(pos, Tile::Type::EDGE_WT_GL33);
            else if (edge_wt_gl34)
                PushTile(pos, Tile::Type::EDGE_WT_GL34);
            else if (edge_wt_gl35)
                PushTile(pos, Tile::Type::EDGE_WT_GL35);
            else if (edge_wt_gl36)
                PushTile(pos, Tile::Type::EDGE_WT_GL36);
            else if (edge_all)
                PushTile(pos, Tile::Type::EDGE_ALL);

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

            else if (wall_loop)
                PushTile(pos, Tile::Type::WALL_LOOP);
            else if (ground && top && left && right && bottom) //ok
                PushTile(pos, Tile::Type::GROUND_LOOP);
            // END WALLs

            // TODO: I don't know if we need to push these, we'll see eventually
            else if (outer)
                PushTile(pos, Tile::Type::WALL);
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
            printf("%c", this->_map[i][j] + 'a');
        }
        std::cout << std::endl;
    }
}

extern std::string tileTypes[];

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
    if (this->mappedTiles[type].getSize().x == 0) {
        switch (type) {
        case Tile::Type::OUTER:
            tile.sprite.setTextureRect({w * 0, h * 0, w, h});
            break;
        case Tile::Type::GROUND:
            tile.sprite.setTextureRect({w * 1, h * 0, w, h});
            break;
        case Tile::Type::GROUND_LOOP:
            tile.sprite.setTextureRect({w * 1, h * 3, w, h});
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
        case Tile::Type::WALL_LOOP:
            tile.sprite.setTextureRect({w * 7, h * 5, w, h});
            break;

        case Tile::Type::EDGE_TB:
            tile.sprite.setTextureRect({w * 0, h * 1, w, h});
            break;
        case Tile::Type::EDGE_LR:
            tile.sprite.setTextureRect({w * 6, h * 1, w, h});
            break;
        case Tile::Type::EDGE_WT_GL:
            tile.sprite.setTextureRect({w * 6, h * 4, w, h});
            break;
        case Tile::Type::EDGE_WT_GL1:
            tile.sprite.setTextureRect({w * 0, h * 5, w, h});
            break;
        case Tile::Type::EDGE_WT_GL2:
            tile.sprite.setTextureRect({w * 3, h * 5, w, h});
            break;
        case Tile::Type::EDGE_WT_GL3:
            tile.sprite.setTextureRect({w * 1, h * 3, w, h});
            break;
        case Tile::Type::EDGE_WT_GL4:
            tile.sprite.setTextureRect({w * 1, h * 4, w, h});
            break;
        case Tile::Type::EDGE_WT_GL5:
            tile.sprite.setTextureRect({w * 1, h * 5, w, h});
            break;
        case Tile::Type::EDGE_WT_GL6:
            tile.sprite.setTextureRect({w * 5, h * 5, w, h});
            break;
        case Tile::Type::EDGE_WT_GL7:
            tile.sprite.setTextureRect({w * 4, h * 5, w, h});
            break;
        case Tile::Type::EDGE_WT_GL8:
            tile.sprite.setTextureRect({w * 2, h * 1, w, h});
            break;
        case Tile::Type::EDGE_WT_GL9:
            tile.sprite.setTextureRect({w * 4, h * 2, w, h});
            break;
        case Tile::Type::EDGE_WT_GL10:
            tile.sprite.setTextureRect({w * 1, h * 2, w, h});
            break;
        case Tile::Type::EDGE_WT_GL11:
            tile.sprite.setTextureRect({w * 4, h * 2, w, h});
            break;

        case Tile::Type::EDGE_WT_GL13:
            tile.sprite.setTextureRect({w * 7, h * 3, w, h});
            break;
        case Tile::Type::EDGE_WT_GL14:
            tile.sprite.setTextureRect({w * 2, h * 2, w, h});
            break;
        case Tile::Type::EDGE_WT_GL15:
            tile.sprite.setTextureRect({w * 0, h * 4, w, h});
            break;
        case Tile::Type::EDGE_WT_GL16:
            tile.sprite.setTextureRect({w * 3, h * 0, w, h});
            break;
        case Tile::Type::EDGE_WT_GL17:
            tile.sprite.setTextureRect({w * 5, h * 4, w, h});
            break;
        case Tile::Type::EDGE_WT_GL18:
            tile.sprite.setTextureRect({w * 7, h * 4, w, h});
            break;
        case Tile::Type::EDGE_WT_GL19:
            tile.sprite.setTextureRect({w * 3, h * 3, w, h});
            break;
        case Tile::Type::EDGE_WT_GL20:
            tile.sprite.setTextureRect({w * 1, h * 2, w, h});
            break;
        case Tile::Type::EDGE_WT_GL21:
            tile.sprite.setTextureRect({w * 0, h * 2, w, h});
            break;
        case Tile::Type::EDGE_WT_GL22:
            tile.sprite.setTextureRect({w * 3, h * 1, w, h});
            break;
        case Tile::Type::EDGE_WT_GL23:
            tile.sprite.setTextureRect({w * 0, h * 2, w, h});
            break;
        case Tile::Type::EDGE_WT_GL24:
            tile.sprite.setTextureRect({w * 3, h * 4, w, h});
            break;
        case Tile::Type::EDGE_WT_GL25:
            tile.sprite.setTextureRect({w * 5, h * 3, w, h});
            break;
        case Tile::Type::EDGE_WT_GL26:
            tile.sprite.setTextureRect({w * 3, h * 2, w, h});
            break;
        case Tile::Type::EDGE_WT_GL27:
            tile.sprite.setTextureRect({w * 7, h * 2, w, h});
            break;
        case Tile::Type::EDGE_WT_GL28:
            tile.sprite.setTextureRect({w * 7, h * 2, w, h});
            break;
        case Tile::Type::EDGE_WT_GL29:
            tile.sprite.setTextureRect({w * 6, h * 3, w, h});
            break;
        case Tile::Type::EDGE_WT_GL30:
            tile.sprite.setTextureRect({w * 6, h * 3, w, h});
            break;
        case Tile::Type::EDGE_WT_GL31:
            tile.sprite.setTextureRect({w * 0, h * 3, w, h});
            break;
        case Tile::Type::EDGE_WT_GL32:
            tile.sprite.setTextureRect({w * 5, h * 2, w, h});
            break;

        case Tile::Type::EDGE_WT_GL33:
            tile.sprite.setTextureRect({w * 6, h * 0, w, h});
            break;
        case Tile::Type::EDGE_WT_GL34:
            tile.sprite.setTextureRect({w * 3, h * 0, w, h});
            break;
        case Tile::Type::EDGE_WT_GL35:
            tile.sprite.setTextureRect({w * 2, h * 2, w, h});
            break;
        case Tile::Type::EDGE_WT_GL36:
            tile.sprite.setTextureRect({w * 7, h * 1, w, h});
            break;
        case Tile::Type::EDGE_ALL:
            tile.sprite.setTextureRect({w * 6, h * 2, w, h});
            break;

        case Tile::Type::WALL_TOP_BOTTOM:
            tile.sprite.setTextureRect({w * 0, h * 1, w, h});
            break;
        case Tile::Type::WALL_LEFT_RIGHT:
            tile.sprite.setTextureRect({w * 2, h * 1, w, h});
            break;

        default:
            tile.sprite.setTextureRect({w * 0, h * 0, w, h});
            break;
        }
    } else {
        printf("mappedTiles type[%d] texture size: %d\n", type, mappedTiles[type].getSize().x);
        tile.sprite.setTexture(mappedTiles[type], true);

        if (tile.type >= Tile::Type::WALL) {
            // tile.sprite.setTextureRect({w * 0, h * 0, w, h});
            // tile.sprite.setColor(sf::Color(40, 0, 0));
            this->wallTiles.push_back(tile.sprite.getGlobalBounds());
        }
    }
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