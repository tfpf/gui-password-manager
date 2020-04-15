/*-----------------------------------------------------------------------------
Encrypt the input array of bytes using 256-bit AES. Automatically allocate
sufficient memory to store the ciphertext.
-----------------------------------------------------------------------------*/
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

/*-----------------------------------------------------------------------------
Decrypt the input array of bytes using 256-bit AES. Automatically allocate
sufficient memory to store the plaintext.
-----------------------------------------------------------------------------*/
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

/*-----------------------------------------------------------------------------
Generate an array of random bytes. It is used to generate an encryption key
and initialisation vector for AES. The length is the number of bytes, not bits.
-----------------------------------------------------------------------------*/
char unsigned *generate_random(int len)
{
	char unsigned *arr = malloc(len * sizeof *arr);
	for(int i = 0; i < len; ++i)
	{
		arr[i] = random();
	}

	return arr;
}

/*-----------------------------------------------------------------------------
Generate a printable random string. Just like `generate_random', but with
ASCII characters only. Can be used to generate random passwords. Length of the
generated string is a random value between the two values provided.
-----------------------------------------------------------------------------*/
char *generate_random_constrained(int lower, int upper)
{
	int len = lower + random() % (upper - lower);
	char *arr = malloc((len + 1) * sizeof *arr);
	arr[len] = '\0';

	char *market = "abcdefghijklmnopqrstuvwxyz"
	               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	               "0123456789"
	               ",.?:;\"\'()[]!"
	               "@#$%^&*{}-_=+";
	int marketlen = strlen(market);
	for(int i = 0; i < len; ++i)
	{
		arr[i] = market[random() % marketlen];
	}

	return arr;
}

