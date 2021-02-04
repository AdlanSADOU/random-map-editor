#include "Game.hpp"
#include "Map.hpp"
#include "MapBuilder.hpp"
#include "SFML/System.hpp"

#include <vector>

void Game::onKeyHeld(sf::Keyboard::Key key)
{
    switch (key) {
    case sf::Keyboard::Space:
        controls.space = true;
        break;
    case sf::Keyboard::Z:
        controls.up = true;
        break;
    case sf::Keyboard::Q:
        controls.left = true;
        break;
    case sf::Keyboard::S:
        controls.down = true;
        break;
    case sf::Keyboard::D:
        controls.right = true;
        break;
    case sf::Keyboard::LShift:
        controls.shift = true;
        if (yAxis > -0.16 && yAxis < 0.01)
            yAxis = 0;
        else
            yAxis -= yAxis / 10 * deltaTime;
        if (xAxis > -0.16 && xAxis < 0.01)
            xAxis = 0;
        else
            xAxis -= xAxis / 10 * deltaTime;
        break;
    default:
        break;
    }
}

void Game::onKeyUp(sf::Keyboard::Key key)
{
    switch (key) {
    case sf::Keyboard::Space:
        controls.space = false;
        break;
    case sf::Keyboard::Z:
        controls.up = false;
        break;
    case sf::Keyboard::Q:
        controls.left = false;
        break;
    case sf::Keyboard::S:
        controls.down = false;
        break;
    case sf::Keyboard::D:
        controls.right = false;
        break;
    case sf::Keyboard::LShift:
        controls.shift = false;
        break;
    default:
        break;
    }
}

void Game::run()
{
    std::cout << "Starting game...\n";
    window.create(sf::VideoMode{1280, 720}, "Proto", sf::Style::Default);
    window.setFramerateLimit(160);
    // window.setVerticalSyncEnabled(true);

    Map map;
    map.create(window.getSize(), {40, 27}, 4, 3, 3, 36);
    map.render();
    InitMapBuilder(window, map);

    skeleton.create();

    sf::Vector2u windowSize = window.getSize();
    sf::View renTexView = map.renderTexture.getView();
    sf::View defaultView = window.getDefaultView();
    renTexView.zoom(1);

    sf::ContextSettings settings = window.getSettings();
    ImGui::SFML::Init(window, true);
    ImGuiWindowFlags fpsFlags = 0;
    fpsFlags |= ImGuiWindowFlags_NoTitleBar;
    fpsFlags |= ImGuiWindowFlags_NoResize;
    fpsFlags |= ImGuiWindowFlags_NoDecoration;
    bool p_open;

    sf::Clock deltaClock;
    sf::Clock clock;
    sf::Time previousTime = clock.getElapsedTime();
    sf::Time currentTime;
    sf::Time t_deltaTime;

    while (window.isOpen()) {
        sf::Event e;
        ImGuiIO io;

        while (window.pollEvent(e)) {
            ImGui::SFML::ProcessEvent(e);
            io = ImGui::GetIO();

            if (e.type == sf::Event::Resized) {
                sf::FloatRect fRect = {0, 0, (float)e.size.width, (float)e.size.height};
                renTexView.setSize({(float)e.size.width, (float)e.size.height});
                renTexView.zoom(0.5);
            }

            // if (e.key.code == sf::Keyboard::Escape || e.type == sf::Event::Closed)
            //     window.close();

            switch (e.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::EventType::KeyPressed:
                onKeyHeld(e.key.code);
                break;

            case sf::Event::EventType::KeyReleased:
                if (e.key.code == sf::Keyboard::Escape)
                    window.close();
                onKeyUp(e.key.code);
                break;
            default:
                break;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
            map.generate();
            map.render();
            // InitMapBuilder(window, map);
            renderMapBuilderGrid(window, map);
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        renTexView.setCenter(skeleton.getPosition());

        window.setView(renTexView);
        window.clear(sf::Color(12, 12, 12, 255));

        { //* update()
            map.update();
            skeleton.update(controls, map);
            updateMapBuilder(window, map);
        }

        { //* render()
            window.draw(map);
            renderMapBuilder(window);
            // skeleton.renderDebugInfo(window);
            window.draw(skeleton);
        }

        // renderMapBuilder(window, map);

        ImGui::ShowDemoWindow();

        ImGui::Begin("Info", &p_open, fpsFlags);
        ImGui::Text("fps: %.2f, %.2fms", fps, deltaTime);

        ImGui::End();

        ImGui::SFML::Render(window);
        window.display();

        t_deltaTime = deltaClock.getElapsedTime();
        currentTime = clock.getElapsedTime();
        fps = 1.0f / (currentTime.asSeconds() - previousTime.asSeconds());
        previousTime = currentTime;
        deltaTime = 1.0f / fps * 100.0f;
    }
    std::cout << "Game Closed!\n";
}
