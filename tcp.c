#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>


//to release the allocated memory
#define FREE()\
for (int i = 0; i < word_count; i++) {\
            free(words[i]);\
        }\
        free(words);\

//processing help command
bool process_help(char *input) {
    char **words; //array for words
    int word_count; 
    //function that split input by words
    split_by_words(input, &words, &word_count);
    //check if this is help command 
    if(strcmp(words[0], "/help") == 0){
        //print help output and return true
        printf("/auth	{Username} {Secret} {DisplayName}\n/join	{ChannelID}	\n/rename	{DisplayName}\n/help \n");
        //free memory
        FREE();
        return true;
    }else{
        //return fale in case this is mot help command
        //free memory
        FREE();
        return false;
    }
}

//check if id/secret has correct type
//count = 20 for id , count = 128 for secret
bool id_or_secret(char *word, int count) {
    //check word length
    int length = strlen(word);
    if (length < 1 || length > count){
        return false;
    }
    //check symbols types
    for (int i = 0; i < length; i++) {
        if (!((word[i] >= 'A' && word[i] <= 'Z') ||
              (word[i] >= 'a' && word[i] <= 'z') ||
              (word[i] >= '0' && word[i] <= '9') ||
              word[i] == '-')){
            return false;
        }
    }
    return true;
}
//check if content has correct type
bool content(char *word) {
    //check word length
    int length = strlen(word);
    if (length < 1 || length > 1400)
        return false;
    //check symbols types
    for (int i = 0; i < length; i++) {
        if (!(word[i] >= 32 && word[i] <= 126) && word[i] != ' ')
            return false;
    }
    return true;
}

//check if displayname has correct type
bool dname(char *word) {
    //check word length
    int length = strlen(word);
    if (length < 1 || length > 20)
        return false;
    //check symbols types
    for (int i = 0; i < length; i++) {
        if (!(word[i] >= 32 && word[i] <= 126))
            return false;
    }
    return true;
}

//split input by words
void split_by_words(char *input, char ***words, int *word_count){
    *word_count = 0;
    //dublicate input in array that we can modify
    char *start = input;
    //allocate the memory
    *words = (char **)malloc(sizeof(char *));
    //delete \r and \n from the input
    start[strcspn(start, "\n")] = 0;
    start[strcspn(start, "\r")] = 0;
    //find first gap in input
    char *space = strchr(start, ' ');

    while (space != NULL) {
        int word_length = space - start;

        //allocate memory for new word and add to the words
        (*words)[*word_count] = (char *)malloc((word_length + 1) * sizeof(char));
        strncpy((*words)[*word_count], start, word_length);
        (*words)[*word_count][word_length] = '\0'; //add '\0'

        (*word_count)++;
        //increase words array
        *words = (char **)realloc(*words, (*word_count + 1) * sizeof(char *));
        //set start to the beginning of the new word
        start = space + 1;
        //search the next gap
        space = strchr(start, ' ');
    }

    //for last word in input (or single word input)
    if (start != NULL) {
        int word_length = strlen(start);
        (*words)[*word_count] = (char *)malloc((word_length + 1) * sizeof(char));
        strcpy((*words)[*word_count], start);
        (*word_count)++;
    }

}

//processing auth
bool process_auth(char *input, char *output, char *displayname) {
    char **words; //array for words
    int word_count; //for words count
    //split input
    split_by_words(input, &words, &word_count);
    //check syntax of command
    if (strcmp(words[0], "/auth") == 0){
        //check arguments of command
        if(id_or_secret(words[1], 20 ) && dname(words[3]) && id_or_secret(words[2], 128) ){
            //write modified message in output variable
            sprintf(output, "AUTH %s AS %s USING %s\r\n", words[1], words[3], words[2]);
            //set the displayname
            for (int i = 0; i < 20; i++) {
                displayname[i] = words[3][i];
            }
            FREE();
            return true;
        }
    }
    FREE();
    return false;
    
}

