#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "debugmalloc.h"
#include "strukturak.h"

void sorveg_levagas(char *s);

static void szelso_levag(char *s) {
    size_t start = 0;
    while (s[start] == ' ' || s[start] == '\t')
        start++;
    if (start > 0)
        memmove(s, s + start, strlen(s + start) + 1);

    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t' || s[len - 1] == '\r' || s[len - 1] == '\n')) {
        s[--len] = '\0';
    }
}

int beolvas_szavazok(const char *path, Szavazo **tomb_out, int *db_out) {
    FILE *fajl = fopen(path, "r");
    if (!fajl) {
        printf("Nem sikerult megnyitni a %s fajlt\n", path);
        return 0;
    }

    Szavazo *tomb = NULL;
    int db = 0, kapacitas = 0;

    char sor[512];
    while (fgets(sor, sizeof sor, fajl)) {
        sorveg_levagas(sor);
        if (sor[0] == '\0')
            continue;

        char *p1 = strchr(sor, '-');
        if (!p1) continue;
        char *p2 = strchr(p1 + 1, '-');
        if (!p2) continue;
        char *p3 = strchr(p2 + 1, '-');
        if (!p3) continue;

        char nev[128] = {0};
        char jelszo[64] = {0};
        char jelolt[128] = {0};
        char part[128] = {0};

        size_t len_nev = (size_t)(p1 - sor);
        if (len_nev >= sizeof nev)
            len_nev = sizeof nev - 1;
        strncpy(nev, sor, len_nev);
        nev[len_nev] = '\0';

        size_t len_jelszo = (size_t)(p2 - (p1 + 1));
        if (len_jelszo >= sizeof jelszo)
            len_jelszo = sizeof jelszo - 1;
        strncpy(jelszo, p1 + 1, len_jelszo);
        jelszo[len_jelszo] = '\0';

        size_t len_jelolt = (size_t)(p3 - (p2 + 1));
        if (len_jelolt >= sizeof jelolt)
            len_jelolt = sizeof jelolt - 1;
        strncpy(jelolt, p2 + 1, len_jelolt);
        jelolt[len_jelolt] = '\0';

        strncpy(part, p3 + 1, sizeof part - 1);
        part[sizeof part - 1] = '\0';

        szelso_levag(nev);
        szelso_levag(jelszo);
        szelso_levag(jelolt);
        szelso_levag(part);

        if (nev[0] == '\0' || jelszo[0] == '\0' || jelolt[0] == '\0' || part[0] == '\0') {
            continue;
        }

        if (db == kapacitas) {
            int uj_kapacitas = (kapacitas == 0) ? 8 : kapacitas * 2;
            Szavazo *uj = (Szavazo *)realloc(tomb, (size_t)uj_kapacitas * sizeof(Szavazo));
            if (!uj) {
                free(tomb);
                fclose(fajl);
                return 0;
            }
            tomb = uj;
            kapacitas = uj_kapacitas;
        }

        memset(&tomb[db], 0, sizeof tomb[db]);
        strncpy(tomb[db].nev, nev, sizeof tomb[db].nev - 1);
        tomb[db].nev[sizeof tomb[db].nev - 1] = '\0';
        strncpy(tomb[db].jelszo, jelszo, sizeof tomb[db].jelszo - 1);
        tomb[db].jelszo[sizeof tomb[db].jelszo - 1] = '\0';
        strncpy(tomb[db].jelolt, jelolt, sizeof tomb[db].jelolt - 1);
        tomb[db].jelolt[sizeof tomb[db].jelolt - 1] = '\0';
        strncpy(tomb[db].part, part, sizeof tomb[db].part - 1);
        tomb[db].part[sizeof tomb[db].part - 1] = '\0';
        db++;
    }

    fclose(fajl);
    *tomb_out = tomb;
    *db_out = db;
    return 1;
}

