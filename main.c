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
