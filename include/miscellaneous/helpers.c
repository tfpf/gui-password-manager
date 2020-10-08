/*-----------------------------------------------------------------------------
Function prototypes.
-----------------------------------------------------------------------------*/
char unsigned *gen_rand(int length);
char *gen_rand_constrained(int lower, int upper);
char unsigned *hash_custom(char const *passphrase);
int encrypt_AES(char unsigned *pt, int ptlen, char unsigned *key, char unsigned *iv, char unsigned **ct);
int decrypt_AES(char unsigned *ct, int ctlen, char unsigned *key, char unsigned *iv, char unsigned **pt);
void toggle_visibility(GtkButton *btn, GtkEntry *entry);
void widget_toast_show(GtkWidget *widget, char const *toast);
gboolean widget_toast_hide(gpointer data);
int request_confirmation(GtkWidget *window, char const *question, char *website, char *username);
void segfault_handler(int sig);
void zero_and_free(char volatile unsigned *data, int length);

/*-----------------------------------------------------------------------------
Generate an array of random bytes. It is used to generate an encryption key
and initialisation vector for AES. The length is the number of bytes, not bits.

`random' returns a number between 0 and `RAND_MAX'. `RAND_MAX' is guaranteed to
be at least 32767. Hence, it may be assigned to each byte, because only the
bottom 8 bits are required.
-----------------------------------------------------------------------------*/
char unsigned *gen_rand(int length)
{
    char unsigned *arr = malloc(length * sizeof *arr);
    for(int i = 0; i < length; ++i)
    {
        arr[i] = random();
    }

    return arr;
}

/*-----------------------------------------------------------------------------
Generate a random printable string. It is used to suggest strong passwords.
-----------------------------------------------------------------------------*/
char *gen_rand_constrained(int lower, int upper)
{
    int length = lower + random() % (upper - lower);
    char alphabet[] = "abcdefghijkmnpqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ123456789";
    char special[] = "!@-#?";

    char *arr = malloc((length + 1) * sizeof *arr);
    for(int i = 0; i < length; ++i)
    {
        if(i != 0 && i % 6 == 0)
        {
            arr[i] = special[random() % strlen(special)];
            continue;
        }
        arr[i] = alphabet[random() % strlen(alphabet)];
    }
    arr[length] = '\0';

    return arr;
}

/*-----------------------------------------------------------------------------
Repeatedly hash the input array of bytes. This is expected to be used on the
passphrase only.
-----------------------------------------------------------------------------*/
char unsigned *hash_custom(char const *passphrase)
{
    char unsigned *passphrase_hash = malloc(SHA512_DIGEST_LENGTH * sizeof *passphrase_hash);
    SHA512((char unsigned *)passphrase, strlen(passphrase), passphrase_hash);
    for(int i = 0; i < NUM_OF_HASHES; ++i)
    {
        SHA512(passphrase_hash, SHA512_DIGEST_LENGTH, passphrase_hash);
    }

    return passphrase_hash;
}

/*-----------------------------------------------------------------------------
Encrypt the input array of bytes using 256-bit AES. Allocate sufficient memory
to store the ciphertext. Return the actual length of the ciphertext.
-----------------------------------------------------------------------------*/
int encrypt_AES(char unsigned *pt, int ptlen, char unsigned *key, char unsigned *iv, char unsigned **ct)
{
    *ct = malloc(2 * ptlen * sizeof **ct);

    // encrypt
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv);
    int len;
    EVP_EncryptUpdate(ctx, *ct, &len, pt, ptlen);
    int ctlen = len;
    EVP_EncryptFinal_ex(ctx, *ct + len, &len);
    ctlen += len;
    EVP_CIPHER_CTX_free(ctx);

    return ctlen;
}

/*-----------------------------------------------------------------------------
Decrypt the input array of bytes using 256-bit AES. Allocate sufficient memory
to store the plaintext. Delimit it with a null byte, since it is expected to be
a printable string. Return the length of the plaintext, which (ideally) should
be the same as the result of calling `strlen' on the plaintext.
-----------------------------------------------------------------------------*/
int decrypt_AES(char unsigned *ct, int ctlen, char unsigned *key, char unsigned *iv, char unsigned **pt)
{
    *pt = malloc(2 * ctlen * sizeof **pt);

    // decrypt
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv);
    int len;
    EVP_DecryptUpdate(ctx, *pt, &len, ct, ctlen);
    int ptlen = len;
    EVP_DecryptFinal_ex(ctx, *pt + len, &len);
    (*pt)[ptlen] = '\0';
    ptlen += len;
    EVP_CIPHER_CTX_free(ctx);

    return ptlen;
}

/*-----------------------------------------------------------------------------
Toggle the appearance of the characters in a GTK entry. 'Appearance' means
whether they are readable or are shown as dots, just like passwords are shown.
-----------------------------------------------------------------------------*/
void toggle_visibility(GtkButton *btn, GtkEntry *entry)
{
    gboolean visibility = gtk_entry_get_visibility(entry);
    gtk_entry_set_visibility(entry, !visibility);
}

