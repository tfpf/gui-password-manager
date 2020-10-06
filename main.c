#define _GNU_SOURCE

#include <execinfo.h>
#include <gtk/gtk.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/*-----------------------------------------------------------------------------
Compile-time constant integers.
-----------------------------------------------------------------------------*/
enum { PRNG_BUF_LENGTH = 256 };
enum { BACKTRACE_ARR_SIZE = 8 };
enum { NUM_OF_HASHES = 65535 };
enum { AES_KEY_LENGTH = 32 };
enum { INIT_VEC_LENGTH = 16 };
enum { TOAST_TIMEOUT = 5 * G_TIME_SPAN_MILLISECOND };

/*-----------------------------------------------------------------------------
Compile-time constant strings. These are the images which will be used as icons
in the GUI.
-----------------------------------------------------------------------------*/
char const *const icon_main = "./include/icons/favicon.png";
char const *const icon_vis = "./include/icons/visible.png";
char const *const icon_warn = "./include/icons/warning.png";
char const *const icon_edit = "./include/icons/edit.png";
char const *const icon_del = "./include/icons/delete.png";

/*-----------------------------------------------------------------------------
Compile-time constant strings. These are strings with formatting information in
Pango (the markup language GTK understands).
-----------------------------------------------------------------------------*/
char const *const msg_passphrase = "<span weight=\"bold\">Enter your passphrase to log in.</span>";
char const *const msg_manage = "<span weight=\"normal\">Manage Passwords</span>";
char const *const msg_manage_header = "<span weight=\"bold\">Type into the search box to display matching items.</span>";
char const *const msg_manage_website = "<span weight=\"bold\">                      Website                      </span>";
char const *const msg_manage_username = "<span weight=\"bold\">                      Username                      </span>";
char const *const msg_manage_password = "<span weight=\"bold\">                      Password                      </span>";
char const *const msg_manage_error = "<span weight=\"bold\">No Matching Items</span>";
char const *const msg_add = "<span weight=\"normal\">Add New Password</span>";
char const *const msg_add_header = "<span weight=\"bold\">Fill these fields to add a new password.</span>";
char const *const msg_change = "<span weight=\"normal\">Change Passphrase</span>";
char const *const msg_change_header = "<span weight=\"bold\">Fill these fields to change the passphrase.</span>";

/*-----------------------------------------------------------------------------
Compile-time constant strings. Names of the files which contain the passphrase
and the passwords respectively.
-----------------------------------------------------------------------------*/
char const *const Master = ".Master.bin";
char const *const Slave = ".Slave.bin";

/*-----------------------------------------------------------------------------
Toggle the appearance of the characters in a GTK entry. 'Appearance' means
whether they are readable or are shown as giant dots, as if the characters are
spelling out a password.
-----------------------------------------------------------------------------*/
void toggle_visibility(GtkButton *btn, gpointer data)
{
    GtkEntry *ent = data;
    gboolean visibility = gtk_entry_get_visibility(ent);
    gtk_entry_set_visibility(ent, !visibility);
}

/*-----------------------------------------------------------------------------
Hide the tooltip.
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
Display a tooltip for a short interval, and then call a function to remove it.
This makes it look like it was a toast.
-----------------------------------------------------------------------------*/
void widget_toast_show(GtkWidget *widget, char *toast)
{
    gtk_widget_set_tooltip_text(widget, toast);
    g_timeout_add(TOAST_TIMEOUT, widget_toast_hide, widget);
}

/*-----------------------------------------------------------------------------
Segmentation fault handler. Whenever the program crashes because of a
segmentation fault, write the backtrace.
-----------------------------------------------------------------------------*/
void segfault_handler(int sig)
{
    fprintf(stderr, "\033[1;31mSignal %d received. Backtrace is as follows.\033[0m\n", sig);
    void *backtrace_array[BACKTRACE_ARR_SIZE];
    size_t size = backtrace(backtrace_array, BACKTRACE_ARR_SIZE);
    backtrace_symbols_fd(backtrace_array, size, STDERR_FILENO);
    exit(EXIT_FAILURE);
}

/*-----------------------------------------------------------------------------
Overwrite the given memory chunk with zeros. Then release the memory. Whatever
the input pointer type may have been, assume that it is volatile when it is
passed to this function. This is ensure that the zeroing operation does not get
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

/*-----------------------------------------------------------------------------
Generate an array of random bytes. It is used to generate an encryption key
and initialisation vector for AES. The length is the number of bytes, not bits.
-----------------------------------------------------------------------------*/
char unsigned *gen_rand(int len)
{
    char unsigned *arr = malloc(len * sizeof *arr);
    for(int i = 0; i < len; ++i)
    {
        arr[i] = random();
    }

    return arr;
}

/*-----------------------------------------------------------------------------
Encrypt the input array of bytes using 256-bit AES. Automatically allocate
sufficient memory to store the ciphertext.
-----------------------------------------------------------------------------*/
int encrypt_AES(char unsigned *pt, int ptlen, char unsigned *key, char unsigned *iv, char unsigned **ct)
{
    // allocate space to store ciphertext
    // to be on the safer side, allocate more space than required
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

    // all the bytes allocated to store the ciphertext may not be used
    // hence, the actual ciphertext length is being returned
    return ctlen;
}

