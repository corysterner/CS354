///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020-2022 Nawaf Alsrehin based on work by Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Spring 2023
///////////////////////////////////////////////////////////////////////////////

#ifndef __p3Heap_h
#define __p3Heap_h

int   init_heap(int sizeOfRegion);
void  disp_heap();

void* balloc(int size);
int   bfree(void *ptr);
int   coalesce();

void* malloc(size_t size) {
    return NULL;
}

#endif // __p3Heap_h__

