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
		for(int j = 0; j < 5; ++j)
		{
			printf("%d:%s,", items[i].lens[j], (char *)(items[i].ptrs[j]));
		}
		printf("\n");
	}
}

///////////////////////////////////////////////////////////////////////////////

void fix_list(item_t *item)
{

	// // initialisation vector is in hexdigest form
	// // convert it to digest form
	// char unsigned *iv = malloc((2 * INIT_VECTOR_LENGTH + 1) * sizeof *iv);
	// strcpy((char *)iv, (char *)(item->iv));
	// hexdigest_to_digest(&iv, INIT_VECTOR_LENGTH);
	// memset(item->iv, 0, 2 * INIT_VECTOR_LENGTH);
	// item->iv = iv;

	// // repeat for encrypted key
	// char unsigned *e_key = malloc((2 * ENCRYPT_KEY_LENGTH + 1) * sizeof *e_key);
	// strcpy((char *)e_key, (char *)(item->e_key));
	// hexdigest_to_digest(&e_key, ENCRYPT_KEY_LENGTH);
	// item->e_key = e_key;

	// // repeat for encrypted password
	// char unsigned *e_pw = malloc((strlen(item->e_pw) + 1) * sizeof *e_pw);
	// strcpy((char *)e_pw, (char *)(item->e_pw));
	// hexdigest_to_digest(&e_pw, strlen(item->e_pw) / 2);
	// item->e_pw = e_pw;

	// // temporarily decrypt the key
	// // use it to decrypt the website and username
	// char unsigned *key;
	// char unsigned *kek = get_credentials_kek();
	// decrypt(e_key, ENCRYPT_KEY_LENGTH, kek, iv, &key);

	// int e_sitelen = strlen(item->site);
	// char unsigned *e_site = malloc((e_sitelen + 1) * sizeof *e_site);
	// strcpy((char *)e_site, (char *)(item->site));
	// hexdigest_to_digest(&e_site, e_sitelen / 2);
	// char *site;
	// decrypt(e_site, e_sitelen / 2, key, iv, &site);
	// printf("site: %s\n", site);

}

///////////////////////////////////////////////////////////////////////////////

// read the data from the file into RAM
// arrange it into the table
void set_list(void)
{
	num_of_items = 0;
	items = NULL;

	// count the number of lines in the password file
	FILE *pw_file = fopen(Slave, "rb");
	while(fscanf(pw_file, "%*[^\n]\n") != EOF)
	{
		++num_of_items;
	}
	num_of_items /= 5;
	items = malloc(num_of_items * sizeof *items);
	rewind(pw_file);

	for(int i = 0; i < num_of_items; ++i)
	{
		// load five lines into RAM
		// these are the five parts of the data
		// char unsigned *e_site, *e_uname, *e_pw, *e_key, *iv;
		// size_t e_sitelen, e_unamelen, e_pwlen;
		// e_site = e_uname = e_pw = e_key = iv = NULL;
		// e_sitelen = e_unamelen = e_pwlen = 0;
		// e_sitelen  = getline(&e_site,  &e_sitelen,  pw_file);
		// e_unamelen = getline(&e_uname, &e_unamelen, pw_file);
		// e_pwlen    = getline(&e_pw,    &e_pwlen,    pw_file);
		// getline
		// linesize = getline(&lineptr, &linesize, pw_file);
		// lineptr[linesize - 1] = '\0'; // LF replaced by NUL

		// // break the line into pieces to store them in `table'
		// // breaking will occur at the separator character
		// // find out where the separator appears
		// int separator_indices[5];
		// for(size_t j = 0, k = 0; j < linesize; ++j)
		// {
		// 	if(lineptr[j] == COLUMN_SEPARATOR)
		// 	{
		// 		lineptr[j] = '\0';
		// 		separator_indices[k++] = j;
		// 		// switch(k++)
		// 		// {
		// 		// 	case 0: items[i].site  =                  lineptr + j + 1; break;
		// 		// 	case 1: items[i].uname =                  lineptr + j + 1; break;
		// 		// 	case 2: items[i].e_pw  = (char unsigned *)lineptr + j + 1; break;
		// 		// 	case 3: items[i].e_key = (char unsigned *)lineptr + j + 1; break;
		// 		// 	case 4: items[i].iv    = (char unsigned *)lineptr + j + 1; break;
		// 		// }
		// 	}
		// }

		// // all the data in `lineptr' is in hexdigest form
		// // convert it to digest form
		// char unsigned *e_site  = lineptr + separator_indices[0] + 1;
		// char unsigned *e_uname = lineptr + separator_indices[1] + 1;
		// char unsigned *e_pw    = lineptr + separator_indices[2] + 1;
		// char unsigned *e_key   = lineptr + separator_indices[3] + 1;
		// char unsigned *iv      = lineptr + separator_indices[4] + 1;
		// printf("%zd,%zd,%zd,%zd,%zd\n", strlen(e_site), strlen(e_uname), strlen(e_pw), strlen(e_key), strlen(iv)); 
		// // hexdigest_to_digest(&e_site, strlen(e_site) / 2);
		// // fix_list(items + i);
		// items[i].valid = TRUE;
		for(int j = 0; j < 5; ++j)
		{
			char *ptr = NULL;
			size_t len = 0;
			len = getline(&ptr, &len, pw_file) - 1;
			ptr[len] = '\0';
			items[i].lens[j] = len / 2;
			hexdigest_to_digest((char unsigned **)&ptr, items[i].lens[j]);
			items[i].ptrs[j] = ptr;
		}
		char *site, *uname, *pw;
		char unsigned *e_pw, *key, *e_key;
		char unsigned *iv = items[i].ptrs[I_IV];
		char unsigned *kek = get_credentials_kek();
		decrypt(items[i].ptrs[I_KEY], ENCRYPT_KEY_LENGTH, kek, iv, &key);
		decrypt(items[i].ptrs[I_SITE], items[i].lens[I_SITE], key, iv, &site); printf("%s\n", site);
		decrypt(items[i].ptrs[I_UNAME], items[i].lens[I_UNAME], key, iv, &uname); printf("%s\n", uname);
		decrypt(items[i].ptrs[I_PW], items[i].lens[I_PW], key, iv, &pw); printf("%s\n", pw);
	}
	fclose(pw_file);

	// see_list();
}

