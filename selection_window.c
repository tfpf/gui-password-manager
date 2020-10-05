/*-----------------------------------------------------------------------------
Wrapper around the GTK window struct. Allows easy access to all sensitive
data and the key encryption key.
-----------------------------------------------------------------------------*/
typedef struct
{
    GtkWidget *window;                 // main window
    gboolean construction_in_progress; // whether all widgets have been created
    password_item_t **items;           // array containing data read from file
    int num_of_items;
    GtkWidget *search_ent;
    GtkWidget *bottom_grid;            // grid containing search results
    GtkWidget *website_ent;
    GtkWidget *username_ent;
    GtkWidget *password1_ent;
    GtkWidget *password2_ent;
    GtkWidget *passphrase1_ent;
    GtkWidget *passphrase2_ent;
    char unsigned *kek;                // key encryption key
}
selection_window_t;

/*-----------------------------------------------------------------------------
Function prototypes.
-----------------------------------------------------------------------------*/
selection_window_t *selection_window_new(char unsigned *kek);
void selection_window_main(selection_window_t *self);
void selection_window_sort_items(selection_window_t *self);
int selection_window_ask_for_confirmation(selection_window_t *self, char *question);
void selection_window_append(selection_window_t *self, password_item_t *item);
void selection_window_clear_entries(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer data);
void selection_window_clear_entry(GtkWidget *widget);
void selection_window_quit(GtkWidget *window, gpointer data);
GtkWidget *manage_box_new(selection_window_t *self);
void manage_box_update(GtkEntry *search_ent, gpointer data);
void manage_box_show_password(GtkButton *btn, gpointer data);
GtkWidget *add_grid_new(selection_window_t *self);
void add_grid_check(GtkButton *btn, gpointer data);
GtkWidget *change_grid_new(selection_window_t *self);
void change_grid_check(GtkButton *btn, gpointer data);

/*-----------------------------------------------------------------------------
Initialiser for the struct defined above. Create the GTK window. Then save it
and its passphrase entry as the struct members.
-----------------------------------------------------------------------------*/
selection_window_t *selection_window_new(char unsigned *kek)
{
    selection_window_t *self = malloc(sizeof *self);
    self->construction_in_progress = TRUE;
    self->kek = kek;

    // window
    self->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(self->window), 0);
    // gtk_window_maximize(GTK_WINDOW(self->window));
    gtk_window_set_icon_from_file(GTK_WINDOW(self->window), icon_main, NULL);
    gtk_window_set_position(GTK_WINDOW(self->window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(self->window), TRUE);
    gtk_window_set_title(GTK_WINDOW(self->window), "Password Manager");

    // notebook
    GtkWidget *notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_LEFT);
    gtk_container_add(GTK_CONTAINER(self->window), notebook);
    g_signal_connect(GTK_NOTEBOOK(notebook), "switch-page", G_CALLBACK(selection_window_clear_entries), self);

    // notebook page to manage passwords
    GtkWidget *manage_box = manage_box_new(self);
    GtkWidget *manage_lbl = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(manage_lbl), msg_manage);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), manage_box, manage_lbl);

    // notebook page to add password
    GtkWidget *add_grid = add_grid_new(self);
    GtkWidget *add_lbl = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(add_lbl), msg_add);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), add_grid, add_lbl);

    // notebook page to change passphrase
    GtkWidget *change_grid = change_grid_new(self);
    GtkWidget *change_lbl = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(change_lbl), msg_change);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), change_grid, change_lbl);

    self->items = password_items_new_from_file(&(self->num_of_items), self->kek);
    selection_window_sort_items(self);

    return self;
}

/*-----------------------------------------------------------------------------
Call the main GTK loop, which will open the window.
-----------------------------------------------------------------------------*/
void selection_window_main(selection_window_t *self)
{
    gtk_widget_show_all(self->window);
    self->construction_in_progress = FALSE;
    g_signal_connect(self->window, "destroy", G_CALLBACK(selection_window_quit), NULL);
    gtk_main();
}

