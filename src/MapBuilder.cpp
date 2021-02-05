#include "MapBuilder.hpp"
#include <vector>

sf::RenderTexture gridRenderTexture;
std::vector<sf::RectangleShape> grid;
std::vector<sf::RectangleShape *> currentSelectedGridSquares;

std::vector<sf::IntRect> tileRects;
sf::Texture tileTexture;
sf::Sprite tileSprite;

/**
 * a spriteSheet should:
 * derive sf::Sprite
 * load it's spriteSheet into a member texture; .create(fineName, tileSize)
 * split itself
 * set it's sprite
 */
struct SpriteSheet {
    std::vector<sf::IntRect> tileRects;
    sf::Texture tileTexture;
    sf::Sprite tileSprite;
};
std::vector<SpriteSheet> spriteSheets;

std::string tileTypes[]{
    {"GROUND"},
    {"TOP"},
    {"LEFT"},
    {"BOTTOM"},
    {"RIGHT"},
    {"TOP_LEFT"},
    {"TOP_RIGHT"},
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
    {"WALL_TOP_LEFT_RIGHT"},
    {"WALL_LEFT_TOP_BOTTOM"},
    {"WALL_BOTTOM_LEFT_RIGHT"},
    {"WALL_RIGHT_TOP_BOTTOM"},
};

void InitMapBuilder(sf::RenderWindow &window, Map &map)
{
    splitSpriteSheet();

    float width = map.getGlobalBounds().width;
    float height = map.getGlobalBounds().height;
    gridRenderTexture.create(width, height);
    renderMapBuilderGrid(window, map);
}

Map::Tile selectedTile;

void updateMapBuilder(sf::RenderWindow &window, Map &map)
{
    ImGuiIO io = ImGui::GetIO();

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    static size_t selectedTileIdx = -1;
    static bool isSelection = false;

    for (size_t gridIdx = 0; gridIdx < grid.size(); gridIdx++) {
        if (grid[gridIdx].getGlobalBounds().contains(mousePos)) {
            grid[gridIdx].setFillColor(sf::Color(20, 20, 20, 100));

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !io.WantCaptureMouse) {
                if (currentSelectedGridSquares.size() > 0)
                    currentSelectedGridSquares.clear();

                selectedTileIdx = gridIdx;
                isSelection = true;

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
        // for (int mapIdx = 0; mapIdx < map.mapTiles.size(); mapIdx++) {
        //     if (selectedTileIdx == -1)
        //         break;
        //     if (map.mapTiles[mapIdx].sprite.getPosition() == map.mapTiles[selectedTileIdx].sprite.getPosition()) {
        //         ImGui::Text("%s", tileTypes[map.mapTiles[mapIdx].type].c_str());
        //     }
        // }
    }

    static bool p_open;
    if (isSelection) {
        static int clickedIndex = -1;

        {   //* GUI: Map Builder//////////////////////////////////////////////////////
            //* //////////////////////////////////////////////////////////////////////
            /**
             * display all loaded sprite sheets
             */
            ImGui::Begin("Map Builder", &p_open, ImGuiWindowFlags_Modal | ImGuiWindowFlags_MenuBar);
            ImGui::Text("Selected Tile:");
            ImGui::Text("wan capture mouse: %d", io.WantCaptureMouse);
            ImGui::Image(map.mapTiles[selectedTileIdx].sprite, {64, 64});
            ImGui::SameLine(0, -1);
            ImGui::Text("Type: %s", tileTypes[map.mapTiles[selectedTileIdx].type].c_str());
            ImGui::Separator();
            ImGui::Text("%d tiles loaded from tileset", tileRects.size());

            for (size_t i = 0; i < tileRects.size(); i++) {
                tileSprite.setTextureRect(tileRects[i]);

                if (i % 4)
                    ImGui::SameLine();
                ImGui::ImageButton(tileSprite, {40, 40}, 1, sf::Color(30, 30, 30));

                if (ImGui::IsItemClicked()) {
                    clickedIndex = i;

                    Map::Tile selectedTile = map.mapTiles[selectedTileIdx];
                    for (size_t j = 0; j < map.mapTiles.size(); j++) {
                        if (map.mapTiles[j].type == selectedTile.type)
                            map.mapTiles[j].sprite.setTextureRect(tileRects[clickedIndex]);
                    }
                    map.redraw();
                    break;
                }
            }
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

void redrawMapBuilder()
{
    gridRenderTexture.clear(sf::Color::Transparent);
    // 2. draw grid into gridRenderTexture
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
            spriteSheetTextures.push_back(tmp);
    }
}