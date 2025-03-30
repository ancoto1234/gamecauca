#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;  // Thêm dòng này


// Kích thước khung hình
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
// Đường ngang cố định
int FIXED_Y = SCREEN_HEIGHT / 2 - 330;

// Kích thước con tàu
const int SIZE_SHIP = 250;
// Trạng thái hướng của con tàu
bool facingRight = true;

// Định nghĩa tính chất vật thể
const int OBJECT_SPEED = 2;
const int OBJECT_YMIN = SCREEN_HEIGHT / 2;
const int OBJECT_YMAX = SCREEN_HEIGHT / 2 + 250;

Uint32 lastSpawnTime = 0;
Uint32 nextSpawnDelay = 1000 + rand() % 2000;

// cấu trúc vật thể di chuyển
struct Fish {
    SDL_Rect rect;
    bool moveRight;
};

// Tính điểm
int Score = 0;

vector<Fish> fishes;

// Cấu trúc rêu
struct Seaweed {
    SDL_Rect rect;
    SDL_Texture* frames[2];
    int frameIndex = 0;
    int frameTime = 0;
};

// Cấu trúc móc kéo
struct Rod{
    SDL_Rect rect;
    int length = 0;
    bool isCasting;
    bool isRetracting;
    Fish* hookedFish = nullptr; // con cá bị móc vào
};

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return (a.x < b.x + b.w && a.x + a.w > b.x
            && a.y < b.y + b.h && a.y + a.h > b.y);
}

// Hàm sinh ngẫu nhiên vị trí của vật thể trong khoảng cho phép
int getRandomY() {
    return rand() % (OBJECT_YMAX - OBJECT_YMIN + 1) + OBJECT_YMIN;
}

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}

void updateRod(Rod &rod) {
    if (rod.isCasting) {
        if (rod.length < 460) {
            rod.rect.y += 6;
            rod.length += 6;

            // Kiểm tra va trạm với cá
            for (auto &fish : fishes) {
                if (checkCollision({rod.rect.x + 15, rod.rect.y, 10, 10}, fish.rect)) {
                    rod.hookedFish = &fish;
                    rod.isCasting = false;
                    rod.isRetracting = true;
                    break;
                }
            }
        } else {
            rod.isCasting = false;
            rod.isRetracting = true;
        }
    }
    else if (rod.isRetracting) {
        if (rod.length > 1) {
            rod.rect.y -= 9;
            rod.length -= 9;

            if (rod.hookedFish) {
                rod.hookedFish->rect.y = rod.rect.y;
                rod.hookedFish->rect.x = rod.rect.x;
            }
        } else {
            rod.isRetracting = false;

            if (rod.hookedFish) {
                Score += 10;
                rod.hookedFish->rect.x = (rand() & 2)? 0 : SCREEN_WIDTH;
                rod.hookedFish->rect.y = getRandomY();
                rod.hookedFish->moveRight = rand() % 2;
                rod.hookedFish = nullptr;
            }
        }
    }

}

SDL_Texture* numberTextures[10]; // Mảng lưu texture của số 0-9
int digitWidth = 100;  // Kích thước ảnh số
int digitHeight = 100;


void moveShip(SDL_Rect &ship, SDL_Event &event, Rod &rod) {
    if (rod.isCasting || rod.isRetracting) return;
    const int speed = 10;
    switch (event.key.keysym.sym) {
        case SDLK_LEFT:
            if (ship.x - speed >= 0) {
                ship.x -= speed;
                rod.rect.x -= speed;
            }
            facingRight = false;
            break;
        case SDLK_RIGHT:
            if (ship.x + speed <= SCREEN_WIDTH - ship.w) {
                ship.x += speed;
                rod.rect.x += speed;
            }
            facingRight = true;
            break;
    }
}





// Hàm chuyển động của rêu
void loadSeaweedTextures1(SDL_Renderer* renderer, Seaweed& seaweed) {
    seaweed.frames[0] = IMG_LoadTexture(renderer, "seaweed/1.png");
    seaweed.frames[1] = IMG_LoadTexture(renderer, "seaweed/2.png");
}

void loadSeaweedTextures2(SDL_Renderer* renderer, Seaweed& seaweed) {
    seaweed.frames[0] = IMG_LoadTexture(renderer, "seaweed2/1.png");
    seaweed.frames[1] = IMG_LoadTexture(renderer, "seaweed2/2.png");
}

void loadSeaweedTextures3(SDL_Renderer* renderer, Seaweed& seaweed) {
    seaweed.frames[0] = IMG_LoadTexture(renderer, "seaweed3/1.png");
    seaweed.frames[1] = IMG_LoadTexture(renderer, "seaweed3/2.png");
}

