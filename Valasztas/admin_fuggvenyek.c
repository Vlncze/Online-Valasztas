#include "debugmalloc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "strukturak.h"

void admin_menu_kiir(void) {
    printf("----------- ADMIN -----------\n");
    printf("1. Szavazok kezelese\n");
    printf("2. Jelolt jelentkezesek\n");
    printf("3. Uzenetek megtekintese\n");
    printf("4. Valasztas allapota\n");
    printf("0. Vissza a fomenube\n");
    printf("-----------------------------\n");
    printf("Valassz egy MENUPONTOT: ");
}

void admin_szavazok_kezelese(void) {
    Szavazo *t = NULL;
    int db = 0;
    if (!beolvas_szavazok("files/szavazok.txt", &t, &db)) {
        printf("ERROR: a szavazok beolvasasa sikertelen\n");
        return;
    }
    while (true) {
        printf("\n--- Szavazok kezelese ---\n");
        printf("Aktualis szavazok:\n");
        for (int i = 0; i < db; ++i) {
            printf("%2d) %s\n", i + 1, t[i].nev);
        }
        printf("Add meg a torlendo szavazo NEV-et, vagy ird be hogy 'vissza' a kilepeshez.\n");
        printf("Bemenet (NEV): ");

        char bevitel[256];
        if (!fgets(bevitel, (int)sizeof bevitel, stdin)) {
            printf("Hiba a bemenet olvasasakor.\n");
            continue;
        }
        sorveg_levagas(bevitel);

        if (strcmp(bevitel, "vissza") == 0) {
            break;
        }
        if (bevitel[0] == '\0') {
            printf("Ures nev nem ervenyes.\n");
            continue;
        }

        int idx = -1;
        for (int i = 0; i < db; ++i) {
            if (strcmp(t[i].nev, bevitel) == 0) {
                idx = i;
                break;
            }
        }
        if (idx < 0) {
            printf("Nincs ilyen szavazo: %s\n", bevitel);
            continue;
        }

        for (int i = idx + 1; i < db; ++i){
            t[i - 1] = t[i];
        }
        db--;
        if (db == 0) {
            free(t);
            t = NULL;
        }

        mentes_szavazok("files/szavazok.txt", t, db);
        printf("Szavazo torolve: %s\n", bevitel);
    }
    free(t);
}

