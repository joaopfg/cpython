#ifndef CUSTOMCRYPT_H
#define CUSTOMCRYPT_H

#ifndef _GNU_SOURCE
#define  _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gcrypt.h>

//const unsigned long int MAX_PROGRAM_LENGTH = 256; //needs to be a multiple of 16
//const unsigned  long int BLOCK_SIZE = 16;

char * get_program_string(char *);
/*
void add_padding(char *);
void remove_padding(char *);
void init_libgcrypt();
char * aes_encrypt(char *, char *);
char * aes_decrypt(char *, char *);
*/

#endif

#endif