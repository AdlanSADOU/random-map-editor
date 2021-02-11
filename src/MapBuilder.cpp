#include "MapBuilder.hpp"
#include <vector>

sf::RenderTexture                 gridRenderTexture;
std::vector<sf::RectangleShape>   grid;
std::vector<sf::RectangleShape *> currentSelectedGridSquares;

std::vector<sf::IntRect> tileRects;
sf::Texture              tileTexture;
sf::Sprite               tileSprite;

/**
 * a spriteSheet should:
 * derive sf::Sprite
 * load it's spriteSheet into a member texture; .create(fineName, tileSize)
 * split itself
 * set it's sprite
 *
 * If our spriteSheet is just a sprite whose tiles are just rects
 */
struct SpriteSheet {
    sf::Sprite  sprite;
    sf::Texture texture;
    sf::Image   image;

    std::vector<sf::IntRect> rects;
    std::vector<sf::Texture> textures;

    sf::Vector2i _textureSize = {};
    sf::Vector2i _tileSize    = {};
    std::string  fileName;

    void create(std::string fileName, sf::Vector2i tileSize)
    {
        this->_tileSize = tileSize;
        this->fileName  = fileName;

        if (!this->texture.loadFromFile(fileName))
            return;

        if (!this->image.loadFromFile(fileName))
            return;

        this->sprite.setTexture(this->texture);
        this->_textureSize = static_cast<sf::Vector2i>(this->texture.getSize());

        this->split();
    }

    void split()
    {
        for (int y = 0; y < this->_textureSize.y; y += this->_tileSize.y) {
            for (int x = 0; x < this->_textureSize.x; x += this->_tileSize.x) {
                static int i;

                sf::IntRect tmpRect;
                tmpRect = {x, y, this->_tileSize.x, this->_tileSize.y};
                this->rects.push_back(tmpRect);

                sf::Texture  tmpTexture;
                sf::Vector2u dimensions = this->image.getSize();
                size_t       size       = dimensions.x * dimensions.y * 4;
                tmpTexture.loadFromImage(this->image, tmpRect);
                this->textures.push_back(tmpTexture);
            }
        }
    }
};

static std::vector<SpriteSheet> spriteSheets;

std::string tileTypes[]{
    {"GROUND"},
    {"GROUND_LOOP"},
    {"TOP"},
    {"LEFT"},
    {"BOTTOM"},
    {"RIGHT"},
    {"TOP_LEFT"},
    {"TOP_RIGHT"},
    {"LEFT_RIGHT"},
    {"TOP_BOTTOM"},
    {"BOTTOM_LEFT"},
    {"BOTTOM_RIGHT"},
    {"TOP_LEFT_RIGHT"},
    {"LEFT_TOP_BOTTOM"},
    {"BOTTOM_LEFT_RIGHT"},
    {"RIGHT_TOP_BOTTOM"},
    {"OUTER"},
    {"WALL"},
    {"WALL_TOP"},
    {"WALL_LEFT"},
    {"WALL_BOTTOM"},
    {"WALL_RIGHT"},
    {"WALL_TOP_LEFT"},
    {"WALL_TOP_RIGHT"},
    {"WALL_BOTTOM_LEFT"},
    {"WALL_BOTTOM_RIGHT"},
    {"WALL_TOP_BOTTOM"},
    {"WALL_LEFT_RIGHT"},
    {"WALL_TOP_LEFT_RIGHT"},
    {"WALL_LEFT_TOP_BOTTOM"},
    {"WALL_BOTTOM_LEFT_RIGHT"},
    {"WALL_RIGHT_TOP_BOTTOM"},
    {"WALLS_TOP_LEFT_CORNER_TL"},
    {"WALLS_TOP_LEFT_BOTTOM_CORNER_TL_BL"},
    {"WALLS_TOP_LEFT_BOTTOM_RIGHT_CORNER_TL_BL_TR"},
    {"WALLS_TOP_LEFT_BOTTOM_RIGHT_CORNER_TL_BL_TR_BR"},
    {"WALLS_TOP_LEFT_RIGHT_CORNER_TL_TR"},
    {"WALLS_LEFT_CORNER_TR"},
    {"WALL_BOTTOM_LEFT_CORNER"},
    {"WALL_TOP_RIGHT_CORNER"},
    {"WALL_TOP_LEFT_CORNER"},
    {"WALL_LOOP"},
    {"EDGE_LR"},
    {"EDGE_TB"},
    {"EDGE_WT_GL"},
    {"EDGE_WT_GL1"},
    {"EDGE_WT_GL2"},
    {"EDGE_WT_GL3"},
    {"EDGE_WT_GL4"},
    {"EDGE_WT_GL5"},
    {"EDGE_WT_GL6"},
    {"EDGE_WT_GL7"},
    {"EDGE_WT_GL8"},
    {"EDGE_WT_GL9"},
    {"EDGE_WT_GL10"},
    {"EDGE_WT_GL11"},
    {"EDGE_WT_GL12"},
    {"EDGE_WT_GL13"},
    {"EDGE_WT_GL14"},
    {"EDGE_WT_GL15"},
    {"EDGE_WT_GL16"},
    {"EDGE_WT_GL17"},
    {"EDGE_WT_GL18"},
    {"EDGE_WT_GL19"},
    {"EDGE_WT_GL20"},
    {"EDGE_WT_GL21"},
    {"EDGE_WT_GL22"},
    {"EDGE_WT_GL23"},
    {"EDGE_WT_GL24"},
    {"EDGE_WT_GL25"},
    {"EDGE_WT_GL26"},
    {"EDGE_WT_GL27"},
    {"EDGE_WT_GL28"},
    {"EDGE_WT_GL29"},
    {"EDGE_WT_GL30"},
    {"EDGE_WT_GL31"},
    {"EDGE_WT_GL32"},
    {"EDGE_WT_GL33"},
    {"EDGE_WT_GL34"},
    {"EDGE_WT_GL35"},
    {"EDGE_WT_GL36"},
    {"EDGE_ALL"},
    };

