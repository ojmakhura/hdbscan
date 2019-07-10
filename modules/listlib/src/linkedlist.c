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

void linkedlist_node_unhook(linkedlist* list, node* nd);
void arraylist_add_by_type(ArrayList* al, enum HTYPES type, void* value);

/**
 * Initialise a linked list.
 */ 
linkedlist *linkedlist_init(enum HTYPES type)
{
    linkedlist *list = (linkedlist *)malloc(sizeof(linkedlist));

    if(list == NULL)
    {
        return NULL;
    }

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->type = type;

    return list;
}

/**
 * Create a new node with the key and value
 */
node* node_creator(int32_t key, void* value)
{
    node* nd = (node *)malloc(sizeof(node));

    if(nd == NULL)
    {
        return NULL;
    }
    
    nd->key = key;
    nd->prev = NULL;
    nd->next = NULL;
    nd->value = value;
    
    return nd;
}

/**
 * Destroy a node and deallocate all memory
 */ 
int32_t node_destroy(node* nd, enum HTYPES type)
{
    if(nd == NULL)
    {
        return 0;
    }

    if(type == H_INT)
    {
        IntArrayList* lst = (IntArrayList *)nd->value;
        int_array_list_delete(lst);
    } else if(type == H_DOUBLE)
    {
        DoubleArrayList* lst = (DoubleArrayList *)nd->value;
        double_array_list_delete(lst);
    } else if(type == H_LONG)
    {
        LongArrayList* lst = (LongArrayList *)nd->value;
        double_array_list_delete(lst);
    } else if(type == H_VOID)
    {
        ArrayList* lst = (ArrayList *)nd->value;
        array_list_delete(lst);
    } 

    free(nd);
    nd = NULL;

    return 1;
}

/**
 * Create a list node and inser it at the front
 * 
 */ 
node* linkedlist_node_front_add(linkedlist* list, int32_t key, void* value)
{
    node* nd = node_creator(key, value);

    if(nd == NULL)
    {
        return NULL;
    }

    nd->prev = NULL; // inserting at the head, nd->prev should be NULL

    // If the list was not empty, assign the previous head's prev to the new node
    if(list->size > 0)
    {
        list->head->prev = nd;
        nd->next = list->head;
    } else {
        list->tail = nd; // When list is empty, the tail should point to the node too.
    }
    list->head = nd;
    list->size += 1;

    return nd;
}

/**
 * Create a list node and insert it at the back
 * 
 */ 
node* linkedlist_node_tail_add(linkedlist* list, int32_t key, void* value)
{
    node* nd = node_creator(key, value);

    if(nd == NULL)
    {
        return NULL;
    }

    nd->next = NULL; // inserting at the end, nd->next should be NULL

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
 * @brief Add an item to the array list based on the type. Since we are using 
 * printive data type pointers, and their respective array lists take 
 * actual values, we do not to maintain the values allowing for temporary
 * values. 
 * 
 * @param al 
 * @param type 
 * @param value 
 */
void arraylist_add_by_type(ArrayList* al, enum HTYPES type, void* value)
{
    if(type == H_INT){
        int32_t* tmp = (int32_t *)value;
        int_array_list_append(al, *tmp);
    } else if(type == H_DOUBLE)
    {
        double* tmp = (double *)value;
        double_array_list_append(al, *tmp);
    } else if(type == H_LONG)
    {
        long* tmp = (long *)value;
        long_array_list_append(al, *tmp);
    } else if(type == H_VOID)
    {
        array_list_append(al, value);
    } 
}

/**
 * Add at the front of the list. If the key does not exist, create a new
 * node with the list and add it to the list.
 */
ArrayList* linkedlist_front_add(linkedlist* list, int32_t key, void* value)
{
    node *nd = linkedlist_lookup_helper(list, key);

    // If the key does not already exist
    // we create new lists and add them to 
    // the linked list. 
    if(nd == NULL)
    {
        if(list->type == H_INT){
            IntArrayList* al = int_array_list_init();
            nd = linkedlist_node_front_add(list, key, al);
            
        } else if(list->type == H_DOUBLE){
            DoubleArrayList* al = double_array_list_init();
            nd = linkedlist_node_front_add(list, key, al);
        } else if(list->type == H_LONG){
            LongArrayList* al = long_array_list_init();
            nd = linkedlist_node_front_add(list, key, al);
        } else if(list->type == H_VOID){
            ArrayList* al = array_list_init(64, sizeof(void *));
            nd = linkedlist_node_front_add(list, key, al);
        }
    }

    if(nd != NULL)
    {        
        arraylist_add_by_type(nd->value, list->type, value);
        return nd->value;
    }

    return NULL;
}

/**
 * Add at the back of the list. If the key does not exist, crate a new
 * node with the list and add it to the list.
 */
ArrayList* linkedlist_tail_add(linkedlist* list, int32_t key, void* value)
{
    node *nd = linkedlist_lookup_helper(list, key);

    // If the key already exists, 
    if(nd == NULL)
    {
        if(list->type == H_INT){
            IntArrayList* al = int_array_list_init();
            nd = linkedlist_node_tail_add(list, key, al);
        } else if(list->type == H_DOUBLE)
        {
            DoubleArrayList* al = double_array_list_init();
            nd = linkedlist_node_tail_add(list, key, al);
        } else if(list->type == H_LONG)
        {
            LongArrayList* al = long_array_list_init();
            nd = linkedlist_node_tail_add(list, key, al);
        } else if(list->type == H_LONG)
        {
            ArrayList* al = array_list_init(64, sizeof(void *));
            nd = linkedlist_node_tail_add(list, key, al);
        }
    }

    if(nd != NULL)
    {
        arraylist_add_by_type(nd->value, list->type, value);
        return nd->value;  
    }

    return NULL;
}

/**
 * Find the node that contains the key. 
 * Returns NULL if the key does not exist.
 * 
 * @param list - the linked list
 * @param key - the id of the node
 * @param remove - option to remove the node after finding it. 0 means no removal and 1 means removal
 */
node* linkedlist_lookup_helper(linkedlist *list, int32_t key)
{
    node* tmp = list->head;

    /// Iterate through the list to find the key
    while(tmp != NULL)
    {
        if(key == tmp->key)
        {
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
void* linkedlist_lookup(linkedlist *list, int32_t key, int32_t remove)
{
    node* tmp = linkedlist_lookup_helper(list, key);

    // Unhook the node from the list
    if(remove == 1)
    {
        linkedlist_node_unhook(list, tmp);
    }

    return tmp->value;
}

/**
 * Remove nodes from the list given the key and the list
 */ 
int32_t linkedlist_remove(linkedlist* list, int32_t key)
{
    // Find the node with the key
    node* tmp = linkedlist_lookup_helper(list, key);

    if(tmp == NULL)
    {
        return 0;
    }

    //  use a loop because more than one node could share a key
    if(tmp != NULL)
    {
        // Unhook the node from the list
        linkedlist_node_unhook(list, tmp);

        // destroy the node
        node_destroy(tmp, list->type);
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
        node_destroy(tmp, list->type);
    }

    // set the head and tail to NULL
    list->head = NULL;
    list->tail = NULL;
}

