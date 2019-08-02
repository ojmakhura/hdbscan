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

#include "listlib/set.h"
#include <string.h>

set_t* set_init(size_t step, int32_t (*compare)(const void *a, const void *b))
{
    return array_list_init(1, step, compare);
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
    if(set_find(_set, data) > -1)
    {
        return 0;
    }

    int32_t r = array_list_append(_set, data);
    //if(_set->compare != NULL)
    //{
    //    set_sort(_set);
    //}
    return r;
}

int32_t set_remove(set_t* _set, void* data)
{
    int32_t d = set_find(_set, data);
    return set_remove_at(_set, d, data);
}

int32_t set_find(set_t *_set, void* data)
{
    return array_list_find(_set, data);
}

int32_t set_value_at(set_t* _set, int32_t index, void* data)
{
    return array_list_value_at(_set, index, data);
}

int32_t set_remove_at(set_t* _set, int32_t pos, void* data)
{
    return array_list_remove_at(_set, pos, data);
}

int32_t set_size(set_t* _set)
{
    return array_list_size(_set);
}
/**
 * @brief 
 * 
 * @param list 
 */
void set_sort(set_t* _set){

	qsort(_set->data, _set->size, _set->step, _set->compare);
}