#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <iostream>

constexpr int WIDTH = 900;
constexpr int HEIGHT = 700;

struct Vec4 {
    double x, y, z, w;
};

struct Vec3 {
    double x, y, z;
};

struct Line4D {
    Vec4 a, b;
};

std::vector<Line4D> lines;

Vec4 camPos{0, 0, -5, 0};

double rotXY = 0;
double rotXZ = 0;
double rotXW = 0;

double speed = 0.05;
double rotSpeed = 0.02;

// Plane rotations
// because in 4D you rotate along planes instead of axes, with a minimum of
// 3 planes of rotation to access every point in the hypersphere

void rotateXY(Vec4& v, const double a) {
    const double x = v.x * cos(a) - v.y * sin(a);
    const double y = v.x * sin(a) + v.y * cos(a);
    v.x = x;
    v.y = y;
}

void rotateXZ(Vec4& v, const double a) {
    const double x = v.x * cos(a) - v.z * sin(a);
    const double z = v.x * sin(a) + v.z * cos(a);
    v.x = x;
    v.z = z;
}

void rotateXW(Vec4& v, const double a) {
    const double x = v.x * cos(a) - v.w * sin(a);
    const double w = v.x * sin(a) + v.w * cos(a);
    v.x = x;
    v.w = w;
}

Vec3 project4Dto3D(const Vec4& v) {
    constexpr double d = 4.0;
    const double factor = d / (d - v.w);
    return {v.x * factor, v.y * factor, v.z * factor};
}

SDL_Point project3Dto2D(const Vec3& v) {
    constexpr double d = 4.0;
    const double factor = d / (d - v.z);

    const int x = static_cast<int>(v.x * factor * 120 + WIDTH / 2);
    const int y = static_cast<int>(-v.y * factor * 120 + HEIGHT / 2);

    return {x, y};
}

Vec4 transform(Vec4 v) {
    v.x -= camPos.x;
    v.y -= camPos.y;
    v.z -= camPos.z;
    v.w -= camPos.w;

    rotateXY(v, rotXY);
    rotateXZ(v, rotXZ);
    rotateXW(v, rotXW);

    return v;
}

void addTesseract() {
    std::vector<Vec4> pts;

    for (int i = 0; i < 16; i++) {
        pts.push_back(Vec4{
            (i & 1) ? 1.0 : -1.0,
            (i & 2) ? 1.0 : -1.0,
            (i & 4) ? 1.0 : -1.0,
            (i & 8) ? 1.0 : -1.0
        });
    }

    for (int i = 0; i < 16; i++) {
        for (int j = i + 1; j < 16; j++) {
            int diff = 0;
            for (int b = 0; b < 4; b++)
                if (((i >> b) & 1) != ((j >> b) & 1)) diff++;

            if (diff == 1)
                lines.push_back({pts[i], pts[j]});
        }
    }
}

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window =
        SDL_CreateWindow("4D Renderer",
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         WIDTH, HEIGHT, 0);

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    addTesseract();

    bool running = true;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT) running = false;

        const Uint8* keys = SDL_GetKeyboardState(nullptr);

        // Movement

        if (keys[SDL_SCANCODE_W]) camPos.z += speed;
        if (keys[SDL_SCANCODE_S]) camPos.z -= speed;
        if (keys[SDL_SCANCODE_A]) camPos.x -= speed;
        if (keys[SDL_SCANCODE_D]) camPos.x += speed;
        if (keys[SDL_SCANCODE_SPACE]) camPos.y += speed;
        if (keys[SDL_SCANCODE_LSHIFT]) camPos.y -= speed;
        if (keys[SDL_SCANCODE_Q]) camPos.w += speed;
        if (keys[SDL_SCANCODE_E]) camPos.w -= speed;

        // Rotations

        if (keys[SDL_SCANCODE_LEFT]) rotXY += rotSpeed;
        if (keys[SDL_SCANCODE_RIGHT]) rotXY -= rotSpeed;
        if (keys[SDL_SCANCODE_UP]) rotXZ += rotSpeed;
        if (keys[SDL_SCANCODE_DOWN]) rotXZ -= rotSpeed;
        if (keys[SDL_SCANCODE_Z]) rotXW += rotSpeed;
        if (keys[SDL_SCANCODE_X]) rotXW -= rotSpeed;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for (auto& L : lines) {
            Vec4 a = transform(L.a);
            Vec4 b = transform(L.b);

            Vec3 a3 = project4Dto3D(a);
            Vec3 b3 = project4Dto3D(b);

            SDL_Point p1 = project3Dto2D(a3);
            SDL_Point p2 = project3Dto2D(b3);

            SDL_RenderDrawLine(renderer,
                               p1.x, p1.y,
                               p2.x, p2.y);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_Quit();
    return 0;
}
