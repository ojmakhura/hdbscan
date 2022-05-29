/*
 * primetests.c
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
 * @file primetests.c
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * 
 * @brief CUnit tests for the primes.h/charge
 *  
 * @version 3.1.6
 * @date 2019-06-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "collections/primes.h"
#include <CUnit/Basic.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Test the function find_prime_less_than
 * 
 */
void test_find_prime_less_than()
{
    printf("find_prime_less_than(10) = %d\n", find_prime_less_than(10));
    printf("find_prime_less_than(29) = %d\n", find_prime_less_than(29));
    printf("find_prime_less_than(19+1) = %d\n", find_prime_less_than(19+1));
    printf("find_prime_less_than(93) = %d\n", find_prime_less_than(93));
    CU_ASSERT_TRUE_FATAL(7 == find_prime_less_than(10));
    CU_ASSERT_TRUE_FATAL(23 == find_prime_less_than(29));
    CU_ASSERT_TRUE_FATAL(87 == find_prime_less_than(93));
}

/**
 * @brief Initialise the test suite
 * 
 * @return int 
 */
int init_suite1(void)
{
    return 0;
}

/**
 * @brief Clean the test suite memory
 * 
 * @return int 
 */
int clean_suite1(void)
{
    return 0;
}

/**
 * @brief The main method to run the tests
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
    suite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
    if (NULL == suite)
    {
        printf("Could not add the test suite\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
    if ((NULL == CU_add_test(suite, "test of find_prime_less_than()", test_find_prime_less_than)))
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