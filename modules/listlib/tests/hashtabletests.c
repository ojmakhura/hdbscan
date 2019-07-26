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
 * @file hashtabletests.c
 * 
 * @author Onalenna Junior Makhura (ojmakh@essex.ac.uk)
 * 
 * @brief CUnit tests for the tash table
 * @version 3.1.6
 * @date 2019-06-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <CUnit/Basic.h>
#include <stdio.h>
#include "listlib/hashtable.h"

/**
 * @brief Initialise the hashtable test suite.
 * 
 * @return int 
 */
int init_hashtable_suite(void)
{
    return 0;
}

/**
 * @brief Clean the hashtable test suite.
 * 
 * @return int 
 */
int clean_hashtable_suite(void)
{
    return 0;
}

/**
 * @brief Run the hashtable tests
 * 
 */
void hash_table_test()
{
    //Testing creation of a hashtable
    printf("\n");
    hashtable* htbl = hashtable_init(45, H_INT);
    CU_ASSERT_PTR_NOT_NULL(htbl);
    CU_ASSERT_PTR_NOT_NULL(htbl->buckets);
    CU_ASSERT_PTR_NOT_NULL(htbl->keys);
    CU_ASSERT_PTR_NOT_NULL(htbl->table);
    CU_ASSERT_EQUAL_FATAL(htbl->buckets, 43);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 0);
    CU_ASSERT_EQUAL_FATAL(htbl->table->max_size, 43);
    CU_ASSERT_EQUAL_FATAL(gl_oset_size(htbl->keys), 0);
    CU_ASSERT_EQUAL_FATAL(htbl->table->step, sizeof(void *));
    
    // Test that each bucket has been properly created
    // since we need to access each bucket through the 
    // array_list_value_at() function, we hack the size
    // to be the same as the number of buckets
    htbl->table->size = htbl->buckets;
    for(int32_t i = 0; i < htbl->buckets; i++)
    {
        linkedlist** tmp = array_list_value_at(htbl->table, i);
         CU_ASSERT_PTR_NOT_NULL(*tmp);
         CU_ASSERT_PTR_NULL((*tmp)->head);
         CU_ASSERT_PTR_NULL((*tmp)->tail);
         CU_ASSERT_EQUAL_FATAL((*tmp)->size, 0);
    }
    htbl->table->size = 0;

    // Test adding to the table
    int32_t x = 23;
    hashtable_insert(htbl, 55, &x);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 1);
    CU_ASSERT_EQUAL_FATAL(gl_oset_size(htbl->keys), 1);
    ArrayList* lst = hashtable_lookup(htbl, 55);
    CU_ASSERT_PTR_NOT_NULL(lst);
    CU_ASSERT_EQUAL_FATAL(lst->size, 1);
    CU_ASSERT_EQUAL_FATAL(htbl->collisions, 0);
    CU_ASSERT_EQUAL_FATAL(htbl->max_collisions, 0);

    // Add another value to the  same key
    x = 88;
    hashtable_insert(htbl, 55, &x);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 1);
    CU_ASSERT_EQUAL_FATAL(gl_oset_size(htbl->keys), 1);
    lst = hashtable_lookup(htbl, 55);
    CU_ASSERT_EQUAL_FATAL(lst->size, 2);


    // Add another value to the  same key
    x = 9;
    hashtable_insert(htbl, 55, &x);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 1);
    CU_ASSERT_EQUAL_FATAL(gl_oset_size(htbl->keys), 1);
    lst = hashtable_lookup(htbl, 55);
    CU_ASSERT_EQUAL_FATAL(lst->size, 3);

    // Testing new keys, one of which is 12 which will colide with 55
    x = 1;
    hashtable_insert(htbl, 12, &x);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 2);
    CU_ASSERT_EQUAL_FATAL(gl_oset_size(htbl->keys), 2);
    CU_ASSERT_EQUAL_FATAL(htbl->collisions, 1); // 12 collides with 55
    CU_ASSERT_EQUAL_FATAL(htbl->max_collisions, 2); // two keys have collided

    x = 33;
    hashtable_insert(htbl, 12, &x);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 2);
    CU_ASSERT_EQUAL_FATAL(gl_oset_size(htbl->keys), 2);
    CU_ASSERT_EQUAL_FATAL(htbl->collisions, 1);
    CU_ASSERT_EQUAL_FATAL(htbl->max_collisions, 2);

    x = 100;
    hashtable_insert(htbl, 2, &x);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 3);
    CU_ASSERT_EQUAL_FATAL(gl_oset_size(htbl->keys), 3);
    CU_ASSERT_EQUAL_FATAL(htbl->collisions, 1);
    CU_ASSERT_EQUAL_FATAL(htbl->max_collisions, 2);

    // Add another key
    x = 8;
    hashtable_insert(htbl, 5, &x);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 4);
    CU_ASSERT_EQUAL_FATAL(gl_oset_size(htbl->keys), 4);
    CU_ASSERT_EQUAL_FATAL(htbl->collisions, 1);
    CU_ASSERT_EQUAL_FATAL(htbl->max_collisions, 2);

    x = 43;
    hashtable_insert(htbl, 5, &x);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 4);
    CU_ASSERT_EQUAL_FATAL(gl_oset_size(htbl->keys), 4);
    CU_ASSERT_EQUAL_FATAL(htbl->collisions, 1);
    CU_ASSERT_EQUAL_FATAL(htbl->max_collisions, 2);

    // test another collision
    x = -2;
    hashtable_insert(htbl, 48, &x);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 5);
    CU_ASSERT_EQUAL_FATAL(gl_oset_size(htbl->keys), 5);
    CU_ASSERT_EQUAL_FATAL(htbl->collisions, 2);
    CU_ASSERT_EQUAL_FATAL(htbl->max_collisions, 2);

    printf("Hash table has %d elements.\n", htbl->size);

    /**
     * @brief Iteration over the hash table
     * 
     */
    for(size_t i = 0; i < gl_oset_size(htbl->keys); i++)
    {
        int key;
        gl_oset_value_at(htbl->keys, i, &key);
        lst = hashtable_lookup(htbl, key);
        printf("%d -> [", key);
        for(int j = 0; j < lst->size; j++){
			int32_t *dpointer = int_array_list_data(lst, j);
			printf("%d ", *dpointer);
		}
		printf("]\n");
    }

    // Test removing a key
    hashtable_remove(htbl, 5);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 4);
    CU_ASSERT_EQUAL_FATAL(gl_oset_size(htbl->keys), 4);
    //CU_ASSERT_EQUAL_FATAL(htbl->collisions, 2);
    //CU_ASSERT_EQUAL_FATAL(htbl->max_collisions, 2);
}

/**
 * @brief Run the test suite
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
    suite = CU_add_suite("Hash Table", init_hashtable_suite, clean_hashtable_suite);
    if (NULL == suite)
    {
        printf("Could not add the test suite\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
    if ((NULL == CU_add_test(suite, "Test for hashtable", hash_table_test)))
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