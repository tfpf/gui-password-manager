/*-----------------------------------------------------------------------------
Display a window in which the user can choose what they want to do.
-----------------------------------------------------------------------------*/
void request_choice(void)
{
	// window
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 0);
	gtk_window_maximize(GTK_WINDOW(window));
	gtk_window_set_icon_from_file(GTK_WINDOW(window), icon_main, NULL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
	gtk_window_set_title(GTK_WINDOW(window), "Password Manager");

	// notebook page to add password
	GtkWidget *add_grd = create_widget_for_add(window);
	GtkWidget *add_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(add_lbl), "<span weight=\"normal\">Add New Password</span>");

	// notebook page to manage passwords
	GtkWidget *mng_box = create_widget_for_mng(window);
	GtkWidget *mng_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(mng_lbl), "<span weight=\"normal\">Manage Passwords</span>");

	// notebook page to change passphrase
	GtkWidget *cpp_grd = create_widget_for_cpp(window);
	GtkWidget *cpp_lbl = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(cpp_lbl), "<span weight=\"normal\">Change Passphrase</span>");

	// notebook
	GtkWidget *notebook = gtk_notebook_new();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), add_grd, add_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), mng_box, mng_lbl);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), cpp_grd, cpp_lbl);
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_LEFT);
	gtk_container_add(GTK_CONTAINER(window), notebook);
	g_signal_connect(GTK_NOTEBOOK(notebook), "switch-page", G_CALLBACK(clear_all_entries), window);

	gtk_widget_show_all(window);
	g_signal_connect(window, "destroy", G_CALLBACK(quit_choice), window);
	gtk_main();
}

/*-----------------------------------------------------------------------------
Populate the 'Add New Password' page of the notebook.
-----------------------------------------------------------------------------*/
GtkWidget *create_widget_for_add(GtkWidget *window)
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
	gtk_label_set_markup(GTK_LABEL(main_label), "<span weight=\"bold\" foreground=\"green\">Fill these fields to add a new password.</span>");
	gtk_grid_attach(GTK_GRID(add_grd), main_label, 0, 0, 3, 1);

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
	GtkWidget *show_button_pw = gtk_button_new();
	gtk_widget_set_can_focus(show_button_pw, FALSE);
	gtk_button_set_image(GTK_BUTTON(show_button_pw), gtk_image_new_from_file(icon_invis));
	gtk_widget_set_tooltip_text(show_button_pw, "Click to show or hide password.");
	g_signal_connect(show_button_pw, "clicked", G_CALLBACK(toggle_visibility), pw_entry);
	gtk_grid_attach(GTK_GRID(add_grd), show_button_pw, 2, 3, 1, 1);

	// confirm password
	GtkWidget *cp_label = gtk_label_new("Confirm Password");
	gtk_grid_attach(GTK_GRID(add_grd), cp_label, 0, 4, 1, 1);
	GtkWidget *cp_entry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(cp_entry), FALSE);
	gtk_grid_attach(GTK_GRID(add_grd), cp_entry, 1, 4, 1, 1);
	GtkWidget *show_button_cp = gtk_button_new();
	gtk_widget_set_can_focus(show_button_cp, FALSE);
	gtk_button_set_image(GTK_BUTTON(show_button_cp), gtk_image_new_from_file(icon_invis));
	gtk_widget_set_tooltip_text(show_button_cp, "Click to show or hide password.");
	g_signal_connect(show_button_cp, "clicked", G_CALLBACK(toggle_visibility), cp_entry);
	gtk_grid_attach(GTK_GRID(add_grd), show_button_cp, 2, 4, 1, 1);

	// prepare data to be sent to callback function
	GtkWidget **data = malloc(5 * sizeof *data);
	data[0] = window;
	data[1] = site_entry;
	data[2] = uname_entry;
	data[3] = pw_entry;
	data[4] = cp_entry;

	// button
	GtkWidget *add_btn = gtk_button_new_with_label("Add New Password");
	gtk_widget_set_can_focus(add_btn, FALSE);
	g_signal_connect(GTK_BUTTON(add_btn), "clicked", G_CALLBACK(add_password), data);
	gtk_grid_attach(GTK_GRID(add_grd), add_btn, 0, 5, 3, 1);

	return add_grd;
}

