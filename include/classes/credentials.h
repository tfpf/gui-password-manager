// a convenient way to access and delete the sensitive data from anywhere
typedef struct
{
	// entry buffers of entries which contain data
	GtkEntryBuffer *site;
	GtkEntryBuffer *uname;
	GtkEntryBuffer *pw;

	// key encryption key which will be used to encrypt the AES keys
	// which themselves will be used to encrypt the data
	char unsigned *kek;
}
credentials_t;
credentials_t *credentials;

///////////////////////////////////////////////////////////////////////////////

// prototypes
void set_credentials(GtkWidget *site_entry, GtkWidget *uname_entry, GtkWidget *pw_entry);
void see_credentials(void);
void del_credentials(void);
gchar const *get_credentials_site(void);
gchar const *get_credentials_uname(void);
gchar const *get_credentials_pw(void);

#include "credentials.c"

