#include <gtk/gtk.h>
#include <stdio.h> // TODO remove this
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

char const *Master = "g45p98f98ur.yew";
char const *Slaves = "wnryff.rgw";
gboolean exit_status = TRUE;

///////////////////////////////////////////////////////////////////////////////

// entry buffers of entries which contain the data are wrapped into this struct
typedef struct
{
	GtkEntryBuffer *site;
	GtkEntryBuffer *uname;
	GtkEntryBuffer *pw;
}
credentials_t;
credentials_t *credentials;

// given some entry buffers, associate them with the struct
// this is like an initialiser of sorts--a constructor, if you will
// inputs are entries
// however, the struct stores entry buffers, not entries
void set_credentials(GtkWidget *site_entry, GtkWidget *uname_entry, GtkWidget *pw_entry)
{
	credentials = malloc(sizeof *credentials);
	credentials->site  =  site_entry == NULL ? NULL : gtk_entry_get_buffer(GTK_ENTRY(site_entry));
	credentials->uname = uname_entry == NULL ? NULL : gtk_entry_get_buffer(GTK_ENTRY(uname_entry));
	credentials->pw    =    pw_entry == NULL ? NULL : gtk_entry_get_buffer(GTK_ENTRY(pw_entry));
}

// only for debugging
// not supposed to be used in the final code
void see_credentials(void)
{
	if(credentials == NULL)
	{
		return;
	}

	if(credentials->site  != NULL) printf("%10s: %s\n", "site",  gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->site)));
	if(credentials->uname != NULL) printf("%10s: %s\n", "uname", gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->uname)));
	if(credentials->pw    != NULL) printf("%10s: %s\n", "pw",    gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->pw)));
}

// trash the information
// when the reference count drops to zero, the object is zeroed automatically
// calling `g_object_unref' here may not drop the reference count to zero
// hence, I manually zero out the entry buffer text
// strings obtained by `gtk_entry_buffer_get_text' will also be zeroed in place
void del_credentials(void)
{
	if(credentials == NULL)
	{
		return;
	}

	if(credentials->site  != NULL) gtk_entry_buffer_delete_text(credentials->site,  0, -1);
	if(credentials->uname != NULL) gtk_entry_buffer_delete_text(credentials->uname, 0, -1);
	if(credentials->pw    != NULL) gtk_entry_buffer_delete_text(credentials->pw,    0, -1);
}

///////////////////////////////////////////////////////////////////////////////

// prototypes
void request_passphrase(void);
void validate_passphrase(GtkWidget *widget, gpointer data);
void quit_passphrase(void);

// ask user to enter their information
void request_passphrase(void)
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
	// there is no website here, so just pass a null pointer
	set_credentials(NULL, uname_entry, pw_entry);

	// button
	GtkWidget *login_button = gtk_button_new_with_label("Log In");
	// gtk_widget_set_halign(login_button, GTK_ALIGN_CENTER);
	// gtk_widget_set_hexpand(login_button, FALSE);
	g_signal_connect(GTK_BUTTON(login_button), "clicked", G_CALLBACK(validate_passphrase), &window);
	gtk_grid_attach(GTK_GRID(grid), login_button, 0, 3, 2, 1);

	// display everything
	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(quit_passphrase), NULL);
	gtk_main();
}

// compare the provided information with the stored information
void validate_passphrase(GtkWidget *widget, gpointer data)
{
	// read provided information
	// `gtk_entry_buffer_get_text' returns `gchar const'
	// `gchar' is typedef'd to `char'
	char const *uname = gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->uname));
	char const *pw    = gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->pw));

	// read stored information
	char *uname_s = malloc(BUFFER_SIZE * sizeof *uname_s);
	char *pw_s    = malloc(BUFFER_SIZE * sizeof *pw_s);
	FILE *pp_file = fopen(Master, "r");
	fgets(uname_s, BUFFER_SIZE, pp_file);
	uname_s[strcspn(uname_s, "\r\n")] = '\0';
	fgets(pw_s, BUFFER_SIZE, pp_file);
	pw_s[strcspn(pw_s, "\r\n")] = '\0';

	// compare
	if(strcmp(uname, uname_s) || strcmp(pw, pw_s))
	{
		del_credentials();
		memset(uname_s, 0, BUFFER_SIZE);
		memset(pw_s,    0, BUFFER_SIZE);
		printf("Authentication failure! :(\n");
		// GtkWidget **window = data;
		// GtkWidget *err = gtk_dialog_new_with_buttons("Authentication Error", GTK_WINDOW(*window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "OK", GTK_RESPONSE_ACCEPT, NULL);
		// gtk_dialog_run(GTK_DIALOG(err));
		return;
	}

	// on success, trash the data and close the login
	// so that the program can proceed
	printf("Authentication success! :)\n");
	del_credentials();
	memset(uname_s, 0, BUFFER_SIZE);
	memset(pw_s,    0, BUFFER_SIZE);
	gtk_main_quit();
}

// close the window
void quit_passphrase(void)
{
	del_credentials();
	free(credentials);
	gtk_main_quit();
	exit(0);
}

///////////////////////////////////////////////////////////////////////////////

int main(int const argc, char const *argv[])
{
	// disable output buffering
	setvbuf(stdout, NULL, _IONBF, 0);

	// must be called before any other GTK functions
	// normally, arguments are pointers to `argc' and `argv'
	gtk_init(0, NULL);

	request_passphrase();

	return 0;
}