/*-----------------------------------------------------------------------------
Write the data provided by the user to the password file. Encrypt the website,
username and password with the randomly generated key. Then encrypt the key
using the key encryption key. Store them and the initialisation vector also in
the file.
-----------------------------------------------------------------------------*/
void add_password(GtkButton *button, gpointer data)
{
	GtkWidget **callback_data = data;
	GtkWidget *window = callback_data[0];
	char const *site  = gtk_entry_get_text(GTK_ENTRY(callback_data[1]));
	char const *uname = gtk_entry_get_text(GTK_ENTRY(callback_data[2]));
	char const *pw    = gtk_entry_get_text(GTK_ENTRY(callback_data[3]));
	char const *cp    = gtk_entry_get_text(GTK_ENTRY(callback_data[4]));

	// sanity
	if(!strcmp(site, ""))
	{
		gtk_widget_set_tooltip_text(window, "Cannot add password. \'Website\' field is empty.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, window);
		return;
	}
	if(!strcmp(uname, ""))
	{
		gtk_widget_set_tooltip_text(window, "Cannot add password. \'Username\' field is empty.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, window);
		return;
	}
	if(!strcmp(pw, ""))
	{
		gtk_widget_set_tooltip_text(window, "Cannot add password. \'Password\' field is empty.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, window);
		return;
	}
	if(!strcmp(cp, ""))
	{
		gtk_widget_set_tooltip_text(window, "Cannot add password. \'Confirm Password\' field is empty.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, window);
		return;
	}
	if(strcmp(pw, cp))
	{
		gtk_widget_set_tooltip_text(window, "Cannot add password. Fields \'Password\' and \'Confirm Password\' do not match.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, window);
		return;
	}

	// grid which will be put in the confirm dialogue box
	GtkWidget *confirm_grd = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(confirm_grd), 25);
	gtk_grid_set_column_spacing(GTK_GRID(confirm_grd), 15);
	gtk_grid_set_row_spacing(GTK_GRID(confirm_grd), 15);
	gtk_widget_set_halign(confirm_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(confirm_grd, TRUE);
	GtkWidget *confirm_img = gtk_image_new_from_file(icon_warn);
	gtk_grid_attach(GTK_GRID(confirm_grd), confirm_img, 0, 0, 1, 1);
	GtkWidget *confirm_lbl = gtk_label_new("Are you sure you want to add this item?");
	gtk_grid_attach(GTK_GRID(confirm_grd), confirm_lbl, 1, 0, 1, 1);

	// create the confirm dialogue box
	GtkWidget *confirm = gtk_dialog_new();
	gtk_window_set_resizable(GTK_WINDOW(confirm), FALSE);
	gtk_window_set_title(GTK_WINDOW(confirm), "Confirm Add");
	gtk_window_set_transient_for(GTK_WINDOW(confirm), GTK_WINDOW(window));

	// put the grid and some buttons in it
	GtkWidget *confirm_box = gtk_dialog_get_content_area(GTK_DIALOG(confirm));
	gtk_container_add(GTK_CONTAINER(confirm_box), confirm_grd);
	gtk_dialog_add_buttons(GTK_DIALOG(confirm), "Cancel", GTK_RESPONSE_REJECT, "Add", GTK_RESPONSE_ACCEPT, NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(confirm), GTK_RESPONSE_ACCEPT);
	gtk_widget_show_all(confirm);

	int response = gtk_dialog_run(GTK_DIALOG(confirm));
	gtk_widget_destroy(confirm);
	if(response != GTK_RESPONSE_ACCEPT)
	{
		return;
	}

	// obtain an AES key and initialisation vector
	char unsigned *key = generate_random(ENCRYPT_KEY_LENGTH);
	char unsigned *iv  = generate_random(INIT_VECTOR_LENGTH);

	// encrypt the website, username, password and key
	char unsigned *e_site, *e_uname, *e_pw, *e_key;
	int e_sitelen  = encrypt_AES((char unsigned *)site,  strlen(site),       key, iv, &e_site);
	int e_unamelen = encrypt_AES((char unsigned *)uname, strlen(uname),      key, iv, &e_uname);
	int e_pwlen    = encrypt_AES((char unsigned *)pw,    strlen(pw),         key, iv, &e_pw);
	int e_keylen   = encrypt_AES(key,                    ENCRYPT_KEY_LENGTH, kek, iv, &e_key);

	// obtain the hexdigest of all these
	char *e_site_hex  = digest_to_hexdigest(e_site,  e_sitelen);
	char *e_uname_hex = digest_to_hexdigest(e_uname, e_unamelen);
	char *e_pw_hex    = digest_to_hexdigest(e_pw,    e_pwlen);
	char *e_key_hex   = digest_to_hexdigest(e_key,   e_keylen);
	char *iv_hex      = digest_to_hexdigest(iv,      INIT_VECTOR_LENGTH);

	// write them all to the file
	FILE *pw_file = fopen(Slave, "a");
	fprintf(pw_file, "%s\n", e_site_hex);
	fprintf(pw_file, "%s\n", e_uname_hex);
	fprintf(pw_file, "%s\n", e_pw_hex);
	fprintf(pw_file, "%s\n", e_key_hex);
	fprintf(pw_file, "%s\n", iv_hex);
	fclose(pw_file);

	// allocate memory to store this additional password item
	// it will be appended to the array maintained in RAM
	items = realloc(items, (num_of_items + 1) * sizeof *items);
	items[num_of_items].lens[I_SITE]  = strlen(site);
	items[num_of_items].lens[I_UNAME] = strlen(uname);
	items[num_of_items].lens[I_PW]    = e_pwlen;
	items[num_of_items].lens[I_KEY]   = e_keylen;
	items[num_of_items].lens[I_IV]    = INIT_VECTOR_LENGTH;

	// for website and username, memory has to be allocated
	// because the contents of the entries are being copied
	// for the rest, direct assignment will work
	items[num_of_items].ptrs[I_SITE]  = malloc((items[num_of_items].lens[I_SITE]  + 1) * sizeof(char));
	items[num_of_items].ptrs[I_UNAME] = malloc((items[num_of_items].lens[I_UNAME] + 1) * sizeof(char));

	strcpy(items[num_of_items].ptrs[I_SITE], site);
	strcpy(items[num_of_items].ptrs[I_UNAME], uname);
	items[num_of_items].ptrs[I_PW]  = e_pw;
	items[num_of_items].ptrs[I_KEY] = e_key;
	items[num_of_items].ptrs[I_IV]  = iv;

	++num_of_items;

	__clear_all_entries(window, NULL);

	// clear RAM
	memset(key,         0, 1 * ENCRYPT_KEY_LENGTH);
	memset(e_site,      0, 1 * e_sitelen);
	memset(e_uname,     0, 1 * e_unamelen);
	memset(e_site_hex,  0, 2 * e_sitelen);
	memset(e_uname_hex, 0, 2 * e_unamelen);
	memset(e_pw_hex,    0, 2 * e_pwlen);
	memset(e_key_hex,   0, 2 * e_keylen);
	memset(iv_hex,      0, 2 * INIT_VECTOR_LENGTH);

	// deallocate
	free(key);
	free(e_site);
	free(e_uname);
	free(e_site_hex);
	free(e_uname_hex);
	free(e_pw_hex);
	free(e_key_hex);
	free(iv_hex);

	qsort(items, num_of_items, sizeof *items, comparator);
	gtk_widget_set_tooltip_text(window, "Password added successfully.");
	g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, window);
}

/*-----------------------------------------------------------------------------
Populate the 'Manage Passwords' page of the notebook.
-----------------------------------------------------------------------------*/
GtkWidget *create_widget_for_mng(GtkWidget *window)
{
	// grid which will be placed in a scrollable window
	GtkWidget *bot_grd = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(bot_grd), 25);
	gtk_grid_set_column_spacing(GTK_GRID(bot_grd), 15);
	gtk_grid_set_row_spacing(GTK_GRID(bot_grd), 15);
	gtk_widget_set_halign(bot_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(bot_grd, TRUE);

	// scrollable window to be placed at the bottom
	GtkWidget *bot_scw = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_set_border_width(GTK_CONTAINER(bot_scw), 0);
	gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(bot_scw), FALSE);
	gtk_scrolled_window_set_placement(GTK_SCROLLED_WINDOW(bot_scw), GTK_CORNER_TOP_LEFT);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(bot_scw), GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
	gtk_container_add(GTK_CONTAINER(bot_scw), bot_grd);

	// grid to be placed at the top
	GtkWidget *top_grd = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(top_grd), 50);
	gtk_grid_set_column_spacing(GTK_GRID(top_grd), 25);
	gtk_grid_set_row_spacing(GTK_GRID(top_grd), 25);
	gtk_widget_set_halign(top_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(top_grd, TRUE);

	// header
	GtkWidget *main_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(main_label), "<span weight=\"bold\" foreground=\"green\">Type into the search box to filter the list below.</span>");
	gtk_grid_attach(GTK_GRID(top_grd), main_label, 0, 0, 2, 1);

	// label
	GtkWidget *search_label = gtk_label_new("Search");
	gtk_grid_attach(GTK_GRID(top_grd), search_label, 0, 1, 1, 1);
	GtkWidget *search_entry = gtk_entry_new();
	g_signal_connect(search_entry, "changed", G_CALLBACK(populate_search_results), bot_grd);
	gtk_grid_attach(GTK_GRID(top_grd), search_entry, 1, 1, 1, 1);

	// put both `top_grd' and `bot_scw' into a box
	// this box will be returned so that it can be placed in a notebook
	GtkWidget *mng_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(GTK_BOX(mng_box), top_grd, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(mng_box), bot_scw, TRUE, TRUE, 0);

	return mng_box;
}

/*-----------------------------------------------------------------------------
Clear whatever was populated in the grid because of a previous call to the
function `populate_search_results'.
-----------------------------------------------------------------------------*/
void delete_previous_search_results(GtkWidget *bot_grd)
{
	GList *children = gtk_container_get_children(GTK_CONTAINER(bot_grd));
	for(GList *child = children; child != NULL; child = g_list_next(child))
	{
		gtk_widget_destroy(child->data);
	}
	g_list_free(children);
}

/*-----------------------------------------------------------------------------
Fill the GTK grid with GTK labels containing the website and username of those
items in the password file which match the search term. Display buttons
alongside the labels. The first button will show the password when clicked. The
second will bring up a window to change the password. The third will delete the
password. The passwords are already loaded in RAM. There is no need to read the
password file from the drive.
-----------------------------------------------------------------------------*/
void populate_search_results(GtkEntry *entry, gpointer data)
{
	// get the entry text and the grid whose children have to be modified
	gchar const *search_term = gtk_entry_get_text(GTK_ENTRY(entry));
	GtkWidget *bot_grd = data;

	delete_previous_search_results(bot_grd);

	// find out which items match the search term
	gboolean is_grid_populated = FALSE;
	for(int i = 0, j = 0; i < num_of_items; ++i)
	{
		if(!strcasestr(items[i].ptrs[I_SITE], search_term) && !strcasestr(items[i].ptrs[I_UNAME], search_term))
		{
			continue;
		}

		// local copy of `i'
		// this is required to pass the correct data on button click
		// cannot simply assign `i' to another `int' variable
		// because it will be destroyed when this scope is exited
		int *k = malloc(sizeof *k);
		*k = i;

		// website
		GtkWidget *site_label = gtk_label_new(items[i].ptrs[I_SITE]);
		gtk_grid_attach(GTK_GRID(bot_grd), site_label, 0, j, 1, 1);

		// username
		GtkWidget *uname_label = gtk_label_new(items[i].ptrs[I_UNAME]);
		gtk_grid_attach(GTK_GRID(bot_grd), uname_label, 1, j, 1, 1);

		// password button
		char pw_text[128];
		sprintf(pw_text, "Click to show password for %ld seconds.", PASSWORD_DISPLAY_TIMEOUT / G_TIME_SPAN_MILLISECOND);
		GtkWidget *pw_button = gtk_button_new_with_label(HIDDEN_PASSWORD_PLACEHOLDER);
		gtk_widget_set_can_focus(pw_button, FALSE);
		gtk_widget_set_tooltip_text(pw_button, pw_text);
		g_signal_connect(pw_button, "clicked", G_CALLBACK(show_password), k);
		gtk_grid_attach(GTK_GRID(bot_grd), pw_button, 2, j, 1, 1);

		// edit button
		GtkWidget *chg_button = gtk_button_new();
		gtk_widget_set_can_focus(chg_button, FALSE);
		gtk_button_set_image(GTK_BUTTON(chg_button), gtk_image_new_from_file(icon_edit));
		gtk_widget_set_tooltip_text(chg_button, "Click to edit this item.");
		g_signal_connect(chg_button, "clicked", G_CALLBACK(change_password), k);
		gtk_grid_attach(GTK_GRID(bot_grd), chg_button, 3, j, 1, 1);

		// delete button
		GtkWidget *del_button = gtk_button_new();
		gtk_widget_set_can_focus(del_button, FALSE);
		gtk_button_set_image(GTK_BUTTON(del_button), gtk_image_new_from_file(icon_del));
		gtk_widget_set_tooltip_text(del_button, "Click to delete this item.");
		g_signal_connect(del_button, "clicked", G_CALLBACK(delete_password), k);
		gtk_grid_attach(GTK_GRID(bot_grd), del_button, 4, j, 1, 1);

		++j;
		is_grid_populated = TRUE;
	}

	// if there were no matches for the search, do not draw the headers
	if(is_grid_populated == FALSE)
	{
		GtkWidget *error_label = gtk_label_new(NULL);
		gtk_label_set_markup(GTK_LABEL(error_label), "<span weight=\"bold\" foreground=\"gray\">No Matching Items</span>");
		gtk_grid_attach(GTK_GRID(bot_grd), error_label, 0, 0, 1, 1);
		gtk_widget_show_all(bot_grd);
		return;
	}

	// website header
	GtkWidget *site_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(site_label), "<span weight=\"bold\" foreground=\"green\">                      Website                      </span>");
	gtk_grid_attach(GTK_GRID(bot_grd), site_label, 0, -1, 1, 1);

	// username header
	GtkWidget *uname_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(uname_label), "<span weight=\"bold\" foreground=\"green\">                      Username                      </span>");
	gtk_grid_attach(GTK_GRID(bot_grd), uname_label, 1, -1, 1, 1);

	// password header
	GtkWidget *pw_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(pw_label), "<span weight=\"bold\" foreground=\"green\">                      Password                      </span>");
	gtk_grid_attach(GTK_GRID(bot_grd), pw_label, 2, -1, 1, 1);

	// action header
	GtkWidget *act_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(act_label), "<span weight=\"bold\" foreground=\"green\">Actions</span>");
	gtk_grid_attach(GTK_GRID(bot_grd), act_label, 3, -1, 2, 1);

	gtk_widget_show_all(bot_grd);
}

