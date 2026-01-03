#ifndef FILEIO_H
#define FILEIO_H

#include "material.h"

void daten_laden(const char *datei, MaterialListe *liste);
void daten_speichern(const char *datei, MaterialListe *liste);

#endif
