void request_choice(void);
void quit_choice(GtkWidget *widget, gpointer data);
void add_password(GtkButton *button, gpointer data);
void change_password(GtkButton *button, gpointer data);
void delete_password(GtkButton *button, gpointer data);
void change_passphrase(GtkButton *button, gpointer data);
void delete_previous_search_results(GtkWidget *bot_grd);
void populate_search_results(GtkEntry *entry, gpointer data);
void show_password(GtkButton *button, gpointer data);
gboolean hide_password(gpointer data);
GtkWidget *create_widget_for_add(GtkWidget *window);
GtkWidget *create_widget_for_mng(GtkWidget *window);
GtkWidget *create_widget_for_cpp(GtkWidget *window);

#include "choice.c"