/*-----------------------------------------------------------------------------
Modify the label of a button. Find the password corresponding to the website
and username. Set it as the button label text.
-----------------------------------------------------------------------------*/
void show_password(GtkButton *button, gpointer data)
{
	// get the index of the item to show
	int *i = data;

	// decrypt the password
	char unsigned *iv = items[*i].ptrs[I_IV];
	char unsigned *key;
	char *pw;
	decrypt_AES(items[*i].ptrs[I_KEY], ENCRYPT_KEY_LENGTH, kek, iv, &key);
	decrypt_AES(items[*i].ptrs[I_PW], items[*i].lens[I_PW], key, iv, (char unsigned **)&pw);
	gtk_button_set_label(button, pw);

	// clear RAM
	memset(key, 0, ENCRYPT_KEY_LENGTH);
	memset(pw,  0, items[*i].lens[I_PW]);

	// deallocate
	free(key);
	free(pw);

	// schedule the password to be hidden after some time
	g_timeout_add(PASSWORD_DISPLAY_TIMEOUT, hide_password, button);
}

/*-----------------------------------------------------------------------------
Check if the input is a valid GTK button. If it is, modify its label: replace
the password text with placeholder text and trash the password in memory. If
the text is already the placeholder text, do nothing.
-----------------------------------------------------------------------------*/
gboolean hide_password(gpointer data)
{
	GtkButton *button = data;
	if(GTK_IS_BUTTON(button) == FALSE)
	{
		return FALSE;
	}

	// `widget' has been confirmed to be a valid GTK widget
	// still, it is possible that it is not
	// this could happen if the widget has already been destroyed
	// in which case, `pw' will be `NULL'
	// hence, check `pw' to prevent a segmentation fault
	char *pw = (char *)gtk_button_get_label(button);
	if(pw == NULL || !strcmp(pw, HIDDEN_PASSWORD_PLACEHOLDER))
	{
		return FALSE;
	}

	// clean up
	memset(pw, 0, strlen(pw));
	gtk_button_set_label(button, HIDDEN_PASSWORD_PLACEHOLDER);
	return FALSE;
}