/*-----------------------------------------------------------------------------
Sort the array of password items.

Selection sort has been implemented here. The library function `qsort' was not
used because the standard does not say anything on whether copies of the data
are created. In this case, copies must strictly not be made, because the data
contains passwords.
-----------------------------------------------------------------------------*/
void selection_window_sort_items(selection_window_t *self)
{
    for(int i = 0; i < self->num_of_items - 1; ++i)
    {
        int i_min = i;
        for(int j = i + 1; j < self->num_of_items; ++j)
        {
            int website_compare = strcasecmp(self->items[i_min]->website, self->items[j]->website);
            int username_compare = strcasecmp(self->items[i_min]->username, self->items[j]->username);
            if(website_compare > 0 || (website_compare == 0 && username_compare > 0))
            {
                i_min = j;
            }
        }
        password_item_t *temp = self->items[i_min];
        self->items[i_min] = self->items[i];
        self->items[i] = temp;
    }
}

/*-----------------------------------------------------------------------------
Display a dialog box asking for confirmation on whether the user wants to
proceed with some action.
-----------------------------------------------------------------------------*/
int selection_window_ask_for_confirmation(selection_window_t *self, char *question)
{
    // window
    GtkWidget *window = gtk_dialog_new();
    gtk_container_set_border_width(GTK_CONTAINER(window), 0);
    gtk_window_set_icon_from_file(GTK_WINDOW(window), icon_main, NULL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_title(GTK_WINDOW(window), "Confirmation");
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(self->window));

    // grid
    GtkWidget *grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(grid), 25);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 15);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 15);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(window))), grid);

    // warning icon
    GtkWidget *image = gtk_image_new_from_file(icon_warn);
    gtk_grid_attach(GTK_GRID(grid), image, 0, 0, 1, 1);

    // header label
    GtkWidget *header = gtk_label_new(question);
    gtk_grid_attach(GTK_GRID(grid), header, 1, 0, 1, 1);

    // buttons
    gtk_dialog_add_button(GTK_DIALOG(window), "No", GTK_RESPONSE_REJECT);
    gtk_dialog_add_button(GTK_DIALOG(window), "Yes", GTK_RESPONSE_ACCEPT);

    gtk_widget_show_all(window);
    int response = gtk_dialog_run(GTK_DIALOG(window));
    gtk_widget_destroy(window);

    return response;
}

/*-----------------------------------------------------------------------------
Write the new password item to the password file. Additionally, append it to
the array of password items in memory.
-----------------------------------------------------------------------------*/
void selection_window_append(selection_window_t *self, password_item_t *item)
{
    // write to file
    FILE *Slave_file = fopen(Slave, "ab");
    fwrite(&(item->e_website_length), sizeof(int), 1, Slave_file);
    fwrite(&(item->e_username_length), sizeof(int), 1, Slave_file);
    fwrite(&(item->e_password_length), sizeof(int), 1, Slave_file);
    fwrite(item->e_website, 1, item->e_website_length, Slave_file);
    fwrite(item->e_username, 1, item->e_username_length, Slave_file);
    fwrite(item->e_password, 1, item->e_password_length, Slave_file);
    fwrite(item->e_key, 1, AES_KEY_LENGTH, Slave_file);
    fwrite(item->iv, 1, INIT_VEC_LENGTH, Slave_file);
    fclose(Slave_file);

    // append to array
    password_item_t **temp = realloc(self->items, (self->num_of_items + 1) * sizeof *temp);
    if(temp == NULL)
    {
        fprintf(stderr, "Could not allocate memory to add password.\n");
        exit(EXIT_FAILURE);
    }
    self->items = temp;
    self->items[self->num_of_items] = item;
    ++self->num_of_items;
    selection_window_sort_items(self);
}

/*-----------------------------------------------------------------------------
Clear all entries on all pages of the GTK notebook in the window. This function
is called whenever the GTK notebook page is changed, which also happens during
page creation. In which case, the GTK window has not been drawn completely, so
nothing needs to be done.
-----------------------------------------------------------------------------*/
void selection_window_clear_entries(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer data)
{
    selection_window_t *self = data;
    if(self->construction_in_progress)
    {
        return;
    }

    selection_window_clear_entry(self->search_ent);
    selection_window_clear_entry(self->website_ent);
    selection_window_clear_entry(self->username_ent);
    selection_window_clear_entry(self->password1_ent);
    selection_window_clear_entry(self->password2_ent);
    selection_window_clear_entry(self->passphrase1_ent);
    selection_window_clear_entry(self->passphrase2_ent);
}

