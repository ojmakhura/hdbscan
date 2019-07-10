/*
 * cuda_distance.h
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
/** @file cuda_distance.h */
#ifndef CUDA_DISTANCE_H_
#define CUDA_DISTANCE_H_

#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

/**
 * @brief 
 * 
 * @param n 
 * @return uint 
 */
uint TRIANGULAR_D(uint n);

/**
 * @brief 
 * 
 * @param p 
 * @return uint 
 */
uint deviceFistTriangleNum(uint p);

/**
 * @brief 
 * 
 * @param a_in 
 * @param d_out 
 * @param w 
 * @param h 
 * @param d 
 */
void distance_compute_kernel(double* a_in, double *d_out, uint w, uint h, uint d);
#endif