/*-----------------------------------------------------------------------------
Delete the password associated with the specified item.
-----------------------------------------------------------------------------*/
void delete_password(GtkButton *button, gpointer data)
{
	int *i = data;

	// obtain the toplevel window
	// it will be used as the parent of the confirm dialogue box
	GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));

	// grid which will be put in the confirm dialogue box
	GtkWidget *confirm_grd = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(confirm_grd), 25);
	gtk_grid_set_column_spacing(GTK_GRID(confirm_grd), 15);
	gtk_grid_set_row_spacing(GTK_GRID(confirm_grd), 15);
	gtk_widget_set_halign(confirm_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(confirm_grd, TRUE);
	GtkWidget *confirm_img = gtk_image_new_from_file(icon_warn);
	gtk_grid_attach(GTK_GRID(confirm_grd), confirm_img, 0, 0, 1, 1);
	GtkWidget *confirm_lbl = gtk_label_new("Are you sure you want to delete this item?\nThis action is irreversible.");
	gtk_grid_attach(GTK_GRID(confirm_grd), confirm_lbl, 1, 0, 2, 1);
	GtkWidget *site_lbl = gtk_label_new(items[*i].ptrs[I_SITE]);
	gtk_grid_attach(GTK_GRID(confirm_grd), site_lbl, 1, 1, 1, 1);
	GtkWidget *uname_lbl = gtk_label_new(items[*i].ptrs[I_UNAME]);
	gtk_grid_attach(GTK_GRID(confirm_grd), uname_lbl, 2, 1, 1, 1);

	// create the confirm dialogue box
	GtkWidget *confirm = gtk_dialog_new();
	gtk_window_set_resizable(GTK_WINDOW(confirm), FALSE);
	gtk_window_set_title(GTK_WINDOW(confirm), "Confirm Delete");
	gtk_window_set_transient_for(GTK_WINDOW(confirm), GTK_WINDOW(window));

	// put the grid and some buttons in it
	GtkWidget *confirm_box = gtk_dialog_get_content_area(GTK_DIALOG(confirm));
	gtk_container_add(GTK_CONTAINER(confirm_box), confirm_grd);
	gtk_dialog_add_buttons(GTK_DIALOG(confirm), "Cancel", GTK_RESPONSE_REJECT, "Delete", GTK_RESPONSE_ACCEPT, NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(confirm), GTK_RESPONSE_REJECT);
	gtk_widget_show_all(confirm);

	int response = gtk_dialog_run(GTK_DIALOG(confirm));
	gtk_widget_destroy(confirm);
	if(response != GTK_RESPONSE_ACCEPT)
	{
		return;
	}

	// clear RAM and deallocate
	// then shift all elements to complete the delete operation
	for(int j = 0; j < PTRS_PER_ITEM; ++j)
	{
		memset(items[*i].ptrs[j], 0, items[*i].lens[j]);
		free(items[*i].ptrs[j]);
	}
	for(int j = *i; j < num_of_items - 1; ++j)
	{
		items[j] = items[j + 1];
	}
	--num_of_items;

	// write the modified list of items to the password file
	for(int j = 0; j < num_of_items; ++j)
	{
		char unsigned *e_pw = items[j].ptrs[I_PW];

		// obtain key
		char unsigned *e_key = items[j].ptrs[I_KEY];
		char unsigned *iv    = items[j].ptrs[I_IV];
		char unsigned *key;
		int keylen = decrypt_AES(e_key, items[j].lens[I_KEY], kek, iv, &key);

		// encrypt website and username
		char *site  = items[j].ptrs[I_SITE];
		char *uname = items[j].ptrs[I_UNAME];
		char unsigned *e_site, *e_uname;
		int e_sitelen  = encrypt_AES((char unsigned *)site,  items[j].lens[I_SITE],  key, iv, &e_site);
		int e_unamelen = encrypt_AES((char unsigned *)uname, items[j].lens[I_UNAME], key, iv, &e_uname);

		// obtain their hexdigests
		char *e_site_hex  = digest_to_hexdigest(e_site,  e_sitelen);
		char *e_uname_hex = digest_to_hexdigest(e_uname, e_unamelen);
		char *e_pw_hex    = digest_to_hexdigest(e_pw,    items[j].lens[I_PW]);
		char *e_key_hex   = digest_to_hexdigest(e_key,   items[j].lens[I_KEY]);
		char *iv_hex      = digest_to_hexdigest(iv,      INIT_VECTOR_LENGTH);

		// write everything to the file
		FILE *pw_file = fopen(__Slave, "a");
		fprintf(pw_file, "%s\n", e_site_hex);
		fprintf(pw_file, "%s\n", e_uname_hex);
		fprintf(pw_file, "%s\n", e_pw_hex);
		fprintf(pw_file, "%s\n", e_key_hex);
		fprintf(pw_file, "%s\n", iv_hex);
		fclose(pw_file);

		// clear RAM
		memset(key,         0, 1 * keylen);
		memset(e_site,      0, 1 * e_sitelen);
		memset(e_uname,     0, 1 * e_unamelen);
		memset(e_site_hex,  0, 2 * e_sitelen);
		memset(e_uname_hex, 0, 2 * e_unamelen);
		memset(e_pw_hex,    0, 2 * items[j].lens[I_PW]);
		memset(e_key_hex,   0, 2 * items[j].lens[I_KEY]);
		memset(iv_hex,      0, 2 * INIT_VECTOR_LENGTH);

		// deallocate
		free(key);
		free(e_site);
		free(e_uname);
		free(e_site_hex);
		free(e_uname_hex);
		free(e_pw_hex);
		free(e_key_hex);
		free(iv_hex);
	}

	// replace old file with new
	remove(Slave);
	rename(__Slave, Slave);

	__clear_all_entries(window, NULL);

	gtk_widget_set_tooltip_text(window, "Password deleted successfully.");
	g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, window);
}

