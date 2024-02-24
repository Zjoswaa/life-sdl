#include "app.h"

App::App() {
    window = nullptr;
    renderer = nullptr;
    aliveCellColor = {0.f, 0.f, 0.f, 1.f};
    deadCellColor = {1.f, 1.f, 1.f, 1.f};
    for (int i = 0; i < GRID_WIDTH / CELL_WIDTH; i++) {
        std::vector<Cell> column;
        for (int j = 0; j < GRID_HEIGHT/ CELL_HEIGHT; j++) {
            column.emplace_back(static_cast<float>(i) * CELL_WIDTH, static_cast<float>(j) * CELL_HEIGHT);
        }
        grid.emplace_back(column);
    }
    LOG("Initialized grid")
    LOG("Grid width: " << grid.size())
    LOG("Grid height: " << grid[0].size())

    backgroundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    running = true;
    paused = true;
    mouseX = 0;
    mouseY = 0;
    delay = 50;
    ctrlDown = false;
}

App::~App() {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int App::init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO)) {
        ERROR("Failed to initialize SDL: " << SDL_GetError())
        return 1;
    }
    LOG("Intialized SDL")

    // Create window
    window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (window == nullptr) {
        ERROR("Failed to create window: " << SDL_GetError())
        return 2;
    }
    LOG("Created window")

    // Create renderer
    renderer = SDL_CreateRenderer(window, nullptr, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        ERROR("Failed to create renderer: " << SDL_GetError())
        return 3;
    }
    LOG("Created renderer")
    LOG("")

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    return 0;
}

int App::run() {
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Set ImGui theme
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    while (running) {
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL3_ProcessEvent(&e);
            switch (e.type) {
                // Quit program
                case SDL_EVENT_QUIT:
                    running = false;
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    if (e.window.windowID == SDL_GetWindowID(window)) {
                        running = false;
                    }

                // Keyboard press
                case SDL_EVENT_KEY_DOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            paused = !paused;
                            LOG("Escape Pressed: " << paused)
                            break;
                        case SDLK_r:
                            addRandomCells();
                            LOG("R Pressed")
                            break;
                        case SDLK_c:
                            clearGrid();
                            LOG("C Pressed")
                            break;
                        case SDLK_UP:
                            if (SDL_GetModState() == SDL_KMOD_LCTRL || SDL_GetModState() == SDL_KMOD_RCTRL) {
                                delay = std::min(MAX_DELAY, delay + 5);
                            } else {
                                delay = std::min(MAX_DELAY, delay + 1);
                            }
                            LOG("Up Pressed")
                            break;
                        case SDLK_DOWN:
                            if (SDL_GetModState() == SDL_KMOD_LCTRL || SDL_GetModState() == SDL_KMOD_RCTRL) {
                                delay = std::max(MIN_DELAY, delay - 5);
                            } else {
                                delay = std::max(MIN_DELAY, delay - 1);
                            }
                            LOG("Down Pressed")
                            break;
                        case SDLK_SPACE:
                            if (paused) {
                                nextGeneration();
                            }
                            LOG("Space Pressed")
                            break;
                        default:
                            break;
                    }

                // Mouse press
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    switch (e.button.button) {
                        case SDL_BUTTON_LEFT:
                            SDL_GetMouseState(&mouseX, &mouseY);
                            if (mouseX < GRID_WIDTH && mouseY < GRID_HEIGHT && mouseX >= 0 && mouseY >= 0) {
                                LOG(mouseX << "(" << static_cast<int>(mouseX / CELL_WIDTH) << ") " << mouseY << "(" << static_cast<int>(mouseY / CELL_HEIGHT) << ")")
                                grid[static_cast<int>(mouseX / CELL_WIDTH)][static_cast<int>(mouseY / CELL_HEIGHT)].flip();
                            }
                            break;
                        default:
                            break;
                    }

                default:
                    break;
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // Create ImGui Windows
        ImGui::SetNextWindowSize(ImVec2(SETTINGS_WIDTH, SETTINGS_HEIGHT));
        ImGui::SetNextWindowPos(ImVec2(GRID_WIDTH, 0));
        ImGui::Begin("Settings", nullptr, windowFlags);
        ImGui::Checkbox("Pause (Esc)", &paused);
        if (!paused) {
            ImGui::BeginDisabled();
            ImGui::Button("Next Generation (Space)");
            ImGui::EndDisabled();
        } else {
            if (ImGui::Button("Next Generation (Space)")) {
                nextGeneration();
            }
        }
        ImGui::ColorEdit3("Alive", reinterpret_cast<float *>(&aliveCellColor));
        ImGui::ColorEdit3("Dead", reinterpret_cast<float *>(&deadCellColor));
        if (ImGui::Button("Clear (C)")) {
            clearGrid();
        }
        if (ImGui::Button("Random (R)")) {
            addRandomCells();
        }
        ImGui::SliderInt("Delay", &delay, MIN_DELAY, MAX_DELAY);
        ImGui::Text("Up/Down: +1/-1");
        ImGui::Text("Ctrl Up/Down: +5/-5");
        // ImGui::ColorEdit3("Background", reinterpret_cast<float*>(&backgroundColor));

        ImGui::End();

        // ImGui::ShowDemoWindow();

        if (!paused) {
            // Evalutation of next generation
            evaluateNextGen();
            // Copy over isAliveNext to isAlive
            for (int i = 0; i < GRID_WIDTH / CELL_WIDTH; i++) {
                for (int j = 0; j < GRID_HEIGHT/ CELL_HEIGHT; j++) {
                    grid[i][j].updateAliveStatus();
                }
            }
        }

        // Rendering
        ImGui::Render();
        for (int i = 0; i < GRID_WIDTH / CELL_WIDTH; i++) {
            for (int j = 0; j < GRID_HEIGHT/ CELL_HEIGHT; j++) {
                if (grid[i][j].isAlive) {
                    SDL_SetRenderDrawColor(renderer, static_cast<Uint8>(aliveCellColor.x * 255), static_cast<Uint8>(aliveCellColor.y * 255), static_cast<Uint8>(aliveCellColor.z * 255), static_cast<Uint8>(aliveCellColor.w * 255));
                } else {
                    SDL_SetRenderDrawColor(renderer, static_cast<Uint8>(deadCellColor.x * 255), static_cast<Uint8>(deadCellColor.y * 255), static_cast<Uint8>(deadCellColor.z * 255), static_cast<Uint8>(deadCellColor.w * 255));
                }
                SDL_RenderFillRect(renderer, &grid[i][j].rect);
            }
        }
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);

        SDL_Delay(delay);
        // while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-start).count() < delay) { }
    }

    return 0;
}

