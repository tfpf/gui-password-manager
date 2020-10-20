/*-----------------------------------------------------------------------------
Wrapper struct to handle addition, deletion and updation of password items, and
updation of the passphrase.

Members:
    window: main window in which widgets will be added
    notif_revealer: struct which shows notifications in `window'
    construction_in_progress: whether all child widgets of `window' are created
    search_ent: entry in which the user can type to search for something
    bottom_grid: grid in which search results will be populated
    website_add_ent: entry to type the website while adding a new item
    username_add_ent: entry to type the username while adding a new item
    password1_add_ent: entry to type the password while adding a new item
    password2_add_ent: entry to retype the password while adding a new item
    passphrase1_ent: entry to type the new passphrase while changing it
    passphrase2_ent: entry to retype the new passphrase while changing it

    window_edit: secondary window which is used to edit an existing item
    notif_revealer_edit: struct which shows notifications in `window_edit'
    website_edit_ent: entry to type the website while editing a new item
    username_edit_ent: entry to type the username while editing a new item
    password1_edit_ent: entry to type the password while editing a new item
    password2_edit_ent: entry to retype the password while editing a new item

    layout: Pango layout used to calculate the rendered text size
    website_width: width of the column which displays websites
    username_width: width of the column which displays usernames
    password_width: width of the column which displays passwords

    items: array of all existing items
    num_of_items: number of items in `items' (duh)

    kek: key encryption key
-----------------------------------------------------------------------------*/
typedef struct
{
    GtkWidget *window;
    notification_revealer_t *notif_revealer;
    gboolean construction_in_progress;
    GtkWidget *search_ent;
    GtkWidget *bottom_grid;
    GtkWidget *website_add_ent;
    GtkWidget *username_add_ent;
    GtkWidget *password1_add_ent;
    GtkWidget *password2_add_ent;
    GtkWidget *passphrase1_ent;
    GtkWidget *passphrase2_ent;

    GtkWidget *window_edit;
    notification_revealer_t *notif_revealer_edit;
    GtkWidget *website_edit_ent;
    GtkWidget *username_edit_ent;
    GtkWidget *password1_edit_ent;
    GtkWidget *password2_edit_ent;

    PangoLayout *layout;
    int website_width;
    int username_width;
    int password_width;

    password_item_t **items;
    int num_of_items;

    char unsigned *kek;
}
selection_window_t;

/*-----------------------------------------------------------------------------
Function prototypes.
-----------------------------------------------------------------------------*/
selection_window_t *selection_window_new(char unsigned *kek);
void selection_window_main(selection_window_t *self);
int selection_window_get_width_of_string(selection_window_t *self, char const *string);
void selection_window_get_widths_of_columns(selection_window_t *self);
void selection_window_sort_items(selection_window_t *self);
void selection_window_clear_entries(GtkNotebook *notebook, GtkWidget *page, guint page_num, selection_window_t *self);
void selection_window_clear_entry(GtkWidget *widget);
void selection_window_quit(GtkWidget *window, selection_window_t *self);

GtkWidget *manage_box_new(selection_window_t *self);
void manage_box_update(GtkEntry *search_ent, selection_window_t *self);
void manage_box_show_password(GtkButton *btn, password_item_t *item);
void manage_box_copy_password(GtkButton *btn, selection_window_t *self);
void manage_box_delete_password(GtkButton *btn, selection_window_t *self);

void edit_window_new(GtkButton *btn, selection_window_t *self);
void edit_window_autofill(GtkButton *btn, selection_window_t *self);
void edit_window_check(GtkButton *btn, selection_window_t *self);
void edit_window_quit(GtkWidget *window, gpointer data);

GtkWidget *add_grid_new(selection_window_t *self);
void add_grid_autofill(GtkButton *btn, selection_window_t *self);
void add_grid_check(GtkButton *btn, selection_window_t *self);

GtkWidget *change_grid_new(selection_window_t *self);
void change_grid_check(GtkButton *btn, selection_window_t *self);

