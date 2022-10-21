#include <stdint.h>
#include <stdlib.h>
#include <string>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 21
#define BOARD_BRICK_SIZE 11

struct GameData {
    int board[BOARD_HEIGHT][BOARD_WIDTH] = {0};
    uint64_t score;

    struct Point
    {
        int x, y;
    } a[4];

    int figures[7][4] =
    {
        1,3,5,7, // |
        2,4,5,7, // Z
        3,5,4,6, // S
        3,5,4,7, // T (писюн кароч)
        2,3,5,7, // L
        3,5,6,7, // J
        2,3,4,5, // O
    };

    SDL_Texture *bricks;

    void init(SDL_Renderer *renderer) {
        // Load bricks texture to prevent lags
        bricks = IMG_LoadTexture(renderer, "images/bricks.png");
        // And also gen some piece
        dropNextPiece();
    };

    // Controls

    int xP = 0;
    bool rotate = true;

    int n = 2;   // Like i say: ID for figure

    void movePiece(int x)
    {
        Point b[4];
        for (size_t i = 0; i < 4; i++)
        {
            b[i].x = a[i].x;
            b[i].y = a[i].y;
        }

        for (size_t i = 0; i < 4; i++)
            b[i].x += x;

        if(check(b) == 1) 
        {
            for (size_t i = 0; i < 4; i++)
                a[i] = b[i];
        }
    }

    int rotatePiece()
    {
        Point p = a[1];     // Center of rotation
        Point b[4];
        for (size_t i = 0; i < 4; i++)
            b[i] = a[i];

        for (size_t i = 0; i < 4; i++)
        {
            int x = b[i].y - p.y;
            int y = b[i].x - p.x;
            b[i].x = p.x - x;
            b[i].y = p.y + y;
        }

        if(check(b) == 1) 
        {
            for (size_t i = 0; i < 4; i++)
                a[i] = b[i];
            return 1;
        } else {
            return 0;
        }
    }

    // Ticks

    /*
     * Returns event ID
     * 0 - no event
     * 1 - figure landed
     * 2-5 - line cleared
     */
    int nextTick() {
        static int returnValue;
        returnValue = 0;

        // Landing
        Point b[4];
        for (size_t i = 0; i < 4; i++)
            b[i] = a[i];

        for (size_t i = 0; i < 4; i++)
            b[i].y+=1;

        if(check(b) == 0)
        {
            for (size_t i = 0; i < 4; i++)
                board[a[i].y][a[i].x] = n+1;

            dropNextPiece();
            returnValue = 1;
        } else {
            for (size_t i = 0; i < 4; i++)
                a[i] = b[i];
        }

        // Check for lines
        int k = BOARD_HEIGHT-1;
        static int linesCount;
        linesCount = 0;

        for (size_t i = BOARD_HEIGHT-1; i > 0; i--)
        {
            int blocksCount = 0;
            for (size_t j = 0; j < BOARD_WIDTH; j++)
            {
                if (board[i][j])
                    blocksCount++;
                
                board[k][j] = board[i][j];
            }

            if (blocksCount < BOARD_WIDTH)
                k--;

            if (blocksCount == BOARD_WIDTH)
                linesCount++;
        }

        if(linesCount > 0)
            returnValue += linesCount;
        
        return returnValue;
    }

    // Rendering!

    void renderBoard(SDL_Renderer *renderer) {
        for (size_t i = 0; i < 4; i++)
        {
            SDL_Rect rect;
            rect.x = (480-11*BOARD_WIDTH)/2 + a[i].x * BOARD_BRICK_SIZE;
            rect.y = (272-11*BOARD_HEIGHT)/2 + a[i].y * BOARD_BRICK_SIZE;
            rect.w = BOARD_BRICK_SIZE;
            rect.h = BOARD_BRICK_SIZE;

            SDL_Rect rectTexture;
            rectTexture.x = 11*n;
            rectTexture.y = 0;
            rectTexture.w = 11;
            rectTexture.h = 11;
            SDL_RenderCopy(renderer, bricks, &rectTexture, &rect);
        }
        

        for (size_t y = 0; y < BOARD_HEIGHT; y++)
        {
            for (size_t x = 0; x < BOARD_WIDTH; x++)
            {
                if(board[y][x] > 0) {
                    SDL_Rect rect;
                    rect.x = (480-11*BOARD_WIDTH)/2 + x * BOARD_BRICK_SIZE;
                    rect.y = (272-11*BOARD_HEIGHT)/2 + y * BOARD_BRICK_SIZE;
                    rect.w = BOARD_BRICK_SIZE;
                    rect.h = BOARD_BRICK_SIZE;

                    SDL_Rect rectTexture;
                    rectTexture.x = 11*(board[y][x]-1);
                    rectTexture.y = 0;
                    rectTexture.w = 11;
                    rectTexture.h = 11;
                    SDL_RenderCopy(renderer, bricks, &rectTexture, &rect);
                }
            }
        }
    }

    // Aaaaand some private stuff...

    private:
        void dropNextPiece()
        {
            n = rand() % 7;
            for (size_t i = 0; i < 4; i++)
            {
                a[i].x = figures[n][i] % 2;
                a[i].y = figures[n][i] / 2;
            }
        }

        int check(Point b[4])
        {
            for (size_t i = 0; i < 4; i++)
            {
                if (b[i].x < 0 || b[i].x >= BOARD_WIDTH || b[i].y >= BOARD_HEIGHT) {
                    return 0;
                }
                else if (board[b[i].y][b[i].x] > 0) 
                    return 0;
            }
            return 1;
        }
};


