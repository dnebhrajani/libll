//--------------------------------------------------------------------------------
// Module: libll
// File: lltst_char.c
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

void print_char(void *data) {
  printf("%c ", *(char *)data);
}

void print_char_str(void *data) {
  sprintf(buffer + strlen(buffer), "%c ", *(char *)data);
}

// User defined compare functions

int32_t compare_char(void *a, void *b) {
  if (*(char *)a == *(char *)b) return 0;
  else if (*(char *)a < *(char *)b) return -1;
  else return 1;
}

char* rand_string(int32_t length) {
  char* str = malloc(sizeof(char) * length + 1);
  const char set[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  // const char special_set[] = "!#$%&()*+,-./:;<=>?@[]^_`{|}~";
  int32_t size_set = sizeof(set) - 1;
  // int32_t size_special = sizeof(special_set) - 1;
  for (int32_t i = 0; i < length; ++i) {
    /*if (flag) {
      int32_t key = rand() % size_special;
      str[i] = special_set[key];
    }
    else {*/
      int32_t key = rand() % size_set;
      str[i] = set[key]; 
      //}
  }
  str[length] = '\0';
  return str;
}


void check_list(ll *list, char arr[], int32_t length) {
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
    char value;
    if (val) {
      value = *(char *)val;
    }
    else {
      printf("Linked list data incorrect. Expected %c, got NULL.\n", arr[i]);
      exit(2);
    }
    if (value != arr[i]) {
      printf("Linked list data incorrect. Expected %c, got %c.\n", arr[i], value);
      exit(3);
    }
  }
}

void llsearch_test(ll *myLL, char arr[], int length) {
  ll *result;
  char target;
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
    result = llsearch(myLL, &target, compare_char);
    if (first == -1) {  // impossible because target selected from array
      if (result != NULL) {
        printf("llsearch returned non-NULL for missing value %c.\n", target);
        exit(5);
      }
    }
    else {
      if (result == NULL) {
        printf("llsearch returned null for existing value %c.\n", target);
        exit(6);
      }
      void *data = llgetdata(result);
      if (data == NULL) {
        printf("llsearch returned node with null data for value %c.\n", target);
        exit(7);
      }
      char value = *(char *)data;
      if (value != target) {
        printf("llsearch returned incorrect node. Expected %c, got %c.\n",
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
        printf("llsearch did not return first occurrence of %c.\n", target);
        exit(9);
      }
    }
  }
  char missing = '\0';
  ll *missing_result = llsearch(myLL, &missing, compare_char);
  if (missing_result != NULL) {
    printf("llsearch returned non-null for missing value %c.\n", missing);
    exit(10);
  }
  // check that llsearch returns every occurrence
  ll *start = llgetnext(result);
  for (int32_t k = first + 1; k < length; ++k) {
    if (arr[k] == target) {
      ll *next_result = llsearch(start, &target, compare_char);
      if (next_result == NULL) {
        printf("llsearch failed to find later occurrence of %c.\n", target);
        exit(10);
      }
      ll *expected = myLL;
      for (int32_t idx = 0; idx < k; ++idx) {
        expected = llgetnext(expected);
      }
      if (next_result != expected) {
        printf("llsearch returned incorrect later occurrence of %c.\n", target);
        exit(11);
      }
      start = llgetnext(next_result);
    }
  }

  ll *extra = llsearch(start, &target, compare_char);
  if (extra != NULL) {
    printf("llsearch found non-existent extra occurrence of %c.\n", target);
    exit(12);
  }
}

void lldelete_test(ll *myLL, char arr[], int length) {
  int32_t original_length = length;
  char original_arr[original_length];
  char track_arr[original_length];
  int32_t track_length = length;
  for (int32_t i = 0; i < original_length; ++i) {
    original_arr[i] = arr[i];
    track_arr[i] = arr[i];
  }
  for (int32_t i = 0; i < original_length; ++i) {
    char target = original_arr[i];
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
      myLL = lldelete(myLL, &target, compare_char, NULL);
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
      ll *search_result = llsearch(myLL, &target, compare_char);
      if (remaining && search_result == NULL) {
        printf("lldelete removed too many occurrences of %c.\n", target);
        exit(20);
      }
      if (!remaining && search_result != NULL) {
        printf("lldelete failed to remove all occurrences of %c.\n", target);
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
          printf("lldelete did not delete first occurrence of %c correctly.\n",
                 target);
          exit(22);
        }
      }
    }

    // ensure deleting non-existent value changes nothing
    ll *before = myLL;
    myLL = lldelete(myLL, &target, compare_char, NULL);
    check_list(myLL, track_arr, track_length);
    if (before != myLL && track_length > 0) {
      printf("lldelete modified list for non-existent value %c.\n", target);
      exit(24);
    }
    ll *should_be_null = llsearch(myLL, &target, compare_char);
    if (should_be_null != NULL) {
      printf("Value %c still exists after all deletions.\n", target);
      exit(25);
    }
  }
}

