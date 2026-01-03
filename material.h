#ifndef MATERIAL_H
#define MATERIAL_H

typedef struct {
    char *bezeichnung;
    int artikelnummer;
    int bestand;
} Material;

typedef struct {
    Material *items;
    int count;
} MaterialListe;

void init_liste(MaterialListe *liste);
void free_liste(MaterialListe *liste);

void material_hinzufuegen(MaterialListe *liste,
                          const char *bez,
                          int nr,
                          int bestand);

void material_loeschen(MaterialListe *liste, int artikelnummer);
Material *material_suchen(MaterialListe *liste, int artikelnummer);

void bestand_aendern(Material *m, int delta);
void material_sortieren(MaterialListe *liste);

#endif
