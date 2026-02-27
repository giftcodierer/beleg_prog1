#include "ui.h"
#include "material.h"
#include <gtk/gtk.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

static MaterialListe *glob_liste;
static GtkListStore  *store;
static GtkWidget     *tree_view;
static char           suchtext[256] = "";

/* Converts each character of the input string to lowercase */
static void str_to_lower(const char *in, char *out, size_t size) {
    for (size_t i = 0; i < size - 1 && in[i]; i++)
        out[i] = tolower((unsigned char)in[i]);
    out[strlen(in)] = '\0';
}

/* Clears and refills the table, only showing rows that match the search string */
static void refresh_table() {
    gtk_list_store_clear(store);
    material_sortieren(glob_liste);
    char such_lower[256];
    str_to_lower(suchtext, such_lower, sizeof(such_lower));

    for (int i = 0; i < glob_liste->count; i++) {
        char name_lower[256], nr_str[32], nr_lower[32];
        str_to_lower(glob_liste->items[i].bezeichnung, name_lower, sizeof(name_lower));
        snprintf(nr_str, sizeof(nr_str), "%d", glob_liste->items[i].artikelnummer);
        str_to_lower(nr_str, nr_lower, sizeof(nr_lower));

        if (suchtext[0] && !g_strrstr(name_lower, such_lower) && !g_strrstr(nr_lower, such_lower))
            continue;

        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, glob_liste->items[i].artikelnummer,
            1, glob_liste->items[i].bezeichnung,
            2, glob_liste->items[i].bestand, -1);
    }
}

/* Adds a new default article to the list */
static void on_add(GtkWidget *w, gpointer d) {
    material_hinzufuegen(glob_liste, "Neuer Artikel", 1000 + glob_liste->count, 0);
    refresh_table();
}

/* Removes the selected article from the list */
static void on_remove(GtkWidget *w, gpointer d) {
    GtkTreeSelection *sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    GtkTreeModel *model;
    GtkTreeIter iter;
    if (!gtk_tree_selection_get_selected(sel, &model, &iter)) return;
    gint nr;
    gtk_tree_model_get(model, &iter, 0, &nr, -1);
    material_loeschen(glob_liste, nr);
    refresh_table();
}

/* Called when a text cell is edited */
static void on_text_edited(GtkCellRendererText *cell, gchar *path_str, gchar *new_text, gpointer d) {
    GtkTreeIter iter;
    GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
    gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path);
    gint nr;
    gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 0, &nr, -1);
    gtk_list_store_set(store, &iter, 1, new_text, -1);
    Material *m = material_suchen(glob_liste, nr);
    if (m) { free(m->bezeichnung); m->bezeichnung = strdup(new_text); }
    gtk_tree_path_free(path);
}

/* Called when the stock value is edited */
static void on_int_edited(GtkCellRendererText *cell, gchar *path_str, gchar *new_text, gpointer d) {
    GtkTreeIter iter;
    GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
    gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path);
    gint nr;
    gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 0, &nr, -1);
    int value = atoi(new_text);
    gtk_list_store_set(store, &iter, 2, value, -1);
    Material *m = material_suchen(glob_liste, nr);
    if (m) m->bestand = value;
    gtk_tree_path_free(path);
}

/* Updates the search string and refreshes the table */
static void on_search_changed(GtkEntry *e, gpointer d) {
    strncpy(suchtext, gtk_entry_get_text(e), sizeof(suchtext) - 1);
    refresh_table();
}

void ui_start(MaterialListe *liste) {
    glob_liste = liste;

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "Materialverwaltung");
    gtk_window_set_default_size(GTK_WINDOW(win), 600, 400);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(win), vbox);

    /* Search bar */
    GtkWidget *search = gtk_search_entry_new();
    g_signal_connect(search, "search-changed", G_CALLBACK(on_search_changed), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), search, FALSE, FALSE, 5);

    /* Menu bar with Wareneingang and Warenausgang */
    GtkWidget *menubar  = gtk_menu_bar_new();
    GtkWidget *btn_add  = gtk_menu_item_new_with_label("Wareneingang");
    GtkWidget *btn_rem  = gtk_menu_item_new_with_label("Warenausgang");
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), btn_add);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), btn_rem);
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
    g_signal_connect(btn_add, "activate", G_CALLBACK(on_add), NULL);
    g_signal_connect(btn_rem, "activate", G_CALLBACK(on_remove), NULL);

    /* List store and tree view */
    store     = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT);
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    /* Article number column (read-only) */
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),
        gtk_tree_view_column_new_with_attributes("Nr",
            gtk_cell_renderer_text_new(), "text", 0, NULL));

    /* Editable description column */
    GtkCellRenderer *r_text = gtk_cell_renderer_text_new();
    g_object_set(r_text, "editable", TRUE, NULL);
    g_signal_connect(r_text, "edited", G_CALLBACK(on_text_edited), NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),
        gtk_tree_view_column_new_with_attributes("Bezeichnung", r_text, "text", 1, NULL));

    /* Editable stock column */
    GtkCellRenderer *r_int = gtk_cell_renderer_text_new();
    g_object_set(r_int, "editable", TRUE, NULL);
    g_signal_connect(r_int, "edited", G_CALLBACK(on_int_edited), NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),
        gtk_tree_view_column_new_with_attributes("Bestand", r_int, "text", 2, NULL));

    gtk_box_pack_start(GTK_BOX(vbox), tree_view, TRUE, TRUE, 0);

    refresh_table();

    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(win);
}
