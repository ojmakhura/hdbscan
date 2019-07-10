/*
 * cuda_distance.cu
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
#include "hdbscan/cuda_distance.h"

__device__
uint TRIANGULAR_D(uint n) {
	return (n * n + n) / 2;
}

__device__
uint deviceFistTriangleNum(uint p) {
	return ceilf((sqrtf(8 * p + 1) - 1) / 2);
}

__global__
void distance_compute_kernel(double* a_in, double *d_out, uint w, uint h, uint d) {
	uint i = blockDim.x * blockIdx.x + threadIdx.x;

	if (i < d) {
		uint r = deviceFistTriangleNum(i+1);
		uint tmp = TRIANGULAR_D(r);
		uint c = i + r - tmp;
		double sum = 0.0, diff = 0.0;

		for (uint k = 0; k < w; k++) {
			double num1 = a_in[r * w + k];
			double num2 = a_in[c * w + k];
			diff = num1 - num2;
			sum += (diff * diff);
		}

		sum = sqrtf(sum);
		d_out[i] = sum;
	}
}