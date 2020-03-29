#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUF_SIZ 1

///////////////////////////////////////////////////////////////////////////////

char const *Slave = ".Slave";

///////////////////////////////////////////////////////////////////////////////

// prototypes
void request_choice(void);
void quit_choice(void);
void add_password(GtkWidget *widget, gpointer data);
GtkWidget *create_add_grd(GtkWidget **window);

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

	// notebook
	GtkWidget *notebook = gtk_notebook_new();

	// notebook tab to add password
	GtkWidget *add_grd = create_add_grd(&window);
	GtkWidget *add_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(add_lbl), "<span size=\"7500\" weight=\"ultralight\">Add Password</span>");
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), add_grd, add_lbl);
	gtk_container_add(GTK_CONTAINER(window), notebook);
	// GtkWidget *add_lbl = gtk_label_new(NULL);
	// gtk_label_set_markup(GTK_LABEL(add_lbl), "<span size=\"7500\" weight=\"ultralight\">Add Password</span>");
	// GtkWidget *add_grd = gtk_grid_new();
	// gtk_container_set_border_width(GTK_CONTAINER(add_grd), 50);
	// gtk_grid_set_column_spacing(GTK_GRID(add_grd), 25);
	// gtk_grid_set_row_spacing(GTK_GRID(add_grd), 25);
	// gtk_widget_set_halign(add_grd, GTK_ALIGN_CENTER);
	// gtk_widget_set_hexpand(add_grd, TRUE);
	// // populate_add_grd(add_grd, window);
	// GtkWidget *add_btn = gtk_button_new_with_label("Add Password");
	// g_signal_connect(GTK_BUTTON(add_btn), "clicked", G_CALLBACK(add_password), &window);
	// gtk_grid_attach(GTK_GRID(add_grd), add_btn, 0, 4, 2, 1);

	// notebook
	// gtk_notebook_append_page(GTK_NOTEBOOK(notebook), add_grd, add_lbl);
	// gtk_container_add(GTK_CONTAINER(window), notebook);

	// show everything
	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(quit_choice), NULL);
	gtk_main();
}

///////////////////////////////////////////////////////////////////////////////

// fill notebook tab to add password
GtkWidget *create_add_grd(GtkWidget **window)
{
	GtkWidget *add_grd = gtk_grid_new();
	GtkWidget *add_btn = gtk_button_new_with_label("Add Password");
	g_signal_connect(GTK_BUTTON(add_btn), "clicked", G_CALLBACK(add_password), window);
	gtk_grid_attach(GTK_GRID(add_grd), add_btn, 0, 0, 1, 1);
	return add_grd;
	// // header
	// GtkWidget *main_label = gtk_label_new(NULL);
	// gtk_label_set_markup(GTK_LABEL(main_label), "<b>Enter the following details to add a new password.</b>");
	// gtk_grid_attach(GTK_GRID(add_grd), main_label, 0, 0, 2, 1);

	// // website
	// GtkWidget *site_label = gtk_label_new("Website");
	// gtk_grid_attach(GTK_GRID(add_grd), site_label, 0, 1, 1, 1);
	// GtkWidget *site_entry = gtk_entry_new();
	// gtk_entry_set_placeholder_text(GTK_ENTRY(site_entry), "e.g. Facebook, Stack Overflow");
	// gtk_grid_attach(GTK_GRID(add_grd), site_entry, 1, 1, 1, 1);

	// // username
	// GtkWidget *uname_label = gtk_label_new("Username");
	// gtk_grid_attach(GTK_GRID(add_grd), uname_label, 0, 2, 1, 1);
	// GtkWidget *uname_entry = gtk_entry_new();
	// gtk_grid_attach(GTK_GRID(add_grd), uname_entry, 1, 2, 1, 1);

	// // password
	// GtkWidget *pw_label = gtk_label_new("Password");
	// gtk_grid_attach(GTK_GRID(add_grd), pw_label, 0, 3, 1, 1);
	// GtkWidget *pw_entry = gtk_entry_new();
	// gtk_entry_set_visibility(GTK_ENTRY(pw_entry), FALSE);
	// gtk_grid_attach(GTK_GRID(add_grd), pw_entry, 1, 3, 1, 1);

	// // set the information in the global struct variable
	// set_credentials(site_entry, uname_entry, pw_entry);

	// // button
	// GtkWidget *add_grd = gtk_grid_new();
	// GtkWidget *add_lbl = gtk_label_new(NULL);
	// GtkWidget *add_btn = gtk_button_new_with_label("Add Password");
	// g_signal_connect(GTK_BUTTON(add_btn), "clicked", G_CALLBACK(add_password), &window);
	// gtk_grid_attach(GTK_GRID(add_grd), add_btn, 0, 4, 2, 1);
}

///////////////////////////////////////////////////////////////////////////////

// write credentials to file
void add_password(GtkWidget *widget, gpointer data)
{
	// window containing the notebook
	GtkWidget **window = data;
	gtk_widget_set_tooltip_text(*window, "Cannot add password. Credentials entered are too long.");
	return;

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
		printf("too long\n");
		gtk_widget_set_tooltip_text(*window, "Cannot add password. Credentials entered are too long.");
		g_timeout_add(8 * G_TIME_SPAN_MILLISECOND, hide_tooltip, NULL);
		return;
	}

	// create string of credentials in the form described above
	// use a non-printable ASCII character as the separator
	char *line = malloc(line_length * sizeof *line);
	sprintf(line, "%s\x1b%s\x1b%s\n", site, uname, pw);

	FILE *pw_file = fopen(Slave, "ab");
	fwrite(line, sizeof *line, line_length - 1, pw_file);
	// fwrite(site, sizeof *site, strlen(site), pw_file);
	// fwrite("\x1b", 1, 1, pw_file);
	// fwrite(uname, sizeof *uname, strlen(uname), pw_file);
	// fwrite("\x1b", 1, 1, pw_file);
	// fwrite(pw, sizeof *pw, strlen(pw), pw_file);
	// fwrite("\n", 1, 1, pw_file);
	fclose(pw_file);

	pw_file = fopen(Slave, "rb");
	fgets(line, line_length + 100, pw_file);
	printf("%c", line[4]);
	fclose(pw_file);
}

///////////////////////////////////////////////////////////////////////////////

void quit_choice(void)
{
	del_credentials();
	free(credentials);
	gtk_main_quit();
	exit(0);
}

