/*
 * linkedlist.h
 *
 *  Created on: 11 Jun 2019
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
/** @file linkedlist.h */
#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include "listlib/list.h"
#include "hdbscan/utils.h"

/**
 * \struct NODE
 * 
 * @brief The node for each linked list. Since this linked list is
 * designed to keep two pieces of information, the node should hold
 * both pieces.
 */
typedef struct NODE
{
    struct NODE *next;
    struct NODE *prev;
    void *data;
} node; /**\typedef node */

/*****************************************************************************************
 * \struct LINKEDLIST
 * @brief Linked list implementation
 * 
 * ***************************************************************************************/
typedef struct LINKEDLIST
{
    size_t step;
    int32_t size;
    node* head;
    node* tail;
} linkedlist; /**\typedef linkedlist */

/**
 * @brief Initialise a linked list.
 * 
 * @param type 
 * @return linkedlist* 
 */
linkedlist *linkedlist_init(size_t step);

/**
 * @brief Add at the front of the list. 
 * 
 * If the key does not exist, create a new
 * node with the list and add it to the list.
 * 
 * @param list 
 * @param key 
 * @param value 
 * @return node* 
 */
node* linkedlist_node_front_add(linkedlist* list, void* value);

/**
 * @brief Add to the front of the list.
 * 
 * The operation is O(1)
 * 
 * @param list 
 * @param key 
 * @param value 
 * @return void* 
 */
int32_t linkedlist_front_add(linkedlist* list, void* value);

/**
 * @brief Add at the back of the list. 
 * 
 * If the key does not exist, create a new
 * node with the list and add it to the list.
 * 
 * @param list 
 * @param key 
 * @param value 
 * @return node* 
 */
node* linkedlist_node_tail_add(linkedlist* list, void* value);

/**
 * @brief Add to the end of the list
 * 
 * The operation is O(1)
 * 
 * @param list 
 * @param key 
 * @param value 
 * @return ArrayList* 
 */
int32_t linkedlist_tail_add(linkedlist* list, void* value);

/**
 * @brief Find the node that contains the key. 
 * Returns NULL if the key does not exist.
 * 
 * @param list - the linked list
 * @param key - the id of the node
 * @param remove - option to remove the node after finding it. 0 means no removal and 1 means removal
 * @return node* 
 */
node* linkedlist_lookup_helper(linkedlist *list, void* data, int32_t (*d_compare)(const void *a, const void* b));

/**
 * @brief Find the key in the data
 * 
 * @param list 
 * @param key 
 * @param remove 
 * @return void* return the data if found and NULL if not 
 */
void* linkedlist_lookup(linkedlist *list, void* data, int32_t remove, int32_t (*d_compare)(const void *a, const void* b)); 

/**
 * @brief Find the data at the tail of the list
 * 
 * @param list 
 * @param remove 
 * @return void* 
 */
void* linkedlist_tail(linkedlist *list, int32_t remove); 

/**
 * @brief Find the data at the head of the list
 * 
 * @param list 
 * @param remove 
 * @return void* 
 */
void* linkedlist_head(linkedlist *list, int32_t remove); 

/**
 * @brief 
 * 
 * @param list 
 * @param pos 
 * @param data 
 * @return int32_t 
 */
int32_t linkedlist_value_at(linkedlist *list, int32_t pos, void* data); 

/**
 *  @brief Remove the node from the list given the key and destroy it.
 * 
 * @param list 
 * @param key 
 * @return int32_t 
 */
int32_t linkedlist_remove(linkedlist* list, void* data, int32_t (*d_compare)(const void *a, const void* b));

//int32_t linkedlist_remove(linkedlist* list, int32_t key);

/**
 *  @brief Delete the linked list
 * 
 * @param list - the list to remove
 */
void linkedlist_delete(linkedlist* list);

/**
 * @brief Clear the linked list of all the data
 * 
 * @param list - the linkedlist to clear
 */ 
void linkedlist_clear(linkedlist* list);

/**
 * @brief Is the list empty
 * 
 * @param list 
 * @return int32_t 1 if found and 0 if not found
 */
int32_t linkedlist_empty(linkedlist* list);

#ifdef __cplusplus
};
#endif
#endif