/*-----------------------------------------------------------------------------
Initialiser for the `selection_window_t' struct. Create a GTK window and
populate it with widgets. Save the widgets of interest as members of the
struct.
-----------------------------------------------------------------------------*/
selection_window_t *selection_window_new(char unsigned *kek)
{
    selection_window_t *self = malloc(sizeof *self);
    self->construction_in_progress = TRUE;
    self->kek = kek;
    self->items = password_items_new_from_file(&(self->num_of_items), self->kek);
    selection_window_sort_items(self);

    // window
    self->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(self->window), 0);
    gtk_window_maximize(GTK_WINDOW(self->window));
    gtk_window_set_icon_from_file(GTK_WINDOW(self->window), icon_main, NULL);
    gtk_window_set_position(GTK_WINDOW(self->window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(self->window), TRUE);
    gtk_window_set_title(GTK_WINDOW(self->window), str_title);
    g_signal_connect(self->window, "destroy", G_CALLBACK(selection_window_quit), self);

    // overlay
    GtkWidget *overlay = gtk_overlay_new();
    gtk_container_add(GTK_CONTAINER(self->window), overlay);

    // notebook
    GtkWidget *notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_LEFT);
    gtk_container_add(GTK_CONTAINER(overlay), notebook);
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

    // notification revealer
    self->notif_revealer = notification_revealer_new();
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), self->notif_revealer->revealer);

    // obtain font description
    PangoFontDescription *font;
    GtkStyleContext *style = gtk_widget_get_style_context(self->window);
    gtk_style_context_get(style, gtk_style_context_get_state(style), GTK_STYLE_PROPERTY_FONT, &font, NULL);
    // gtk_style_context_get(style, GTK_STATE_FLAG_SELECTED, GTK_STYLE_PROPERTY_FONT, &font, NULL);

    // obtain Pango layout
    self->layout = pango_layout_new(gtk_widget_get_pango_context(self->window));
    pango_layout_set_font_description(self->layout, font);

    selection_window_get_widths_of_columns(self);

    return self;
}

/*-----------------------------------------------------------------------------
Start the main event loop.
-----------------------------------------------------------------------------*/
void selection_window_main(selection_window_t *self)
{
    gtk_widget_show_all(self->window);
    self->construction_in_progress = FALSE;
    manage_box_update(GTK_ENTRY(self->search_ent), self);
    gtk_main();
}

/*-----------------------------------------------------------------------------
Obtain the size (in pixels) of a string as it is rendered.
-----------------------------------------------------------------------------*/
int selection_window_get_width_of_string(selection_window_t *self, char const *string)
{
    pango_layout_set_text(self->layout, string, -1);
    int w;
    pango_layout_get_pixel_size(self->layout, &w, NULL);
    return w;
}

/*-----------------------------------------------------------------------------
Obtain the sizes of the columns which display the website, username and
password. The size of a column would be the size of the widest string which
goes into that column.
-----------------------------------------------------------------------------*/
void selection_window_get_widths_of_columns(selection_window_t *self)
{
    self->website_width = -1;
    self->username_width = -1;
    self->password_width = -1;

    for(int i = 0; i < self->num_of_items; ++i)
    {
        int website_width = selection_window_get_width_of_string(self, self->items[i]->website);
        if(website_width > self->website_width)
        {
            self->website_width = website_width;
        }

        int username_width = selection_window_get_width_of_string(self, self->items[i]->username);
        if(username_width > self->username_width)
        {
            self->username_width = username_width;
        }

        int password_width = selection_window_get_width_of_string(self, self->items[i]->password);
        if(password_width > self->password_width)
        {
            self->password_width = password_width;
        }
    }

    float multiplier = 1.2;
    self->website_width *= multiplier;
    self->username_width *= multiplier;
    self->password_width *= multiplier;
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
Clear all entries on all pages of the GTK notebook in the window. This function
is called automatically when:
    (1) the GTK notebook page is changed.
    (2) GTK notebook page creation.
In the latter case, the GTK window has not been drawn completely, so nothing
needs to be done.
-----------------------------------------------------------------------------*/
void selection_window_clear_entries(GtkNotebook *notebook, GtkWidget *page, guint page_num, selection_window_t *self)
{
    if(self->construction_in_progress)
    {
        return;
    }

    selection_window_clear_entry(self->search_ent);
    selection_window_clear_entry(self->website_add_ent);
    selection_window_clear_entry(self->username_add_ent);
    selection_window_clear_entry(self->password1_add_ent);
    selection_window_clear_entry(self->password2_add_ent);
    selection_window_clear_entry(self->passphrase1_ent);
    selection_window_clear_entry(self->passphrase2_ent);
}

/*-----------------------------------------------------------------------------
Clear the given GTK entry.

Calling `gtk_entry_buffer_delete_text' will clear the text, but it will not
cause the entry to emit a `changed' signal. I use this signal to update the
search results, so I need the entry to emit this signal. Hence, the contents of
the entry are first set to a non-empty string, and then cleared again.
-----------------------------------------------------------------------------*/
void selection_window_clear_entry(GtkWidget *widget)
{
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(widget));
    gtk_entry_buffer_delete_text(buffer, 0, -1);
    gtk_entry_buffer_set_text(buffer, " ", -1);
    gtk_entry_buffer_delete_text(buffer, 0, -1);
}

