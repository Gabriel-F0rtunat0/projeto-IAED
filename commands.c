#include "commands.h"
#include "memory_manager.h"
#include "vaccine.h"
#include "inoculation.h"
#include "date_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static Date current_date = {1, 1, 2025};  // Data inicial do sistema

// ==================================================================
// Funções Auxiliares
// ==================================================================

const char* get_error_message(ErrorCode code, Language lang) {
    const char* messages_en[] = {
        [ERR_TOO_MANY_VACCINES] = "too many vaccines",
        [ERR_DUPLICATE_BATCH] = "duplicate batch number",
        [ERR_INVALID_BATCH] = "invalid batch",
        [ERR_INVALID_NAME] = "invalid name",
        [ERR_INVALID_DATE] = "invalid date",
        [ERR_INVALID_QUANTITY] = "invalid quantity",
        [ERR_NO_STOCK] = "no stock",
        [ERR_ALREADY_VACCINATED] = "already vaccinated",
        [ERR_NO_SUCH_BATCH] = "no such batch",
        [ERR_NO_SUCH_VACCINE] = "no such vaccine",
        [ERR_NO_SUCH_USER] = "no such user",
        [ERR_INVALID_COMMAND] = "invalid command"
    };

    const char* messages_pt[] = {
        [ERR_TOO_MANY_VACCINES] = "demasiadas vacinas",
        [ERR_DUPLICATE_BATCH] = "número de lote duplicado",
        [ERR_INVALID_BATCH] = "lote inválido",
        [ERR_INVALID_NAME] = "nome inválido",
        [ERR_INVALID_DATE] = "data inválida",
        [ERR_INVALID_QUANTITY] = "quantidade inválida",
        [ERR_NO_STOCK] = "esgotado",
        [ERR_NO_SUCH_VACCINE] = "vacina inexistente",
        [ERR_ALREADY_VACCINATED] = "já vacinado",
        [ERR_NO_SUCH_BATCH] = "lote inexistente",
        [ERR_NO_SUCH_USER] = "utente inexistente",
        [ERR_INVALID_COMMAND] = "comando inválido"
    };

    return (lang == LANG_PT) ? messages_pt[code] : messages_en[code];
}

static void trim_quotes(char *str) {
    size_t len = strlen(str);
    if (len >= 2 && str[0] == '"' && str[len-1] == '"') {
        memmove(str, str+1, len-2);
        str[len-2] = '\0';
    }
}

// ==================================================================
// Implementação dos Comandos
// ==================================================================

void handle_add_batch(char *args, HashTable *vaccines_ht) {
    char lot[MAX_LOT_ID + 1], name[MAX_VACCINE_NAME];
    Date exp_date;
    int quantity;
    
    if (sscanf(args, "%20s %d-%d-%d %d %50[^\n]", 
              lot, &exp_date.day, &exp_date.month, &exp_date.year, 
              &quantity, name) != 6) {
        printf("%s\n", get_error_message(ERR_INVALID_COMMAND, current_lang));
        return;
    }

    if (!validate_lot(lot)) {
        printf("%s\n", get_error_message(ERR_INVALID_BATCH, current_lang));
        return;
    }

    if (!is_valid_date(exp_date.day, exp_date.month, exp_date.year) ||
        date_cmp(exp_date, current_date) < 0) {
        printf("%s\n", get_error_message(ERR_INVALID_DATE, current_lang));
        return;
    }

    Vaccine *v = find_vaccine(vaccines_ht, name);
    if (v == NULL) {
        if (vaccine_count >= MAX_NUMBER_VACCINES) {
            printf("%s\n", get_error_message(ERR_TOO_MANY_VACCINES, current_lang));
            return;
        }
        v = create_vaccine(name);
        hash_insert(vaccines_ht, name, v);
    }

    if (add_lot_to_vaccine(v, lot, exp_date, quantity) != 0) {
        printf("%s\n", get_error_message(ERR_DUPLICATE_BATCH, current_lang));
    } else {
        printf("%s\n", lot);
    }
}

