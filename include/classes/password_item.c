/*-----------------------------------------------------------------------------
Wrapper struct. Store the plaintext and ciphertext data, and the lengths of the
ciphertext data. (The lengths of `key' and `e_key' are not stored, because they
are already known: both will be of length `AES_KEY_LENGTH'.)

There is no need to store the length of the encrypted key, because it is known.
Encrypting a 256-bit value using AES256 gives a 256-bit value.

Members:
    website
    username
    password
    e_website: `website' encrypted with `key'
    e_username: `username' encrypted with `key'
    e_password: `password' encrypted with `key'
    e_website_length
    e_username_length
    e_password_length
    iv: initialisation vector for encryption
    key: encryption key (it is also used for decryption)
    e_key: `key' encrypted with the key encryption key
-----------------------------------------------------------------------------*/
typedef struct
{
    char *website;
    char *username;
    char *password;

    char unsigned *e_website;
    char unsigned *e_username;
    char unsigned *e_password;

    int e_website_length;
    int e_username_length;
    int e_password_length;

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
void password_items_write_to_file(password_item_t **items, int num_of_items);
void password_item_delete(password_item_t *self);

/*-----------------------------------------------------------------------------
Initialiser for the `password_item_t' struct. Encrypt the plaintext data and
set up all members of the struct. Note that copies of the plaintext data are
created.
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
Read the ciphertext data in the password file into an array.
-----------------------------------------------------------------------------*/
password_item_t **password_items_new_from_file(int *num_of_items, char unsigned *kek)
{
    // this is initialised to a null pointer
    // because I intend to call `realloc' on it later
    password_item_t **items = NULL;

    // check `password_item_append' to see the format in which data is written
    // here data is read conforming to the same format
    FILE *Slave_file = fopen(Slave, "rb");
    for(*num_of_items = 0;; ++*num_of_items)
    {
        int e_website_length, e_username_length, e_password_length;
        if(fread(&e_website_length, sizeof(int), 1, Slave_file) != 1 || fread(&e_username_length, sizeof(int), 1, Slave_file) != 1 || fread(&e_password_length, sizeof(int), 1, Slave_file) != 1)
        {
            fclose(Slave_file);
            break;
        }

        // read
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

        // decrypt
        char *website, *username, *password;
        char unsigned *key;
        decrypt_AES(e_key, AES_KEY_LENGTH, kek, iv, &key);
        decrypt_AES(e_website, e_website_length, key, iv, (char unsigned **)&website);
        decrypt_AES(e_username, e_username_length, key, iv, (char unsigned **)&username);
        decrypt_AES(e_password, e_password_length, key, iv, (char unsigned **)&password);

        // create struct with these variables as members
        password_item_t *item = malloc(sizeof *item);
        item->website = website;
        item->username = username;
        item->password = password;
        item->key = key;
        item->iv = iv;
        item->e_website = e_website;
        item->e_username = e_username;
        item->e_password = e_password;
        item->e_key = e_key;
        item->e_website_length = e_website_length;
        item->e_username_length = e_username_length;
        item->e_password_length = e_password_length;

        password_item_t **temp = realloc(items, (*num_of_items + 1) * sizeof *temp);
        if(temp == NULL)
        {
            fprintf(stderr, "Could not allocate memory to read password file.\n");
            exit(EXIT_FAILURE);
        }
        items = temp;
        items[*num_of_items] = item;
    }

    return items;
}

/*-----------------------------------------------------------------------------
Write the ciphertext data in the array to the password file.
-----------------------------------------------------------------------------*/
void password_items_write_to_file(password_item_t **items, int num_of_items)
{
    FILE *Slave_file = fopen(Slave__, "w");
    for(int i = 0; i < num_of_items; ++i)
    {
        password_item_t *item = items[i];
        fwrite(&(item->e_website_length), sizeof(int), 1, Slave_file);
        fwrite(&(item->e_username_length), sizeof(int), 1, Slave_file);
        fwrite(&(item->e_password_length), sizeof(int), 1, Slave_file);
        fwrite(item->e_website, 1, item->e_website_length, Slave_file);
        fwrite(item->e_username, 1, item->e_username_length, Slave_file);
        fwrite(item->e_password, 1, item->e_password_length, Slave_file);
        fwrite(item->e_key, 1, AES_KEY_LENGTH, Slave_file);
        fwrite(item->iv, 1, INIT_VEC_LENGTH, Slave_file);
    }
    fclose(Slave_file);

    remove(Slave);
    rename(Slave__, Slave);
}

/*-----------------------------------------------------------------------------
Sanitise and release the memory used by the struct and its members.
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
    free(self);
}

