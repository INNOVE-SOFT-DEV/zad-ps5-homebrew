/* Installe le titre ZADA00001 (dossier deja depose dans /user/app/ et /system_ex/app/).
 * Ecrit /data/zad-install.log et affiche une notification avec le resultat. Base sur le SDK. */
#include <stdbool.h>
#include <stdio.h>

int sceAppInstUtilInitialize(void);
int sceAppInstUtilAppInstallTitleDir(const char *, const char *, void *);
int sceAppInstUtilAppExists(const char *, int *);
int sceNotificationSend(int userId, bool isLogged, const char *payload);

static void journal(const char *msg)
{
    FILE *f = fopen("/data/zad-install.log", "a");
    if (f) {
        fprintf(f, "%s\n", msg);
        fclose(f);
    }
    printf("%s\n", msg);
}

static void notifier(const char *texte)
{
    char json[1024];
    snprintf(json, sizeof json,
             "{\"rawData\":{\"viewTemplateType\":\"InteractiveToastTemplateB\","
             "\"channelType\":\"Downloads\",\"useCaseId\":\"IDC\","
             "\"toastOverwriteType\":\"No\",\"isImmediate\":true,\"priority\":100,"
             "\"viewData\":{\"icon\":{\"type\":\"Predefined\",\"parameters\":{\"icon\":\"download\"}},"
             "\"message\":{\"body\":\"%s\"}},"
             "\"platformViews\":{\"previewDisabled\":{\"viewData\":{\"icon\":{\"type\":\"Predefined\","
             "\"parameters\":{\"icon\":\"download\"}},\"message\":{\"body\":\"%s\"}}}}},"
             "\"createdDateTime\":\"2026-11-05T00:00:00.000Z\",\"localNotificationId\":\"4243\"}",
             texte, texte);
    sceNotificationSend(0xFE, false, json);
}

int main(void)
{
    char m[160];
    int err, existe = -1;

    journal("installer: demarrage");
    if ((err = sceAppInstUtilInitialize())) {
        snprintf(m, sizeof m, "installer: Initialize erreur %x", err);
        journal(m);
        notifier(m);
        return -1;
    }
    journal("installer: init ok");

    err = sceAppInstUtilAppInstallTitleDir(TITLE_ID, "/user/app/", 0);
    snprintf(m, sizeof m, "installer: InstallTitleDir retour %x", err);
    journal(m);

    int e2 = sceAppInstUtilAppExists(TITLE_ID, &existe);
    snprintf(m, sizeof m, "installer: Exists retour %x existe=%d", e2, existe);
    journal(m);

    if (err) {
        snprintf(m, sizeof m, "ZAD: installation echouee (code %x)", err);
    } else {
        snprintf(m, sizeof m, "ZAD installe (existe=%d)", existe);
    }
    notifier(m);
    return err ? -1 : 0;
}
