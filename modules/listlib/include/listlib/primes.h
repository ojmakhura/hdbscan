/*
 * primes.h
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

/** @file primes.h */
#ifndef PRIMES_H_
#define PRIMES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <limits.h>

/**
 * @brief  Find the largest prime number less than num
 * 
 * @param num 
 * @return int32_t 
 */
int32_t find_prime_less_than(int32_t num);

/**
 * @brief Get a prime number larger that the parameter min. This function
 * is relatively simple but not designed for speed. As such, we are
 * assumming the required prime is not too big. Certainly not bigger than
 * the limit of 32 bit integer.
 * 
 * @param min 
 * @return int32_t 
 */
int32_t get_prime(int32_t min);

/**
 * @brief A recursive binary search function. It returns 
 * location of x in given array arr[l..r] is present, otherwise -1
 * 
 * This function does not search for the exact value, rather, it looks
 * the first number less than or equals to x. This search is used when 
 * for initialising the number of buckets in the chained hash table
 * 
 * @param l 
 * @param r 
 * @param x 
 * @return int32_t 
 */
int32_t binary_search_primes(int32_t l, int32_t r, int32_t x) ;

#define sz 200
// Creating an array of prime numbers for creating key hashes
// We only need 120 primes from 29 to 409. If we need more we
// will have to calculate them.
static const int32_t primes[sz] = {
    3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,
    71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,
    173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,
    281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,
    409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,
    541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,
    659,661,673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,
    809,811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,911,919,929,937,
    941,947,953,967,971,977,983,991,997,1009,1013,1019,1021,1031,1033,1039,1049,1051,1061,1063,
    1069,1087,1091,1093,1097,1103,1109,1117,1123,1129,1151,1153,1163,1171,1181,1187,1193,1201,1213,1217
};

#ifdef __cplusplus
};
#endif
#endif