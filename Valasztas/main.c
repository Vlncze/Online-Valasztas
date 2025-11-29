#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "debugmalloc.h"
#include "strukturak.h"

static void admin_felulet(int *allapot) {
    char jelszo[128];
    if (!kerdezz_sor("Add meg az ADMIN JELSZOT: ", jelszo, sizeof jelszo)) {
        printf("Hiba a jelszo beolvasasakor.\n");
        return;
    }
    if (strcmp(jelszo, "adminpass") != 0) {
        printf("Hibas jelszo.\n");
        return;
    }

    while (true) {
        admin_menu_kiir();
        int v = -1;
        if (scanf("%d", &v) != 1) {
            sor_urites();
            printf("Ervenytelen bevitel. Kerlek 0–4 kozott valassz.\n");
            continue;
        }
        sor_urites();
        if (v == 0) break;
        switch (v) {
            case 1: admin_szavazok_kezelese(); break;
            case 2: admin_jelolt_jelentkezesek(); break;
            case 3: admin_uzenetek_megtekintese(); break;
            case 4: admin_allapot(allapot); break;
            default: printf("Kerlek 0–4 kozott valassz.\n"); break;
        }
    }
}

int main(void) {
    int allapot = 0;
    beolvas_allapot("files/allapot.txt", &allapot);

    int input = 0;

    while (true) {
        input = mainmenu(allapot);
        sor_urites();

        switch (input) {
            case 1:
                szavazas_leadasa();
                break;

            case 2:
                statisztika_megtekintese();
                break;

            case 3:
                szavazatom_megtekintese();
                break;

            case 4:
                jelolt_jelentkezes();
                break;

            case 5:
                admin_felulet(&allapot);
                break;

            case 6:
                uzenet_adminnak();
                break;

            case 0:
                printf("Kilepes...\n");
                return 0;

            default:
                printf("Kerlek 0–6 kozott valassz.\n");
                break;
        }
    }

    return 0;
}
