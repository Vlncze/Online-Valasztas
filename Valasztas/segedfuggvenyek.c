#include "debugmalloc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "strukturak.h"

void sor_urites(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void sorveg_levagas(char *s) {
    size_t len = strlen(s);
    if (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
        if (len > 0 && (s[len - 1] == '\r')) {
            s[--len] = '\0';
        }
    }
}

static JeloltLista *jeloltlista_uj_node(const Jelolt *j) {
    JeloltLista *n = (JeloltLista *)malloc(sizeof(JeloltLista));
    if (!n) return NULL;
    n->kov = NULL;
    if (j) {
        n->adat = *j;
    } else {
        memset(&n->adat, 0, sizeof n->adat);
    }
    return n;
}

int jeloltlista_beszur_vegere(JeloltLista **eleje, const Jelolt *j) {
    if (!eleje || !j) return 0;
    JeloltLista *uj = jeloltlista_uj_node(j);
    if (!uj) return 0;
    if (*eleje == NULL) {
        *eleje = uj;
        return 1;
    }
    JeloltLista *it = *eleje;
    while (it->kov) it = it->kov;
    it->kov = uj;
    return 1;
}

int jeloltlista_torol_index(JeloltLista **eleje, int index) {
    if (!eleje || !*eleje || index < 0) return 0;
    JeloltLista *curr = *eleje;
    if (index == 0) {
        *eleje = curr->kov;
        free(curr);
        return 1;
    }
    JeloltLista *prev = NULL;
    int i = 0;
    while (curr && i < index) {
        prev = curr;
        curr = curr->kov;
        i++;
    }
    if (!curr) return 0;
    prev->kov = curr->kov;
    free(curr);
    return 1;
}

int jeloltlista_keres_nev(const JeloltLista *eleje, const char *nev) {
    int idx = 0;
    for (const JeloltLista *it = eleje; it; it = it->kov, ++idx) {
        if (strcmp(it->adat.nev, nev) == 0) return idx;
    }
    return -1;
}

void jeloltlista_felszabadit(JeloltLista *eleje) {
    JeloltLista *it = eleje;
    while (it) {
        JeloltLista *next = it->kov;
        free(it);
        it = next;
    }
}

int mai_ido_yyyy_mm_dd_hh_mm_ss(char *buf, size_t cap) {
    if (!buf || cap == 0) return 0;
    time_t now = time(NULL);
    if (now == (time_t)-1) return 0;
    struct tm *lt = localtime(&now);
    if (!lt) return 0;
    int n = snprintf(buf, cap, "%04d-%02d-%02d %02d:%02d:%02d",
                     lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
                     lt->tm_hour, lt->tm_min, lt->tm_sec);
    return (n > 0 && (size_t)n < cap) ? 1 : 0;
}

int lezaro_idopont_elmult(void) {
    struct tm cutoff = {0};
    cutoff.tm_year = 2026 - 1900;
    cutoff.tm_mon  = 4 - 1;
    cutoff.tm_mday = 5;
    cutoff.tm_hour = 12;
    cutoff.tm_min  = 0;
    cutoff.tm_sec  = 0;

    time_t cutoff_t = mktime(&cutoff);
    if (cutoff_t == (time_t)-1) {
        return 0;
    }

    time_t now = time(NULL);
    if (now == (time_t)-1) {
        return 0;
    }

    return now >= cutoff_t ? 1 : 0;
}
