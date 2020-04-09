/*-----------------------------------------------------------------------------
Display a window in which the user can choose what they want to do.
-----------------------------------------------------------------------------*/
void request_choice(void)
{
	// window
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 0);
	gtk_window_set_icon_from_file(GTK_WINDOW(window), "favicon.png", NULL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_title(GTK_WINDOW(window), "Password Manager");

	// notebook tab to add password
	GtkWidget *add_grd = create_widget_for_add(&window);
	GtkWidget *add_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(add_lbl), "<span weight=\"normal\">Add Password</span>");

	// notebook tab to edit or delete password
	GtkWidget *chg_grd = create_widget_for_chg(&window);
	GtkWidget *chg_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(chg_lbl), "<span weight=\"normal\">Edit/Delete Password</span>");

	// notebook tab to view password
	GtkWidget *see_box = create_widget_for_see(&window);
	GtkWidget *see_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(see_lbl), "<span weight=\"normal\">View Password</span>");

	// notebook tab to change passphrase
	GtkWidget *cpp_grd = create_widget_for_cpp(&window);
	GtkWidget *cpp_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(cpp_lbl), "<span weight=\"normal\">Change Passphrase</span>");

	// notebook
	GtkWidget *notebook = gtk_notebook_new();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), add_grd, add_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), chg_grd, chg_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), see_box, see_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), cpp_grd, cpp_lbl);
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_LEFT);
	gtk_container_add(GTK_CONTAINER(window), notebook);

	// show everything
	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(quit_choice), NULL);
	gtk_main();
}

/*-----------------------------------------------------------------------------
Populate the 'Add Password' page of the notebook.
-----------------------------------------------------------------------------*/
GtkWidget *create_widget_for_add(GtkWidget **window)
{
	// the grid to return
	GtkWidget *add_grd = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(add_grd), 50);
	gtk_grid_set_column_spacing(GTK_GRID(add_grd), 25);
	gtk_grid_set_row_spacing(GTK_GRID(add_grd), 25);
	gtk_widget_set_halign(add_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(add_grd, TRUE);

	// header
	GtkWidget *main_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(main_label), "<b>Enter the following details to add a new password.</b>");
	gtk_grid_attach(GTK_GRID(add_grd), main_label, 0, 0, 2, 1);

	// website
	GtkWidget *site_label = gtk_label_new("Website");
	gtk_grid_attach(GTK_GRID(add_grd), site_label, 0, 1, 1, 1);
	GtkWidget *site_entry = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(site_entry), "e.g. FB, Twitter");
	gtk_grid_attach(GTK_GRID(add_grd), site_entry, 1, 1, 1, 1);

	// username
	GtkWidget *uname_label = gtk_label_new("Username");
	gtk_grid_attach(GTK_GRID(add_grd), uname_label, 0, 2, 1, 1);
	GtkWidget *uname_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(add_grd), uname_entry, 1, 2, 1, 1);

	// password
	GtkWidget *pw_label = gtk_label_new("Password");
	gtk_grid_attach(GTK_GRID(add_grd), pw_label, 0, 3, 1, 1);
	GtkWidget *pw_entry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(pw_entry), FALSE);
	gtk_grid_attach(GTK_GRID(add_grd), pw_entry, 1, 3, 1, 1);

	// confirm password
	GtkWidget *cp_label = gtk_label_new("Confirm Password");
	gtk_grid_attach(GTK_GRID(add_grd), cp_label, 0, 4, 1, 1);
	GtkWidget *cp_entry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(cp_entry), FALSE);
	gtk_grid_attach(GTK_GRID(add_grd), cp_entry, 1, 4, 1, 1);

	// set the information in the global struct variable
	set_credentials(site_entry, uname_entry, pw_entry, cp_entry);

	// button
	GtkWidget *add_btn = gtk_button_new_with_label("Add Password");
	g_signal_connect(GTK_BUTTON(add_btn), "clicked", G_CALLBACK(add_password), window);
	gtk_grid_attach(GTK_GRID(add_grd), add_btn, 0, 5, 2, 1);

	return add_grd;
}

/*-----------------------------------------------------------------------------
Populate the 'Edit/Delete Password' page of the notebook.
-----------------------------------------------------------------------------*/
GtkWidget *create_widget_for_chg(GtkWidget **window)
{
	// the scrollable window to return
	GtkWidget *chg_scw = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(chg_scw), FALSE);
	gtk_scrolled_window_set_placement(GTK_SCROLLED_WINDOW(chg_scw), GTK_CORNER_TOP_LEFT);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(chg_scw), GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);

	GtkWidget *chg_grd = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(chg_scw), chg_grd);

	for(int i = 0; i < 56; ++i)
	{
		GtkWidget *btn = gtk_button_new_with_label("Test");
		gtk_grid_attach(GTK_GRID(chg_grd), btn, 0, i, 1, 1);
	}

	return chg_scw;
}