int beolvas_jeloltek(const char *path, Jelolt **tomb_out, int *db_out) {
    FILE *fajl = fopen(path, "r");
    if (!fajl) {
        printf("Nem sikerult megnyitni a %s fajlt\n", path);
        return 0;
    }

    Jelolt *tomb = NULL;
    int db = 0, kapacitas = 0;

    char sor[512];
    while (fgets(sor, sizeof sor, fajl)) {
        sorveg_levagas(sor);
        if (sor[0] == '\0') continue;

        char *elsoMinusz = strchr(sor, '-');
        if (!elsoMinusz) {
            continue;
        }

        char nev[128];
        char part[128];

        size_t nevLen = (size_t)(elsoMinusz - sor);
        if (nevLen >= sizeof nev)
            nevLen = sizeof nev - 1;
        strncpy(nev, sor, nevLen);
        nev[nevLen] = '\0';

        strncpy(part, elsoMinusz + 1, sizeof part - 1);
        part[sizeof part - 1] = '\0';

        szelso_levag(nev);
        szelso_levag(part);

        if (db == kapacitas) {
            int uj_kapacitas = kapacitas == 0 ? 8 : kapacitas * 2;
            Jelolt *ideiglenes = (Jelolt *)realloc(tomb, (size_t)uj_kapacitas * sizeof(Jelolt));
            if (!ideiglenes) {
                free(tomb);
                fclose(fajl);
                return 0;
            }
            tomb = ideiglenes;
            kapacitas = uj_kapacitas;
        }

        strncpy(tomb[db].nev, nev, sizeof tomb[db].nev - 1);
        tomb[db].nev[sizeof tomb[db].nev - 1] = '\0';
        strncpy(tomb[db].part, part, sizeof tomb[db].part - 1);
        tomb[db].part[sizeof tomb[db].part - 1] = '\0';
        db++;
    }

    fclose(fajl);
    *tomb_out = tomb;
    *db_out = db;
    return 1;
}

int beolvas_eredmeny(const char *path, Eredmeny **tomb_out, int *db_out) {
    FILE *fajl = fopen(path, "r");
    if (!fajl) {
        printf("Nem sikerult megnyitni a %s fajlt\n", path);
        return 0;
    }

    Eredmeny *tomb = NULL;
    int db = 0, kapacitas = 0;
    char sor[512];

    while (fgets(sor, sizeof sor, fajl)) {
        sorveg_levagas(sor);
        if (sor[0] == '\0')
            continue;

        char *elsoMinusz = strchr(sor, '-');
        char *utolsoMinusz = strrchr(sor, '-');
        if (!elsoMinusz || !utolsoMinusz || elsoMinusz == utolsoMinusz) {
            continue;
        }

        char nev[128];
        char part[128];
        char szavStr[32];

        size_t nevLen = (size_t)(elsoMinusz - sor);
        if (nevLen >= sizeof nev)
            nevLen = sizeof nev - 1;
        strncpy(nev, sor, nevLen);
        nev[nevLen] = '\0';

        size_t partLen = (size_t)(utolsoMinusz - (elsoMinusz + 1));
        if (partLen >= sizeof part)
            partLen = sizeof part - 1;
        strncpy(part, elsoMinusz + 1, partLen);
        part[partLen] = '\0';

        strncpy(szavStr, utolsoMinusz + 1, sizeof szavStr - 1);
        szavStr[sizeof szavStr - 1] = '\0';

        szelso_levag(nev);
        szelso_levag(part);
        szelso_levag(szavStr);

        int szavazat = atoi(szavStr);

        if (db == kapacitas) {
            int uj_kapacitas = kapacitas == 0 ? 8 : kapacitas * 2;
            Eredmeny *ideiglenes = (Eredmeny *)realloc(tomb, (size_t)uj_kapacitas * sizeof(Eredmeny));
            if (!ideiglenes) {
                free(tomb);
                fclose(fajl);
                return 0;
            }
            tomb = ideiglenes;
            kapacitas = uj_kapacitas;
        }

        strncpy(tomb[db].nev, nev, sizeof tomb[db].nev - 1);
        tomb[db].nev[sizeof tomb[db].nev - 1] = '\0';
        strncpy(tomb[db].part, part, sizeof tomb[db].part - 1);
        tomb[db].part[sizeof tomb[db].part - 1] = '\0';
        tomb[db].szavazat = szavazat;
        db++;
    }

    fclose(fajl);
    *tomb_out = tomb;
    *db_out = db;
    return 1;
}

int mentes_eredmeny(const char *path, const Eredmeny *tomb, int db) {
    FILE *fajl = fopen(path, "w");
    if (!fajl) {
        printf("Nem sikerult megnyitni a %s fajlt\n", path);
        return 0;
    }

    for (int i = 0; i < db; ++i) {
        fprintf(fajl, "%s-%s-%d\n", tomb[i].nev, tomb[i].part, tomb[i].szavazat);
    }

    fclose(fajl);
    return 1;
}

int mentes_uj_jelolt(const char *path, const Jelolt *j) {
    FILE *fajl = fopen(path, "a");
    if (!fajl) {
        printf("Nem sikerult megnyitni a %s fajlt\n", path);
        return 0;
    }

    fprintf(fajl, "%s-%s\n", j->nev, j->part);
    fclose(fajl);
    return 1;
}

int mentes_jeloltek(const char *path, const Jelolt *arr, int db) {
    FILE *f = fopen(path, "w");
    if (!f) {
        printf("Nem sikerult megnyitni a %s fajlt\n", path);
        return 0;
    }
    for (int i = 0; i < db; ++i) {
        fprintf(f, "%s-%s\n", arr[i].nev, arr[i].part);
    }
    fclose(f);
    return 1;
}