/*-----------------------------------------------------------------------------
Stop the main event loop.
-----------------------------------------------------------------------------*/
void selection_window_quit(GtkWidget *window, selection_window_t *self)
{
    gtk_widget_destroy(window);
    gtk_main_quit();

    // clear the sensitive data from memory
    password_items_delete(self->items, self->num_of_items);
    zero_and_free(self->kek, AES_KEY_LENGTH);
    g_object_unref(self->layout);

    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_clear(clipboard);
}

/*-----------------------------------------------------------------------------
Create a GTK box. Add a grid in it. This grid is saved as a member of the
`selection_window_t' struct.
-----------------------------------------------------------------------------*/
GtkWidget *manage_box_new(selection_window_t *self)
{
    // box
    GtkWidget *manage_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // grid to be put in the box
    GtkWidget *top_grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(top_grid), 50);
    gtk_grid_set_column_spacing(GTK_GRID(top_grid), 25);
    gtk_grid_set_row_spacing(GTK_GRID(top_grid), 25);
    gtk_widget_set_halign(top_grid, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(top_grid, TRUE);
    gtk_box_pack_start(GTK_BOX(manage_box), top_grid, FALSE, FALSE, 0);

    // header label
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), msg_manage_header);
    gtk_grid_attach(GTK_GRID(top_grid), header, 0, 0, 2, 1);

    // search prompt label
    GtkWidget *search_lbl = gtk_label_new(str_search);
    gtk_grid_attach(GTK_GRID(top_grid), search_lbl, 0, 1, 1, 1);

    // search response entry
    self->search_ent = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(top_grid), self->search_ent, 1, 1, 1, 1);
    g_signal_connect(self->search_ent, "changed", G_CALLBACK(manage_box_update), self);

    // scrollable window to be put in the box
    GtkWidget *scrollable = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(scrollable), 0);
    gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(scrollable), FALSE);
    gtk_scrolled_window_set_placement(GTK_SCROLLED_WINDOW(scrollable), GTK_CORNER_TOP_LEFT);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollable), GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
    gtk_widget_set_can_focus(scrollable, FALSE);
    gtk_box_pack_start(GTK_BOX(manage_box), scrollable, TRUE, TRUE, 0);

    // grid to be put in the scrollable window
    self->bottom_grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(self->bottom_grid), 25);
    gtk_grid_set_column_spacing(GTK_GRID(self->bottom_grid), 15);
    gtk_grid_set_row_spacing(GTK_GRID(self->bottom_grid), 15);
    gtk_widget_set_halign(self->bottom_grid, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(self->bottom_grid, TRUE);
    gtk_container_add(GTK_CONTAINER(scrollable), self->bottom_grid);

    return manage_box;
}

/*-----------------------------------------------------------------------------
Create widgets to display those items for which the search term is a
case-insensitive substring of either the website or the username. If such
matches are found, they are added to the GTK grid, and the headers for the grid
are created. The widths of these headers are such that all strings can fit in
them comfortably.

If no matches are found, the headers are not created, and an error message is
displayed instead.

Note that the search entry is received as the first argument of the function.
Wherefore, there is no need to refer to the entry as `self->search_ent'.
-----------------------------------------------------------------------------*/
void manage_box_update(GtkEntry *search_ent, selection_window_t *self)
{
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
        if(!my_strcasestr(self->items[i]->website, search) && !my_strcasestr(self->items[i]->username, search))
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
        GtkWidget *password_btn = gtk_button_new_with_label(str_placeholder);
        gtk_widget_set_can_focus(password_btn, FALSE);
        gtk_widget_set_tooltip_text(password_btn, str_show_password);
        g_signal_connect(password_btn, "clicked", G_CALLBACK(manage_box_show_password), self->items[i]);
        gtk_grid_attach(GTK_GRID(self->bottom_grid), password_btn, 2, i, 1, 1);

        // name for identifying edit and delete buttons
        int length = snprintf(NULL, 0, "%d", i);
        char *name = malloc((length + 1) * sizeof *name);
        snprintf(name, length + 1, "%d", i);

        // copy button
        GtkWidget *copy_btn = gtk_button_new();
        gtk_button_set_image(GTK_BUTTON(copy_btn), gtk_image_new_from_file(icon_copy));
        gtk_widget_set_can_focus(copy_btn, FALSE);
        gtk_widget_set_name(copy_btn, name);
        gtk_widget_set_tooltip_text(copy_btn, str_copy_password);
        g_signal_connect(copy_btn, "clicked", G_CALLBACK(manage_box_copy_password), self);
        gtk_grid_attach(GTK_GRID(self->bottom_grid), copy_btn, 3, i, 1, 1);

        // edit button
        GtkWidget *edit_btn = gtk_button_new();
        gtk_button_set_image(GTK_BUTTON(edit_btn), gtk_image_new_from_file(icon_edit));
        gtk_widget_set_can_focus(edit_btn, FALSE);
        gtk_widget_set_name(edit_btn, name);
        gtk_widget_set_tooltip_text(edit_btn, str_edit_password);
        g_signal_connect(edit_btn, "clicked", G_CALLBACK(edit_window_new), self);
        gtk_grid_attach(GTK_GRID(self->bottom_grid), edit_btn, 4, i, 1, 1);

        // delete button
        GtkWidget *delete_btn = gtk_button_new();
        gtk_button_set_image(GTK_BUTTON(delete_btn), gtk_image_new_from_file(icon_del));
        gtk_widget_set_can_focus(delete_btn, FALSE);
        gtk_widget_set_name(delete_btn, name);
        gtk_widget_set_tooltip_text(delete_btn, str_delete_password);
        g_signal_connect(delete_btn, "clicked", G_CALLBACK(manage_box_delete_password), self);
        gtk_grid_attach(GTK_GRID(self->bottom_grid), delete_btn, 5, i, 1, 1);

        free(name);
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
    gtk_widget_set_size_request(header_website, self->website_width, -1);
    gtk_grid_attach(GTK_GRID(self->bottom_grid), header_website, 0, -1, 1, 1);

    // username header label
    GtkWidget *header_username = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header_username), msg_manage_username);
    gtk_widget_set_size_request(header_username, self->username_width, -1);
    gtk_grid_attach(GTK_GRID(self->bottom_grid), header_username, 1, -1, 1, 1);

    // password header label
    GtkWidget *header_password = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header_password), msg_manage_password);
    gtk_widget_set_size_request(header_password, self->password_width, -1);
    gtk_grid_attach(GTK_GRID(self->bottom_grid), header_password, 2, -1, 1, 1);

    gtk_widget_show_all(self->bottom_grid);
}