/*-----------------------------------------------------------------------------
Populate the 'View Password' page of the notebook.
-----------------------------------------------------------------------------*/
GtkWidget *create_widget_for_see(GtkWidget **window)
{
	// grid to be placed at the top
	GtkWidget *top_grd = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(top_grd), 50);
	gtk_grid_set_column_spacing(GTK_GRID(top_grd), 25);
	gtk_grid_set_row_spacing(GTK_GRID(top_grd), 25);
	gtk_widget_set_halign(top_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(top_grd, TRUE);

	// header
	GtkWidget *main_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(main_label), "<b>Enter a search term to narrow down the list.</b>");
	gtk_grid_attach(GTK_GRID(top_grd), main_label, 0, 0, 2, 1);

	// label
	GtkWidget *search_label = gtk_label_new("Search Term");
	gtk_grid_attach(GTK_GRID(top_grd), search_label, 0, 1, 1, 1);
	GtkWidget *search_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(top_grd), search_entry, 1, 1, 1, 1);

	// scrollable window to be placed at the bottom
	GtkWidget *bot_scw = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_set_border_width(GTK_CONTAINER(bot_scw), 0);
	gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(bot_scw), FALSE);
	gtk_scrolled_window_set_placement(GTK_SCROLLED_WINDOW(bot_scw), GTK_CORNER_TOP_LEFT);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(bot_scw), GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);

	// grid to place in the scrollable window
	GtkWidget *bot_grd = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(bot_grd), 5);
	gtk_grid_set_column_spacing(GTK_GRID(bot_grd), 25);
	gtk_grid_set_row_spacing(GTK_GRID(bot_grd), 5);
	gtk_widget_set_halign(bot_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(bot_grd, TRUE);

	// put the grid in the scrolled window
	gtk_container_add(GTK_CONTAINER(bot_scw), bot_grd);

	// put both `top_grd' and `bot_scw' into a box
	GtkWidget *see_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(GTK_BOX(see_box), top_grd, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(see_box), bot_scw, TRUE, TRUE, 0);

	// whenever the search term is changed, search results are updated
	g_signal_connect(search_entry, "changed", G_CALLBACK(populate_search_results), bot_grd);
	printf("send bot_grd @ %20p, &bot_grd @ %20p ...\n", bot_grd, &bot_grd);

	GtkWidget *site_head_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(site_head_label), "<b>Website</b>");
	gtk_grid_attach(GTK_GRID(bot_grd), site_head_label, 0, -1, 1, 1);
	// GtkWidget *uname_head_label = gtk_label_new(NULL);
	// gtk_label_set_markup(GTK_LABEL(uname_head_label), "<b>Username</b>");
	// gtk_grid_attach(GTK_GRID(bot_grd), uname_head_label, 1, -1, 1, 1);
	// for(int i = 0; i < num_of_items; ++i)
	// {
	// 	GtkWidget *l1 = gtk_label_new(items[i].ptrs[I_SITE]);
	// 	GtkWidget *l2 = gtk_label_new(items[i].ptrs[I_UNAME]);
	// 	gtk_grid_attach(GTK_GRID(bot_grd), l1, 0, i, 1, 1);
	// 	gtk_grid_attach(GTK_GRID(bot_grd), l2, 1, i, 1, 1);
	// }

	return see_box;
}

/*-----------------------------------------------------------------------------
Populate the 'Change Passphrase' page of the notebook.
-----------------------------------------------------------------------------*/
GtkWidget *create_widget_for_cpp(GtkWidget **window)
{
	GtkWidget *cpp_grd = gtk_grid_new();
	return cpp_grd;
}

