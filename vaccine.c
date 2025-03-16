#include "vaccine.h"
#include "memory_manager.h"
#include "date_utils.h"
#include <ctype.h>
#include <string.h>

int vaccine_count = 0; // Contador global de vacinas

// Função de comparação para ordenação de lotes
static int compare_lots(void *a, void *b) {
    Lot *lot_a = (Lot*)a;
    Lot *lot_b = (Lot*)b;
    return date_cmp(lot_a->expiration, lot_b->expiration);
}

Vaccine* create_vaccine(const char *name) {
    if (vaccine_count >= MAX_NUMBER_VACCINES) return NULL;
    
    Vaccine *v = safe_malloc(sizeof(Vaccine));
    strncpy(v->name, name, MAX_VACCINE_NAME);
    v->lots = NULL;
    vaccine_count++;
    return v;
}

int add_lot_to_vaccine(Vaccine *vaccine, const char *lot_id, Date expiration, int doses) {
    // Verificar se o lote já existe
    ListNode *current = vaccine->lots;
    while (current) {
        Lot *l = (Lot*)current->data;
        if (strcmp(l->id, lot_id) == 0) return -1;
        current = current->next;
    }

    // Criar novo lote
    Lot *new_lot = safe_malloc(sizeof(Lot));
    strncpy(new_lot->id, lot_id, MAX_LOT_ID);
    new_lot->expiration = expiration;
    new_lot->total_doses = doses;
    new_lot->used_doses = 0;

    // Inserir ordenado por validade
    vaccine->lots = list_insert_sorted(vaccine->lots, new_lot, compare_lots);
    return 0;
}

Vaccine* find_vaccine(HashTable *ht, const char *name) {
    return hash_get(ht, name);
}

int validate_lot(const char *lot_id) {
    size_t len = strlen(lot_id);
    if (len == 0 || len > MAX_LOT_ID - 1) return 0;
    
    for (size_t i = 0; i < len; i++) {
        if (!isxdigit(lot_id[i]) || islower(lot_id[i])) return 0;
    }
    return 1;
}

Lot* find_earliest_valid_lot(const Vaccine *vaccine, Date current_date) {
    ListNode *current = vaccine->lots;
    while (current) {
        Lot *lot = (Lot*)current->data;
        int cmp = date_cmp(lot->expiration, current_date);
        
        if (cmp >= 0 && (lot->total_doses - lot->used_doses) > 0) {
            return lot;
        }
        current = current->next;
    }
    return NULL;
}

Vaccine* find_vaccine_by_lot(HashTable *ht, const char *lot_id) {
    for (int i = 0; i < ht->size; i++) {
        ListNode *current = ht->buckets[i];
        while (current) {
            Vaccine *v = (Vaccine*)current->data;
            ListNode *lot_node = v->lots;
            while (lot_node) {
                Lot *lot = (Lot*)lot_node->data;
                if (strcmp(lot->id, lot_id) == 0) return v;
                lot_node = lot_node->next;
            }
            current = current->next;
        }
    }
    return NULL;
}