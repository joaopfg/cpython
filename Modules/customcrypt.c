#include "customcrypt.h"

char * get_program_string(char * filename){
    FILE * fp;

    fp = fopen(filename, "r");

    if (fp == NULL)
        exit(EXIT_FAILURE);

    char * line = NULL;
    char ** lines = NULL;

    size_t len = 0, count = 0, total_len= 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1) {
        size_t line_size = strlen(line);
        total_len += line_size;
        lines = realloc(lines, (count + 1) * sizeof(char*));
        lines[count] = malloc((line_size + 1) * sizeof(char));
        strcpy(lines[count++], line);
    }

    fclose(fp);

    if (line)
        free(line);

    char * text = malloc((total_len + 1) * sizeof(char));
    size_t cur_len = 0;

    for(size_t i = 0; i < count; ++i){
        memcpy(text + cur_len, lines[i], strlen(lines[i]) + 1);
        cur_len += strlen(lines[i]);
    }

    for(size_t i = 0; i < count; ++i){
        free(lines[i]);
    }

    free(lines);

    return text;
}

/*
void add_padding(char * plaintxt){
    size_t len = strlen(plaintxt) + 1;

    if(len % BLOCK_SIZE == 0){
        return;
    }

    size_t padding_size = BLOCK_SIZE - (len % BLOCK_SIZE);

    plaintxt = realloc(plaintxt, (len + padding_size) * sizeof(char));

    for(size_t i = len - 1; i < len + padding_size - 1; ++i){
        plaintxt[i] = '$';
    }

    plaintxt[len + padding_size - 1] = '\0';
}

void remove_padding(char * plaintxt){
    size_t len = strlen(plaintxt), end = len - 1;

    while(plaintxt[end] == '$'){
        if(end == 0){
            break;
        }

        --end;
    }

    if(plaintxt[end] == '$'){
        plaintxt = realloc(plaintxt, end);
        plaintxt[end] = '\0';
    }
    else{
        plaintxt = realloc(plaintxt, end + 1);
        plaintxt[end + 1] = '\0';
    }
}

void init_libgcrypt(){
    if (!gcry_check_version (GCRYPT_VERSION))
    {
        fprintf (stderr, "libgcrypt is too old (need %s, have %s)\n",
                 GCRYPT_VERSION, gcry_check_version (NULL));

        exit (2);
    }

    gcry_control (GCRYCTL_SUSPEND_SECMEM_WARN);
    gcry_control (GCRYCTL_INIT_SECMEM, 16384, 0);
    gcry_control (GCRYCTL_RESUME_SECMEM_WARN);
    gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
}

char * aes_encrypt(char * plaintxt, char * sym_key){
    add_padding(plaintxt);

#define GCRY_CIPHER GCRY_CIPHER_AES256
    gcry_error_t     gcry_ret;
    gcry_cipher_hd_t cipher_hd;
    size_t           index;

    gcry_ret = gcry_cipher_open(&cipher_hd,GCRY_CIPHER,GCRY_CIPHER_MODE_ECB,0);

    if (gcry_ret) {
        printf("gcry_cipher_open failed:  %s/%s\n",
               gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));

        return NULL;
    }

    size_t key_length = gcry_cipher_get_algo_keylen(GCRY_CIPHER);

    gcry_ret = gcry_cipher_setkey(cipher_hd, sym_key, key_length);

    if (gcry_ret) {
        printf("gcry_cipher_setkey failed:  %s/%s\n",
               gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));

        return NULL;
    }

    size_t blk_length = gcry_cipher_get_algo_blklen(GCRY_CIPHER);
    char * init_vector = "Xn2r5u8x/A%D*G-KaPdSgVkYp3s6v9y$B&E(H+MbQeThWmZq4t7w!z%C*F-J@NcR";

    gcry_ret = gcry_cipher_setiv(cipher_hd, init_vector, blk_length);

    if (gcry_ret) {
        printf("gcry_cipher_setiv failed:  %s/%s\n",
               gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));

        return NULL;
    }

    size_t plaintxt_length = strlen(plaintxt) + 1;
    char * encrypted_txt = malloc(plaintxt_length);

    printf("plaintxt      = \"%s\"\n    length=%zd\n", plaintxt, plaintxt_length);

    gcry_ret = gcry_cipher_encrypt(cipher_hd,encrypted_txt,plaintxt_length,plaintxt,plaintxt_length);

    if (gcry_ret) {
        printf("gcry_cipher_encrypt failed:  %s/%s\n",
               gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));

        return NULL;
    }
    printf("encrypted_txt  = ");

    for (index = 0; index < plaintxt_length; index++)
        printf("%02X", (unsigned char)encrypted_txt[index]);

    printf("\n");

    gcry_cipher_close(cipher_hd);

    return encrypted_txt;
}

char * aes_decrypt(char * encrypted_txt, char * sym_key){
#define GCRY_CIPHER GCRY_CIPHER_AES256
    gcry_error_t     gcry_ret;
    gcry_cipher_hd_t cipher_hd;

    gcry_ret = gcry_cipher_open(&cipher_hd,GCRY_CIPHER,GCRY_CIPHER_MODE_ECB,0);

    if (gcry_ret) {
        printf("gcry_cipher_open failed:  %s/%s\n",
               gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));

        return NULL;
    }

    size_t key_length = gcry_cipher_get_algo_keylen(GCRY_CIPHER);

    gcry_ret = gcry_cipher_setkey(cipher_hd, sym_key, key_length);

    if (gcry_ret) {
        printf("gcry_cipher_setkey failed:  %s/%s\n",
               gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));

        return NULL;
    }

    size_t blk_length = gcry_cipher_get_algo_blklen(GCRY_CIPHER);
    char * init_vector = "Xn2r5u8x/A%D*G-KaPdSgVkYp3s6v9y$B&E(H+MbQeThWmZq4t7w!z%C*F-J@NcR";

    gcry_ret = gcry_cipher_setiv(cipher_hd, init_vector, blk_length);

    if (gcry_ret) {
        printf("gcry_cipher_setiv failed:  %s/%s\n",
               gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));

        return NULL;
    }

    char * decrypted_txt = malloc(MAX_PROGRAM_LENGTH);
    gcry_ret = gcry_cipher_setiv(cipher_hd, init_vector, blk_length);

    if (gcry_ret) {
        printf("gcry_cipher_setiv failed:  %s/%s\n",
               gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));

        return NULL;
    }
    gcry_ret = gcry_cipher_decrypt(cipher_hd,decrypted_txt,MAX_PROGRAM_LENGTH,encrypted_txt,MAX_PROGRAM_LENGTH);

    if (gcry_ret) {
        printf("gcry_cipher_decrypt failed:  %s/%s\n",
               gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));

        return NULL;
    }

    gcry_cipher_close(cipher_hd);
    remove_padding(decrypted_txt);
    return decrypted_txt;
}
 */