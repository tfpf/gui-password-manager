#include <gtk/gtk.h>
#include <stdlib.h>

// prototypes
void request_choice(void);
void quit_choice(void);
void handle_choice(GtkWidget *widget, gpointer data);
void populate_add_grd(gpointer data);

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

	// notebook tab to add password
	GtkWidget *add_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(add_lbl), "<span size=\"7500\" weight=\"ultralight\">Add Password</span>");
	GtkWidget *add_grd = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(add_grd), 50);
	gtk_grid_set_column_spacing(GTK_GRID(add_grd), 25);
	gtk_grid_set_row_spacing(GTK_GRID(add_grd), 25);
	populate_add_grd(add_grd);

	// notebook tab to delete password
	GtkWidget *del_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(del_lbl), "<span size=\"7500\" weight=\"ultralight\">Delete Password</span>");
	GtkWidget *del_grd = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(del_grd), 25);
	gtk_grid_set_row_spacing(GTK_GRID(del_grd), 25);

	// notebook tab to change password
	GtkWidget *chg_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(chg_lbl), "<span size=\"7500\" weight=\"ultralight\">Change Password</span>");
	GtkWidget *chg_grd = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(chg_grd), 25);
	gtk_grid_set_row_spacing(GTK_GRID(chg_grd), 25);

	// notebook tab to view password
	GtkWidget *see_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(see_lbl), "<span size=\"7500\" weight=\"ultralight\">View Password</span>");
	GtkWidget *see_grd = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(see_grd), 25);
	gtk_grid_set_row_spacing(GTK_GRID(see_grd), 25);

	// notebook tab to change passphrase
	GtkWidget *cpp_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(cpp_lbl), "<span size=\"7500\" weight=\"ultralight\">Change Passphrase</span>");
	GtkWidget *cpp_grd = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(cpp_grd), 25);
	gtk_grid_set_row_spacing(GTK_GRID(cpp_grd), 25);

	// notebook
	GtkWidget *notebook = gtk_notebook_new();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), add_grd, add_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), del_grd, del_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), chg_grd, chg_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), see_grd, see_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), cpp_grd, cpp_lbl);
	gtk_container_add(GTK_CONTAINER(window), notebook);

	// // notebook tabs
	// GtkWidget *add_frame = gtk_frame_new(NULL);
	// GtkWidget *del_frame = gtk_frame_new(NULL);
	// GtkWidget *chg_frame = gtk_frame_new(NULL);
	// GtkWidget *see_frame = gtk_frame_new(NULL);
	// GtkWidget *cpp_frame = gtk_frame_new(NULL);
	// GtkWidget *add_label = gtk_label_new("Add Password");
	// GtkWidget *del_label = gtk_label_new("Delete Password");
	// GtkWidget *chg_label = gtk_label_new("Change Password");
	// GtkWidget *see_label = gtk_label_new("View Password");
	// GtkWidget *cpp_label = gtk_label_new("Change Passphrase");
	// gtk_container_set_border_width(GTK_CONTAINER(add_frame), 50);
	// gtk_container_set_border_width(GTK_CONTAINER(del_frame), 50);
	// gtk_container_set_border_width(GTK_CONTAINER(chg_frame), 50);
	// gtk_container_set_border_width(GTK_CONTAINER(see_frame), 50);
	// gtk_container_set_border_width(GTK_CONTAINER(cpp_frame), 50);

	// // notebook
	// GtkWidget *notebook = gtk_notebook_new();
	// gtk_notebook_append_page(GTK_NOTEBOOK(notebook), add_frame, add_label);
	// gtk_notebook_append_page(GTK_NOTEBOOK(notebook), del_frame, del_label);
	// gtk_notebook_append_page(GTK_NOTEBOOK(notebook), chg_frame, chg_label);
	// gtk_notebook_append_page(GTK_NOTEBOOK(notebook), see_frame, see_label);
	// gtk_notebook_append_page(GTK_NOTEBOOK(notebook), cpp_frame, cpp_label);
	// gtk_container_add(GTK_CONTAINER(window), notebook);

	// show everything
	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(quit_choice), NULL);
	gtk_main();
}

void populate_add_grd(gpointer data)
{
	GtkWidget *add_grd = data;
	gtk_widget_set_halign(add_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(add_grd, TRUE);

	GtkWidget *main_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(main_label), "<b>Enter the following details to add a new password.</b>");
	gtk_grid_attach(GTK_GRID(add_grd), main_label, 0, 0, 2, 1);

	GtkWidget *uname_label = gtk_label_new("Username");
	gtk_grid_attach(GTK_GRID(add_grd), uname_label, 0, 1, 1, 1);
	GtkWidget *pw_label = gtk_label_new("Password");
	gtk_grid_attach(GTK_GRID(add_grd), pw_label, 1, 1, 1, 1);
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

