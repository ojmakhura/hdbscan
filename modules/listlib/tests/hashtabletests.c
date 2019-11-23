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
#include "listlib/list.h"

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
void hash_table_int_test()
{
    //Testing creation of a hashtable
    printf("\n");
    hashtable* htbl = hashtable_init(45, H_INT, H_INT, int_compare);
    CU_ASSERT_PTR_NOT_NULL(htbl);
    CU_ASSERT_PTR_NOT_NULL(htbl->buckets);
    CU_ASSERT_PTR_NOT_NULL(htbl->keys);
    CU_ASSERT_PTR_NOT_NULL(htbl->table);
    CU_ASSERT_EQUAL_FATAL(htbl->buckets, 43);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 0);
    CU_ASSERT_EQUAL_FATAL(htbl->keys->size, 0);
    
    // Test that each bucket has been properly created
    // since we need to access each bucket through the 
    // array_list_value_at() function, we hack the size
    // to be the same as the number of buckets
    for(size_t i = 0; i < htbl->buckets; i++)
    {
        hashtable_entry* tmp = htbl->table[i];
        CU_ASSERT_PTR_NULL_FATAL(tmp);
    }

    // Test adding to the table
    int32_t x = 23;
    int32_t k = 55;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_insert(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(htbl->size, 1);
    printf("size is %d\n", (int32_t)set_size(htbl->keys));
    CU_ASSERT_EQUAL_FATAL(set_size(htbl->keys), 1);
    
    x = 0;     
    CU_ASSERT_EQUAL_FATAL(1, hashtable_lookup(htbl, &k, &x));
    printf("\nk = %d and x = %d\n", k, x);
    CU_ASSERT_EQUAL_FATAL(23, x);
    CU_ASSERT_EQUAL_FATAL(htbl->collisions, 0);

    // Add another value to the  same key
    x = 88;
    CU_ASSERT_EQUAL_FATAL(-1, hashtable_insert(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(23, x);
    CU_ASSERT_EQUAL_FATAL(1, htbl->size);
    CU_ASSERT_EQUAL_FATAL(1, set_size(htbl->keys));
    CU_ASSERT_EQUAL_FATAL(1, hashtable_lookup(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(88, x);

    // Add another value to the  same key
    x = 9;
    CU_ASSERT_EQUAL_FATAL(-1, hashtable_insert(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(88, x);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 1);
    CU_ASSERT_EQUAL_FATAL(set_size(htbl->keys), 1);
    x = 0;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_lookup(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(9, x);

    // Looking for a key that does not exist
    k = 34;
    CU_ASSERT_EQUAL_FATAL(0, hashtable_lookup(htbl, &k, &x));

    // Testing new keys, one of which is 12 which will colide with 55
    x = 1;
    k = 12;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_insert(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(htbl->size, 2);
    CU_ASSERT_EQUAL_FATAL(set_size(htbl->keys), 2);
    CU_ASSERT_EQUAL_FATAL(htbl->collisions, 1); // 12 collides with 55

    k = 12;
    x = 33;
    CU_ASSERT_EQUAL_FATAL(-1, hashtable_insert(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(1, x);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 2);
    CU_ASSERT_EQUAL_FATAL(set_size(htbl->keys), 2);
    CU_ASSERT_EQUAL_FATAL(htbl->collisions, 1);

    x = 0;
    k = 55;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_lookup(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(9, x);

    x = 100;
    k = 2;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_insert(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(htbl->size, 3);
    CU_ASSERT_EQUAL_FATAL(set_size(htbl->keys), 3);
    CU_ASSERT_EQUAL_FATAL(htbl->collisions, 1);

    x = 0;
    k = 55;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_lookup(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(9, x);

    // Add another key
    x = 8;
    k = 5;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_insert(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(htbl->size, 4);
    CU_ASSERT_EQUAL_FATAL(set_size(htbl->keys), 4);
    CU_ASSERT_EQUAL_FATAL(htbl->collisions, 1);

    x = 0;
    k = 55;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_lookup(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(9, x);

    x = 43;
    k = 5;
    CU_ASSERT_EQUAL_FATAL(-1, hashtable_insert(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(8, x);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 4);
    CU_ASSERT_EQUAL_FATAL(set_size(htbl->keys), 4);
    CU_ASSERT_EQUAL_FATAL(htbl->collisions, 1);

    x = 0;
    k = 55;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_lookup(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(9, x);

    // Looking for a key that does exist
    k = 2;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_lookup(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(100, x);

    // test another collision
    x = -2;
    k = 48;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_insert(htbl, &k, &x));
    CU_ASSERT_EQUAL_FATAL(htbl->size, 5);
    CU_ASSERT_EQUAL_FATAL(set_size(htbl->keys), 5);
    CU_ASSERT_EQUAL_FATAL(htbl->collisions, 2);

    printf("\nHash table has %ld elements.\n", htbl->size);
    
    /**
     * @brief Iteration over the hash table
     * 
     */
    for(size_t i = 0; i < set_size(htbl->keys); i++)
    {
        int32_t key;
        set_value_at(htbl->keys, i, &key);
        int32_t value;
        hashtable_lookup(htbl, &key, &value);
        printf("%d -> %d\n", key, value);
    }

    // Test removing a key
    k = 5;
    /////CU_ASSERT_EQUAL_FATAL(1, hashtable_remove(htbl, &k, &x));
    //CU_ASSERT_EQUAL_FATAL(htbl->size, 4);
    //CU_ASSERT_EQUAL_FATAL(set_size(htbl->keys), 4);
    //CU_ASSERT_EQUAL_FATAL(htbl->collisions, 2);
    
    hashtable_destroy(htbl, NULL, NULL);
    printf("\n********************************************************************************************\n");
}

/**
 * @brief Run the hashtable tests
 * 
 */
void hash_table_str_test()
{
    //Testing creation of a hashtable
    printf("\n");
    hashtable* htbl = hashtable_init(45, H_STRING, H_INT, (int32_t (*)(const void *, const void *))strcmp);
    CU_ASSERT_PTR_NOT_NULL(htbl);
    CU_ASSERT_PTR_NOT_NULL(htbl->buckets);
    CU_ASSERT_PTR_NOT_NULL(htbl->keys);
    CU_ASSERT_PTR_NOT_NULL(htbl->table);
    CU_ASSERT_EQUAL_FATAL(htbl->buckets, 43);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 0);
    CU_ASSERT_EQUAL_FATAL(htbl->keys->size, 0);

    char* key = "junior";
    int32_t value = 34;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_insert(htbl, &key, &value));
    CU_ASSERT_EQUAL_FATAL(htbl->size, 1);
    CU_ASSERT_EQUAL_FATAL(set_size(htbl->keys), 1);

    key = "michael";
    value = 7;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_insert(htbl, &key, &value));
    CU_ASSERT_EQUAL_FATAL(htbl->size, 2);
    CU_ASSERT_EQUAL_FATAL(set_size(htbl->keys), 2);

    printf("\nHash table has %ld elements.\n", htbl->size);
    
    /**
     * @brief Iteration over the hash table
     * 
     */
    for(size_t i = 0; i <set_size(htbl->keys); i++)
    {
        char* k = NULL;
        //array_list_value_at(htbl->keys, i, &k);
        set_value_at(htbl->keys, i, &k);
        int32_t v;
        hashtable_lookup(htbl, &k, &v);
        printf("%s -> %d\n", (char*)k, v);
    }

    hashtable_destroy(htbl, NULL, NULL);
    printf("\n********************************************************************************************\n");
}

/**
 * @brief Testing hashtable with int array list
 * 
 */
/*void hash_table_int_list_test()
{
    //Testing creation of a hashtable
    printf("\n");
    hashtable* htbl = hashtable_init(45, H_INT, H_PTR, int_compare);
    CU_ASSERT_PTR_NOT_NULL(htbl);
    CU_ASSERT_PTR_NOT_NULL(htbl->buckets);
    CU_ASSERT_PTR_NOT_NULL(htbl->keys);
    CU_ASSERT_PTR_NOT_NULL(htbl->table);
    CU_ASSERT_EQUAL_FATAL(htbl->buckets, 43);
    CU_ASSERT_EQUAL_FATAL(htbl->size, 0);
    CU_ASSERT_EQUAL_FATAL(htbl->keys->size, 0);

    IntArrayList* list = int_array_list_init_exact_size(5);
    int_array_list_append(list, 23);
    int_array_list_append(list, 23);
    int_array_list_append(list, 2);
    int_array_list_append(list, -1);
    int_array_list_append(list, 45);
    int_array_list_append(list, 13);

    int32_t key = 12;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_insert(htbl, &key, &list));
    CU_ASSERT_EQUAL_FATAL(htbl->size, 1);
    CU_ASSERT_EQUAL_FATAL(set_size(htbl->keys), 1);

    list = NULL;
    list = int_array_list_init_exact_size(5);
    int_array_list_append(list, 65);
    int_array_list_append(list, 88);
    int_array_list_append(list, -9);
    int_array_list_append(list, 12);
    int_array_list_append(list, -12);
    int_array_list_append(list, -36);
    int_array_list_append(list, -9);

    key = 45;
    CU_ASSERT_EQUAL_FATAL(1, hashtable_insert(htbl, &key, &list));
    CU_ASSERT_EQUAL_FATAL(htbl->size, 2);
    CU_ASSERT_EQUAL_FATAL(set_size(htbl->keys), 2);

    /// Replacing the list at key 12
    key = 12;
    list = int_array_list_init_exact_size(3);
    int_array_list_append(list, 2);
    int_array_list_append(list, 4);
    int_array_list_append(list, 6);
    int_array_list_append(list, 8);
    int_array_list_append(list, 10);
    int_array_list_append(list, 12);
    int_array_list_append(list, 14);
    int_array_list_append(list, 16);
    int_array_list_append(list, 18);
    int_array_list_append(list, 20);

    CU_ASSERT_EQUAL_FATAL(-1, hashtable_insert(htbl, &key, &list));
    int32_t v;
    int_array_list_data(list, 0, &v);
    CU_ASSERT_EQUAL_FATAL(23, v);
    int_array_list_data(list, 1, &v);
    CU_ASSERT_EQUAL_FATAL(23, v);
    int_array_list_data(list, 2, &v);
    CU_ASSERT_EQUAL_FATAL(2, v);
    int_array_list_data(list, 3, &v);
    CU_ASSERT_EQUAL_FATAL(-1, v);
    int_array_list_data(list, 4, &v);
    CU_ASSERT_EQUAL_FATAL(45, v);
    int_array_list_data(list, 5, &v);
    CU_ASSERT_EQUAL_FATAL(13, v);
    CU_ASSERT_EQUAL_FATAL(6, list->size);
    int_array_list_delete(list);

    CU_ASSERT_EQUAL_FATAL(htbl->size, 2);
    CU_ASSERT_EQUAL_FATAL(set_size(htbl->keys), 2);


    printf("\nHash table has %ld elements.\n", htbl->size);
    */
    /**
     * @brief Iteration over the hash table
     * 
     */
    /*for(size_t i = 0; i <set_size(htbl->keys); i++)
    {
        set_value_at(htbl->keys, i, &key);
        hashtable_lookup(htbl, &key, &list);
        printf("%d -> [", key);
		int32_t dpointer;

		for(int j = 0; j < list->size; j++){
			int_array_list_data(list, j, &dpointer);
			printf("%d ", dpointer);
		}
		printf("]\n");
    }

    hashtable_destroy(htbl, NULL, (void (*)(void *))int_array_list_delete);
    printf("\n********************************************************************************************\n");
}*/

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
    if ((NULL == CU_add_test(suite, "Test for int:int hashtable", hash_table_int_test)))
    {
        printf("Could not add the test to the suite\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(suite, "Test for str:int hashtable", hash_table_str_test)))
    {
        printf("Could not add the test to the suite\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    /*if ((NULL == CU_add_test(suite, "Test for int:IntArrayList hashtable", hash_table_int_list_test)))
    {
        printf("Could not add the test to the suite\n");
        CU_cleanup_registry();
        return CU_get_error();
    }*/

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    
    return CU_get_error();
}