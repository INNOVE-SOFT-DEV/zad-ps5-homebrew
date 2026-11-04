/* Outils communs installeur / désinstalleur : bibliothèques système chargées à l'exécution
 * (dlopen), pour que chaque étape soit journalisée dans /data/zad-install.log. */
#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>

static void journal(const char *msg)
{
    FILE *f = fopen("/data/zad-install.log", "a");
    if (f) {
        fprintf(f, "%s\n", msg);
        fflush(f);
        fclose(f);
    }
    printf("%s\n", msg);
}

static void *charger(const char *nom)
{
    char m[200];
    void *h = dlopen(nom, RTLD_LAZY);
    snprintf(m, sizeof m, "dlopen %s : %s", nom, h ? "ok" : dlerror());
    journal(m);
    return h;
}

static void *symbole(void *h, const char *nom)
{
    char m[200];
    void *s = h ? dlsym(h, nom) : 0;
    snprintf(m, sizeof m, "dlsym %s : %s", nom, s ? "ok" : "introuvable");
    journal(m);
    return s;
}

static void notifier(void *libnotif, const char *texte)
{
    int (*envoyer)(int, bool, const char *) = symbole(libnotif, "sceNotificationSend");
    if (!envoyer)
        return;
    char json[1024];
    snprintf(json, sizeof json,
             "{\"rawData\":{\"viewTemplateType\":\"InteractiveToastTemplateB\","
             "\"channelType\":\"Downloads\",\"useCaseId\":\"IDC\","
             "\"toastOverwriteType\":\"No\",\"isImmediate\":true,\"priority\":100,"
             "\"viewData\":{\"icon\":{\"type\":\"Predefined\",\"parameters\":{\"icon\":\"download\"}},"
             "\"message\":{\"body\":\"%s\"}},"
             "\"platformViews\":{\"previewDisabled\":{\"viewData\":{\"icon\":{\"type\":\"Predefined\","
             "\"parameters\":{\"icon\":\"download\"}},\"message\":{\"body\":\"%s\"}}}}},"
             "\"createdDateTime\":\"2026-11-05T00:00:00.000Z\",\"localNotificationId\":\"4244\"}",
             texte, texte);
    envoyer(0xFE, false, json);
}
