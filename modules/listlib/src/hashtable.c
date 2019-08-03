/*
 * hashtable.h
 *
 *  Created on: 10 Jun 2019
 *      Author: ojmakh
 * 
 * Copyright 2019 Onalenna Junior Makhura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file hashtable.c
 * 
 * @author your name (you@domain.com)
 * 
 * @brief 
 * @version 0.1
 * @date 2019-06-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "listlib/hashtable.h"
#include "listlib/list.h"
#include <string.h>

#include "config.h"
#ifdef USE_OMP
#include <omp.h>
#endif
/**
 * @brief Create or set a hashtable entry data and key
 * 
 * @param htbl 
 * @param entry 
 * @param key 
 * @param data 
 * @return hashtable_entry* 
 */
hashtable_entry* hashtable_set_entry(hashtable* htbl, hashtable_entry *entry, void* key, void* data);

/**
 * @brief Destroy a hashtable entry
 * 
 * @param htbl 
 * @param entry 
 */
void hashtable_entry_destroy(hashtable* htbl, hashtable_entry *entry, void* key, void* data);

/**
 * @brief Look for the key in htbl
 * 
 * @param htbl 
 * @param key 
 * @return hashtable_entry* 
 */
hashtable_entry* hashtable_lookup_entry(hashtable* htbl, void* key);

/**
 * @brief 
 * 
 * @param a 
 * @param b 
 * @return int32_t 
 */
int32_t hashtable_entry_compare(const void* a, const void* b)
{
    hashtable_entry* e1 = *(hashtable_entry **)a;
    hashtable_entry* e2 = *(hashtable_entry **)b;

    return e1->key_compare(e1->key, e2->key);
}

/**
 * @brief 
 * 
 * @param buckets 
 * @param ktype 
 * @param vtype 
 * @return hashtable* 
 */
hashtable* hashtable_init(size_t buckets, enum HTYPES ktype, enum HTYPES dtype, int32_t (*compare)(const void *a, const void *b))
{
    hashtable* htbl = (hashtable *)malloc(sizeof(hashtable));

    if(htbl == NULL)
    {
        printf("ERROR: Hash table memory allocation failed\n");
        return NULL;
    }

    htbl->buckets = find_prime_less_than(buckets);
    if(htbl->buckets <= 0)
    {
        htbl->buckets = buckets;
    }

    htbl->ktype = ktype;
    htbl->dtype = dtype;
    htbl->table = (linkedlist **)malloc(htbl->buckets * sizeof(linkedlist *));
    if(htbl->table == NULL)
    {
        printf("ERROR: Hash table bucket memory allocation failed\n");
        free(htbl);
        return NULL;
    }

    // We need to initialise each bucket to an empty linked list
    for(int32_t i = 0; i < htbl->buckets; i++)
    {
        htbl->table[i] = linkedlist_init(H_PTR);
    }

    htbl->size = 0;
    htbl->collisions = 0;
    htbl->key_compare = compare;
    htbl->keys = set_init(get_htype_size(htbl->ktype), htbl->key_compare);

    /**
     * @brief Determine the function to use for key hashing
     * 
     */
    if(htbl->ktype == H_INT) 
    {
        //printf("hashtable_init: int_hash\n");
        htbl->key_hash = int_hash;
    } else if(htbl->ktype == H_CHAR) 
    {
        //printf("hashtable_init: char_hash\n");
        htbl->key_hash = char_hash;
    } else if(htbl->ktype == H_SHORT) 
    {
        //printf("hashtable_init: short_hash\n");
        htbl->key_hash = short_hash;
    } else if(htbl->ktype == H_LONG) 
    {
        //printf("hashtable_init: long_hash\n");
        htbl->key_hash = long_hash;
    } else if(htbl->ktype == H_DOUBLE) 
    {
        ///printf("hashtable_init: double_hash\n");
        htbl->key_hash = double_hash;
    } else if(htbl->ktype == H_FLOAT) 
    {
        //printf("hashtable_init: float_hash\n");
        htbl->key_hash = float_hash;
    } else if(htbl->ktype == H_STRING) {
        //printf("hashtable_init: str_hash\n");
        htbl->key_hash = str_hash;
    } else {
        //printf("hashtable_init: NULL\n");
        htbl->key_hash = NULL;
    }

    return htbl;
}

