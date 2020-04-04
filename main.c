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
// decryption key is same as encryption key, hence length is same
#define ENCRYPT_KEY_LENGTH 32
#define DECRYPT_KEY_LENGTH 32
#define INIT_VECTOR_LENGTH 16

// time for which a tooltip message is visible
#define TOOLTIP_MESSAGE_TIMEOUT 8 * G_TIME_SPAN_MILLISECOND

// character which separates columns in password file
#define COLUMN_SEPARATOR 0x1b

// files which contain the passphrase and other passwords respectively
char const *Master = ".Master";
char const *Slave = ".Slave";

#include "credentials.c"
#include "helpers.c"
#include "list.c"
#include "passphrase.c"
#include "choice.c"

///////////////////////////////////////////////////////////////////////////////

int main(int const argc, char const *argv[])
{
	// disable output buffering
	setvbuf(stdout, NULL, _IONBF, 0);

	// seed the random number generator
	struct timespec *t = malloc(sizeof *t);
	timespec_get(t, TIME_UTC);
	srandom(t->tv_nsec ^ t->tv_sec);
	free(t);

	// must be called before any other GTK functions
	// normally, arguments are pointers to `argc' and `argv'
	gtk_init(0, NULL);

	// get the passphrase
	// to get past this point, user must enter correct passphrase
	// otherwise, program will terminate here
	request_passphrase();

	// ask what the user wants to do
	request_choice();

	return 0;
}