/*-----------------------------------------------------------------------------
Display a password on the button which was clicked.
-----------------------------------------------------------------------------*/
void manage_box_show_password(GtkButton *btn, password_item_t *item)
{
    gtk_button_set_label(btn, item->password);
    gtk_widget_set_has_tooltip(GTK_WIDGET(btn), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(btn), FALSE);
}

/*-----------------------------------------------------------------------------
Copy a password to the clipboard.
-----------------------------------------------------------------------------*/
void manage_box_copy_password(GtkButton *btn, selection_window_t *self)
{
    char const *name = gtk_widget_get_name(GTK_WIDGET(btn));
    int i = atoi(name);
    password_item_t *item = self->items[i];

    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_text(clipboard, item->password, -1);
    notification_revealer_show(self->notif_revealer, str_copy_password_done);
}

/*-----------------------------------------------------------------------------
Delete an item.
-----------------------------------------------------------------------------*/
void manage_box_delete_password(GtkButton *btn, selection_window_t *self)
{
    char const *name = gtk_widget_get_name(GTK_WIDGET(btn));
    int i = atoi(name);
    password_item_t *item = self->items[i];

    // confirmation
    int response = request_confirmation(self->window, str_delete_password_question, item->website, item->username);
    if(response != GTK_RESPONSE_ACCEPT)
    {
        return;
    }

    password_item_delete(item);
    --self->num_of_items;
    for(int j = i; j < self->num_of_items; ++j)
    {
        self->items[j] = self->items[j + 1];
    }
    password_items_write_to_file(self->items, self->num_of_items);

    selection_window_get_widths_of_columns(self);
    manage_box_update(GTK_ENTRY(self->search_ent), self);
    notification_revealer_show(self->notif_revealer, str_delete_password_done);
}

