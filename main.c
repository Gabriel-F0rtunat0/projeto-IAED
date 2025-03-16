#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "data_structures.h"
#include "memory_manager.h"
#include "vaccine.h"
#include "inoculation.h"
#include "date_utils.h"

/* Variável estática para controle de idioma (não é global!) */
Language current_lang = LANG_EN;

/* Protótipo da função de configuração de idioma */
static void set_language(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    /* 1. Configurar idioma com base nos argumentos */
    set_language(argc, argv);
    
    /* 2. Inicializar estruturas de dados principais */
    HashTable *vaccines_ht = hash_create(128);  // Tabela hash para vacinas
    HashTable *users_ht = hash_create(1024);    // Tabela hash para utentes
    char input_buffer[65535];                   // Buffer para leitura de comandos
    
    /* 3. Loop principal de processamento de comandos */
    while (1) {
        /* 3.1 Ler entrada do usuário */
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            if (feof(stdin)) break;  // Finalizar com EOF (Ctrl+D)
            else continue;           // Ignorar erros de leitura
        }
        
        /* 3.2 Processar comando de término */
        if (input_buffer[0] == 'q' && input_buffer[1] == '\n') {
            break;
        }
        
        /* 3.3 Processar outros comandos */
        process_command(input_buffer, vaccines_ht, users_ht);
    }
    
    /* 4. Libertar todos os recursos alocados */
    free_all(vaccines_ht, users_ht);
    
    return EXIT_SUCCESS;
}

/* Função interna para configuração de idioma */
static void set_language(int argc, char *argv[]) {
    if (argc > 1) {
        if (strcmp(argv[1], "pt") == 0) {
            current_lang = LANG_PT;
        }
    }
}