#include "memory_manager.h"
#include <stdio.h>
#include <stdlib.h>

// ==================================================================
// Funções Internas
// ==================================================================

static void free_vaccine(void *vaccine_ptr) {
    Vaccine *v = (Vaccine*)vaccine_ptr;
    list_free(v->lots, free); // Liberta todos os lotes
    free(v);
}

static void free_user_record(void *user_ptr) {
    UserRecord *ur = (UserRecord*)user_ptr;
    list_free(ur->applications, free); // Liberta todas as inoculações
    free(ur);
}

void free_lot(void *lot_ptr) {
    free(lot_ptr); // Lote é uma estrutura simples
}

// ==================================================================
// Implementação Pública
// ==================================================================

void* safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "No memory.\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void free_all(HashTable *vaccines_ht, HashTable *users_ht) {
    if (vaccines_ht) {
        hash_free(vaccines_ht, free_vaccine); // Já libera os lotes via free_vaccine
    }
    if (users_ht) {
        hash_free(users_ht, free_user_record);
    }
}