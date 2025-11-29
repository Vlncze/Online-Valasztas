#define STRUKTURAK_H

typedef struct Szavazo {
    char nev[128];
    char jelszo[64];
    char jelolt[128];
    char part[128];
} Szavazo;

typedef struct Jelolt {
    char nev[128];
    char part[128];
} Jelolt;

typedef struct Eredmeny {
    char nev[128];
    char part[128];
    int  szavazat;
} Eredmeny;


int mainmenu(int allapot);

int beolvas_szavazok(const char *path, Szavazo **tomb_out, int *db_out);
int beolvas_jeloltek(const char *path, Jelolt **tomb_out, int *db_out);
int beolvas_eredmeny(const char *path, Eredmeny **tomb_out, int *db_out);
int mentes_eredmeny(const char *path, const Eredmeny *arr, int count);
int mentes_uj_jelolt(const char *path, const Jelolt *j);

void sor_urites(void);
void sorveg_levagas(char *s);

int mentes_jeloltek(const char *path, const Jelolt *arr, int db);
int mentes_szavazok(const char *path, const Szavazo *arr, int db);
int beolvas_allapot(const char *path, int *allapot_out);
int mentes_allapot(const char *path, int allapot);
int beolvas_uzenetek_adminnak(const char *path);

int mentes_uzenet_adminnak(const char *path, const char *nev, const char *uzenet);

void admin_menu_kiir(void);
void admin_szavazok_kezelese(void);
void admin_jelolt_jelentkezesek(void);
void admin_uzenetek_megtekintese(void);
void admin_allapot(int *allapot);

int kerdezz_sor(const char *prompt, char *buf, size_t cap);
int keres_szavazo_index(const Szavazo *t, int db, const char *nev);
int ellenoriz_es_biztosit_jelszo(Szavazo *s);
void szavazas_leadasa(void);
void statisztika_megtekintese(void);
void szavazatom_megtekintese(void);
void jelolt_jelentkezes(void);
void uzenet_adminnak(void);

typedef struct JeloltLista {
    Jelolt adat;
    struct JeloltLista *kov;
} JeloltLista;

int beolvas_uj_jeloltek_list(const char *path, JeloltLista **eleje_out);
int mentes_uj_jeloltek_list(const char *path, const JeloltLista *eleje);
void jeloltlista_felszabadit(JeloltLista *eleje);
int jeloltlista_beszur_vegere(JeloltLista **eleje, const Jelolt *j);
int jeloltlista_torol_index(JeloltLista **eleje, int index);
int jeloltlista_keres_nev(const JeloltLista *eleje, const char *nev);

int lezaro_idopont_elmult(void);
int mai_ido_yyyy_mm_dd_hh_mm_ss(char *buf, size_t cap);