void handle_apply_dose(char *args, HashTable *vaccines_ht, HashTable *users_ht) {
    char formatted_user[MAX_USER_NAME], name[MAX_VACCINE_NAME];
    
    if (sscanf(args, " \"%200[^\"]\" %50s", formatted_user, name) == 2 ||
        sscanf(args, "%200s %50s", formatted_user, name) == 2) {
        trim_quotes(formatted_user);
    } else {
        printf("%s\n", get_error_message(ERR_INVALID_COMMAND, current_lang));
        return;
    }

    Vaccine *v = find_vaccine(vaccines_ht, name);
    if (v == NULL) {
        printf("%s: %s\n", name, get_error_message(ERR_NO_SUCH_VACCINE, current_lang));
        return;
    }

    Lot *best_lot = find_earliest_valid_lot(v, current_date);
    if (best_lot == NULL || (best_lot->total_doses - best_lot->used_doses) <= 0) {
        printf("%s\n", get_error_message(ERR_NO_STOCK, current_lang));
        return;
    }

    // Obter o registro do usuário
    UserRecord *user_record = hash_get(users_ht, formatted_user);
    if (!user_record) {
        // Se não existir, não há aplicações
        user_record = safe_malloc(sizeof(UserRecord));
        strncpy(user_record->user_name, formatted_user, MAX_USER_NAME);
        user_record->applications = NULL;
        hash_insert(users_ht, formatted_user, user_record);
    }

    // Verificar aplicação na data atual
    if (has_application_on_date(user_record, current_date)) {
        printf("%s\n", get_error_message(ERR_ALREADY_VACCINATED, current_lang));
        return;
    }

    apply_dose(users_ht, formatted_user, best_lot->id, current_date);
    best_lot->used_doses++;
    printf("%s\n", best_lot->id);
}

void handle_list_vaccines(char *args, HashTable *vaccines_ht) {
    char *token = strtok(args, " ");
    if (token == NULL) { // Listar todas as vacinas
        for (int i = 0; i < vaccines_ht->size; i++) {
            ListNode *current = vaccines_ht->buckets[i];
            while (current) {
                Vaccine *v = (Vaccine*)current->data;
                ListNode *lot_node = v->lots;
                while (lot_node) {
                    Lot *lot = (Lot*)lot_node->data;
                    printf("%s %s %02d-%02d-%04d %d %d\n", v->name, lot->id,
                           lot->expiration.day, lot->expiration.month, lot->expiration.year,
                           lot->total_doses - lot->used_doses, lot->used_doses);
                    lot_node = lot_node->next;
                }
                current = current->next;
            }
        }
    } else { // Listar vacinas específicas
        do {
            Vaccine *v = hash_get(vaccines_ht, token);
            if (!v) {
                printf("%s: %s\n", token, get_error_message(ERR_NO_SUCH_VACCINE, current_lang));
                continue;
            }
            ListNode *lot_node = v->lots;
            while (lot_node) {
                Lot *lot = (Lot*)lot_node->data;
                printf("%s %s %02d-%02d-%04d %d %d\n", v->name, lot->id,
                       lot->expiration.day, lot->expiration.month, lot->expiration.year,
                       lot->total_doses - lot->used_doses, lot->used_doses);
                lot_node = lot_node->next;
            }
        } while ((token = strtok(NULL, " ")));
    }
}

void handle_remove_batch(char *args, HashTable *vaccines_ht) {
    char lot_id[MAX_LOT_ID];
    if (sscanf(args, "%20s", lot_id) != 1) {
        printf("%s\n", get_error_message(ERR_INVALID_COMMAND, current_lang));
        return;
    }

    Lot *found_lot = NULL;
    Vaccine *found_vaccine = NULL;
    // Procurar o lote em todas as vacinas
    for (int i = 0; i < vaccines_ht->size; i++) {
        ListNode *current_vaccine = vaccines_ht->buckets[i];
        while (current_vaccine) {
            Vaccine *v = (Vaccine*)current_vaccine->data;
            ListNode *lot_node = v->lots;
            while (lot_node) {
                Lot *lot = (Lot*)lot_node->data;
                if (strcmp(lot->id, lot_id) == 0) {
                    found_lot = lot;
                    found_vaccine = v;
                    break;
                }
                lot_node = lot_node->next;
            }
            if (found_lot) break;
            current_vaccine = current_vaccine->next;
        }
        if (found_lot) break;
    }

    if (!found_lot) {
        printf("%s: %s\n", lot_id, get_error_message(ERR_NO_SUCH_BATCH, current_lang));
        return;
    }

    printf("%d\n", found_lot->used_doses);
    if (found_lot->used_doses == 0) {
        // Remove o lote se não houver aplicações
        found_vaccine->lots = list_remove(found_vaccine->lots, found_lot, 
            (int (*)(void*, void*))strcmp, free_lot);
    } else {
        // Atualiza o total para as doses usadas
        found_lot->total_doses = found_lot->used_doses;
    }
}

