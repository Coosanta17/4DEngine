#include "../include/AppWindow.hpp"
#include <utility>

AppWindow::AppWindow(const std::string_view title, const int w, const int h)
    : width(w), height(h) {
    window = SDL_CreateWindow(
        title.data(),
        w, h,
        SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_VULKAN //| SDL_WINDOW_RESIZABLE
    );
    if (window) {
        sdlRenderer = SDL_CreateRenderer(window, nullptr);
    }
}

AppWindow::~AppWindow() {
    if (sdlRenderer) {
        SDL_DestroyRenderer(sdlRenderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
}

AppWindow::AppWindow(AppWindow&& other) noexcept
    : window(std::exchange(other.window, nullptr)),
      sdlRenderer(std::exchange(other.sdlRenderer, nullptr)),
      width(other.width),
      height(other.height) {}

AppWindow& AppWindow::operator=(AppWindow&& other) noexcept {
    if (this != &other) {
        if (sdlRenderer) SDL_DestroyRenderer(sdlRenderer);
        if (window) SDL_DestroyWindow(window);

        window = std::exchange(other.window, nullptr);
        sdlRenderer = std::exchange(other.sdlRenderer, nullptr);
        width = other.width;
        height = other.height;
    }
    return *this;
}

void AppWindow::clear(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const {
    SDL_SetRenderDrawColor(sdlRenderer, r, g, b, a);
    SDL_RenderClear(sdlRenderer);
}

void AppWindow::present() const {
    SDL_RenderPresent(sdlRenderer);
}

void AppWindow::setMouseGrab(const bool grab) const {
    SDL_SetWindowMouseGrab(window, grab);
}

void AppWindow::setRelativeMouseMode(const bool relative) const {
    SDL_SetWindowRelativeMouseMode(window, relative);
}
