#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////

// prototypes
void wait(int unsigned delay);
gboolean hide_tooltip(gpointer data);
void digest_to_hexdigest(char unsigned **p, size_t size);

///////////////////////////////////////////////////////////////////////////////

// block the program for some amount of time
void wait(int unsigned delay)
{
	int unsigned end_time = time(NULL) + delay;
	while(time(NULL) < end_time);
}

///////////////////////////////////////////////////////////////////////////////

// input array is a digest (possibly a hash value) of `size' bytes
// each byte in it can be represented using two hexadecimal characters
// hence, the corresponding hexdigest will be `2 * size + 1' characters long
// the extra character is the null character at the end
void digest_to_hexdigest(char unsigned **p, size_t size)
{
	char unsigned *digest = *p;
	char unsigned *hexdigest = malloc((2 * size + 1) * sizeof *hexdigest);

	// for each byte in `digest', write two bytes in `hexdigest'
	for(size_t i = 0; i < size; ++i)
	{
		sprintf((char *)hexdigest + 2 * i, "%02x", digest[i]);
	}

	// make the argument point to `hexdigest'
	memset(digest, 0, size);
	*p = hexdigest;
}

///////////////////////////////////////////////////////////////////////////////

// hide the tooltip, if the widget has any
gboolean hide_tooltip(gpointer data)
{
	GtkWidget *window = data;
	gtk_widget_set_has_tooltip(window, FALSE);
	return G_SOURCE_REMOVE;
}