/*-----------------------------------------------------------------------------
Clear the GTK entry given if it is possible to do so. Change the contents of
the entry to a non-empty string, and clear it again.

The latter action is not strictly necessary. But it causes the affected GTK
entry to emit a signal indicating that its contents changed (this does not
happen on simply clearing the contents). This signal is used to populate the
password search results. That is why the additional action is also performed.
-----------------------------------------------------------------------------*/
void selection_window_clear_entry(GtkWidget *widget)
{
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(widget));
    gtk_entry_buffer_delete_text(buffer, 0, -1);
    gtk_entry_buffer_set_text(buffer, " ", -1);
    gtk_entry_buffer_delete_text(buffer, 0, -1);
}

/*-----------------------------------------------------------------------------
Automatically called when the window is closed. Quit the GTK main loop.
-----------------------------------------------------------------------------*/
void selection_window_quit(GtkWidget *window, gpointer data)
{
    gtk_widget_destroy(window);
    gtk_main_quit();
}

/*-----------------------------------------------------------------------------
Create a GTK box. Populate it with all the password items present in the
password file.
-----------------------------------------------------------------------------*/
GtkWidget *manage_box_new(selection_window_t *self)
{
    // box
    GtkWidget *manage_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // grid
    GtkWidget *top_grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(top_grid), 50);
    gtk_grid_set_column_spacing(GTK_GRID(top_grid), 25);
    gtk_grid_set_row_spacing(GTK_GRID(top_grid), 25);
    gtk_widget_set_halign(top_grid, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(top_grid, TRUE);
    gtk_box_pack_start(GTK_BOX(manage_box), top_grid, FALSE, FALSE, 0);

    // scrollable window
    GtkWidget *scrollable = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(scrollable), 0);
    gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(scrollable), FALSE);
    gtk_scrolled_window_set_placement(GTK_SCROLLED_WINDOW(scrollable), GTK_CORNER_TOP_LEFT);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollable), GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
    gtk_box_pack_start(GTK_BOX(manage_box), scrollable, TRUE, TRUE, 0);

    // grid to be put in the scrollable window
    self->bottom_grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(self->bottom_grid), 25);
    gtk_grid_set_column_spacing(GTK_GRID(self->bottom_grid), 15);
    gtk_grid_set_row_spacing(GTK_GRID(self->bottom_grid), 15);
    gtk_widget_set_halign(self->bottom_grid, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(self->bottom_grid, TRUE);
    gtk_container_add(GTK_CONTAINER(scrollable), self->bottom_grid);

    // header label
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), msg_manage_header);
    gtk_grid_attach(GTK_GRID(top_grid), header, 0, 0, 2, 1);

    // search prompt label
    GtkWidget *search_lbl = gtk_label_new("Search");
    gtk_grid_attach(GTK_GRID(top_grid), search_lbl, 0, 1, 1, 1);

    // search response entry
    self->search_ent = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(top_grid), self->search_ent, 1, 1, 1, 1);
    g_signal_connect(self->search_ent, "changed", G_CALLBACK(manage_box_update), self);

    return manage_box;
}

