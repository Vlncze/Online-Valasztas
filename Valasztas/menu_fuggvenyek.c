#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "debugmalloc.h"
#include "strukturak.h"

int kerdezz_sor(const char *prompt, char *buf, size_t cap) {
    printf("%s", prompt);
    if (!fgets(buf, (int)cap, stdin)) {
        return 0;
    }
    sorveg_levagas(buf);
    return 1;
}

int keres_szavazo_index(const Szavazo *t, int db, const char *nev) {
    for (int i = 0; i < db; ++i) {
        if (strcmp(t[i].nev, nev) == 0)
            return i;
    }
    return -1;
}

int ellenoriz_es_biztosit_jelszo(Szavazo *s) {
    if ((s->jelszo[0] == '\0' || strcmp(s->jelszo, "--") == 0)) {
        char uj_jelszo[64];
        if (!kerdezz_sor("Meg nem rendelkezel jelszoval. Hozz letre egyet: ", uj_jelszo, sizeof uj_jelszo)) {
            printf("ERROR: nem sikerult beolvasni a jelszot\n");
            return 0;
        }
        if (uj_jelszo[0] == '\0') {
            printf("ERROR: ures jelszo nem engedelyezett\n");
            return 0;
        }
        strncpy(s->jelszo, uj_jelszo, sizeof s->jelszo - 1);
        s->jelszo[sizeof s->jelszo - 1] = '\0';
        return 1;
    } else {
        char bejelszo[64];
        if (!kerdezz_sor("Add meg a JELSZOT: ", bejelszo, sizeof bejelszo)) {
            printf("ERROR: nem sikerult beolvasni a jelszot\n");
            return 0;
        }
        if (strcmp(bejelszo, s->jelszo) != 0) {
            printf("Hibas jelszo.\n");
            return 0;
        }
        return 1;
    }
}

