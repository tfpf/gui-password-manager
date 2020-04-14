// prototypes
int comparator(void const *e1, void const *e2);
void see_list(void);
void set_list(void);

/*-----------------------------------------------------------------------------
This is a global variable which will store the password items in RAM. The
website and username will be decrypted, but the password will be stored under
a layer of encryption. Instead of creating five members (two character pointers
and three unsigned character pointers), an array of five void pointers is
created for convenience. The lengths of these character arrays is stored in
another array.
-----------------------------------------------------------------------------*/
int num_of_items;
typedef struct
{
	void *ptrs[5];
	int lens[5];
} items_t;
items_t *items;

/*-----------------------------------------------------------------------------
Display the username and website. This is only for debugging. It is not
supposed to be used in the final code.
-----------------------------------------------------------------------------*/
void see_list(void)
{
	for(int i = 0; i < num_of_items; ++i)
	{
		printf("%3d: ", i);
		for(int j = 0; j < 2; ++j)
		{
			printf("%s,", (char *)(items[i].ptrs[j]));
		}
		printf("\n");
	}
}

/*-----------------------------------------------------------------------------
Read the data stored in the user's file and store it into RAM. Then decrypt
only the website and username, leaving everything else as it is.
-----------------------------------------------------------------------------*/
void set_list(void)
{
	// count the number of lines in the password file
	// there are `PTRS_PER_ITEM' lines for each password item
	num_of_items = 0;
	FILE *pw_file = fopen(Slave, "rb");
	while(fscanf(pw_file, "%*s") != EOF)
	{
		++num_of_items;
	}
	num_of_items /= PTRS_PER_ITEM;
	items = malloc(num_of_items * sizeof *items);
	rewind(pw_file);

	// store the five parts of each item in the struct defined above
	for(int i = 0; i < num_of_items; ++i)
	{
		for(int j = 0; j < PTRS_PER_ITEM; ++j)
		{
			char *ptr_hex = NULL;
			size_t len = 0;
			len = getline(&ptr_hex, &len, pw_file) - 1;
			ptr_hex[len] = '\0'; // replacing LF with NUL
			items[i].lens[j] = len / 2;
			char unsigned *ptr = hexdigest_to_digest(ptr_hex, items[i].lens[j]);
			free(ptr_hex);
			items[i].ptrs[j] = ptr;
		}

		// decrypt and store website and username
		// initialisation vector was not encrypted--store as is
		// store encrypted password and encrypted key
		char unsigned *kek = get_credentials_kek();
		char *site, *uname; // store as plaintext
		char unsigned *key; // do not store
		char unsigned *iv = items[i].ptrs[I_IV];

		// to get `site' and `uname', `key' has to be obtained
		// I have its encrypted version, so decrypt it first
		decrypt(items[i].ptrs[I_KEY],   ENCRYPT_KEY_LENGTH,     kek, iv, &key);
		decrypt(items[i].ptrs[I_SITE],  items[i].lens[I_SITE],  key, iv, (char unsigned **)&site);
		decrypt(items[i].ptrs[I_UNAME], items[i].lens[I_UNAME], key, iv, (char unsigned **)&uname);

		// clear RAM
		memset(key,                    0, ENCRYPT_KEY_LENGTH);
		memset(items[i].ptrs[I_SITE],  0, items[i].lens[I_SITE]);
		memset(items[i].ptrs[I_UNAME], 0, items[i].lens[I_UNAME]);

		// deallocate
		free(key);
		free(items[i].ptrs[I_SITE]);
		free(items[i].ptrs[I_UNAME]);

		items[i].ptrs[I_SITE]  = site;
		items[i].lens[I_SITE]  = strlen(site);
		items[i].ptrs[I_UNAME] = uname;
		items[i].lens[I_UNAME] = strlen(uname);
	}
	fclose(pw_file);
	qsort(items, num_of_items, sizeof *items, comparator);
}

/*-----------------------------------------------------------------------------
A comparator function to be used with `qsort'. It defines the relative order of
two elements in the `items' array.
-----------------------------------------------------------------------------*/
int comparator(void const *e1, void const *e2)
{
	// obtain the strings which have to be compared
	items_t const *p1 = e1;
	items_t const *p2 = e2;

	// cannot use normal `strcmp' here
	// it will result in sort order as: A, B, C, ..., Z, a, b, c, ...
	int result = strcasecmp(p1->ptrs[I_SITE], p2->ptrs[I_SITE]);
	if(result == 0)
	{
		result = strcasecmp(p1->ptrs[I_UNAME], p2->ptrs[I_UNAME]);
	}

	if(result < 0)
	{
		return -1;
	}

	if(result > 0)
	{
		return 1;
	}

	return result;
}

