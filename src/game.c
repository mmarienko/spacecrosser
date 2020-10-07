/* Language Includes */
#include "minilib.h"

/* Standard System Include */

#ifndef F_OK
#define F_OK 0
#endif

/* SDL Library */
#include <SDL.h>

/* Local Includes */
#include "types.h"
#include "wrappers.h"
#include "object.h"
#include "game.h"
#include "global.h"
#include "extern.h"
#include "audio.h"

void endSDL()
{

    SDL_free(Global->basePath);
    SDL_DestroyRenderer(Global->renderer);
    SDL_DestroyWindow(Global->window);

    free(Global);
    SDL_Quit();
}

void startSDL()
{
    SDL_Init(SDL_INIT_AUDIO);

    initAudio();
    /* Global game object */
    Global = calloc(1, sizeof(GlobalSDL));

    if (Global == NULL)
    {
        fprintf(stderr, "[%s][%s: %d]Fatal Error: Memory c-allocation error\n", getDate(), __FILE__, __LINE__);
        exit(0);
    }

    Global->basePath = SDL_GetBasePath();

    if (Global->basePath == NULL)
    {
        fprintf(stderr, "[%s][%s: %d]Fatal Error: Could not get SDL Basepath String, error: %s\n", getDate(), __FILE__, __LINE__, SDL_GetError());
        exit(0);
    }

    if (freopen(getAbsolutePath(ERROR_FILE), "a", stderr) == NULL)
    {
        fprintf(stderr, "[%s][%s: %d]Warning: Freopen could not pipe stream\n", getDate(), __FILE__, __LINE__);
    }

    /* Initialize Random Seed */
    srand(time(NULL));

    Global->window = NULL;
    Global->renderer = NULL;

    /* Initialize Window */
    if (!initSDL("Naquadah Asteroids"))
    {
        exit(0);
    }

    setWindowIcon(getAbsolutePath(IMG_DIR "main-ship.bmp"));

    /* Constant Logic / initialize */
    Global->screenWidth = 1920;  //getNativeWidth();
    Global->screenHeight = 1080; //getNativeHeight();
    Global->screenTop = 70;
    Global->screenBottom = 70;
    Global->screenLeft = 5;
    Global->screenRight = 5;
    /* Make this an option */
    Global->framesPerSecond = 60.0;
    Global->gameTickRatio = (60.0 / Global->framesPerSecond);

    /* Initialize Keystates */
    Global->keystates = SDL_GetKeyboardState(NULL);

    Global->exit = 0;
    Global->state = DEFAULT;

    SHIP_SPEED = (7.5 * Global->gameTickRatio);
    ASTEROID_SPEED = (3.5 * Global->gameTickRatio);
    BULLET_TINY_SPEED = (13.0 * Global->gameTickRatio);

    /* Global Score */
    score = 0;
}

void updateObjectCollision(Object **ship, Object **bullets, Object **rockets, Object **asteroids)
{
    Object *asteroidsRoot = *asteroids;
    Object *bulletsRoot;
    Object *rocketsRoot;

    while (asteroidsRoot != NULL)
    {
        bulletsRoot = *bullets;
        rocketsRoot = *rockets;

        if (objectCollision(asteroidsRoot, *ship))
        {
            if (asteroidsRoot->type != ENEMY_BOSS)
            {
                (*ship)->lives--;
                asteroidsRoot->lives = 0;
            }
            else
            {
                (*ship)->lives = 0;
                asteroidsRoot->lives = 0;
            }
            
        }

        while (bulletsRoot != NULL)
        {
            if (objectCollision(asteroidsRoot, bulletsRoot))
            {
                asteroidsRoot->lives--;
                bulletsRoot->lives = 0;

                if (asteroidsRoot->lives <= 0)
                {
                    if (asteroidsRoot->type == ASTEROID_SMALL)
                    {
                        score += 1;
                    }
                    else if (asteroidsRoot->type == ENEMY_SMALL)
                    {
                        score += 2;
                        playSound(MUSIC_DIR "expSmall.wav", SDL_MIX_MAXVOLUME);
                    }
                    else if (asteroidsRoot->type == ASTEROID_MEDIUM)
                    {
                        score += 3;
                    }
                    else if (asteroidsRoot->type == ENEMY_MEDIUM){
                        score += 4;
                        playSound(MUSIC_DIR "expMedium.wav", SDL_MIX_MAXVOLUME);
                    }
                    else if (asteroidsRoot->type == ASTEROID_LARGE){
                        score += 5;
                    }
                    else if (asteroidsRoot->type == ENEMY_BOSS)
                    {
                        score += 20;
                        playSound(MUSIC_DIR "expBoss.wav", SDL_MIX_MAXVOLUME);
                    }
                    
                }
            }

            bulletsRoot = bulletsRoot->next;
        }

        while (rocketsRoot != NULL)
        {
            if (objectCollision(asteroidsRoot, rocketsRoot))
                {
                    asteroidsRoot->lives -= 5;
                    rocketsRoot->lives = 0;

                    if (asteroidsRoot->lives <= 0)
                    {
                        if (asteroidsRoot->type == ASTEROID_SMALL)
                        {
                            score += 1;
                        }
                        else if (asteroidsRoot->type == ENEMY_SMALL)
                        {
                            score += 2;
                            playSound(MUSIC_DIR "expSmall.wav", SDL_MIX_MAXVOLUME);
                        }
                        else if (asteroidsRoot->type == ASTEROID_MEDIUM)
                        {
                            score += 3;
                        }
                        else if (asteroidsRoot->type == ENEMY_MEDIUM){
                            score += 4;
                            playSound(MUSIC_DIR "expMedium.wav", SDL_MIX_MAXVOLUME);
                        }
                        else if (asteroidsRoot->type == ASTEROID_LARGE){
                            score += 5;
                        }
                        else if (asteroidsRoot->type == ENEMY_BOSS)
                        {
                            score += 20;
                            playSound(MUSIC_DIR "expBoss.wav", SDL_MIX_MAXVOLUME);
                        }        
                    }
                }
            rocketsRoot = rocketsRoot->next;    
            }
        asteroidsRoot = asteroidsRoot->next;
    }
}

