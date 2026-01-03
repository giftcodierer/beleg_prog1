#include "ui.h"
#include "fileio.h"
#include <gtk/gtk.h>

static MaterialListe *glob_liste;
static GtkListStore *store;

static void refresh_table() {
    gtk_list_store_clear(store);
    material_sortieren(glob_liste);

    for (int i = 0; i < glob_liste->count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, glob_liste->items[i].artikelnummer,
            1, glob_liste->items[i].bezeichnung,
            2, glob_liste->items[i].bestand,
            -1);
    }
}

static void on_add(GtkWidget *w, gpointer d) {
    material_hinzufuegen(glob_liste, "Neuer Artikel",
                         1000 + glob_liste->count, 0);
    refresh_table();
}

void ui_start(MaterialListe *liste) {
    glob_liste = liste;

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "Materialverwaltung");
    gtk_window_set_default_size(GTK_WINDOW(win), 600, 400);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(win), vbox);

    GtkWidget *menubar = gtk_menu_bar_new();
    GtkWidget *menu = gtk_menu_new();
    GtkWidget *item = gtk_menu_item_new_with_label("Artikel");
    GtkWidget *add = gtk_menu_item_new_with_label("Neu");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), add);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), item);
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    g_signal_connect(add, "activate", G_CALLBACK(on_add), NULL);

    store = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT);
    GtkWidget *tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree),
        gtk_tree_view_column_new_with_attributes(
            "Nr", gtk_cell_renderer_text_new(), "text", 0, NULL));

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree),
        gtk_tree_view_column_new_with_attributes(
            "Bezeichnung", gtk_cell_renderer_text_new(), "text", 1, NULL));

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree),
        gtk_tree_view_column_new_with_attributes(
            "Bestand", gtk_cell_renderer_text_new(), "text", 2, NULL));

    gtk_box_pack_start(GTK_BOX(vbox), tree, TRUE, TRUE, 0);

    refresh_table();

    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(win);
}
