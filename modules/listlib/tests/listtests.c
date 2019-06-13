/*
 * listtests.c
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
#include "listlib/list.h"
#include "listlib/doublelist.h"
#include "listlib/intlist.h"
#include "listlib/longlist.h"
#include <CUnit/Basic.h>
#include <stdio.h>

int init_array_list_suite(void)
{
    return 0;
}

int clean_array_list_suite(void)
{
    return 0;
}
/********************************************************************
 * IntArrayList tests
 * 
 ********************************************************************/
void int_array_list_test()
{
    IntArrayList* list = NULL;
    CU_ASSERT_PTR_NULL(list);
    list = int_array_list_init();
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL_FATAL(list->size, 0);
    CU_ASSERT_EQUAL_FATAL(list->max_size, 256);

    int_array_list_delete(list);
    list = int_array_list_init_exact_size(5);
    CU_ASSERT_EQUAL_FATAL(list->max_size, 5);
    
    // Testing inserting
    int_array_list_append(list, 12);
    int_array_list_append(list, 1);
    int_array_list_append(list, 120);
    int_array_list_append(list, 55);
    int_array_list_append(list, -5);
    CU_ASSERT_EQUAL_FATAL(list->max_size, list->size);
    int32_t* d = int_array_list_data(list, 2);
    CU_ASSERT_EQUAL_FATAL(*d, 120);

    // Testing resizing
    int_array_list_append(list, 87);
    CU_ASSERT_EQUAL_FATAL(6, list->size);
    int_array_list_append(list, 333);
    int_array_list_append(list, -98);
    int_array_list_append(list, 23);
    CU_ASSERT_EQUAL_FATAL(list->max_size, 16);

    int_array_list_pop(list);
    CU_ASSERT_EQUAL_FATAL(8, list->size);

    int_array_list_sort(list);
    d = int_array_list_data(list, 0);
    CU_ASSERT_EQUAL_FATAL(*d, -98);
    int_array_list_delete(list);
}

/**
 * Just testing that we can use the generic ArrayList to
 * safely work with int, long and double. The list created
 * in this test is for storing 
 */ 
void int_ptr_array_list_test()
{
    ArrayList* list = NULL;
    list = array_list_init( 4, sizeof(int32_t *));
    CU_ASSERT_PTR_NOT_NULL(list);

    // Testing inserting integers
    int32_t a1 = 55;
    CU_ASSERT_EQUAL_FATAL(array_list_append(list, &a1), 1);
    CU_ASSERT_EQUAL_FATAL(list->size, 1);
    int32_t *a1_ptr = array_list_value_at(list, 0);
    CU_ASSERT_EQUAL_FATAL(a1, *a1_ptr); 
    CU_ASSERT_EQUAL_FATAL(55, *a1_ptr);

    // Testing inserting doubles
    double d1 = 32.6;
    CU_ASSERT_EQUAL_FATAL(array_list_append(list, &d1), 1);
    CU_ASSERT_EQUAL_FATAL(list->size, 2);
    double *d1_ptr = array_list_value_at(list, 1);
    CU_ASSERT_EQUAL_FATAL(d1, *d1_ptr); 
    CU_ASSERT_EQUAL_FATAL(32.6, *d1_ptr);

    // Testing inserting other lists
    IntArrayList* l2 = int_array_list_init_exact_size(5);
    int_array_list_append(l2, 12);
    int_array_list_append(l2, 1);
    int_array_list_append(l2, 120);
    int_array_list_append(l2, 55);
    int_array_list_append(l2, -5);
    CU_ASSERT_EQUAL_FATAL(array_list_append(list, &l2), 1);
    CU_ASSERT_EQUAL_FATAL(list->size, 3);

    IntArrayList** tmp = array_list_value_at(list, 2);
    IntArrayList* l3 = *tmp;
    CU_ASSERT_EQUAL_FATAL(l3->size, 5);
    CU_ASSERT_PTR_EQUAL(l2, l3);
    int32_t* l3d = array_list_value_at(l3, 4);
    int32_t* l2d = array_list_value_at(l2, 4);
    CU_ASSERT_EQUAL_FATAL(*l2d, *l3d);
}

int main()
{
    CU_pSuite suite = NULL;
    /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

    /* add a suite to the registry */
    suite = CU_add_suite("Array List test", init_array_list_suite, clean_array_list_suite);
    if (NULL == suite)
    {
        printf("Could not add the test suite\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
    if ((NULL == CU_add_test(suite, "Test for IntArrayList", int_array_list_test)))
    {
        printf("Could not add the IntArrayList test to the suite\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(suite, "Test for ArrayList with integers", int_ptr_array_list_test)))
    {
        printf("Could not add the Arraylist test to the suite\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}