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
/** @file hashtable.h 
 * 
 * 
 */
#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include "linkedlist.h"
#include "list.h"
#include "set.h"
#include "primes.h"
#include "hdbscan/utils.h"
#include "gnulib/gl_array_oset.h"

typedef struct HTBL_ENTRY
{
    void* key;
    void* data;
    int32_t (*key_compare)(const void *a, const void *b);
    struct HTBL_ENTRY *next;
    struct HTBL_ENTRY *prev;
} hashtable_entry;

/**
 * \struct HASHTABLE_
 * \brief The structure of the hashtable. 
 * 
 * We are implementing this as a
 * chained hash table. The structure of the hashtable is such that
 * the buckects are made up of a pointer ArrayList. The contents 
 * of each bucket are linkedlist's in which each node contains a
 * a key which is an int and an Arraylist value whose content datatype
 * is determined by the HTYPES enum.
 * 
 * \typedef hashtable
 */ 
typedef struct HASHTABLE_
{
    size_t buckets;            //! Number of buckets in the table
    size_t size;               //! The size of table
    enum HTYPES ktype;               //! The type of the data of the keys
    enum HTYPES dtype;               //! The type of the data of the values
    hashtable_entry** table;           //! An array list for storing linked list pointers.
    set_t* keys;                //! A set of all the keys in the table.
    int32_t collisions;         //! For diagnostic information on the collision
    int32_t (*key_compare)(const void *a, const void *b);
    void (*key_deallocate)(void *key);
    int32_t (*key_hash)(void* key, size_t buckets);
} hashtable;

/**
 * @brief Compare hashtable entries using keys
 * 
 * @param a 
 * @param b 
 * @return int32_t 
 */
//int32_t hashtable_entry_compare(const void* a, const void* b);

/**
 * \brief Initialise a chained hash table.
 * 
 * \param buckets - number of possible locations to hash to
 * \param type    - the type of data
 * \return hashtable* 
 */ 
hashtable* hashtable_init(size_t buckets, enum HTYPES ktype, enum HTYPES dtype, int32_t (*compare)(const void *a, const void *b));

/**
 * \brief Initialise a chained hash table with an actual number of buckets.
 * 
 * \param buckets 
 * \param ktype 
 * \param dtype 
 * \param compare 
 * \return hashtable* 
 */
hashtable* hashtable_init_size(size_t buckets, enum HTYPES ktype, enum HTYPES dtype, int32_t (*compare)(const void *a, const void *b));

/**
 * \brief Insert the value into the table. 
 * 
 * The void pointer does not have to
 * survive as long as the table since the data will be copied to the
 * location that was aloocated when the list was being created.
 * 
 * \param htbl 
 * \param key 
 * \param value 
 * \return int32_t 0 if failed, 1 if new entry created, -1 if replaced the data
 */
int32_t hashtable_insert(hashtable* htbl, void* key, void* value);

/**
 * @brief Find the linked list located at the key's hash location.
 * 
 * @param htbl 
 * @param key 
 * @return ArrayList* 
 */
int32_t hashtable_lookup(hashtable* htbl, void* key, void* data);

/**
 * @brief Remove all the key and it's associated value from the hashtable
 * 
 * @param htbl
 * @param key
 * @return 
 */ 
int32_t hashtable_remove(hashtable* htbl, void* key, void* data);

/**
 * @brief Remove all items in the hash table. If the key and value are
 * dynamically allocated, key_destroy and value_destroy functions must
 * be provided to avoid memory leaks.
 * 
 * @param htbl - the hashtable to clear.
 * @param key_destroy - the function to clean up key memory
 * @param value_destroy - the function to clean up data memory
 * @return int32_t 
 */
int32_t hashtable_clear(hashtable* htbl, void (*key_destroy)(void *key), void (*value_destroy)(void *value));

/**
 * @brief Delete the hashtable and clean up all the allocated memory.
 * If the key and value are dynamically allocated, key_destroy and
 * value_destroy functions must be provided to avoid memory leaks.
 * 
 * @param htbl - the hashtable
 * @param key_destroy - the function to clean up key memory
 * @param value_destroy - the function to clean up data memory
 * @return int32_t 
 */ 
int32_t hashtable_destroy(hashtable* htbl, void (*key_destroy)(void *key), void (*value_destroy)(void *value));

/**
 * \brief  get the size of the table
 * 
 */
size_t hashtable_size(hashtable* htbl);

/**
 * @brief Check if the hashtable is empty
 * 
 * @param htbl 
 * @return int32_t 1 if empty and 0 if not
 */
int32_t hashtable_empty(hashtable* htbl);

#ifdef __cplusplus
};
#endif
#endif