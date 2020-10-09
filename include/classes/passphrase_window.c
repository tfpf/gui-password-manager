/*-----------------------------------------------------------------------------
Wrapper struct to allow easy but scoped access to the passphrase and the key
encryption key.
-----------------------------------------------------------------------------*/
typedef struct
{
    GtkWidget *window;         // main window
    GtkWidget *passphrase_ent;

    char unsigned *kek;        // key encryption key
}
passphrase_window_t;

/*-----------------------------------------------------------------------------
Function prototypes.
-----------------------------------------------------------------------------*/
passphrase_window_t *passphrase_window_new(void);
void passphrase_window_main(passphrase_window_t *self);
void passphrase_window_check(GtkButton *btn, passphrase_window_t *self);
char unsigned *passphrase_hash_from_file(void);
void passphrase_hash_to_file(char const *passphrase);
void passphrase_window_quit(GtkWidget *window, gpointer data);

/*-----------------------------------------------------------------------------
Initialiser for the struct defined above. Create the GTK window. Then save it
and its passphrase entry as the struct members.
-----------------------------------------------------------------------------*/
passphrase_window_t *passphrase_window_new(void)
{
    passphrase_window_t *self = malloc(sizeof *self);
    self->kek = NULL;

    // window
    self->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(self->window), 0);
    gtk_window_set_icon_from_file(GTK_WINDOW(self->window), icon_main, NULL);
    gtk_window_set_position(GTK_WINDOW(self->window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(self->window), FALSE);
    gtk_window_set_title(GTK_WINDOW(self->window), str_title);
    g_signal_connect(self->window, "destroy", G_CALLBACK(passphrase_window_quit), NULL);

    // grid
    GtkWidget *grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(grid), 50);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 25);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 25);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_container_add(GTK_CONTAINER(self->window), grid);

    // header label
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), msg_passphrase);
    gtk_grid_attach(GTK_GRID(grid), header, 0, 0, 3, 1);

    // passphrase prompt label
    GtkWidget *passphrase_lbl = gtk_label_new(str_passphrase);
    gtk_grid_attach(GTK_GRID(grid), passphrase_lbl, 0, 1, 1, 1);

    // passphrase response entry
    self->passphrase_ent = gtk_entry_new();
    gtk_entry_set_activates_default(GTK_ENTRY(self->passphrase_ent), TRUE);
    gtk_entry_set_visibility(GTK_ENTRY(self->passphrase_ent), FALSE);
    gtk_widget_grab_focus(self->passphrase_ent);
    gtk_grid_attach(GTK_GRID(grid), self->passphrase_ent, 1, 1, 1, 1);

    // passphrase visibility toggling button
    GtkWidget *toggle_btn = gtk_button_new();
    gtk_widget_set_can_focus(toggle_btn, FALSE);
    gtk_button_set_image(GTK_BUTTON(toggle_btn), gtk_image_new_from_file(icon_vis));
    gtk_widget_set_tooltip_text(toggle_btn, str_toggle_passphrase);
    g_signal_connect(toggle_btn, "clicked", G_CALLBACK(toggle_visibility), self->passphrase_ent);
    gtk_grid_attach(GTK_GRID(grid), toggle_btn, 2, 1, 1, 1);

    // submit button
    GtkWidget *submit_btn = gtk_button_new_with_label(str_submit);
    gtk_widget_set_can_focus(submit_btn, FALSE);
    g_signal_connect(GTK_BUTTON(submit_btn), "clicked", G_CALLBACK(passphrase_window_check), self);
    gtk_grid_attach(GTK_GRID(grid), submit_btn, 0, 2, 3, 1);
    gtk_widget_set_can_default(submit_btn, TRUE);
    gtk_widget_grab_default(submit_btn);

    return self;
}

/*-----------------------------------------------------------------------------
Call the main GTK loop, which will open the window.
-----------------------------------------------------------------------------*/
void passphrase_window_main(passphrase_window_t *self)
{
    gtk_widget_show_all(self->window);
    gtk_main();
}

/*-----------------------------------------------------------------------------
Read the passphrase entered by the user. Check whether its hash matches the
hash stored in a file. If it matches, obtain the SHA256 of the passphrase (it
will be used as the key encryption key). Otherwise, do nothing.
-----------------------------------------------------------------------------*/
void passphrase_window_check(GtkButton *btn, passphrase_window_t *self)
{
    char const *passphrase = gtk_entry_get_text(GTK_ENTRY(self->passphrase_ent));
    char unsigned *passphrase_hash = hash_custom(passphrase);
    char unsigned *stored_hash = passphrase_hash_from_file();

    // compare
    if(memcmp(passphrase_hash, stored_hash, SHA512_DIGEST_LENGTH))
    {
        widget_toast_show(self->window, str_wrong_passphrase);
        zero_and_free(passphrase_hash, SHA512_DIGEST_LENGTH);
        zero_and_free(stored_hash, SHA512_DIGEST_LENGTH);
        return;
    }

    // set up the key encryption key
    self->kek = malloc(SHA256_DIGEST_LENGTH * sizeof *(self->kek));
    SHA256((char unsigned *)passphrase, strlen(passphrase), self->kek);
    zero_and_free(passphrase_hash, SHA512_DIGEST_LENGTH);
    zero_and_free(stored_hash, SHA512_DIGEST_LENGTH);

    passphrase_window_quit(self->window, NULL);
}

/*-----------------------------------------------------------------------------
Read the hash from the passphrase file.
-----------------------------------------------------------------------------*/
char unsigned *passphrase_hash_from_file(void)
{
    char unsigned *hash = malloc(SHA512_DIGEST_LENGTH * sizeof *hash);
    FILE *Master_file = fopen(Master, "rb");
    fread(hash, 1, SHA512_DIGEST_LENGTH, Master_file);
    fclose(Master_file);

    return hash;
}

/*-----------------------------------------------------------------------------
Write the hash to the passphrase file. This will be used while changing the
passphrase.
-----------------------------------------------------------------------------*/
void passphrase_hash_to_file(char const *passphrase)
{
    char unsigned *passphrase_hash = hash_custom(passphrase);
    FILE *Master_file = fopen(Master__, "w");
    fwrite(passphrase_hash, 1, SHA512_DIGEST_LENGTH, Master_file);
    fclose(Master_file);

    remove(Master);
    rename(Master__, Master);
}

/*-----------------------------------------------------------------------------
Automatically called when the window is closed. Quit the GTK main loop.
-----------------------------------------------------------------------------*/
void passphrase_window_quit(GtkWidget *window, gpointer data)
{
    gtk_widget_destroy(window);
    gtk_main_quit();
}

