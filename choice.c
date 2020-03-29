#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUF_SIZ 1024

///////////////////////////////////////////////////////////////////////////////

char const *Slave = ".Slave";

///////////////////////////////////////////////////////////////////////////////

// prototypes
void request_choice(void);
void quit_choice(void);
void add_password(GtkWidget *widget, gpointer data);
GtkWidget *create_add_grd(GtkWidget **window);
GtkWidget *create_del_grd(GtkWidget **window);
GtkWidget *create_chg_grd(GtkWidget **window);
GtkWidget *create_see_grd(GtkWidget **window);
GtkWidget *create_cpp_grd(GtkWidget **window);

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
	GtkWidget *add_grd = create_add_grd(&window);
	GtkWidget *add_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(add_lbl), "<span weight=\"ultralight\">Add Password</span>");

	// notebook tab to delete password
	GtkWidget *del_grd = create_del_grd(&window);
	GtkWidget *del_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(del_lbl), "<span weight=\"ultralight\">Delete Password</span>");

	// notebook tab to change password
	GtkWidget *chg_grd = create_chg_grd(&window);
	GtkWidget *chg_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(chg_lbl), "<span weight=\"ultralight\">Change Password</span>");

	// notebook tab to view password
	GtkWidget *see_grd = create_see_grd(&window);
	GtkWidget *see_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(see_lbl), "<span weight=\"ultralight\">View Password</span>");

	// notebook tab to change passphrase
	GtkWidget *cpp_grd = create_cpp_grd(&window);
	GtkWidget *cpp_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(cpp_lbl), "<span weight=\"ultralight\">Change Passphrase</span>");

	// notebook
	GtkWidget *notebook = gtk_notebook_new();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), add_grd, add_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), del_grd, del_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), chg_grd, chg_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), see_grd, see_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), cpp_grd, cpp_lbl);
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_LEFT);
	gtk_container_add(GTK_CONTAINER(window), notebook);

	// show everything
	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(quit_choice), NULL);
	gtk_main();
}

///////////////////////////////////////////////////////////////////////////////

// populate the notebook tab to add password
GtkWidget *create_add_grd(GtkWidget **window)
{
	// the object to return
	GtkWidget *add_grd = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(add_grd), 50);
        gtk_grid_set_column_spacing(GTK_GRID(add_grd), 25);
        gtk_grid_set_row_spacing(GTK_GRID(add_grd), 25);
        gtk_widget_set_halign(add_grd, GTK_ALIGN_CENTER);
        gtk_widget_set_hexpand(add_grd, TRUE);

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
	g_signal_connect(GTK_BUTTON(add_btn), "clicked", G_CALLBACK(add_password), window);
	gtk_grid_attach(GTK_GRID(add_grd), add_btn, 0, 4, 2, 1);

	return add_grd;
}

///////////////////////////////////////////////////////////////////////////////

// populate the notebook tab to delete password
GtkWidget *create_del_grd(GtkWidget **window)
{
	GtkWidget *del_grd = gtk_grid_new();
	return del_grd;
}

///////////////////////////////////////////////////////////////////////////////

// populate the notebook tab to change password
GtkWidget *create_chg_grd(GtkWidget **window)
{
	GtkWidget *del_grd = gtk_grid_new();
	return del_grd;
}

///////////////////////////////////////////////////////////////////////////////

// populate the notebook tab to view password
GtkWidget *create_see_grd(GtkWidget **window)
{
	GtkWidget *del_grd = gtk_grid_new();
	return del_grd;
}

///////////////////////////////////////////////////////////////////////////////

// populate the notebook tab to change passphrase
GtkWidget *create_cpp_grd(GtkWidget **window)
{
	GtkWidget *del_grd = gtk_grid_new();
	return del_grd;
}

///////////////////////////////////////////////////////////////////////////////

// write credentials to file
void add_password(GtkWidget *widget, gpointer data)
{
	// get the window in which tooltips will be shown
	GtkWidget **window = data;

	// read provided information
	char const *site  = get_credentials_site();
	char const *uname = get_credentials_uname();
	char const *pw    = get_credentials_pw();

	// calculate required string length
	// 2 extra characters required to separate `site', `uname' and `pw'
	// 1 extra character required for the LF character
	// 1 extra character required for the null character added by `sprintf'
	// total 4 extra characters required
	size_t line_length = strlen(site) + strlen(uname) + strlen(pw) + 4;

	// do not allow extremely long credentials
	if(line_length > MAX_BUF_SIZ)
	{
		gtk_widget_set_tooltip_text(*window, "Cannot add password. Credentials entered are too long.");
		g_timeout_add(8 * G_TIME_SPAN_MILLISECOND, hide_tooltip, *window);
		return;
	}

	// create string of credentials in the form described above
	// use a non-printable ASCII character as the separator
	// write `line_length - 1' characters to the file
	// i.e. write everything in `line' except the final null character
	char *line = malloc(line_length * sizeof *line);
	sprintf(line, "%s\x1b%s\x1b%s\n", site, uname, pw);
	FILE *pw_file = fopen(Slave, "ab");
	fwrite(line, sizeof *line, line_length - 1, pw_file);
	fclose(pw_file);
	del_credentials();

	// display success message
	gtk_widget_set_tooltip_text(*window, "Password added successfully.");
	g_timeout_add(8 * G_TIME_SPAN_MILLISECOND, hide_tooltip, *window);
}

///////////////////////////////////////////////////////////////////////////////

void quit_choice(void)
{
	del_credentials();
	free(credentials);
	gtk_main_quit();
	exit(0);
}

