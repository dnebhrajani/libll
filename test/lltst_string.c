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

void printFloat(void* data) {
  printf("%f ", *(float*)data);
}

void printChar(void* data) {
  printf("%c ", *(char*)data);
}

void printString(void* data) {
  printf("%s ", (char*)data);
}

// User defined compare functions
  
int compareInt(void* a, void* b) {
  if (*(int*)a == *(int*)b) return 0;
  else if (*(int*)a < *(int*)b) return -1;
  else return 1;
}

int compareFloat(void* a, void* b) {
  if (*(float*)a == *(float*)b) return 0;
  else if (*(float*)a < *(float*)b) return -1;
  else return 1;
}

int compareChar(void* a, void* b) {
  if (*(char*)a == *(char*)b) return 0;
  else if (*(char*)a < *(char*)b) return -1;
  else return 1;
}

int compareString(void* a, void* b) {
  int check = strcmp((char*)a, (char*)b);
  return check;
}

int main(void) {
  ll* myLL = NULL;
  char* a = "hello";
  char* b = "my";
  char* c = "name";
  char* d = "is";
  char* e = "durga";

  
  myLL = lladd(myLL, a);
  myLL = lladd(myLL, b);
  myLL = lladd(myLL, c);
  myLL = lladd(myLL, d);
  myLL = lladd(myLL, e);
  llprint(myLL, printString);
  
  
  char* st = "phew"; // search term
  char* st1 = "i'm"; // search term
  char* st2 = "so"; // search term
  
  ll* found = llsearch(myLL, st, compareString);
  if (found) {
    printString(found->data);
    printf("\n");
  }
  else {
    printf("Node %s not found.\n", st);
  }

  myLL = lldelete(myLL, st, compareString);
  //llprint(myLL, printString);

  ll* myLL1 = NULL;
  char* x = "i'm";
  char* y = "so";
  char* z = "tired";
  char* w = "in";
  char* v = "life";
  
  myLL1 = lladd(myLL1, x);
  myLL1 = lladd(myLL1, y);
  myLL1 = lladd(myLL1, z);
  myLL1 = lladd(myLL1, w);
  myLL1 = lladd(myLL1, v);
  llprint(myLL1, printString);

  myLL = llappend(myLL, myLL1);
  llprint(myLL, printString);

  ll* myLL2 = NULL;
  myLL2 = llsplit(myLL, st1, compareString);
  llprint(myLL, printString);
  llprint(myLL2, printString);

  char* u = "very";

  myLL2 = llinsert(myLL2, st2, u, compareString);
  llprint(myLL2, printString);

  ll* myLL1 = NULL;
  int x = 16;
  int y = 18;
  int z = 20;
  int w = 15;
  int v = 17;
  int u = 19;
  
  myLL1 = lladd(myLL1, &x);
  myLL1 = lladd(myLL1, &y);
  myLL1 = lladd(myLL1, &z);
  myLL1 = lladd(myLL1, &w);
  myLL1 = lladd(myLL1, &v);
  myLL1 = lladd(myLL1, &u);
  llprint(myLL1, printInt);

  ll* myLL3 = NULL;
  
  
  //ll* sorted = llmergesort(myLL1, compareInt);
  ll* sorted = llquicksort(myLL1, NULL);
  //ll* sorted = llqsort(myLL1, compareInt);
  llprint(sorted, printInt);
  printf("\n");
  
}



