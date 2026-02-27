/*
* Belegarbeit Programmieren 1, WS2025/26, Materialverwaltung
* Name: Apel, Julian
* Matrikelnummer: 57849
* HTW-ID: s90353
*/

#ifndef FILEIO_H
#define FILEIO_H

#include "material.h"

void daten_laden(const char *datei, MaterialListe *liste);
void daten_speichern(const char *datei, MaterialListe *liste);

#endif
