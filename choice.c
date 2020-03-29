#include <gtk/gtk.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////

char const *Slave = ".Slave";

///////////////////////////////////////////////////////////////////////////////

// prototypes
void request_choice(void);
void quit_choice(void);
void add_password(GtkWidget *widget, gpointer data);
void populate_add_grd(gpointer data);

///////////////////////////////////////////////////////////////////////////////

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
	gtk_widget_set_halign(add_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(add_grd, TRUE);
	populate_add_grd(add_grd);

	// notebook tab to delete password
	GtkWidget *del_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(del_lbl), "<span size=\"7500\" weight=\"ultralight\">Delete Password</span>");
	GtkWidget *del_grd = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(del_grd), 25);
	gtk_grid_set_row_spacing(GTK_GRID(del_grd), 25);
	gtk_widget_set_halign(add_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(add_grd, TRUE);

	// notebook tab to change password
	GtkWidget *chg_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(chg_lbl), "<span size=\"7500\" weight=\"ultralight\">Change Password</span>");
	GtkWidget *chg_grd = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(chg_grd), 25);
	gtk_grid_set_row_spacing(GTK_GRID(chg_grd), 25);
	gtk_widget_set_halign(add_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(add_grd, TRUE);

	// notebook tab to view password
	GtkWidget *see_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(see_lbl), "<span size=\"7500\" weight=\"ultralight\">View Password</span>");
	GtkWidget *see_grd = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(see_grd), 25);
	gtk_grid_set_row_spacing(GTK_GRID(see_grd), 25);
	gtk_widget_set_halign(add_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(add_grd, TRUE);

	// notebook tab to change passphrase
	GtkWidget *cpp_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(cpp_lbl), "<span size=\"7500\" weight=\"ultralight\">Change Passphrase</span>");
	GtkWidget *cpp_grd = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(cpp_grd), 25);
	gtk_grid_set_row_spacing(GTK_GRID(cpp_grd), 25);
	gtk_widget_set_halign(add_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(add_grd, TRUE);

	// notebook
	GtkWidget *notebook = gtk_notebook_new();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), add_grd, add_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), del_grd, del_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), chg_grd, chg_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), see_grd, see_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), cpp_grd, cpp_lbl);
	gtk_container_add(GTK_CONTAINER(window), notebook);

	// show everything
	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(quit_choice), NULL);
	gtk_main();
}

///////////////////////////////////////////////////////////////////////////////

// fill notebook tab to add password
void populate_add_grd(gpointer data)
{
	// get the grid in which widgets will be shown
	GtkWidget *add_grd = data;

	// header
	GtkWidget *main_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(main_label), "<b>Enter the following details to add a new password.</b>");
	gtk_grid_attach(GTK_GRID(add_grd), main_label, 0, 0, 2, 1);

	// website
	GtkWidget *site_label = gtk_label_new("Website");
	gtk_grid_attach(GTK_GRID(add_grd), site_label, 0, 1, 1, 1);
	GtkWidget *site_entry = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(site_entry), "e.g. Facebook, Stack Overflow");
	gtk_grid_attach(GTK_GRID(add_grd), site_entry, 1, 1, 1, 1);

	// username
	GtkWidget *uname_label = gtk_label_new("Username");
	gtk_grid_attach(GTK_GRID(add_grd), uname_label, 0, 2, 1, 1);
	GtkWidget *uname_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(add_grd), uname_entry, 1, 2, 1, 1);

	// password
	GtkWidget *pw_label = gtk_label_new("Password");
	gtk_grid_attach(GTK_GRID(add_grd), pw_label, 0, 3, 1, 1);
	GtkWidget *pw_entry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(pw_entry), FALSE);
	gtk_grid_attach(GTK_GRID(add_grd), pw_entry, 1, 3, 1, 1);

	// set the information in the global struct variable
	set_credentials(site_entry, uname_entry, pw_entry);

	// button
	GtkWidget *add_btn = gtk_button_new_with_label("Add Password");
	g_signal_connect(GTK_BUTTON(add_btn), "clicked", G_CALLBACK(add_password), NULL);
	gtk_grid_attach(GTK_GRID(add_grd), add_btn, 0, 4, 2, 1);
}

///////////////////////////////////////////////////////////////////////////////

// write credentials to file
void add_password(GtkWidget *widget, gpointer data)
{
	see_credentials();
	FILE *pw_file = fopen(Slave, "a");
	// fprintf();
}

///////////////////////////////////////////////////////////////////////////////

void quit_choice(void)
{
	del_credentials();
	free(credentials);
	gtk_main_quit();
	exit(0);
}

