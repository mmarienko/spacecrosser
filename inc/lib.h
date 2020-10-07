#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

void write_err(const char *err)
{
    for (int i = 0; err[i] != 0; i++)
    {
        write(2, &err[i], 1);
    }
    const char *f = SDL_GetError();
    for (int i = 0; f[i] != 0; i++)
    {
        write(2, &f[i], 1);
    }
    write(2, "\n", 1);
}

int SCREEN_HEIGHT = 600;
int SCREEN_WIDTH = 800;

void renderTexture1(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, SDL_Rect *clip)
{
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    if (clip != NULL)
    {
        dst.w = clip->w;
        dst.h = clip->h;
    }
    else
    {
        SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    }
    SDL_RenderCopy(ren, tex, clip, &dst);
}

typedef struct
{
    int health;
    bool able_to_move;
    bool able_to_hit;
} Boxer;

void reinitialize(Boxer *left, Boxer *right, int *X1, int *X2, int MAX_LEFT, int MAX_RIGHT, int *useClip1,
                  int *useClip2, int *useClipHP1, int *useClipHP2, int *Switch1, int *Switch2, int *check_end)
{
    left->able_to_hit = true;
    left->able_to_move = true;
    left->health = 3;
    *X1 = MAX_LEFT;

    right->able_to_hit = true;
    right->able_to_move = true;
    right->health = 3;
    *X2 = MAX_RIGHT;

    *useClip1 = 0;
    *useClip2 = 2;

    *useClipHP1 = 0;
    *useClipHP2 = 0;

    *Switch1 = 0;
    *Switch2 = 0;

    *check_end = 0;
}