/*-----------------------------------------------------------------------------
Display a tooltip. Schedule a function to hide the tooltip after some time.
This makes it look like it was a toast.
-----------------------------------------------------------------------------*/
void widget_toast_show(GtkWidget *widget, char const *toast)
{
    gtk_widget_set_tooltip_text(widget, toast);
    g_timeout_add(TOAST_TIMEOUT, widget_toast_hide, widget);
}

/*-----------------------------------------------------------------------------
Hide the tooltip of a widget. If the widget has no tooltip, or if the widget is
no more (perhaps because its parent was destroyed), this has no effect.
-----------------------------------------------------------------------------*/
gboolean widget_toast_hide(gpointer data)
{
    GtkWidget *widget = data;
    if(GTK_IS_WIDGET(widget))
    {
        gtk_widget_set_has_tooltip(widget, FALSE);
    }

    return FALSE;
}

/*-----------------------------------------------------------------------------
Display a box asking for confirmation on whether the user wants to proceed with
some action. If the last two arguments are not null pointers, display them as
the website and username.
-----------------------------------------------------------------------------*/
int request_confirmation(GtkWidget *window, char const *question, char *website, char *username)
{
    // dialogue box
    GtkWidget *dialogue = gtk_dialog_new();
    gtk_container_set_border_width(GTK_CONTAINER(dialogue), 0);
    gtk_dialog_set_default_response(GTK_DIALOG(dialogue), GTK_RESPONSE_REJECT);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialogue), TRUE);
    gtk_window_set_icon_from_file(GTK_WINDOW(dialogue), icon_main, NULL);
    gtk_window_set_modal(GTK_WINDOW(dialogue), TRUE);
    gtk_window_set_position(GTK_WINDOW(dialogue), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(dialogue), FALSE);
    gtk_window_set_title(GTK_WINDOW(dialogue), str_confirmation);
    gtk_window_set_transient_for(GTK_WINDOW(dialogue), GTK_WINDOW(window));

    // grid
    GtkWidget *grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(grid), 25);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 15);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 15);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialogue))), grid);

    // warning icon
    GtkWidget *image = gtk_image_new_from_file(icon_warn);
    gtk_grid_attach(GTK_GRID(grid), image, 0, 0, 1, 1);

    // header label
    GtkWidget *header = gtk_label_new(question);
    gtk_grid_attach(GTK_GRID(grid), header, 1, 0, 2, 1);

    if(website != NULL && username != NULL)
    {
        // website prompt label
        GtkWidget *website1_lbl = gtk_label_new(str_website);
        gtk_grid_attach(GTK_GRID(grid), website1_lbl, 1, 1, 1, 1);

        // website response label
        GtkWidget *website2_lbl = gtk_label_new(website);
        gtk_grid_attach(GTK_GRID(grid), website2_lbl, 2, 1, 1, 1);

        // username prompt label
        GtkWidget *username1_lbl = gtk_label_new(str_username);
        gtk_grid_attach(GTK_GRID(grid), username1_lbl, 1, 2, 1, 1);

        // username response label
        GtkWidget *username2_lbl = gtk_label_new(username);
        gtk_grid_attach(GTK_GRID(grid), username2_lbl, 2, 2, 1, 1);
    }

    // buttons
    gtk_dialog_add_button(GTK_DIALOG(dialogue), str_reject, GTK_RESPONSE_REJECT);
    gtk_dialog_add_button(GTK_DIALOG(dialogue), str_accept, GTK_RESPONSE_ACCEPT);

    gtk_widget_show_all(dialogue);
    gtk_widget_grab_focus(dialogue);
    int response = gtk_dialog_run(GTK_DIALOG(dialogue));
    gtk_widget_destroy(dialogue);

    return response;
}

/*-----------------------------------------------------------------------------
Segmentation fault handler. Whenever the program crashes because of a
segmentation fault, write the backtrace.
-----------------------------------------------------------------------------*/
void segfault_handler(int sig)
{
    fprintf(stderr, "\033[1;31msignal %d; backtrace:\033[0m\n", sig);
    void *backtrace_array[BACKTRACE_ARR_SIZE];
    size_t size = backtrace(backtrace_array, BACKTRACE_ARR_SIZE);
    backtrace_symbols_fd(backtrace_array, size, STDERR_FILENO);
    exit(EXIT_FAILURE);
}

/*-----------------------------------------------------------------------------
Overwrite the given memory chunk with zeros. Then release the memory. Whatever
the input pointer type may have been, assume that it is volatile when it is
passed to this function. This ensures that the zeroing operation does not get
removed because of compiler optimisation.

It is not possible to replace this loop with a call to the `memset' function,
because `memset' discards the `volatile' qualifier.

The final cast to a pointer of type `void' ensures that the compiler does not
give a warning about `free' discarding the `volatile' qualifier.
-----------------------------------------------------------------------------*/
void zero_and_free(char volatile unsigned *data, int length)
{
    for(int i = 0; i < length; ++i)
    {
        data[i] = '\0';
    }
    free((void *)data);
}

