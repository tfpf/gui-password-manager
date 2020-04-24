// prototypes
void wait(int unsigned delay);
gboolean hide_tooltip(gpointer data);
char *digest_to_hexdigest(char unsigned *digest, size_t size);
char unsigned *hexdigest_to_digest(char *hexdigest, size_t size);
void clear_all_entries(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer data);
void __clear_all_entries(GtkWidget *widget, gpointer data);

/*-----------------------------------------------------------------------------
Block program execution for some amount of time. This will not be used in the
code, but is being left just for fun (and in case it is required later).
-----------------------------------------------------------------------------*/
void wait_time_delay(int unsigned delay)
{
	int unsigned end_time = time(NULL) + delay;
	while(time(NULL) < end_time);
}

/*-----------------------------------------------------------------------------
Represent a sequence of bits as a hexadecimal number. The input is an array of
bytes. Each byte in this array can be represented using two hexadecimal
symbols. Thus, the hexadecimal result is a string whose length is twice that
of the input array. This result is supposed to be in printable form. A null
character is appended at the end automatically by `sprintf'.
-----------------------------------------------------------------------------*/
char *digest_to_hexdigest(char unsigned *digest, size_t size)
{
	char *hexdigest = malloc((2 * size + 1) * sizeof *hexdigest);
	for(size_t i = 0; i < size; ++i)
	{
		sprintf(hexdigest + 2 * i, "%02x", digest[i]);
	}
	return hexdigest;
}

/*-----------------------------------------------------------------------------
Reverse whatever `digest_to_hexdigest' does. The input array is the hexadecimal
representation of some sequence of bits. Every two characters in it can be
written as a single byte. This result is not expected to be printed. Do not
append a null character at the end.
-----------------------------------------------------------------------------*/
char unsigned *hexdigest_to_digest(char *hexdigest, size_t size)
{
	// Question. Why am I allocating twice the memory required?
	// Answer. If the exact amount required is allocated, then sometimes,
	// trying to free the allocated memory fails with this error.
	// free(): invalid next size (fast)
	// an explanation may be found here
	// https://stackoverflow.com/a/44940249/6286575
	char unsigned *digest = malloc(2 * size * sizeof *digest);
	for(size_t i = 0; i < size; ++i)
	{
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wformat"
		sscanf(hexdigest + 2 * i, "%2x", digest + i);
		#pragma GCC diagnostic pop
	}
	return digest;
}

/*-----------------------------------------------------------------------------
Check if the input is a valid GTK widget. If it is, hide the tooltip (if any)
associated with the widget.
-----------------------------------------------------------------------------*/
gboolean hide_tooltip(gpointer data)
{
	GtkWidget *widget = data;
	if(GTK_IS_WIDGET(widget) == TRUE)
	{
		gtk_widget_set_has_tooltip(widget, FALSE);
		return FALSE;
	}

	return FALSE;
}

/*-----------------------------------------------------------------------------
Case-insensitive substring searching. It uses the naive substring searching
algorithm, because the text is not expected to be very long. This is no longer
needed, because I figured out how to use `strcasestr', a non-standard extension
which happens to be available on my system.
-----------------------------------------------------------------------------*/
char *strstr_ci(char const *txt, char const *pat)
{
	// corner case checks
	if(pat[0] == '\0')
	{
		return (char *)txt;
	}
	size_t patlen = strlen(pat);
	if(patlen > strnlen(txt, patlen + 256))
	{
		return NULL;
	}

	// traverse the text until the last potential match location
	for(; txt[patlen - 1] != '\0'; ++txt)
	{
		// locate first character match
		for(char *loc_txt = (char *)txt, *loc_pat = (char *)pat;; ++loc_txt, ++loc_pat)
		{
			if(*loc_pat == '\0')
			{
				return (char *)txt;
			}

			if(tolower(*loc_txt) != tolower(*loc_pat))
			{
				break;
			}
		}
	}

	return NULL;
}

/*-----------------------------------------------------------------------------
Hand over the provided GTK container to the `__clear_single_entry' function,
which will recursively clear each GTK entry which is a child (direct or
indirect) of the container.
-----------------------------------------------------------------------------*/
void clear_all_entries(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer data)
{
	GtkWidget *widget = data;
	gtk_container_foreach(GTK_CONTAINER(widget), __clear_all_entries, NULL);
}

/*-----------------------------------------------------------------------------
If the widget is an entry, clear it. If it is container, check its children
and clear those which are entries.
Question. Why did I create a separate function `__clear_all_entries'? I could
just have inserted the below code in `clear_all_entries'. That way, there need
be only one function.
Answer. `clear_all_entries' is a callback for the `switch-page' signal of a
notebook. `__clear_all_entries' is a callback for the `foreach' function. They
have different signatures. That is why I created the latter, a separate
function.
-----------------------------------------------------------------------------*/
void __clear_all_entries(GtkWidget *widget, gpointer data)
{
	if(GTK_IS_ENTRY(widget) == TRUE)
	{
		// calling `gtk_entry_buffer_set_text' after deleting text
		// this forces the entry to emit the "changed" signal
		GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(widget));
		gtk_entry_buffer_delete_text(buffer, 0, -1);
		gtk_entry_buffer_set_text(buffer, "", -1);
		return;
	}

	if(GTK_IS_CONTAINER(widget) == TRUE)
	{
		gtk_container_foreach(GTK_CONTAINER(widget), __clear_all_entries, NULL);
	}
}

/*-----------------------------------------------------------------------------
Show or hide the string written in the provided GTK entry.
-----------------------------------------------------------------------------*/
void toggle_visibility(GtkButton *button, gpointer data)
{
	GtkEntry *entry = data;

	if(gtk_entry_get_visibility(entry) == FALSE)
	{
		gtk_entry_set_visibility(entry, TRUE);
		return;
	}

	gtk_entry_set_visibility(entry, FALSE);
}

/*-----------------------------------------------------------------------------
Segmentation fault handler. Whenever the program crashes because of a
segmentation fault, write the backtrace.
-----------------------------------------------------------------------------*/
void segmentation_fault_handler(int sig)
{
	fprintf(stderr, "\033[1;31mSegmentation fault: backtrace is as follows.\033[0m\n");
	void *backtrace_array[BACKTRACE_ARR_SIZE];
	size_t size = backtrace(backtrace_array, BACKTRACE_ARR_SIZE);
	backtrace_symbols_fd(backtrace_array, size, STDERR_FILENO);
	exit(EXIT_FAILURE);
}

