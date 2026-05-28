//--------------------------------------------------------------------------------
// Module: libll
// File: lltst_int.c
// Author: Durga V. Nebhrajani
// Description:
//      Test file for a generic linked list library that allows
//      user-specific datatype for payload for int32_t datatype.
// Copyright:
//      Copyright (c) 2026 Durga V. Nebhrajani
//      All rights reserved.
//--------------------------------------------------------------------------------

#include "libll.h"

char *buffer;

// User defined print functions

void print_int(void *data) {
  printf("%d ", *(int32_t *)data);
}

void print_int_str(void *data) {
  sprintf(buffer + strlen(buffer), "%d ", *(int32_t *)data);
}

// User defined compare functions
  
int32_t compare_int(void *a, void *b) {
  if (*(int32_t *)a == *(int32_t *)b) return 0;
  else if (*(int32_t *)a < *(int32_t *)b) return -1;
  else return 1;
}

void check_list(ll *list, int32_t arr[], int32_t length) {
  ll *curr = list;

  void *check_arr[length];
  int32_t len = 0;

  // Retrieve list into array
  for (curr = list; curr; curr = llgetnext(curr)) {
    if (len < length) {
      check_arr[len] = llgetdata(curr);
    }
    len++;
  }
  if (len > length) {
    printf("Linked list too long. Expected %d, got %d.\n", length, len);
    exit(1);
  }
  else if (len < length) {
    printf("Linked list too short. Expected %d, got %d.\n", length, len);
    exit(1);
  }
// Check data correctness
  
  for (int32_t i = 0; i < length; ++i) { 
    void *val = check_arr[i];
    int32_t value;
    if (val) {
      value = *(int32_t *)val;
    }
    else {
      printf("Linked list data incorrect. Expected %d, got NULL.\n", arr[i]);
      exit(2);
    }
    if (value != arr[i]) {
      printf("Linked list data incorrect. Expected %d, got %d.\n", arr[i], value);
      exit(3);
    }
  }
}

void llsearch_test(ll *myLL, int arr[], int length) {
  ll *result;
  int32_t target;
  int32_t first;
  for (int32_t i = 0; i < length; ++i) {
    target = arr[i];
    first = -1;
    for (int32_t j = 0; j < length; ++j) {
      if (arr[j] == target) {
        first = j;
        break;
      }
    }
    result = llsearch(myLL, &target, compare_int);
    if (first == -1) {  // impossible because target selected from array
      if (result != NULL) {
        printf("llsearch returned non-NULL for missing value %d.\n", target);
        exit(5);
      }
    }
    else {
      if (result == NULL) {
        printf("llsearch returned null for existing value %d.\n", target);
        exit(6);
      }
      void *data = llgetdata(result);
      if (data == NULL) {
        printf("llsearch returned node with null data for value %d.\n", target);
        exit(7);
      }
      int32_t value = *(int32_t *)data;
      if (value != target) {
        printf("llsearch returned incorrect node. Expected %d, got %d.\n",
               target, value);
        exit(8);
      }
      // verify first occurrence
      ll *curr;
      int32_t idx = 0;
      for (curr = myLL; curr; curr = llgetnext(curr)) {
        if (idx >= first) {
          break;
        }
        idx++;
      }
      if (curr != result) {
        printf("llsearch did not return first occurrence of %d.\n", target);
        exit(9);
      }
    }
  }
  int32_t missing = INT32_MIN;
  ll *missing_result = llsearch(myLL, &missing, compare_int);
  if (missing_result != NULL) {
    printf("llsearch returned non-null for missing value %d.\n", missing);
    exit(10);
  }
  // check that llsearch returns every occurrence
  ll *start = llgetnext(result);
  for (int32_t k = first + 1; k < length; ++k) {
    if (arr[k] == target) {
      ll *next_result = llsearch(start, &target, compare_int);
      if (next_result == NULL) {
        printf("llsearch failed to find later occurrence of %d.\n", target);
        exit(10);
      }
      ll *expected = myLL;
      for (int32_t idx = 0; idx < k; ++idx) {
        expected = llgetnext(expected);
      }
      if (next_result != expected) {
        printf("llsearch returned incorrect later occurrence of %d.\n", target);
        exit(11);
      }
      start = llgetnext(next_result);
    }
  }

  ll *extra = llsearch(start, &target, compare_int);
  if (extra != NULL) {
    printf("llsearch found non-existent extra occurrence of %d.\n", target);
    exit(12);
  }
}

