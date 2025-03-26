#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// Kích thước khung hình
const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 600;
// Đường ngang cố định
const int FIXED_Y = SCREEN_HEIGHT / 2 - 180;

// Kích thước con tàu
const int SIZE_SHIP = 120;
// Trạng thái hướng của con tàu
bool facingRight = true;

// Định nghĩa tính chất vật thể
const int OBJECT_SPEED = 3;
const int OBJECT_YMIN = SCREEN_HEIGHT / 2;
const int OBJECT_YMAX = SCREEN_HEIGHT / 2 + 250;

Uint32 lastSpawnTime = 0;
Uint32 nextSpawnDelay = 1000 + rand() % 2000;

Uint32 startTime; // lưu thời gian bắt đầu chương trình

// cấu trúc vật thể di chuyển
struct Fish {
    SDL_Rect rect;
    bool moveRight;
};

vector<Fish> fishes;

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    return texture;
}

void moveShip(SDL_Rect &ship, SDL_Event &event) {
    const int speed = 10;
    switch (event.key.keysym.sym) {
        //case SDLK_UP:
        //    if (rect.y - speed >= 0) rect.y -= speed;
        //    break;
        //case SDLK_DOWN:
        //    if (rect.y + speed <= SCREEN_HEIGHT - RECT_SIZE) rect.y += speed;
        //    break;
        case SDLK_LEFT:
            if (ship.x - speed >= 0) ship.x -= speed;
            facingRight = false;
            break;
        case SDLK_RIGHT:
            if (ship.x + speed <= SCREEN_WIDTH - ship.w) ship.x += speed;
            facingRight = true;
            break;
    }
}

// Hàm sinh ngẫu nhiên vị trí của vật thể trong khoảng cho phép
int getRandomY() {
    return rand() % (OBJECT_YMAX - OBJECT_YMIN + 1) + OBJECT_YMIN;
}

void spawnFish() {
    Fish fish;
    fish.rect.w = 50;
    fish.rect.h = 50;
    fish.rect.y = getRandomY();

    if (rand() % 2 == 0) {
        fish.rect.x = 0;
        fish.moveRight = true;
    }
    else {
        fish.rect.x = SCREEN_WIDTH - fish.rect.w;
        fish.moveRight = false;
    }

    fishes.push_back(fish);

    lastSpawnTime = SDL_GetTicks();
    nextSpawnDelay = 1000 + rand() % 2000;
}

void updateFishes() {
    for (size_t i = 0;i < fishes.size();i++) {
        if (fishes[i].moveRight) {
            fishes[i].rect.x += OBJECT_SPEED;
        }
        else {
            fishes[i].rect.x -= OBJECT_SPEED;
        }
        if (fishes[i].rect.x < -fishes[i].rect.w || fishes[i].rect.x > SCREEN_WIDTH) {
            fishes.erase(fishes.begin() + i);
        }
    }
}

void updateFishSpawn() {
    if (SDL_GetTicks() - lastSpawnTime >= nextSpawnDelay) {
        spawnFish();
    }
}


int main(int argc, char* argv[]) {

    // Khởi tạo SDL và SDL2_image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    srand(time(0));

    // Tạo cửa sổ
    SDL_Window* window = SDL_CreateWindow("Test",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Định nghĩa tàu
    SDL_Texture* shipTexture = loadTexture("ship.png", renderer);

    // Định nghĩa vật thể
    SDL_Texture* objectTexture = loadTexture("objects.png", renderer);

    SDL_Rect ship;
    ship.w = SIZE_SHIP;
    ship.h = SIZE_SHIP;
    ship.x = SCREEN_WIDTH / 2 - ship.w / 2;
    ship.y = FIXED_Y;

    // Vòng lặp game
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                moveShip(ship, event);
            }
        }

        updateFishSpawn();
        updateFishes();

        // Vẽ màn hình
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Vẽ con tàu, cho tàu xoay theo hướng di chuyển
        SDL_RenderCopyEx(renderer, shipTexture, NULL, &ship, 0, NULL,
                         facingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
        // vẽ cá
        for (const auto &fish : fishes) {
            SDL_RenderCopyEx(renderer, objectTexture, NULL, &fish.rect, 0, NULL,
                             fish.moveRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
        }

        // Hiển thị buffer
        SDL_RenderPresent(renderer);
        SDL_Delay(16); //Gioi han toc do khung hinh 60FPS
    }

    // Dọn dẹp tài nguyên
    SDL_DestroyTexture(shipTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;

}
