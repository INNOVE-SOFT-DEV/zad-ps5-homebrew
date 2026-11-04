/* Desinstalle le titre ZADA00001. */
#include "common.h"

int main(void)
{
    char m[200];
    journal("uninstaller: demarrage");
    charger("libSceIpmi.sprx");
    charger("libSceUserService.sprx");
    charger("libSceSystemService.sprx");
    void *inst = charger("libSceAppInstUtil.sprx");
    void *notif = charger("libSceNotification.sprx");
    int (*init)(void) = symbole(inst, "sceAppInstUtilInitialize");
    int (*desinstaller)(const char *) = symbole(inst, "sceAppInstUtilAppUnInstall");
    if (!init || !desinstaller)
        return -1;
    int err = init();
    snprintf(m, sizeof m, "Initialize retour %x", err);
    journal(m);
    err = desinstaller(TITLE_ID);
    snprintf(m, sizeof m, "UnInstall retour %x", err);
    journal(m);
    notifier(notif, err ? "ZAD: desinstallation echouee" : "ZAD desinstalle");
    return err ? -1 : 0;
}