/*-----------------------------------------------------------------------------
Open a window to edit an item of the array.
-----------------------------------------------------------------------------*/
void edit_window_new(GtkButton *btn, selection_window_t *self)
{
    char const *name = gtk_widget_get_name(GTK_WIDGET(btn));
    int i = atoi(name);
    password_item_t *item = self->items[i];

    // window
    self->window_edit = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(self->window_edit), 0);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(self->window_edit), TRUE);
    gtk_window_set_icon_from_file(GTK_WINDOW(self->window_edit), icon_main, NULL);
    gtk_window_set_modal(GTK_WINDOW(self->window_edit), TRUE);
    gtk_window_set_position(GTK_WINDOW(self->window_edit), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(self->window_edit), FALSE);
    gtk_window_set_title(GTK_WINDOW(self->window_edit), str_title);
    gtk_window_set_transient_for(GTK_WINDOW(self->window_edit), GTK_WINDOW(self->window));
    g_signal_connect(self->window_edit, "destroy", G_CALLBACK(edit_window_quit), NULL);

    // overlay
    GtkWidget *overlay = gtk_overlay_new();
    gtk_container_add(GTK_CONTAINER(self->window_edit), overlay);

    // grid
    GtkWidget *grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(grid), 50);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 25);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 25);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_container_add(GTK_CONTAINER(overlay), grid);

    // header label
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), msg_edit_header);
    gtk_grid_attach(GTK_GRID(grid), header, 0, 0, 3, 1);

    // website prompt label
    GtkWidget *website_lbl = gtk_label_new(str_website);
    gtk_grid_attach(GTK_GRID(grid), website_lbl, 0, 1, 1, 1);

    // website response entry
    self->website_edit_ent = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(self->website_edit_ent), item->website);
    gtk_entry_set_text(GTK_ENTRY(self->website_edit_ent), item->website);
    gtk_entry_set_width_chars(GTK_ENTRY(self->website_edit_ent), ENTRY_WIDTH);
    gtk_grid_attach(GTK_GRID(grid), self->website_edit_ent, 1, 1, 1, 1);

    // username prompt label
    GtkWidget *username_lbl = gtk_label_new(str_username);
    gtk_grid_attach(GTK_GRID(grid), username_lbl, 0, 2, 1, 1);

    // username response entry
    self->username_edit_ent = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(self->username_edit_ent), item->username);
    gtk_entry_set_text(GTK_ENTRY(self->username_edit_ent), item->username);
    gtk_entry_set_width_chars(GTK_ENTRY(self->username_edit_ent), ENTRY_WIDTH);
    gtk_grid_attach(GTK_GRID(grid), self->username_edit_ent, 1, 2, 1, 1);

    // password prompt label
    GtkWidget *password1_lbl = gtk_label_new(str_password1);
    gtk_grid_attach(GTK_GRID(grid), password1_lbl, 0, 3, 1, 1);

    // password response entry
    self->password1_edit_ent = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(self->password1_edit_ent), FALSE);
    gtk_entry_set_width_chars(GTK_ENTRY(self->password1_edit_ent), ENTRY_WIDTH);
    gtk_grid_attach(GTK_GRID(grid), self->password1_edit_ent, 1, 3, 1, 1);

    // password visibility toggling button
    GtkWidget *password1_btn = gtk_button_new();
    gtk_widget_set_can_focus(password1_btn, FALSE);
    gtk_button_set_image(GTK_BUTTON(password1_btn), gtk_image_new_from_file(icon_vis));
    gtk_widget_set_tooltip_text(password1_btn, str_toggle_password);
    g_signal_connect(password1_btn, "clicked", G_CALLBACK(toggle_visibility), self->password1_edit_ent);
    gtk_grid_attach(GTK_GRID(grid), password1_btn, 2, 3, 1, 1);

    // confirm password prompt label
    GtkWidget *password2_lbl = gtk_label_new(str_password2);
    gtk_grid_attach(GTK_GRID(grid), password2_lbl, 0, 4, 1, 1);

    // confirm password response entry
    self->password2_edit_ent = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(self->password2_edit_ent), FALSE);
    gtk_entry_set_width_chars(GTK_ENTRY(self->password2_edit_ent), ENTRY_WIDTH);
    gtk_grid_attach(GTK_GRID(grid), self->password2_edit_ent, 1, 4, 1, 1);

    // confirm password visibility toggling button
    GtkWidget *password2_btn = gtk_button_new();
    gtk_widget_set_can_focus(password2_btn, FALSE);
    gtk_button_set_image(GTK_BUTTON(password2_btn), gtk_image_new_from_file(icon_vis));
    gtk_widget_set_tooltip_text(password2_btn, str_toggle_password);
    g_signal_connect(password2_btn, "clicked", G_CALLBACK(toggle_visibility), self->password2_edit_ent);
    gtk_grid_attach(GTK_GRID(grid), password2_btn, 2, 4, 1, 1);

    // autofill button
    GtkWidget *autofill_btn = gtk_button_new_with_label(str_password_gen);
    gtk_widget_set_can_focus(autofill_btn, FALSE);
    gtk_widget_set_tooltip_text(autofill_btn, str_suggest_password);
    g_signal_connect(GTK_BUTTON(autofill_btn), "clicked", G_CALLBACK(edit_window_autofill), self);
    gtk_grid_attach(GTK_GRID(grid), autofill_btn, 0, 5, 3, 1);

    // submit button
    GtkWidget *submit_btn = gtk_button_new_with_label(str_edit);
    gtk_widget_set_can_focus(submit_btn, FALSE);
    gtk_widget_set_name(submit_btn, name);
    g_signal_connect(GTK_BUTTON(submit_btn), "clicked", G_CALLBACK(edit_window_check), self);
    gtk_grid_attach(GTK_GRID(grid), submit_btn, 0, 6, 3, 1);

    // notification revealer
    self->notif_revealer_edit = notification_revealer_new();
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), self->notif_revealer_edit->revealer);

    gtk_widget_show_all(self->window_edit);
    gtk_main();
}

