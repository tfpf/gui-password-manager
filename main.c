#include <gtk/gtk.h>
#include <time.h>

char const *Master = ".Master";
char const *Slave = ".Slave";

#include "credentials.c"
#include "helpers.c"
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

