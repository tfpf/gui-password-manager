/*-----------------------------------------------------------------------------
Preprocessor directives.
-----------------------------------------------------------------------------*/
#define _GNU_SOURCE

#include <ctype.h>
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


/*-----------------------------------------------------------------------------
Definition of constants.
-----------------------------------------------------------------------------*/
enum {HASH_COUNT = 65535};

enum
{
	ENCRYPT_KEY_LENGTH =  32,
	INIT_VECTOR_LENGTH =  16,
	RNGS_BUFFER_LENGTH = 256
};

guint const TOOLTIP_MESSAGE_TIMEOUT  =  5 * G_TIME_SPAN_MILLISECOND;
guint const PASSWORD_DISPLAY_TIMEOUT = 30 * G_TIME_SPAN_MILLISECOND;

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

// where data is stored
char const *Master = ".Master.o";
char const *Slave  = ".Slave.o";

// where data will be temporarily stored while changing passphrase
char const *__Master = ".Master.n";
char const *__Slave  = ".Slave.n";

/*-----------------------------------------------------------------------------
More preprocessor directives.
-----------------------------------------------------------------------------*/
#include "credentials.h"
#include "helpers.c"
#include "cipher.c"
#include "list.c"
#include "passphrase.h"
#include "choice.h"

/*-----------------------------------------------------------------------------
Main function.
-----------------------------------------------------------------------------*/
int main(int const argc, char const *argv[])
{
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

