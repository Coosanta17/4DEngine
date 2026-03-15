#include <SDL2/SDL.h>

#include "include/renderer.hpp"

int main() {
    constexpr int width = 800;
    constexpr int height = 600;

    // FIXME
    SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "0");
    SDL_SetHint(SDL_HINT_VIDEO_WAYLAND_EMULATE_MOUSE_WARP, "1");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "3D Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS
    );
    if (window == nullptr) {
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(100);
    SDL_PumpEvents();
    SDL_SetRelativeMouseMode(SDL_TRUE);

    if (sdlRenderer == nullptr) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const Renderer renderer{sdlRenderer, width, height};

    Camera cam;
    const Mesh cube = Mesh::makeCube();
    const Mesh axes = Mesh::makeAxes();

    bool running = true;
    bool mouseLocked = true;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        const Uint8* keys = SDL_GetKeyboardState(nullptr);

        static bool escapePressed = false;
        if (keys[SDL_SCANCODE_ESCAPE]) {
            if (!escapePressed) {
                mouseLocked = !mouseLocked;
                SDL_SetRelativeMouseMode(mouseLocked ? SDL_TRUE : SDL_FALSE);
                SDL_SetWindowMouseGrab(window, mouseLocked ? SDL_TRUE : SDL_FALSE);
                escapePressed = true;
            }
        }
        else {
            escapePressed = false;
        }

        if (keys[SDL_SCANCODE_W]) cam.moveForward();
        if (keys[SDL_SCANCODE_S]) cam.moveBackward();
        if (keys[SDL_SCANCODE_A]) cam.moveLeft();
        if (keys[SDL_SCANCODE_D]) cam.moveRight();
        if (keys[SDL_SCANCODE_SPACE]) cam.moveUp();
        if (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT]) cam.moveDown();

        int mouseX = 0, mouseY = 0;
        if (mouseLocked) {
            SDL_GetRelativeMouseState(&mouseX, &mouseY);
            cam.deltaLook(mouseX, mouseY);
        }

        if (keys[SDL_SCANCODE_LEFT]) cam.lookLeft();
        if (keys[SDL_SCANCODE_RIGHT]) cam.lookRight();
        if (keys[SDL_SCANCODE_UP]) cam.lookUp();
        if (keys[SDL_SCANCODE_DOWN]) cam.lookDown();

        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
        SDL_RenderClear(sdlRenderer);

        SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 255);
        renderer.drawMesh(cube, cam);

        SDL_SetRenderDrawColor(sdlRenderer, 255, 0, 0, 128);
        renderer.drawMesh(axes, cam);

        SDL_RenderPresent(sdlRenderer);
    }

    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
