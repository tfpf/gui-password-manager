// prototypes
void wait(int unsigned delay);
gboolean hide_tooltip(gpointer data);
char *digest_to_hexdigest(char unsigned *digest, size_t size);
char unsigned *hexdigest_to_digest(char *hexdigest, size_t size);

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
character is appended at the end.
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
	// important note: why am I allocating twice the memory required
	// sometimes, calling `free' on the returned pointer causes this error:
	// free(): invalid next size (fast)
	// an explanation may be found here
	// https://stackoverflow.com/a/44940249/6286575
	char unsigned *digest = malloc(2 * size * sizeof *digest);
	for(size_t i = 0; i < size; ++i)
	{
		#pragma GCC diagnostic ignored "-Wformat"
		sscanf(hexdigest + 2 * i, "%2x", digest + i);
		#pragma GCC diagnostic error "-Wformat"
	}
	return digest;
}

/*-----------------------------------------------------------------------------
Check if the input is a valid GTK widget. If it is, hide the tooltip (if any)
associated with the widget.
-----------------------------------------------------------------------------*/
gboolean hide_tooltip(gpointer data)
{
	GtkWidget *window = data;
	if(GTK_IS_WIDGET(window) == FALSE)
	{
		return FALSE;
	}

	gtk_widget_set_has_tooltip(window, FALSE);
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
Clear the provided GTK entry. If the provided widget is not an entry,
recursively clear all those children which are entries. This still needs a lot
of work.
-----------------------------------------------------------------------------*/
void clear_all_entries(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer data)
{
	GtkWidget *widget = data;
	return;
	printf("%p,", widget);
	printf("%s\n", G_OBJECT_TYPE_NAME(widget));

	// if(widget == NULL) printf("is null\n");
	// if(GTK_IS_WIDGET(widget)) printf("%s\n", G_OBJECT_TYPE_NAME(widget));

	// if(GTK_IS_ENTRY(widget) == TRUE)
	// {
	// 	GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(widget));
	// 	gtk_entry_buffer_delete_text(buffer, 0, -1);
	// 	return;
	// }

	if(GTK_IS_CONTAINER(widget) == FALSE)
	{
		return;
	}

	// if this widget is not an entry, recursively check its children
	GList *children = gtk_container_get_children(GTK_CONTAINER(widget));
	for(GList *child = children; child != NULL; child = g_list_next(child))
	{
		clear_all_entries(NULL, NULL, 0, child);
	}
}

