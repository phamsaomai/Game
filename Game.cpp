#include "Game.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>

Game::Game() : window(nullptr), renderer(nullptr), playerTex(nullptr), carTex(nullptr), treeTex(nullptr),
               backgroundTex(nullptr), menuBackgroundTex(nullptr), buttonBgTex(nullptr), font(nullptr),
               largeFont(nullptr), backgroundMusic(nullptr), clickSound(nullptr), collisionSound(nullptr),
               soundEnabled(true), playerRow(0), scrollY(0), generatedRow(0), state(MENU), running(true),
               lastUpdateTime(0) {
    player.rect.w = TILE_SIZE;
    player.rect.h = TILE_SIZE;
    player.rect.x = SCREEN_WIDTH / 2 - TILE_SIZE / 2;
    player.rect.y = SCREEN_HEIGHT / 2;

    textColor = {255, 255, 255, 255};
    hoverColor = {255, 255, 0, 255};
}

Game::~Game() {
    for (auto& button : mainMenuButtons) SDL_DestroyTexture(button.texture);
    for (auto& button : gameOverButtons) SDL_DestroyTexture(button.texture);
    for (auto& button : settingsButtons) SDL_DestroyTexture(button.texture);
    SDL_DestroyTexture(playerTex);
    SDL_DestroyTexture(carTex);
    SDL_DestroyTexture(treeTex);
    SDL_DestroyTexture(backgroundTex);
    SDL_DestroyTexture(menuBackgroundTex);
    SDL_DestroyTexture(buttonBgTex);
    TTF_CloseFont(font);
    TTF_CloseFont(largeFont);
    Mix_FreeMusic(backgroundMusic);
    Mix_FreeChunk(clickSound);
    Mix_FreeChunk(collisionSound);
    Mix_CloseAudio();
    Mix_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << "\n";
        return false;
    }

    window = SDL_CreateWindow("Crossy Road", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
        std::cerr << "SDL_image Init Error: " << IMG_GetError() << "\n";
        return false;
    }

    if (TTF_Init() < 0) {
        std::cerr << "SDL_ttf Init Error: " << TTF_GetError() << "\n";
        return false;
    }

    if (Mix_Init(MIX_INIT_MP3) == 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer Init Error: " << Mix_GetError() << "\n";
        return false;
    }

    playerTex = IMG_LoadTexture(renderer, "player.png");
    carTex = IMG_LoadTexture(renderer, "car.png");
    treeTex = IMG_LoadTexture(renderer, "Tree.png");
    backgroundTex = IMG_LoadTexture(renderer, "back1.jpg");
    menuBackgroundTex = IMG_LoadTexture(renderer, "background_menu.jpg");
    buttonBgTex = IMG_LoadTexture(renderer, "front.png");
    font = TTF_OpenFont("font.ttf", 32);
    largeFont = TTF_OpenFont("font3.ttf", 48);
    backgroundMusic = Mix_LoadMUS("background.mp3");
    clickSound = Mix_LoadWAV("click.mp3");
    collisionSound = Mix_LoadWAV("game-over.mp3");

    if (!playerTex || !carTex || !treeTex || !backgroundTex || !menuBackgroundTex || !font || !largeFont || !clickSound || !collisionSound) {
        std::cerr << "Không thể tải tài nguyên.\n";
        if (!menuBackgroundTex) std::cerr << "Lỗi tải background_menu.jpg: " << IMG_GetError() << "\n";
        return false;
    }

    if (soundEnabled && backgroundMusic) {
        Mix_PlayMusic(backgroundMusic, -1);
    }

    // Khởi tạo nút
    mainMenuButtons = {
        {{200, 180, 240, 60}, "Start", nullptr, false},
        {{200, 280, 240, 60}, "Sound", nullptr, false}
    };

    gameOverButtons = {
        {{200, 130, 240, 60}, "Replay", nullptr, false},
        {{200, 230, 240, 60}, "Sound", nullptr, false},
        {{200, 330, 240, 60}, "Quit", nullptr, false}
    };

    settingsButtons = {
        {{200, 180, 240, 60}, "Sound On", nullptr, false},
        {{200, 280, 240, 60}, "Sound Off", nullptr, false},
        {{200, 380, 240, 60}, "Back", nullptr, false}
    };

    for (auto& button : mainMenuButtons) {
        button.texture = createTextTexture(renderer, font, button.text, textColor);
    }
    for (auto& button : gameOverButtons) {
        button.texture = createTextTexture(renderer, font, button.text, textColor);
    }
    for (auto& button : settingsButtons) {
        button.texture = createTextTexture(renderer, font, button.text, textColor);
    }

    lastUpdateTime = SDL_GetTicks();
    return true;
}

