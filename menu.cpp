#include "menu.h"
#include "constants.h"
#include <iostream>


bool showMenu(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_Texture* menuBackground = IMG_LoadTexture(renderer, "menu_background.png");
    SDL_Texture* playTex = IMG_LoadTexture(renderer, "play_button.png");
    SDL_Texture* exitTex = IMG_LoadTexture(renderer, "exit_button.png");

    if (!menuBackground || !playTex || !exitTex) {
        std::cerr << "Không thể tải hình ảnh menu.\n";
        return false;
    }

    int currentSelection = 0;
    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }

            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        if (currentSelection > 0) currentSelection--;
                        break;
                    case SDLK_DOWN:
                        if (currentSelection < 1) currentSelection++;
                        break;
                    case SDLK_RETURN:
                        if (currentSelection == 0) {
                            return true;
                        } else if (currentSelection == 1) {
                            running = false;
                        }
                        break;
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, menuBackground, nullptr, nullptr);

        SDL_Rect playRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 50 };
        if (currentSelection == 0) {
            SDL_SetTextureColorMod(playTex, 255, 255, 0);
        } else {
            SDL_SetTextureColorMod(playTex, 255, 255, 255);
        }
        SDL_RenderCopy(renderer, playTex, nullptr, &playRect);

        SDL_Rect exitRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 10, 200, 50 };
        if (currentSelection == 1) {
            SDL_SetTextureColorMod(exitTex, 255, 255, 0);
        } else {
            SDL_SetTextureColorMod(exitTex, 255, 255, 255);
        }
        SDL_RenderCopy(renderer, exitTex, nullptr, &exitRect);

        SDL_RenderPresent(renderer);

        SDL_Delay(1000 / 60);
    }

    SDL_DestroyTexture(menuBackground);
    SDL_DestroyTexture(playTex);
    SDL_DestroyTexture(exitTex);

    return false;
}