void InitMapBuilder(sf::RenderWindow &window, Map &map)
{
    float width  = map.getGlobalBounds().width;
    float height = map.getGlobalBounds().height;
    gridRenderTexture.create(width, height);

    {
        SpriteSheet sheet1;
        sheet1.create("res/sprites/walls.png", {32, 32});
        spriteSheets.push_back(sheet1);
    }
    {
        SpriteSheet sheet1;
        sheet1.create("res/sprites/tile_set.png", {32, 32});
        spriteSheets.push_back(sheet1);
    }
    renderMapBuilderGrid(window, map);
}

Map::Tile selectedTile;

void updateMapBuilder(sf::RenderWindow &window, Map &map)
{
    ImGuiIO io = ImGui::GetIO();

    sf::Vector2f  mousePos        = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    static size_t selectedTileIdx = -1;
    static bool   isSelection     = false;

    for (size_t gridIdx = 0; gridIdx < grid.size(); gridIdx++) {
        if (grid[gridIdx].getGlobalBounds().contains(mousePos)) {
            grid[gridIdx].setFillColor(sf::Color(20, 20, 20, 100));

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !io.WantCaptureMouse) {
                if (currentSelectedGridSquares.size() > 0)
                    currentSelectedGridSquares.clear(); // TODO: we should also clear when the map gets regenerated @Map::generate

                selectedTileIdx = gridIdx;
                isSelection     = true;

                Map::Tile selection = map.mapTiles[gridIdx];
                for (int mapIdx = 0; mapIdx < map.mapTiles.size(); mapIdx++) {
                    if (map.mapTiles[mapIdx].type == selection.type) {
                        currentSelectedGridSquares.push_back(&(grid[mapIdx]));
                    }
                }
            }
        } else {
            grid[gridIdx].setFillColor(sf::Color::Transparent);
        }
    }

    for (size_t i = 0; i < currentSelectedGridSquares.size(); i++) {
        currentSelectedGridSquares[i]->setFillColor(sf::Color(20, 160, 20, 100));
    }

    redrawMapBuilder();

    { //* tile type debug info
        for (int mapIdx = 0; mapIdx < map.mapTiles.size(); mapIdx++) {
            if (selectedTileIdx == -1)
                break;
            if (map.mapTiles[mapIdx].sprite.getPosition() == map.mapTiles[selectedTileIdx].sprite.getPosition()) {
                ImGui::Text("%s", tileTypes[map.mapTiles[mapIdx].type].c_str());
            }
        }
    }

    if (isSelection) {
        {
            //* GUI: Map Builder//////////////////////////////////////////////////////
            //* //////////////////////////////////////////////////////////////////////
            /**
             * display all loaded sprite sheets
             */
            static bool             p_open;
            static int              clickedIndex = -1;
            static ImGuiWindowFlags builder_flags;
            builder_flags |= ImGuiWindowFlags_Modal;
            builder_flags |= ImGuiWindowFlags_MenuBar;

            ImGui::Begin("Map Builder", &p_open, builder_flags);
            ImGui::Text("Selected Tile: %d", clickedIndex);
            ImGui::Text("wan capture mouse: %d", io.WantCaptureMouse);
            ImGui::Image(map.mapTiles[selectedTileIdx].sprite, {64, 64});
            ImGui::SameLine(0, -1);
            ImGui::Text("Type: %s", tileTypes[map.mapTiles[selectedTileIdx].type].c_str());
            ImGui::Separator();

            // static char str0[128] = "Hello, world!";
            // if (ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0))) {
            //     if ()
            // }

            static int y = 0;
            for (size_t k = 0; k < spriteSheets.size(); k++) {
                ImGui::Text("%d tiles loaded from tileset", spriteSheets[k].textures.size());
                static sf::Vector2i selectedTilePosition = {};

                // looping over textures in given spriteSheet
                for (size_t i = 0; i < spriteSheets[k].textures.size(); i++) {
                    // tileSprite.setTextureRect(spriteSheets[k].textures[i]);

                    int x = (spriteSheets[k]._textureSize.x / spriteSheets[k]._tileSize.x);
                    if (i % x) {
                        ImGui::SameLine();
                    } else if (i > 0)
                        y++;

                    ImGui::ImageButton((spriteSheets[k].textures[i]), {40, 40}, 1, sf::Color(30, 30, 30));

                    if (ImGui::IsItemClicked()) {
                        clickedIndex         = i;
                        selectedTilePosition = {(int)i % x, (int)y};

                        Map::Tile selectedTile = map.mapTiles[selectedTileIdx];
                        for (size_t j = 0; j < map.mapTiles.size(); j++) {
                            if (map.mapTiles[j].type == selectedTile.type) {
                                // map.mapTiles[j].sprite.setTextureRect(spriteSheets[k].rects[clickedIndex]);
                                // Ni
                                map.mapTiles[j].sprite.setTexture(spriteSheets[k].textures[clickedIndex], true);
                            }
                        }
                        map.redraw();
                        break;
                    }
                }
                ImGui::Text("tile at (%d, %d)", selectedTilePosition.x, selectedTilePosition.y);
            }
            y = 0;
            ImGui::End();
        } //* //////////////////////////////////////////////////////
    }
}

