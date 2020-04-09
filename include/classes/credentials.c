// given some entry buffers, associate them with the struct
// this is like an initialiser of sorts--a constructor, if you will
// inputs are entries
// however, the struct stores entry buffers, not entries
void set_credentials(GtkWidget *site_entry, GtkWidget *uname_entry, GtkWidget *pw_entry, GtkWidget *cp_entry)
{
	// if the struct previously held some data, remove it
	if(credentials != NULL)
	{
		free(credentials);
	}

	// store new data
	credentials = malloc(sizeof *credentials);
	credentials->site   =  site_entry == NULL ? NULL : gtk_entry_get_buffer(GTK_ENTRY(site_entry));
	credentials->uname  = uname_entry == NULL ? NULL : gtk_entry_get_buffer(GTK_ENTRY(uname_entry));
	credentials->pw     =    pw_entry == NULL ? NULL : gtk_entry_get_buffer(GTK_ENTRY(pw_entry));
	credentials->cp     =    cp_entry == NULL ? NULL : gtk_entry_get_buffer(GTK_ENTRY(cp_entry));
}

///////////////////////////////////////////////////////////////////////////////

// get the website of the login
gchar const *get_credentials_site(void)
{
	if(credentials->site == NULL)
	{
		return "";
	}

	return gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->site));
}

///////////////////////////////////////////////////////////////////////////////

// get the username of the login
gchar const *get_credentials_uname(void)
{
	if(credentials->uname == NULL)
	{
		return "";
	}

	return gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->uname));
}

///////////////////////////////////////////////////////////////////////////////

// get the password of the login
gchar const *get_credentials_pw(void)
{
	if(credentials->pw == NULL)
	{
		return "";
	}

	return gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->pw));
}

///////////////////////////////////////////////////////////////////////////////

// get the password confirmation of the login
gchar const *get_credentials_cp(void)
{
	if(credentials->cp == NULL)
	{
		return "";
	}

	return gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(credentials->cp));
}

///////////////////////////////////////////////////////////////////////////////

// get the key encryption key
char unsigned *get_credentials_kek(void)
{
	return credentials->kek;
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

	printf("%10s: %s\n", "site",  get_credentials_site());
	printf("%10s: %s\n", "uname", get_credentials_uname());
	printf("%10s: %s\n", "pw",    get_credentials_pw());
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
	if(credentials->cp    != NULL) gtk_entry_buffer_delete_text(credentials->cp,    0, -1);
}

