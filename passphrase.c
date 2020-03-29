#include <gtk/gtk.h>
#include <openssl/sha.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////

char const *Master = ".Master";
char const *Slaves = ".Slave";

///////////////////////////////////////////////////////////////////////////////

// prototypes
void request_passphrase(void);
void validate_passphrase(GtkWidget *widget, gpointer data);
void quit_passphrase(void);
void digest_to_hexdigest(char unsigned **p);
gboolean hide_tooltip(gpointer data);

///////////////////////////////////////////////////////////////////////////////

// show a window to let the user enter their passphrase
void request_passphrase(void)
{
	// window
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 0);
	gtk_window_set_icon_from_file(GTK_WINDOW(window), "favicon.png", NULL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_title(GTK_WINDOW(window), "Password Manager");

	// grid
	GtkWidget *grid = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(grid), 50);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 25);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 25);
	gtk_container_add(GTK_CONTAINER(window), grid);

	// header
	GtkWidget *main_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(main_label), "<b>Enter your passphrase to log in.</b>");
	gtk_grid_attach(GTK_GRID(grid), main_label, 0, 0, 2, 1);

	// passphrase
	GtkWidget *pw_label = gtk_label_new("Passphrase");
	gtk_grid_attach(GTK_GRID(grid), pw_label, 0, 1, 1, 1);
	GtkWidget *pw_entry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(pw_entry), FALSE);
	gtk_grid_attach(GTK_GRID(grid), pw_entry, 1, 1, 1, 1);

	// set the information in the global struct variable
	set_credentials(NULL, NULL, pw_entry);
	credentials->status = FALSE;

	// button
	GtkWidget *login_button = gtk_button_new_with_label("Log In");
	// gtk_widget_set_halign(login_button, GTK_ALIGN_CENTER);
	// gtk_widget_set_hexpand(login_button, FALSE);
	g_signal_connect(GTK_BUTTON(login_button), "clicked", G_CALLBACK(validate_passphrase), &window);
	gtk_grid_attach(GTK_GRID(grid), login_button, 0, 2, 2, 1);

	// display everything
	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(quit_passphrase), NULL);
	gtk_main();
}

///////////////////////////////////////////////////////////////////////////////

// compare the provided information with the stored information
void validate_passphrase(GtkWidget *widget, gpointer data)
{
	// get the window in which tooltips will be shown
	GtkWidget **window = data;

	// read provided information
	// `gtk_entry_buffer_get_text' returns `gchar const'
	// `gchar' is typedef'd to `char'
	char const *pw = gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->pw));

	// sanity
	if(!strcmp(pw, ""))
	{
		gtk_widget_set_tooltip_text(*window, "Cannot log in. \'Passphrase\' field is empty");
		g_timeout_add(8 * G_TIME_SPAN_MILLISECOND, hide_tooltip, *window);
		return;
	}

	// hash the passphrase
	char unsigned *pwh = malloc(SHA512_DIGEST_LENGTH * sizeof *pwh);
	SHA512((char unsigned *)pw, strlen(pw), pwh);
	digest_to_hexdigest(&pwh);

	// read stored information
	// `pwh_s' is the stored hexdigest of the SHA512 of the passphrase
	// hence, it must be 257 characters long (remember null byte)
	char *pwh_s = malloc(2 * SHA512_DIGEST_LENGTH * sizeof *pwh_s + 1);
	FILE *pp_file = fopen(Master, "r");
	fgets(pwh_s, 2 * SHA512_DIGEST_LENGTH + 1, pp_file);

	// compare
	if(strcmp((char *)pwh, pwh_s))
	{
		del_credentials();
		memset(pwh  , 0, 2 * SHA512_DIGEST_LENGTH + 1);
		memset(pwh_s, 0, 2 * SHA512_DIGEST_LENGTH + 1);
		gtk_widget_set_tooltip_text(*window, "Cannot log in. Wrong passphrase entered.");
		g_timeout_add(8 * G_TIME_SPAN_MILLISECOND, hide_tooltip, *window);
		return;
	}

	// on success, trash the data and close the login
	// so that the program can proceed
	del_credentials();
	memset(pwh  , 0, 2 * SHA512_DIGEST_LENGTH + 1);
	memset(pwh_s, 0, 2 * SHA512_DIGEST_LENGTH + 1);
	credentials->status = TRUE;
	gtk_widget_destroy(*window);
}

///////////////////////////////////////////////////////////////////////////////

// hide the tooltip, if any
gboolean hide_tooltip(gpointer data)
{
	GtkWidget *window = data;
	gtk_widget_set_has_tooltip(window, FALSE);
	return G_SOURCE_REMOVE;
}

///////////////////////////////////////////////////////////////////////////////

// called automatically when the window is destroyed
void quit_passphrase(void)
{
	// trash sensitive data, then exit the GTK loop
	del_credentials();
	free(credentials);
	gtk_main_quit();

	// if user failed to enter correct passphrase, quit the program
	if(credentials->status == FALSE)
	{
		exit(0);
	}
}

