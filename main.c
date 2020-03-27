#include "credentials.c"
#include "helpers.c"
#include "passphrase.c"

// gboolean exit_status = TRUE;

///////////////////////////////////////////////////////////////////////////////

int main(int const argc, char const *argv[])
{
	// setvbuf(stdout, NULL, _IONBF, 0);

	// must be called before any other GTK functions
	// normally, arguments are pointers to `argc' and `argv'
	gtk_init(0, NULL);

	request_passphrase();

	return 0;
}

