/*
* Belegarbeit Programmieren 1, WS2025/26, Materialverwaltung
* Name: Apel, Julian
* Matrikelnummer: 57849
* HTW-ID: s90353
*
* Die Umsetzung erfolgte unter Verwendung der GTK-Bibliothek für die grafische Benutzeroberfläche. Es wurden drei Hauptmodule erstellt:
* 1. material.c/h: Enthält die Definitionen für die Materialstruktur und Funktionen zur Verwaltung der Materialliste.
* 2. fileio.c/h: Verantwortlich für das Laden und Speichern der Materialdaten in einer Textdatei. Die Daten werden im Format "Artikelnummer;Bezeichnung;Bestand" gespeichert.
* 3. ui.c/h: Implementiert die grafische Benutzeroberfläche mit GTK.
* Die Daten werden in der Textdatei "data.txt" im Format "Artikelnummer;Bezeichnung;Bestand" gespeichert.
*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
* Durch anklicken eines Eintrages und darauffolgendem klicken auf Warenausgang wird der Datensatz entfernt.
* Durch einen Doppelklick in die Spalte "Bezeichnung" oder "Bestand" kann der Eintrag bearbeitet werden.
* Durch einen Klick auf Wareneingang öffnet sich ein Dialog, in dem man einen neuen Eintrag anlegen kann.
*/


#include <gtk/gtk.h>
#include "material.h"
#include "fileio.h"
#include "ui.h"

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    MaterialListe liste;
    init_liste(&liste);

    daten_laden("data.txt", &liste);
    ui_start(&liste);
    gtk_main();

    daten_speichern("data.txt", &liste);
    free_liste(&liste);
    return 0;
}
