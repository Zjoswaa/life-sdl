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

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    return 0;
}

int App::run() {
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

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
                        default:
                            break;
                    }

                // Mouse press
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    switch (e.button.button) {
                        case SDL_BUTTON_LEFT:
                            SDL_GetMouseState(&mouseX, &mouseY);
                            std::cout << "Left mouse pressed: " << mouseX << "(" << static_cast<int>(mouseX / CELL_WIDTH) << ") " << mouseY << "(" << static_cast<int>(mouseY / CELL_HEIGHT) << ")" << std::endl;
                            grid[static_cast<int>(mouseX / CELL_WIDTH)][static_cast<int>(mouseY / CELL_HEIGHT)].flip();
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
        ImGui::ColorEdit3("Alive", reinterpret_cast<float *>(&aliveCellColor));
        ImGui::ColorEdit3("Dead", reinterpret_cast<float *>(&deadCellColor));
        // ImGui::ColorEdit3("Background", reinterpret_cast<float*>(&backgroundColor));
        ImGui::End();

        // ImGui::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, static_cast<unsigned char>(backgroundColor.x * 255), static_cast<unsigned char>(backgroundColor.y * 255), static_cast<unsigned char>(backgroundColor.z * 255), static_cast<unsigned char>(backgroundColor.w * 255));
        SDL_RenderClear(renderer);
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
    }

    return 0;
}
