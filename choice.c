#include <gtk/gtk.h>
#include <stdlib.h>

// prototypes
void request_choice(void);
void quit_choice(void);
void handle_choice(GtkWidget *widget, gpointer data);

// show a window to ask the user what they want to do
void request_choice(void)
{
	// window
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 50);
	gtk_window_set_icon_from_file(GTK_WINDOW(window), "favicon.png", NULL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_title(GTK_WINDOW(window), "Password Manager");

	// grid
	GtkWidget *grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(grid), 25);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 25);
	gtk_container_add(GTK_CONTAINER(window), grid);

	// header
	GtkWidget *main_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(main_label), "<b>What would you like to do?</b>");
	gtk_grid_attach(GTK_GRID(grid), main_label, 0, 0, 1, 1);

	// buttons
	GtkWidget *add_button = gtk_button_new_with_label("Add a Password");
	GtkWidget *del_button = gtk_button_new_with_label("Delete a Password");
	GtkWidget *chg_button = gtk_button_new_with_label("Change a Password");
	GtkWidget *see_button = gtk_button_new_with_label("View a Password");
	GtkWidget *cpp_button = gtk_button_new_with_label("Change the Passphrase");
	g_signal_connect(GTK_BUTTON(add_button), "clicked", G_CALLBACK(handle_choice), &window);
	g_signal_connect(GTK_BUTTON(del_button), "clicked", G_CALLBACK(handle_choice), &window);
	g_signal_connect(GTK_BUTTON(chg_button), "clicked", G_CALLBACK(handle_choice), &window);
	g_signal_connect(GTK_BUTTON(see_button), "clicked", G_CALLBACK(handle_choice), &window);
	g_signal_connect(GTK_BUTTON(cpp_button), "clicked", G_CALLBACK(handle_choice), &window);
	gtk_grid_attach(GTK_GRID(grid), add_button, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), del_button, 0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), chg_button, 0, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), see_button, 0, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), cpp_button, 0, 5, 1, 1);

	// display everything
	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(quit_choice), NULL);
	gtk_main();
}

void handle_choice(GtkWidget *widget, gpointer data)
{
	GtkWidget **window = data;
	printf("click detected\n");
	gtk_widget_hide(*window);
	GtkWidget *w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_show_all(w);
	g_signal_connect(w, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_main();
	gtk_widget_show_all(*window);
}

void quit_choice(void)
{
	gtk_main_quit();
	exit(0);
}

