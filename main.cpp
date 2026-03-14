#include <SDL2/SDL.h>

#include "include/renderer.hpp"

int main() {
    constexpr int width = 800;
    constexpr int height = 600;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "3D Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        0
    );
    if (window == nullptr) {
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (sdlRenderer == nullptr) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const Renderer renderer{sdlRenderer, width, height};

    Camera cam;
    const Mesh cube = Mesh::makeCube();

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        const Uint8* keys = SDL_GetKeyboardState(nullptr);

        constexpr float speed = 0.0005f;
        constexpr float rotSpeed = 0.0003f;

        if (keys[SDL_SCANCODE_W]) cam.pos += cam.forward() * speed;
        if (keys[SDL_SCANCODE_S]) cam.pos -= cam.forward() * speed;
        if (keys[SDL_SCANCODE_A]) cam.pos -= cam.right() * speed;
        if (keys[SDL_SCANCODE_D]) cam.pos += cam.right() * speed;
        if (keys[SDL_SCANCODE_SPACE]) cam.pos -= cam.up() * speed;
        if (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT]) cam.pos += cam.up() * speed;

        if (keys[SDL_SCANCODE_LEFT]) cam.yaw -= rotSpeed;
        if (keys[SDL_SCANCODE_RIGHT]) cam.yaw += rotSpeed;
        if (keys[SDL_SCANCODE_UP]) cam.pitch += rotSpeed;
        if (keys[SDL_SCANCODE_DOWN]) cam.pitch -= rotSpeed;

        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
        SDL_RenderClear(sdlRenderer);

        SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 255);
        renderer.drawMesh(cube, cam);

        SDL_RenderPresent(sdlRenderer);
    }

    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