/*-----------------------------------------------------------------------------
Change the password associated with the specified item.
-----------------------------------------------------------------------------*/
void change_password(GtkButton *button, gpointer data)
{
	int *i = data;
	printf("change %d\n", *i);
	// gtk_widget_hide(window);
}

/*-----------------------------------------------------------------------------
Populate the 'Change Passphrase' page of the notebook.
-----------------------------------------------------------------------------*/
GtkWidget *create_widget_for_cpp(GtkWidget *window)
{
	// the grid to return
	GtkWidget *cpp_grd = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(cpp_grd), 50);
	gtk_grid_set_column_spacing(GTK_GRID(cpp_grd), 25);
	gtk_grid_set_row_spacing(GTK_GRID(cpp_grd), 25);
	gtk_widget_set_halign(cpp_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(cpp_grd, TRUE);

	// header
	GtkWidget *main_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(main_label), "<span weight=\"bold\" foreground=\"green\">Fill these fields to change the passphrase.</span>");
	gtk_grid_attach(GTK_GRID(cpp_grd), main_label, 0, 0, 3, 1);

	// passphrase
	GtkWidget *pp_label = gtk_label_new("New Passphrase");
	gtk_grid_attach(GTK_GRID(cpp_grd), pp_label, 0, 1, 1, 1);
	GtkWidget *pp_entry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(pp_entry), FALSE);
	gtk_grid_attach(GTK_GRID(cpp_grd), pp_entry, 1, 1, 1, 1);
	GtkWidget *show_button_pp = gtk_button_new();
	gtk_widget_set_can_focus(show_button_pp, FALSE);
	gtk_button_set_image(GTK_BUTTON(show_button_pp), gtk_image_new_from_file(icon_invis));
	gtk_widget_set_tooltip_text(show_button_pp, "Click to show or hide passphrase.");
	g_signal_connect(show_button_pp, "clicked", G_CALLBACK(toggle_visibility), pp_entry);
	gtk_grid_attach(GTK_GRID(cpp_grd), show_button_pp, 2, 1, 1, 1);

	// confirm passphrase
	GtkWidget *cp_label = gtk_label_new("Confirm New Passphrase");
	gtk_grid_attach(GTK_GRID(cpp_grd), cp_label, 0, 2, 1, 1);
	GtkWidget *cp_entry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(cp_entry), FALSE);
	gtk_grid_attach(GTK_GRID(cpp_grd), cp_entry, 1, 2, 1, 1);
	GtkWidget *show_button_cp = gtk_button_new();
	gtk_widget_set_can_focus(show_button_cp, FALSE);
	gtk_button_set_image(GTK_BUTTON(show_button_cp), gtk_image_new_from_file(icon_invis));
	gtk_widget_set_tooltip_text(show_button_cp, "Click to show or hide passphrase.");
	g_signal_connect(show_button_cp, "clicked", G_CALLBACK(toggle_visibility), cp_entry);
	gtk_grid_attach(GTK_GRID(cpp_grd), show_button_cp, 2, 2, 1, 1);

	// prepare data to be sent to callback function
	GtkWidget **data = malloc(3 * sizeof *data);
	data[0] = window;
	data[1] = pp_entry;
	data[2] = cp_entry;

	// button
	GtkWidget *cpp_btn = gtk_button_new_with_label("Change Passphrase");
	gtk_widget_set_can_focus(cpp_btn, FALSE);
	g_signal_connect(GTK_BUTTON(cpp_btn), "clicked", G_CALLBACK(change_passphrase), data);
	gtk_grid_attach(GTK_GRID(cpp_grd), cpp_btn, 0, 3, 3, 1);

	return cpp_grd;
}

