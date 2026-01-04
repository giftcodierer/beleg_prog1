#include "ui.h"
#include "fileio.h"
#include <gtk/gtk.h>

static MaterialListe *glob_liste;
static GtkListStore *store;
static GtkTreeModelFilter *filter;
static char suchtext[256] = "";


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

static void on_text_edited(GtkCellRendererText *cell,
                           gchar *path_str,
                           gchar *new_text,
                           gpointer user_data)
{
    int column = GPOINTER_TO_INT(user_data);
    GtkTreeIter iter;
    GtkTreePath *path = gtk_tree_path_new_from_string(path_str);

    gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path);
    gtk_list_store_set(store, &iter, column, new_text, -1);

    int index = gtk_tree_path_get_indices(path)[0];
    if (column == 1) {
        free(glob_liste->items[index].bezeichnung);
        glob_liste->items[index].bezeichnung = strdup(new_text);
    }

    gtk_tree_path_free(path);
}

static void on_int_edited(GtkCellRendererText *cell,
                          gchar *path_str,
                          gchar *new_text,
                          gpointer user_data)
{
    int value = atoi(new_text);
    GtkTreeIter iter;
    GtkTreePath *path = gtk_tree_path_new_from_string(path_str);

    gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path);
    gtk_list_store_set(store, &iter, 2, value, -1);

    int index = gtk_tree_path_get_indices(path)[0];
    glob_liste->items[index].bestand = value;

    gtk_tree_path_free(path);
}

static gboolean filter_func(GtkTreeModel *model,
                            GtkTreeIter *iter,
                            gpointer data)
{
    if (suchtext[0] == '\0')
        return TRUE;

    gchar *name;
    gint nr;
    char nr_str[32];

    gtk_tree_model_get(model, iter,
                       0, &nr,
                       1, &name,
                       -1);

    snprintf(nr_str, sizeof(nr_str), "%d", nr);

    gboolean match =
        g_strrstr(name, suchtext) != NULL ||
        g_strrstr(nr_str, suchtext) != NULL;

    g_free(name);
    return match;
}


static void on_search_changed(GtkEntry *e, gpointer d)
{
    const char *t = gtk_entry_get_text(e);
    strncpy(suchtext, t, sizeof(suchtext) - 1);
    suchtext[sizeof(suchtext) - 1] = '\0';

    gtk_tree_model_filter_refilter(filter);
}


void ui_start(MaterialListe *liste) {
    glob_liste = liste;

// Initialize GTK window
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "Materialverwaltung");
    gtk_window_set_default_size(GTK_WINDOW(win), 600, 400);

// Create vertical box layout
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(win), vbox);

// Create search entry
	GtkWidget *search = gtk_search_entry_new();
	g_signal_connect(search, "search-changed",
                 G_CALLBACK(on_search_changed), NULL);
	gtk_box_pack_start(GTK_BOX(vbox), search, FALSE, FALSE, 5);
// Create menu bar
    GtkWidget *menubar = gtk_menu_bar_new();

//Create menu and menu items
    GtkWidget *menu = gtk_menu_new();
    GtkWidget *item = gtk_menu_item_new_with_label("Artikel");
    GtkWidget *add = gtk_menu_item_new_with_label("Neu");

/* Attach the submenu to the menu item */
gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);

/* Add the "add" menu item to the submenu */
gtk_menu_shell_append(GTK_MENU_SHELL(menu), add);

/* Add the menu item (with submenu) to the menu bar */
gtk_menu_shell_append(GTK_MENU_SHELL(menubar), item);

/* Pack the menu bar at the top of the vertical box */
gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

/* Connect the "activate" signal of the menu item to the add callback */
g_signal_connect(add, "activate", G_CALLBACK(on_add), NULL);


/* Create the list store (model) with three columns:
 *  - column 0: article number (int)
 *  - column 1: article description (string)
 *  - column 2: stock quantity (int)
 */
store = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT);

/* Create a filter model on top of the list store */
filter = GTK_TREE_MODEL_FILTER(
    gtk_tree_model_filter_new(GTK_TREE_MODEL(store), NULL));

/* Assign the filter function which decides which rows are visible */
gtk_tree_model_filter_set_visible_func(
    filter, filter_func, NULL, NULL);

/* Create the tree view using the filtered model */
GtkWidget *tree =
    gtk_tree_view_new_with_model(GTK_TREE_MODEL(filter));


/* Create and append the column for the article number (read-only) */
gtk_tree_view_append_column(GTK_TREE_VIEW(tree),
    gtk_tree_view_column_new_with_attributes(
        "Nr", gtk_cell_renderer_text_new(), "text", 0, NULL));


/* Create an editable text renderer for the article description */
GtkCellRenderer *r_text = gtk_cell_renderer_text_new();
g_object_set(r_text, "editable", TRUE, NULL);

/* Connect the "edited" signal to update the underlying data */
g_signal_connect(r_text, "edited",
                 G_CALLBACK(on_text_edited),
                 GINT_TO_POINTER(1));

/* Append the description column to the tree view */
gtk_tree_view_append_column(GTK_TREE_VIEW(tree),
    gtk_tree_view_column_new_with_attributes(
        "Bezeichnung", r_text, "text", 1, NULL));


/* Create an editable text renderer for the stock quantity */
GtkCellRenderer *r_int = gtk_cell_renderer_text_new();
g_object_set(r_int, "editable", TRUE, NULL);

/* Connect the edit signal to update the stock value */
g_signal_connect(r_int, "edited",
                 G_CALLBACK(on_int_edited),
                 NULL);

/* Append the stock column to the tree view */
gtk_tree_view_append_column(GTK_TREE_VIEW(tree),
    gtk_tree_view_column_new_with_attributes(
        "Bestand", r_int, "text", 2, NULL));


/* Pack the tree view so it fills the remaining window space */
gtk_box_pack_start(GTK_BOX(vbox), tree, TRUE, TRUE, 0);

/* Populate the table with the current data */
refresh_table();

/* Quit the GTK main loop when the window is closed */
g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

/* Make all widgets visible */
gtk_widget_show_all(win);

}