/**
 * 
 */ 
int32_t hashtable_insert(hashtable* htbl, void *key, void* value)
{
    int32_t bucket = htbl->key_hash(key, htbl->buckets); // hash the key
    linkedlist* list = htbl->table[bucket];
    hashtable_entry* entry = hashtable_lookup_entry(htbl, key);

    if(entry == NULL) /// The key is not in the table
    {
        entry = hashtable_set_entry(htbl, entry, key, value);
        if(list->size > 0)
        {
            htbl->collisions++;
        }
        linkedlist_tail_add(list, &entry);
        htbl->size++;        
        set_insert(htbl->keys, key);
    } else { /// The key is in the table 
        // In case the value memory is externally managed, we have to get the data
        // in the entry before replacing it.
        size_t dsize = get_htype_size(htbl->dtype);
        void* tmp = malloc(dsize);
        memcpy(tmp, entry->data, dsize);          // Copy the data to a tmp variable
        memcpy(entry->data, value, dsize);        // Replace the data
        memcpy(value, tmp, dsize);          // Copy the original data into value
        free(tmp);

        return -1;
    }

    return 1;
}

/**
 * @brief Look for an entry that matches the key
 * 
 * @param htbl 
 * @param key 
 * @return htbl_entry* 
 */
hashtable_entry* hashtable_lookup_entry(hashtable* htbl, void* key)
{
    int32_t bucket = htbl->key_hash(key, htbl->buckets);
    linkedlist* list = htbl->table[bucket];    
    hashtable_entry* entry = NULL;
    
    node* nd = list->head;
    int32_t i = 0;
    while(nd)
    {
        entry = *(hashtable_entry **)nd->data;
        if(htbl->key_compare(entry->key, key) == 0)
        {
            break;
        }

        nd = nd->next;
        i++;
    }

    // Check that we actually found the data
    return nd ? *(hashtable_entry **)nd->data : NULL;
}

/**
 * @brief Look for the ArrayList at the key location.
 * 
 * First we find the bucket then if it is not empty, we iterate
 * through the nodes to find one that matches the key. If we find
 * the key in one of the nodes, we then return the ArrayList in the
 * node, otherwise we return NULL
 * 
 * @param htbl 
 * @param key 
 * @param data 
 * @return int32_t 1 if found and 0 is not found
 */
int32_t hashtable_lookup(hashtable* htbl, void* key, void* data)
{
    hashtable_entry* tmp = hashtable_lookup_entry(htbl, key);
    if(tmp != NULL)
    {
        size_t dsize = get_htype_size(htbl->dtype);
        memcpy(data, tmp->data, dsize);
        return 1;
    }

    return 0;
}

/**
 * @brief Remove all the key and it's associated value from the hashtable
 * 
 * @param htbl 
 * @param key 
 * @param data 
 * @return int32_t 1 if successfully removed and 0 otherwise
 */
int32_t hashtable_remove(hashtable* htbl, void* key, void* data)
{
    int32_t bucket = htbl->key_hash(key, htbl->buckets);
    linkedlist* list = htbl->table[bucket];
    hashtable_entry* entry = malloc(sizeof(hashtable_entry));
    size_t ksize = get_htype_size(htbl->ktype);
    size_t dsize = get_htype_size(htbl->dtype);
    entry->key = malloc(ksize);
    entry->data = malloc(dsize);
    memcpy(entry->key, key, ksize);
    memcpy(entry->data, data, dsize);

    int32_t ret = linkedlist_remove(list, &entry, hashtable_entry_compare);
    
    if(ret == 1)
    {
        set_remove(htbl->keys, entry->key);
        htbl->size--;
    }

    free(entry->key);
    free(entry->data);
    free(entry);

    return ret;
}

