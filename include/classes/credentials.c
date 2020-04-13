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
	credentials->site  =  site_entry == NULL ? NULL : gtk_entry_get_buffer(GTK_ENTRY(site_entry));
	credentials->uname = uname_entry == NULL ? NULL : gtk_entry_get_buffer(GTK_ENTRY(uname_entry));
	credentials->pw    =    pw_entry == NULL ? NULL : gtk_entry_get_buffer(GTK_ENTRY(pw_entry));
	credentials->cp    =    cp_entry == NULL ? NULL : gtk_entry_get_buffer(GTK_ENTRY(cp_entry));
}

/*-----------------------------------------------------------------------------
Obtain the website from the correct GTK entry buffer.
-----------------------------------------------------------------------------*/
gchar const *get_credentials_site(void)
{
	if(credentials->site == NULL)
	{
		return "";
	}

	return gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->site));
}

/*-----------------------------------------------------------------------------
Obtain the username from the correct GTK entry buffer.
-----------------------------------------------------------------------------*/
gchar const *get_credentials_uname(void)
{
	if(credentials->uname == NULL)
	{
		return "";
	}

	return gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->uname));
}

/*-----------------------------------------------------------------------------
Obtain the password from the correct GTK entry buffer.
-----------------------------------------------------------------------------*/
gchar const *get_credentials_pw(void)
{
	if(credentials->pw == NULL)
	{
		return "";
	}

	return gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->pw));
}

/*-----------------------------------------------------------------------------
Obtain the password entered for the second time from the correct GTK entry
buffer.
-----------------------------------------------------------------------------*/
gchar const *get_credentials_cp(void)
{
	if(credentials->cp == NULL)
	{
		return "";
	}

	return gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->cp));
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
	printf("%10s: %s\n", "site",  get_credentials_site());
	printf("%10s: %s\n", "uname", get_credentials_uname());
	printf("%10s: %s\n", "pw",    get_credentials_pw());
	printf("%10s: %s\n", "cp",    get_credentials_cp());
}

/*-----------------------------------------------------------------------------
Clear the data stored in the global struct variable. Normally, GTK
automatically zeros out an object when the reference count of the object drops
to zero, which may not happen on calling `g_object_unref'. Hence, the contents
are manually zeroed out. It is worth noting that clearing happens in place--if
any of the `get_credentials_*' functions were called before calling this
function, the pointers returned by those functions will not have the old data
intact. Also note that there is no need to deallocate the struct members
because they are automatically deallocated when the GTK object is destroyed.
-----------------------------------------------------------------------------*/
void del_credentials(void)
{
	if(credentials == NULL)
	{
		return;
	}

	if(GTK_IS_ENTRY_BUFFER(credentials->site)  == TRUE) gtk_entry_buffer_delete_text(credentials->site,  0, -1);
	if(GTK_IS_ENTRY_BUFFER(credentials->uname) == TRUE) gtk_entry_buffer_delete_text(credentials->uname, 0, -1);
	if(GTK_IS_ENTRY_BUFFER(credentials->pw)    == TRUE) gtk_entry_buffer_delete_text(credentials->pw,    0, -1);
	if(GTK_IS_ENTRY_BUFFER(credentials->cp)    == TRUE) gtk_entry_buffer_delete_text(credentials->cp,    0, -1);
	set_credentials(NULL, NULL, NULL, NULL);
}

