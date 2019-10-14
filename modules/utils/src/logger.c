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

#include "hdbscan/logger.h"
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

logger* logger_init(logger* log, enum LOGTYPE type)
{
    if(log == NULL) {
        log = malloc(sizeof(logger));
        if(log == NULL) {
            return NULL;
        }

        if(type == LOGFILE) {

            log->log_file = fopen("hdbscan.log", "a");

            if(log->log_file == NULL) {
            printf("Log file not opened."); 
            }
        }

        log->log_type = type;
    }

    return log;
}

void logger_write(logger* log, enum LOGLEVEL level, const char* str) {
    
    assert(log != NULL);
    const char* date = getDateString();
    const char* lvl;

    if(level == FATAL) {
        lvl = "FATAL";
    } else if (level == ERROR) {
        lvl = "ERROR";
    } else if (level == INFO) {
        lvl = "INFO";
    } else {
        lvl = "WARN";
    } 
    size_t sz = strlen(date) + strlen(lvl) + strlen(str) + 10;
    char buffer[sz];
    int n = sprintf(buffer, "%s %s: %s\n", date, lvl, str);
    free(date);

    if(log->log_type == CONSOLE) {
        printf("%s", buffer);
    } else {
        fprintf(log->log_file, "%s", buffer);
    }
}

void logger_close(logger* log) {

    assert(log != NULL);

    if(log->log_type == LOGFILE)
        fclose(log->log_file);

    free(log);
}