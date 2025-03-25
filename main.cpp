#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// Kich thuoc khung hinh
const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 600;

// Duong ngang co dinh
const int FIXED_Y = SCREEN_HEIGHT / 2 - 180;

// Kich thuoc con tau
const int SIZE_SHIP = 120;

// Trang thai huong con tau
bool facingRight = true;

// Dinh nghia vat the
const int OBJECT_SPEED = 3;
const int OBJECT_Y = SCREEN_HEIGHT / 2;

//Cau truc vat the di chuyen
struct MovingObject {
    SDL_Rect rect;
    int speed;
    bool moveRight;
};

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

void updateObject(vector<MovingObject> &objects) {
    for (auto &obj : objects) {
        if (obj.moveRight) {
            obj.rect.x += obj.speed;
            if (obj.rect.x > SCREEN_WIDTH) obj.rect.x = -obj.rect.w;
        }

        else {
            obj.rect.x -= obj.speed;
            if (obj.rect.x + obj.rect.w < 0) obj.rect.x = SCREEN_WIDTH;
        }
    }
}

int main(int argc, char* argv[]) {

    //Khoi tao SDL va SDL2_image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    srand(time(0));

    //Tao cua so
    SDL_Window* window = SDL_CreateWindow("Test",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //Dinh nghia tau
    SDL_Texture* shipTexture = loadTexture("ship.png", renderer);

    //Dinh nghia vat the
    SDL_Texture* objectTexture = loadTexture("objects.png", renderer);

    SDL_Rect ship;
    ship.w = SIZE_SHIP;
    ship.h = SIZE_SHIP;
    ship.x = SCREEN_WIDTH / 2 - ship.w / 2;
    ship.y = FIXED_Y;

    vector<MovingObject> objects;
    for (int i = 0;i < 2;i++) {
        MovingObject obj;
        obj.rect = {rand() % SCREEN_WIDTH, OBJECT_Y, 50, 50};
        obj.speed = OBJECT_SPEED;
        obj.moveRight = rand() % 2;
        objects.push_back(obj);
    }

    //Vong lap game
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

        updateObject(objects);

        //Ve man hinh
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderClear(renderer);

        //Xoay tau dua theo huong di
        SDL_RenderCopyEx(renderer, shipTexture, NULL, &ship, 0, NULL,
                         facingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
        //Ve vat the
        for (const auto &obj : objects) {
            SDL_RenderCopy(renderer, objectTexture, NULL, &obj.rect);
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
