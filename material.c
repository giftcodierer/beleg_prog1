#include "material.h"
#include <stdlib.h>
#include <string.h>

void init_liste(MaterialListe *liste) {
    liste->items = NULL;
    liste->count = 0;
}

void free_liste(MaterialListe *liste) {
    for (int i = 0; i < liste->count; i++) {
        free(liste->items[i].bezeichnung);
    }
    free(liste->items);
}

void material_hinzufuegen(MaterialListe *liste,
                          const char *bez,
                          int nr,
                          int bestand) {
    liste->items = realloc(liste->items,
                            (liste->count + 1) * sizeof(Material));

    Material *m = &liste->items[liste->count];
    m->bezeichnung = malloc(strlen(bez) + 1);
    strcpy(m->bezeichnung, bez);
    m->artikelnummer = nr;
    m->bestand = bestand;

    liste->count++;
}

void material_loeschen(MaterialListe *liste, int nr) {
    for (int i = 0; i < liste->count; i++) {
        if (liste->items[i].artikelnummer == nr) {
            free(liste->items[i].bezeichnung);
            for (int j = i; j < liste->count - 1; j++) {
                liste->items[j] = liste->items[j + 1];
            }
            liste->count--;
            liste->items = realloc(liste->items,
                                   liste->count * sizeof(Material));
            return;
        }
    }
}

Material *material_suchen(MaterialListe *liste, int nr) {
    for (int i = 0; i < liste->count; i++) {
        if (liste->items[i].artikelnummer == nr)
            return &liste->items[i];
    }
    return NULL;
}

void bestand_aendern(Material *m, int delta) {
    if (m) m->bestand += delta;
}

static int cmp(const void *a, const void *b) {
    return ((Material *)a)->artikelnummer -
           ((Material *)b)->artikelnummer;
}

void material_sortieren(MaterialListe *liste) {
    qsort(liste->items, liste->count,
          sizeof(Material), cmp);
}
