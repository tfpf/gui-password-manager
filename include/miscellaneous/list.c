// this is a table to store the list of passwords in RAM
// sensitive information will be decrypted on demand
// remaining information will be held in RAM in decrypted form
int num_of_items;
typedef struct
{
	void *ptrs[5];
	int lens[5];
	gboolean valid;
}
item_t;
item_t *items;

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

// read the data from the file into RAM
// arrange it into the table
void set_list(void)
{
	num_of_items = 0;
	items = NULL;

	// count the number of lines in the password file
	// there are 5 lines for each password item
	FILE *pw_file = fopen(Slave, "rb");
	while(fscanf(pw_file, "%*[^\n]\n") != EOF)
	{
		++num_of_items;
	}
	num_of_items /= 5;
	items = malloc(num_of_items * sizeof *items);
	rewind(pw_file);

	// store the five parts of each item in the struct defined above
	for(int i = 0; i < num_of_items; ++i)
	{
		for(int j = 0; j < 5; ++j)
		{
			char *ptr = NULL;
			size_t len = 0;
			len = getline(&ptr, &len, pw_file) - 1;
			ptr[len] = '\0'; // replacing LF with NUL
			items[i].lens[j] = len / 2;
			hexdigest_to_digest((char unsigned **)&ptr, items[i].lens[j]);
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
		decrypt(items[i].ptrs[I_KEY], ENCRYPT_KEY_LENGTH, kek, iv, &key);
		decrypt(items[i].ptrs[I_SITE], items[i].lens[I_SITE], key, iv, (char unsigned **)&site);
		decrypt(items[i].ptrs[I_UNAME], items[i].lens[I_UNAME], key, iv, (char unsigned **)&uname);

		// clean up
		memset(key, 0, ENCRYPT_KEY_LENGTH);
		free(key);
		free(items[i].ptrs[I_SITE]);
		free(items[i].ptrs[I_UNAME]);
		items[i].ptrs[I_SITE] = site;
		items[i].ptrs[I_UNAME] = uname;

		// mark this item as valid
		// this will be used to populate search results
		items[i].valid = TRUE;
	}
	fclose(pw_file);
}