void llinsert_test(ll *myLL, char arr[], int32_t length) {
  int32_t max_inserts = length * 2 + 10;
  char insert_store[max_inserts];
  int32_t store_idx = 0;
  char track_arr[length + max_inserts];
  int32_t track_length = length;
  for (int32_t i = 0; i < length; ++i) {
    track_arr[i] = arr[i];
  }
  const char special_set[] = "#$%&()*+,-./:;<=>?@[]^_`{|}~";
  int32_t size_special = sizeof(special_set) - 1;
  //before_head = false
  for (int32_t i = 0; i < length; ++i) {
    char target = arr[i];
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

    insert_store[store_idx] = special_set[rand() % size_special];
    char *new_val = &insert_store[store_idx++];

    myLL = llinsert(myLL, &target, new_val, compare_char, false);

    for (int32_t j = track_length; j > first + 1; --j) {
      track_arr[j] = track_arr[j - 1];
    }
    track_arr[first + 1] = *new_val;
    track_length++;

    check_list(myLL, track_arr, track_length);

    ll *search_result = llsearch(myLL, new_val, compare_char);
    if (search_result == NULL) {
      printf("llinsert failed: inserted value %c not found.\n", *new_val);
      exit(31);
    }

    ll *before_node = myLL;
    for (int32_t j = 0; j < first; ++j) {
      before_node = llgetnext(before_node);
    }
    if (*(char *)llgetdata(before_node) != target) {
      printf("llinsert corrupted predecessor of inserted value %c.\n", *new_val);
      exit(33);
    }
  }

  // before_head = true
  insert_store[store_idx] = special_set[rand() % size_special];
  char *bh_val = &insert_store[store_idx++];
  ll *old_head = myLL;
  myLL = llinsert(myLL, NULL, bh_val, compare_char, true);
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

  if (*(char *)llgetdata(myLL) != *bh_val) {
    printf("llinsert before head did not place %c at head.\n", *bh_val);
    exit(34);
  }

  ll *second = llgetnext(myLL);
  if (*(char *)llgetdata(second) != track_arr[1]) {
    printf("llinsert before head corrupted second node.\n");
    exit(35);
  }

  // val not found, appends to end
  char nonexistent = '!';
  insert_store[store_idx] = special_set[rand() % size_special];
  char *nf_val = &insert_store[store_idx++];
  myLL = llinsert(myLL, &nonexistent, nf_val, compare_char, false);
  track_arr[track_length] = *nf_val;
  track_length++;
  check_list(myLL, track_arr, track_length);
  ll *tail = myLL;
  while (llgetnext(tail) != NULL) {
    tail = llgetnext(tail);
  }
  if (*(char *)llgetdata(tail) != *nf_val) {
    printf("llinsert did not append %c to end for non-existent val.\n", *nf_val);
    exit(36);
  }
  check_list(myLL, track_arr, track_length);
}

void llsplit_test(ll *myLL, char arr[], int32_t length) {
  char track_arr[length];
  for (int32_t i = 0; i < length; ++i) {
    track_arr[i] = arr[i];
  }
  // split in the middle
  char mid_val = arr[length / 2];
  int32_t split_idx = -1;
  for (int32_t i = 0; i < length; ++i) {
    if (track_arr[i] == mid_val) {
      split_idx = i;
      break;
    }
  }
  ll *second = llsplit(myLL, &mid_val, compare_char);
  check_list(myLL, track_arr, split_idx);
  check_list(second, track_arr + split_idx, length - split_idx);

  // rebuild
  myLL = NULL;
  for (int32_t i = 0; i < length; ++i) {
    myLL = lladd(myLL, &arr[i]);
  }
  // split at head
  char head_val = arr[0];
  ll *second_head = llsplit(myLL, &head_val, compare_char);
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
  int32_t nonexistent = '!';
  ll *second_nf = llsplit(myLL, &nonexistent, compare_char);
  if (second_nf != NULL) {
    printf("llsplit returned non-NULL for non-existent val.\n");
    exit(41);
  }
  check_list(myLL, track_arr, length);
}

void llappend_test(ll *myLL, char arr[], int32_t length) {
  char track_arr[length * 2];
  for (int32_t i = 0; i < length; ++i) {
    track_arr[i] = arr[i];
  }
  // build second list
  char* arr2 = rand_string(length);
  ll *list2 = NULL;
  for (int32_t i = 0; i < length; ++i) {
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

int compare_char_qsort(const void *a, const void *b) {
  return *(char *)a - *(char *)b;
}

void llsort_test(ll *myLL, char arr[], int32_t length,
                 ll *(*sortfn)(ll *, int (*)(void *, void *))) {
  // build sorted reference array
  char sorted_arr[length];
  for (int32_t i = 0; i < length; ++i) {
    sorted_arr[i] = arr[i];
  }
  // sorted array for reference
  qsort(sorted_arr, length, sizeof(char), compare_char_qsort);

  // sorted order correct, no elements lost or gained
  myLL = sortfn(myLL, compare_char);
  check_list(myLL, sorted_arr, length);

  // already sorted input, sort again, verify unchanged
  myLL = sortfn(myLL, compare_char);
  check_list(myLL, sorted_arr, length);
}

int main(void) {
  ll *myLL = NULL;
  int32_t length = (rand() % 50) + 1;
  char *arr = rand_string(length);
  char *buffer1 = calloc(length * 4, sizeof(char));
  for (int i = 0; i < length; ++i) {
    myLL = lladd(myLL, &arr[i]);
    sprintf(buffer1 + strlen(buffer1), "%c ", arr[i]);
  }

  printf("Testing Suite for Char Datatype:\n");
  printf("----------------------------------------\n");

  // testing lladd
  printf("Testing lladd\n");
  check_list(myLL, arr, length);
  printf("lladd passed all tests\n");
  
  // testing llprint
  printf("\n");
  buffer = calloc(length * 4, sizeof(char));
  printf("Testing llprint\n");
  llprint(myLL, print_char_str);
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
