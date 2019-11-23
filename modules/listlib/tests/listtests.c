/** 
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
 * @file listtests.c
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * @brief This is a CUnit test file for testing the lsit implementations.
 * The file test each of the void*, int and other primitive datatype
 * lists.
 * 
 * @TODO implements other primitive datatype tests
 * 
 * @version 3.1.6
 * @date 2019-07-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "listlib/list.h"
#include "hdbscan/utils.h"
#include <CUnit/Basic.h>
#include <stdio.h>

/**
 * @brief Initialise the int array list test suite
 * 
 * @return int 
 */
int init_array_list_suite(void)
{
    return 0;
}

/**
 * @brief Clean the int array list test suite
 * 
 * @return int 
 */
int clean_array_list_suite(void)
{
    return 0;
}
/********************************************************************
 * IntArrayList tests
 * 
 ********************************************************************/


/**
 * @brief Testing that we can use the generic ArrayList to
 * safely work with void *, int, long and double. The list created
 * in this test is for storing 
 * 
 */
void int_ptr_array_list_test()
{
    ArrayList* list = NULL;
    CU_ASSERT_PTR_NULL(list);
    list = array_list_init(5, sizeof(int32_t *), int_ptr_compare);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL_FATAL(list->size, 0);
    CU_ASSERT_EQUAL_FATAL(list->max_size, 5);
        
    // Testing inserting
    int32_t* d = malloc(sizeof(int32_t));
    *d = 12;
    array_list_append(list, &d);
    array_list_value_at(list, 0, &d);
    CU_ASSERT_EQUAL_FATAL(*d, 12);

    d = malloc(sizeof(int32_t));
    *d = 1;
    array_list_append(list, &d);
    array_list_value_at(list, 1, &d);
    CU_ASSERT_EQUAL_FATAL(*d, 1);
    
    d = malloc(sizeof(int32_t));
    *d = 120;
    array_list_append(list, &d);
    array_list_value_at(list, 2, &d);
    CU_ASSERT_EQUAL_FATAL(*d, 120);
    
    d = malloc(sizeof(int32_t));
    *d = 55;
    array_list_append(list, &d);
    array_list_value_at(list, 3, &d);
    CU_ASSERT_EQUAL_FATAL(*d, 55);

    int32_t d1 = 1;
    int32_t* d2 = &d1;
    CU_ASSERT_EQUAL_FATAL(1, array_list_find(list, &d2, 0));

    d1 = 120;
    CU_ASSERT_EQUAL_FATAL(2, array_list_find(list, &d2, 0));

    d1 = 999;
    CU_ASSERT_EQUAL_FATAL(-1, array_list_find(list, &d2, 0));
    
    d = malloc(sizeof(int32_t));
    *d = -5;
    array_list_append(list, &d);
    array_list_value_at(list, 4, &d);
    CU_ASSERT_EQUAL_FATAL(*d, -5);
    
    CU_ASSERT_EQUAL_FATAL(list->max_size, list->size);
    array_list_value_at(list, 2, &d);
    CU_ASSERT_EQUAL_FATAL(*d, 120);

    // Testing resizing
    d = malloc(sizeof(int32_t));
    *d = 87;
    array_list_append(list, &d);
    CU_ASSERT_EQUAL_FATAL(6, list->size);

    d = malloc(sizeof(int32_t));
    *d = 333;
    array_list_append(list, &d);

    d = malloc(sizeof(int32_t));
    *d = -98;
    array_list_append(list, &d);
    
    d = malloc(sizeof(int32_t));
    *d = 23;
    array_list_append(list, &d);
    CU_ASSERT_EQUAL_FATAL(list->max_size, 16);
    CU_ASSERT_EQUAL_FATAL(9, list->size);

    array_list_pop(list, &d);
    CU_ASSERT_EQUAL_FATAL(*d, 23);
    CU_ASSERT_EQUAL_FATAL(8, list->size);
    free(d);

    array_list_value_at(list, 7, &d);
    CU_ASSERT_EQUAL_FATAL(*d, -98);
    
    array_list_sort(list);
    printf("\n");
    for(size_t i = 0; i < list->size; i++)
    {
        array_list_value_at(list, i, &d);
        printf("%d ", *d);
    }    
    printf("\n");

    array_list_value_at(list, 0, &d);
    CU_ASSERT_EQUAL_FATAL(*d, -98);

    /// Deallocate dynamically created memory
    for(size_t i = 0; i < list->size; i++)
    {
        array_list_value_at(list, i, &d);
        free(d);
    }    

    array_list_delete(list);
    printf("\n********************************************************************************************\n");
}



void add_list(ArrayList* list, char* d)
{
    size_t ff = list->size;
    array_list_append(list, &d);
    CU_ASSERT_EQUAL_FATAL(ff+1, list->size);
}

void str_array_list_test()
{
    printf("\n");
    ArrayList* list = NULL;
    CU_ASSERT_PTR_NULL(list);
    list = array_list_init(5, sizeof(void *), (int32_t (*)(const void *, const void *))strcmp);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL_FATAL(list->size, 0);
    CU_ASSERT_EQUAL_FATAL(list->max_size, 5);

    //char* d = "Junior";
    
    add_list(list, "Junior");
    add_list(list, "Michael");
    add_list(list, "Junior");
    add_list(list, "Wire");

    printf("\n");
    for(size_t i = 0; i < list->size; i++)
    {
        char* d;
        array_list_value_at(list, i, &d);
        printf("%s\n", d);
    }    
    array_list_delete(list);
    
    printf("\n********************************************************************************************\n");
}

/**
 * @brief Run the tests
 * 
 * @return int 
 */
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

    if ((NULL == CU_add_test(suite, "Test for ArrayList with integers", int_ptr_array_list_test)))
    {
        printf("Could not add the Arraylist test to the suite\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(suite, "Test for ArrayList with strings", str_array_list_test)))
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