/*-----------------------------------------------------------------------------
Write the data provided by the user to the password file. Encrypt the website,
username and password with the randomly generated key. Then encrypt the key
using the key encryption key. Store them and the initialisation vector also in
the file.
-----------------------------------------------------------------------------*/
void add_password(GtkWidget *widget, gpointer data)
{
	// get the window in which tooltips will be shown
	GtkWidget **window = data;

	// read provided information
	char const *site  = get_credentials_site();
	char const *uname = get_credentials_uname();
	char const *pw    = get_credentials_pw();
	char const *cp    = get_credentials_cp();

	// sanity
	if(!strcmp(site, ""))
	{
		gtk_widget_set_tooltip_text(*window, "Cannot add password. \'Website\' field is empty.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, *window);
		return;
	}
	if(!strcmp(uname, ""))
	{
		gtk_widget_set_tooltip_text(*window, "Cannot add password. \'Username\' field is empty.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, *window);
		return;
	}
	if(!strcmp(pw, ""))
	{
		gtk_widget_set_tooltip_text(*window, "Cannot add password. \'Password\' field is empty.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, *window);
		return;
	}
	if(!strcmp(cp, ""))
	{
		gtk_widget_set_tooltip_text(*window, "Cannot add password. \'Confirm Password\' field is empty.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, *window);
		return;
	}
	if(strcmp(pw, cp))
	{
		gtk_widget_set_tooltip_text(*window, "Cannot add password. Fields \'Password\' and \'Confirm Password\' do not match.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, *window);
		return;
	}

	// generate encryption key and initialisation vector for AES
	char unsigned *key = generate_random(ENCRYPT_KEY_LENGTH);
	char unsigned *iv  = generate_random(INIT_VECTOR_LENGTH);

	// encrypt the website, username and password using the AES key
	char unsigned *e_site, *e_uname, *e_pw;
	int e_sitelen  = encrypt((char unsigned *)site,  strlen(site),  key, iv, &e_site);
	int e_unamelen = encrypt((char unsigned *)uname, strlen(uname), key, iv, &e_uname);
	int e_pwlen    = encrypt((char unsigned *)pw,    strlen(pw),    key, iv, &e_pw);
	digest_to_hexdigest(&e_site,  e_sitelen);
	digest_to_hexdigest(&e_uname, e_unamelen);
	digest_to_hexdigest(&e_pw,    e_pwlen);

	// encrypt the AES key using key encryption key
	// no need to check return value of `encrypt' function
	// encrypted form of 32-byte key is 32 bytes long
	char unsigned *kek = get_credentials_kek();
	char unsigned *e_key;
	encrypt(key, ENCRYPT_KEY_LENGTH, kek, iv, &e_key);
	digest_to_hexdigest(&e_key, ENCRYPT_KEY_LENGTH);

	// initialisation vector is not encypted
	// because it is required for decryption
	digest_to_hexdigest(&iv, INIT_VECTOR_LENGTH);

	// write them all to the file
	FILE *pw_file = fopen(Slave, "ab");
	fprintf(pw_file, "%s\n", e_site);
	fprintf(pw_file, "%s\n", e_uname);
	fprintf(pw_file, "%s\n", e_pw);
	fprintf(pw_file, "%s\n", e_key);
	fprintf(pw_file, "%s\n", iv);
	fclose(pw_file);

	// trash all data
	del_credentials();
	memset(key,     0, ENCRYPT_KEY_LENGTH);
	memset(iv,      0, 2 * INIT_VECTOR_LENGTH);
	memset(e_site,  0, 2 * e_sitelen);
	memset(e_uname, 0, 2 * e_unamelen);
	memset(e_pw,    0, 2 * e_pwlen);
	memset(e_key,   0, 2 * ENCRYPT_KEY_LENGTH);

	// display success message
	gtk_widget_set_tooltip_text(*window, "Password added successfully.");
	g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, *window);
}

///////////////////////////////////////////////////////////////////////////////

void populate_search_results(GtkWidget *widget, gpointer data)
{
	GtkWidget *bot_grd = data;
	printf("read bot_grd @ %20p, *bot_grd @ %20p ...\n", bot_grd, bot_grd);
	GtkWidget *uname_head_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(uname_head_label), "<b>Username</b>");
	gtk_grid_attach(GTK_GRID(bot_grd), uname_head_label, 1, -1, 1, 1);
	// get the grid which has to be populated with search results
	// GtkWidget **bot_grd = data;
	// printf("%p, %p\n", bot_grd, *bot_grd);

	// // obtain the data to be written in this grid
	// int number_of_lines = 0;
	// char ***lines = retrieve_data_from_file(&number_of_lines);

	// // put this data in the grid
	// for(int i = 0; i < number_of_lines; ++i)
	// {
	// 	for(int j = 0; j < 5; ++j)
	// 	{
	// 		GtkWidget *l = gtk_label_new(lines[i][j]);
	// 		gtk_grid_attach(GTK_GRID(bot_grd), l, j, i, 1, 1);
	// 	}
	// }
}

///////////////////////////////////////////////////////////////////////////////

void quit_choice(void)
{
	del_credentials();
	gtk_main_quit();
	memset(credentials->kek, 0, ENCRYPT_KEY_LENGTH);
	free(credentials->kek);
	free(credentials);
	exit(0);
}