/*-----------------------------------------------------------------------------
Decrypt the input array of bytes using 256-bit AES. Automatically allocate
sufficient memory to store the plaintext.
-----------------------------------------------------------------------------*/
int decrypt_AES(char unsigned *ct, int ctlen, char unsigned *key, char unsigned *iv, char unsigned **pt)
{
    // allocate space to store plaintext
    // to be on the safer side, allocate more space than required
    *pt = malloc(2 * ctlen * sizeof **pt);

    // decrypt
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv);
    int len;
    EVP_DecryptUpdate(ctx, *pt, &len, ct, ctlen);
    int ptlen = len;
    EVP_DecryptFinal_ex(ctx, *pt + len, &len);
    ptlen += len;
    EVP_CIPHER_CTX_free(ctx);

    // plaintext is supposed to be in printable form
    // hence, a null character must be added at the end
    (*pt)[ptlen] = '\0';

    // all the bytes allocated to store the plaintext may not be used
    // hence, the actual plaintext length is being returned
    return ptlen;
}

#include "passphrase_window.c"
#include "password_item.c"
#include "selection_window.c"

/*-----------------------------------------------------------------------------
The main function.
-----------------------------------------------------------------------------*/
int main(void)
{
    // install segmentation fault handler
    signal(SIGSEGV, segfault_handler);

    // disable output buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    // seed the random number generator
    struct timespec t;
    timespec_get(&t, TIME_UTC);
    char state_buffer[PRNG_BUF_LENGTH];
    initstate(t.tv_nsec ^ t.tv_sec, state_buffer, PRNG_BUF_LENGTH);

    gtk_init(0, NULL);

    // ask for the passphrase and get the key encryption key
    passphrase_window_t *passphrase_window = passphrase_window_new();
    passphrase_window_main(passphrase_window);
    char unsigned *kek = passphrase_window->kek;
    free(passphrase_window);
    if(kek == NULL)
    {
        return EXIT_FAILURE;
    }

    // display the main menu, where the user can choose what to do
    selection_window_t *selection_window = selection_window_new(kek);
    selection_window_main(selection_window);
    free(selection_window);

    return EXIT_SUCCESS;
}

// /*-----------------------------------------------------------------------------
// Preprocessor directives.
// -----------------------------------------------------------------------------*/
// 
// #include <ctype.h>
// #include <execinfo.h>
// #include <gtk/gtk.h>
// #include <openssl/conf.h>
// #include <openssl/err.h>
// #include <openssl/evp.h>
// #include <openssl/sha.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <strings.h>
// #include <time.h>
// #include <unistd.h>
// 
// /*-----------------------------------------------------------------------------
// Key encryption key declaration. It has to be available throughout the program,
// because it is used to encrypt the keys which are used to encrypt the passwords.
// If this is `NULL' when the login window is destroyed, the program will exit.
// -----------------------------------------------------------------------------*/
// char unsigned *kek = NULL;
// 
// /*-----------------------------------------------------------------------------
// Definition of constants.
// -----------------------------------------------------------------------------*/
// enum
// {
//     INIT_VECTOR_LENGTH =    16,
//     ENCRYPT_KEY_LENGTH =    32,
//     BACKTRACE_ARR_SIZE =    48,
//     RNGS_BUFFER_LENGTH =   256,
//     HASH_COUNT         = 65535
// };
// 
// guint const TOOLTIP_MESSAGE_TIMEOUT  =  5 * G_TIME_SPAN_MILLISECOND;
// guint const PASSWORD_DISPLAY_TIMEOUT = 10 * G_TIME_SPAN_MILLISECOND;
// 
// // the order in which the different pieces of each item are stored
// enum
// {
//     I_SITE        = 0, // website
//     I_UNAME       = 1, // username
//     I_PW          = 2, // (encrypted) password
//     I_KEY         = 3, // (encrypted) key
//     I_IV          = 4, // initialisation vector
//     PTRS_PER_ITEM = 5  // total number of above items
// };
// 
// char const *HIDDEN_PASSWORD_PLACEHOLDER = "[HIDDEN]";
// 
// char const *icon_main       = "./include/icons/favicon.png";
// char const *icon_vis        = "./include/icons/visible.png";
// char const *icon_invis      = "./include/icons/invisible.png";
// char const *icon_edit       = "./include/icons/edit.png";
// char const *icon_del        = "./include/icons/delete.png";
// char const *icon_warn       = "./include/icons/warning.png";
// 
// // where data is stored
// char const *Master = ".Master.o";
// char const *Slave  = ".Slave.o";
// 
// // where data will be temporarily stored while changing passphrase
// char const *__Master = ".Master.n";
// char const *__Slave  = ".Slave.n";
// 
// /*-----------------------------------------------------------------------------
// More preprocessor directives.
// -----------------------------------------------------------------------------*/
// #include "cipher.c"
// #include "helpers.c"
// #include "list.c"
// #include "passphrase.h"
// #include "choice.h"
// 
// /*-----------------------------------------------------------------------------
// Main function.
// -----------------------------------------------------------------------------*/
// int main(int const argc, char const *argv[])
// {
//     // install segmentation fault handler
//     signal(SIGSEGV, segmentation_fault_handler);
// 
//     // disable output buffering
//     setvbuf(stdout, NULL, _IONBF, 0);
// 
//     // seed the random number generator
//     struct timespec t;
//     timespec_get(&t, TIME_UTC);
//     char state_buffer[RNGS_BUFFER_LENGTH];
//     initstate(t.tv_nsec ^ t.tv_sec, state_buffer, RNGS_BUFFER_LENGTH);
// 
//     gtk_init(0, NULL);
// 
//     // get the passphrase
//     // to get past this point, user must enter correct passphrase
//     // otherwise, program will terminate here
//     request_passphrase();
// 
//     request_choice();
// 
//     return 0;
// }

