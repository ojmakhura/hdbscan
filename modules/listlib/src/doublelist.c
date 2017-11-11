/*
 * doublelist.c
 *
 *  Created on: 3 Oct 2017
 *      Author: ojmakh
 */

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "listlib/doublelist.h"
#include "listlib/utils.h"


/****************************************************************************************
 * Implementation of array lists
 ****************************************************************************************/


DoubleArrayList* double_array_list_init(){

	return double_array_list_init_size(256);
}

DoubleArrayList* double_array_list_init_size(int32_t size){

	DoubleArrayList* list = (DoubleArrayList*)array_list_init( highestPowerof2(size*2), sizeof(double));

	return list;
}

DoubleArrayList* double_array_list_init_full(int32_t size, double value){

	DoubleArrayList* list = double_array_list_init_size(size);
	double* ldata = (double *)list->data;

	for(int32_t i = 0; i < size; i++){
		ldata[i] = value;
	}
	list->size = size;
	return list;
}

void double_array_list_append(DoubleArrayList* list, double data){

	if(list->size == list->max_size){
		array_list_grow(list);
	}
	double* ldata = (double *)list->data;
	ldata[list->size] = data;
	list->size++;
}


void double_array_list_pop(DoubleArrayList* list){
	list->size--;
}

int32_t double_array_list_search(DoubleArrayList* list, double data){
	int32_t idx = -1;

	double* ldata = (double *)list->data;
	for(int32_t i = 0; i < list->size; i++){
		if(ldata[i] == data){
			idx = i;
			break;
		}
	}

	return idx;
}


int32_t double_array_list_search_sorted(DoubleArrayList* list, double data){

	int32_t idx = -1;
	double* ldata = (double *)list->data;
	double* dptr = bsearch(&data, ldata, sizeof(double), list->size, double_compare);

	if(dptr != NULL){
		idx = dptr - ldata;
	}

	return idx;
}

int32_t double_array_list_remove_at(DoubleArrayList* list, double data, int32_t idx){

	if(idx < 0 || idx > list->size){
		return -1;
	}
	double* ldata = (double *)list->data;
	double* to = ldata + idx;
	double* from = ldata + idx +1;
	int32_t cpysize = list->size - idx -1;

	memcpy(to, from, cpysize * sizeof(double));

	list->size--;
	return idx;
}

int32_t double_array_list_remove(DoubleArrayList* list, double data){
	int32_t idx = double_array_list_search(list, data);
	return double_array_list_remove_at(list, data, idx);
}

void double_array_list_insert_at(DoubleArrayList* list, double data, int32_t index){
	if(list->size == list->max_size){
		array_list_grow(list);
	}

	double* ldata = (double *)list->data;
	double* to = ldata + index + 1;
	double* from = ldata + index;
	int32_t cpysize = list->size - index;

	memcpy(to, from, cpysize * sizeof(double));
	list->size++;

}

void double_array_list_extend(DoubleArrayList* dest, DoubleArrayList* src){
	int32_t cmbsize = dest->size + src->size;

	if(cmbsize >= dest->size){
		array_list_grow(dest);
	}

	double* ddata = (double *)dest->data;
	double* sdata = (double *)src->data;
	if(dest->data != NULL){
		double* to = ddata + dest->size;
		memcpy(to, sdata, (src->size)*sizeof(double));
	}

}

void double_array_list_delete(DoubleArrayList* list){
	array_list_delete(list);
}

double* double_array_list_data(DoubleArrayList* list, int32_t idx){
	double* it = (double *) array_list_value_at(list, idx);
	return it;
}


void double_array_list_sort(DoubleArrayList* list){
	double* ldata = list->data;
	qsort(ldata, list->size, sizeof(double), double_compare);
}

void double_array_list_set_value_at(DoubleArrayList* list, double data, int32_t index){

	double* ldata = (double *)list->data;
	ldata[index] = data;
}

