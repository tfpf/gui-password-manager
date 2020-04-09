// prototypes
void wait(int unsigned delay);
gboolean hide_tooltip(gpointer data);
void digest_to_hexdigest(char unsigned **p, size_t size);
void hexdigest_to_digest(char unsigned **p, size_t size);

/*-----------------------------------------------------------------------------
Block program execution for aome amount of time. This will not be used in the
code, but is being left just for fun (and in case it is required later).
-----------------------------------------------------------------------------*/
void wait(int unsigned delay)
{
	int unsigned end_time = time(NULL) + delay;
	while(time(NULL) < end_time);
}

/*-----------------------------------------------------------------------------
Represent a sequence of bits as a hexadecimal number. The input is an array of
bytes. Each byte in this array can be represented using two hexadecimal
symbols. Thus, the hexadecimal result is a string whose length is twice that
of the input array. This result is supposed to be in printable form, so a null
character is appended at the end. Clear the bytes of the input array to zeros.
-----------------------------------------------------------------------------*/
void digest_to_hexdigest(char unsigned **p, size_t size)
{
	char unsigned *digest    = *p;
	char unsigned *hexdigest = malloc((2 * size + 1) * sizeof *hexdigest);
	for(size_t i = 0; i < size; ++i)
	{
		sprintf((char *)hexdigest + 2 * i, "%02x", digest[i]);
	}
	memset(digest, 0, size);
	free(digest);
	*p = hexdigest;
}

/*-----------------------------------------------------------------------------
Reverse whatever `digest_to_hexdigest' does. The input array is the hexadecimal
representation of some sequence of bits. Every two characters in it can be
written as a single byte. This result is not expected to be printed. Do not
append a null character at the end. Clear the bytes of the input array to
zeros.
-----------------------------------------------------------------------------*/
void hexdigest_to_digest(char unsigned **p, size_t size)
{
	char unsigned *hexdigest = *p;
	char unsigned *digest    = malloc(size * sizeof *digest);
	for(size_t i = 0; i < size; ++i)
	{
		#pragma GCC diagnostic ignored "-Wformat"
		sscanf((char *)hexdigest + 2 * i, "%2x", digest + i);
		#pragma GCC diagnostic error "-Wformat"
	}
	memset(hexdigest, 0, 2 * size + 1);
	free(hexdigest);
	*p = digest;
}

/*-----------------------------------------------------------------------------
Hide the tooltip (if any) associated with the input GTK widget.
-----------------------------------------------------------------------------*/
gboolean hide_tooltip(gpointer data)
{
	GtkWidget *window = data;
	gtk_widget_set_has_tooltip(window, FALSE);
	return G_SOURCE_REMOVE;
}

