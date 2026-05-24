//--------------------------------------------------------------------------------
// Module: libll
// File: lltst.c
// Author: Durga V. Nebhrajani
// Description:
//      Test for a generic linked list library that allows
//      user-specific datatype for payload.
// Copyright:
//      Copyright (c) 2026 Durga V. Nebhrajani
//      All rights reserved.
//--------------------------------------------------------------------------------


#include "libll.h"

// User defined print functions

void printInt(void* data) {
  printf("%d ", *(int*)data);
}

// User defined compare functions
  
int compareInt(void* a, void* b) {
  if (*(int*)a == *(int*)b) return 0;
  else if (*(int*)a < *(int*)b) return -1;
  else return 1;
}

int main(void) {
  ll* myLL = NULL;

  int x = 16;
  int y = 18;
  int z = 20;
  int w = 15;
  int v = 17;
  int u = 19;
  
  myLL = lladd(myLL, &x);
  myLL = lladd(myLL, &y);
  myLL = lladd(myLL, &z);
  myLL = lladd(myLL, &w);
  myLL = lladd(myLL, &v);
  myLL = lladd(myLL, &u);
  llprint(myLL, printInt);

  //ll* sorted = llmergesort(myLL1, compareInt);
  ll* sorted = llquicksort(myLL, compareInt);

  ll* qsorted = llqsort(myLL, compareInt);
  
  llprint(sorted, printInt);
  llprint(myLL, printInt);
  llprint(qsorted, printInt);
  
  printf("\n");
  
}