/*-----------------------------------------------------------------------------
Create widgets to display those items for which the search term is a
case-insensitive substring of either the website or the username. If such
matches are found, they are added to the GTK grid, and the headers for the grid
are created. If no matches are found, the headers are not created, and an error
message is displayed instead.

Note that the search entry is received as the first argument of the function.
Wherefore, there is no need to refer to the entry as `self->search_ent'.
-----------------------------------------------------------------------------*/
void manage_box_update(GtkEntry *search_ent, gpointer data)
{
    selection_window_t *self = data;
    char const *search = gtk_entry_get_text(search_ent);
    gboolean found = FALSE;

    // clear all widgets in `bottom_grid' (if any)
    GList *children = gtk_container_get_children(GTK_CONTAINER(self->bottom_grid));
    for(GList *child = children; child != NULL; child = g_list_next(child))
    {
        gtk_widget_destroy(child->data);
    }
    g_list_free(children);

    // show matching items
    // if the array element at position `i' contains `search', it will be added
    // the rows of the GTK grid do not have to be consecutive integers
    // so, the item can simply be attached to row `i' of the grid
    for(int i = 0; i < self->num_of_items; ++i)
    {
        if(!strcasestr(self->items[i]->website, search) && !strcasestr(self->items[i]->username, search))
        {
            continue;
        }
        found = TRUE;

        // website label
        GtkWidget *website_lbl = gtk_label_new(self->items[i]->website);
        gtk_grid_attach(GTK_GRID(self->bottom_grid), website_lbl, 0, i, 1, 1);

        // username label
        GtkWidget *username_lbl = gtk_label_new(self->items[i]->username);
        gtk_grid_attach(GTK_GRID(self->bottom_grid), username_lbl, 1, i, 1, 1);

        // password button
        GtkWidget *password_btn = gtk_button_new_with_label("...");
        gtk_widget_set_can_focus(password_btn, FALSE);
        gtk_widget_set_tooltip_text(password_btn, "Click to show passsword.");
        g_signal_connect(password_btn, "clicked", G_CALLBACK(manage_box_show_password), self->items[i]);
        gtk_grid_attach(GTK_GRID(self->bottom_grid), password_btn, 2, i, 1, 1);

        // edit button
        GtkWidget *edit_btn = gtk_button_new();
        gtk_widget_set_can_focus(edit_btn, FALSE);
        gtk_button_set_image(GTK_BUTTON(edit_btn), gtk_image_new_from_file(icon_edit));
        gtk_widget_set_tooltip_text(edit_btn, "Click to edit this item.");
        // g_signal_connect(edit_btn, "clicked", G_CALLBACK(manage_box_change_password), self->items[i]);
        gtk_grid_attach(GTK_GRID(self->bottom_grid), edit_btn, 3, i, 1, 1);

        // delete button
        GtkWidget *delete_btn = gtk_button_new();
        gtk_widget_set_can_focus(delete_btn, FALSE);
        gtk_button_set_image(GTK_BUTTON(delete_btn), gtk_image_new_from_file(icon_del));
        gtk_widget_set_tooltip_text(delete_btn, "Click to delete this item.");
        // g_signal_connect(delete_btn, "clicked", G_CALLBACK(manage_box_delete_password), self->items[i]);
        gtk_grid_attach(GTK_GRID(self->bottom_grid), delete_btn, 4, i, 1, 1);
    }

    // if nothing was found, do not display the headers
    if(!found)
    {
        GtkWidget *error_lbl = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(error_lbl), msg_manage_error);
        gtk_grid_attach(GTK_GRID(self->bottom_grid), error_lbl, 0, 0, 1, 1);
        gtk_widget_show_all(self->bottom_grid);
        return;
    }

    // website header label
    GtkWidget *header_website = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header_website), msg_manage_website);
    gtk_grid_attach(GTK_GRID(self->bottom_grid), header_website, 0, -1, 1, 1);

    // username header label
    GtkWidget *header_username = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header_username), msg_manage_username);
    gtk_grid_attach(GTK_GRID(self->bottom_grid), header_username, 1, -1, 1, 1);

    // password header label
    GtkWidget *header_password = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header_password), msg_manage_password);
    gtk_grid_attach(GTK_GRID(self->bottom_grid), header_password, 2, -1, 1, 1);

    gtk_widget_show_all(self->bottom_grid);
}

/*-----------------------------------------------------------------------------
Display the password of the particular item whose button was clicked.
-----------------------------------------------------------------------------*/
void manage_box_show_password(GtkButton *btn, gpointer data)
{
    password_item_t *item = data;
    gtk_button_set_label(btn, item->password);
    gtk_widget_set_has_tooltip(GTK_WIDGET(btn), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(btn), FALSE);
}