Object *updateAsteroids(Object *asteroids, SDL_Texture *asteroidSmall, SDL_Texture *enemySmall, SDL_Texture *asteroidMedium, SDL_Texture *enemyMedium, SDL_Texture *asteroidLarge, SDL_Texture *enemyBoss)
{
    Object *asteroidsRoot;
    Object *asteroid;
    Object *previousAsteroid;
    uint8_t random;
    uint8_t root = 1;

    ASTEROID_SPEED *= (ASTEROID_SPEED < SHIP_SPEED * 2) ? 1.00025 : 1;

    if ((rand() % (int)(30 / ((ASTEROID_SPEED < 30) ? ASTEROID_SPEED : 30))) == 0)
    {
        random = rand() % 5;

        if (random == 1)
        {
            asteroid = createObject(asteroidSmall, 0, 1, ASTEROID_SMALL, 1, 0, 0, 32, 32, 1.0);
        }
        else if (random == 2)
        {
            asteroid = createObject(enemySmall, 0, 1, ENEMY_SMALL, 2, 0, 0, 48, 48, 1.0);
        }
        else if (random == 3)
        {
            asteroid = createObject(asteroidMedium, 0, 1, ASTEROID_MEDIUM, 3, 0, 0, 64, 64, 1.0);
        } 
        else if (random == 4)
        {
            asteroid = createObject(enemyMedium, 0, 1, ENEMY_MEDIUM, 4, 0, 0, 80, 80, 1.0);
        } 
        else if(score > 50 && (score % 50 == 0 || score % 51 == 0 || score % 52 == 0 || score % 53 == 0 || score % 54 == 0 || score % 55 == 0))
        {
            asteroid = createObject(enemyBoss, 0, 1, ENEMY_BOSS, 20, 0, 0, 256, 280, 1.0);
        }
        else
        {
            asteroid = createObject(asteroidLarge, 0, 1, ASTEROID_LARGE, 5, 0, 0, 96, 96, 1.0);
        } 

        

        asteroid->x = (int)((rand() % (Global->screenWidth)) - (asteroid->clip.w / 2));
        asteroid->y = -(asteroid->clip.h);

        asteroid->next = asteroids;
        asteroids = asteroid;
    }

    asteroidsRoot = asteroids;
 

    while (asteroids != NULL)
    {
        if (asteroids->y > (Global->screenHeight + asteroids->clip.h) || asteroids->lives <= 0)
        {
            asteroid = asteroids;
            asteroids = asteroids->next;

            if (root)
            {
                asteroidsRoot = asteroids;
            }
            else
            {
                previousAsteroid->next = asteroids;
            }

            asteroid->next = NULL;
            freeObjects(asteroid);
            continue;
        }

        previousAsteroid = asteroids;

        moveObject(asteroids, 0, ASTEROID_SPEED);
        asteroids = asteroids->next;

        if (root)
        {
            root = 0;
        }
    }

    return asteroidsRoot;
}

Object *updateUserBullets(Object *ship, Object *bullets, SDL_Texture *image, uint32_t *timer, int queue)
{
    Object *bulletsRoot;
    Object *bullet;
    Object *previousBullet;
    uint8_t root = 1;

    if (timer[BULLET_TINY_TIMER] < SDL_GetTicks())
    {
        if (Global->keystates[SDL_SCANCODE_SPACE])
        {
            bullet = createObject(image, 0, 2, BULLET_TINY, 1, 0, 0, 5, 12, 1.0);

            if (queue == 0)
            {
                bullet->x = (ship->x + (bullet->clip.w / 2) + 10);
                bullet->y = (ship->y - (bullet->clip.w / 2));
            }
            else
            {
                bullet->x = (ship->x + (bullet->clip.w / 2) + 44);
                bullet->y = (ship->y - (bullet->clip.w / 2));
            }

            bullet->next = bullets;

            bullets = bullet;

            playSound(MUSIC_DIR "shot.wav", SDL_MIX_MAXVOLUME / 2);

            timer[BULLET_TINY_TIMER] = (SDL_GetTicks() + 150);
        }
    }

    bulletsRoot = bullets;

    while (bullets != NULL)
    {
        if (bullets->y <= Global->screenTop || bullets->lives <= 0)
        {
            bullet = bullets;
            bullets = bullets->next;

            if (root)
            {
                bulletsRoot = bullets;
            }
            else
            {
                previousBullet->next = bullets;
            }

            bullet->next = NULL;
            freeObjects(bullet);
            continue;
        }

        previousBullet = bullets;

        moveObject(bullets, 0, (-1 * BULLET_TINY_SPEED));
        bullets = bullets->next;

        if (root)
        {
            root = 0;
        }
    }

    return bulletsRoot;
}

