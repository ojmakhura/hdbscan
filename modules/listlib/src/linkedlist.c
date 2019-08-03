/*
 * linkedlist.c
 *
 *  Created on: 3 Oct 2017
 *      Author: ojmakh
 * 
 * Copyright 2018 Onalenna Junior Makhura
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
 * @file linkedlist.c
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * 
 * @brief Linked list implementation.
 * 
 * @version 3.1.6
 * @date 2017-10-03
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#include "listlib/linkedlist.h"
#include <string.h>

#include "config.h"
#ifdef USE_OMP
#include <omp.h>
#endif

void linkedlist_node_unhook(linkedlist* list, node* nd);
void arraylist_add_by_type(ArrayList* al, enum HTYPES type, void* value);

/**
 * Initialise a linked list.
 */ 
linkedlist *linkedlist_init(size_t step)
{
    linkedlist *list = (linkedlist *)malloc(sizeof(linkedlist));

    if(list == NULL)
    {
        return NULL;
    }

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->step = step;

    return list;
}

/**
 * @brief Sets the node data based on the type.
 * 
 * @param nd 
 * @param data 
 * @param type 
 */
void node_set_data(node* nd, void* data, size_t step)
{    
    nd->data = malloc(step);
    memcpy(nd->data, data, step);
}

/**
 * @brief Create a new node with the data
 * 
 * @param data 
 * @param type 
 * @return node* 
 */
node* node_creator(void* data, enum HTYPES type)
{
    node* nd = (node *)malloc(sizeof(node));

    if(nd == NULL)
    {
        return NULL;
    }
    
    nd->prev = NULL;
    nd->next = NULL;

    node_set_data(nd, data, type);
    
    return nd;
}

/**
 * @brief Destroy a node and deallocate all memory
 * 
 * @param nd 
 * @param type 
 * @return int32_t 
 */
int32_t node_destroy(node* nd, enum HTYPES type)
{
    if(nd == NULL)
    {
        return 0;
    }
    free(nd->data);
    free(nd);
    nd = NULL;

    return 1;
}

/**
 * Create a list node and insert it at the front
 * 
 */ 
node* linkedlist_node_front_add(linkedlist* list, void* data)
{
    node* nd = node_creator(data, list->step);

    if(nd == NULL)
    {
        return NULL;
    }

    // If the list was not empty, assign the previous head's prev to the new node
    if(list->size > 0)
    {
        list->head->prev = nd;
        nd->next = list->head;
    } else {
        list->tail = nd; // When list is empty, the tail should point to the node too.
    }
    //printf("%ld : %ld\n", list->head, nd);
    list->head = nd;
    //printf("%ld : %ld\n", list->head, nd);
    list->size += 1;

    return nd;
}

/**
 * Create a list node and insert it at the back
 */ 
node* linkedlist_node_tail_add(linkedlist* list, void* data)
{
    node* nd = node_creator(data, list->step);

    if(nd == NULL)
    {
        return NULL;
    }

    // If the list was not empty, assign the previous tails's 
    // next to the new node
    if(list->size > 0)
    {
        list->tail->next = nd; // The node at the end should have next as the current node
        nd->prev = list->tail;
    } else {
        list->head = nd; // When list is empty, the head should point to the node too.
    }

    list->tail = nd; // nd is the new tail
    list->size += 1;

    return nd;
}

/**
 * Add at the front of the list.
 */
int32_t linkedlist_front_add(linkedlist* list, void* data)
{

    node *nd = linkedlist_node_front_add(list, data);

    if(!nd)
    {
        return 0;
    }

    return 1;
}

/**
 * Add at the back of the list. If the key does not exist, crate a new
 * node with the list and add it to the list.
 */
int32_t linkedlist_tail_add(linkedlist* list, void* data)
{
    node *nd = linkedlist_node_tail_add(list, data);

    if(!nd)
    {
        return 0;
    }

    return 1;
}

/**
 * Find the node that contains the key. 
 * Returns NULL if the key does not exist.
 * 
 * @param list - the linked list
 * @param key - the id of the node
 * @param remove - option to remove the node after finding it. 0 means no removal and 1 means removal
 */
node* linkedlist_lookup_helper(linkedlist *list, void* data, int32_t (*d_compare)(const void *a, const void* b))
{
    node* tmp = list->head;

    /// Iterate through the list to find the key
    while(tmp != NULL)
    {
        int32_t r = d_compare(tmp->data, data);
        if(r == 0) {
            break;
        }

        tmp = tmp->next;
    }

    return tmp;
}

