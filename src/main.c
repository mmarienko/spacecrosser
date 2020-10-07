/* Standard Includes */
#include "minilib.h"

/* SDL Library */
#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>
#include <SDL_image.h>

/* Local Includes */
#include "types.h"
#include "wrappers.h"
#include "object.h"
#include "game.h"
#include "global.h"
#include "display.h"
#include "extern.h"
#include "audio.h"

/* Environmental Dependancies */
#ifdef linux
#endif

#ifdef __APPLE__
#endif

#ifdef __WIN32
#include <windows.h>
#define sleep(x) Sleep(x * 1000)
#endif

/* Define Global Variables */
GlobalSDL *Global;

float SHIP_SPEED;
float ASTEROID_SPEED;
float BULLET_TINY_SPEED;

// variable declarations

uint32_t score;

int main()
{
	// Initialize SDL2 Audio only
	SDL_Init(SDL_INIT_AUDIO);

	IMG_Init(IMG_INIT_JPG);

	// Initialize Simple-SDL2-Audio
	initAudio();

	// Play music and a sound
	playMusic(MUSIC_DIR "main.wav", SDL_MIX_MAXVOLUME / 2);

	/* SDL Variables */
	SDL_Texture *laserSprite = NULL;
	SDL_Texture *fontLarge = NULL;
	SDL_Texture *background = NULL;
	SDL_Texture *shipSheet = NULL;
	SDL_Texture *enemySmall = NULL;
	SDL_Texture *asteroidSmall = NULL;
	SDL_Texture *asteroidMedium = NULL;
	SDL_Texture *enemyMedium = NULL;
	SDL_Texture *asteroidLarge = NULL;
	SDL_Texture *enemyBoss = NULL;
	SDL_Texture *rocketSprite = NULL;
	SDL_Texture *startbackground = NULL;

	FILE *fpscoreRead = NULL;

	SDL_Event event;

	/* User Variables */
	Object *ship = NULL;
	Object *bullets = NULL;
	Object *rockets = NULL;

	/* NPC Variables */
	Object *asteroids = NULL;

	/* Setup Pre-Game Logic and Constants */
	startSDL();

	/* Audio */
	SDL_PauseAudio(0);

	/* Load Bitmaps */
	shipSheet = loadTextureBack(getAbsolutePath(IMG_DIR "main-ship.bmp"), 0x0, 0x0, 0x0);
	rocketSprite = loadTextureBack(getAbsolutePath(IMG_DIR "rocket.bmp"), 0x0, 0x0, 0x0);
	laserSprite = loadTextureBack(getAbsolutePath(IMG_DIR "laser.bmp"), 0x0, 0x0, 0x0);
	enemySmall = loadTextureBack(getAbsolutePath(IMG_DIR "enemy_small.bmp"), 0x0, 0x0, 0x0);
	asteroidSmall = loadTextureBack(getAbsolutePath(IMG_DIR "asteroid_small.bmp"), 0x0, 0x0, 0x0);
	asteroidMedium = loadTextureBack(getAbsolutePath(IMG_DIR "asteroid_medium.bmp"), 0x0, 0x0, 0x0);
	asteroidLarge = loadTextureBack(getAbsolutePath(IMG_DIR "asteroid_large.bmp"), 0x0, 0x0, 0x0);
	enemyMedium = loadTextureBack(getAbsolutePath(IMG_DIR "enemy_medium.bmp"), 0x0, 0x0, 0x0);
	enemyBoss = loadTextureBack(getAbsolutePath(IMG_DIR "enemy_boss.bmp"), 0x0, 0x0, 0x0);
	fontLarge = loadTextureBack(getAbsolutePath(FONT_DIR "fontx64.bmp"), 0x0, 0x0, 0x0);
	background = loadTextureBack(getAbsolutePath(IMG_DIR "background.bmp"), 0x0, 0x0, 0x0);
	startbackground = loadTextureBack(getAbsolutePath(IMG_DIR "startbackground.bmp"), 0x0, 0x0, 0x0);

	/* Load User Objects */
	ship = createObject(shipSheet, 0, 0, SHIP, 5, 0, 0, 64, 64, 1.0);
	positionObject(ship, ((Global->screenWidth - Global->screenRight) / 2 + (ship->clip.w / 2)) + Global->screenLeft, ((Global->screenHeight - Global->screenTop) * 0.75 + (ship->clip.h / 2)) + Global->screenBottom);
	Global->timer[BULLET_TINY_TIMER] = SDL_GetTicks();

	int queue = 0;
	int flag = 0;

	while (!Global->exit)
	{
		/* Global Timer */
		Global->timer[GLOBAL_TIMER] = SDL_GetTicks();

		/* Clear Screen */
		clearScreen();
		SDL_RenderCopy(Global->renderer, background, NULL, NULL);

		/* SDL Events */
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				Global->exit = 1;
				break;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_p:
					Global->state = (Global->state == DEFAULT ? PAUSE : DEFAULT);
					break;
				case SDLK_r:
					displayTextMiddle(fontLarge, "The game will be restarted in three seconds", FONT_LARGE, 1.0, 0);
					updateWindow();
					SDL_Delay(3 * 1000);
					asteroids = NULL;
					Global->timer[GLOBAL_TIMER] = 0;
					score = 0;

					positionObject(ship, ((Global->screenWidth - Global->screenRight) / 2 + (ship->clip.w / 2)) + Global->screenLeft, ((Global->screenHeight - Global->screenTop) * 0.75 + (ship->clip.h / 2)) + Global->screenBottom);
					break;
				case SDLK_ESCAPE:
					SDL_Delay(500);
					Global->exit = 1;
					break;
				}
			}
		}

		/* Load Event Stack */
		SDL_PumpEvents();

		/* Default game state, player playing the game */
		if (Global->state == DEFAULT)
		{
			if (Global->timer[DISPLAY_GAME_TIMER] >= 0 && Global->timer[DISPLAY_GAME_TIMER] <= 3 && flag == 0)
			{
				SDL_RenderCopy(Global->renderer, startbackground, NULL, NULL);
				asteroids = NULL;
			}
			else if (Global->timer[DISPLAY_GAME_TIMER] == 3)
			{
				SDL_RenderCopy(Global->renderer, background, NULL, NULL);

				flag = 1;
			}
			else
			{
				SDL_RenderCopy(Global->renderer, background, NULL, NULL);
				SDL_DestroyTexture(startbackground);
			}

			/* Have a function to update all timers */
			Global->timer[DISPLAY_GAME_TIMER] = SDL_GetTicks() / 1000;
			if (ship->lives <= 0)
			{
				Global->exit = 1;
				continue;
			}

			asteroids = updateAsteroids(asteroids, asteroidSmall, enemySmall, asteroidMedium, enemyMedium, asteroidLarge, enemyBoss);

			if (queue == 0)
			{
				bullets = updateUserActions(ship, bullets, laserSprite, Global->timer, 0);
				queue = 1;
			}
			else
			{
				bullets = updateUserActions(ship, bullets, laserSprite, Global->timer, queue);
				queue = 0;
			}
			rockets = updateUserActionsR(ship, rockets, rocketSprite, Global->timer);

			updateObjectCollision(&ship, &bullets, &rockets, &asteroids);

			displayHUD(ship, fontLarge, Global->timer[DISPLAY_GAME_TIMER]);
		}
		/* Game Paused */
		else if (Global->state == PAUSE)
		{
			char *estr;
			fpscoreRead = openFile(getAbsolutePath(SCORE_FILE), "r");
			char str[50];

			int dist = -140;

			for (int i = 0; i < 7; i++)
			{

				estr = fgets(str, sizeof(str), fpscoreRead);

				if (estr == NULL)
				{
					if (feof(fpscoreRead) != 0)
					{
						break;
					}
					else
					{
						break;
					}
				}

				displayTextMiddle(fontLarge, estr, FONT_LARGE, 1.0, dist);

				dist += 70;
			}

			displayTextMiddle(fontLarge, "Pause", FONT_LARGE, 1.0, -420);
			displayTextMiddle(fontLarge, "Last Score", FONT_LARGE, 1.0, -210);
			displayHUD(ship, fontLarge, Global->timer[DISPLAY_GAME_TIMER]);
		}

		/* Redraw Window */
		updateWindow();

		/* Frames Per Second Delay */
		delayFramesPerSecond(Global->timer[GLOBAL_TIMER]);
	}

	/* Game Over */
	if (ship->lives <= 0)
	{
		playSound(MUSIC_DIR "mainshipdeath.wav", SDL_MIX_MAXVOLUME);
		playMusic(MUSIC_DIR "gameover.wav", SDL_MIX_MAXVOLUME / 2);
		displayGameOver(fontLarge);
	}

	endAudio();
	IMG_Quit();
	SDL_DestroyTexture(laserSprite);
	SDL_DestroyTexture(fontLarge);
	SDL_DestroyTexture(shipSheet);
	SDL_DestroyTexture(background);
	SDL_DestroyTexture(enemySmall);
	SDL_DestroyTexture(asteroidSmall);
	SDL_DestroyTexture(asteroidMedium);
	SDL_DestroyTexture(enemyMedium);
	SDL_DestroyTexture(asteroidLarge);
	SDL_DestroyTexture(enemyBoss);
	SDL_DestroyTexture(rocketSprite);
	SDL_DestroyRenderer(Global->renderer);
	SDL_DestroyWindow(Global->window);
	freeObjects(ship);
	freeObjects(asteroids);
	freeObjects(rockets);
	freeObjects(bullets);

	/* Close SDL framework */
	endSDL();

	return 0;
}
