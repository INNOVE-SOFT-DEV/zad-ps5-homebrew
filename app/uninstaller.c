/* Desinstalle le titre ZADA00001. */
#include <stdio.h>
int sceAppInstUtilInitialize(void);
int sceAppInstUtilAppUnInstall(const char *);
int main(void)
{
    int err;
    if ((err = sceAppInstUtilInitialize())) { printf("sceAppInstUtilInitialize: %x\n", err); return -1; }
    if ((err = sceAppInstUtilAppUnInstall(TITLE_ID))) { printf("sceAppInstUtilAppUnInstall: %x\n", err); return -1; }
    printf("ZAD desinstalle\n");
    return 0;
}
