#include <gtk/gtk.h>
#include <openssl/sha.h>
#include <stdio.h> // TODO remove this
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 1024

char const *Master = "g45p98f98ur.yew";
char const *Slaves = "wnryff.rgw";
gboolean exit_status = TRUE;

///////////////////////////////////////////////////////////////////////////////

// block the program for some amount of time
void wait(int unsigned delay)
{
	int unsigned end_time = time(0) + delay;
	while(time(0) < end_time);
}

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
void digest_to_hexdigest(char unsigned **p);
gboolean hide_tooltip(gpointer data);

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

	// passphrase
	GtkWidget *pw_label = gtk_label_new("Passphrase");
	gtk_grid_attach(GTK_GRID(grid), pw_label, 0, 1, 1, 1);
	GtkWidget *pw_entry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(pw_entry), FALSE);
	gtk_grid_attach(GTK_GRID(grid), pw_entry, 1, 1, 1, 1);

	// wrap username and password entries in a struct
	// there is no website or username here, so just pass a null pointer
	set_credentials(NULL, NULL, pw_entry);

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
		memset(pwh_s, 0, 2 * SHA512_DIGEST_LENGTH + 1);
		gtk_widget_set_tooltip_text(*window, "Cannot log in. Wrong passphrase entered.");
		g_timeout_add(8 * G_TIME_SPAN_MILLISECOND, hide_tooltip, *window);
		return;
	}

	// on success, trash the data and close the login
	// so that the program can proceed
	del_credentials();
	memset(pwh_s, 0, 2 * SHA512_DIGEST_LENGTH + 1);
	gtk_main_quit();
}

gboolean hide_tooltip(gpointer data)
{
	GtkWidget *widget = data;
	gtk_widget_set_has_tooltip(widget, FALSE);
	return G_SOURCE_REMOVE;
}

// close the window
void quit_passphrase(void)
{
	del_credentials();
	free(credentials);
	gtk_main_quit();
	exit(0);
}

void digest_to_hexdigest(char unsigned **p)
{
	// `digest' is (basically) a 512-bit number
	// thus, it is represented using 128 bytes
	// `hexdigest' is hexadecimal form of the same number
	// it will be 257 bytes long
	// because two hex characters are required to represent one byte
	// plus one extra byte, for the null character
	char unsigned *digest = *p;
	char unsigned *hexdigest = malloc(2 * SHA512_DIGEST_LENGTH * sizeof *hexdigest + 1);

	// for each byte in `digest', write two bytes in `hexdigest'
	for(int i = 0; i < SHA512_DIGEST_LENGTH; ++i)
	{
		sprintf((char *)hexdigest + 2 * i, "%02x", digest[i]);
	}

	// make the argument point to `hexdigest'
	memset(digest, 0, SHA512_DIGEST_LENGTH);
	*p = hexdigest;
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

