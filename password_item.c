/*-----------------------------------------------------------------------------
Container to save and easily access the encrypted and decrypted data.
-----------------------------------------------------------------------------*/
typedef struct
{
    // plaintext strings
    char *website;
    char *username;
    char *password;

    // ciphertext bytearrays
    char unsigned *e_website;
    char unsigned *e_username;
    char unsigned *e_password;

    // lengths of above
    // no need to store encrypted key length, because it is known
    // encrypting a 256-bit value using AES256 gives a 256-bit value
    int e_website_length;
    int e_username_length;
    int e_password_length;

    // cryptographic bytearrays
    char unsigned *iv;
    char unsigned *key;
    char unsigned *e_key;
}
password_item_t;

/*-----------------------------------------------------------------------------
Function prototypes.
-----------------------------------------------------------------------------*/
password_item_t *password_item_new_from_plaintext(char const *website, char const *username, char const *password, char unsigned *kek);
password_item_t **password_items_new_from_file(int *num_of_items, char unsigned *kek);
void password_item_append(password_item_t *self);
void password_item_delete(password_item_t *self);

/*-----------------------------------------------------------------------------
Read the plaintext data. Encrypt and save the ciphertext data. Copies of the
plaintext are created. The original plaintext strings come from GTK entries,
and must not be modified.
-----------------------------------------------------------------------------*/
password_item_t *password_item_new_from_plaintext(char const *website, char const *username, char const *password, char unsigned *kek)
{
    password_item_t *self = malloc(sizeof *self);
    self->website = strdup(website);
    self->username = strdup(username);
    self->password = strdup(password);
    self->iv = gen_rand(INIT_VEC_LENGTH);
    self->key = gen_rand(AES_KEY_LENGTH);

    // encrypt
    self->e_website_length = encrypt_AES((char unsigned *)self->website, strlen(self->website), self->key, self->iv, &(self->e_website));
    self->e_username_length = encrypt_AES((char unsigned *)self->username, strlen(self->username), self->key, self->iv, &(self->e_username));
    self->e_password_length = encrypt_AES((char unsigned *)self->password, strlen(self->password), self->key, self->iv, &(self->e_password));
    encrypt_AES(self->key, AES_KEY_LENGTH, kek, self->iv, &(self->e_key));

    return self;
}

/*-----------------------------------------------------------------------------
Load all the password items saved in the password file.
-----------------------------------------------------------------------------*/
password_item_t **password_items_new_from_file(int *num_of_items, char unsigned *kek)
{
    FILE *Slave_file = fopen(Slave, "rb");

    // read the data the same way it is written in `password_item_append'
    for(*num_of_items = 0;; ++*num_of_items)
    {
        int e_website_length, e_username_length, e_password_length;
        if(fread(&e_website_length, sizeof(int), 1, Slave_file) != 1 || fread(&e_username_length, sizeof(int), 1, Slave_file) != 1 || fread(&e_password_length, sizeof(int), 1, Slave_file) != 1)
        {
            fclose(Slave_file);
            break;
        }

        char unsigned *e_website = malloc(e_website_length * sizeof *e_website);
        char unsigned *e_username = malloc(e_username_length * sizeof *e_username);
        char unsigned *e_password = malloc(e_password_length * sizeof *e_password);
        char unsigned *e_key = malloc(AES_KEY_LENGTH * sizeof *e_key);
        char unsigned *iv = malloc(INIT_VEC_LENGTH * sizeof *iv);
        fread(e_website, 1, e_website_length, Slave_file);
        fread(e_username, 1, e_username_length, Slave_file);
        fread(e_password, 1, e_password_length, Slave_file);
        fread(e_key, 1, AES_KEY_LENGTH, Slave_file);
        fread(iv, 1, INIT_VEC_LENGTH, Slave_file);

        char *website, *username, *password;
        char unsigned *key;
        decrypt_AES(e_key, AES_KEY_LENGTH, kek, iv, &key);
        decrypt_AES(e_website, e_website_length, key, iv, (char unsigned **)&website);
        decrypt_AES(e_username, e_username_length, key, iv, (char unsigned **)&username);
        decrypt_AES(e_password, e_password_length, key, iv, (char unsigned **)&password);
        printf("%s,%s,%s\n", website, username, password);
    }
}

/*-----------------------------------------------------------------------------
Append the password item to the password file.
-----------------------------------------------------------------------------*/
void password_item_append(password_item_t *self)
{
    FILE *Slave_file = fopen(Slave, "ab");
    fwrite(&(self->e_website_length), sizeof(int), 1, Slave_file);
    fwrite(&(self->e_username_length), sizeof(int), 1, Slave_file);
    fwrite(&(self->e_password_length), sizeof(int), 1, Slave_file);
    fwrite(self->e_website, 1, self->e_website_length, Slave_file);
    fwrite(self->e_username, 1, self->e_username_length, Slave_file);
    fwrite(self->e_password, 1, self->e_password_length, Slave_file);
    fwrite(self->e_key, 1, AES_KEY_LENGTH, Slave_file);
    fwrite(self->iv, 1, INIT_VEC_LENGTH, Slave_file);
    fclose(Slave_file);
}

/*-----------------------------------------------------------------------------
Sanitise the sensitive data and release the memory.
-----------------------------------------------------------------------------*/
void password_item_delete(password_item_t *self)
{
    zero_and_free((char unsigned *)self->website, strlen(self->website));
    zero_and_free((char unsigned *)self->username, strlen(self->username));
    zero_and_free((char unsigned *)self->password, strlen(self->password));
    zero_and_free(self->iv, INIT_VEC_LENGTH);
    zero_and_free(self->key, AES_KEY_LENGTH);
    zero_and_free(self->e_website, self->e_website_length);
    zero_and_free(self->e_username, self->e_username_length);
    zero_and_free(self->e_password, self->e_password_length);
    zero_and_free(self->e_key, AES_KEY_LENGTH);
}

