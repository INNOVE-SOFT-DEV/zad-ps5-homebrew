/* ZAD PS5 - hello world : ecran violet + "ZAD" + indicateur de vie.
 * Meme code pour PC (Linux/WSL) et PS5. Dessin direct sur la surface de la
 * fenetre (SDL_GetWindowSurface) : c'est la seule voie geree par le portage SDL PS5.
 * Pas de SDL2_ttf : les lettres sont dessinees avec des rectangles. */
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define W 1920
#define H 1080

#ifdef PLATFORM_PS5
#define LOG_PATH "/data/zad.log"
#else
#define LOG_PATH "zad.log"
#endif

static void journal(const char *fmt, const char *arg)
{
    FILE *f = fopen(LOG_PATH, "a");
    if (!f)
        return;
    fprintf(f, fmt, arg ? arg : "");
    fputc('\n', f);
    fclose(f);
}

static const char *LETTRES[3][7] = {
    {"11111", "00001", "00010", "00100", "01000", "10000", "11111"}, /* Z */
    {"01110", "10001", "10001", "11111", "10001", "10001", "10001"}, /* A */
    {"11110", "10001", "10001", "10001", "10001", "10001", "11110"}, /* D */
};

static void dessiner_zad(SDL_Surface *s, Uint32 couleur, int x0, int y0, int px)
{
    for (int l = 0; l < 3; l++)
        for (int ry = 0; ry < 7; ry++)
            for (int rx = 0; rx < 5; rx++)
                if (LETTRES[l][ry][rx] == '1') {
                    SDL_Rect c = {x0 + l * 6 * px + rx * px, y0 + ry * px, px - 6, px - 6};
                    SDL_FillRect(s, &c, couleur);
                }
}

int main(void)
{
    journal("demarrage ZAD%s", "");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
        journal("SDL_Init: %s", SDL_GetError());
        return 1;
    }
    SDL_Window *win = SDL_CreateWindow("ZAD", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN);
    if (!win) {
        journal("SDL_CreateWindow: %s", SDL_GetError());
        return 1;
    }
    SDL_Surface *surf = SDL_GetWindowSurface(win);
    if (!surf) {
        journal("SDL_GetWindowSurface: %s", SDL_GetError());
        return 1;
    }
    journal("surface ok", "");

    for (int i = 0; i < SDL_NumJoysticks(); i++)
        if (SDL_IsGameController(i))
            SDL_GameControllerOpen(i);

    const Uint32 violet = SDL_MapRGB(surf->format, 0x2B, 0x21, 0x70);
    const Uint32 ambre = SDL_MapRGB(surf->format, 0xF2, 0xA0, 0x07);
    const Uint32 clair = SDL_MapRGB(surf->format, 0xD8, 0xD3, 0xF5);

    const char *shot = getenv("ZAD_SHOT"); /* capture d'ecran PC, pour les tests */
    int quitter = 0;
    Uint32 frame = 0;
    while (!quitter) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quitter = 1;
            if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_q))
                quitter = 1;
            /* Options (PS5) = bouton START cote SDL */
            if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.button == SDL_CONTROLLER_BUTTON_START)
                quitter = 1;
        }
        SDL_FillRect(surf, NULL, violet);
        dessiner_zad(surf, ambre, surf->w / 2 - (17 * 36) / 2, surf->h / 2 - 7 * 36 / 2 - 40, 36);

        /* barre qui avance : preuve que la boucle tourne */
        SDL_Rect rail = {surf->w / 2 - 300, surf->h - 160, 600, 12};
        SDL_FillRect(surf, &rail, clair);
        SDL_Rect cur = {surf->w / 2 - 300 + (int)((frame * 6) % 520), surf->h - 164, 80, 20};
        SDL_FillRect(surf, &cur, ambre);

        if (SDL_UpdateWindowSurface(win) != 0 && frame == 0)
            journal("SDL_UpdateWindowSurface: %s", SDL_GetError());
        if (frame == 0)
            journal("premiere image envoyee", "");
        if (shot && frame == 3) {
            SDL_SaveBMP(surf, shot);
            quitter = 1;
        }
        frame++;
        SDL_Delay(16);
    }
    SDL_Quit();
    return 0;
}
