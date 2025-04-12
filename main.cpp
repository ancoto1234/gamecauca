#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;


// Kích thước khung hình
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

// Tạo cửa sổ
SDL_Window* window = SDL_CreateWindow("FISHING ADVENTURE",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

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

Uint32 startTime;
Uint32 timeLeft = 30;

Mix_Chunk* catchSound = Mix_LoadWAV("point_bonus.wav");

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

// Hàm kiểm tra xem móc kéo đã trúng cá chưa
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

// Cập nhật móc kéo


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
                timeLeft += 2;

                bool check = rand() % 2;
                rod.hookedFish->rect.x = (check)? 0 : SCREEN_WIDTH;
                rod.hookedFish->rect.y = getRandomY();
                rod.hookedFish->moveRight = (check)? true : false;
                rod.hookedFish = nullptr;
            }
        }
    }

}

SDL_Texture* numberTextures[10]; // Mảng lưu texture của số 0-9
int digitWidth = 70;  // Kích thước ảnh số
int digitHeight = 70;


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
// Cập nhật chuyển động của rêu
void updateSeaweed(Seaweed& seaweed) {
    seaweed.frameTime += 1;
    if (seaweed.frameTime > 10) {
        seaweed.frameTime = 0;
        seaweed.frameIndex = (seaweed.frameIndex + 1) % 2;
    }
}
// In rêu
void renderSeaweed(SDL_Renderer* renderer, Seaweed& seaweed) {
    SDL_RenderCopy(renderer, seaweed.frames[seaweed.frameIndex], NULL, &seaweed.rect);
}
// Hàm sinh ngẫu nhiên cá
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

// Main Menu
bool isMenu = true;

void handleMenuEvent(SDL_Event &event) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    SDL_Rect startButtonRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 200, 80};
    SDL_Rect exitButtonRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 100, 200, 80};

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (mouseX >= startButtonRect.x && mouseX <= startButtonRect.x + startButtonRect.w &&
            mouseY >= startButtonRect.y && mouseY <= startButtonRect.y + startButtonRect.h) {
            isMenu = false;  // Bắt đầu game
        }

        if (mouseX >= exitButtonRect.x && mouseX <= exitButtonRect.x + exitButtonRect.w &&
            mouseY >= exitButtonRect.y && mouseY <= exitButtonRect.y + exitButtonRect.h) {
            exit(0);  // Thoát game
        }
    }
}

bool isGameOver = false;

int main(int argc, char* argv[]) {

    // Khởi tạo SDL và SDL2_image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    srand(time(0));

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
    Mix_Volume(-1,MIX_MAX_VOLUME);


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


    // Định nghĩa cá
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


    SDL_Texture* menuBackgroundTexture = loadTexture("menu_background.png", renderer);
    SDL_Texture* startButtonTexture = loadTexture("start_button.png", renderer);
    SDL_Texture* exitButtonTexture = loadTexture("exit_button.png", renderer);

    SDL_Rect backgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Rect startButtonRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 200, 80};
    SDL_Rect exitButtonRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 100, 200, 80};

    SDL_Texture* endBGTexture = loadTexture("endbackground.png", renderer);
    SDL_Rect endgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    // Am thanh
    Mix_Chunk* catchSound = Mix_LoadWAV("point_bonus.wav");
    Mix_Chunk* gameoverSound = Mix_LoadWAV("game_over.wav");
    bool hasPlaySound = false;

    // cập nhật thời gian đếm
    startTime = SDL_GetTicks();


    // Vòng lặp game
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (isMenu) {
                handleMenuEvent(event);
            }

            if (isGameOver) {
                if (event.key.keysym.sym == SDLK_r) {
                    timeLeft = 30;
                    Score = 0;
                    isGameOver = false;
                    hasPlaySound = false;
                }

                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }

            if (event.type == SDL_KEYDOWN) {
                moveShip(ship, event, rod);
                if (event.key.keysym.sym == SDLK_SPACE && !rod.isCasting && !rod.isRetracting) {
                    rod.isCasting = true;

                }
            }
        }

        if (isMenu) {
            SDL_RenderCopy(renderer, menuBackgroundTexture, NULL, &backgroundRect);
            SDL_RenderCopy(renderer, startButtonTexture, NULL, &startButtonRect);
            SDL_RenderCopy(renderer, exitButtonTexture, NULL, &exitButtonRect);



            SDL_RenderPresent(renderer);

        } else if (isGameOver) {
            // Vẽ chữ Game Over
            SDL_RenderCopy(renderer, endBGTexture, NULL, &endgroundRect);

            if (!Mix_Playing(-1) && !hasPlaySound) {
                    Mix_PlayChannel(-1,gameoverSound,0);
                    hasPlaySound = true;
            }

            string scoreStr = to_string(Score);

            for (size_t i = 0; i < scoreStr.size(); i++) {
                int digit = scoreStr[i] - '0';
                SDL_Rect destRect = {600 + i * (170)/2, 335,  150, 150};
                SDL_RenderCopy(renderer, numberTextures[digit], NULL, &destRect);
            }

            SDL_RenderPresent(renderer);

        } else {
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

            // Vẽ đường nối từ tàu đến móc kéo
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Màu đen
            SDL_RenderDrawLine(renderer, ship.x + SIZE_SHIP / 2 - 10, ship.y + SIZE_SHIP / 2, rod.rect.x + 25, rod.rect.y);


            // Vẽ móc
            SDL_RenderCopy(renderer, rodTexture, NULL, &rod.rect);

            // Vẽ con tàu, cho tàu xoay theo hướng di chuyển
            SDL_RenderCopyEx(renderer, shipTexture, NULL, &ship, 0, NULL,
                             facingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

            // vẽ cá
            for (const auto &fish : fishes) {
                SDL_RenderCopyEx(renderer, objectTexture, NULL, &fish.rect, 0, NULL,
                                 fish.moveRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
            }
            // Cau dc ca thi phat ra am thanh
            if (rod.isRetracting) {
                if (rod.hookedFish) {
                    if (!Mix_Playing(-1)) Mix_PlayChannel(-1,catchSound,0);

                }
            }

            string scoreStr = to_string(Score);

            for (size_t i = 0; i < scoreStr.size(); i++) {
                int digit = scoreStr[i] - '0';
                SDL_Rect destRect = {20 + i * (digitWidth+20)/2, 20, digitWidth, digitHeight};
                SDL_RenderCopy(renderer, numberTextures[digit], NULL, &destRect);
            }

            if (SDL_GetTicks() - startTime >= 1000) {
                timeLeft -= 1;
                startTime = SDL_GetTicks();
                if (timeLeft <= 0) {
                    isGameOver = true;
                }
            }
            string timeStr = to_string(timeLeft);
            if (timeLeft < 10) {
                timeStr = '0' + timeStr;
            }
            for (size_t i = 0;i < timeStr.size(); i++) {
                int digit = timeStr[i] - '0';
                SDL_Rect dest = {SCREEN_WIDTH - 140 + i*(digitWidth+20)/2, 20, 70, 70};
                SDL_RenderCopy(renderer, numberTextures[digit], NULL, &dest);
            }
            // Hiển thị buffer
            SDL_RenderPresent(renderer);
            SDL_Delay(16); //Gioi han toc do khung hinh 60FPS
            }
    }

    // Dọn dẹp tài nguyên
    SDL_DestroyTexture(shipTexture);
    SDL_DestroyTexture(groundTexture);
    SDL_DestroyTexture(seaTexture);
    SDL_DestroyTexture(rodTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Giai phong am thanh
    Mix_FreeChunk(catchSound);

    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();

    return 0;

}
