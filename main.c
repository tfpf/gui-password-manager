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

#include "global_constants.c"
#include "helpers.c"
#include "notification_revealer.c"
#include "passphrase_window.c"
#include "password_item.c"
#include "selection_window.c"

/*-----------------------------------------------------------------------------
Main function.
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

    // `kek' will be a null pointer if the correct passphrase was not entered
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