/*-----------------------------------------------------------------------------
Create a GTK grid. Create labels and entries to add a new password.
-----------------------------------------------------------------------------*/
GtkWidget *add_grid_new(selection_window_t *self)
{
    // grid
    GtkWidget *add_grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(add_grid), 50);
    gtk_grid_set_column_spacing(GTK_GRID(add_grid), 25);
    gtk_grid_set_row_spacing(GTK_GRID(add_grid), 25);
    gtk_widget_set_halign(add_grid, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(add_grid, TRUE);

    // header label
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), msg_add_header);
    gtk_grid_attach(GTK_GRID(add_grid), header, 0, 0, 3, 1);

    // website prompt label
    GtkWidget *website_lbl = gtk_label_new("Website");
    gtk_grid_attach(GTK_GRID(add_grid), website_lbl, 0, 1, 1, 1);

    // website response entry
    self->website_ent = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(self->website_ent), "e.g. FB, Twitter");
    gtk_grid_attach(GTK_GRID(add_grid), self->website_ent, 1, 1, 1, 1);

    // username prompt label
    GtkWidget *username_lbl = gtk_label_new("Username");
    gtk_grid_attach(GTK_GRID(add_grid), username_lbl, 0, 2, 1, 1);

    // username response label
    self->username_ent = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(add_grid), self->username_ent, 1, 2, 1, 1);

    // password prompt label
    GtkWidget *password1_lbl = gtk_label_new("Password");
    gtk_grid_attach(GTK_GRID(add_grid), password1_lbl, 0, 3, 1, 1);

    // password response entry
    self->password1_ent = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(self->password1_ent), FALSE);
    gtk_grid_attach(GTK_GRID(add_grid), self->password1_ent, 1, 3, 1, 1);

    // password visibility toggling button
    GtkWidget *password1_btn = gtk_button_new();
    gtk_widget_set_can_focus(password1_btn, FALSE);
    gtk_button_set_image(GTK_BUTTON(password1_btn), gtk_image_new_from_file(icon_vis));
    gtk_widget_set_tooltip_text(password1_btn, "Click to show or hide password.");
    g_signal_connect(password1_btn, "clicked", G_CALLBACK(toggle_visibility), self->password1_ent);
    gtk_grid_attach(GTK_GRID(add_grid), password1_btn, 2, 3, 1, 1);

    // confirm password prompt label
    GtkWidget *password2_lbl = gtk_label_new("Confirm Password");
    gtk_grid_attach(GTK_GRID(add_grid), password2_lbl, 0, 4, 1, 1);

    // confirm password response entry
    self->password2_ent = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(self->password2_ent), FALSE);
    gtk_grid_attach(GTK_GRID(add_grid), self->password2_ent, 1, 4, 1, 1);

    // confirm password visibility toggling button
    GtkWidget *password2_btn = gtk_button_new();
    gtk_widget_set_can_focus(password2_btn, FALSE);
    gtk_button_set_image(GTK_BUTTON(password2_btn), gtk_image_new_from_file(icon_vis));
    gtk_widget_set_tooltip_text(password2_btn, "Click to show or hide password.");
    g_signal_connect(password2_btn, "clicked", G_CALLBACK(toggle_visibility), self->password2_ent);
    gtk_grid_attach(GTK_GRID(add_grid), password2_btn, 2, 4, 1, 1);

    // autofill button
    GtkWidget *autofill_btn = gtk_button_new_with_label("Generate Random Password");
    gtk_widget_set_can_focus(autofill_btn, FALSE);
    gtk_widget_set_tooltip_text(autofill_btn, "Click to suggest a strong password.");
    // g_signal_connect(GTK_BUTTON(autofill_btn), "clicked", G_CALLBACK(autofill_passwords), self);
    gtk_grid_attach(GTK_GRID(add_grid), autofill_btn, 0, 5, 3, 1);

    // submit button
    GtkWidget *submit_btn = gtk_button_new_with_label("Add New Password");
    gtk_widget_set_can_focus(submit_btn, FALSE);
    g_signal_connect(GTK_BUTTON(submit_btn), "clicked", G_CALLBACK(add_grid_check), self);
    gtk_grid_attach(GTK_GRID(add_grid), submit_btn, 0, 6, 3, 1);

    return add_grid;
}

/*-----------------------------------------------------------------------------
Validate the entries for adding a new password. If valid, encrypt the data and
append it to the password file.
-----------------------------------------------------------------------------*/
void add_grid_check(GtkButton *btn, gpointer data)
{
    selection_window_t *self = data;
    char const *website = gtk_entry_get_text(GTK_ENTRY(self->website_ent));
    char const *username = gtk_entry_get_text(GTK_ENTRY(self->username_ent));
    char const *password1 = gtk_entry_get_text(GTK_ENTRY(self->password1_ent));
    char const *password2 = gtk_entry_get_text(GTK_ENTRY(self->password2_ent));

    // sanity 1
    if(!strcmp(website, "") || !strcmp(username, "") || !strcmp(password1, "") || !strcmp(password2, ""))
    {
        widget_toast_show(self->window, "Cannot add password. One or more fields are empty.");
        return;
    }

    // sanity 2
    if(strcmp(password1, password2))
    {
        widget_toast_show(self->window, "Cannot add password. Fields \'Password\' and \'Confirm Password\' do not match.");
        return;
    }

    // confirmation
    int response = selection_window_ask_for_confirmation(self, "Are you sure you want to add this password?");
    if(response != GTK_RESPONSE_ACCEPT)
    {
        return;
    }

    // encrypt and write to password file
    password_item_t *item = password_item_new_from_plaintext(website, username, password1, self->kek);
    selection_window_append(self, item);

    selection_window_clear_entries(NULL, NULL, 0, self);
    widget_toast_show(self->window, "Password added successfully.");
}

