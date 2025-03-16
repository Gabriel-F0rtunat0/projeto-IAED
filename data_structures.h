#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stddef.h>

// Estrutura de nó para lista ligada genérica
typedef struct ListNode {
    void *data;               // Dados armazenados
    struct ListNode *next;    // Próximo nó
} ListNode;

// Estrutura de tabela hash
typedef struct {
    ListNode **buckets;       // Array de baldes
    int size;                 // Tamanho da tabela
} HashTable;

// Funções de lista ligada
ListNode* list_insert(ListNode *head, void *data);
ListNode* list_insert_sorted(ListNode *head, void *data, int (*cmp)(void*, void*));
ListNode* list_remove(ListNode *head, void *data, int (*cmp)(void*, void*), void (*free_data)(void*));
void list_free(ListNode *head, void (*free_data)(void*));
void* list_find(ListNode *head, void *key, int (*cmp)(void*, void*));

// Funções de hash table
HashTable* hash_create(int size);
void hash_insert(HashTable *ht, const char *key, void *data);
void* hash_get(HashTable *ht, const char *key);
void hash_remove(HashTable *ht, const char *key, int (*cmp)(void*, void*), void (*free_data)(void*));
void hash_free(HashTable *ht, void (*free_data)(void*));

// Função de hash para strings (djb2)
unsigned long hash_function(const char *str);

#endif