void App::clearGrid() {
    for (int i = 0; i < GRID_WIDTH / CELL_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT / CELL_HEIGHT; j++) {
            grid[i][j].isAlive = false;
            grid[i][j].isAliveNext = false;
        }
    }
}

void App::addRandomCells() {
    for (int i = 0; i < GRID_WIDTH / CELL_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT/ CELL_HEIGHT; j++) {
            if (!grid[i][j].isAlive && rand() % 2) {
                grid[i][j].isAlive = true;
            }
        }
    }
    evaluateNextGen();
}

void App::evaluateNextGen() {
    for (int i = 0; i < GRID_WIDTH / CELL_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT/ CELL_HEIGHT; j++) {
            const short aliveNeighbours = aliveNeigbourCount(i, j);
            // When a cell is currently alive
            if (grid[i][j].isAlive) {
                // If it has less than 2 or more than 3 alive neighbours, it dies in the next generation
                if (aliveNeighbours < 2 || aliveNeighbours > 3) {
                    grid[i][j].isAliveNext = false;
                }
                // Else it has 2 or 3 alive neigbours, it stays alive in the next generation
                else {
                    grid[i][j].isAliveNext = true;
                }
            }
            // When a cell is currently dead
            else {
                // If it had 3 alive neighbours, it becomes alive in the next generation
                if (aliveNeighbours == 3) {
                    grid[i][j].isAliveNext = true;
                }
            }
        }
    }
}

short App::aliveNeigbourCount(const int x, const int y) const {
    short count = 0;
    if (x - 1 >= 0 && y - 1 >= 0 && grid[x - 1][(y - 1)].isAlive) {
        count++;
    } if (y - 1 >= 0 && grid[x][y - 1].isAlive) {
        count++;
    } if (x + 1 <= (GRID_WIDTH / CELL_WIDTH) - 1 && y - 1 >= 0 && grid[x + 1][y - 1].isAlive) {
        count++;
    } if (x + 1 <= (GRID_WIDTH / CELL_WIDTH) - 1 && grid[x + 1][y].isAlive) {
        count++;
    } if (x + 1 <= (GRID_WIDTH / CELL_WIDTH) - 1 && y + 1 <= (GRID_HEIGHT / CELL_HEIGHT) - 1 && grid[x + 1][y + 1].isAlive) {
        count++;
    } if (y + 1 <= (GRID_HEIGHT / CELL_HEIGHT) - 1 && grid[x][y + 1].isAlive) {
        count++;
    } if (x - 1 >= 0 && y + 1 <= (GRID_HEIGHT / CELL_HEIGHT) - 1 && grid[x - 1][y + 1].isAlive) {
        count++;
    } if (x - 1 >= 0 && grid[x - 1][y].isAlive) {
        count++;
    }
    return count;
}

void App::nextGeneration() {
    evaluateNextGen();
    // Copy over isAliveNext to isAlive
    for (int i = 0; i < GRID_WIDTH / CELL_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT/ CELL_HEIGHT; j++) {
            grid[i][j].updateAliveStatus();
        }
    }
    // Rendering
    for (int i = 0; i < GRID_WIDTH / CELL_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT/ CELL_HEIGHT; j++) {
            if (grid[i][j].isAlive) {
                SDL_SetRenderDrawColor(renderer, static_cast<Uint8>(aliveCellColor.x * 255), static_cast<Uint8>(aliveCellColor.y * 255), static_cast<Uint8>(aliveCellColor.z * 255), static_cast<Uint8>(aliveCellColor.w * 255));
            } else {
                SDL_SetRenderDrawColor(renderer, static_cast<Uint8>(deadCellColor.x * 255), static_cast<Uint8>(deadCellColor.y * 255), static_cast<Uint8>(deadCellColor.z * 255), static_cast<Uint8>(deadCellColor.w * 255));
            }
            SDL_RenderFillRect(renderer, &grid[i][j].rect);
        }
    }
    SDL_RenderPresent(renderer);
}
