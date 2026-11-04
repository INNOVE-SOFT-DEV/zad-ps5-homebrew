/* Installe le titre ZADA00001 (dossier deja depose dans /user/app/). Base sur l'exemple du SDK. */
#include <stdio.h>
int sceAppInstUtilInitialize(void);
int sceAppInstUtilAppInstallTitleDir(const char *, const char *, void *);
int main(void)
{
    int err;
    if ((err = sceAppInstUtilInitialize())) { printf("sceAppInstUtilInitialize: %x\n", err); return -1; }
    if ((err = sceAppInstUtilAppInstallTitleDir(TITLE_ID, "/user/app/", 0))) {
        printf("sceAppInstUtilAppInstallTitleDir: %x\n", err);
        return -1;
    }
    printf("ZAD installe\n");
    return 0;
}