Object *updateUserRockets(Object *ship, Object *rockets, SDL_Texture *image, uint32_t *timer)
{
    Object *rocketsRoot;
    Object *rocket;
    Object *previousRocket;
    uint8_t root = 1;

    if (timer[ROCKET_TIMER] < SDL_GetTicks())
    {
        if (Global->keystates[SDL_SCANCODE_X])
        {
            rocket = createObject(image, 0, 2, ROCKET, 5, 0, 0, 12, 20, 1.0);

            rocket->x = (ship->x + (rocket->clip.w / 2) + 19);
            rocket->y = (ship->y - (rocket->clip.w / 2));

            rocket->next = rockets;
            rockets = rocket;
            playSound(MUSIC_DIR "rocketLaunch.wav", SDL_MIX_MAXVOLUME / 2);

            timer[ROCKET_TIMER] = (SDL_GetTicks() + 3000);
        }
    }

    rocketsRoot = rockets;

    while (rockets != NULL)
    {
        if (rockets->y <= Global->screenTop || rockets->lives <= 0)
        {
            rocket = rockets;
            rockets = rockets->next;

            if (root)
            {
                rocketsRoot = rockets;
            }
            else
            {
                previousRocket->next = rockets;
            }

            rocket->next = NULL;
            freeObjects(rocket);
            continue;
        }

        previousRocket = rockets;

        moveObject(rockets, 0, (-1 * BULLET_TINY_SPEED));
        rockets = rockets->next;

        if (root)
        {
            root = 0;
        }
    }

    return rocketsRoot;
}

void updateUserShipMovement(Object *ship)
{
    int8_t shipX = 0;
    int8_t shipY = 0;
    int temp = 0;

    /* User Keyboard  */
    if (Global->keystates[SDL_SCANCODE_LEFT] || Global->keystates[SDL_SCANCODE_A])
    {
        shipX--;
    }

    if (Global->keystates[SDL_SCANCODE_RIGHT] || Global->keystates[SDL_SCANCODE_D])
    {
        shipX++;
    }

    if (Global->keystates[SDL_SCANCODE_UP] || Global->keystates[SDL_SCANCODE_W])
    {
        shipY--;
    }

    if (Global->keystates[SDL_SCANCODE_DOWN] || Global->keystates[SDL_SCANCODE_S])
    {
        shipY++;
    }

    /* Updating Ship Animation */
    // if (shipX == 0)
    // {
    //     ship->subImage = 0;
    // }
    // else if (shipX == -1)
    // {
    //     ship->subImage = 1;
    // }
    // else if (shipX == 1)
    // {
    //     ship->subImage = 2;
    // }

    shipX *= SHIP_SPEED;
    shipY *= SHIP_SPEED;

    /* Setting Ship Boundaries */
    if ((ship->x + shipX) < Global->screenLeft)
    {
        temp = (Global->screenLeft - ship->x);
        shipX = temp > 0 ? temp : 0;
    }

    if ((ship->x + ship->clip.w + shipX) > (Global->screenWidth - Global->screenRight))
    {
        temp = ((Global->screenWidth - Global->screenRight) - ship->x - ship->clip.w);
        shipX = temp > 0 ? temp : 0;
    }

    if ((ship->y + shipY) < Global->screenTop)
    {
        temp = (Global->screenTop - ship->y);
        shipY = temp > 0 ? temp : 0;
    }

    if ((ship->y + ship->clip.h + shipY) > (Global->screenHeight - Global->screenBottom))
    {
        temp = ((Global->screenHeight - Global->screenBottom) - ship->y - ship->clip.h);
        shipY = temp > 0 ? temp : 0;
    }

    moveObject(ship, shipX, shipY);
}

Object *updateUserActions(Object *ship, Object *bullets, SDL_Texture *image, uint32_t *timer, int queue)
{
    updateUserShipMovement(ship);
    return updateUserBullets(ship, bullets, image, timer, queue);
}

Object *updateUserActionsR(Object *ship, Object *rockets, SDL_Texture *image, uint32_t *timer)
{
    updateUserShipMovement(ship);
    return updateUserRockets(ship, rockets, image, timer);
}

void updateObjectAnimation(Object *obj)
{
    obj->subImage = ((obj->subImage + 1) >= obj->subImageNumber) ? 0 : (obj->subImage + 1);
}