void Game::resetGame() {
    player.rect.x = SCREEN_WIDTH / 2 - TILE_SIZE / 2;
    player.rect.y = SCREEN_HEIGHT / 2;
    playerRow = 0;
    scrollY = 0;
    generatedRow = 0;
    carLanes.clear();
    trees.clear();
}

void Game::handleEvents() {
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        } else if (e.type == SDL_MOUSEMOTION && (state == MENU || state == GAME_OVER || state == SETTINGS)) {
            int mouseX = e.motion.x;
            int mouseY = e.motion.y;
            std::vector<Button>* activeButtons = (state == MENU) ? &mainMenuButtons : (state == GAME_OVER) ? &gameOverButtons : &settingsButtons;
            for (auto& button : *activeButtons) {
                button.hovered = (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                                  mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h);
            }
        } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && (state == MENU || state == GAME_OVER || state == SETTINGS)) {
            if (soundEnabled && clickSound) Mix_PlayChannel(-1, clickSound, 0);
            int mouseX = e.button.x;
            int mouseY = e.button.y;
            if (state == MENU) {
                for (const auto& button : mainMenuButtons) {
                    if (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                        mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h) {
                        if (button.text == "Start") {
                            state = PLAYING;
                        } else if (button.text == "Sound") {
                            state = SETTINGS;
                        }
                    }
                }
            } else if (state == GAME_OVER) {
                for (const auto& button : gameOverButtons) {
                    if (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                        mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h) {
                        if (button.text == "Replay") {
                            resetGame();
                            state = PLAYING;
                        } else if (button.text == "Sound") {
                            state = SETTINGS;
                        } else if (button.text == "Quit") {
                            running = false;
                        }
                    }
                }
            } else if (state == SETTINGS) {
                for (const auto& button : settingsButtons) {
                    if (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                        mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h) {
                        if (button.text == "Sound On") {
                            soundEnabled = true;
                            if (backgroundMusic) Mix_PlayMusic(backgroundMusic, -1);
                        } else if (button.text == "Sound Off") {
                            soundEnabled = false;
                            Mix_HaltMusic();
                        } else if (button.text == "Back") {
                            state = (state == SETTINGS && state != GAME_OVER) ? MENU : GAME_OVER;
                        }
                    }
                }
            }
        } else if (e.type == SDL_KEYDOWN && state == PLAYING) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    playerRow++;
                    scrollY += TILE_SIZE;
                    for (auto& lane : carLanes)
                        for (auto& car : lane.cars) car.rect.y += TILE_SIZE;
                    for (auto& tree : trees) tree.rect.y += TILE_SIZE;
                    break;
                case SDLK_DOWN:
                    if (playerRow > 0) {
                        playerRow--;
                        scrollY -= TILE_SIZE;
                        for (auto& lane : carLanes)
                            for (auto& car : lane.cars) car.rect.y -= TILE_SIZE;
                        for (auto& tree : trees) tree.rect.y -= TILE_SIZE;
                    }
                    break;
                case SDLK_LEFT:
                    player.rect.x -= TILE_SIZE;
                    break;
                case SDLK_RIGHT:
                    player.rect.x += TILE_SIZE;
                    break;
            }
        }
    }
}