/**
 * helper function for unhooking a node from the list.
 */ 
void linkedlist_node_unhook(linkedlist* list, node* nd)
{
    // Dont attempt to unhook a NULL node or from a NULL list
    // or if the node is unlinked
    if(list == NULL || nd == NULL 
                || list->size == 0) 
    {
        return;
    }
    
    // Get the previous node to point to the next node
    if(nd->prev != NULL)
    {
        nd->prev->next = nd->next;
        nd->prev = NULL;
    } else { // this is the first node
        list->head = nd->next;
    }

    // Get the next node to point to the preious node
    if(nd->next != NULL)
    {
        nd->next->prev = nd->prev;
        nd->next = NULL;
    } else { // This is the las node
        list->tail = nd->prev;
    }
    list->size--;
}

/**
 * 
 */ 
void* linkedlist_lookup(linkedlist *list, void* data, int32_t remove, int32_t (*d_compare)(const void *a, const void* b))
{
    node* tmp = linkedlist_lookup_helper(list, data, d_compare);

    // Unhook the node from the list
    if(remove == 1)
    {
        linkedlist_node_unhook(list, tmp);
    }

    return tmp ? tmp->data : NULL;
}

/**
 * \brief Remove the first occurance of data from list
 * 
 */ 
int32_t linkedlist_remove(linkedlist* list, void* data, int32_t (*d_compare)(const void *a, const void* b))
{
    // Find the node with the key
    node* tmp = linkedlist_lookup_helper(list, data, d_compare);

    if(tmp == NULL)
    {
        return 0;
    }

    if(tmp != NULL)
    {
        // Unhook the node from the list
        linkedlist_node_unhook(list, tmp);

        // destroy the node
        node_destroy(tmp, list->step);
    } else {
        return 0;
    }

    return 1;
}

/**
 * Delete the linked list
 * 
 * @param list - the list to remove
 */
void linkedlist_delete(linkedlist* list)
{
    linkedlist_clear(list); /// Clear the list first

    // Free the list memory
    free(list);
    list = NULL;
}

/**
 * Clear the linked list of all the data. We start removing from the
 * tail. We unhook the tail and then delete the node.
 * 
 * @param list - the linkedlist to clear
 */ 
void linkedlist_clear(linkedlist* list)
{
    node* nd = list->head;
    while(nd != NULL)
    {   
        node* tmp = nd;
        nd = nd->next;     
        // Unhook the last node from the list
        linkedlist_node_unhook(list, tmp);
        
        // destroy the node
        node_destroy(tmp, list->step);
    }

    // set the head and tail to NULL
    list->head = NULL;
    list->tail = NULL;
}

/**
 * @brief Find the data at the tail of the list
 * 
 * @param list 
 * @param remove 
 * @return void* 
 */
void* linkedlist_tail(linkedlist *list, int32_t remove)
{
    if(!linkedlist_empty(list))
    {
        return NULL;
    }

    if(list->tail == NULL)
    {
        return NULL;
    }

    return list->tail->data;
}

/**
 * @brief Find the data at the head of the list
 * 
 * @param list 
 * @param remove 
 * @return void* 
 */
void* linkedlist_head(linkedlist *list, int32_t remove){

    if(!linkedlist_empty(list) || list->head == NULL)
    {
        return NULL;
    }

    return list->head->data;
}

/**
 * @brief Is the list empty
 * 
 * @param list 
 * @return int32_t 1 if not empty and 0 if empty
 */
int32_t linkedlist_empty(linkedlist* list)
{
    if(list == NULL || list->size <= 0)
    {
        return 0;
    }

    return 1;
}


/**
 * @brief 
 * 
 * @param list 
 * @param pos 
 * @param data 
 * @return int32_t 
 */
int32_t linkedlist_value_at(linkedlist *list, int32_t pos, void* data)
{
    if(pos >= list->size)
    {
        return -1;
    }

    int32_t i = 0;
    node* tmp = list->head;
    while(tmp != NULL)
    {
        if(i == pos)
        {
            memcpy(data, tmp->data, list->step);
            return i;    
        }

        tmp = tmp->next;
        i++;
    }

    return -1;
}
