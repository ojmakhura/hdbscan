/**
 * set.c
 *
 *  Created on: 30 Jul 2019
 *      Author: junior
 * 
 * Copyright 2018 Onalenna Junior Makhura
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
 * @file set.c
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * @brief Implementation of set_t based on ArrayList
 * @version 3.1.6
 * @date 2019-07-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "collections/set.h"
#include <string.h>

set_t* set_init(size_t step, int32_t (*compare)(const void *a, const void *b))
{
    return array_list_init(2, step, compare);
}

set_t* set_delete(set_t* _set)
{
    return array_list_delete(_set);
}

void set_clear(set_t* _set)
{
    array_list_clear(_set, 1);
}

int32_t set_insert(set_t* _set, void* data)
{
    size_t low = 0;
    size_t t1;
    size_t high = _set->size;
    if(_set->size > 0) {
        
        do {
            size_t mid = low + (high - low) / 2; /* low <= mid < high */
            t1 = mid * _set->step;
            
            int cmp = _set->compare(_set->data + t1, data);

            if(cmp < 0) {
                low = mid + 1;
            } else if(cmp > 0) {
                high = mid;
            } else {
                return 0;
            }
        } while(low < high);
    }

    return array_list_insert_at(_set, data, low);
}

int32_t set_remove(set_t* _set, void* data)
{
    int64_t d = set_find(_set, data);

    if(d < 0) {
        return -1;
    }

    return set_remove_at(_set, (size_t)d, data);
}

int64_t set_find(set_t *_set, void* data)
{
    return array_list_find(_set, data, 1);
}

int32_t set_value_at(set_t* _set, size_t index, void* data)
{
    return array_list_value_at(_set, index, data);
}

int32_t set_remove_at(set_t* _set, size_t pos, void* data)
{
    return array_list_remove_at(_set, pos, data);
}

size_t set_size(set_t* _set)
{
    return array_list_size(_set);
}
/**
 * @brief 
 * 
 * @param list 
 */
void set_sort(set_t* _set){

	array_list_sort(_set);
}

int32_t set_empty(set_t* _set) {
    return array_list_empty(_set);
}
