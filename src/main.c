#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

int main(void)
{
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("/mnt/SDCARD/inziu-iosevkaCC-SC-regular.ttf", 24);
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Surface *screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE);
    SDL_WM_SetCaption("SDL Test", NULL);

    // 1. Clear the screen
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_Flip(screen);
    
    // 2. Draw text to the screen
    SDL_Color color = { 255, 255, 255 };
    SDL_Surface *text = TTF_RenderText_Solid(font, "Hello World!", color);
    SDL_Rect position = { 0, 0, 0, 0 };
    SDL_BlitSurface(text, NULL, screen, &position);
    SDL_FreeSurface(text);
    SDL_Flip(screen);
        
    SDL_Event event;
    int running = 1;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = 0;
                    break;
            }
        }

    }

    SDL_Quit();
    TTF_Quit();

    return 0;
}