/*-----------------------------------------------------------------------------
Change the passphrase used to encrypt the data. Decrypt the data using the old
passphrase, then re-ecnrypt it using the new passphrase. Rewrite the files that
store the passphrase hash and the encrypted passwords.
-----------------------------------------------------------------------------*/
void change_passphrase(GtkButton *button, gpointer data)
{
	GtkWidget **callback_data = data;
	GtkWidget *window = callback_data[0];
	char const *pp = gtk_entry_get_text(GTK_ENTRY(callback_data[1]));
	char const *cp = gtk_entry_get_text(GTK_ENTRY(callback_data[2]));

	// sanity
	if(!strcmp(pp, ""))
	{
		gtk_widget_set_tooltip_text(window, "Cannot change passphrase. \'New passphrase\' field is empty.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, window);
		return;
	}
	if(!strcmp(cp, ""))
	{
		gtk_widget_set_tooltip_text(window, "Cannot change passphrase. \'Confirm New passphrase\' field is empty.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, window);
		return;
	}
	if(strcmp(pp, cp))
	{
		gtk_widget_set_tooltip_text(window, "Cannot change passphrase. Fields \'New Passphrase\' and \'Confirm New Passphrase\' do not match.");
		g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, window);
		return;
	}

	// grid which will be put in the confirm dialogue box
	GtkWidget *confirm_grd = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(confirm_grd), 25);
	gtk_grid_set_column_spacing(GTK_GRID(confirm_grd), 15);
	gtk_grid_set_row_spacing(GTK_GRID(confirm_grd), 15);
	gtk_widget_set_halign(confirm_grd, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(confirm_grd, TRUE);
	GtkWidget *confirm_img = gtk_image_new_from_file(icon_warn);
	gtk_grid_attach(GTK_GRID(confirm_grd), confirm_img, 0, 0, 1, 1);
	GtkWidget *confirm_lbl = gtk_label_new("Are you sure you want to change the passphrase?\nIf you forget the new passphrase, you won\'t be able to recover your passwords.");
	gtk_grid_attach(GTK_GRID(confirm_grd), confirm_lbl, 1, 0, 1, 1);

	// create the confirm dialogue box
	GtkWidget *confirm = gtk_dialog_new();
	gtk_window_set_resizable(GTK_WINDOW(confirm), FALSE);
	gtk_window_set_title(GTK_WINDOW(confirm), "Confirm Change");
	gtk_window_set_transient_for(GTK_WINDOW(confirm), GTK_WINDOW(window));

	// put the grid and some buttons in it
	GtkWidget *confirm_box = gtk_dialog_get_content_area(GTK_DIALOG(confirm));
	gtk_container_add(GTK_CONTAINER(confirm_box), confirm_grd);
	gtk_dialog_add_buttons(GTK_DIALOG(confirm), "Cancel", GTK_RESPONSE_REJECT, "Change", GTK_RESPONSE_ACCEPT, NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(confirm), GTK_RESPONSE_REJECT);
	gtk_widget_show_all(confirm);

	int response = gtk_dialog_run(GTK_DIALOG(confirm));
	gtk_widget_destroy(confirm);
	if(response != GTK_RESPONSE_ACCEPT)
	{
		return;
	}

	// write new hash to file and obtain new key encryption key
	char unsigned *pph = malloc(SHA512_DIGEST_LENGTH * sizeof *pph);
	SHA512((char unsigned *)pp, strlen(pp), pph);
	for(int i = 0; i < HASH_COUNT; ++i)
	{
		SHA512(pph, SHA512_DIGEST_LENGTH, pph);
	}
	char *pph_hex = digest_to_hexdigest(pph, SHA512_DIGEST_LENGTH);
	FILE *pp_file = fopen(__Master, "w");
	fprintf(pp_file, "%s\n", pph_hex);
	fclose(pp_file);
	char unsigned *__kek = malloc(SHA256_DIGEST_LENGTH * sizeof *__kek);
	SHA256((char unsigned *)pp, strlen(pp), __kek);

	// clear RAM
	memset(pph,     0, 1 * SHA512_DIGEST_LENGTH);
	memset(pph_hex, 0, 2 * SHA512_DIGEST_LENGTH);

	// deallocate
	free(pph);
	free(pph_hex);

	// decrypt all the data, then re-encrypt it using new stuff
	for(int i = 0; i < num_of_items; ++i)
	{
		// decrypt the key
		char unsigned *key;
		char unsigned *e_key = items[i].ptrs[I_KEY];
		char unsigned *iv    = items[i].ptrs[I_IV];
		decrypt_AES(e_key, items[i].lens[I_KEY], kek, iv, &key);

		// decrypt the password
		char *pw;
		char unsigned *e_pw = items[i].ptrs[I_PW];
		int pwlen = decrypt_AES(e_pw, items[i].lens[I_PW], key, iv, (char unsigned **)&pw);

		// clear RAM
		memset(key,   0, ENCRYPT_KEY_LENGTH);
		memset(iv,    0, INIT_VECTOR_LENGTH);
		memset(e_pw,  0, items[i].lens[I_PW]);
		memset(e_key, 0, items[i].lens[I_KEY]);

		// deallocate
		free(key);
		free(iv);
		free(e_pw);
		free(e_key);

		// obtain website and username, which were not encrypted
		char *site  = items[i].ptrs[I_SITE];
		char *uname = items[i].ptrs[I_UNAME];

		// obtain new key and initialisation vector
		key = generate_random(ENCRYPT_KEY_LENGTH);
		iv  = generate_random(INIT_VECTOR_LENGTH);

		// encrypt
		char unsigned *e_site, *e_uname;
		int e_sitelen  = encrypt_AES((char unsigned *)site,  items[i].lens[I_SITE],  key,   iv, &e_site);
		int e_unamelen = encrypt_AES((char unsigned *)uname, items[i].lens[I_UNAME], key,   iv, &e_uname);
		int e_pwlen    = encrypt_AES((char unsigned *)pw,    pwlen,                  key,   iv, &e_pw);
		int e_keylen   = encrypt_AES(key,                    ENCRYPT_KEY_LENGTH,     __kek, iv, &e_key);

		// obtain the hexdigest of all these
		char *e_site_hex  = digest_to_hexdigest(e_site,  e_sitelen);
		char *e_uname_hex = digest_to_hexdigest(e_uname, e_unamelen);
		char *e_pw_hex    = digest_to_hexdigest(e_pw,    e_pwlen);
		char *e_key_hex   = digest_to_hexdigest(e_key,   e_keylen);
		char *iv_hex      = digest_to_hexdigest(iv,      INIT_VECTOR_LENGTH);

		// write them all to the file
		FILE *pw_file = fopen(__Slave, "a");
		fprintf(pw_file, "%s\n", e_site_hex);
		fprintf(pw_file, "%s\n", e_uname_hex);
		fprintf(pw_file, "%s\n", e_pw_hex);
		fprintf(pw_file, "%s\n", e_key_hex);
		fprintf(pw_file, "%s\n", iv_hex);
		fclose(pw_file);

		// store the required data in RAM
		items[i].ptrs[I_PW]  = e_pw;
		items[i].lens[I_PW]  = e_pwlen;
		items[i].ptrs[I_KEY] = e_key;
		items[i].lens[I_KEY] = e_keylen;
		items[i].ptrs[I_IV]  = iv;

		// clear RAM
		memset(pw,          0, 1 * pwlen);
		memset(key,         0, 1 * ENCRYPT_KEY_LENGTH);
		memset(e_site,      0, 1 * e_sitelen);
		memset(e_uname,     0, 1 * e_unamelen);
		memset(e_site_hex,  0, 2 * e_sitelen);
		memset(e_uname_hex, 0, 2 * e_unamelen);
		memset(e_pw_hex,    0, 2 * e_pwlen);
		memset(e_key_hex,   0, 2 * e_keylen);
		memset(iv_hex,      0, 2 * INIT_VECTOR_LENGTH);

		// deallocate
		free(pw);
		free(key);
		free(e_site);
		free(e_uname);
		free(e_site_hex);
		free(e_uname_hex);
		free(e_pw_hex);
		free(e_key_hex);
		free(iv_hex);
	}

	// delete the old files, and replace them with the new files
	remove(Slave);
	rename(__Slave, Slave);
	remove(Master);
	rename(__Master, Master);

	__clear_all_entries(window, NULL);

	// clear RAM
	memset(kek, 0, ENCRYPT_KEY_LENGTH);

	// deallocate
	free(kek);

	kek = __kek;

	gtk_widget_set_tooltip_text(window, "Passphrase changed successfully.");
	g_timeout_add(TOOLTIP_MESSAGE_TIMEOUT, hide_tooltip, window);
}

/*-----------------------------------------------------------------------------
Clear the key encryption key. Clear the list of passwords in RAM. Deallocate
all memory and quit.
-----------------------------------------------------------------------------*/
void quit_choice(GtkWindow *window, gpointer data)
{
	__clear_all_entries(GTK_WIDGET(window), NULL);
	gtk_main_quit();
	memset(kek, 0, ENCRYPT_KEY_LENGTH);
	free(kek);
	for(int i = 0; i < num_of_items; ++i)
	{
		for(int j = 0; j < PTRS_PER_ITEM; ++j)
		{
			memset(items[i].ptrs[j], 0, items[i].lens[j]);
			free(items[i].ptrs[j]);
		}
	}
	free(items);
	exit(EXIT_SUCCESS);
}

