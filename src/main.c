/* ZAD PS5 - hello world : ecran violet + "ZAD" + indicateur de vie.
 * Meme code pour PC (Linux/WSL) et PS5. Pas de SDL2_ttf ici : les lettres
 * sont dessinees avec des rectangles, pour ne tester que SDL2 + la toolchain. */
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define W 1920
#define H 1080

/* Couleurs du design ZAD */
#define VIOLET 0x2B, 0x21, 0x70
#define AMBRE 0xF2, 0xA0, 0x07
#define CLAIR 0xD8, 0xD3, 0xF5

static const char *LETTRES[3][7] = {
    {"11111", "00001", "00010", "00100", "01000", "10000", "11111"}, /* Z */
    {"01110", "10001", "10001", "11111", "10001", "10001", "10001"}, /* A */
    {"11110", "10001", "10001", "10001", "10001", "10001", "11110"}, /* D */
};

static void dessiner_zad(SDL_Renderer *r, int x0, int y0, int px)
{
    for (int l = 0; l < 3; l++)
        for (int ry = 0; ry < 7; ry++)
            for (int rx = 0; rx < 5; rx++)
                if (LETTRES[l][ry][rx] == '1') {
                    SDL_Rect c = {x0 + l * 6 * px + rx * px, y0 + ry * px, px - 6, px - 6};
                    SDL_RenderFillRect(r, &c);
                }
}

static void capture(SDL_Renderer *r, const char *chemin)
{
    SDL_Surface *s = SDL_CreateRGBSurfaceWithFormat(0, W, H, 32, SDL_PIXELFORMAT_ARGB8888);
    if (!s)
        return;
    SDL_RenderReadPixels(r, NULL, SDL_PIXELFORMAT_ARGB8888, s->pixels, s->pitch);
    SDL_SaveBMP(s, chemin);
    SDL_FreeSurface(s);
}

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    Uint32 fenetre_flags = SDL_WINDOW_SHOWN;
#ifdef PLATFORM_PS5
    fenetre_flags |= SDL_WINDOW_FULLSCREEN;
#endif
    SDL_Window *win = SDL_CreateWindow("ZAD", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, fenetre_flags);
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Renderer *r = SDL_CreateRenderer(win, -1, 0);
    if (!r)
        r = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    if (!r) {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        return 1;
    }
    SDL_RenderSetLogicalSize(r, W, H);

    for (int i = 0; i < SDL_NumJoysticks(); i++)
        if (SDL_IsGameController(i))
            SDL_GameControllerOpen(i);

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
        SDL_SetRenderDrawColor(r, VIOLET, 255);
        SDL_RenderClear(r);

        SDL_SetRenderDrawColor(r, AMBRE, 255);
        dessiner_zad(r, W / 2 - (17 * 36) / 2, H / 2 - 7 * 36 / 2 - 40, 36);

        /* barre ambre qui avance : preuve que la boucle tourne */
        SDL_SetRenderDrawColor(r, CLAIR, 255);
        SDL_Rect rail = {W / 2 - 300, H - 160, 600, 12};
        SDL_RenderFillRect(r, &rail);
        SDL_SetRenderDrawColor(r, AMBRE, 255);
        SDL_Rect cur = {W / 2 - 300 + (int)((frame * 6) % 520), H - 164, 80, 20};
        SDL_RenderFillRect(r, &cur);

        SDL_RenderPresent(r);
        if (shot && frame == 3) {
            capture(r, shot);
            quitter = 1;
        }
        frame++;
        SDL_Delay(16);
    }
    SDL_Quit();
    return 0;
}
