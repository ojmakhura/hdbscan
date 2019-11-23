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

#include "listlib/set.h"
#include "hdbscan/utils.h"
#include <CUnit/Basic.h>
#include <stdio.h>
#include<time.h> 

/**
 * @brief Initialise the int array list test suite
 * 
 * @return int 
 */
int init_set_suite(void)
{
    return 0;
}

/**
 * @brief Clean the int array list test suite
 * 
 * @return int 
 */
int clean_set_suite(void)
{
    return 0;
}
/********************************************************************
 * IntArrayList tests
 * 
 ********************************************************************/

/**
 * @brief Runing the tests for the int array list
 * 
 */
void set_int_test()
{
    set_t* set = set_init(sizeof(int32_t), int_compare);
    CU_ASSERT_PTR_NOT_NULL(set);
    CU_ASSERT_EQUAL_FATAL(0, set->size);
    CU_ASSERT_PTR_NOT_NULL(set->data);

    int32_t d = 2;
    CU_ASSERT_EQUAL_FATAL(1, set_insert(set, &d));
    CU_ASSERT_EQUAL_FATAL(1, set->size);
    d = -1;
    CU_ASSERT_EQUAL_FATAL(1, set_value_at(set, 0, &d));
    CU_ASSERT_EQUAL_FATAL(2, d);

    d = 34;
    CU_ASSERT_EQUAL_FATAL(1, set_insert(set, &d));
    CU_ASSERT_EQUAL_FATAL(2, set->size);

    d = 2;
    CU_ASSERT_EQUAL_FATAL(0, set_insert(set, &d));
    CU_ASSERT_EQUAL_FATAL(2, set->size);

    d = 5;
    CU_ASSERT_EQUAL_FATAL(1, set_insert(set, &d));
    CU_ASSERT_EQUAL_FATAL(3, set->size);

    d = -45;
    CU_ASSERT_EQUAL_FATAL(1, set_insert(set, &d));
    CU_ASSERT_EQUAL_FATAL(4, set->size);
    printf("\n");
    for(size_t i = 0; i < set_size(set); i++)
    {
        set_value_at(set, i, &d);
        printf("%d ", d);
    }

    d = 2;
    CU_ASSERT_EQUAL_FATAL(1, set_find(set, &d));
    d = -7;
    CU_ASSERT_EQUAL_FATAL(-1, set_find(set, &d));

    d = 5;
    CU_ASSERT_EQUAL_FATAL(1, set_remove(set, &d));
    
    printf("\n");
    set_remove_at(set, 2, &d);

    for(size_t i = 0; i < set_size(set); i++)
    {
        set_value_at(set, i, &d);
        printf("%d ", d);
    }
    
    /*clock_t start, end;
    double cpu_time_used;
     
    start = clock();

    printf("\n");
    srand(time(0)); 
    for(int32_t i = 0; i < 10000; i++) {
        d = rand() % 10000;
        set_insert(set, &d);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("\nInserted in %f seconds\n", cpu_time_used);
    for(size_t i = 0; i < set_size(set); i++)
    {
        set_value_at(set, i, &d);
        printf("%d ", d);
    }*/

    set_delete(set);
    printf("\n********************************************************************************************\n");
}

void set_str_test()
{
    set_t* set = set_init(sizeof(void *), (int32_t (*)(const void *, const void *))strcmp);
    CU_ASSERT_PTR_NOT_NULL(set);
    CU_ASSERT_EQUAL_FATAL(0, set->size);
    CU_ASSERT_PTR_NOT_NULL(set->data);

    char* dt = "Junior";
    CU_ASSERT_EQUAL_FATAL(1, set_insert(set, &dt));
    CU_ASSERT_EQUAL_FATAL(1, set->size);

    dt = "Junior";
    CU_ASSERT_EQUAL_FATAL(0, set_insert(set, &dt));
    CU_ASSERT_EQUAL_FATAL(1, set->size);

    dt = "Michael";
    CU_ASSERT_EQUAL_FATAL(1, set_insert(set, &dt));
    CU_ASSERT_EQUAL_FATAL(2, set->size);

    dt = "Michael";
    CU_ASSERT_EQUAL_FATAL(0, set_insert(set, &dt));
    CU_ASSERT_EQUAL_FATAL(2, set->size);

    dt = "Makhura";
    CU_ASSERT_EQUAL_FATAL(1, set_insert(set, &dt));
    CU_ASSERT_EQUAL_FATAL(3, set->size);

    dt = "Michael";
    CU_ASSERT_EQUAL_FATAL(0, set_insert(set, &dt));
    CU_ASSERT_EQUAL_FATAL(3, set->size);

    printf("\n");
    for(size_t i = 0; i < set->size; i++)
    {
        char *s;
        set_value_at(set, i, &s);
        printf("%s\n", s);
    }

    set_delete(set);
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
    suite = CU_add_suite("Set test", init_set_suite, clean_set_suite);
    if (NULL == suite)
    {
        printf("Could not add the test suite\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
    if ((NULL == CU_add_test(suite, "Test for integer Set", set_int_test)))
    {
        printf("Could not add the set test to the suite\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(suite, "Test for string Set", set_str_test)))
    {
        printf("Could not add the set test to the suite\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}