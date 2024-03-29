#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_LEN 2048
char *displayname;
void process_help() {
    printf("/auth	{Username} {Secret} {DisplayName}\n/join	{ChannelID}	\n/rename	{DisplayName}\n/help \n");
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
    //delete new line symbol from the end
    input[strcspn(input, "\n")] = 0;
    //divide input into words
    char *token = strtok(input, " ");
    while (token != NULL) {
        strcpy(words[word_count], token);
        word_count++;
        token = strtok(NULL, " ");
    }

}
// Функція для обробки команди AUTH
void process_auth(char *input, char *output) {
    printf("ok\n");
    char words[100][130]; 
    split_by_words(input, words);
    if (strcmp(words[0], "/auth") == 0){
        printf("ok1\n");
        printf("check input : %s\n", words[1]);
        if(id_or_secret(words[1], 20 ) && dname(words[3]) && id_or_secret(words[2], 128) ){
            printf("ok2\n");
            sprintf(output, "AUTH %s AS %s USING %s\r\n", words[1], words[3], words[2]);
            displayname = words[3]; //set the displaynamp
            
        }
    }
}

// Функція для обробки команди JOIN
void process_join(char *input, char *output) {
    char words[100][130]; 
    split_by_words(input, words);
    if (strcmp(words[0], "/join") == 0){
        if(id_or_secret(words[1], 20 ) ){
            sprintf(output, "JOIN %s AS %s \r\n", words[1], displayname);
            
        }
    }
}

void process_rename(char *input) {
    char words[100][130]; 
    split_by_words(input, words);
    if (strcmp(words[0], "/rename") == 0){
        if(dname(words[1])){
            //set the dislayname
            displayname = words[1];
        }
    }
}


void process_message(char *input, char *output) {
    sprintf(output, "MSG FROM %s IS %s\r\n", displayname, input);
   
}

void income_replye(char *input, char *output) {
    char words[100][130]; 
    split_by_words(input, words);
    if (strcmp(words[1], "OK") == 0){
        sprintf(output, "Succes : %s \n", words[3]);
    }
    else if (strcmp(words[1], "NOK") == 0){
        sprintf(output, "Succes : %s \n", words[3]);
    }
    
}


void income_err(char *input, char *output) {
    char words[100][130]; 
    split_by_words(input, words);
    // Рядок, в який будемо конкатенувати всі елементи, починаючи з третього
    char rest_of_words[130 * (100 - 3)];
    rest_of_words[0] = '\0'; // Ініціалізуємо як порожній рядок

    // Конкатенуємо всі елементи після другого
    for (int i = 3; i < 100; i++) {
        strcat(rest_of_words, words[i]);
        strcat(rest_of_words, " "); // Додаємо пробіл між словами
    }

    sprintf(output, "ERR FROM %s : %s \n", words[2], rest_of_words);
}





// int process_input(char *input, char *output) {
    
//     char words[100][130]; 
//     split_by_words(input, words);


        

//         if (strcmp(words[0], "/auth") == 0){
//             process_auth(words, output);
//         }else if (strcmp(words[0], "/join") == 0){
//             process_join(words, output);
//         }else if (strcmp(words[0], "/rename") == 0){
//             // process_rename(words, output);
//             if(dname(words[1])){
//                 //set the dislayname
//                 displayname = words[1];
//             }
            
//         }else if (strcmp(words[0], "/help") == 0){
//             process_help();
//         }else{
//             printf("ERR Unknown command\r\n");
//         }

//         printf("dnaem %s\n", displayname);
//         // printf("\r\n"); // Вивід порожнього рядка для розділення виводу
    
//     return 0;
// }
