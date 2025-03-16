#ifndef COMMANDS_H
#define COMMANDS_H

#include "data_structures.h"
#include "vaccine.h"
#include "inoculation.h"
#include "date_utils.h"

typedef enum { LANG_PT, LANG_EN } Language;

extern Language current_lang; 

typedef enum {
    ERR_TOO_MANY_VACCINES,
    ERR_DUPLICATE_BATCH,
    ERR_INVALID_BATCH,
    ERR_INVALID_NAME,
    ERR_INVALID_DATE,
    ERR_INVALID_QUANTITY,
    ERR_NO_STOCK,
    ERR_ALREADY_VACCINATED,
    ERR_NO_SUCH_VACCINE,
    ERR_NO_SUCH_BATCH,
    ERR_NO_SUCH_USER,
    ERR_INVALID_COMMAND
} ErrorCode;

// Interface principal
void process_command(const char *input, HashTable *vaccines_ht, HashTable *users_ht);

// Funções auxiliares
const char* get_error_message(ErrorCode code, Language lang);
void handle_add_batch(char *args, HashTable *vaccines_ht);
void handle_apply_dose(char *args, HashTable *vaccines_ht, HashTable *users_ht);
void handle_list_vaccines(char *args, HashTable *vaccines_ht);
void handle_remove_batch(char *args, HashTable *vaccines_ht);
void handle_delete_records(char *args, HashTable *users_ht);
void handle_list_applications(char *args, HashTable *users_ht);
void handle_time_shift(char *args);

#endif