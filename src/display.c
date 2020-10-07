/* Language Includes */
#include "minilib.h"

/* SDL Library */
#include <SDL.h>

/* Local Includes */
#include "types.h"
#include "global.h"
#include "object.h"
#include "wrappers.h"
#include "display.h"
#include "extern.h"

FILE *fpscore = NULL;
FILE *fpcopy = NULL;

void displayGameOver(SDL_Texture *font)
{
    clearScreen();
    displayTextMiddle(font, "Game Over", FONT_LARGE, 1.0, 0);
    fpscore = openFile(getAbsolutePath(SCORE_FILE), "r");
    fpcopy = openFile(getAbsolutePath(SCORE_COPY_FILE), "w");
    char str[50];
    char *estr;
    while (1)
    {
        estr = fgets(str, sizeof(str), fpscore);
        if (estr == NULL)
        {
            if (feof(fpscore) != 0)
            {
                break;
            }
            else
            {
                break;
            }
        }
        fprintf(fpcopy, estr, Global->timer[DISPLAY_GAME_TIMER]);
    }
    fclose(fpscore);
    fclose(fpcopy);
    fpscore = openFile(getAbsolutePath(SCORE_FILE), "w");
    fprintf(fpscore, "Score: %d, Time: %d\n", score, Global->timer[DISPLAY_GAME_TIMER]);
    fclose(fpscore);
    fpscore = openFile(getAbsolutePath(SCORE_FILE), "a");
    fpcopy = openFile(getAbsolutePath(SCORE_COPY_FILE), "r");
    while (1)
    {
        estr = fgets(str, sizeof(str), fpcopy);
        if (estr == NULL)
        {
            if (feof(fpscore) != 0)
            {
                break;
            }
            else
            {
                break;
            }
        }
        fprintf(fpscore, estr, Global->timer[DISPLAY_GAME_TIMER]);
    }
    fclose(fpcopy);
    fclose(fpscore);
    updateWindow();
    SDL_Delay(5 * 1000);
}

void displayTextMiddle(SDL_Texture *font, char *text, objectType type, float scale, int dist)
{
    Object *temp = createTextObject(font, text, type, scale);

    positionTextObject(temp, (((Global->screenWidth - Global->screenRight) / 2) + Global->screenLeft) - (getTextObjectLength(temp, 0) / 2), dist + (((Global->screenHeight - Global->screenBottom) / 2) + Global->screenTop) - (temp->clip.h / 2));
    freeObjects(temp);
}

void displayHUD(Object *ship, SDL_Texture *font, uint32_t timer)
{
    Object *temp;
    char buffer[BUFFER_SIZE] = {0};
    int previous = 0;
    SDL_Rect bar;

    /* Set the HUD bar */
    bar.x = 0;
    bar.y = -64;
    bar.w = Global->screenWidth;
    bar.h = Global->screenTop;

    SDL_RenderFillRect(Global->renderer, &bar);

    /* Display score text */
    strncpy(buffer, "SCORE:", BUFFER_SIZE);
    temp = createTextObject(font, buffer, FONT_LARGE, 1.0);
    previous = (temp->x + ((countObjects(temp) + 5 - snprintf(buffer, BUFFER_SIZE, "%d", score)) * temp->clip.w * 0.5 * temp->scale));
    positionTextObject(temp, (Global->screenWidth - (countObjects(temp) * temp->clip.w)), 15);
    freeObjects(temp);

    /* Display score number */
    temp = createTextObject(font, buffer, FONT_LARGE, 1.0);
    positionTextObject(temp, Global->screenWidth - 190, 15);

    // /* Display lives */
    SDL_Texture *heartSheet = loadTextureBack(getAbsolutePath(IMG_DIR "heart.bmp"), 0x0, 0x0, 0x0);
    Object *heart = createObject(heartSheet, 0, 0, HEART, 3, 0, 0, 64, 64, 1.0);

    for (int i = 0, width = 0; i < ship->lives; i++)
    {
        positionTextObject(heart, ((Global->screenWidth - (countObjects(temp) * temp->clip.w)) / 2) - (64 * ship->lives / 2) + width, 20);
        width += 64;
    }

    /* Display Timer */
    snprintf(buffer, BUFFER_SIZE, "TIMER: %d", timer);
    temp = createTextObject(font, buffer, FONT_LARGE, 1.0);

    positionTextObject(temp, 20, 15);
    freeObjects(temp);

    // strncpy(buffer, "TIMER:", BUFFER_SIZE);
    // temp = createTextObject(font, buffer, FONT_LARGE, 1.0);
    // previous = (temp->x + ((countObjects(temp) - snprintf(buffer, BUFFER_SIZE, "%d", (int)Global->timer)) * temp->clip.w * 0.5 * temp->scale));
    // positionTextObject(temp, 20, 15);
    // freeObjects(temp);

    timer = 0;

    strncpy(buffer, "R - Restart, P - Pause, X - Shot rocket, Space - shot lazer, ESC - Exit", BUFFER_SIZE);
    temp = createTextObject(font, buffer, FONT_LARGE, 1.0);

    positionTextObject(temp, 10, 1000);
    freeObjects(temp);
}
