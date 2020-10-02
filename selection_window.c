/*-----------------------------------------------------------------------------
Wrapper around the GTK window struct. Its members are all the GTK entries in
the pages of the GTK notebook which will be added in the GTK window, and the
aforementioned GTK window itself.

The key encryption key (obtained from the initialiser) is also a member.
-----------------------------------------------------------------------------*/
typedef struct
{
    GtkWidget *window;

    // entries on the notebook page for adding passwords
    GtkWidget *website_ent;
    GtkWidget *username_ent;
    GtkWidget *password1_ent;
    GtkWidget *password2_ent;

    char unsigned *kek;
}
selection_window_t;

/*-----------------------------------------------------------------------------
Function prototypes.
-----------------------------------------------------------------------------*/
selection_window_t *selection_window_new(char unsigned *kek);
void selection_window_main(selection_window_t *self);
void selection_window_quit(GtkWidget *window, gpointer data);
GtkWidget *manage_box_new(selection_window_t *self);
GtkWidget *add_grid_new(selection_window_t *self);
void add_grid_check(GtkButton *btn, gpointer data);

/*-----------------------------------------------------------------------------
Initialiser for the struct defined above. Create the GTK window. Then save it
and its passphrase entry as the struct members.
-----------------------------------------------------------------------------*/
selection_window_t *selection_window_new(char unsigned *kek)
{
    selection_window_t *self = malloc(sizeof *self);
    self->kek = kek;

    // window
    self->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(self->window), 0);
    gtk_window_set_icon_from_file(GTK_WINDOW(self->window), icon_main, NULL);
    gtk_window_set_position(GTK_WINDOW(self->window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(self->window), TRUE);
    gtk_window_set_title(GTK_WINDOW(self->window), "Password Manager");

    // notebook
    GtkWidget *notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_LEFT);
    gtk_container_add(GTK_CONTAINER(self->window), notebook);
    // g_signal_connect(GTK_NOTEBOOK(notebook), "switch-page", G_CALLBACK(clear_all_entries), window);

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

    return self;
}

/*-----------------------------------------------------------------------------
Create a GTK box. Populate it with all the password items present in the
password file.
-----------------------------------------------------------------------------*/
GtkWidget *manage_box_new(selection_window_t *self)
{
    GtkWidget *manage_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    return manage_box;
}

/*-----------------------------------------------------------------------------
Create a GTK grid. Create labels and entries to add a new password.
-----------------------------------------------------------------------------*/
GtkWidget *add_grid_new(selection_window_t *self)
{
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
}

/*-----------------------------------------------------------------------------
Call the main GTK loop, which will open the window.
-----------------------------------------------------------------------------*/
void selection_window_main(selection_window_t *self)
{
    gtk_widget_show_all(self->window);
    gtk_window_maximize(GTK_WINDOW(self->window));
    g_signal_connect(self->window, "destroy", G_CALLBACK(selection_window_quit), NULL);
    gtk_main();
}

/*-----------------------------------------------------------------------------
Automatically called when the window is closed. Quit the GTK main loop.
-----------------------------------------------------------------------------*/
void selection_window_quit(GtkWidget *window, gpointer data)
{
    gtk_widget_destroy(window);
    gtk_main_quit();
}