void szavazas_leadasa(void) {
    printf("Szavazas leadasa\n");
    printf("-------------------------------------\n\n");

    Szavazo *szavazok = NULL;
    int szav_db = 0;
    if (!beolvas_szavazok("files/szavazok.txt", &szavazok, &szav_db)) {
        printf("ERROR: a szavazok beolvasasa sikertelen \n");
        return;
    }

    char nev[128];
    if (!kerdezz_sor("Add meg a NEV-et: ", nev, sizeof nev)) {
        printf("ERROR: nem sikerult beolvasni a nevet\n");
        free(szavazok);
        return;
    }
    if (nev[0] == '\0') {
        printf("ERROR: ures nev nem ervenyes\n");
        free(szavazok);
        return;
    }

    int idx = keres_szavazo_index(szavazok, szav_db, nev);

    if (idx < 0) {
        char uj_jelszo[64];
        if (!kerdezz_sor("Uj szavazo. Adj meg egy JELSZOT a fiokodhoz: ", uj_jelszo, sizeof uj_jelszo)) {
            printf("ERROR: nem sikerult beolvasni a jelszot\n");
            free(szavazok);
            return;
        }
        if (uj_jelszo[0] == '\0') {
            printf("ERROR: ures jelszo nem engedelyezett\n");
            free(szavazok);
            return;
        }

        Szavazo *ujt = realloc(szavazok, (size_t)(szav_db + 1) * sizeof(Szavazo));
        
        szavazok = ujt;
        idx = szav_db;
        szav_db += 1;

        memset(&szavazok[idx], 0, sizeof(Szavazo));
        strncpy(szavazok[idx].nev, nev, sizeof szavazok[idx].nev - 1);
        szavazok[idx].nev[sizeof szavazok[idx].nev - 1] = '\0';
        strncpy(szavazok[idx].jelszo, uj_jelszo, sizeof szavazok[idx].jelszo - 1);
        szavazok[idx].jelszo[sizeof szavazok[idx].jelszo - 1] = '\0';
        szavazok[idx].jelolt[0] = '\0';
        szavazok[idx].part[0]   = '\0';

        printf("Fiok letrehozva. Folytathatod a szavazast.\n");
    } else {
        if (!(szavazok[idx].jelolt[0] == '\0' && szavazok[idx].part[0] == '\0')) {
            printf("Nem szavazhatsz, mert mar leadtad a szavazatod.\n");
            free(szavazok);
            return;
        }
        if (!ellenoriz_es_biztosit_jelszo(&szavazok[idx])) {
            free(szavazok);
            return;
        }
    }

    Jelolt *jeloltek = NULL;
    int jelolt_db = 0;
    if (!beolvas_jeloltek("files/jeloltek.txt", &jeloltek, &jelolt_db)) {
        printf("ERROR: a jeloltek beolvasasa sikertelen \n");
        free(jeloltek);
        free(szavazok);
        return;
    }
    if (jelolt_db == 0) {
        printf("ERROR: nincs jelolt a fajlban \n");
        free(jeloltek);
        free(szavazok);
        return;
    }
    printf("Jeloltek:\n");
    for (int i = 0; i < jelolt_db; ++i) {
        printf("- %s - %s\n", jeloltek[i].nev, jeloltek[i].part);
    }

    char valasztott_jelolt[128];
    if (!kerdezz_sor("\nValassz egy jeloltet (NEV szerint): ", valasztott_jelolt, sizeof valasztott_jelolt)) {
        printf("ERROR: nem sikerult beolvasni a valasztott jelolt nevet\n");
        free(jeloltek);
        free(szavazok);
        return;
    }
    if (valasztott_jelolt[0] == '\0') {
        printf("ERROR: ures jeloltnev nem ervenyes\n");
        free(jeloltek);
        free(szavazok);
        return;
    }

    char valasztott_part[128] = "";
    int letezo = 0;
    for (int i = 0; i < jelolt_db; ++i) {
        if (strcmp(jeloltek[i].nev, valasztott_jelolt) == 0) {
            strncpy(valasztott_part, jeloltek[i].part, sizeof valasztott_part - 1);
            valasztott_part[sizeof valasztott_part - 1] = '\0';
            letezo = 1;
            break;
        }
    }
    if (!letezo) {
        printf("ERROR: nincs ilyen jelolt a listaban\n");
        free(jeloltek);
        free(szavazok);
        return;
    }

    Eredmeny *eredm = NULL;
    int eredm_db = 0;
    if (!beolvas_eredmeny("files/eredmeny.txt", &eredm, &eredm_db)) {
        printf("ERROR: az eredmeny beolvasasa sikertelen \n");
        free(jeloltek);
        free(szavazok);
        return;
    }
    if (eredm_db == 0) {
        printf("ERROR: ures eredmeny fajl \n");
        free(jeloltek);
        free(eredm);
        free(szavazok);
        return;
    }

    int talalat = 0;
    for (int i = 0; i < eredm_db; ++i) {
        if (strcmp(eredm[i].nev, valasztott_jelolt) == 0) {
            eredm[i].szavazat += 1;
            talalat = 1;
            break;
        }
    }
    if (!talalat) {
        printf("ERROR: nincs ilyen jelolt az eredmenyben \n");
        free(jeloltek);
        free(eredm);
        free(szavazok);
        return;
    }

    if (!mentes_eredmeny("files/eredmeny.txt", eredm, eredm_db)) {
        printf("ERROR: nem sikerult az eredmenyt a fajlba irni\n");
        free(jeloltek);
        free(eredm);
        free(szavazok);
        return;
    }

    strncpy(szavazok[idx].jelolt, valasztott_jelolt, sizeof szavazok[idx].jelolt - 1);
    szavazok[idx].jelolt[sizeof szavazok[idx].jelolt - 1] = '\0';
    strncpy(szavazok[idx].part, valasztott_part, sizeof szavazok[idx].part - 1);
    szavazok[idx].part[sizeof szavazok[idx].part - 1] = '\0';

    if (!mentes_szavazok("files/szavazok.txt", szavazok, szav_db)) {
        printf("ERROR: nem sikerult a szavazot fajlba menteni\n");
        free(jeloltek);
        free(eredm);
        free(szavazok);
        return;
    }

    printf("\nSzavazat leadva a kovetkezo jeloltre: %s\n", valasztott_jelolt);
    printf("=====================================\n");
    printf("Koszonjuk, hogy szavaztal!\n");

    free(jeloltek);
    free(eredm);
    free(szavazok);
}

void statisztika_megtekintese(void) {
    printf("Statisztika megtekintese\n");
    printf("-------------------------------------\n");

    Eredmeny *eredm = NULL;
    int eredm_db = 0;
    if (!beolvas_eredmeny("files/eredmeny.txt", &eredm, &eredm_db)) {
        printf("ERROR: az eredmeny beolvasasa sikertelen \n");
        return;
    }

    if (eredm_db == 0) {
        printf("Nincs megjelenitheto eredmeny \n");
        free(eredm);
        return;
    }

    printf("Eredmenyek:\n");
    for (int i = 0; i < eredm_db; ++i) {
        printf("%s - %s - %d\n", eredm[i].nev, eredm[i].part, eredm[i].szavazat);
    }

    free(eredm);
}