/*-----------------------------------------------------------------------------
Suggest a random password.
-----------------------------------------------------------------------------*/
void edit_window_autofill(GtkButton *btn, selection_window_t *self)
{
    char *suggestion = gen_rand_constrained(20, 30);
    gtk_entry_set_text(GTK_ENTRY(self->password1_edit_ent), suggestion);
    gtk_entry_set_text(GTK_ENTRY(self->password2_edit_ent), suggestion);
    zero_and_free((char unsigned *)suggestion, strlen(suggestion));
}

/*-----------------------------------------------------------------------------
Validate the entries for editing an item. If valid, save it to the array and
write it to the password file.
-----------------------------------------------------------------------------*/
void edit_window_check(GtkButton *btn, selection_window_t *self)
{
    char const *website = gtk_entry_get_text(GTK_ENTRY(self->website_edit_ent));
    char const *username = gtk_entry_get_text(GTK_ENTRY(self->username_edit_ent));
    char const *password1 = gtk_entry_get_text(GTK_ENTRY(self->password1_edit_ent));
    char const *password2 = gtk_entry_get_text(GTK_ENTRY(self->password2_edit_ent));

    // sanity 1
    if(!strcmp(website, "") || !strcmp(username, "") || !strcmp(password1, "") || !strcmp(password2, ""))
    {
        notification_revealer_show(self->notif_revealer_edit, str_edit_err1);
        return;
    }

    // sanity 2
    if(strcmp(password1, password2))
    {
        notification_revealer_show(self->notif_revealer_edit, str_edit_err2);
        return;
    }

    // confirmation
    int response = request_confirmation(self->window_edit, str_edit_password_question, NULL, NULL);
    if(response != GTK_RESPONSE_ACCEPT)
    {
        return;
    }

    char const *name = gtk_widget_get_name(GTK_WIDGET(btn));
    int i = atoi(name);

    password_item_delete(self->items[i]);
    self->items[i] = password_item_new_from_plaintext(website, username, password1, self->kek);
    password_items_write_to_file(self->items, self->num_of_items);

    edit_window_quit(self->window_edit, NULL);
    selection_window_sort_items(self);
    selection_window_get_widths_of_columns(self);
    manage_box_update(GTK_ENTRY(self->search_ent), self);
    notification_revealer_show(self->notif_revealer, str_edit_password_done);
}

