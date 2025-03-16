#include "inoculation.h"
#include "memory_manager.h"
#include "vaccine.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Função auxiliar para comparar inoculações por data
static int compare_application_dates(void *a, void *b) {
    Inoculation *ia = (Inoculation*)a;
    Inoculation *ib = (Inoculation*)b;
    return date_cmp(ia->application_date, ib->application_date);
}

void apply_dose(HashTable *users_ht, const char *user, const char *lot_id, Date date) {
    // Buscar ou criar registro do utente
    UserRecord *user_record = hash_get(users_ht, user);
    
    if (!user_record) {
        user_record = safe_malloc(sizeof(UserRecord));
        strncpy(user_record->user_name, user, sizeof(user_record->user_name));
        user_record->applications = NULL;
        hash_insert(users_ht, user, user_record);
    }

    // Verificar aplicação no mesmo dia
    if (has_application_on_date(user_record, date)) return;

    // Criar nova inoculação
    Inoculation *new_app = safe_malloc(sizeof(Inoculation));
    strncpy(new_app->lot_id, lot_id, sizeof(new_app->lot_id));
    new_app->application_date = date;

    // Inserir ordenado por data
    user_record->applications = list_insert_sorted(
        user_record->applications, 
        new_app, 
        compare_application_dates
    );
}

int delete_inoculations(HashTable *users_ht, const char *user, const Date *date, const char *lot_id) {
    UserRecord *user_record = hash_get(users_ht, user);
    if (!user_record) return 0;

    int removed = 0;
    ListNode **pp = &user_record->applications;
    
    while (*pp) {
        Inoculation *app = (Inoculation*)(*pp)->data;
        int match_date = !date || (date_cmp(app->application_date, *date) == 0);
        int match_lot = !lot_id || (strcmp(app->lot_id, lot_id) == 0);
        
        if (match_date && match_lot) {
            ListNode *to_remove = *pp;
            *pp = to_remove->next;
            free(to_remove->data);
            free(to_remove);
            removed++;
        } else {
            pp = &(*pp)->next;
        }
    }
    
    // Se todas aplicações foram removidas, remover o próprio usuário
    if (user_record->applications == NULL) {
        hash_remove(users_ht, user, (int (*)(void*, void*))strcmp, free);
    }
    
    return removed;
}

void list_user_applications(HashTable *users_ht, const char *user) {
    UserRecord *user_record = hash_get(users_ht, user);
    if (!user_record) return;

    ListNode *current = user_record->applications;
    char date_str[11];
    
    while (current) {
        Inoculation *app = (Inoculation*)current->data;
        format_date(app->application_date, date_str, sizeof(date_str));
        printf("%s %s %s\n", user_record->user_name, app->lot_id, date_str);
        current = current->next;
    }
}

int has_application_on_date(const UserRecord *user_record, Date date) {
    ListNode *current = user_record->applications;
    while (current) {
        Inoculation *app = (Inoculation*)current->data;
        if (date_cmp(app->application_date, date) == 0) return 1;
        current = current->next;
    }
    return 0;
}