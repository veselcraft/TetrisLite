#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <pspkernel.h>

#include "menu.cpp"
#include "game.cpp"

void drawText(SDL_Renderer *renderer, char *text, int x, int y, int size, SDL_Texture **texture, SDL_Rect *rect)
{
    TTF_Font* font = TTF_OpenFont("font.ttf", size);
    if (font == NULL) {
        SDL_Log("Unable to initialize font: %s\n", TTF_GetError());
    }
    SDL_Color color = {50,50,50};
    SDL_Surface* surfaceMessage = TTF_RenderUTF8_Solid(font, text, color);
    *texture = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    rect->x = 0;
    rect->y = 0;
    rect->w = surfaceMessage->w;
    rect->h = surfaceMessage->h;
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);

    Mix_OpenAudio(44100, 
        MIX_DEFAULT_FORMAT, 
        MIX_DEFAULT_CHANNELS, 
        2048
    );

    TTF_Init();
    
    SDL_Window * window = SDL_CreateWindow(
        "window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        480,
        272,
        0
    );

    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Rect square = {216, 96, 34, 64}; 

    int running = 1;
    SDL_Event event;
    
    // Init Audio

    Mix_Chunk *snd_menuSelect, *snd_pieceRotate, *snd_pieceLanded;
    Mix_Chunk *snd_linesClear1, *snd_linesClear2, *snd_linesClear3, *snd_linesClear4;
    snd_menuSelect = Mix_LoadWAV("sounds/menu_select.wav");
    snd_pieceRotate = Mix_LoadWAV("sounds/rotate.wav");
    snd_pieceLanded = Mix_LoadWAV("sounds/landed.wav");
    snd_linesClear1 = Mix_LoadWAV("sounds/clear1.wav");
    snd_linesClear2 = Mix_LoadWAV("sounds/clear2.wav");
    snd_linesClear3 = Mix_LoadWAV("sounds/clear3.wav");
    snd_linesClear4 = Mix_LoadWAV("sounds/clear4.wav");

    Mix_Music *music_mainMenu, *music_inGame;
    music_mainMenu = Mix_LoadMUS("sounds/music_mainmenu.wav");
    music_inGame = Mix_LoadMUS("sounds/music_korobeiniki.wav");

    // Background

    /// int wT, hT;

    SDL_Texture *img = NULL;
    img = IMG_LoadTexture(renderer, "images/background.png");
    /// SDL_QueryTexture(img, NULL, NULL, &wT, &hT);
    SDL_Rect texr;
    texr.x = 0;
    texr.y = 0;
    texr.w = 480;
    texr.h = 272;

    /* Text

    SDL_Texture *text;
    SDL_Rect textRect;

    drawText(renderer, "иди нахуй", 0, 0, 24, &text, &textRect);
    */

    // Init menu

    /*
     * 0 - In game
     * 1 - In main menu
     */
    int screenId = 1;

    menu m;
    m.init(renderer);

    Mix_PlayMusic(music_mainMenu, -1);

    // Init game itself

    srand(time(NULL)); // crutch to fix rand func

    GameData tetris;

    int tick = 0;   // max 60
    int rate = 20;  // per second

    while (running) {
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_CONTROLLERDEVICEADDED:
                    SDL_GameControllerOpen(event.cdevice.which);
                    break;
                case SDL_CONTROLLERBUTTONDOWN:
                    // In menu
                    if(screenId == 1) {
                        if(event.cbutton.button == SDL_CONTROLLER_BUTTON_A) {
                            Mix_PlayChannel(1, snd_menuSelect, 0);
                            switch (m.getSelectedItemId())
                            {
                                // Exit from game    
                                case 0:
                                    running = 0;
                                    break;
                                
                                case 1:
                                    screenId = 0;
                                    tetris.init(renderer);
                                    Mix_PlayMusic(music_inGame, -1);
                                    img = IMG_LoadTexture(renderer, "images/background_game.png");
                                    break;
                                
                                default:
                                    break;
                            } 

                        } else if(event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP) {
                            Mix_PlayChannel(1, snd_menuSelect, 0);
                            m.changePosition(0, 1);
                        } else if(event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) {
                            Mix_PlayChannel(1, snd_menuSelect, 0);
                            m.changePosition(0, -1);
                        } else if(event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) {
                            Mix_PlayChannel(1, snd_menuSelect, 0);
                            m.changePosition(-1, 0);
                        } else if(event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
                            Mix_PlayChannel(1, snd_menuSelect, 0);
                            m.changePosition(1, 0);
                        }   
                    } else 
                    // In game
                    if (screenId == 0) {
                        if(event.cbutton.button == SDL_CONTROLLER_BUTTON_A) {
                            if(tetris.rotatePiece() == 1)
                                Mix_PlayChannel(1, snd_pieceRotate, 0);
                        } else if(event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) {
                            tetris.movePiece(-1);
                        } else if(event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
                            tetris.movePiece(1);
                        }   
                    }
                    break;
            }
        }

        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, img, NULL, &texr);

        if (screenId == 1) {
            m.updateFrame(renderer);
        } else if (screenId == 0) {
            if (tick >= 60)
                tick = 0;

            tick++;

            if (tick % rate == 0) {
                switch (tetris.nextTick())
                {
                    case 1:
                        Mix_PlayChannel(2, snd_pieceLanded, 0);
                        break;
                    
                    // Уххх, запах говнокода
                    case 2:
                        Mix_PlayChannel(2, snd_linesClear1, 0);
                        break;

                    case 3:
                        Mix_PlayChannel(2, snd_linesClear2, 0);
                        break;

                    case 4:
                        Mix_PlayChannel(2, snd_linesClear3, 0);
                        break;

                    case 5:
                        Mix_PlayChannel(2, snd_linesClear4, 0);
                        break;
                    
                    default:
                        break;
                }
            }

            tetris.renderBoard(renderer);
        }

        // SDL_RenderCopy(renderer, text, NULL, &textRect);
        SDL_RenderPresent(renderer);
        
    }

    Mix_FreeMusic(music_mainMenu);
    Mix_FreeChunk(snd_menuSelect);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}