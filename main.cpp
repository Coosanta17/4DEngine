#include <SDL2/SDL.h>

#include "include/renderer.hpp"


constexpr int kRenderWidth = 800;
constexpr int kRenderHeight = 600;

enum class RenderMode {
    None,
    Mode3D,
    Mode4D,
};

bool isPointInsideRect(const int x, const int y, const SDL_Rect& rect) {
    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
}

void drawWireDigit3(SDL_Renderer* renderer, const SDL_Rect& area) {
    const int l = area.x;
    const int r = area.x + area.w;
    const int t = area.y;
    const int b = area.y + area.h;
    const int m = area.y + area.h / 2;

    SDL_RenderDrawLine(renderer, l, t, r, t);
    SDL_RenderDrawLine(renderer, r, t, r, b);
    SDL_RenderDrawLine(renderer, l, m, r, m);
    SDL_RenderDrawLine(renderer, l, b, r, b);
}

void drawWireDigit4(SDL_Renderer* renderer, const SDL_Rect& area) {
    const int l = area.x;
    const int r = area.x + area.w;
    const int t = area.y;
    const int b = area.y + area.h;
    const int m = area.y + area.h / 2;

    SDL_RenderDrawLine(renderer, l, t, l, m);
    SDL_RenderDrawLine(renderer, l, m, r, m);
    SDL_RenderDrawLine(renderer, r, t, r, b);
}

void drawWireLetterD(SDL_Renderer* renderer, const SDL_Rect& area) {
    const int l = area.x;
    const int r = area.x + area.w;
    const int t = area.y;
    const int b = area.y + area.h;

    SDL_RenderDrawLine(renderer, l, t, l, b);
    SDL_RenderDrawLine(renderer, l, t, r - 2, t + 2);
    SDL_RenderDrawLine(renderer, r - 2, t + 2, r, b / 2 + t / 2);
    SDL_RenderDrawLine(renderer, r, b / 2 + t / 2, r - 2, b - 2);
    SDL_RenderDrawLine(renderer, r - 2, b - 2, l, b);
}

void drawMenuButton(SDL_Renderer* renderer,
                    const SDL_Rect& button,
                    const bool hovered,
                    const bool is4DButton) {
    if (is4DButton) {
        SDL_SetRenderDrawColor(renderer, hovered ? 120 : 90, hovered ? 130 : 90, hovered ? 220 : 180, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, hovered ? 90 : 70, hovered ? 170 : 130, hovered ? 210 : 180, 255);
    }
    SDL_RenderFillRect(renderer, &button);

    SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
    SDL_RenderDrawRect(renderer, &button);

    const SDL_Rect firstGlyph{button.x + button.w / 4 - 18, button.y + button.h / 2 - 18, 26, 36};
    const SDL_Rect secondGlyph{button.x + button.w / 4 + 14, button.y + button.h / 2 - 18, 26, 36};

    if (is4DButton) {
        drawWireDigit4(renderer, firstGlyph);
    }
    else {
        drawWireDigit3(renderer, firstGlyph);
    }
    drawWireLetterD(renderer, secondGlyph);
}

RenderMode runRendererSelectionMenu() {
    constexpr int menuWidth = 420;
    constexpr int menuHeight = 220;

    SDL_Window* window = SDL_CreateWindow(
        "Select Renderer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        menuWidth,
        menuHeight,
        SDL_WINDOW_SHOWN
    );
    if (window == nullptr) {
        return RenderMode::None;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        return RenderMode::None;
    }

    const SDL_Rect button3D{30, 60, 160, 100};
    const SDL_Rect button4D{230, 60, 160, 100};

    RenderMode selected = RenderMode::None;
    bool running = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                const int mouseX = event.button.x;
                const int mouseY = event.button.y;

                if (isPointInsideRect(mouseX, mouseY, button3D)) {
                    selected = RenderMode::Mode3D;
                    running = false;
                }
                else if (isPointInsideRect(mouseX, mouseY, button4D)) {
                    selected = RenderMode::Mode4D;
                    running = false;
                }
            }
        }

        int mouseX = 0;
        int mouseY = 0;
        SDL_GetMouseState(&mouseX, &mouseY);

        SDL_SetRenderDrawColor(renderer, 24, 24, 24, 255);
        SDL_RenderClear(renderer);

        drawMenuButton(renderer, button3D, isPointInsideRect(mouseX, mouseY, button3D), false);
        drawMenuButton(renderer, button4D, isPointInsideRect(mouseX, mouseY, button4D), true);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return selected;
}

template <typename CameraT, typename RendererT, int N, typename ExtraInputFn>
int runRendererLoop(const char* title, ExtraInputFn&& handleExtraInput) {
    SDL_Window* window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        kRenderWidth,
        kRenderHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS
    );
    if (window == nullptr) {
        return 1;
    }

    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (sdlRenderer == nullptr) {
        SDL_DestroyWindow(window);
        return 1;
    }

    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(100);
    SDL_PumpEvents();
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetWindowMouseGrab(window, SDL_TRUE);

    const RendererT renderer{sdlRenderer, kRenderWidth, kRenderHeight};
    CameraT cam;

    const Mesh<N> cube = Mesh<N>::makeCube();
    const Mesh<N> axes = Mesh<N>::makeAxes();

    bool running = true;
    bool mouseLocked = true;
    bool escapePressed = false;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        const Uint8* keys = SDL_GetKeyboardState(nullptr);

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
            cam.deltaLook(static_cast<float>(mouseX), static_cast<float>(-mouseY));
        }

        if (keys[SDL_SCANCODE_LEFT]) cam.lookLeft();
        if (keys[SDL_SCANCODE_RIGHT]) cam.lookRight();
        if (keys[SDL_SCANCODE_UP]) cam.lookUp();
        if (keys[SDL_SCANCODE_DOWN]) cam.lookDown();

        handleExtraInput(keys, cam);

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
    return 0;
}

int run3DRenderer() {
    return runRendererLoop<Camera3D, Renderer3D, 3>("3D Renderer", [](const Uint8*, Camera3D&) {});
}

int run4DRenderer() {
    return runRendererLoop<Camera4D, Renderer4D, 4>("4D Renderer", [](const Uint8* keys, Camera4D& cam) {
        if (keys[SDL_SCANCODE_Q]) cam.moveAta();
        if (keys[SDL_SCANCODE_E]) cam.moveKata();
        if (keys[SDL_SCANCODE_Z]) cam.lookAna();
        if (keys[SDL_SCANCODE_X]) cam.lookKata();
    });
}


int main(int argc, char* argv[]) {
    SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "0");
    SDL_SetHint(SDL_HINT_VIDEO_WAYLAND_EMULATE_MOUSE_WARP, "1");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }

    const RenderMode mode = runRendererSelectionMenu();

    int exitCode = 0;
    if (mode == RenderMode::Mode3D) {
        exitCode = run3DRenderer();
    }
    else if (mode == RenderMode::Mode4D) {
        exitCode = run4DRenderer();
    }

    SDL_Quit();
    return exitCode;
}
