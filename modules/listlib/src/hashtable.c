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
#include <assert.h>

#ifdef _OPENMP
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
void hashtable_entry_destroy(hashtable_entry *entry, void* data, size_t dsize);

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
    return hashtable_init_size(find_prime_less_than(buckets), ktype, dtype, compare);
}

/**
 * @brief 
 * 
 * @param buckets 
 * @param ktype 
 * @param dtype 
 * @param compare 
 * @return hashtable* 
 */
hashtable* hashtable_init_size(size_t buckets, enum HTYPES ktype, enum HTYPES dtype, int32_t (*compare)(const void *a, const void *b))
{
    hashtable* htbl = (hashtable *)malloc(sizeof(hashtable));

    if(htbl == NULL)
    {
        printf("ERROR: Hash table memory allocation failed\n");
        return NULL;
    }

    htbl->buckets = buckets;
    if(buckets <= 0)
    {
        htbl->buckets = 37;
    }

    htbl->ktype = ktype;
    htbl->dtype = dtype;

    // Make sure all buckets are NULL
    htbl->table = calloc(htbl->buckets, sizeof(hashtable_entry *));
    if(htbl->table == NULL)
    {
        printf("ERROR: Hash table bucket memory allocation failed\n");
        free(htbl);
        return NULL;
    }

    htbl->size = 0;
    htbl->collisions = 0;
    htbl->key_compare = compare;
    htbl->keys = set_init(get_htype_size(htbl->ktype), compare);

    /**
     * @brief Determine the function to use for key hashing
     * 
     */
    if(htbl->ktype == H_INT) 
    {
        htbl->key_hash = int_hash;
    } else if(htbl->ktype == H_CHAR) 
    {
        htbl->key_hash = char_hash;
    } else if(htbl->ktype == H_SHORT) 
    {
        htbl->key_hash = short_hash;
    } else if(htbl->ktype == H_LONG) 
    {
        htbl->key_hash = long_hash;
    } else if(htbl->ktype == H_DOUBLE) 
    {
        htbl->key_hash = double_hash;
    } else if(htbl->ktype == H_FLOAT) 
    {
        htbl->key_hash = float_hash;
    } else if(htbl->ktype == H_STRING) {
        htbl->key_hash = str_hash;
    } else {
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
    hashtable_entry* entry = hashtable_lookup_entry(htbl, key);

    if(entry == NULL) /// The key is not in the table
    {
        entry = hashtable_set_entry(htbl, entry, key, value);

        // The bucket has at least one entry
        if(htbl->table[bucket] != NULL)
        {
            htbl->collisions++;

            // Always and to the front of the entry chain
            entry->next = htbl->table[bucket];
            htbl->table[bucket]->prev = entry;
        } 

        htbl->table[bucket] = entry;
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
    hashtable_entry* entry = htbl->table[bucket];
    
    while(entry)
    {
        if(htbl->key_compare(entry->key, key) == 0)
        {
            break;
        }

        entry = entry->next;
    }

    // Check that we actually found the data
    return entry;
}

/**
 * @brief Look for the ArrayList at the key location.
 * 
 * First we find the bucket then if it is not empty, we iterate
 * through the entries to find one that matches the key. If we find
 * the key in one of the entries, we then return the data in the
 * entry, otherwise we return NULL
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
    if(hashtable_empty(htbl) || key == NULL)
    {
        return 0;
    }

    int32_t bucket = htbl->key_hash(key, htbl->buckets);
    hashtable_entry* entry = hashtable_lookup_entry(htbl, key);

    if(entry == NULL)
    {
        return 0;
    }

    // Get the previous entry to point to the next entry
    if(entry->prev != NULL)
    {
        entry->prev->next = entry->next;
        entry->prev = NULL;
    } else { // this is the first entry
        htbl->table[bucket] = entry->next;
        if(htbl->table[bucket] != NULL)
        {
            htbl->table[bucket]->prev = NULL;
        }
    }

    if(entry->next != NULL)
    {
        entry->next->prev = entry->prev;
        entry->next = NULL;
    } else {
        if(entry->prev != NULL)
        {
            entry->prev->next = NULL;
        }
    }

    // destroy the node and get the data
    hashtable_entry_destroy(entry, data, get_htype_size(htbl->ktype));
    htbl->size--;    
    set_remove(htbl->keys, key);

    return 1;
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
    hashtable_entry* tmp = NULL;

    for(int32_t i = 0; i < set_size(htbl->keys); i++)
    {
        set_value_at(htbl->keys, i, key);
        int32_t bucket = htbl->key_hash(key, htbl->buckets);
        entry = htbl->table[bucket];
        htbl->table[bucket] = NULL; /// unhook the entries from the bucket

        /// The colliding entries will be removed as well 
        /// so check to make sure we do not attempt to remove again.
        if(entry == NULL)
        {
            continue;
        }

        while(entry)
        {
            tmp = entry;
            entry = entry->next;
            hashtable_entry_destroy(tmp, data, dsize);

            if(key_destroy != NULL && *(void **)key != NULL)
            {
                key_destroy(*(void **)key);
            }

            if(value_destroy != NULL && *(void **)data != NULL)
            {
                value_destroy(*(void **)data);
            }
        }
    }

    free(key);
    free(data);
    set_clear(htbl->keys);
    htbl->size = 0;
   
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

        // Assign proper memory sizes.
        // TODO: see if data and key memories can be allocated in a
        // single contigous memory with the malloc above.
        entry->data = malloc(dsize);
        entry->key = malloc(ksize);

        // Make sure next and prev are NULL
        entry->next = NULL;
        entry->prev = NULL;
    }

    entry->key_compare = htbl->key_compare;
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
void hashtable_entry_destroy(hashtable_entry *entry, void* data, size_t dsize)
{
    // In case the data are managed somewhere else
    // We should copy them for proper deallocation
    memcpy(data, entry->data, dsize);

    // Release all malloc'd memory
    free(entry->key);
    free(entry->data);
    free(entry);
}

size_t hashtable_size(hashtable* htbl)
{
    assert(htbl != NULL);
    //printf("hashtable_size: size is %ld\n", htbl->size);
    return htbl->keys->size;
}

int32_t hashtable_empty(hashtable* htbl)
{
    if(htbl == NULL || htbl->size == 0)
    {
        return 1;
    }

    return 0;
}