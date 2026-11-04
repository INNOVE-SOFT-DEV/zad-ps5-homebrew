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

## 2. Version PS5 — `zad.elf` compilé et lié, **pas encore essayé sur console**

Le SDK (`ps5-payload-dev/sdk`) ne contient pas SDL2. Il se construit à part, depuis le portage
`ps5-payload-dev/SDL` ; `scripts/setup-ps5-sdl.sh` le fait pour vous (SDL en bibliothèque
statique, sans OpenGL ni Vulkan : rendu logiciel, suffisant pour ce projet) ainsi que
`libsamplerate`, dont SDL a besoin.

```
sudo apt-get install bash clang-18 lld-18 wget unzip cmake pkg-config git curl
wget https://github.com/ps5-payload-dev/sdk/releases/latest/download/ps5-payload-sdk.zip
sudo unzip -d /opt ps5-payload-sdk.zip
export PS5_PAYLOAD_SDK=/opt/ps5-payload-sdk
chmod +x $PS5_PAYLOAD_SDK/bin/*

./scripts/setup-ps5-sdl.sh                  # une seule fois
make ps5                                    # produit build/zad.elf
make test-ps5 PS5_HOST=192.168.x.x          # équivaut à : nc <IP> 9021 < build/zad.elf
```

## 3. ZAD dans « Contenu multimédia » (méthode retenue)

Un ELF envoyé au chargeur n'est pas une application : la console l'exécute mais n'affiche pas
son image (l'interface passe par-dessus). Les apps de l'onglet « Contenu multimédia »
(ShadowMount, SVT Play…) sont en réalité des **applications web** : `param.json` contient
`webAppUri`, l'adresse que la console ouvre en plein écran.

- `docs/` : la page ZAD pour la télé (HTML/CSS/JS sans dépendance, 1920×1080, manette :
  Croix = Entrée, Rond = Échap, flèches, Triangle = F1, Carré = F2). Données d'exemple dans
  `docs/js/data.js`. Publiée par **GitHub Pages** (branche `main`, dossier `/docs`) à
  `https://innove-soft-dev.github.io/zad-ps5-homebrew/`.
- `webapp/` : `zad-install.elf`, qui installe l'icône ZAD (titre `ZADW00001`) et enregistre cette
  adresse. À envoyer une seule fois au chargeur d'ELF ; l'app survit ensuite aux redémarrages.
  Code dérivé de `ps5-payload-dev/svtplay` (GPL-3.0+, voir `webapp/LICENSE-GPL3`).

```
make -C webapp                                  # produit webapp/zad-install.elf
make -C webapp install PS5_HOST=192.168.x.x
```

Tester la page sur PC : ouvrir `docs/index.html` (flèches, Entrée, Échap, Q/E).

## Problèmes courants

- `PS5_PAYLOAD_SDK est indefini` : exportez la variable (voir plus haut).
- `Lancez d'abord scripts/setup-ps5-sdl.sh` : SDL n'est pas encore construit.
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
