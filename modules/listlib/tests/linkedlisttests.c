/*
 * linkedlisttests.c
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
#include "listlib/linkedlist.h"
#include <CUnit/Basic.h>
#include <stdio.h>

int init_linkedlist_suite(void)
{
    return 0;
}

int clean_linkedlist_suite(void)
{
    return 0;
}
/********************************************************************
 * IntArrayList tests
 * 
 ********************************************************************/
void int_linked_list_test()
{
    // Testing the creation of the arraylist
    linkedlist* list = linkedlist_init(H_INT);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_PTR_NULL(list->head);
    CU_ASSERT_PTR_NULL(list->tail);
    CU_ASSERT_EQUAL_FATAL(list->size, 0);

    // Testing adding the first key-value pair
    int a = 12;
    IntArrayList* al = linkedlist_front_add(list, 5, &a);
    CU_ASSERT_PTR_NOT_NULL(al);
    CU_ASSERT_PTR_NOT_NULL(list->head);
    CU_ASSERT_PTR_NOT_NULL(list->tail);
    CU_ASSERT_EQUAL_FATAL(list->size, 1);
    CU_ASSERT_EQUAL_FATAL(al->size, 1);
    CU_ASSERT_PTR_EQUAL(list->head, list->tail);
    CU_ASSERT_PTR_EQUAL(list->head->value, al);
    
    // Testing the list we just added
    int* d = int_array_list_data(al, 0);
    CU_ASSERT_EQUAL_FATAL(*d, 12);
    int32_t c = list->head->key;
    CU_ASSERT_EQUAL_FATAL(c, 5);

    // Add a second value at the same key
    a = 777;
    al = linkedlist_front_add(list, 5, &a);
    CU_ASSERT_EQUAL_FATAL(list->size, 1);
    CU_ASSERT_EQUAL_FATAL(al->size, 2);

    // Testing adding another key to the front
    a = 7;
    al = linkedlist_front_add(list, 2, &a);
    CU_ASSERT_EQUAL_FATAL(list->size, 2);
    CU_ASSERT_EQUAL_FATAL(al->size, 1);
    CU_ASSERT_PTR_EQUAL(list->head->value, al);

    // Testing adding a list and to the back
    IntArrayList* l2 = int_array_list_init_exact_size(5);
    int_array_list_append(l2, 12);
    al = linkedlist_tail_add(list, 23, &a);
    CU_ASSERT_PTR_EQUAL(list->tail->value, al);
    CU_ASSERT_EQUAL_FATAL(list->size, 3);
    CU_ASSERT_EQUAL_FATAL(al->size, 1);
    CU_ASSERT_EQUAL_FATAL(al->size, ((IntArrayList*)list->tail->value)->size);

    // Testing removing a node
    CU_ASSERT_EQUAL_FATAL(linkedlist_remove(list, 23), 1);
    CU_ASSERT_EQUAL_FATAL(list->size, 2);

    /// Test the lookup
    IntArrayList* a2 = linkedlist_lookup(list, 5, 0);
    CU_ASSERT_EQUAL_FATAL(a2->size, 2);
    d = int_array_list_data(a2, 0);
    CU_ASSERT_EQUAL_FATAL(*d, 12);
    d = int_array_list_data(a2, 1);
    CU_ASSERT_EQUAL_FATAL(*d, 777);

    linkedlist_clear(list);
    CU_ASSERT_EQUAL_FATAL(list->size, 0);

    linkedlist_delete(list);
}

int main()
{
    CU_pSuite suite = NULL;
    /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

    /* add a suite to the registry */
    suite = CU_add_suite("Linked List", init_linkedlist_suite, clean_linkedlist_suite);
    if (NULL == suite)
    {
        printf("Could not add the test suite\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
    if ((NULL == CU_add_test(suite, "Test for linkedlist", int_linked_list_test)))
    {
        printf("Could not add the test to the suite\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}