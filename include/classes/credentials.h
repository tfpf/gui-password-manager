// a convenient way to access and delete the sensitive data from anywhere
struct
{
	// entry buffers of entries which contain data
	char const *site;
	char const *uname;
	char const *pw;
	char const *cp;

	// key encryption key which will be used to encrypt the AES keys
	// which themselves will be used to encrypt the data
	char unsigned *kek;
}
*credentials = NULL;

///////////////////////////////////////////////////////////////////////////////

// prototypes
void set_credentials(GtkWidget *site_entry, GtkWidget *uname_entry, GtkWidget *pw_entry, GtkWidget *cp_entry);
void see_credentials(void);
void del_credentials(void);
gchar const *get_credentials_site(void);
gchar const *get_credentials_uname(void);
gchar const *get_credentials_pw(void);

#include "credentials.c"

