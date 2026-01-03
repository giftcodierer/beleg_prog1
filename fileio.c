#include "fileio.h"
#include <stdio.h>

void daten_laden(const char *datei, MaterialListe *liste) {
    FILE *f = fopen(datei, "r");
    if (!f) return;

    char bez[256];
    int nr, bestand;

    while (fscanf(f, "%d;%255[^;];%d\n",
                  &nr, bez, &bestand) == 3) {
        material_hinzufuegen(liste, bez, nr, bestand);
                  }
    fclose(f);
}

void daten_speichern(const char *datei, MaterialListe *liste) {
    FILE *f = fopen(datei, "w");
    if (!f) return;

    for (int i = 0; i < liste->count; i++) {
        fprintf(f, "%d;%s;%d\n",
                liste->items[i].artikelnummer,
                liste->items[i].bezeichnung,
                liste->items[i].bestand);
    }
    fclose(f);
}
