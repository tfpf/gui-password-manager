#include <execinfo.h>
#include <gtk/gtk.h>
#include <openssl/sha.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*-----------------------------------------------------------------------------
Compile-time constant integers.
-----------------------------------------------------------------------------*/
enum { NOISE_GEN_BUF_LENGTH = 256 };
enum { BACKTRACE_ARR_SIZE = 8 };
enum { NUM_OF_HASHES = 65535 };
guint const TOAST_TIMEOUT = 5 * G_TIME_SPAN_MILLISECOND;

/*-----------------------------------------------------------------------------
Compile-time constant strings. These are the images which will be used as icons
in the GUI.
-----------------------------------------------------------------------------*/
char const *const icon_main = "./include/icons/favicon.png";
char const *const icon_vis = "./include/icons/visible.png";

/*-----------------------------------------------------------------------------
Compile-time constant strings. These are strings with formatting information in
Pango (the markup language GTK understands).
-----------------------------------------------------------------------------*/
char const *const msg_passphrase = "<span weight=\"bold\" foreground=\"green\">Enter your passphrase to log in.</span>";

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
    fprintf(stderr, "\033[1;31m\t%d segfault backtrace:\033[0m\n", sig);
    void *backtrace_array[BACKTRACE_ARR_SIZE];
    size_t size = backtrace(backtrace_array, BACKTRACE_ARR_SIZE);
    backtrace_symbols_fd(backtrace_array, size, STDERR_FILENO);
    exit(EXIT_FAILURE);
}

/*-----------------------------------------------------------------------------
Overwrite the given memory chunk with zeros. Then release the memory.
-----------------------------------------------------------------------------*/
void zero_and_free(char unsigned *data, int length)
{
    for(int i = 0; i < length; ++i)
    {
        data[i] = '\0';
    }
    free(data);
}

#include "passphrase_window.c"

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
    char state_buffer[NOISE_GEN_BUF_LENGTH];
    initstate(t.tv_nsec ^ t.tv_sec, state_buffer, NOISE_GEN_BUF_LENGTH);

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

    return EXIT_SUCCESS;
}

// /*-----------------------------------------------------------------------------
// Preprocessor directives.
// -----------------------------------------------------------------------------*/
// #define _GNU_SOURCE
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
// 	INIT_VECTOR_LENGTH =    16,
// 	ENCRYPT_KEY_LENGTH =    32,
// 	BACKTRACE_ARR_SIZE =    48,
// 	RNGS_BUFFER_LENGTH =   256,
// 	HASH_COUNT         = 65535
// };
// 
// guint const TOOLTIP_MESSAGE_TIMEOUT  =  5 * G_TIME_SPAN_MILLISECOND;
// guint const PASSWORD_DISPLAY_TIMEOUT = 10 * G_TIME_SPAN_MILLISECOND;
// 
// // the order in which the different pieces of each item are stored
// enum
// {
// 	I_SITE        = 0, // website
// 	I_UNAME       = 1, // username
// 	I_PW          = 2, // (encrypted) password
// 	I_KEY         = 3, // (encrypted) key
// 	I_IV          = 4, // initialisation vector
// 	PTRS_PER_ITEM = 5  // total number of above items
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
// 	// install segmentation fault handler
// 	signal(SIGSEGV, segmentation_fault_handler);
// 
// 	// disable output buffering
// 	setvbuf(stdout, NULL, _IONBF, 0);
// 
// 	// seed the random number generator
// 	struct timespec t;
// 	timespec_get(&t, TIME_UTC);
// 	char state_buffer[RNGS_BUFFER_LENGTH];
// 	initstate(t.tv_nsec ^ t.tv_sec, state_buffer, RNGS_BUFFER_LENGTH);
// 
// 	gtk_init(0, NULL);
// 
// 	// get the passphrase
// 	// to get past this point, user must enter correct passphrase
// 	// otherwise, program will terminate here
// 	request_passphrase();
// 
// 	request_choice();
// 
// 	return 0;
// }