/**
 * @brief Remove all items in the hash table. If deallocate has been given
 * we use it to deallocate data memory
 * 
 * @param htbl 
 * @param key_destroy 
 * @param value_destroy 
 * @return int32_t 
 */
int32_t hashtable_clear(hashtable* htbl, void (*key_destroy)(void *key), void (*value_destroy)(void *value))
{
    void* key = malloc(get_htype_size(htbl->ktype));
    size_t dsize = get_htype_size(htbl->dtype);
    void* data = malloc(dsize);
    hashtable_entry* entry = NULL;

    for(int32_t i = 0; i < set_size(htbl->keys); i++)
    {
        set_value_at(htbl->keys, i, key);
        int32_t bucket = htbl->key_hash(key, htbl->buckets);
        linkedlist* list = htbl->table[bucket];

        if(list == NULL || list->size == 0) // We might have already removed the bucket entries if there was a collision
        {
            continue;
        }

        node* nd = list->head;
        
        int32_t i = 0;
        while(nd)
        {
            entry = *(hashtable_entry **)nd->data;
            hashtable_entry_destroy(htbl, entry, key, data);
            nd = nd->next;
            i++;
        }
        linkedlist_clear(list);
           
        if(value_destroy != NULL)
        {
            value_destroy(*(void **)data);
        }

        if(key_destroy != NULL)
        {
            key_destroy(*(void **)key);
        } 
    }

    free(key);
    free(data);
    set_clear(htbl->keys);
   
    return 1;
}

/**
 * Delete the hashtable and clean up all the allocated memory.
 * 
 * @param htbl - the hashtable
 */ 
int32_t hashtable_destroy(hashtable* htbl, void (*key_destroy)(void *key), void (*value_destroy)(void *value))
{
    if(hashtable_clear(htbl, key_destroy, value_destroy))
    {
        #ifdef USE_OMP
        #pragma omp parallel for
        #endif
        for(size_t i = 0; i < htbl->buckets; i++)
        {
            linkedlist* list = htbl->table[i];
            linkedlist_delete(list);
        }

        set_delete(htbl->keys);
        free(htbl->table);
        free(htbl);
    } else {
        return 0;
    }

    return 1;
}

/**
 * @brief Initialise a table entry
 * 
 * @param key 
 * @param data 
 * @return htbl_entry* 
 */
hashtable_entry* hashtable_set_entry(hashtable* htbl, hashtable_entry *entry, void* key, void* data)
{
    size_t ksize = get_htype_size(htbl->ktype);
    size_t dsize = get_htype_size(htbl->dtype);
    if(entry == NULL)
    {
        entry = (hashtable_entry *)malloc(sizeof(hashtable_entry));
        entry->data = malloc(dsize);
        entry->key = malloc(ksize);
        entry->next = NULL;
        entry->prev = NULL;
    }

    memcpy(entry->key, key, ksize);
    memcpy(entry->data, data, dsize);
    
    return entry;
}

/**
 * @brief 
 * 
 * @param htbl 
 * @param entry 
 * @param key 
 * @param data 
 */
void hashtable_entry_destroy(hashtable* htbl, hashtable_entry *entry, void* key, void* data)
{
    entry->next = NULL;
    entry->prev = NULL;

    // In case the data are managed somewhere else
    // We should copy them for proper deallocation
    size_t dsize = get_htype_size(htbl->dtype);
    memcpy(data, entry->data, dsize);

    free(entry->key);
    free(entry->data);
    free(entry);
}

size_t hashtable_size(hashtable* htbl)
{
    return htbl->size;
}

int32_t hashtable_empty(hashtable* htbl)
{
    if(htbl == NULL || htbl->size == 0)
    {
        return 1;
    }

    return 0;
}