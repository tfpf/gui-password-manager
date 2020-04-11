/*-----------------------------------------------------------------------------
Displays a window in which the user can enter their passphrase.
-----------------------------------------------------------------------------*/
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
	gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(grid, TRUE);
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
	set_credentials(NULL, NULL, pw_entry, NULL);
	credentials->kek = NULL;

	// button
	GtkWidget *login_button = gtk_button_new_with_label("Log In");
	g_signal_connect(GTK_BUTTON(login_button), "clicked", G_CALLBACK(validate_passphrase), window);
	gtk_grid_attach(GTK_GRID(grid), login_button, 0, 2, 2, 1);

	// display everything
	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(quit_passphrase), NULL);
	gtk_widget_grab_focus(pw_entry);
	gtk_main();
}

/*-----------------------------------------------------------------------------
Compare repeated SHA512 hashed of the passphrase entered by the user against
the SHA512 stored on the drive. If nothing is entered, or if the wrong
passphrase is entered, display a tooltip as an error message. If the correct
passphrase is entered, the key encryption key is determined and stored as a
member of the global struct variable.
-----------------------------------------------------------------------------*/
void validate_passphrase(GtkWidget *widget, gpointer data)
{
	// get the window in which tooltips will be shown
	GtkWidget *window = data;

	// read provided information
	// `get_credentials_pw' returns `gchar const *'
	// but `gchar' is typedef'd to `char' in GTK
	char const *pp = get_credentials_pw();

	// sanity
	if(!strcmp(pp, ""))
	{
		gtk_widget_set_tooltip_text(window, "Cannot log in. \'Passphrase\' field is empty.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, window);
		return;
	}

	// hash the passphrase multiple times
	char unsigned *pph = malloc(SHA512_DIGEST_LENGTH * sizeof *pph);
	SHA512((char unsigned *)pp, strlen(pp), pph);
	for(int i = 0; i < 65535; ++i)
	{
		SHA512(pph, SHA512_DIGEST_LENGTH, pph);
	}
	char *pph_hex = digest_to_hexdigest(pph, SHA512_DIGEST_LENGTH);

	// read stored information
	// `pph_s' is the stored hexdigest of the SHA512 of the passphrase
	// hence, it must be 257 characters long (remember null byte)
	char *pph_s = malloc((2 * SHA512_DIGEST_LENGTH + 1) * sizeof *pph_s);
	FILE *pp_file = fopen(Master, "r");
	fgets(pph_s, 2 * SHA512_DIGEST_LENGTH + 1, pp_file);
	fclose(pp_file);

	// compare
	if(strcmp(pph_hex, pph_s))
	{
		del_credentials();
		free(pph);
		free(pph_hex);
		free(pph_s);
		gtk_widget_set_tooltip_text(window, "Cannot log in. Wrong passphrase entered.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, window);
		return;
	}

	// on success, store a hash of the passphrase
	// it will be used as the key encryption key
	char unsigned *kek = malloc(SHA256_DIGEST_LENGTH * sizeof *kek);
	SHA256((char unsigned *)pp, strlen(pp), kek);
	credentials->kek = kek;

	// trash the data and close the login so that the program can proceed
	del_credentials();
	free(pph);
	free(pph_hex);
	free(pph_s);
	gtk_widget_destroy(window);

	// read all items into RAM
	set_list();
}

/*-----------------------------------------------------------------------------
Clear the data stored in the global struct variable and exit from the GTK
window loop. Find out the status (whether the user entered the correct
passphrase or not) by checking whether the key encryption key has been set or
not. If user failed to log in, quit the application.
-----------------------------------------------------------------------------*/
void quit_passphrase(void)
{
	del_credentials();
	gtk_main_quit();
	if(credentials->kek == NULL)
	{
		free(credentials);
		exit(0);
	}
}