int mentes_szavazok(const char *path, const Szavazo *arr, int db) {
    FILE *f = fopen(path, "w");
    if (!f) {
        printf("Nem sikerult megnyitni a %s fajlt\n", path);
        return 0;
    }
    for (int i = 0; i < db; ++i) {
        fprintf(f, "%s-%s-%s-%s\n",
                arr[i].nev,
                arr[i].jelszo,
                arr[i].jelolt,
                arr[i].part);
    }
    fclose(f);
    return 1;
}

int beolvas_allapot(const char *path, int *allapot_out) {
    if (!allapot_out) return 0;
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Nem sikerult beolvasni az allapotot (%s)\n", path);
        return 0;
    }
    int a = 0;
    if (fscanf(f, "%d", &a) != 1) {
        fclose(f);
        fprintf(stderr, "Nem sikerult ertelmezni az allapot fajlt (%s)\n", path);
        return 0;
    }
    fclose(f);
    *allapot_out = (a != 0) ? 1 : 0;
    return 1;
}

int mentes_allapot(const char *path, int allapot) {
    FILE *f = fopen(path, "w");
    if (!f) return 0;
    fprintf(f, "%d\n", allapot ? 1 : 0);
    fclose(f);
    return 1;
}

int beolvas_uzenetek_adminnak(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        printf("Nem sikerult megnyitni a %s fajlt\n", path);
        return 0;
    }
    char sor[1024];
    int volt = 0;
    while (fgets(sor, sizeof sor, f)) {
        sorveg_levagas(sor);
        if (sor[0] == '\0') continue;
        printf("%s\n", sor);
        volt = 1;
    }
    if (!volt) {
        printf("(Nincsenek uzenetek)\n");
    }
    fclose(f);
    return 1;
}

int mentes_uzenet_adminnak(const char *path, const char *nev, const char *uzenet) {
    if (!nev || !uzenet || nev[0] == '\0' || uzenet[0] == '\0') return 0;
    FILE *f = fopen(path, "a");
    if (!f) {
        printf("Nem sikerult megnyitni a %s fajlt\n", path);
        return 0;
    }
    fprintf(f, "%s-%s\n", nev, uzenet);
    fclose(f);
    return 1;
}

int beolvas_uj_jeloltek_list(const char *path, JeloltLista **eleje_out) {
    if (!eleje_out) return 0;
    *eleje_out = NULL;
    FILE *f = fopen(path, "r");
    if (!f) {
        printf("Nem sikerult megnyitni a %s fajlt\n", path);
        return 0;
    }
    char sor[512];
    while (fgets(sor, sizeof sor, f)) {
        sorveg_levagas(sor);
        if (sor[0] == '\0') continue;
        char *minusz = strchr(sor, '-');
        if (!minusz) continue;
        char nev[128], part[128];
        size_t lenNev = (size_t)(minusz - sor);
        if (lenNev >= sizeof nev) lenNev = sizeof nev - 1;
        strncpy(nev, sor, lenNev);
        nev[lenNev] = '\0';
        strncpy(part, minusz + 1, sizeof part - 1);
        part[sizeof part - 1] = '\0';
        szelso_levag(nev);
        szelso_levag(part);
        Jelolt j = {0};
        strncpy(j.nev, nev, sizeof j.nev - 1);
        j.nev[sizeof j.nev - 1] = '\0';
        strncpy(j.part, part, sizeof j.part - 1);
        j.part[sizeof j.part - 1] = '\0';
        if (!jeloltlista_beszur_vegere(eleje_out, &j)) {
            jeloltlista_felszabadit(*eleje_out);
            *eleje_out = NULL;
            fclose(f);
            return 0;
        }
    }
    fclose(f);
    return 1;
}

int mentes_uj_jeloltek_list(const char *path, const JeloltLista *eleje) {
    FILE *f = fopen(path, "w");
    if (!f) {
        printf("Nem sikerult megnyitni a %s fajlt\n", path);
        return 0;
    }
    for (const JeloltLista *it = eleje; it; it = it->kov) {
        fprintf(f, "%s-%s\n", it->adat.nev, it->adat.part);
    }
    fclose(f);
    return 1;
}

int mentes_lezaras_idopont(const char *path) {
    char ido[32];
    if (!mai_ido_yyyy_mm_dd_hh_mm_ss(ido, sizeof ido)) {
        return 0;
    }
    FILE *f = fopen(path, "w");
    if (!f) return 0;
    fprintf(f, "%s\n", ido);
    fclose(f);
    return 1;
}

int beolvas_lezaras_idopont(const char *path, char *buf, size_t cap) {
    if (!buf || cap == 0) return 0;
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    if (!fgets(buf, (int)cap, f)) {
        fclose(f);
        return 0;
    }
    fclose(f);
    sorveg_levagas(buf);
    return buf[0] != '\0';
}
