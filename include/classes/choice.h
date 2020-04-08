void request_choice(void);
void quit_choice(void);
void add_password(GtkWidget *widget, gpointer data);
void populate_search_results(GtkWidget *widget, gpointer data);
GtkWidget *create_widget_for_add(GtkWidget **window);
GtkWidget *create_widget_for_chg(GtkWidget **window);
GtkWidget *create_widget_for_see(GtkWidget **window);
GtkWidget *create_widget_for_cpp(GtkWidget **window);

#include "choice.c"

