#include "Game.hpp"
#include "Map.hpp"
#include "MapBuilder.hpp"
#include "SFML/System.hpp"

#include <vector>

#if _WIN32

#include <windows.h>
#endif

std::vector<std::string> fileNames;
std::vector<sf::Texture> spriteSheetTextures;

void loadDroppedImages();

void run()
{
    {
        window.create(sf::VideoMode{1280, 720}, "Map Builder", sf::Style::Default);
        window.setFramerateLimit(160);
        // window.setVerticalSyncEnabled(true);

        map.create(window.getSize(), {40, 30}, 4, 3, 3, 36);
        map.render();
        InitMapBuilder(window, map);

        skeleton.create();
    }

    sf::Vector2u windowSize  = window.getSize();
    sf::View     defaultView = window.getDefaultView();
    sf::View     renTexView  = map.renderTexture.getView();
    renTexView.zoom(zoom);

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

    HWND         hwnd       = window.getSystemHandle();
    LPDROPTARGET dropTarget = NULL;

    RegisterDragDrop(hwnd, dropTarget);
    DragAcceptFiles(hwnd, TRUE);

    MSG msg;
    msg.message = static_cast<UINT>(~WM_QUIT);

    /**
     * Make a proper message loop some day
     * right now, WM_DROPFILES gets skipped sometimes, somehow
     */
    while (msg.message != WM_QUIT) {

        //* Win32 messages /////////////////////////////////////////
        HACCEL hAccelTable = NULL;

        if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
            if (!TranslateAcceleratorW(msg.hwnd, hAccelTable, &msg)) {
                if (msg.message == WM_DROPFILES) {
                    fileNames.clear();

                    // MessageBox(hwnd, "some text", "idk", MB_OK | MB_RETRYCANCEL);

                    HDROP    drop                  = (HDROP)msg.wParam;
                    UINT     filePathesCount       = DragQueryFileW(drop, 0xFFFFFFFF, NULL, 512); //If "0xFFFFFFFF" as the second parameter: return the count of files dropped
                    wchar_t *fileName              = NULL;
                    UINT     longestFileNameLength = 0;
                    for (UINT i = 0; i < filePathesCount; ++i) {
                        //If NULL as the third parameter: return the length of the path, not counting the trailing '0'
                        UINT fileNameLength = DragQueryFileW(drop, i, NULL, 512) + 1;
                        if (fileNameLength > longestFileNameLength) {
                            longestFileNameLength = fileNameLength;
                            fileName              = (wchar_t *)realloc(fileName, longestFileNameLength * sizeof(*fileName));
                        }
                        DragQueryFileW(drop, i, fileName, fileNameLength);

                        // std::wcout << fileName << std::endl;

                        char * s    = (char *)malloc(fileNameLength);
                        size_t size = std::wcstombs(s, fileName, fileNameLength);
                        if (size <= 0)
                            printf("something went wrong...! size: %zd\n", size);

                        std::string stmp(s);
                        if (!std::strstr(stmp.c_str(), ".png")) {
                            printf("not a png file\n");
                            break;
                        }
                        fileNames.push_back(s);
                    }
                    free(fileName);
                    DragFinish(drop);

                    /**
                     *! make some kind of ImGui popup asking for the size of a tile
                     * now that we have the file names that were droppped
                     * we can load those .png files into textures
                    */
                    loadDroppedImages(); // should this be called here?
                }

                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        //* END Win32 messages
        //* ///////////////////////////////////////

        else {
            sf::Event e;
            ImGuiIO   io;
            while (window.pollEvent(e)) {
                ImGui::SFML::ProcessEvent(e);
                sf::FloatRect fRect     = {0, 0, (float)e.size.width, (float)e.size.height};
                static float  zoomDelta = 1;

                switch (e.type) {
                case sf::Event::MouseWheelScrolled:
                    renTexView.zoom(zoomDelta += (0.03f * e.mouseWheelScroll.delta));
                    printf("%f\n", zoomDelta);
                    zoomDelta = 1;
                    break;
                case sf::Event::EventType::Resized:
                    renTexView.setSize({(float)e.size.width, (float)e.size.height});
                    // renTexView.zoom(zoom);
                    break;
                case sf::Event::Closed:
                    // window.close();
                    msg.message = WM_QUIT;
                    break;
                case sf::Event::KeyPressed:
                    onKeyHeld(e.key.code);
                    break;

                case sf::Event::KeyReleased:
                    if (e.key.code == sf::Keyboard::Escape) {
                        // window.close();
                        msg.message = WM_QUIT;
                    }

                    onKeyUp(e.key.code);
                    break;
                default:
                    break;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
                map.generate();
                map.render();

                // renderMapBuilderGrid(window, map);
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
                //renderMapBuilder(window);
                window.draw(skeleton);
            }

            ImGui::ShowDemoWindow();
            ImGui::Begin("Info", &p_open, fpsFlags);
            ImGui::Text("fps: %.2f, %.2fms", fps, deltaTime);
            ImGui::End();

            window.setView(defaultView);
            ImGui::SFML::Render(window);

            window.display();

            {
                t_deltaTime  = deltaClock.getElapsedTime();
                currentTime  = clock.getElapsedTime();
                fps          = 1.0f / (currentTime.asSeconds() - previousTime.asSeconds());
                previousTime = currentTime;
                deltaTime    = 1.0f / fps * 100.0f;
            }
        }
    }

    DragAcceptFiles(hwnd, FALSE);
    std::cout << "Game Closed!\n";
}

//* Utility functions ///////////////////////////

void onKeyHeld(sf::Keyboard::Key key)
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

void onKeyUp(sf::Keyboard::Key key)
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