void lldelete_test(ll *myLL, int arr[], int length) {
  int32_t original_length = length;
  int32_t original_arr[original_length];
  int32_t track_arr[original_length];
  int32_t track_length = length;
  for (int32_t i = 0; i < original_length; ++i) {
    original_arr[i] = arr[i];
    track_arr[i] = arr[i];
  }
  for (int32_t i = 0; i < original_length; ++i) {
    int32_t target = original_arr[i];
    // count current occurrences of target in track_arr
    int32_t occur_count = 0;
    for (int32_t j = 0; j < track_length; ++j) {
      if (track_arr[j] == target) {
        occur_count++;
      }
    }
    // delete occurrences one by one
    for (int32_t occ = 0; occ < occur_count; ++occ) {
      int32_t first = -1;
      for (int32_t j = 0; j < track_length; ++j) {
        if (track_arr[j] == target) {
          first = j;
          break;
        }
      }
      myLL = lldelete(myLL, &target, compare_int, NULL);
      // remove first occurrence from track_arr
      for (int32_t j = first; j < track_length - 1; ++j) {
        track_arr[j] = track_arr[j + 1];
      }
      track_length--;
      check_list(myLL, track_arr, track_length);
      // verify remaining occurrences still searchable
      int remaining = 0;
      for (int32_t j = 0; j < track_length; ++j) {
        if (track_arr[j] == target) {
          remaining = 1;
          break;
        }
      }
      ll *search_result = llsearch(myLL, &target, compare_int);
      if (remaining && search_result == NULL) {
        printf("lldelete removed too many occurrences of %d.\n", target);
        exit(20);
      }
      if (!remaining && search_result != NULL) {
        printf("lldelete failed to remove all occurrences of %d.\n", target);
        exit(21);
      }
      // verify first occurrence deletion 
      if (remaining) {
        int32_t next_first = -1;
        for (int32_t j = 0; j < track_length; ++j) {
          if (track_arr[j] == target) {
            next_first = j;
            break;
          }
        }
        ll *expected = myLL;
        for (int32_t j = 0; j < next_first; ++j) {
          expected = llgetnext(expected);
        }
        if (search_result != expected) {
          printf("lldelete did not delete first occurrence of %d correctly.\n",
                 target);
          exit(22);
        }
      }
    }

    // ensure deleting non-existent value changes nothing
    ll *before = myLL;
    myLL = lldelete(myLL, &target, compare_int, NULL);
    check_list(myLL, track_arr, track_length);
    if (before != myLL && track_length > 0) {
      printf("lldelete modified list for non-existent value %d.\n", target);
      exit(24);
    }
    ll *should_be_null = llsearch(myLL, &target, compare_int);
    if (should_be_null != NULL) {
      printf("Value %d still exists after all deletions.\n", target);
      exit(25);
    }
  }
}

