#include "app.h"
#include "constants.h"

App::App() {
    window = nullptr;
    renderer = nullptr;

    backgroundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    running = true;
    paused = false;
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
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    constexpr SDL_FRect source{0, 0, (WINDOW_WIDTH - 100) / TEXTURE_ZOOM, WINDOW_HEIGHT / TEXTURE_ZOOM};
    constexpr SDL_FRect dest{0, 0, WINDOW_WIDTH - 100, WINDOW_HEIGHT};
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (WINDOW_WIDTH - 100) / TEXTURE_ZOOM, WINDOW_HEIGHT / TEXTURE_ZOOM);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    std::vector<SDL_Point> pv;
    for (int i = 0; i < 20; i++) {
        pv.emplace_back(rand() % (WINDOW_WIDTH - 100) / TEXTURE_ZOOM, rand() % WINDOW_HEIGHT / TEXTURE_ZOOM);
    }

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
                            std::cout << "Left mouse pressed: " << mouseX << " " << mouseY << std::endl;
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
        ImGui::SetNextWindowSize(ImVec2(100, 200));
        ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH - 100, 0));
        ImGui::Begin("Settings", nullptr, windowFlags);
        ImGui::Checkbox("Pause", &paused);
        ImGui::End();

        // ImGui::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        if (!paused) {
            for (SDL_Point& point: pv) {
                point.x += rand() % 3 - 1;
                point.y += rand() % 3 - 1;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (SDL_Point& point: pv) {
            SDL_RenderPoint(renderer, point.x, point.y);
        }
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, &source, &dest);
        // SDL_SetRenderDrawColor(renderer, static_cast<unsigned char>(backgroundColor.x * 255), static_cast<unsigned char>(backgroundColor.y * 255), static_cast<unsigned char>(backgroundColor.z * 255), static_cast<unsigned char>(backgroundColor.w * 255));
        // SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);

        SDL_Delay(50);
    }

    return 0;
}
