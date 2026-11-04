# zad-ps5-homebrew

Version PS5 (homebrew) de l'app ZAD, en C et SDL2. Étape actuelle : **hello world**
(écran violet `#2B2170`, lettres « ZAD » en ambre `#F2A007`, barre qui avance pour prouver
que la boucle tourne). L'app complète (Découvrir, Détail, Réservations, Profil) viendra
après validation de cette étape sur la console.

Le même fichier `src/main.c` sert pour le PC et la PS5 ; seul `-DPLATFORM_PS5` change
(plein écran).

## 1. Version PC (Linux ou WSL) — testée

```
sudo apt-get install build-essential libsdl2-dev
make pc
./build/zad-pc
```

Échap ou Q quitte. Sur manette, le bouton Options (START pour SDL) quitte aussi.

## 2. Version PS5 — **non testée, voir les réserves**

Le PS5 Payload SDK (`ps5-payload-dev/sdk`) fournit la chaîne de compilation, **mais pas SDL2** :
j'ai téléchargé et ouvert l'archive officielle, elle contient `libc`, `libc++` et les
bibliothèques système, aucun SDL. Il faut donc installer un portage SDL2 pour PS5 et le
rendre visible à `prospero-pkg-config` (le Makefile appelle
`prospero-pkg-config --cflags --libs sdl2`). Je n'ai pas pu vérifier ici où se trouve ce
portage ni comment l'installer : à confirmer dans la documentation du SDK avant de
continuer.

Installation du SDK (Debian/Ubuntu/WSL) :

```
sudo apt-get install bash clang-18 lld-18 wget unzip
wget https://github.com/ps5-payload-dev/sdk/releases/latest/download/ps5-payload-sdk.zip
sudo unzip -d /opt ps5-payload-sdk.zip
export PS5_PAYLOAD_SDK=/opt/ps5-payload-sdk
```

Compilation et envoi (le chargeur d'ELF écoute sur le port 9021) :

```
make ps5
make test-ps5 PS5_HOST=192.168.x.x     # équivaut à : nc <IP> 9021 < build/zad.elf
```

## Problèmes courants

- `PS5_PAYLOAD_SDK est indefini` : exportez la variable (voir plus haut).
- Erreur sur `sdl2` à `make ps5` : le portage SDL2 PS5 n'est pas installé ou pas trouvé
  par `prospero-pkg-config`.
- Écran noir sur la console : essayez de retirer `SDL_WINDOW_FULLSCREEN` dans `src/main.c`,
  ou de forcer le rendu logiciel (déjà utilisé en repli).
- Manette non détectée : le hello world n'en a pas besoin pour démarrer, mais Options
  ne quittera pas ; testez à partir de l'écran suivant.
- Rien ne se lance : vérifiez que le chargeur d'ELF tourne sur la console et que votre
  firmware est couvert par votre jailbreak.

## Police et arabe

Pas de police dans le hello world (lettres dessinées à la main). Plus tard :
Plus Jakarta Sans (licence OFL) via SDL2_ttf, embarquée dans l'ELF, et l'arabe avec
HarfBuzz/FriBidi.
