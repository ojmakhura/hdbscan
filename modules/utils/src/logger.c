/**
 * logger.c
 *
 *  Created on: 13 October 2019
 *      Author: junior
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
 * @file logger.c
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 *
 * @brief Logging facility implementation
 *
 * @version
 * @date 2019-10-13
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "logger.h"
#include <assert.h>
#include <time.h>

static char* getDateString() {
    // Initialize and get current time
    time_t t = time( NULL );

    // Allocate space for date string
    char* date = (char*)malloc( 100 );

    // Format the time correctly
    strftime(date, 100, "[%F %T]", localtime(&t));

    return date;
}

void logger_init()
{
    if(log_file == NULL) {
    	
        log_file = fopen("hdbscan.log", "a");

        if(log_file == NULL) {
            printf("Log file not opened.");
        }
    }

}

void logger_write(enum LOGTYPE type, const char* str) {

    assert(log_file != NULL);
    const char* date = getDateString();
    const char* tp;

    if(type == FATAL) {
        tp = "FATAL";
    } else if (type == ERROR) {
        tp = "ERROR";
    } else if (type == INFO) {
        tp = "INFO";
    } else {
        tp = "WARN";
    }
    size_t sz = strlen(date) + strlen(tp) + strlen(str) + 10;
    char buffer[sz];
    int n = sprintf(buffer, "%s %s: %s\n", date, tp, str);
    free(date);

    fprintf(log_file, "%s", buffer);
}

void logger_close() {

    assert(log_file != NULL);
    fclose(log_file);
}
