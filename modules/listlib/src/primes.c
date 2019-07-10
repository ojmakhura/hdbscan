/*
 * primes.c
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
 * @file primes.c
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * 
 * @brief Functions for handling prime numbers for using the with the hashtable
 * 
 * @version 3.1.6
 * @date 2019-06-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "listlib/primes.h"

int32_t get_prime(int32_t min)
{
    int32_t prime, n_primes = 0;

    for(int i = min + 1; n_primes < INT_MAX; i++)
    {
        for (int j = 0; i < n_primes; i++)
        {
            if (i % primes[j] == 0) {
                goto not_prime;
            }
        }

        // Once we have found a prime, then we need to exit the loop
        prime = i;
        break;
        
        not_prime:
            ;
    }

    return prime;
}

int32_t binary_search_primes(int32_t l, int32_t r, int32_t x) 
{ 
    // If the number is less than the first prime, then return
    // the first prime.
    if(l == 0 && x <= primes[0])
    {
        return primes[0];
    }

    if (r >= l) { 
        int32_t mid = l + (r - l) / 2; 
  
        // If the element is present at the middle 
        // itself or it is greater than the element
        // before the middle but not greater the middle
        if (primes[mid] == x || (primes[mid] >= x && primes[mid-1] < x)) 
            return primes[mid-1];
  
        // If element is smaller than mid, then 
        // it can only be present in left subarray

        if (primes[mid] > x && primes[mid - 1] > x) {
            return binary_search_primes(l, mid - 1, x); 
        } 
  
        // Else the element can only be present 
        // in right subarray 
        return binary_search_primes(mid + 1, r, x); 
    } 
  
    // We reach here when element is not 
    // present in array 
    return -1; 
} 

/**
 * Find the first prime number less than num.
 */ 
int32_t find_prime_less_than(int32_t num)
{
    //if(num > primes[sz-1])
   // {
   //     return get_prime(num);
   // } else {
    return binary_search_primes(0, sz-1, num);
    //}
}