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
#include "listlib/hashtable.h"
#include "listlib/list.h"

linkedlist** hashtable_access_location(ArrayList *list, int32_t bucket);

/**
 * 
 */ 
hashtable* hashtable_init(int32_t buckets, enum HTYPES type)
{
    hashtable* htbl = (hashtable *)malloc(sizeof(hashtable));

    if(htbl == NULL)
    {
        return NULL;
    }

    htbl->buckets = find_prime_less_than(buckets);
    htbl->type = type;

    htbl->table = array_list_init(htbl->buckets, sizeof(void *));

    if(htbl->table == NULL)
    {
        free(htbl);
        return NULL;
    }
    htbl->size = 0;
    htbl->collisions = 0;
    htbl->max_collisions = 0;

    // We need to initialise each bucket to an empty linked list
    for(int32_t i = 0; i < htbl->buckets; i++)
    {
        linkedlist* lnk = linkedlist_init(htbl->type);
        array_list_append(htbl->table, &lnk);
    }

    // A hack to keep the hash table as empty even through
    // we added the lists to the buckets. The linked lists
    // themselves are not the data that needs to be counted.
    htbl->table->size = 0;

    // initialise the keys to an empty array ordered set from the gnulib module
    htbl->keys = gl_oset_nx_create_empty (GL_ARRAY_OSET, (gl_setelement_compar_fn) int_compare, NULL);

    return htbl;
}

/**
 * Since the list does not allow access to memor locations beyond the size
 * of the hash table, we need a little hack to allow access to those location.
 * Basically we are bypassing the array_list_value_at() function and going 
 * directly to the data array in the list.
 */
linkedlist** hashtable_access_location(ArrayList *list, int32_t bucket)
{
    /// Should guard against accessing beyond the array memory
    if(bucket > list->max_size)
    {
        return NULL;
    }

    size_t p = bucket * list->step;
    return list->data + p;
}


/**
 * 
 */ 
int32_t hashtable_insert(hashtable* htbl, int32_t key, void* value)
{
    int32_t bucket = key % htbl->buckets; // hash the key
    linkedlist** lnk = hashtable_access_location(htbl->table, bucket);

    if(lnk == NULL)
    {
        *lnk = linkedlist_init(htbl->type);

        if(*lnk == NULL)
        {
            return 0;
        }
        array_list_insert_at(htbl->table, *lnk, bucket);
    }

    // We always add to the tail.
    int32_t s1 = (*lnk)->size;
    IntArrayList* al = linkedlist_tail_add(*lnk, key, value);
    if(al == 0)
    {
        return 0;
    }

    // Size must only be increased if a new node has been added signalling
    // the key was not in the table.
    if(s1 < (*lnk)->size){
        htbl->size++;
        gl_oset_nx_add(htbl->keys, key);

        // Detect collisions by looking at the linked list size.
        // When there are no collisions, the size will be at 0 or 1.
        // Anything above that shows that a collision occured.
        if((*lnk)->size > 1)
        {
            htbl->collisions += 1;
            if((*lnk)->size > htbl->max_collisions)
            {
                htbl->max_collisions = (*lnk)->size;
            }
        }
    }    

    return 1;
}

/**
 * Look for the ArrayList at the key location. First we find
 * the bucket then if it is not empty, we iterate through the
 * nodes to find one that matches the key. If we find the key
 * in one of the nodes, we then return the ArrayList in the
 * node, otherwise we return NULL
 */ 
ArrayList* hashtable_lookup(hashtable* htbl, int32_t key)
{
    int32_t bucket = key % htbl->buckets; // hash the key
    // The linked lists were already initialised so no need to check
    // validity.
    linkedlist** lnk = hashtable_access_location(htbl->table, bucket);
    
    if((*lnk)->size == 0) // If no data has been added to the bucket
    {
        return NULL;
    }

    return linkedlist_lookup(*lnk, key, 0);
}