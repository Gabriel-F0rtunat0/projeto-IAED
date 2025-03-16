#ifndef INOCULATION_H
#define INOCULATION_H

#include "data_structures.h"
#include "date_utils.h"

#define MAX_USER_NAME 201

typedef struct {
    char lot_id[21];        // ID do lote utilizado
    Date application_date;  // Data da aplicação
} Inoculation;

typedef struct {
    char user_name[201];    // Nome do utente
    ListNode *applications; // Lista de inoculações (Inoculation*)
} UserRecord;

/**
 * Aplica uma dose de vacina a um utente
 * @param users_ht Tabela hash de utentes
 * @param user Nome do utente
 * @param lot_id ID do lote utilizado
 * @param date Data da aplicação
 */
void apply_dose(HashTable *users_ht, const char *user, const char *lot_id, Date date);

/**
 * Remove inoculações de acordo com os critérios
 * @param users_ht Tabela hash de utentes
 * @param user Nome do utente
 * @param date Data específica (NULL para todas)
 * @param lot_id ID específico do lote (NULL para todos)
 * @return Número de inoculações removidas
 */
int delete_inoculations(HashTable *users_ht, const char *user, const Date *date, const char *lot_id);

/**
 * Lista todas as inoculações de um utente
 * @param users_ht Tabela hash de utentes
 * @param user Nome do utente
 */
void list_user_applications(HashTable *users_ht, const char *user);

/**
 * Verifica se já existe aplicação no mesmo dia para a mesma vacina
 * @param user_record Registro do utente
 * @param date Data para verificar
 * @return 1 se existe, 0 caso contrário
 */
int has_application_on_date(const UserRecord *user_record, Date date);

#endif