/*-----------------------------------------------------------------------------
Stop the main event loop.
-----------------------------------------------------------------------------*/
void edit_window_quit(GtkWidget *window, gpointer data)
{
    gtk_widget_destroy(window);
    gtk_main_quit();
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
    GtkWidget *website_lbl = gtk_label_new(str_website);
    gtk_grid_attach(GTK_GRID(add_grid), website_lbl, 0, 1, 1, 1);

    // website response entry
    self->website_add_ent = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(self->website_add_ent), str_website_hint);
    gtk_entry_set_width_chars(GTK_ENTRY(self->website_add_ent), ENTRY_WIDTH);
    gtk_grid_attach(GTK_GRID(add_grid), self->website_add_ent, 1, 1, 1, 1);

    // username prompt label
    GtkWidget *username_lbl = gtk_label_new(str_username);
    gtk_grid_attach(GTK_GRID(add_grid), username_lbl, 0, 2, 1, 1);

    // username response entry
    self->username_add_ent = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(self->username_add_ent), ENTRY_WIDTH);
    gtk_grid_attach(GTK_GRID(add_grid), self->username_add_ent, 1, 2, 1, 1);

    // password prompt label
    GtkWidget *password1_lbl = gtk_label_new(str_password1);
    gtk_grid_attach(GTK_GRID(add_grid), password1_lbl, 0, 3, 1, 1);

    // password response entry
    self->password1_add_ent = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(self->password1_add_ent), FALSE);
    gtk_entry_set_width_chars(GTK_ENTRY(self->password1_add_ent), ENTRY_WIDTH);
    gtk_grid_attach(GTK_GRID(add_grid), self->password1_add_ent, 1, 3, 1, 1);

    // password visibility toggling button
    GtkWidget *password1_btn = gtk_button_new();
    gtk_widget_set_can_focus(password1_btn, FALSE);
    gtk_button_set_image(GTK_BUTTON(password1_btn), gtk_image_new_from_file(icon_vis));
    gtk_widget_set_tooltip_text(password1_btn, str_toggle_password);
    g_signal_connect(password1_btn, "clicked", G_CALLBACK(toggle_visibility), self->password1_add_ent);
    gtk_grid_attach(GTK_GRID(add_grid), password1_btn, 2, 3, 1, 1);

    // confirm password prompt label
    GtkWidget *password2_lbl = gtk_label_new(str_password2);
    gtk_grid_attach(GTK_GRID(add_grid), password2_lbl, 0, 4, 1, 1);

    // confirm password response entry
    self->password2_add_ent = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(self->password2_add_ent), FALSE);
    gtk_entry_set_width_chars(GTK_ENTRY(self->password2_add_ent), ENTRY_WIDTH);
    gtk_grid_attach(GTK_GRID(add_grid), self->password2_add_ent, 1, 4, 1, 1);

    // confirm password visibility toggling button
    GtkWidget *password2_btn = gtk_button_new();
    gtk_widget_set_can_focus(password2_btn, FALSE);
    gtk_button_set_image(GTK_BUTTON(password2_btn), gtk_image_new_from_file(icon_vis));
    gtk_widget_set_tooltip_text(password2_btn, str_toggle_password);
    g_signal_connect(password2_btn, "clicked", G_CALLBACK(toggle_visibility), self->password2_add_ent);
    gtk_grid_attach(GTK_GRID(add_grid), password2_btn, 2, 4, 1, 1);

    // autofill button
    GtkWidget *autofill_btn = gtk_button_new_with_label(str_password_gen);
    gtk_widget_set_can_focus(autofill_btn, FALSE);
    gtk_widget_set_tooltip_text(autofill_btn, str_suggest_password);
    g_signal_connect(GTK_BUTTON(autofill_btn), "clicked", G_CALLBACK(add_grid_autofill), self);
    gtk_grid_attach(GTK_GRID(add_grid), autofill_btn, 0, 5, 3, 1);

    // submit button
    GtkWidget *submit_btn = gtk_button_new_with_label(str_add);
    gtk_widget_set_can_focus(submit_btn, FALSE);
    g_signal_connect(GTK_BUTTON(submit_btn), "clicked", G_CALLBACK(add_grid_check), self);
    gtk_grid_attach(GTK_GRID(add_grid), submit_btn, 0, 6, 3, 1);

    return add_grid;
}

/*-----------------------------------------------------------------------------
Suggest a random password.
-----------------------------------------------------------------------------*/
void add_grid_autofill(GtkButton *btn, selection_window_t *self)
{
    char *suggestion = gen_rand_constrained(20, 30);
    gtk_entry_set_text(GTK_ENTRY(self->password1_add_ent), suggestion);
    gtk_entry_set_text(GTK_ENTRY(self->password2_add_ent), suggestion);
    zero_and_free((char unsigned *)suggestion, strlen(suggestion));
}

