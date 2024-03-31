#ifndef _TCP_H
#define _TCP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


void split_by_words(char *input, char (*words)[130]);
bool process_help(char *input);
bool id_or_secret(char *word, int count);
bool content(char *word);
bool dname(char *word);
bool process_auth(char *input, char *output, char *displayname);
bool process_join(char *input, char *output, char *displayname);
bool process_rename(char *input, char *displayname) ;
void process_message(char *input, char *output, char *displayname);

bool income_replye(char *input, char *output);
bool income_err(char *input, char *output);
bool income_msg(char *input, char *output);
bool income_bye(char *input, char *output);

#endif //_TCP_H