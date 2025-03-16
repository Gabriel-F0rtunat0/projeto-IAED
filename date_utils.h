#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <stddef.h>

// Estrutura para representar datas
typedef struct {
    int day;
    int month;
    int year;
} Date;

/**
 * Valida se uma data é cronologicamente válida
 * @param day Dia (1-31)
 * @param month Mês (1-12)
 * @param year Ano (> 0)
 * @return 1 se válida, 0 caso contrário
 */
int is_valid_date(int day, int month, int year);

/**
 * Compara duas datas
 * @param a Primeira data
 * @param b Segunda data
 * @return <0 se a < b, 0 se iguais, >0 se a > b
 */
int date_cmp(const Date a, const Date b);

/**
 * Converte uma string no formato dd-mm-aaaa para estrutura Date
 * @param date_str String com a data
 * @param date Ponteiro para estrutura de saída
 * @return 1 em sucesso, 0 em formato inválido/data inválida
 */
int parse_date(const char *date_str, Date *date);

/**
 * Formata uma data para string no formato dd-mm-aaaa
 * @param date Data a formatar
 * @param buffer Buffer de saída
 * @param buffer_size Tamanho do buffer (mínimo 11 caracteres)
 */
void format_date(Date date, char *buffer, size_t buffer_size);

#endif