/*-----------------------------------------------------------------------------
Validate the entries for adding a new password. If valid, encrypt the data and
append it to the password file.
-----------------------------------------------------------------------------*/
void add_grid_check(GtkButton *btn, selection_window_t *self)
{
    char const *website = gtk_entry_get_text(GTK_ENTRY(self->website_add_ent));
    char const *username = gtk_entry_get_text(GTK_ENTRY(self->username_add_ent));
    char const *password1 = gtk_entry_get_text(GTK_ENTRY(self->password1_add_ent));
    char const *password2 = gtk_entry_get_text(GTK_ENTRY(self->password2_add_ent));

    // sanity 1
    if(!strcmp(website, "") || !strcmp(username, "") || !strcmp(password1, "") || !strcmp(password2, ""))
    {
        notification_revealer_show(self->notif_revealer, str_add_err1);
        return;
    }

    // sanity 2
    if(strcmp(password1, password2))
    {
        notification_revealer_show(self->notif_revealer, str_add_err2);
        return;
    }

    // confirmation
    int response = request_confirmation(self->window, str_add_password_question, NULL, NULL);
    if(response != GTK_RESPONSE_ACCEPT)
    {
        return;
    }

    password_item_t **temp = realloc(self->items, (self->num_of_items + 1) * sizeof *temp);
    if(temp == NULL)
    {
        fprintf(stderr, "Could not allocate memory to add password.\n");
        exit(EXIT_FAILURE);
    }
    self->items = temp;

    password_item_t *item = password_item_new_from_plaintext(website, username, password1, self->kek);
    self->items[self->num_of_items] = item;
    ++self->num_of_items;
    password_item_write_to_file(item);

    selection_window_sort_items(self);
    selection_window_get_widths_of_columns(self);
    selection_window_clear_entries(NULL, NULL, 0, self);
    notification_revealer_show(self->notif_revealer, str_add_password_done);
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
    GtkWidget *passphrase1_lbl = gtk_label_new(str_passphrase1);
    gtk_grid_attach(GTK_GRID(change_grid), passphrase1_lbl, 0, 1, 1, 1);

    // passphrase response entry
    self->passphrase1_ent = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(self->passphrase1_ent), FALSE);
    gtk_entry_set_width_chars(GTK_ENTRY(self->passphrase1_ent), ENTRY_WIDTH);
    gtk_grid_attach(GTK_GRID(change_grid), self->passphrase1_ent, 1, 1, 1, 1);

    // passphrase visibility toggling button
    GtkWidget *passphrase1_btn = gtk_button_new();
    gtk_widget_set_can_focus(passphrase1_btn, FALSE);
    gtk_button_set_image(GTK_BUTTON(passphrase1_btn), gtk_image_new_from_file(icon_vis));
    gtk_widget_set_tooltip_text(passphrase1_btn, str_toggle_passphrase);
    g_signal_connect(passphrase1_btn, "clicked", G_CALLBACK(toggle_visibility), self->passphrase1_ent);
    gtk_grid_attach(GTK_GRID(change_grid), passphrase1_btn, 2, 1, 1, 1);

    // confirm passphrase prompt label
    GtkWidget *passphrase2_lbl = gtk_label_new(str_passphrase2);
    gtk_grid_attach(GTK_GRID(change_grid), passphrase2_lbl, 0, 2, 1, 1);

    // confirm passphrase response entry
    self->passphrase2_ent = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(self->passphrase2_ent), FALSE);
    gtk_entry_set_width_chars(GTK_ENTRY(self->passphrase2_ent), ENTRY_WIDTH);
    gtk_grid_attach(GTK_GRID(change_grid), self->passphrase2_ent, 1, 2, 1, 1);

    // confirm passphrase visibility toggling button
    GtkWidget *passphrase2_btn = gtk_button_new();
    gtk_widget_set_can_focus(passphrase2_btn, FALSE);
    gtk_button_set_image(GTK_BUTTON(passphrase2_btn), gtk_image_new_from_file(icon_vis));
    gtk_widget_set_tooltip_text(passphrase2_btn, str_toggle_passphrase);
    g_signal_connect(passphrase2_btn, "clicked", G_CALLBACK(toggle_visibility), self->passphrase2_ent);
    gtk_grid_attach(GTK_GRID(change_grid), passphrase2_btn, 2, 2, 1, 1);

    // submit button
    GtkWidget *submit_btn = gtk_button_new_with_label(str_change);
    gtk_widget_set_can_focus(submit_btn, FALSE);
    g_signal_connect(GTK_BUTTON(submit_btn), "clicked", G_CALLBACK(change_grid_check), self);
    gtk_grid_attach(GTK_GRID(change_grid), submit_btn, 0, 3, 3, 1);

    return change_grid;
}

/*-----------------------------------------------------------------------------
Validate the entries for changing the passphrase. If valid, obtain the new key
encryption key, re-encrypt the passwords, and write to the passphrase and
password file.
-----------------------------------------------------------------------------*/
void change_grid_check(GtkButton *btn, selection_window_t *self)
{
    char const *passphrase1 = gtk_entry_get_text(GTK_ENTRY(self->passphrase1_ent));
    char const *passphrase2 = gtk_entry_get_text(GTK_ENTRY(self->passphrase2_ent));

    // sanity 1
    if(!strcmp(passphrase1, "") || !strcmp(passphrase2, ""))
    {
        notification_revealer_show(self->notif_revealer, str_change_passphrase_err1);
        return;
    }

    // sanity 2
    if(strcmp(passphrase1, passphrase2))
    {
        notification_revealer_show(self->notif_revealer, str_change_passphrase_err2);
        return;
    }

    // confirmation
    int response = request_confirmation(self->window, str_change_passphrase_question, NULL, NULL);
    if(response != GTK_RESPONSE_ACCEPT)
    {
        return;
    }

    // obtain new key encryption key
    // remember that the encryption system is AES256
    // whence, the AES key is 256 bits long
    // so is an SHA256 hash
    zero_and_free(self->kek, AES_KEY_LENGTH);
    self->kek = malloc(SHA256_DIGEST_LENGTH * sizeof *(self->kek));
    SHA256((char unsigned *)passphrase1, strlen(passphrase1), self->kek);

    for(int i = 0; i < self->num_of_items; ++i)
    {
        password_item_t *item = self->items[i];
        self->items[i] = password_item_new_from_plaintext(item->website, item->username, item->password, self->kek);
        password_item_delete(item);
    }
    password_items_write_to_file(self->items, self->num_of_items);
    passphrase_hash_to_file(passphrase1);

    selection_window_clear_entries(NULL, NULL, 0, self);
    notification_revealer_show(self->notif_revealer, str_change_passphrase_done);
}

