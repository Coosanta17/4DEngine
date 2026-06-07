#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include "../../include/AppWindow.hpp"

#include "../../include/Camera4D.hpp"
#include "../../include/Camera3D.hpp"
#include "../../include/mesh.hpp"
#include "../../include/renderer.hpp"

constexpr int kRenderWidth = 800;
constexpr int kRenderHeight = 600;

enum class RenderMode {
    None,
    Mode3D,
    Mode4D,
};

bool isPointInsideRect(const float x, const float y, const SDL_FRect& rect) {
    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
}

void drawWireDigit3(SDL_Renderer* renderer, const SDL_FRect& area) {
    const float l = area.x;
    const float r = area.x + area.w;
    const float t = area.y;
    const float b = area.y + area.h;
    const float m = area.y + area.h / 2;

    SDL_RenderLine(renderer, l, t, r, t);
    SDL_RenderLine(renderer, r, t, r, b);
    SDL_RenderLine(renderer, l, m, r, m);
    SDL_RenderLine(renderer, l, b, r, b);
}

void drawWireDigit4(SDL_Renderer* renderer, const SDL_FRect& area) {
    const float l = area.x;
    const float r = area.x + area.w;
    const float t = area.y;
    const float b = area.y + area.h;
    const float m = area.y + area.h / 2;

    SDL_RenderLine(renderer, l, t, l, m);
    SDL_RenderLine(renderer, l, m, r, m);
    SDL_RenderLine(renderer, r, t, r, b);
}

void drawWireLetterD(SDL_Renderer* renderer, const SDL_FRect& area) {
    const float l = area.x;
    const float r = area.x + area.w;
    const float t = area.y;
    const float b = area.y + area.h;

    SDL_RenderLine(renderer, l, t, l, b);
    SDL_RenderLine(renderer, l, t, r - 2, t + 2);
    SDL_RenderLine(renderer, r - 2, t + 2, r, b / 2 + t / 2);
    SDL_RenderLine(renderer, r, b / 2 + t / 2, r - 2, b - 2);
    SDL_RenderLine(renderer, r - 2, b - 2, l, b);
}

