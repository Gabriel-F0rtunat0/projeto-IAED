#include "date_utils.h"
#include <stdio.h>
#include <string.h>

// ==================================================================
// Implementação
// ==================================================================

static int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int is_valid_date(int day, int month, int year) {
    const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Validação básica
    if (year < 1 || month < 1 || month > 12 || day < 1) 
        return 0;

    // Ajuste para ano bissexto
    int max_day = days_in_month[month - 1];
    if (month == 2 && is_leap_year(year))
        max_day = 29;

    return day <= max_day;
}

int date_cmp(const Date a, const Date b) {
    if (a.year != b.year) return a.year - b.year;
    if (a.month != b.month) return a.month - b.month;
    return a.day - b.day;
}

int parse_date(const char *date_str, Date *date) {
    int d, m, y;
    if (sscanf(date_str, "%d-%d-%d", &d, &m, &y) != 3)
        return 0;

    if (!is_valid_date(d, m, y))
        return 0;

    date->day = d;
    date->month = m;
    date->year = y;
    return 1;
}

void format_date(Date date, char *buffer, size_t buffer_size) {
    if (buffer_size < 11) { // dd-mm-yyyy + null terminator
        if (buffer_size > 0) buffer[0] = '\0';
        return;
    }
    snprintf(buffer, buffer_size, "%02d-%02d-%04d", 
             date.day, date.month, date.year);
}