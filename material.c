/*
* Belegarbeit Programmieren 1, WS2025/26, Materialverwaltung
* Name: Apel, Julian
* Matrikelnummer: 57849
* HTW-ID: s90353
*/

#include "material.h"
#include <stdlib.h>
#include <string.h>

// Initializes an empty material list
void init_liste(MaterialListe *liste) {
    liste->items = NULL;
    liste->count = 0;
}

// Frees all memory associated with the material list
void free_liste(MaterialListe *liste) {
    for (int i = 0; i < liste->count; i++) {
        free(liste->items[i].bezeichnung);
    }
    free(liste->items);
}

/* Adds a new material to the list */
void material_hinzufuegen(MaterialListe *liste,
                          const char *bez,
                          int nr,
                          int bestand) {
    Material *tmp = realloc(liste->items,
                            (liste->count + 1) * sizeof(Material));
    if (!tmp) return;
    liste->items = tmp;

    Material *m = &liste->items[liste->count];
    m->bezeichnung = malloc(strlen(bez) + 1);
    strcpy(m->bezeichnung, bez);
    m->artikelnummer = nr;
    m->bestand = bestand;
    liste->count++;
}

/* Deletes a material with the given article number from the list */
void material_loeschen(MaterialListe *liste, int nr) {
    for (int i = 0; i < liste->count; i++) {
        if (liste->items[i].artikelnummer == nr) {
            free(liste->items[i].bezeichnung);
            for (int j = i; j < liste->count - 1; j++)
                liste->items[j] = liste->items[j + 1];
            liste->count--;

            if (liste->count == 0) {
                free(liste->items);
                liste->items = NULL;
            } else {
                Material *tmp = realloc(liste->items,
                                        liste->count * sizeof(Material));
                if (tmp) liste->items = tmp;
            }
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
