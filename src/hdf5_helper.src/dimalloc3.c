/*
** Copyright (C) 2003-2005 Meridian Environmental Technology, Inc.
** All rights reserved
** 
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 
** THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
*
*   2019-08 == Modified by SLJ to free malloc'd memory, stop memory leak
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/*
** dimalloc - allocates an N dimensional array from the heap
**
** A data structure that mimics a compile time multidimensional array:
**
**	double ***array;
**	array = dimalloc(sizeof(double), 3, 100, 20, 1024);
**	array[94][15][954] = 1 / 3;
**	free(array);
**
** You must declare the destination variable with the correct number
** of levels of indirection. That is, a three dimensional array must
** be declares such as:  TYPE ***array;
**
** If there are any problems allocating memory, NULL is returned.
**
** The space can be released with a single free of the return value.
*/

#define DEBUG_DIMALLOC 0

void* dimalloc(size_t size, size_t dimensions, ...) {
    va_list args;
    size_t  bytes = 0;
    size_t  stride;
    size_t* sizes = NULL;
    char*   mem;
    char*   data;
    void**  start;
    void**  base;
    size_t  i,j;

    if (dimensions <= 0) return(NULL);
    
    /* Make an array of the sizes of the dimensions */
    if ((sizes = (size_t *) malloc(dimensions * sizeof(size_t))) == NULL) return(NULL);
    va_start(args, dimensions);
    for (i = 0; i < dimensions; i++) sizes[i] = va_arg(args, size_t);
    va_end(args);
    
    /*
    ** Figure out how much memory we need.
    ** First we calculate how many pointer slots we need, for the first n-1 dimensions.
    */
    stride = 1;
    for (i = 0; i < (dimensions - 1); i++) {
		bytes += stride * sizes[i] * sizeof(void *);
		stride *= sizes[i];
	    if (DEBUG_DIMALLOC) printf("dim %lu is %lu long\n", i, sizes[i]);
    }
    if (DEBUG_DIMALLOC) printf("dim %lu is %lu long\n", i, sizes[i]);
    
    /* Add pigeon holes for actual data slots */
    bytes += stride * sizes[dimensions - 1] * size;

    /* Allocate the memory */
    if (DEBUG_DIMALLOC) printf("allocating %lu bytes\n", bytes);
    mem = malloc(bytes);
    if (mem == NULL) return(NULL);

    /* Now we assign all the pointers TO the pointer slots. */
    stride = 1;
    base = (void **) mem;
    for (i = 0; i < (dimensions - 2); i++) {
		start = base;
		base = base + sizes[i] * stride;
		for (j = 0; j < stride * sizes[i]; j++) {
			start[j] = base + j * sizes[i + 1];
		}
		stride *= sizes[i];
    }

    /* Assign all the pointers to the data */
    start = base;
    
    /* We have a special case if its a one dimensional array */
    if (dimensions > 1) data = (void *) (base + sizes[dimensions - 2] * stride); else data = (void *) base;
    for (j = 0; j < stride * sizes[dimensions - 2]; j++) {
		start[j] = data + j * sizes[dimensions - 1] * size;
    }

	free(sizes);
    return(mem);
}

/*
int main() {
	double ***array;
	array = dimalloc(sizeof(double), 3, 100, 20, 1024);
	
	array[0][0][0] = 1. / 8.;
	array[99][19][1023] = 5. / 8.;
	array[94][15][954] = 6. / 8.;
	
	printf("%f %f %f\n", array[0][0][0], array[99][19][1023], array[94][15][954]);
	free(array);
}
*/