void drawMenuButton(SDL_Renderer* renderer, const SDL_FRect& button, const bool hovered, const bool is4DButton) {
    if (is4DButton) {
        SDL_SetRenderDrawColor(renderer, hovered ? 120 : 90, hovered ? 130 : 90, hovered ? 220 : 180, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, hovered ? 90 : 70, hovered ? 170 : 130, hovered ? 210 : 180, 255);
    }
    SDL_RenderFillRect(renderer, &button);

    SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
    SDL_RenderRect(renderer, &button);

    const SDL_FRect firstGlyph{button.x + button.w / 4 - 18, button.y + button.h / 2 - 18, 26, 36};
    const SDL_FRect secondGlyph{button.x + button.w / 4 + 14, button.y + button.h / 2 - 18, 26, 36};

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
        menuWidth,
        menuHeight,
        SDL_WINDOW_VULKAN //| SDL_WINDOW_RESIZABLE
    );
    if (window == nullptr) {
        return RenderMode::None;
    }

    SDL_PumpEvents();

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        return RenderMode::None;
    }

    auto selected = RenderMode::None;
    bool running = true;

    while (running) {
        constexpr SDL_FRect button4D{230, 60, 160, 100};
        constexpr SDL_FRect button3D{30, 60, 160, 100};
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
                const float mouseX = event.button.x;
                const float mouseY = event.button.y;

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

        float mouseX = 0;
        float mouseY = 0;
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

Mesh4D makeTesseract(const float size = 1.0f) {
    Mesh4D m;
    for (int i = 0; i < 16; ++i) {
        m.addVertex(Vec4{
            (i & 1) ? size : -size,
            (i & 2) ? size : -size,
            (i & 4) ? size : -size,
            (i & 8) ? size : -size
        });
    }

    // 24 faces
    for (int dim1 = 0; dim1 < 3; ++dim1) {
        for (int dim2 = dim1 + 1; dim2 < 4; ++dim2) {
            int other1 = -1, other2 = -1;
            for (int k = 0; k < 4; ++k) {
                if (k != dim1 && k != dim2) {
                    if (other1 == -1) other1 = k;
                    else other2 = k;
                }
            }

            for (int k = 0; k < 4; ++k) {
                const int bit1 = (k & 1) ? 1 : 0;
                const int bit2 = (k & 2) ? 1 : 0;

                const int baseMask = (bit1 << other1) | (bit2 << other2);

                const int v00 = baseMask;
                const int v10 = baseMask | (1 << dim1);
                const int v01 = baseMask | (1 << dim2);
                const int v11 = baseMask | (1 << dim1) | (1 << dim2);

                m.addTriangle(v00, v10, v01);
                m.addTriangle(v10, v11, v01);
            }
        }
    }
    return m;
}

template <typename CameraT, typename RendererT, int N, typename ExtraInputFn>
int runRendererLoop(const char* title, ExtraInputFn&& handleExtraInput) {
    AppWindow window(title, kRenderWidth, kRenderHeight);
    if (!window.isValid()) return 1;

    window.present();
    SDL_Delay(100);
    SDL_PumpEvents();
    window.setRelativeMouseMode(true);
    window.setMouseGrab(true);

    CameraT cam;

    bool running = true;
    bool mouseLocked = true;
    bool escapePressed = false;

    if constexpr (N == 3) {
        const Renderer3D renderer = window.createRenderer3D();
        const Mesh<3> cube = Mesh<3>::makeCube();
        const Mesh<3> axes = Mesh<3>::makeAxes();

        while (running) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_EVENT_QUIT) running = false;
            }

            const bool* keys = SDL_GetKeyboardState(nullptr);
            if (keys[SDL_SCANCODE_ESCAPE]) {
                if (!escapePressed) {
                    mouseLocked = !mouseLocked;
                    window.setRelativeMouseMode(mouseLocked);
                    window.setMouseGrab(mouseLocked);
                    escapePressed = true;
                }
            }
            else { escapePressed = false; }

            if (keys[SDL_SCANCODE_W]) cam.moveForward();
            if (keys[SDL_SCANCODE_S]) cam.moveBackward();
            if (keys[SDL_SCANCODE_A]) cam.moveLeft();
            if (keys[SDL_SCANCODE_D]) cam.moveRight();
            if (keys[SDL_SCANCODE_SPACE]) cam.moveUp();
            if (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT]) cam.moveDown();

            float mouseX = 0, mouseY = 0;
            if (mouseLocked) {
                SDL_GetRelativeMouseState(&mouseX, &mouseY);
                cam.deltaLook(static_cast<float>(mouseX), static_cast<float>(-mouseY));
            }

            if (keys[SDL_SCANCODE_LEFT]) cam.lookLeft();
            if (keys[SDL_SCANCODE_RIGHT]) cam.lookRight();
            if (keys[SDL_SCANCODE_UP]) cam.lookUp();
            if (keys[SDL_SCANCODE_DOWN]) cam.lookDown();

            handleExtraInput(keys, cam);

            window.clear(0, 0, 0, 255);
            SDL_SetRenderDrawColor(window.getSDLRenderer(), 255, 255, 255, 255);
            renderer.drawMesh(cube, cam);
            SDL_SetRenderDrawColor(window.getSDLRenderer(), 255, 0, 0, 128);
            renderer.drawMesh(axes, cam);

            window.present();
        }
    }
    else {
        const Renderer4D renderer = window.createRenderer4D();
        const Mesh4D tesseract = makeTesseract(1.0f);
        const Mesh<4> axes = Mesh<4>::makeAxes();

        while (running) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_EVENT_QUIT) running = false;
            }

            const bool* keys = SDL_GetKeyboardState(nullptr);
            if (keys[SDL_SCANCODE_ESCAPE]) {
                if (!escapePressed) {
                    mouseLocked = !mouseLocked;
                    window.setRelativeMouseMode(mouseLocked);
                    window.setMouseGrab(mouseLocked);
                    escapePressed = true;
                }
            }
            else { escapePressed = false; }

            if (keys[SDL_SCANCODE_W]) cam.moveForward();
            if (keys[SDL_SCANCODE_S]) cam.moveBackward();
            if (keys[SDL_SCANCODE_A]) cam.moveLeft();
            if (keys[SDL_SCANCODE_D]) cam.moveRight();
            if (keys[SDL_SCANCODE_SPACE]) cam.moveUp();
            if (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT]) cam.moveDown();

            float mouseX = 0, mouseY = 0;
            if (mouseLocked) {
                SDL_GetRelativeMouseState(&mouseX, &mouseY);
                cam.deltaLook(static_cast<float>(mouseX), static_cast<float>(-mouseY));
            }

            if (keys[SDL_SCANCODE_LEFT]) cam.lookLeft();
            if (keys[SDL_SCANCODE_RIGHT]) cam.lookRight();
            if (keys[SDL_SCANCODE_UP]) cam.lookUp();
            if (keys[SDL_SCANCODE_DOWN]) cam.lookDown();

            handleExtraInput(keys, cam);

            window.clear(0, 0, 0, 255);
            SDL_SetRenderDrawColor(window.getSDLRenderer(), 255, 255, 255, 255);
            renderer.drawMesh(tesseract, cam);
            SDL_SetRenderDrawColor(window.getSDLRenderer(), 255, 0, 0, 128);
            renderer.drawMesh(axes, cam);

            window.present();
        }
    }

    return 0;
}

int run3DRenderer() {
    return runRendererLoop<Camera3D, Renderer3D, 3>("3D Renderer", [](const bool*, Camera3D&) {});
}

int run4DRenderer() {
    return runRendererLoop<Camera4D, Renderer4D, 4>("4D Renderer", [](const bool* keys, Camera4D& cam) {
        if (keys[SDL_SCANCODE_Q]) cam.moveAta();
        if (keys[SDL_SCANCODE_E]) cam.moveKata();
        if (keys[SDL_SCANCODE_Z]) cam.lookAna();
        if (keys[SDL_SCANCODE_X]) cam.lookKata();
    });
}


int main(int argc, char* argv[]) {
    //std::cout << SDL_GetCurrentVideoDriver() << std::endl;

    SDL_SetHint(SDL_HINT_MOUSE_EMULATE_WARP_WITH_RELATIVE, "1");
    // SDL_SetHint(SDL_HINT_RENDER_DRIVER, "vulkan");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
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
