#include "data_structures.h"
#include "memory_manager.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ==================================================================
// Implementação de Listas Ligadas
// ==================================================================

ListNode* list_insert(ListNode *head, void *data) {
    ListNode *new_node = safe_malloc(sizeof(ListNode));
    new_node->data = data;
    new_node->next = head;
    return new_node;
}

ListNode* list_insert_sorted(ListNode *head, void *data, int (*cmp)(void*, void*)) {
    ListNode *new_node = safe_malloc(sizeof(ListNode));
    new_node->data = data;
    
    if (!head || cmp(data, head->data) < 0) {
        new_node->next = head;
        return new_node;
    }

    ListNode *current = head;
    while (current->next && cmp(data, current->next->data) >= 0) {
        current = current->next;
    }
    
    new_node->next = current->next;
    current->next = new_node;
    return head;
}

ListNode* list_remove(ListNode *head, void *data, int (*cmp)(void*, void*), void (*free_data)(void*)) {
    ListNode *current = head, *prev = NULL;
    
    while (current) {
        if (cmp(current->data, data) == 0) {
            if (prev) prev->next = current->next;
            else head = current->next;
            
            if (free_data) free_data(current->data);
            free(current);
            return head;
        }
        prev = current;
        current = current->next;
    }
    return head;
}

void list_free(ListNode *head, void (*free_data)(void*)) {
    ListNode *current = head;
    while (current) {
        ListNode *next = current->next;
        if (free_data) free_data(current->data);
        free(current);
        current = next;
    }
}

void* list_find(ListNode *head, void *key, int (*cmp)(void*, void*)) {
    ListNode *current = head;
    while (current) {
        if (cmp(current->data, key) == 0) return current->data;
        current = current->next;
    }
    return NULL;
}

// ==================================================================
// Implementação de Hash Tables
// ==================================================================

HashTable* hash_create(int size) {
    HashTable *ht = safe_malloc(sizeof(HashTable));
    ht->size = size;
    ht->buckets = safe_malloc(size * sizeof(ListNode*));
    for (int i = 0; i < size; i++) ht->buckets[i] = NULL;
    return ht;
}

void hash_insert(HashTable *ht, const char *key, void *data) {
    unsigned long index = hash_function(key) % ht->size;
    ht->buckets[index] = list_insert(ht->buckets[index], data);
}

void* hash_get(HashTable *ht, const char *key) {
    unsigned long index = hash_function(key) % ht->size;
    return list_find(ht->buckets[index], (void*)key, (int (*)(void*, void*))strcmp);
}

void hash_remove(HashTable *ht, const char *key, int (*cmp)(void*, void*), void (*free_data)(void*)) {
    unsigned long index = hash_function(key) % ht->size;
    ht->buckets[index] = list_remove(ht->buckets[index], (void*)key, cmp, free_data);
}

void hash_free(HashTable *ht, void (*free_data)(void*)) {
    for (int i = 0; i < ht->size; i++) {
        list_free(ht->buckets[i], free_data);
    }
    free(ht->buckets);
    free(ht);
}

// ==================================================================
// Função de Hash (djb2)
// ==================================================================
unsigned long hash_function(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
}