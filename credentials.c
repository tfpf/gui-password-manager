#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////

// entry buffers of entries which contain data are wrapped into this struct
// a convenient way to access and delete the sensitive data from anywhere
typedef struct
{
	GtkEntryBuffer *site;
	GtkEntryBuffer *uname;
	GtkEntryBuffer *pw;
}
credentials_t;
credentials_t *credentials;

///////////////////////////////////////////////////////////////////////////////

// prototypes
void set_credentials(GtkWidget *site_entry, GtkWidget *uname_entry, GtkWidget *pw_entry);
void see_credentials(void);
void del_credentials(void);

///////////////////////////////////////////////////////////////////////////////

// given some entry buffers, associate them with the struct
// this is like an initialiser of sorts--a constructor, if you will
// inputs are entries
// however, the struct stores entry buffers, not entries
void set_credentials(GtkWidget *site_entry, GtkWidget *uname_entry, GtkWidget *pw_entry)
{
	// if the struct previously held some data, remove it
	del_credentials();
	if(credentials != NULL)
	{
		if(credentials->site  != NULL) free(credentials->site);
		if(credentials->uname != NULL) free(credentials->uname);
		if(credentials->pw    != NULL) free(credentials->pw);
		free(credentials);
	}

	// store new data
	credentials = malloc(sizeof *credentials);
	credentials->site  =  site_entry == NULL ? NULL : gtk_entry_get_buffer(GTK_ENTRY(site_entry));
	credentials->uname = uname_entry == NULL ? NULL : gtk_entry_get_buffer(GTK_ENTRY(uname_entry));
	credentials->pw    =    pw_entry == NULL ? NULL : gtk_entry_get_buffer(GTK_ENTRY(pw_entry));
}

///////////////////////////////////////////////////////////////////////////////

// this function is not supposed to be used in the final code
// it is only for debug prints so that I can see what data is stored
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

///////////////////////////////////////////////////////////////////////////////

// trash the information
// when the reference count drops to zero, the object is zeroed automatically
// calling `g_object_unref' here may not drop the reference count to zero
// hence, I manually zero out the entry buffer text
// if `gtk_entry_buffer_get_text' was used to get the text earlier, it is okay
// because even that string gets zeroed when `del_credentials' is called
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

