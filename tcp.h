#ifndef _TCP_H
#define _TCP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void split_by_words(char *input, char (*words)[130]);
void process_help();
bool id_or_secret(char *word, int count);
bool content(char *word);
bool dname(char *word);
void process_auth(char *input, char *output);
void process_join(char *input, char *output);
void process_rename(char *input) ;
void process_message(char *input, char *output);

void income_replye(char *input, char *output);
void income_err(char *input, char *output);
// void process_bye(char *output);

#endif //_TCP_H