void updateSeaweed(Seaweed& seaweed) {
    seaweed.frameTime += 1;
    if (seaweed.frameTime > 10) {
        seaweed.frameTime = 0;
        seaweed.frameIndex = (seaweed.frameIndex + 1) % 2;
    }
}

void renderSeaweed(SDL_Renderer* renderer, Seaweed& seaweed) {
    SDL_RenderCopy(renderer, seaweed.frames[seaweed.frameIndex], NULL, &seaweed.rect);
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
    SDL_Rect ship;
    ship.w = SIZE_SHIP;
    ship.h = SIZE_SHIP;
    ship.x = SCREEN_WIDTH / 2 - ship.w / 2;
    ship.y = FIXED_Y;

    // Đinh nghĩa móc kéo
    SDL_Texture* rodTexture = loadTexture("moc.png", renderer);
    Rod rod;
    rod.rect.y = FIXED_Y + 165;
    rod.rect.x = SCREEN_WIDTH / 2 - ship.w / 2 + 90;
    rod.rect.h = 50;
    rod.rect.w = 50;


    // Định nghĩa vật thể
    SDL_Texture* objectTexture = loadTexture("objects.png", renderer);

    // Định nghĩa nền cát
    SDL_Texture* groundTexture = loadTexture("background.png", renderer);
    SDL_Rect groundRect;

    // Định nghĩa nước
    SDL_Texture* seaTexture = loadTexture("sea.png", renderer);
    SDL_Rect seaRect;

    // Định nghĩa đá
    SDL_Texture* Stone1 = loadTexture("stone1.png", renderer);
    SDL_Texture* Stone2 = loadTexture("stone2.png", renderer);

    SDL_Rect Stone2Rect = {300,500,140,140};
    SDL_Rect Stone1Rect = {350,510,140,140};

    // Tải hình ảnh điểm số
    for (int i = 0; i < 10; i++) {
        string path = "numbers/" + to_string(i) + ".png"; // Ví dụ: "numbers/0.png"
        numberTextures[i] = loadTexture(path.c_str(), renderer);
    }

    // Định nghĩa rêu
    Seaweed seaweed1;
    seaweed1.rect = {330, 390, 140, 140};
    loadSeaweedTextures1(renderer, seaweed1);

    Seaweed seaweed2;
    seaweed2.rect = {888, 380, 140, 140};
    loadSeaweedTextures2(renderer, seaweed2);

    Seaweed seaweed3;
    seaweed3.rect = {370, 390, 140, 140};
    loadSeaweedTextures3(renderer, seaweed3);


    // Vòng lặp game
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                moveShip(ship, event, rod);
                if (event.key.keysym.sym == SDLK_SPACE && !rod.isCasting && !rod.isRetracting) {
                    rod.isCasting = true;
                }
            }
        }



        updateFishSpawn();
        updateFishes();
        updateSeaweed(seaweed1);
        updateSeaweed(seaweed2);
        updateSeaweed(seaweed3);
        updateRod(rod);

        // Vẽ màn hình
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderClear(renderer);

        //Vẽ nước
        SDL_RenderCopy(renderer, seaTexture, NULL, NULL);

        //Vẽ rêu
        renderSeaweed(renderer, seaweed1);
        renderSeaweed(renderer, seaweed2);
        renderSeaweed(renderer, seaweed3);

        //Vẽ nền cát
        SDL_RenderCopy(renderer, groundTexture, NULL, NULL);

        // Vẽ đá
        SDL_RenderCopy(renderer, Stone2, NULL, &Stone2Rect);
        SDL_RenderCopy(renderer, Stone1, NULL, &Stone1Rect);

        // Vẽ móc
        SDL_RenderCopy(renderer, rodTexture, NULL, &rod.rect);

        cout << Score << endl;
        // Vẽ con tàu, cho tàu xoay theo hướng di chuyển
        SDL_RenderCopyEx(renderer, shipTexture, NULL, &ship, 0, NULL,
                         facingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

        // vẽ cá
        for (const auto &fish : fishes) {
            SDL_RenderCopyEx(renderer, objectTexture, NULL, &fish.rect, 0, NULL,
                             fish.moveRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
        }

        string scoreStr = to_string(Score);

        for (size_t i = 0; i < scoreStr.size(); i++) {
            int digit = scoreStr[i] - '0';
            SDL_Rect destRect = {50 + i * (digitWidth+20)/2, 50, digitWidth, digitHeight};
            SDL_RenderCopy(renderer, numberTextures[digit], NULL, &destRect);
        }

        // Hiển thị buffer
        SDL_RenderPresent(renderer);
        SDL_Delay(16); //Gioi han toc do khung hinh 60FPS
    }

    // Dọn dẹp tài nguyên
    SDL_DestroyTexture(shipTexture);
    SDL_DestroyTexture(groundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;

}
