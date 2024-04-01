#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_LEN 2048

bool process_help(char *input) {
    char words[100][130]; 
    split_by_words(input, words);
    if(strcmp(words[0], "/help") == 0){
        printf("/auth	{Username} {Secret} {DisplayName}\n/join	{ChannelID}	\n/rename	{DisplayName}\n/help \n");
        return true;
    }else{
        return false;
    }
}

// Функція для перевірки, чи є слово допустимим ідентифікатором (згідно з граматикою)
//count = 20 for id , count = 128 for secret
bool id_or_secret(char *word, int count) {
    int length = strlen(word);
    if (length < 1 || length > count){
        return false;
    }

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

bool content(char *word) {
    int length = strlen(word);
    if (length < 1 || length > 1400)
        return false;

    for (int i = 0; i < length; i++) {
        if (!(word[i] >= 32 && word[i] <= 126) && word[i] != ' ')
            return false;
    }
    return true;
}
bool dname(char *word) {
    int length = strlen(word);
    if (length < 1 || length > 20)
        return false;

    for (int i = 0; i < length; i++) {
        if (!(word[i] >= 32 && word[i] <= 126))
            return false;
    }
    return true;
}
void split_by_words(char *input, char(*words)[130]){
    int word_count = 0;
    //copy of the input
    char input_copy[1000];
    strcpy(input_copy, input);
    //delete new line symbol from the end
    input_copy[strcspn(input_copy, "\n")] = 0;
    //divide input into words
    char *token = strtok(input_copy, " ");
    while (token != NULL) {
        strcpy(words[word_count], token);
        word_count++;
        token = strtok(NULL, " ");
    }

}

// Функція для обробки команди AUTH
bool process_auth(char *input, char *output, char *displayname) {
    char words[100][130]; 
    split_by_words(input, words);
    if (strcmp(words[0], "/auth") == 0){
        if(id_or_secret(words[1], 20 ) && dname(words[3]) && id_or_secret(words[2], 128) ){
            sprintf(output, "AUTH %s AS %s USING %s\r\n", words[1], words[3], words[2]);
            // printf("AUTH %s AS %s USING %s\r\n", words[1], words[3], words[2]);
            for (int i = 0; i < 20; i++) {
                displayname[i] = words[3][i];
            }
            return true;
        }
    }
    return false;
    
}

// Функція для обробки команди JOIN
bool process_join(char *input, char *output, char *displayname) {
    char words[100][130]; 
    split_by_words(input, words);
    if (strcmp(words[0], "/join") == 0){
        if(id_or_secret(words[1], 20 ) ){
            sprintf(output, "JOIN %s AS %s\r\n", words[1], displayname);
            // printf("JOIN %s AS %s \r\n", words[1], displayname);

            return true;
        }
    }
    return false;
    
}

bool process_rename(char *input, char *displayname) {
    char words[100][130]; 
    split_by_words(input, words);
    if (strcmp(words[0], "/rename") == 0){
        if(dname(words[1])){
            //set the dislayname
            for (int i = 0; i < 20; i++) {
                displayname[i] = words[1][i];
            }
            return true;
        }
    }
    return false;
    
}


void process_message(char *input, char *output, char *displayname) {
    input[strcspn(input, "\n")] = 0;
    sprintf(output, "MSG FROM %s IS %s\r\n", displayname,input);
    // printf("MSG FROM %s IS %s\r\n", displayname, rest_of_words);
}

bool income_replye(char *input, char *output) {
    char words[100][130]; 
    // memset(words, 0, sizeof(words));
    split_by_words(input, words);
    if(strcmp(words[0], "REPLY") != 0){
        return false;

    }

    char rest_of_words[130 * (100 - 3)];
    rest_of_words[0] = '\0'; 
    // Конкатенуємо всі елементи після другого
    for (int i = 3; i < 100; i++) {
        // Перевіряємо, чи рядок не порожній
        if (strlen(words[i]) == 0) {
            break; // Якщо порожній, перериваємо цикл
        }
        strcat(rest_of_words, words[i]);
        strcat(rest_of_words, " "); // Додаємо пробіл між словами
    }
    if (strcmp(words[1], "OK") == 0){
        sprintf(output, "Success: %s\n", rest_of_words);
        fprintf(stderr, "%s", output);
        return true;
    }
    else if (strcmp(words[1], "NOK") == 0){
        sprintf(output, "Failure: %s\n", rest_of_words);
        fprintf(stderr, "%s", output);
        return false;
    }
    return false;
}


bool income_err(char *input, char *output) {
    char words[100][130]; 
    split_by_words(input, words);
    if(strcmp(words[0], "ERR") != 0){
        return false;

    }

    // Рядок, в який будемо конкатенувати всі елементи, починаючи з третього
    char rest_of_words[130 * (100 - 3)];
    rest_of_words[0] = '\0'; // Ініціалізуємо як порожній рядок
    
    // Конкатенуємо всі елементи після другого
    for (int i = 4; i < 100; i++) {
        // Перевіряємо, чи рядок не порожній
        if (strlen(words[i]) == 0) {
            break; // Якщо порожній, перериваємо цикл
        }
        strcat(rest_of_words, words[i]);
        strcat(rest_of_words, " "); // Додаємо пробіл між словами
    }

    sprintf(output, "ERR FROM %s: %s\n", words[2], rest_of_words);
    fprintf(stderr, "%s", output);
    return true;
}

bool income_msg(char *input){
    char words[100][130]; 
    split_by_words(input, words);
    if(strcmp(words[0], "MSG") != 0){
        return false;

    }
    char rest_of_words[130 * (100 - 3)];
    rest_of_words[0] = '\0'; // Ініціалізуємо як порожній рядок

    // Конкатенуємо всі елементи після другого
    for (int i = 4; i < 100; i++) {
        // Перевіряємо, чи рядок не порожній
        if (strlen(words[i]) == 0) {
            break; // Якщо порожній, перериваємо цикл
        }
        strcat(rest_of_words, words[i]);
        strcat(rest_of_words, " "); // Додаємо пробіл між словами
    }

    // sprintf(output, " %s : %s \n", words[2], rest_of_words);
    printf("%s : %s \n", words[2], rest_of_words);
    return true;
}

bool income_bye(char *input){
    char words[100][130]; 
    split_by_words(input, words);
    if(strcmp(words[0], "BYE") == 0){
        printf("BYE\n");
        return true;

    }
    return false;
}