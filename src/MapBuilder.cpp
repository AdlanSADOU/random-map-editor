#include "MapBuilder.hpp"
#include <vector>

sf::RenderTexture gridRenderTexture;
std::vector<sf::RectangleShape> grid;
std::vector<sf::RectangleShape *> currentSelectedGridSquares;

std::vector<sf::IntRect> tileRects;
sf::Texture tileTexture;
sf::Sprite tileSprite;

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
Map::Tile::Type PrevSelectedType;

void updateMapBuilder(sf::RenderWindow &window, Map &map)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    static size_t selectedTileIdx = -1;
    static bool isSelection = false;

    /** 3. iterate over grid
     *     3.01 for earch square
     *          3.02 highlight on hover
     *          3.03 if clicked & square contains mouse
     *              3.04 reset currentSelected... (previous selection)
     *              3.05 push squares into currentSelected...
     *              3.06 highlight all squares of the same type
     */

    for (size_t gridIdx = 0; gridIdx < grid.size(); gridIdx++) {
        if (grid[gridIdx].getGlobalBounds().contains(mousePos)) {
            grid[gridIdx].setFillColor(sf::Color(20, 20, 20, 100));

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
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
        currentSelectedGridSquares[i]->setFillColor(sf::Color(20, 20, 20, 100));
    }

    redrawMapBuilder();

    for (int mapIdx = 0; mapIdx < map.mapTiles.size(); mapIdx++) {
        if (selectedTileIdx == -1) break;
        if (map.mapTiles[mapIdx].sprite.getPosition() == map.mapTiles[selectedTileIdx].sprite.getPosition()) {
            ImGui::Text("%s", tileTypes[map.mapTiles[mapIdx].type].c_str());
        }
    }

    if (isSelection) {
        static int clickedIndex = -1;
        ImGui::Begin("Map Builder");

        ImGui::Text("Selected Tile:");
        ImGui::Image(map.mapTiles[selectedTileIdx].sprite, {64, 64});
        ImGui::SameLine(0, -1);
        ImGui::Text("Type: %s", tileTypes[map.mapTiles[selectedTileIdx].type].c_str());
        ImGui::Separator();

        for (size_t i = 0; i < tileRects.size(); i++) {
            tileSprite.setTextureRect(tileRects[i]);

            if (i % 5)
                ImGui::SameLine();
            ImGui::ImageButton(tileSprite, {40, 40}, 1, sf::Color(30, 30, 30));
            if (ImGui::IsItemHovered()) {
            }
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
 * push the selected tile into a vector
 * use that vector to set the right tile rects in the Map
 * then redraw the map
 *
 * Ultimately, the selected tiles must be saved into a file
 * then loaded at runtime
 */
void splitSpriteSheet()
{
    sf::Vector2i tileSize = {32, 32};
    sf::Vector2i textureSize = {};
    tileSprite.setTexture(tileTexture);

    tileTexture.loadFromFile("./res/sprites/Tileset.png");
    textureSize = static_cast<sf::Vector2i>(tileTexture.getSize());

    for (int y = 0; y <= textureSize.y; y += tileSize.y) {
        for (int x = 0; x <= textureSize.x; x += tileSize.x) {
            sf::IntRect tileRect;
            tileRect = {x, y, tileSize.x, tileSize.y};
            tileRects.push_back(tileRect);
        }
    }
}