void renderMapBuilderGrid(sf::RenderWindow &window, Map &map)
{
    grid.clear();
    for (int i = 0; i < map.mapTiles.size(); i++) {
        auto e = &map.mapTiles[i];

        sf::RectangleShape square;

        square.setSize({e->sprite.getLocalBounds().width, e->sprite.getLocalBounds().height});
        square.setPosition({e->sprite.getGlobalBounds().left, e->sprite.getGlobalBounds().top});
        square.setFillColor(sf::Color::Transparent);
        square.setOutlineThickness(0.6);
        square.setOutlineColor(sf::Color(60, 30, 30, 155));

        // 1. push square into grid
        grid.push_back(square);
        gridRenderTexture.draw(square);
    }
    redrawMapBuilder();
}

// 2. draw grid into gridRenderTexture
void redrawMapBuilder()
{
    gridRenderTexture.clear(sf::Color::Transparent);

    for (size_t i = 0; i < grid.size(); i++) {
        gridRenderTexture.draw(grid[i]);
    }

    gridRenderTexture.display();
    gridSprite.setTexture(gridRenderTexture.getTexture());
}

void renderMapBuilder(sf::RenderWindow &window)
{
    window.draw(gridSprite);
}

//* Utility functions

/**
 * Load the sprite sheets
 */
void splitSpriteSheet(const std::string fileName, sf::Vector2i tileSize)
{
    sf::Vector2i textureSize = {};
    tileSprite.setTexture(tileTexture);

    tileTexture.loadFromFile(fileName);
    textureSize = static_cast<sf::Vector2i>(tileTexture.getSize());

    for (int y = 0; y < textureSize.y; y += tileSize.y) {
        for (int x = 0; x < textureSize.x; x += tileSize.x) {
            sf::IntRect tileRect;
            tileRect = {x, y, tileSize.x, tileSize.y};
            tileRects.push_back(tileRect);
        }
    }
}

extern std::vector<std::string> fileNames;
extern std::vector<sf::Texture> spriteSheetTextures;

void loadDroppedImages()
{
    for (auto &&name : fileNames) {
        printf(name.c_str());
        puts("\n");
        sf::Texture tmp;
        if (!tmp.loadFromFile(name))
            printf("Could not load file: %s", name.c_str());
        else
            spriteSheetTextures.push_back(tmp); // TODO: this is redundant right now, not used
        SpriteSheet sheet;
        sheet.create(name, {32, 32});
        spriteSheets.push_back(sheet);
    }
}