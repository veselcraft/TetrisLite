#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct menuElement
{
    SDL_Texture *image = nullptr;
    int action;
};

struct menu
{
    unsigned int pointer[2] = {0, 0};        // X and Y
    SDL_Texture *pointerTexture;    // Texture for pointer
    menuElement elements[2][2];     // Since menu is just 2x2 box, we'll do some limiting

    // We don't need SDL_Rect, since menu is full screen
    void init(SDL_Renderer *renderer)
    {
        // Start game
        elements[0][0].action = 1; 
        elements[0][0].image = IMG_LoadTexture(renderer, "images/menu_play.png");

        // Exit game
        elements[0][1].action = 0;
        elements[0][1].image = IMG_LoadTexture(renderer, "images/menu_exit.png");

        // Let's fix it: 0 is always back or exit action

        pointerTexture = IMG_LoadTexture(renderer, "images/menu_pointer.png");

        updateFrame(renderer);
    }

    void updateFrame(SDL_Renderer *renderer)
    {
        // Yeah, it's hardcoded.
        for (size_t y = 0; y < 2; y++)
        {
            for (size_t x = 0; x < 2; x++)
            {
                if (elements[y][x].image == nullptr) {
                    continue;
                }
                SDL_Rect rect;
                rect.x = (480-(138*2))/2 - 4 + (138*x) + (x*8);
                rect.y = (272-(88*2))/2 - 4 + (88*y) + (y*8);
                rect.w = 138;
                rect.h = 88;
                SDL_RenderCopy(renderer, elements[y][x].image, NULL, &rect);
            }
        }

        // We're also render some pointers xd
        int xP, yP;

        xP = pointer[0];
        yP = pointer[1];

        SDL_Rect rectPointer;
        rectPointer.x = (480-(140*2))/2 - 4 + (142*xP) + (xP*4);
        rectPointer.y = (272-(90*2))/2 - 4 + (92*yP) + (yP*4);
        rectPointer.w = 142;
        rectPointer.h = 92;
        SDL_RenderCopy(renderer, pointerTexture, NULL, &rectPointer);
    }

    void changePosition(int x, int y) {
        if      (x <= 1 && x >= -1 && pointer[0]+x <= 1 && pointer[0]+x >= 0) {
            pointer[0] += x;
        }else if(y <= 1 && y >= -1 && pointer[1]+y <= 1 && pointer[1]+y >= 0) {
            pointer[1] += y;
        }
    }

    unsigned int getSelectedItemId() {
        int x, y;

        x = pointer[0];
        y = pointer[1];

        return elements[y][x].action; 
    }
};