void admin_jelolt_jelentkezesek(void) {
    JeloltLista *lista = NULL;
    if (!beolvas_uj_jeloltek_list("files/uj_jelolt.txt", &lista)) {
        printf("ERROR: a jelentkezesek beolvasasa sikertelen\n");
        return;
    }
    while (true){
        printf("\n--- Jelolt jelentkezesek ---\n");
        int vdb = 0;
        if (lista == NULL) {
            printf("(Nincs jelentkezes)\n");
        } else {
            const JeloltLista *it = lista;
            while (it) {
                printf("%2d) %s - %s\n", vdb + 1, it->adat.nev, it->adat.part);
                vdb++;
                it = it->kov;
            }
        }
        printf("e) ELFOGADAS SORSZAM-mal\n");
        printf("x) ELUTASITAS SORSZAM-mal\n");
        printf("v) Vissza\n");
        printf("Valasztas: ");
        char val[8];
        if (scanf("%7s", val) != 1) {
            sor_urites();
            printf("Ervenytelen bevitel. Valassz: e (ELFOGADAS), x (ELUTASITAS), v (VISSZA)\n");
            continue;
        }
        sor_urites();

        if (strcmp(val, "e") == 0) {
            int idx = -1;
            printf("ELFOGADANDO SORSZAM: ");
            if (scanf("%d", &idx) != 1) { 
                sor_urites(); 
                printf("Hibas SORSZAM. Adj meg egy ervenyes SZAM-ot.\n"); 
                continue; 
            }
            sor_urites();
            idx -= 1;
            if (idx < 0) { 
                printf("Ervenytelen SORSZAM\n"); 
                continue; 
            }
            int i = 0;
            Jelolt jel = {0};
            const JeloltLista *it = lista;
            while (it && i < idx) { it = it->kov; i++; }
            if (!it) {
                printf("Ervenytelen SORSZAM\n");
                continue; }
            jel = it->adat;

            Jelolt *jelT = NULL; int jdb = 0;
            if (beolvas_jeloltek("files/jeloltek.txt", &jelT, &jdb)) {
                int dupla = 0;
                for (int k = 0; k < jdb; ++k) if (strcmp(jelT[k].nev, jel.nev) == 0) { dupla = 1; break; }
                if (!dupla) {
                    Jelolt *uj = realloc(jelT, (size_t)(jdb + 1) * sizeof(Jelolt));
                    if (uj) {
                        jelT = uj;
                        jelT[jdb] = jel;
                        jdb++;
                        if (!mentes_jeloltek("files/jeloltek.txt", jelT, jdb)) {
                            printf("FIGYELEM: nem sikerult a jeloltekhez hozzaadni\n");
                        }
                    }
                }
                free(jelT);
            }

            Eredmeny *ered = NULL; int edb = 0;
            if (beolvas_eredmeny("files/eredmeny.txt", &ered, &edb)) {
                int dupla = 0;
                for (int k = 0; k < edb; ++k) if (strcmp(ered[k].nev, jel.nev) == 0) { dupla = 1; break; }
                if (!dupla) {
                    Eredmeny *ujE = realloc(ered, (size_t)(edb + 1) * sizeof(Eredmeny));
                    if (ujE) {
                        ered = ujE;
                        strncpy(ered[edb].nev, jel.nev, sizeof ered[edb].nev - 1);
                        ered[edb].nev[sizeof ered[edb].nev - 1] = '\0';
                        strncpy(ered[edb].part, jel.part, sizeof ered[edb].part - 1);
                        ered[edb].part[sizeof ered[edb].part - 1] = '\0';
                        ered[edb].szavazat = 0;
                        edb++;
                        if (!mentes_eredmeny("files/eredmeny.txt", ered, edb)) {
                            printf("FIGYELEM: nem sikerult az eredmenyhez hozzaadni\n");
                        }
                    }
                }
                free(ered);
            }

            if (!jeloltlista_torol_index(&lista, idx)) {
                printf("FIGYELEM: nem sikerult a varolistabol torolni\n");
            } else {
                if (!mentes_uj_jeloltek_list("files/uj_jelolt.txt", lista)) {
                    printf("FIGYELEM: nem sikerult a varolista frissitese\n");
                } else {
                    printf("Jelentkezes ELFOGADVA.\n");
                }
            }

        } else if (strcmp(val, "x") == 0) {
            int idx = -1;
            printf("ELUTASITANDO SORSZAM: ");
            if (scanf("%d", &idx) != 1) { 
                sor_urites(); 
                printf("Hibas SORSZAM. Adj meg egy ervenyes SZAM-ot.\n"); 
                continue; 
            }
            sor_urites();
            idx -= 1;
            if (idx < 0) { printf("Ervenytelen SORSZAM\n"); continue; }

            if (!jeloltlista_torol_index(&lista, idx)) {
                printf("FIGYELEM: nem sikerult a varolistabol torolni\n");
            } else {
                if (!mentes_uj_jeloltek_list("files/uj_jelolt.txt", lista)) {
                    printf("FIGYELEM: nem sikerult a varolista frissitese\n");
                } else {
                    printf("Jelentkezes ELUTASITVA.\n");
                }
            }

        } else if (strcmp(val, "v") == 0) {
            break;
        } else {
            printf("Ismeretlen valasztas. Valassz: e (ELFOGADAS), x (ELUTASITAS), v (VISSZA)\n");
        }
    }
    jeloltlista_felszabadit(lista);
}

void admin_uzenetek_megtekintese(void) {
    printf("\n--- Uzenetek adminnak ---\n");
    beolvas_uzenetek_adminnak("files/uzenetek_adminnak.txt");
}

void admin_allapot(int *allapot) {
    printf("\n--- Valasztas allapota ---\n");
    int fajl_allapot = 0;
    if (beolvas_allapot("files/allapot.txt", &fajl_allapot)) {
        printf("Fajl szerinti allapot: %s\n", fajl_allapot ? "ZART" : "NYITOTT");
    } else {
        printf("Allapot fajl nem elerheto, futasi allapot: %s\n", *allapot ? "ZART" : "NYITOTT");
    }
    printf("Jelenlegi futasi allapot: %s\n", *allapot ? "ZART" : "NYITOTT");
    printf("a) Allapot valtasa\n");
    printf("v) Vissza\n");
    printf("Valasztas: ");
    char val[8];
    if (scanf("%7s", val) != 1) { 
        sor_urites(); 
        printf("Ervenytelen bevitel. Valassz: a (ALLAPOT VALTASA), v (VISSZA)\n");
        return; 
    }
    sor_urites();
    if (strcmp(val, "a") == 0) {
        *allapot = (*allapot == 0) ? 1 : 0;

        if (!mentes_allapot("files/allapot.txt", *allapot)) {
            printf("FIGYELEM: allapot mentese nem sikerult\n");
        } else {
            printf("Allapot mentve: %s\n", *allapot ? "ZART" : "NYITOTT");
        }
    } else if (strcmp(val, "v") == 0) {
        
    } else {
        printf("Ismeretlen valasztas. Valassz: a (ALLAPOT VALTASA), v (VISSZA)\n");
    }
}
