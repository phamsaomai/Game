#include "menu.h"
#include "constants.h"
#include <iostream>


bool showMenu(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_Texture* menuBackground = IMG_LoadTexture(renderer, "menu_background.png");

    if (!menuBackground) {
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

        SDL_RenderPresent(renderer);

        SDL_Delay(1000 / 60);
    }

    SDL_DestroyTexture(menuBackground);

    return false;
}
