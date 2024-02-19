#ifndef APP_H
#define APP_H

#include <iostream>
#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#ifdef DEBUG
#define LOG(x) std::cout << x << std::endl;
#define ERROR(x) std::cerr << x << std::endl;
#else
#define LOG(x)
#define ERROR(x)
#endif

class App {
public:
    App();
    ~App();

    int init();
    int run();
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};

#endif
