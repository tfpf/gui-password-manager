/*-----------------------------------------------------------------------------
Displays a window in which the user can enter their passphrase.
-----------------------------------------------------------------------------*/
void request_passphrase(void)
{
	// window
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 0);
	gtk_window_set_icon_from_file(GTK_WINDOW(window), icon_main, NULL);
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
	gtk_label_set_markup(GTK_LABEL(main_label), "<span weight=\"bold\" foreground=\"green\">Enter your passphrase to log in.</span>");
	gtk_grid_attach(GTK_GRID(grid), main_label, 0, 0, 3, 1);


	// passphrase
	GtkWidget *pp_label = gtk_label_new("Passphrase");
	gtk_grid_attach(GTK_GRID(grid), pp_label, 0, 1, 1, 1);
	GtkWidget *pp_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(pp_entry), TRUE);
	gtk_entry_set_visibility(GTK_ENTRY(pp_entry), FALSE);
	gtk_widget_grab_focus(pp_entry);
	gtk_grid_attach(GTK_GRID(grid), pp_entry, 1, 1, 1, 1);
	GtkWidget *show_button = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(show_button), gtk_image_new_from_file(icon_visibility));
	gtk_widget_set_tooltip_text(show_button, "Click to show or hide passphrase.");
	g_signal_connect(show_button, "clicked", G_CALLBACK(toggle_visibility), pp_entry);
	gtk_grid_attach(GTK_GRID(grid), show_button, 2, 1, 1, 1);

	// prepare data to be sent to callback function
	GtkWidget **data = malloc(2 * sizeof *data);
	data[0] = window;
	data[1] = pp_entry;

	// button
	GtkWidget *login_button = gtk_button_new_with_label("Log In");
	g_signal_connect(GTK_BUTTON(login_button), "clicked", G_CALLBACK(validate_passphrase), data);
	gtk_grid_attach(GTK_GRID(grid), login_button, 0, 2, 3, 1);
	gtk_widget_set_can_default(login_button, TRUE);
	gtk_widget_grab_default(login_button);

	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(quit_passphrase), NULL);
	gtk_main();
}


/*-----------------------------------------------------------------------------
Compare repeated SHA512 hashes of the passphrase entered by the user against
the SHA512 stored on the drive. If nothing is entered, or if the wrong
passphrase is entered, display a tooltip as an error message. If the correct
passphrase is entered, the key encryption key is determined and and this window
is closed.
-----------------------------------------------------------------------------*/
void validate_passphrase(GtkButton *button, gpointer data)
{
	GtkWidget **callback_data = data;
	GtkWidget *window = callback_data[0];
	char const *pp = gtk_entry_get_text(GTK_ENTRY(callback_data[1]));

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
	for(int i = 0; i < HASH_COUNT; ++i)
	{
		SHA512(pph, SHA512_DIGEST_LENGTH, pph);
	}
	char *pph_hex = digest_to_hexdigest(pph, SHA512_DIGEST_LENGTH);

	// read stored information
	// `pph_s' is the stored hexdigest of the SHA512 of the passphrase
	// hexdigest is twice as long as digest (plus null byte)
	char *pph_s = malloc((2 * SHA512_DIGEST_LENGTH + 1) * sizeof *pph_s);
	FILE *pp_file = fopen(Master, "r");
	fgets(pph_s, 2 * SHA512_DIGEST_LENGTH + 1, pp_file);
	fclose(pp_file);

	// compare
	if(strcmp(pph_hex, pph_s))
	{
		__clear_all_entries(window, NULL);

		// clear RAM
		memset(pph,     0, 1 * SHA512_DIGEST_LENGTH);
		memset(pph_hex, 0, 2 * SHA512_DIGEST_LENGTH);
		memset(pph_s,   0, 2 * SHA512_DIGEST_LENGTH);

		// deallocate
		free(pph);
		free(pph_hex);
		free(pph_s);

		gtk_widget_set_tooltip_text(window, "Cannot log in. Wrong passphrase entered.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, window);
		return;
	}

	// on success, store a hash of the passphrase
	// it will be used as the key encryption key
	kek = malloc(SHA256_DIGEST_LENGTH * sizeof *kek);
	SHA256((char unsigned *)pp, strlen(pp), kek);

	__clear_all_entries(window, NULL);

	// clear RAM
	memset(pph,     0, 1 * SHA512_DIGEST_LENGTH);
	memset(pph_hex, 0, 2 * SHA512_DIGEST_LENGTH);
	memset(pph_s,   0, 2 * SHA512_DIGEST_LENGTH);

	// deallocate
	free(pph);
	free(pph_hex);
	free(pph_s);

	gtk_widget_destroy(window);

	load_list_of_passwords();
}

/*-----------------------------------------------------------------------------
Exit from the GTK window loop. Find out the status (whether the user entered
the correct passphrase or not) by checking whether the key encryption key has
been set or not. If user failed to log in, quit the application.
-----------------------------------------------------------------------------*/
void quit_passphrase(GtkWindow *window, gpointer data)
{
	__clear_all_entries(GTK_WIDGET(window), NULL);
	gtk_main_quit();
	if(kek == NULL)
	{
		exit(EXIT_FAILURE);
	}
}