void llinsert_test(ll *myLL, int arr[], int32_t length) {
  int32_t max_inserts = length * 2 + 10;
  int32_t insert_store[max_inserts];
  int32_t store_idx = 0;
  int32_t track_arr[length + max_inserts];
  int32_t track_length = length;
  for (int32_t i = 0; i < length; ++i) {
    track_arr[i] = arr[i];
  }

  //before_head = false
  for (int32_t i = 0; i < length; ++i) {
    int32_t target = arr[i];
    int32_t first = -1;
    for (int32_t j = 0; j < track_length; ++j) {
      if (track_arr[j] == target) {
        first = j;
        break;
      }
    }
    if (first == -1) { // impossible as target is chosen from array
      continue;
    }
    insert_store[store_idx] = target + 100 + store_idx;
    int32_t *new_val = &insert_store[store_idx++];

    myLL = llinsert(myLL, &target, new_val, compare_int, false);

    for (int32_t j = track_length; j > first + 1; --j) {
      track_arr[j] = track_arr[j - 1];
    }
    track_arr[first + 1] = *new_val;
    track_length++;

    check_list(myLL, track_arr, track_length);

    ll *search_result = llsearch(myLL, new_val, compare_int);
    if (search_result == NULL) {
      printf("llinsert failed: inserted value %d not found.\n", *new_val);
      exit(31);
    }

    ll *before_node = myLL;
    for (int32_t j = 0; j < first; ++j) {
      before_node = llgetnext(before_node);
    }
    if (*(int32_t *)llgetdata(before_node) != target) {
      printf("llinsert corrupted predecessor of inserted value %d.\n", *new_val);
      exit(32);
    }
  }

  // before_head = true
  insert_store[store_idx] = 1000 + rand() % 1000;
  int32_t *bh_val = &insert_store[store_idx++];
  ll *old_head = myLL;
  myLL = llinsert(myLL, NULL, bh_val, compare_int, true);
  if (myLL != old_head) {
    printf("llinsert before head changed the head pointer.\n");
    exit(33);
  }

  for (int32_t j = track_length; j > 0; --j) {
    track_arr[j] = track_arr[j - 1];
  }
  track_arr[0] = *bh_val;
  track_length++;

  check_list(myLL, track_arr, track_length);

  if (*(int32_t *)llgetdata(myLL) != *bh_val) {
    printf("llinsert before head did not place %d at head.\n", *bh_val);
    exit(34);
  }

  ll *second = llgetnext(myLL);
  if (*(int32_t *)llgetdata(second) != track_arr[1]) {
    printf("llinsert before head corrupted second node.\n");
    exit(35);
  }

  // val not found, appends to end
  int32_t nonexistent = -9999;
  insert_store[store_idx] = 1000 + rand() % 1000;
  int32_t *nf_val = &insert_store[store_idx++];
  myLL = llinsert(myLL, &nonexistent, nf_val, compare_int, false);
  track_arr[track_length] = *nf_val;
  track_length++;
  check_list(myLL, track_arr, track_length);
  ll *tail = myLL;
  while (llgetnext(tail) != NULL) {
    tail = llgetnext(tail);
  }
  if (*(int32_t *)llgetdata(tail) != *nf_val) {
    printf("llinsert did not append %d to end for non-existent val.\n", *nf_val);
    exit(36);
  }
  check_list(myLL, track_arr, track_length);
}

void llsplit_test(ll *myLL, int arr[], int32_t length) {
  int32_t track_arr[length];
  for (int32_t i = 0; i < length; ++i) {
    track_arr[i] = arr[i];
  }
  // split in the middle
  int32_t mid_val = arr[length / 2];
  int32_t split_idx = -1;
  for (int32_t i = 0; i < length; ++i) {
    if (track_arr[i] == mid_val) {
      split_idx = i;
      break;
    }
  }
  ll *second = llsplit(myLL, &mid_val, compare_int);
  check_list(myLL, track_arr, split_idx);
  check_list(second, track_arr + split_idx, length - split_idx);

  // rebuild
  myLL = NULL;
  for (int32_t i = 0; i < length; ++i) {
    myLL = lladd(myLL, &arr[i]);
  }
  // split at head
  int32_t head_val = arr[0];
  ll *second_head = llsplit(myLL, &head_val, compare_int);
  if (second_head != myLL) {
    printf("llsplit at head did not return full list.\n");
    exit(40);
  }
  check_list(second_head, track_arr, length);
  // rebuild
  myLL = NULL;
  for (int32_t i = 0; i < length; ++i) {
    myLL = lladd(myLL, &arr[i]);
  }
  // val not found
  int32_t nonexistent = -9999;
  ll *second_nf = llsplit(myLL, &nonexistent, compare_int);
  if (second_nf != NULL) {
    printf("llsplit returned non-NULL for non-existent val.\n");
    exit(41);
  }
  check_list(myLL, track_arr, length);
}

void llappend_test(ll *myLL, int arr[], int32_t length) {
  int32_t track_arr[length * 2];
  for (int32_t i = 0; i < length; ++i) {
    track_arr[i] = arr[i];
  }
  // build second list
  int32_t arr2[length];
  ll *list2 = NULL;
  for (int32_t i = 0; i < length; ++i) {
    arr2[i] = rand() % 20;
    list2 = lladd(list2, &arr2[i]);
    track_arr[length + i] = arr2[i];
  }
  // normal append, head pointer preserved
  ll *old_head = myLL;
  myLL = llappend(myLL, list2);
  if (myLL != old_head) {
    printf("llappend changed head pointer.\n");
    exit(50);
  }
  check_list(myLL, track_arr, length * 2);
  // rebuild
  myLL = NULL;
  for (int32_t i = 0; i < length; ++i) {
    myLL = lladd(myLL, &arr[i]);
  }
  // list1 is NULL
  ll *result = llappend(NULL, myLL);
  if (result != myLL) {
    printf("llappend with NULL list1 did not return list2.\n");
    exit(51);
  }
  check_list(result, arr, length);
  // list2 is NULL
  result = llappend(myLL, NULL);
  if (result != myLL) {
    printf("llappend with NULL list2 did not return list1.\n");
    exit(52);
  }
  check_list(result, arr, length);
}

