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

/**
 * @file linkedlisttests.c
 * 
 * @author Onalenna Junior Makhura (ojmakh@essex.ac.uk)
 * 
 * @brief 
 * @version 0.1
 * @date 2019-06-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "collections/linkedlist.h"
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
    linkedlist* list = linkedlist_init(sizeof(int32_t));
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_PTR_NULL(list->head);
    CU_ASSERT_PTR_NULL(list->tail);
    CU_ASSERT_EQUAL_FATAL(0, list->size);

    int32_t d = 23;
    CU_ASSERT_EQUAL_FATAL(1, linkedlist_front_add(list, &d));
    CU_ASSERT_EQUAL_FATAL(1, list->size);

    d = -25;
    CU_ASSERT_EQUAL_FATAL(1, linkedlist_tail_add(list, &d));
    CU_ASSERT_EQUAL_FATAL(2, list->size);

    d = 23;
    CU_ASSERT_EQUAL_FATAL(1, linkedlist_front_add(list, &d));
    CU_ASSERT_EQUAL_FATAL(3, list->size);

    d = -123;
    CU_ASSERT_EQUAL_FATAL(1, linkedlist_tail_add(list, &d));
    CU_ASSERT_EQUAL_FATAL(4, list->size);

    d = 0;
    CU_ASSERT_EQUAL_FATAL(1, linkedlist_front_add(list, &d));
    CU_ASSERT_EQUAL_FATAL(5, list->size);

    d = 100;
    CU_ASSERT_EQUAL_FATAL(1, linkedlist_front_add(list, &d));
    CU_ASSERT_EQUAL_FATAL(6, list->size);

    CU_ASSERT_EQUAL_FATAL(1, linkedlist_value_at(list, 1, &d));
    CU_ASSERT_EQUAL_FATAL(0, d);

    CU_ASSERT_EQUAL_FATAL(0, linkedlist_value_at(list, 0, &d));
    CU_ASSERT_EQUAL_FATAL(100, d);

    d = -25;
    int *x;
    
    x = linkedlist_lookup(list, &d, 0, int_compare);
    printf(">>>>>>>>>> %d\n", x ? *x : 0);

    node* nd = list->head;
    printf("\n");
    int32_t i = 0;
    while(nd)
    {

        printf("%d : %d\n", i, *(int32_t *)nd->data);
        nd = nd->next;
        i++;
    }
    
    d = *(int32_t *)linkedlist_head(list, 0);
    CU_ASSERT_EQUAL_FATAL(100, d);

    d = *(int32_t *)linkedlist_tail(list, 0);
    CU_ASSERT_EQUAL_FATAL(-123, d);

    // Test removing
    d = 23;
    CU_ASSERT_EQUAL_FATAL(1, linkedlist_remove(list, &d, int_compare));
    CU_ASSERT_EQUAL_FATAL(5, list->size);

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