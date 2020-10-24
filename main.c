/*-----------------------------------------------------------------------------
General preprocessor directives.
-----------------------------------------------------------------------------*/
#include <ctype.h>
#include <gtk/gtk.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/*-----------------------------------------------------------------------------
OS-specific preprocessor directives.
-----------------------------------------------------------------------------*/
#ifdef _WIN32
    #include <memoryapi.h>
#endif

#ifdef __linux__
    #include <execinfo.h>
    #include <sys/mman.h>
    #include <signal.h>
#endif

#ifdef _WIN32
    #define SET_MEM_LOCK(ptr, size) VirtualLock(ptr, size);
    #define CLR_MEM_LOCK(ptr, size) VirtualUnlock(ptr, size);
#elif __linux__
    #define SET_MEM_LOCK(ptr, size) mlock(ptr, size);
    #define CLR_MEM_LOCK(ptr, size) munlock(ptr, size);
#endif

/*-----------------------------------------------------------------------------
Local includes.
-----------------------------------------------------------------------------*/
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
    #ifdef __linux__
    signal(SIGSEGV, segfault_handler);
    #endif

    // initialise random number generator
    if(sodium_init() == -1)
    {
        fprintf(stderr, "Failed to initialise Sodium.\n");
        return EXIT_FAILURE;
    }

    // disable output buffering
    setvbuf(stdout, NULL, _IONBF, 0);

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