int compare_int_qsort(const void *a, const void *b) {
  return *(int32_t *)a - *(int32_t *)b;
}

void llsort_test(ll *myLL, int arr[], int32_t length,
                 ll *(*sortfn)(ll *, int (*)(void *, void *))) {
  // build sorted reference array
  int32_t sorted_arr[length];
  for (int32_t i = 0; i < length; ++i) {
    sorted_arr[i] = arr[i];
  }
  // sorted array for reference
  qsort(sorted_arr, length, sizeof(int32_t), compare_int_qsort);

  // sorted order correct, no elements lost or gained
  myLL = sortfn(myLL, compare_int);
  check_list(myLL, sorted_arr, length);

  // already sorted input, sort again, verify unchanged
  myLL = sortfn(myLL, compare_int);
  check_list(myLL, sorted_arr, length);
}

int main(void) {
  ll *myLL = NULL;
  int32_t length = (rand() % 50) + 1;
  int32_t arr[length];
  char *buffer1 = calloc(length * 11, sizeof(char));
  for (int i = 0; i < length; ++i) {
    arr[i] = rand() % 20;
    myLL = lladd(myLL, &arr[i]);
    sprintf(buffer1 + strlen(buffer1), "%d ", arr[i]);
  }

  printf("Testing Suite for Int Datatype:\n");
  printf("----------------------------------------\n");

  // testing lladd
  printf("Testing lladd\n");
  check_list(myLL, arr, length);
  printf("lladd passed all tests\n");
  
  // testing llprint
  printf("\n");
  buffer = calloc(length * 11, sizeof(char));
  printf("Testing llprint\n");
  llprint(myLL, print_int_str);
  if (strcmp(buffer, buffer1) != 0) {
    printf("llprint printed incorrect list. Expected %s, got %s\n",
                                                   buffer1, buffer);
    exit(4);
  }
  else { // strcmp returned 0, buffers are identical
    printf("llprint passed all tests\n");
  }

  // testing llsearch
  printf("\n");
  printf("Testing llsearch\n");
  llsearch_test(myLL, arr, length);
  printf("llsearch passed all tests\n");

  // testing lldelete (empties list entirely)
  printf("\n");
  printf("Testing lldelete\n");
  lldelete_test(myLL, arr, length);
  printf("lldelete passed all tests\n");

  // testing llinsert (rebuilding myLL from base array arr)
  printf("\n");
  myLL = NULL;
  for (int i = 0; i < length; ++i) {
    myLL = lladd(myLL, &arr[i]);
  }
  printf("Testing llinsert\n");
  llinsert_test(myLL, arr, length);
  printf("llinsert passed all tests\n");

  // testing llsplit (rebuilding myLL from base array as llinsert modified it)
  printf("\n");
  printf("Testing llsplit\n");
  myLL = NULL;
  for (int i = 0; i < length; ++i) {
    myLL = lladd(myLL, &arr[i]);
  }
  llsplit_test(myLL, arr, length);
  printf("llsplit passed all tests\n");

  // testing llappend
  printf("\n");
  printf("Testing llappend\n");
  myLL = NULL;
  for (int i = 0; i < length; ++i) {
    myLL = lladd(myLL, &arr[i]);
  }
  llappend_test(myLL, arr, length);
  printf("llappend passed all tests\n");

  // testing llmergesort
  printf("\n");
  printf("Testing llmergesort\n");
  myLL = NULL;
  for (int i = 0; i < length; ++i) {
    myLL = lladd(myLL, &arr[i]);
  }
  llsort_test(myLL, arr, length, llmergesort);
  printf("llmergesort passed all tests\n");

  // testing llquicksort
  printf("\n");
  printf("Testing llquicksort\n");
  myLL = NULL;
  for (int i = 0; i < length; ++i) {
    myLL = lladd(myLL, &arr[i]);
  }
  llsort_test(myLL, arr, length, llquicksort);
  printf("llquicksort passed all tests\n");

  // testing llqsort
  printf("\n");
  printf("Testing llqsort\n");
  myLL = NULL;
  for (int i = 0; i < length; ++i) {
    myLL = lladd(myLL, &arr[i]);
  }
  llsort_test(myLL, arr, length, llqsort);
  printf("llqsort passed all tests\n");
  
}



