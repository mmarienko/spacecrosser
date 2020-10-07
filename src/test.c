#include "../inc/lib.h"
#include "../inc/audio.h"

int main()
{

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        write_err("SDL_INIT error: ");
        SDL_Quit();
        return 1;
    }

    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        write_err("SDL_INIT error: ");
        SDL_Quit();
        return 1;
    }

    initAudio();

    SDL_Window *window = SDL_CreateWindow("SDL Game", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_HIDDEN);
    if (window == NULL)
    {
        write_err("SDL_CreateWindow error: ");
        SDL_Quit();
        return 1;
    }

    SDL_ShowWindow(window);

    SDL_Renderer *ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL)
    {
        write_err("SDL_CreateRenderer error: ");
        SDL_Quit();
        return 1;
    }

    SDL_Texture *background = IMG_LoadTexture(ren, "resource/assets/img/background.png");
    if (background == NULL)
    {
        write_err("IMG_LoadTexture error: ");
        SDL_Quit();
        return 1;
    }

    SDL_Texture *box_right = IMG_LoadTexture(ren, "resource/assets/img/right1.png");
    if (box_right == NULL)
    {
        write_err("IMG_LoadTexture error: ");
        SDL_Quit();
        return 1;
    }

    SDL_Texture *box_left = IMG_LoadTexture(ren, "resource/assets/img/left1.png");
    if (box_left == NULL)
    {
        write_err("IMG_LoadTexture error: ");
        SDL_Quit();
        return 1;
    }

    SDL_Texture *box_right_finish = IMG_LoadTexture(ren, "resource/assets/img/right_final1.png");
    if (box_right_finish == NULL)
    {
        write_err("IMG_LoadTexture error: ");
        SDL_Quit();
        return 1;
    }

    SDL_Texture *box_left_finish = IMG_LoadTexture(ren, "resource/assets/img/left_final1.png");
    if (box_left_finish == NULL)
    {
        write_err("IMG_LoadTexture error: ");
        SDL_Quit();
        return 1;
    }

    SDL_Texture *HP = IMG_LoadTexture(ren, "resource/assets/img/hp.png");
    if (box_left_finish == NULL)
    {
        write_err("IMG_LoadTexture error: ");
        SDL_Quit();
        return 1;
    }

    SDL_Texture *Round = IMG_LoadTexture(ren, "resource/assets/img/count.png");
    if (Round == NULL)
    {
        write_err("IMG_LoadTexture error: ");
        SDL_Quit();
        return 1;
    }

    SDL_Texture *Win = IMG_LoadTexture(ren, "resource/assets/img/won.png");
    if (Win == NULL)
    {
        write_err("IMG_LoadTexture error: ");
        SDL_Quit();
        return 1;
    }

    SDL_Event event;

    int running = 1;

    int check_end = 0;

    int count_win1 = 0;

    int count_win2 = 0;

    int MAX_LEFT = SCREEN_WIDTH / 16;

    int MAX_RIGHT = SCREEN_WIDTH - SCREEN_WIDTH * 0.3;

    int X1 = MAX_LEFT;

    int X2 = MAX_RIGHT;

    int Y = SCREEN_HEIGHT / 1.6;

    int iW = 200;
    int iH = 200;

    SDL_Rect clips1[4];
    for (int i = 0; i < 4; ++i)
    {
        clips1[i].x = i / 2 * iW;
        clips1[i].y = i % 2 * iH;
        clips1[i].w = iW;
        clips1[i].h = iH;
    }

    iH = 12;

    SDL_Rect clips2[4];
    for (int i = 0; i < 4; i++)
    {
        clips2[i].x = 0;
        clips2[i].y = i * iH;
        clips2[i].w = 50;
        clips2[i].h = iH;
    }

    iW = 11;

    SDL_Rect clips3[6];
    for (int i = 0; i < 4; i++)
    {
        clips3[i].x = 39 * 4;
        clips3[i].y = 11 * i * 4;
        clips3[i].h = 11 * 4;
        clips3[i].w = 13 * 4;
    }
    clips3[0].x += 2;
    clips3[0].h += 1;
    clips3[1].y += 6;
    clips3[1].h -= 2;
    clips3[2].y++;
    clips3[2].h--;

    clips3[4].x = 26 * 4;
    clips3[4].y = 33 * 4;
    clips3[4].h = 11 * 4;
    clips3[4].w = 13 * 4;

    clips3[5].x = 0;
    clips3[5].y = 0;
    clips3[5].h = 11 * 4;
    clips3[5].w = 29 * 4;

    SDL_Rect clips4[2];
    for (int i = 0; i < 2; i++)
    {
        clips4[i].x = 0;
        clips4[i].y = i * 200;
        clips4[i].w = 400;
        clips4[i].h = 200;
    }

    int useClip1 = 0;
    int useClip2 = 2;
    int useClipHP1 = 0;
    int useClipHP2 = 0;

    int useClipRound = 0;

    int useClipWin = 0;

    int Switch1 = 0;
    int Switch2 = 0;

    int STEP = 20;

    int KEEP_DISTANCE = 150;

    int HEALTH = 3;

    Boxer *left;
    Boxer *right;
    left = malloc(sizeof(int) + sizeof(bool) * 2);
    left->health = HEALTH;
    left->able_to_move = true;
    left->able_to_hit = true;
    right = malloc(sizeof(int) + sizeof(bool) * 2);
    right->health = HEALTH;
    right->able_to_move = true;
    right->able_to_hit = true;

    int round = 0;

    int quantity = 4;

    while (running)
    {
        if (left->health == 0 || right->health == 0)
        {
            if (round < quantity)
            {
                round++;
                SDL_Delay(2000);
                reinitialize(left, right, &X1, &X2, MAX_LEFT, MAX_RIGHT, &useClip1,
                             &useClip2, &useClipHP1, &useClipHP2, &Switch1, &Switch2, &check_end);
                useClipRound++;
            }
        }
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
                round = 5;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_LEFT:
                    if (right->able_to_move)
                    {
                        if (Switch2 == 0)
                        {
                            useClip2 = 0;
                            Switch2 = 1;
                        }
                        else
                        {
                            useClip2 = 3;
                            Switch2 = 0;
                        }
                        if (X2 > X1 + KEEP_DISTANCE)
                        {
                            X2 -= STEP;
                        }
                    }
                    break;
                case SDLK_RIGHT:
                    if (right->able_to_move)
                    {
                        if (Switch2 == 0)
                        {
                            useClip2 = 0;
                            Switch2 = 1;
                        }
                        else
                        {
                            useClip2 = 3;
                            Switch2 = 0;
                        }
                        if (X2 < MAX_RIGHT)
                        {
                            X2 += STEP;
                        }
                    }
                    break;
                case SDLK_a:
                    if (left->able_to_move)
                    {
                        if (Switch1 == 0)
                        {
                            useClip1 = 1;
                            Switch1 = 1;
                        }
                        else
                        {
                            useClip1 = 2;
                            Switch1 = 0;
                        }
                        if (X1 > MAX_LEFT)
                        {
                            X1 -= STEP;
                        }
                    }
                    break;
                case SDLK_d:
                    if (left->able_to_move)
                    {
                        if (Switch1 == 0)
                        {
                            useClip1 = 1;
                            Switch1 = 1;
                        }
                        else
                        {
                            useClip1 = 2;
                            Switch1 = 0;
                        }
                        if (X1 < X2 - KEEP_DISTANCE)
                        {
                            X1 += STEP;
                        }
                    }
                    break;
                case SDLK_s:
                    if (right->able_to_move && left->able_to_hit)
                    {
                        if (X1 > X2 - KEEP_DISTANCE - STEP)
                        {
                            right->health--;
                            useClipHP2++;
                            playSound("resource/assets/music/punch.wav", SDL_MIX_MAXVOLUME);
                            if (right->health == 0)
                            {
                                check_end = 1;
                                count_win1++;
                                right->able_to_move = false;
                                right->able_to_hit = false;
                            }
                        }
                        useClip1 = 3;
                        left->able_to_hit = false;
                    }
                    break;
                case SDLK_DOWN:
                    if (left->able_to_move && right->able_to_hit)
                    {
                        if (X1 > X2 - KEEP_DISTANCE - STEP)
                        {
                            left->health--;
                            useClipHP1++;
                            playSound("resource/assets/music/punch.wav", SDL_MIX_MAXVOLUME);
                            if (left->health == 0)
                            {
                                check_end = 1;
                                count_win2++;
                                left->able_to_move = false;
                                left->able_to_hit = false;
                            }
                        }
                        useClip2 = 1;
                        right->able_to_hit = false;
                    }
                    break;
                case SDLK_ESCAPE:
                    running = 0;
                    break;

                case SDLK_w:
                    if (left->able_to_move)
                    {
                        useClip1 = 0;
                        left->able_to_hit = true;
                    }
                    break;

                case SDLK_UP:
                    if (right->able_to_move)
                    {
                        useClip2 = 2;
                        right->able_to_hit = true;
                    }
                    break;

                case SDLK_r:
                    reinitialize(left, right, &X1, &X2, MAX_LEFT, MAX_RIGHT, &useClip1,
                                 &useClip2, &useClipHP1, &useClipHP2, &Switch1, &Switch2, &check_end);
                    round = 0;
                    useClipRound = 0;

                default:
                    break;
                }
            }
        }

        SDL_RenderClear(ren);

        SDL_RenderCopy(ren, background, NULL, NULL);

        if (check_end)
        {
            if (right->health == 0 && left->health == 0)
            {
                renderTexture1(box_right_finish, ren, X2, Y, NULL);
                renderTexture1(box_left_finish, ren, X1, Y, NULL);
            }
            else
            {
                if (right->health == 0)
                {
                    renderTexture1(box_right_finish, ren, X2, Y, NULL);
                    renderTexture1(box_left, ren, X1, Y, &clips1[useClip1]);
                }
                else
                {
                    renderTexture1(box_left_finish, ren, X1, Y, NULL);
                    renderTexture1(box_right, ren, X2, Y, &clips1[useClip2]);
                }
            }
        }
        else
        {

            renderTexture1(box_right, ren, X2, Y, &clips1[useClip2]);
            renderTexture1(box_left, ren, X1, Y, &clips1[useClip1]);
        }

        if (round == quantity && check_end)
        {
            if (count_win2 > count_win1)
            {
                useClipWin = 1;
            }
            else
            {
                useClipWin = 0;
            }
            renderTexture1(Win, ren, 200, 200, &clips4[useClipWin]);
        }

        renderTexture1(HP, ren, 50, 100, &clips2[useClipHP1]);

        renderTexture1(HP, ren, SCREEN_WIDTH - 100, 100, &clips2[useClipHP2]);

        renderTexture1(Round, ren, 300, 55, &clips3[5]);

        renderTexture1(Round, ren, 450, 55, &clips3[useClipRound]);

        SDL_RenderPresent(ren);

        SDL_Delay(5);
    }

    SDL_DestroyTexture(background);

    SDL_DestroyTexture(box_right);

    SDL_DestroyTexture(box_left);

    SDL_DestroyRenderer(ren);

    SDL_DestroyWindow(window);

    SDL_Quit();

    free(left);

    free(right);

    return 0;
}
