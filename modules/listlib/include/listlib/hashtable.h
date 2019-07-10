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
/** @file hashtable.h */
#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "linkedlist.h"
#include "list.h"
#include "primes.h"
#include "hdbscan/utils.h"
#include "gnulib/gl_array_oset.h"

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
    int32_t buckets;          //! Number of buckets in the table
    int32_t size;             //! The size of table
    enum HTYPES type;         //! The type of the data of the values
    ArrayList* table;         //! An array list for storing linked list pointers.
    gl_oset_t keys;           //! A set of all the keys in the table.
    int32_t collisions;       //! For diagnostic information on the collision
    int32_t max_collisions;   //! The largest number of keys that collided
} hashtable;

/**
 * \brief Initialise a chained hash table.
 * 
 * @param buckets - number of possible locations to hash to
 * @param type    - the type of data
 * @return hashtable* 
 */ 
hashtable* hashtable_init(int32_t buckets, enum HTYPES type);

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
 * \return int32_t 
 */
int32_t hashtable_insert(hashtable* htbl, int32_t key, void* value);

/**
 * @brief Find the linked list located at the key's hash location.
 * 
 * @param htbl 
 * @param key 
 * @return ArrayList* 
 */
ArrayList* hashtable_lookup(hashtable* htbl, int32_t key);

/**
 * @brief Remove all the key and it's associated value from the hashtable
 * 
 * @param htbl
 * @param key
 * @return 
 */ 
int32_t hashtable_remove(hashtable* htbl, int32_t key);

/**
 * @brief Remove all items in the hash table.
 * 
 * @param htbl - the hashtable to clear.
 * @return int32_t 
 */
int32_t hashtable_clear(hashtable* htbl);

/**
 * @brief Delete the hashtable and clean up all the allocated memory.
 * 
 * @param htbl - the hashtable
 * @return int32_t 
 */ 
int32_t hashtable_destroy(hashtable* htbl);


#ifdef __cplusplus
};
#endif
#endif