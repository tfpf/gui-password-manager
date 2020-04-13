#include <ctype.h>
#include <gtk/gtk.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// lengths in bytes
#define ENCRYPT_KEY_LENGTH 32
#define DECRYPT_KEY_LENGTH 32
#define INIT_VECTOR_LENGTH 16

#define TOOLTIP_MESSAGE_TIMEOUT   5 * G_TIME_SPAN_MILLISECOND
#define PASSWORD_DISPLAY_TIMEOUT 30 * G_TIME_SPAN_MILLISECOND

#define PTRS_PER_ITEM 5
#define I_SITE  0
#define I_UNAME 1
#define I_PW    2
#define I_KEY   3
#define I_IV    4

#define HIDDEN_PASSWORD_PLACEHOLDER "[HIDDEN]"

// files which contain the passphrase and other passwords respectively
char const *Master = "._Master";
char const *Slave = "._Slave";

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
	srandom(t.tv_nsec ^ t.tv_sec);

	gtk_init(0, NULL);

	// get the passphrase
	// to get past this point, user must enter correct passphrase
	// otherwise, program will terminate here
	request_passphrase();

	request_choice();

	return 0;
}

