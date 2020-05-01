/*-----------------------------------------------------------------------------
Preprocessor directives.
-----------------------------------------------------------------------------*/
#define _GNU_SOURCE

#include <ctype.h>
#include <execinfo.h>
#include <gtk/gtk.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

/*-----------------------------------------------------------------------------
Key encryption key declaration. It has to be available throughout the program,
because it is used to encrypt the keys which are used to encrypt the passwords.
If this is `NULL' when the login window is destroyed, the program will exit.
-----------------------------------------------------------------------------*/
char unsigned *kek = NULL;

/*-----------------------------------------------------------------------------
Definition of constants.
-----------------------------------------------------------------------------*/
enum
{
	INIT_VECTOR_LENGTH =    16,
	ENCRYPT_KEY_LENGTH =    32,
	BACKTRACE_ARR_SIZE =    48,
	RNGS_BUFFER_LENGTH =   256,
	HASH_COUNT         = 65535
};

guint const TOOLTIP_MESSAGE_TIMEOUT  =  5 * G_TIME_SPAN_MILLISECOND;
guint const PASSWORD_DISPLAY_TIMEOUT = 10 * G_TIME_SPAN_MILLISECOND;

// the order in which the different pieces of each item are stored
enum
{
	I_SITE        = 0, // website
	I_UNAME       = 1, // username
	I_PW          = 2, // (encrypted) password
	I_KEY         = 3, // (encrypted) key
	I_IV          = 4, // initialisation vector
	PTRS_PER_ITEM = 5  // total number of above items
};

char const *HIDDEN_PASSWORD_PLACEHOLDER = "[HIDDEN]";

char const *icon_main       = "./include/icons/favicon.png";
char const *icon_vis        = "./include/icons/visible.png";
char const *icon_invis      = "./include/icons/invisible.png";
char const *icon_edit       = "./include/icons/edit.png";
char const *icon_del        = "./include/icons/delete.png";
char const *icon_warn       = "./include/icons/warning.png";

// where data is stored
char const *Master = ".Master.o";
char const *Slave  = ".Slave.o";

// where data will be temporarily stored while changing passphrase
char const *__Master = ".Master.n";
char const *__Slave  = ".Slave.n";

/*-----------------------------------------------------------------------------
More preprocessor directives.
-----------------------------------------------------------------------------*/
#include "cipher.c"
#include "helpers.c"
#include "list.c"
#include "passphrase.h"
#include "choice.h"

/*-----------------------------------------------------------------------------
Main function.
-----------------------------------------------------------------------------*/
int main(int const argc, char const *argv[])
{
	// install segmentation fault handler
	signal(SIGSEGV, segmentation_fault_handler);

	// disable output buffering
	setvbuf(stdout, NULL, _IONBF, 0);

	// seed the random number generator
	struct timespec t;
	timespec_get(&t, TIME_UTC);
	char state_buffer[RNGS_BUFFER_LENGTH];
	initstate(t.tv_nsec ^ t.tv_sec, state_buffer, RNGS_BUFFER_LENGTH);

	gtk_init(0, NULL);

	// get the passphrase
	// to get past this point, user must enter correct passphrase
	// otherwise, program will terminate here
	request_passphrase();

	request_choice();

	return 0;
}

