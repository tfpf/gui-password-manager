/*-----------------------------------------------------------------------------
Store the GTK entry buffers corresponding to the input GTK entries in the
global struct variable. Think of this function as an initialiser of sorts--a
constructor, if you will.
-----------------------------------------------------------------------------*/
void set_credentials(GtkWidget *site_entry, GtkWidget *uname_entry, GtkWidget *pw_entry, GtkWidget *cp_entry)
{
	// if the struct previously held some data, remove it
	// sensitive data is cleared when `del_credentials' is called
	// hence, just deallocating memory is enough here
	if(credentials != NULL)
	{
		free(credentials);
	}

	// store new data
	credentials = malloc(sizeof *credentials);
	credentials->site  =  site_entry == NULL ? "" : gtk_entry_get_text(GTK_ENTRY(site_entry));
	credentials->uname = uname_entry == NULL ? "" : gtk_entry_get_text(GTK_ENTRY(uname_entry));
	credentials->pw    =    pw_entry == NULL ? "" : gtk_entry_get_text(GTK_ENTRY(pw_entry));
	credentials->cp    =    cp_entry == NULL ? "" : gtk_entry_get_text(GTK_ENTRY(cp_entry));
}

/*-----------------------------------------------------------------------------
Obtain the website from the correct GTK entry buffer.
-----------------------------------------------------------------------------*/
gchar const *get_credentials_site(void)
{
	return credentials->site;
}

/*-----------------------------------------------------------------------------
Obtain the username from the correct GTK entry buffer.
-----------------------------------------------------------------------------*/
gchar const *get_credentials_uname(void)
{
	return credentials->uname;
}

/*-----------------------------------------------------------------------------
Obtain the password from the correct GTK entry buffer.
-----------------------------------------------------------------------------*/
gchar const *get_credentials_pw(void)
{
	return credentials->pw;
}

/*-----------------------------------------------------------------------------
Obtain the password entered for the second time from the correct GTK entry
buffer.
-----------------------------------------------------------------------------*/
gchar const *get_credentials_cp(void)
{
	return credentials->cp;
}

/*-----------------------------------------------------------------------------
Obtain the key encryption key. This is stored as a member of the global struct
variable after the user has entered the correct passphrase.
-----------------------------------------------------------------------------*/
char unsigned *get_credentials_kek(void)
{
	return credentials->kek;
}

/*-----------------------------------------------------------------------------
Display the contents of the global struct variable. This function is not
supposed to be used in the final code. It is only for debug prints so that I
can see what data is stored in the global struct variable.
-----------------------------------------------------------------------------*/
void see_credentials(void)
{
	fprintf(stdout, "%10s: %s\n", "site",  get_credentials_site());
	fprintf(stdout, "%10s: %s\n", "uname", get_credentials_uname());
	fprintf(stdout, "%10s: %s\n", "pw",    get_credentials_pw());
	fprintf(stdout, "%10s: %s\n", "cp",    get_credentials_cp());
}

