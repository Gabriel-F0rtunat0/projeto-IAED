#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "data_structures.h"
#include "vaccine.h"
#include "inoculation.h"

/**
 * Alocação de memória segura com tratamento de erros
 * @param size Tamanho a alocar
 * @return Ponteiro para memória alocada
 * @note Termina o programa com mensagem de erro em caso de falha
 */
void* safe_malloc(size_t size);

/**
 * Libertação completa de todos os recursos do sistema
 * @param vaccines_ht Tabela hash de vacinas
 * @param users_ht Tabela hash de utentes
 */
void free_all(HashTable *vaccines_ht, HashTable *users_ht);

void free_lot(void *lot_ptr);

#endif