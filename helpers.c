#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////

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
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext)
{
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv);
	int len;
	EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
	int ciphertext_len = len;
	EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
	ciphertext_len += len;
	EVP_CIPHER_CTX_free(ctx);

	return ciphertext_len;
}

///////////////////////////////////////////////////////////////////////////////

// decrypt data using 256-bit AES
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext)
{
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv);
	int len;
	EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
	int plaintext_len = len;
	EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
	plaintext_len += len;
	EVP_CIPHER_CTX_free(ctx);

	return plaintext_len;
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

