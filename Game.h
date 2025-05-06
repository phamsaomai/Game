#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include "Entity.h"
#include "CarLane.h"
#include "Button.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TILE_SIZE = 40;

enum GameState { MENU, PLAYING, GAME_OVER, SETTINGS };

struct Game {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* playerTex;
    SDL_Texture* carTex;
    SDL_Texture* treeTex;
    SDL_Texture* backgroundTex;
    SDL_Texture* menuBackgroundTex;
    SDL_Texture* buttonBgTex;
    TTF_Font* font;
    TTF_Font* largeFont;
    Mix_Music* backgroundMusic;
    Mix_Chunk* clickSound;
    Mix_Chunk* collisionSound;
    bool soundEnabled;
    Entity player;
    std::vector<CarLane> carLanes;
    std::vector<Entity> trees;
    int playerRow;
    int scrollY;
    int generatedRow;
    GameState state;
    bool running;
    SDL_Event e;
    std::vector<Button> mainMenuButtons;
    std::vector<Button> gameOverButtons;
    std::vector<Button> settingsButtons;
    Uint32 lastUpdateTime;
    SDL_Color textColor;
    SDL_Color hoverColor;

    Game();
    ~Game();
    bool init();
    void resetGame();
    void handleEvents();
    void update();
    void render();
};

#endif