//processing join
bool process_join(char *input, char *output, char *displayname) {
    //split input on words
    char **words; 
    int word_count; 
    split_by_words(input, &words, &word_count);
    //check syntax of the command
    if (strcmp(words[0], "/join") == 0){
        //check argument
        if(id_or_secret(words[1], 20 ) ){
            //fill the output variable
            sprintf(output, "JOIN %s AS %s\r\n", words[1], displayname);
            FREE();
            return true;
        }
    }
    FREE();
    return false;
    
}
//processing rename
bool process_rename(char *input, char *displayname) {
    //split input on words
    char **words;
    int word_count;
    split_by_words(input, &words, &word_count);
    //check syntax of the command
    if (strcmp(words[0], "/rename") == 0){
        if(dname(words[1])){
            //set the dislayname
            for (int i = 0; i < 20; i++) {
                displayname[i] = words[1][i];
            }
            FREE();
            return true;
        }
    }
    FREE();
    return false;
    
}

//process message 
void process_message(char *input, char *output, char *displayname) {
    //delete '\n' from the end
    input[strcspn(input, "\n")] = 0;
    //fill the output variable
    sprintf(output, "MSG FROM %s IS %s\r\n", displayname,input);
}
//process server reply
bool income_replye(char *input, char *output) {
    //split input on words
    char **words; 
    int word_count; 
    split_by_words(input, &words, &word_count);
    //check syntax of the command
    if(strcmp(words[0], "REPLY") != 0){
        FREE();
        return false;

    }
    //helping array for reply content
    char rest_of_words[130 * word_count];
    rest_of_words[0] = '\0'; //initialise like empty string
    //concatenate reply content 
    for (int i = 3; i < word_count-1; i++) {
        //check if it is empty
        if (strlen(words[i]) == 0) {
            break;
        }
        //add word to the array and gap after it
        strcat(rest_of_words, words[i]);
        strcat(rest_of_words, " "); 
    }
    //last word process separately
    //(we do not want gap after last word)
    strcat(rest_of_words, words[word_count-1]);
    //process success and failure message
    if (strcmp(words[1], "OK") == 0){
        sprintf(output, "Success: %s\n", rest_of_words);
        fprintf(stderr, "%s", output);
        FREE();
        return true;
    }
    else if (strcmp(words[1], "NOK") == 0){
        sprintf(output, "Failure: %s\n", rest_of_words);
        fprintf(stderr, "%s", output);
        FREE();
        return false;
    }
    FREE();
    return false;
}

//process error from server
bool income_err(char *input, char *output) {
    //split input on words
    char **words; 
    int word_count; 
    split_by_words(input, &words, &word_count);
    if(strcmp(words[0], "ERR") != 0){
        FREE();
        return false;

    }
    //helping array for error content
    char rest_of_words[130 * word_count];
    rest_of_words[0] = '\0'; //initialise like empty string
    //concatenate error content 
    for (int i = 4; i < word_count-1; i++) {
        if (strlen(words[i]) == 0) {
            break; 
        }
        //add word to the array and gap after it
        strcat(rest_of_words, words[i]);
        strcat(rest_of_words, " "); 
    }
    //last word process separately
    strcat(rest_of_words, words[word_count-1]);
    //fill the output variable
    sprintf(output, "ERR FROM %s: %s\n", words[2], rest_of_words);
    fprintf(stderr, "%s", output);
    FREE();
    return true;
}

bool income_msg(char *input){
    //split input on words
    char **words;
    int word_count; 
    split_by_words(input, &words, &word_count);
    if(strcmp(words[0], "MSG") != 0){
        FREE();
        return false;

    }
    //helping array for error content
    char rest_of_words[130 * word_count];
    rest_of_words[0] = '\0'; //initialise like empty string
    //concatenate msg content 
    for (int i = 4; i < word_count-1; i++) {
        if (strlen(words[i]) == 0) {
            break; 
        }
        strcat(rest_of_words, words[i]);
        strcat(rest_of_words, " "); // Додаємо пробіл між словами
    };
    //last word process separately
    strcat(rest_of_words, words[word_count-1]);
    //fill the output variable
    printf("%s: %s\n", words[2], rest_of_words);
    FREE();
    return true;
}

bool income_bye(char *input){
    //split input on words
    char **words; 
    int word_count; 
    split_by_words(input, &words, &word_count);
    //check syntax of the command
    if(strcmp(words[0], "BYE") == 0){
        printf("BYE\n");
        FREE();
        return true;

    }
    FREE();
    return false;
}