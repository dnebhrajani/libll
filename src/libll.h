//--------------------------------------------------------------------------------
// Module: libll
// File: libll.h
// Author: Durga V. Nebhrajani
// Description:
//       Header file for a generic linked list library that allows user-specific
//       datatype for payload.
// Copyright:
//      Copyright (c) 2026 Durga V. Nebhrajani
//      All rights reserved.
//--------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//--------------------------------------------------------------------------------
// Structure:   ll
//
// Description:
//      Represents a single node in a generic linked list.
//
// Members:
//      data:   Generic payload pointer stored within the node.
//      next:   Pointer to the next node in the linked list.
//--------------------------------------------------------------------------------

typedef struct ll_t {
  void* data;
  struct ll_t* next;
} ll;

//--------------------------------------------------------------------------------
// Member functions of the library
//--------------------------------------------------------------------------------

extern ll* lladd(ll*, void*);

extern ll* llinsert(ll*, void*, void*, int (*)(void*, void*));

extern int llprint(ll*, void (*)(void*));

extern ll* llsearch(ll*, void*, int (*)(void*, void*));

extern ll* lldelete(ll*, void*, int (*)(void*, void*), void (*)(void*));

extern ll* llappend(ll*, ll*);

extern ll* llsplit(ll*, void*, int (*)(void*, void*));

extern ll* llmergesort(ll*, int (*)(void*, void*));

extern ll* llquicksort(ll*, int (*)(void*, void*));

extern ll* llqsort(ll*, int (*)(void*, void*));

extern void lldestroy(ll*, void (*)(void*));

