#ifndef VACCINE_H
#define VACCINE_H

#include "data_structures.h"
#include "date_utils.h"

#define MAX_VACCINE_NAME 51    // 50 caracteres + \0
#define MAX_LOT_ID 21          // 20 dígitos hex + \0
#define MAX_NUMBER_VACCINES 1000


// Adicionar declaração
extern int vaccine_count;

typedef struct {
    char id[MAX_LOT_ID];      // Identificador único do lote
    Date expiration;          // Data de validade
    int total_doses;          // Doses totais no lote
    int used_doses;           // Doses já utilizadas
} Lot;

typedef struct {
    char name[MAX_VACCINE_NAME]; // Nome da vacina
    ListNode *lots;              // Lista de lotes ordenados por validade
} Vaccine;

/**
 * Cria uma nova vacina
 * @param name Nome da vacina
 * @return Ponteiro para a vacina criada
 */
Vaccine* create_vaccine(const char *name);

/**
 * Adiciona um novo lote à vacina
 * @param vaccine Vacina alvo
 * @param lot_id Identificador do lote
 * @param expiration Data de validade
 * @param doses Número de doses
 * @return 0 em sucesso, -1 em erro
 */
int add_lot_to_vaccine(Vaccine *vaccine, const char *lot_id, Date expiration, int doses);

/**
 * Encontra uma vacina na tabela hash
 * @param ht Tabela hash de vacinas
 * @param name Nome da vacina
 * @return Ponteiro para a vacina ou NULL
 */
Vaccine* find_vaccine(HashTable *ht, const char *name);

/**
 * Valida o formato de um identificador de lote
 * @param lot_id Identificador a validar
 * @return 1 se válido, 0 caso contrário
 */
int validate_lot(const char *lot_id);

/**
 * Encontra o lote válido mais antigo
 * @param vaccine Vacina alvo
 * @param current_date Data atual do sistema
 * @return Ponteiro para o lote ou NULL
 */
Lot* find_earliest_valid_lot(const Vaccine *vaccine, Date current_date);

Vaccine* find_vaccine_by_lot(HashTable *ht, const char *lot_id);

#endif