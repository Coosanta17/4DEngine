#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <string_view>
#include <SDL3/SDL.h>
#include "renderer.hpp"

class AppWindow {
    SDL_Window* window = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;
    int width, height;

public:
    AppWindow(std::string_view title, int w, int h);
    ~AppWindow();

    // Prevent copying
    AppWindow(const AppWindow&) = delete;
    AppWindow& operator=(const AppWindow&) = delete;
    // Allow moving
    AppWindow(AppWindow&& other) noexcept;
    AppWindow& operator=(AppWindow&& other) noexcept;

    [[nodiscard]] bool isValid() const noexcept { return window && sdlRenderer; }

    [[nodiscard]] SDL_Window* getSDLWindow() const noexcept { return window; }
    [[nodiscard]] SDL_Renderer* getSDLRenderer() const noexcept { return sdlRenderer; }

    [[nodiscard]] int getWidth() const noexcept { return width; }
    [[nodiscard]] int getHeight() const noexcept { return height; }

    void clear(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255) const;
    void present() const;

    void setMouseGrab(bool grab) const;
    void setRelativeMouseMode(bool relative) const;

    [[nodiscard]] Renderer3D createRenderer3D() const { return Renderer3D{sdlRenderer, width, height}; }
    [[nodiscard]] Renderer4D createRenderer4D() const { return Renderer4D{sdlRenderer, width, height}; }
};

#endif
