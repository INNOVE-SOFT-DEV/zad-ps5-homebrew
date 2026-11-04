/* Installe le titre ZADA00001 (dossiers deja deposes par FTP). Base sur samples/install_app du SDK. */
#include "common.h"

int main(void)
{
    char m[200];
    journal("installer v3: demarrage");
    void *ipmi = charger("libSceIpmi.sprx");
    void *user = charger("libSceUserService.sprx");
    void *sys = charger("libSceSystemService.sprx");
    void *inst = charger("libSceAppInstUtil.sprx");
    void *notif = charger("libSceNotification.sprx");
    (void)ipmi; (void)user; (void)sys;

    int (*init)(void) = symbole(inst, "sceAppInstUtilInitialize");
    int (*install)(const char *, const char *, void *) = symbole(inst, "sceAppInstUtilAppInstallTitleDir");
    int (*existe)(const char *, int *) = symbole(inst, "sceAppInstUtilAppExists");
    if (!init || !install) {
        notifier(notif, "ZAD: bibliotheque d'installation introuvable");
        return -1;
    }
    int err = init();
    snprintf(m, sizeof m, "Initialize retour %x", err);
    journal(m);
    if (err) {
        snprintf(m, sizeof m, "ZAD: init erreur %x", err);
        notifier(notif, m);
        return -1;
    }
    err = install(TITLE_ID, "/user/app/", 0);
    snprintf(m, sizeof m, "InstallTitleDir retour %x", err);
    journal(m);
    int ex = -1, e2 = existe ? existe(TITLE_ID, &ex) : -2;
    snprintf(m, sizeof m, "Exists retour %x existe=%d", e2, ex);
    journal(m);
    snprintf(m, sizeof m, err ? "ZAD: installation echouee (%x)" : "ZAD installe (existe=%d)", err ? err : ex);
    notifier(notif, m);
    return err ? -1 : 0;
}
