#include <gtk/gtk.h>
#include <stdio.h> // TODO remove this
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

char const *Master = "g45p98f98ur.yew";
char const *Slaves = "wnryff.rgw";
gboolean exit_status = TRUE;

///////////////////////////////////////////////////////////////////////////////

// wrap entry buffers which contain the user's information
typedef struct
{
	GtkEntryBuffer *uname;
	GtkEntryBuffer *pw;
}
credentials_t;
credentials_t *credentials;

// given some entry buffers, associate them with the struct
void set_credentials(GtkWidget *uname_entry, GtkWidget *pw_entry)
{
	credentials = malloc(sizeof *credentials);
	credentials->uname = gtk_entry_get_buffer(GTK_ENTRY(uname_entry));
	credentials->pw = gtk_entry_get_buffer(GTK_ENTRY(pw_entry));
}

// trash the user's information
// as per the GTK source code, calling `g_object_unref' zeros buffer data
void del_credentials(void)
{
	g_object_unref(credentials->uname);
	g_object_unref(credentials->pw);
	free(credentials);
}

///////////////////////////////////////////////////////////////////////////////

// close the window
void quit_passphrase(void)
{
	del_credentials();
	gtk_main_quit();
	exit(0);
}

// compare the provided information with the store information
void validate_passphrase(GtkWidget *widget, gpointer data)
{
	// read provided information
	gchar const *uname = gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->uname));
	gchar const *pw = gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->pw));

	// read stored information
	char *uname_s = malloc(BUFFER_SIZE * sizeof *uname_s);
	char *pw_s = malloc(BUFFER_SIZE * sizeof *pw_s);
	FILE *pp_file = fopen(Master, "r");
	fgets(uname_s, BUFFER_SIZE, pp_file);
	uname_s[strcspn(uname_s, "\r\n")] = '\0';
	fgets(pw_s, BUFFER_SIZE, pp_file);
	pw_s[strcspn(pw_s, "\r\n")] = '\0';

	// compare
	gboolean *exit_status = data;
	if(strcmp(uname, uname_s) || strcmp(pw, pw_s))
	{
		printf("Username or password may be wrong.\n");
		*exit_status = FALSE;
	}
	// del_credentials();
	gtk_main_quit();
}

// ask user to enter their information
gboolean request_passphrase(void)
{
	// window
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 50);
	// gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
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
	gtk_label_set_markup(GTK_LABEL(main_label), "<b>Enter your passphrase to log in.</b>");
	gtk_grid_attach(GTK_GRID(grid), main_label, 0, 0, 2, 1);

	// username
	GtkWidget *uname_label = gtk_label_new("Username");
	gtk_grid_attach(GTK_GRID(grid), uname_label, 0, 1, 1, 1);
	GtkWidget *uname_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid), uname_entry, 1, 1, 1, 1);

	// password
	GtkWidget *pw_label = gtk_label_new("Passphrase");
	gtk_grid_attach(GTK_GRID(grid), pw_label, 0, 2, 1, 1);
	GtkWidget *pw_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid), pw_entry, 1, 2, 1, 1);

	// wrap username and password entries in a struct
	set_credentials(uname_entry, pw_entry);

	// button
	GtkWidget *login_button = gtk_button_new_with_label("Log In");
	// gtk_widget_set_halign(login_button, GTK_ALIGN_CENTER);
	// gtk_widget_set_hexpand(login_button, FALSE);
	g_signal_connect(GTK_BUTTON(login_button), "clicked", G_CALLBACK(validate_passphrase), &exit_status);
	gtk_grid_attach(GTK_GRID(grid), login_button, 0, 3, 2, 1);

	// display everything
	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(quit_passphrase), NULL);
	gtk_main();

	return exit_status;
}

///////////////////////////////////////////////////////////////////////////////

int main(int const argc, char const *argv[])
{
	// must be called before any other GTK functions
	// normally, arguments are pointers to `argc' and `argv'
	gtk_init(0, NULL);

	// if user fails to log in, quit
	while(request_passphrase() == FALSE)
	{
		// gtk_main_quit();
	}

	return 0;
}

