// prototypes
void wait(int unsigned delay);
gboolean hide_tooltip(gpointer data);
void digest_to_hexdigest(char unsigned **p, size_t size);
void hexdigest_to_digest(char unsigned **p, size_t size);

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
// so that `hexdigest' can be printed as a string
void digest_to_hexdigest(char unsigned **p, size_t size)
{
	char unsigned *digest    = *p;
	char unsigned *hexdigest = malloc((2 * size + 1) * sizeof *hexdigest);

	// for every byte in `digest', write two bytes in `hexdigest'
	for(size_t i = 0; i < size; ++i)
	{
		sprintf((char *)hexdigest + 2 * i, "%02x", digest[i]);
	}

	// make the argument point to `hexdigest'
	memset(digest, 0, size);
	free(digest);
	*p = hexdigest;
}

///////////////////////////////////////////////////////////////////////////////

// reverse of `digest_to_hexdigest'
// `digest' is just an array of bytes, not supposed to be printed like a string
// hence, no null character will be added at the end
void hexdigest_to_digest(char unsigned **p, size_t size)
{
	char unsigned *hexdigest = *p;
	char unsigned *digest    = malloc(size * sizeof *digest);

	// for every two bytes in `hexdigest', write one byte in `digest'
	// a warning is expected here
	// because putting `int unsigned' in `char unsigned'
	// I can think of no other way to do this
	for(size_t i = 0; i < size; ++i)
	{
		#pragma GCC diagnostic ignored "-Wformat"
		sscanf((char *)hexdigest + 2 * i, "%2x", digest + i);
		#pragma GCC diagnostic error "-Wformat"
	}

	// make the argument point to `digest'
	memset(hexdigest, 0, 2 * size + 1);
	free(hexdigest);
	*p = digest;
}

///////////////////////////////////////////////////////////////////////////////

// hide the tooltip, if the widget has any
gboolean hide_tooltip(gpointer data)
{
	GtkWidget *window = data;
	gtk_widget_set_has_tooltip(window, FALSE);
	return G_SOURCE_REMOVE;
}

///////////////////////////////////////////////////////////////////////////////

// encrypt data using 256-bit AES
// this encrypts data to be stored on the disk, hence XTS mode is used
int encrypt(char unsigned *pt, int ptlen, char unsigned *key, char unsigned *iv, char unsigned **ct)
{
	// allocate space to store ciphertext
	// to be on the safer side, allocate more space than required
	*ct = malloc(2 * ptlen * sizeof **ct);

	// encrypt
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv);
	int len;
	EVP_EncryptUpdate(ctx, *ct, &len, pt, ptlen);
	int ctlen = len;
	EVP_EncryptFinal_ex(ctx, *ct + len, &len);
	ctlen += len;
	EVP_CIPHER_CTX_free(ctx);

	// all the bytes allocated to store the ciphertext may not be used
	// hence, the actual ciphertext length is being returned
	return ctlen;
}

///////////////////////////////////////////////////////////////////////////////

// decrypt data using 256-bit AES
int decrypt(char unsigned *ct, int ctlen, char unsigned *key, char unsigned *iv, char unsigned **pt)
{
	// allocate space to store plaintext
	// to be on the safer side, allocate more space than required
	*pt = malloc(2 * ctlen * sizeof **pt);

	// decrypt
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv);
	int len;
	EVP_DecryptUpdate(ctx, *pt, &len, ct, ctlen);
	int ptlen = len;
	EVP_DecryptFinal_ex(ctx, *pt + len, &len);
	ptlen += len;
	EVP_CIPHER_CTX_free(ctx);

	// plaintext is supposed to be in printable form
	// hence, a null character must be added at the end
	(*pt)[ptlen] = '\0';

	// all the bytes allocated to store the plaintext may not be used
	// hence, the actual plaintext length is being returned
	return ptlen;
}

///////////////////////////////////////////////////////////////////////////////

// generate a random value
// `key_len' is the width of the value in bytes
// this can be used to generate a 256-bit encryption key
// or a 128-bit initialisation vector
char unsigned *generate_random(int key_len)
{
	char unsigned *key = malloc(key_len * sizeof *key);
	for(int i = 0; i < key_len; ++i)
	{
		key[i] = random();
	}

	return key;
}

///////////////////////////////////////////////////////////////////////////////

// decrypt the website and username from the file
// leave the password encrypted
// put it all in a table and return that table
char ***retrieve_data_from_file(int *number_of_lines)
{
	// first, traverse the file and calculate the number of lines in it
	FILE *pw_file = fopen(Slave, "rb");
	while(fscanf(pw_file, "%*[^\n]\n") != EOF)
	{
		++*number_of_lines;
	}
	rewind(pw_file);

	// allocate space to store the table
	char ***lines = malloc(*number_of_lines * sizeof *lines);
	for(int i = 0; i < *number_of_lines; ++i)
	{
		// get the length of this line
		// this line is line number `i' of the file
		off_t current_offset = ftello(pw_file);
		int len = 0;
		int ch;
		while((ch = fgetc(pw_file)) != '\n')
		{
			++len;
		}
		fseeko(pw_file, current_offset, 0);

		// each row in the table has 6 columns
		// website, username, password, key and initialisation vector
		// sixth column is for validity
		// it will be used to decide if a row matches a search result
		lines[i] = malloc(6 * sizeof **lines);

		// store the entire line as a string
		// then break it wherever the separator character occurs
		// assign the broken pieces to the 5 columns of the table
		char *line = malloc((len + 2) * sizeof *line);
		fgets(line, len + 2, pw_file);
		line[len] = '\0';
		for(int j = 0, k = 0; j < len; ++j)
		{
			if(line[j] == 0x1b)
			{
				line[j] = '\0';
				lines[i][k++] = line + j + 1;
			}
		}
		lines[i][5] = lines[i][0];
	}

	fclose(pw_file);

	return lines;
}
