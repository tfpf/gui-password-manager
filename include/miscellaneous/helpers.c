/*-----------------------------------------------------------------------------
Function prototypes.
-----------------------------------------------------------------------------*/
char unsigned *gen_rand(int length);
char *gen_rand_constrained(int lower, int upper);
char unsigned *hash_custom(char const *passphrase);
int encrypt_AES(char unsigned *pt, int ptlen, char unsigned *key, char unsigned *iv, char unsigned **ct);
int decrypt_AES(char unsigned *ct, int ctlen, char unsigned *key, char unsigned *iv, char unsigned **pt);
void toggle_visibility(GtkButton *btn, GtkEntry *entry);
int request_confirmation(GtkWidget *window, char const *question, char *website, char *username);
void segfault_handler(int sig);
char *my_strcasestr(char const *txt, char const *pat);
char *my_strdup(char const *string);
char unsigned *my_fread(int length, FILE *Slave_file);
void zero_and_free(char volatile unsigned *data, int length);

/*-----------------------------------------------------------------------------
Generate an array of random bytes. It is used to generate an encryption key
and initialisation vector for AES. The length is the number of bytes, not bits.
-----------------------------------------------------------------------------*/
char unsigned *gen_rand(int length)
{
    char unsigned *arr = malloc(length * sizeof *arr);
    SET_MEM_LOCK(arr, length * sizeof *arr)
    randombytes_buf(arr, length);

    return arr;
}

/*-----------------------------------------------------------------------------
Generate a random printable string. It is used to suggest strong passwords.
-----------------------------------------------------------------------------*/
char *gen_rand_constrained(int lower, int upper)
{
    int length = lower + randombytes_uniform(upper - lower);
    char alphabet[] = "abcdefghijkmnpqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ123456789";
    char special[] = "!@-#?";

    char *arr = malloc((length + 1) * sizeof *arr);
    for(int i = 0; i < length; ++i)
    {
        if(i != 0 && i % 6 == 0)
        {
            arr[i] = special[randombytes_uniform(strlen(special))];
            continue;
        }
        arr[i] = alphabet[randombytes_uniform(strlen(alphabet))];
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
    SET_MEM_LOCK(*ct, 2 * ptlen * sizeof **ct)

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
    SET_MEM_LOCK(*pt, 2 * ctlen * sizeof **pt)

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

#ifdef __linux__
/*-----------------------------------------------------------------------------
Segmentation fault handler. Whenever the program crashes because of a
segmentation fault, write the backtrace. This functionality is available only
on Linux.
-----------------------------------------------------------------------------*/
void segfault_handler(int sig)
{
    fprintf(stderr, "***** signal %d; backtrace as follows *****\n", sig);
    void *backtrace_array[BACKTRACE_ARR_SIZE];
    size_t size = backtrace(backtrace_array, BACKTRACE_ARR_SIZE);
    backtrace_symbols_fd(backtrace_array, size, STDERR_FILENO);
    exit(EXIT_FAILURE);
}
#endif

/*-----------------------------------------------------------------------------
Case-insensitive substring search. The non-standard extension `strcasestr'
is not available on Windows, but `strncasecmp' is.
-----------------------------------------------------------------------------*/
char *my_strcasestr(char const *txt, char const *pat)
{
    // sanity 1
    if(pat[0] == '\0')
    {
        return (char *)txt;
    }

    // sanity 2
    size_t patlen = strlen(pat);
    if(patlen > strnlen(txt, patlen + 256))
    {
        return NULL;
    }

    // search
    for(char *loc = (char *)txt; loc[patlen - 1] != '\0'; ++loc)
    {
        if(!strncasecmp(pat, loc, patlen))
        {
            return loc;
        }
    }

    return NULL;
}

/*-----------------------------------------------------------------------------
Duplicate a string. Mark the memory used by it as non-swappable.
-----------------------------------------------------------------------------*/
char *my_strdup(char const *string)
{
    char *duplicate = strdup(string);
    SET_MEM_LOCK(duplicate, strlen(duplicate))
    return duplicate;
}

/*-----------------------------------------------------------------------------
Read encrypted data from a file. Mark the memory used by the buffer as
non-swappable.
-----------------------------------------------------------------------------*/
char unsigned *my_fread(int length, FILE *Slave_file)
{
    char unsigned *buffer = malloc(length * sizeof *buffer);
    SET_MEM_LOCK(buffer, length * sizeof *buffer)
    fread(buffer, 1, length, Slave_file);
    return buffer;
}

/*-----------------------------------------------------------------------------
Overwrite the given memory chunk with zeros. Then release the memory. Whatever
the input pointer type may have been, assume that it is volatile when it is
passed to this function. This ensures that the zeroing operation does not get
removed because of compiler optimisation.

It is not possible to replace this loop with a call to the `memset' function,
because `memset' discards the `volatile' qualifier.

The final cast to a pointer of type `void' ensures that the compiler does not
give a warning about discarding the `volatile' qualifier.
-----------------------------------------------------------------------------*/
void zero_and_free(char volatile unsigned *data, int length)
{
    for(char volatile unsigned *p = data; length != 0; --length, ++p)
    {
        *p = '\0';
    }
    // CLR_MEM_LOCK((void *)data, length);
    free((void *)data);
}

