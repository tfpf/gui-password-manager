#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////

// block the program for some amount of time
void wait(int unsigned delay)
{
	int unsigned end_time = time(NULL) + delay;
	while(time(NULL) < end_time);
}

///////////////////////////////////////////////////////////////////////////////

// read an array of 128 bytes (the array is the SHA512 digest of something)
// convert this array into an array of 257 bytes
// the converted array is the SHA512 hexdigest of that same something
void digest_to_hexdigest(char unsigned **p)
{
	// `digest' is a 128-byte array
	// `hexdigest' is hexadecimal form of the numerical value of `digest'
	// `hexdigest' it will be 2 * 128 + 1 = 257 bytes long
	// each byte in `digest' will be displayed as two hex characters
	// plus one extra byte at the end, for the null character
	char unsigned *digest = *p;
	char unsigned *hexdigest = malloc((2 * SHA512_DIGEST_LENGTH + 1) * sizeof *hexdigest);

	// for each byte in `digest', write two bytes in `hexdigest'
	for(int i = 0; i < SHA512_DIGEST_LENGTH; ++i)
	{
		sprintf((char *)hexdigest + 2 * i, "%02x", digest[i]);
	}

	// make the argument point to `hexdigest'
	memset(digest, 0, SHA512_DIGEST_LENGTH);
	*p = hexdigest;
}

