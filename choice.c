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
	gtk_container_set_border_width(GTK_CONTAINER(window), 0);
	gtk_window_set_icon_from_file(GTK_WINDOW(window), "favicon.png", NULL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_title(GTK_WINDOW(window), "Password Manager");

	// notebook tabs
	GtkWidget *add_frame = gtk_frame_new(NULL);
	GtkWidget *del_frame = gtk_frame_new(NULL);
	GtkWidget *chg_frame = gtk_frame_new(NULL);
	GtkWidget *see_frame = gtk_frame_new(NULL);
	GtkWidget *cpp_frame = gtk_frame_new(NULL);
	GtkWidget *add_label = gtk_label_new("Add Password");
	GtkWidget *del_label = gtk_label_new("Delete Password");
	GtkWidget *chg_label = gtk_label_new("Change Password");
	GtkWidget *see_label = gtk_label_new("View Password");
	GtkWidget *cpp_label = gtk_label_new("Change Passphrase");
	gtk_container_set_border_width(GTK_CONTAINER(add_frame), 50);
	gtk_container_set_border_width(GTK_CONTAINER(del_frame), 50);
	gtk_container_set_border_width(GTK_CONTAINER(chg_frame), 50);
	gtk_container_set_border_width(GTK_CONTAINER(see_frame), 50);
	gtk_container_set_border_width(GTK_CONTAINER(cpp_frame), 50);

	// notebook
	GtkWidget *notebook = gtk_notebook_new();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), add_frame, add_label);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), del_frame, del_label);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), chg_frame, chg_label);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), see_frame, see_label);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), cpp_frame, cpp_label);
	gtk_container_add(GTK_CONTAINER(window), notebook);

	// show everything
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