void handle_delete_records(char *args, HashTable *users_ht) {
    char user[MAX_USER_NAME] = {0};
    Date date = {0};
    char lot_id[MAX_LOT_ID] = {0};
    int parsed = sscanf(args, " \"%200[^\"]\" %d-%d-%d %20s", 
                      user, &date.day, &date.month, &date.year, lot_id);
    
    // Casos de parsing
    if (parsed < 1) {
        if (sscanf(args, "%200s", user) != 1) {
            printf("%s\n", get_error_message(ERR_INVALID_COMMAND, current_lang));
            return;
        }
    }
    
    // Validações
    UserRecord *ur = hash_get(users_ht, user);
    if (!ur) {
        printf("%s: %s\n", user, get_error_message(ERR_NO_SUCH_USER, current_lang));
        return;
    }

    int removed = 0;
    if (parsed >= 5) { // Todos os argumentos
        if (!is_valid_date(date.day, date.month, date.year)) {
            printf("%s\n", get_error_message(ERR_INVALID_DATE, current_lang));
            return;
        }
        removed = delete_inoculations(users_ht, user, &date, lot_id);
    } 
    else if (parsed >= 4) { // Data sem lote
        removed = delete_inoculations(users_ht, user, &date, NULL);
    } 
    else { // Apenas usuário
        removed = delete_inoculations(users_ht, user, NULL, NULL);
    }
    
    printf("%d\n", removed);
}

void handle_list_applications(char *args, HashTable *users_ht) {
    if (args[0] == '\0') { // Listar todos
        for (int i = 0; i < users_ht->size; i++) {
            ListNode *current = users_ht->buckets[i];
            while (current) {
                UserRecord *ur = (UserRecord*)current->data;
                list_user_applications(users_ht, ur->user_name);
                current = current->next;
            }
        }
    } else { // Listar usuário específico
        char user[MAX_USER_NAME];
        if (sscanf(args, " \"%200[^\"]\"", user) == 1 ||
            sscanf(args, "%200s", user) == 1) {
            trim_quotes(user);
            if (!hash_get(users_ht, user)) {
                printf("%s: %s\n", user, get_error_message(ERR_NO_SUCH_USER, current_lang));
                return;
            }
            list_user_applications(users_ht, user);
        }
    }
}

void handle_time_shift(char *args) {
    if (args[0] == '\0') { // Sem argumentos: imprime a data atual
        char date_str[11];
        format_date(current_date, date_str, sizeof(date_str));
        printf("%s\n", date_str);
        return;
    }

    Date new_date;
    if (!parse_date(args, &new_date)) {
        printf("%s\n", get_error_message(ERR_INVALID_DATE, current_lang));
        return;
    }

    if (date_cmp(new_date, current_date) < 0) {
        printf("%s\n", get_error_message(ERR_INVALID_DATE, current_lang));
        return;
    }

    current_date = new_date;
    char date_str[11];
    format_date(current_date, date_str, sizeof(date_str));
    printf("%s\n", date_str);
}


// ==================================================================
// Função Principal de Processamento
// ==================================================================

void process_command(const char *input, HashTable *vaccines_ht, HashTable *users_ht) {
    if (strlen(input) == 0) return;
    
    char command = input[0];
    char args_buffer[65535];
    
    if (sscanf(input, " %c %65534[^\n]", &command, args_buffer) < 1) {
        printf("%s\n", get_error_message(ERR_INVALID_COMMAND, current_lang));
        return;
    }

    switch (command) {
        case 'c': handle_add_batch(args_buffer, vaccines_ht); break;
        case 'a': handle_apply_dose(args_buffer, vaccines_ht, users_ht); break;
        case 'l': handle_list_vaccines(args_buffer, vaccines_ht); break;
        case 'r': handle_remove_batch(args_buffer, vaccines_ht); break;
        case 'd': handle_delete_records(args_buffer, users_ht); break;
        case 'u': handle_list_applications(args_buffer, users_ht); break;
        case 't': handle_time_shift(args_buffer); break;
        default: 
            printf("%s: %c\n", get_error_message(ERR_INVALID_COMMAND, current_lang), command);
    }
}