/*-----------------------------------------------------------------------------
Create a GTK grid. Create labels and entries to change the passphrase.
-----------------------------------------------------------------------------*/
GtkWidget *change_grid_new(selection_window_t *self)
{
    // grid
    GtkWidget *change_grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(change_grid), 50);
    gtk_grid_set_column_spacing(GTK_GRID(change_grid), 25);
    gtk_grid_set_row_spacing(GTK_GRID(change_grid), 25);
    gtk_widget_set_halign(change_grid, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(change_grid, TRUE);

    // header label
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), msg_change_header);
    gtk_grid_attach(GTK_GRID(change_grid), header, 0, 0, 3, 1);

    // passphrase prompt label
    GtkWidget *passphrase1_lbl = gtk_label_new("New Passphrase");
    gtk_grid_attach(GTK_GRID(change_grid), passphrase1_lbl, 0, 1, 1, 1);

    // passphrase response entry
    self->passphrase1_ent = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(self->passphrase1_ent), FALSE);
    gtk_grid_attach(GTK_GRID(change_grid), self->passphrase1_ent, 1, 1, 1, 1);

    // passphrase visibility toggling button
    GtkWidget *passphrase1_btn = gtk_button_new();
    gtk_widget_set_can_focus(passphrase1_btn, FALSE);
    gtk_button_set_image(GTK_BUTTON(passphrase1_btn), gtk_image_new_from_file(icon_vis));
    gtk_widget_set_tooltip_text(passphrase1_btn, "Click to show or hide passphrase.");
    g_signal_connect(passphrase1_btn, "clicked", G_CALLBACK(toggle_visibility), self->passphrase1_ent);
    gtk_grid_attach(GTK_GRID(change_grid), passphrase1_btn, 2, 1, 1, 1);

    // confirm passphrase prompt label
    GtkWidget *passphrase2_lbl = gtk_label_new("Confirm New Passphrase");
    gtk_grid_attach(GTK_GRID(change_grid), passphrase2_lbl, 0, 2, 1, 1);

    // confirm passphrase response entry
    self->passphrase2_ent = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(self->passphrase2_ent), FALSE);
    gtk_grid_attach(GTK_GRID(change_grid), self->passphrase2_ent, 1, 2, 1, 1);

    // confirm passphrase visibility toggling button
    GtkWidget *passphrase2_btn = gtk_button_new();
    gtk_widget_set_can_focus(passphrase2_btn, FALSE);
    gtk_button_set_image(GTK_BUTTON(passphrase2_btn), gtk_image_new_from_file(icon_vis));
    gtk_widget_set_tooltip_text(passphrase2_btn, "Click to show or hide passphrase.");
    g_signal_connect(passphrase2_btn, "clicked", G_CALLBACK(toggle_visibility), self->passphrase2_ent);
    gtk_grid_attach(GTK_GRID(change_grid), passphrase2_btn, 2, 2, 1, 1);

    // submit button
    GtkWidget *submit_btn = gtk_button_new_with_label("Change Passphrase");
    gtk_widget_set_can_focus(submit_btn, FALSE);
    g_signal_connect(GTK_BUTTON(submit_btn), "clicked", G_CALLBACK(change_grid_check), self);
    gtk_grid_attach(GTK_GRID(change_grid), submit_btn, 0, 3, 3, 1);

    return change_grid;
}

/*-----------------------------------------------------------------------------
Validate the entries for changing the passphrase. If valid, obtain the new key
encryption key, re-encrypt the passwords, and overwrite the password file.
-----------------------------------------------------------------------------*/
void change_grid_check(GtkButton *btn, gpointer data)
{
}

