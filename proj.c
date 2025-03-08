#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_VACCINE_NAME 50
#define MAX_LOT_NAME 21
#define MAX_USER_NAME 200
#define MAX_NUMBER_VACCINES 1000
#define DIM 65535

typedef enum {
    LANG_EN,
    LANG_PT
} Language;

typedef struct {
    int day;
    int month;
    int year;
} Expiration;

typedef struct {
    char name[MAX_VACCINE_NAME];
    char lot[MAX_LOT_NAME];
    Expiration expiration;
    int numDoses;
} Vaccine;

typedef struct {
    char user[MAX_USER_NAME];
    Vaccine vacina;
    Expiration aplicationDate;
} Inoculation;

Language current_language = LANG_EN;

void set_language(int argc, char *argv[]) {
    if (argc > 1)
        if(strcmp(argv[1], "pt") == 0) {
            current_language = LANG_PT;
        }
        else
            exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    Vaccine* vaccines[MAX_NUMBER_VACCINES];
    char s[DIM];

    set_language(argc, argv);

    
    return 0;
}