void szavazatom_megtekintese(void) {
    printf("Szavaztom:\n");

    char keresett_nev[128];
    if (!kerdezz_sor("Add meg a NEV-et: ", keresett_nev, sizeof keresett_nev)) {
        printf("ERROR: nem sikerult beolvasni a nevet\n");
        return;
    }
    if (keresett_nev[0] == '\0') {
        printf("ERROR: ures nev nem ervenyes\n");
        return;
    }

    Szavazo *tomb = NULL;
    int db = 0;

    if (!beolvas_szavazok("files/szavazok.txt", &tomb, &db)) {
        printf("ERROR: a szavazok beolvasasa sikertelen \n");
        return;
    }
    if (db == 0) {
        printf("Nincs szavazo \n");
        free(tomb);
        return;
    }

    int talalt = 0;
    for (int i = 0; i < db; ++i) {
        if (strcmp(tomb[i].nev, keresett_nev) == 0) {
            if (tomb[i].jelszo[0] == '\0' || strcmp(tomb[i].jelszo, "--") == 0) {
                printf("Nincs meg jelszod beallitva ehhez a fiokhoz. A megtekinteshez allits be jelszot a szavazas leadasa menupontban.\n");
                talalt = 1;
                break;
            } else {
                char bejelszo[64];
                if (!kerdezz_sor("Add meg a JELSZOT: ", bejelszo, sizeof bejelszo)) {
                    printf("ERROR: nem sikerult beolvasni a jelszot\n");
                    talalt = 1;
                    break;
                }
                if (strcmp(bejelszo, tomb[i].jelszo) != 0) {
                    printf("Hibas jelszo.\n");
                    talalt = 1;
                    break;
                }
                printf("\nNEV: %s\n", tomb[i].nev);
                printf("Jelolt: %s\n", tomb[i].jelolt[0] ? tomb[i].jelolt : "--");
                printf("Part: %s\n", tomb[i].part[0] ? tomb[i].part : "--");
                talalt = 1;
                break;
            }
        }
    }

    if (!talalt) {
        printf("\nNincs ilyen nevu szavazo: %s\n", keresett_nev);
    }

    free(tomb);
}

void jelolt_jelentkezes(void) {
    char uj_nev[128];
    char uj_part[128];

    if (!kerdezz_sor("Add meg a NEV-et: ", uj_nev, sizeof uj_nev)) {
        printf("ERROR: nem sikerult beolvasni a nevet\n");
        return;
    }
    if (!kerdezz_sor("Add meg a PART-ot: ", uj_part, sizeof uj_part)) {
        printf("ERROR: nem sikerult beolvasni a part nevet\n");
        return;
    }
    if (uj_nev[0] == '\0' || uj_part[0] == '\0') {
        printf("ERROR: nev es part nem lehet ures\n");
        return;
    }

    Jelolt *jeloltek = NULL;
    int jelolt_db = 0;
    if (!beolvas_jeloltek("files/jeloltek.txt", &jeloltek, &jelolt_db)) {
        printf("ERROR: a jeloltek beolvasasa sikertelen \n");
        free(jeloltek);
        return;
    }

    int dupla = 0;
    for (int i = 0; i < jelolt_db; ++i) {
        if (strcmp(jeloltek[i].nev, uj_nev) == 0) {
            dupla = 1;
            break;
        }
    }
    free(jeloltek);

    if (dupla) {
        printf("Mar letezik ilyen nevu jelolt\n");
        return;
    }

    Jelolt uj = (Jelolt){0};
    strncpy(uj.nev, uj_nev, sizeof uj.nev - 1);
    uj.nev[sizeof uj.nev - 1] = '\0';
    strncpy(uj.part, uj_part, sizeof uj.part - 1);
    uj.part[sizeof uj.part - 1] = '\0';

    if (!mentes_uj_jelolt("files/uj_jelolt.txt", &uj)) {
        printf("ERROR: nem sikerult hozzaadni a varolistahoz \n");
    } else {
        printf("Sikeresen hozzaadva a varolistahoz: %s - %s\n", uj.nev, uj.part);
    }
}

void uzenet_adminnak(void) {
    printf("Uzenet az adminnak\n");
    printf("-------------------------------------\n");
    char nev[128];
    char uzenet[896];

    if (!kerdezz_sor("Add meg a NEV-et: ", nev, sizeof nev)) {
        printf("ERROR: nem sikerult beolvasni a nevet\n");
        return;
    }
    if (nev[0] == '\0') {
        printf("ERROR: ures nev nem ervenyes\n");
        return;
    }

    if (!kerdezz_sor("Ird be az UZENET-et (egy sorban): ", uzenet, sizeof uzenet)) {
        printf("ERROR: nem sikerult beolvasni az uzenetet\n");
        return;
    }
    if (uzenet[0] == '\0') {
        printf("ERROR: ures uzenet nem ervenyes\n");
        return;
    }

    mentes_uzenet_adminnak("files/uzenetek_adminnak.txt", nev, uzenet);

    printf("Uzenet elkuldve az adminnak.\n");
}

int mainmenu(int allapot) {
    printf("\n=========== FOMENU ===========\n");
    printf("Allapot: %s\n", allapot ? "ZART" : "NYITOTT");
    printf("1) Szavazas leadasa\n");
    printf("2) Statisztika megtekintese\n");
    printf("3) Szavazatom megtekintese\n");
    printf("4) Jelolt jelentkezes\n");
    printf("5) Admin felulet\n");
    printf("6) Uzenet az adminnak\n");
    printf("0) Kilepes\n");
    printf("Valasztas: ");

    int v = -1;
    if (scanf("%d", &v) != 1) {
        sor_urites();
        printf("Ervenytelen bevitel. Kerlek SZAM-ot adj meg.\n");
        return -1;
    }
    return v;
}
