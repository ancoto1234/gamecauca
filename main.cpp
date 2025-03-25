#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 600;

// Duong ngang co dinh
const int FIXED_Y = SCREEN_HEIGHT / 2 - 180;

// Trang thai huong con tau
bool facingRight = true;

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

int main(int argc, char* argv[]) {

    //Khoi tao SDL va SDL2_image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    //Tao cua so
    SDL_Window* window = SDL_CreateWindow("Test",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //Dinh nghia tau
    SDL_Texture* shipTexture = loadTexture("ship.png", renderer);

    SDL_Rect ship;
    ship.w = 100;
    ship.h = 100;
    ship.x = SCREEN_WIDTH / 2 - ship.w / 2;
    ship.y = FIXED_Y;

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

        //Ve man hinh
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        //Ve tau
        SDL_RenderCopy(renderer, shipTexture, NULL, &ship);

        //Xoay tau dua theo huong di
        SDL_RenderCopyEx(renderer, shipTexture, NULL, &ship, 0, NULL,
                         facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);

        // Hiển thị buffer
        SDL_RenderPresent(renderer);
        // SDL_Delay(1); //Gioi han toc do khung hinh 60FPS
    }

    // Dọn dẹp tài nguyên
    SDL_DestroyTexture(shipTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;

}