void Game::update() {
    if (state == PLAYING) {
        while (generatedRow < playerRow + 12) {
            spawnObstacles(generatedRow, carLanes, trees);
            generatedRow++;
        }

        Uint32 currentTime = SDL_GetTicks();
        for (auto& lane : carLanes) {
            lane.regenerateCars(currentTime);
        }
        for (auto& lane : carLanes)
            for (auto& car : lane.cars)
                car.move();

        for (auto& lane : carLanes) {
            lane.cars.erase(std::remove_if(lane.cars.begin(), lane.cars.end(), [](const Entity& e) {
                return e.rect.x + e.rect.w < 0;
            }), lane.cars.end());
        }
        trees.erase(std::remove_if(trees.begin(), trees.end(), [](const Entity& e) {
            return e.rect.y > SCREEN_HEIGHT;
        }), trees.end());

        if (player.rect.x < 0 || player.rect.x + player.rect.w > SCREEN_WIDTH) {
            std::cout << "Game Over! Out of bounds.\n";
            if (soundEnabled && collisionSound) Mix_PlayChannel(-1, collisionSound, 0);
            state = GAME_OVER;
        }

        for (const auto& lane : carLanes)
            for (const auto& car : lane.cars)
                if (player.checkCollision(car.rect)) {
                    std::cout << "Game Over! Hit by car.\n";
                    if (soundEnabled && collisionSound) Mix_PlayChannel(-1, collisionSound, 0);
                    state = GAME_OVER;
                }

        for (const auto& tree : trees)
            if (player.checkCollision(tree.rect)) {
                std::cout << "Game Over! Hit a tree.\n";
                if (soundEnabled && collisionSound) Mix_PlayChannel(-1, collisionSound, 0);
                state = GAME_OVER;
            }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderClear(renderer);

    if (state == MENU) {
        SDL_Rect menuBgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, menuBackgroundTex, nullptr, &menuBgRect);
        for (const auto& button : mainMenuButtons) {
            int texW, texH;
            SDL_QueryTexture(button.texture, nullptr, nullptr, &texW, &texH);
            SDL_Rect dstRect = {button.rect.x + (button.rect.w - texW) / 2, button.rect.y + (button.rect.h - texH) / 2, texW, texH};
            SDL_SetTextureColorMod(button.texture, button.hovered ? 255 : 255, button.hovered ? 255 : 255, button.hovered ? 0 : 255);
            SDL_RenderCopy(renderer, button.texture, nullptr, &dstRect);
        }
    } else if (state == GAME_OVER) {
        SDL_Rect menuBgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, menuBackgroundTex, nullptr, &menuBgRect);
        for (const auto& button : gameOverButtons) {
            int texW, texH;
            SDL_QueryTexture(button.texture, nullptr, nullptr, &texW, &texH);
            SDL_Rect dstRect = {button.rect.x + (button.rect.w - texW) / 2, button.rect.y + (button.rect.h - texH) / 2, texW, texH};
            SDL_SetTextureColorMod(button.texture, button.hovered ? 255 : 255, button.hovered ? 255 : 255, button.hovered ? 0 : 255);
            SDL_RenderCopy(renderer, button.texture, nullptr, &dstRect);
        }
        SDL_Texture* gameOverText = createTextTexture(renderer, largeFont, "Game Over!", textColor);
        int texW, texH;
        SDL_QueryTexture(gameOverText, nullptr, nullptr, &texW, &texH);
        SDL_Rect textRect = {(SCREEN_WIDTH - texW) / 2, 50, texW, texH};
        SDL_RenderCopy(renderer, gameOverText, nullptr, &textRect);
        SDL_DestroyTexture(gameOverText);
    } else if (state == PLAYING) {
        int bgHeight = SCREEN_HEIGHT;
        int bgY1 = -(scrollY % bgHeight);
        int bgY2 = bgY1 + bgHeight;

        SDL_Rect bgRect1 = {0, bgY1, SCREEN_WIDTH, bgHeight};
        SDL_Rect bgRect2 = {0, bgY2, SCREEN_WIDTH, bgHeight};

        SDL_RenderCopy(renderer, backgroundTex, nullptr, &bgRect1);
        SDL_RenderCopy(renderer, backgroundTex, nullptr, &bgRect2);

        for (const auto& tree : trees)
            SDL_RenderCopy(renderer, treeTex, nullptr, &tree.rect);

        for (const auto& lane : carLanes)
            for (const auto& car : lane.cars)
                SDL_RenderCopy(renderer, carTex, nullptr, &car.rect);

        SDL_RenderCopy(renderer, playerTex, nullptr, &player.rect);
    } else if (state == SETTINGS) {
        SDL_Rect menuBgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, menuBackgroundTex, nullptr, &menuBgRect);
        for (const auto& button : settingsButtons) {
            int texW, texH;
            SDL_QueryTexture(button.texture, nullptr, nullptr, &texW, &texH);
            SDL_Rect dstRect = {button.rect.x + (button.rect.w - texW) / 2, button.rect.y + (button.rect.h - texH) / 2, texW, texH};
            SDL_SetTextureColorMod(button.texture, button.hovered ? 255 : 255, button.hovered ? 255 : 255, button.hovered ? 0 : 255);
            SDL_RenderCopy(renderer, button.texture, nullptr, &dstRect);
        }
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(1000 / 60);
}
