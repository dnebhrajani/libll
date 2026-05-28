//--------------------------------------------------------------------------------
// Module: libll
// File: lltst_string.c
// Author: Durga V. Nebhrajani
// Description:
//      Test file for a generic linked list library that allows
//      user-specific datatype for payload for string datatype.
// Copyright:
//      Copyright (c) 2026 Durga V. Nebhrajani
//      All rights reserved.
//--------------------------------------------------------------------------------

#include "libll.h"

char *buffer;

// User defined print functions

void print_str(void *data) {
  printf("%s ", *(char **)data);
}

void print_str_str(void *data) {
  sprintf(buffer + strlen(buffer), "%s ", *(char **)data);
}

// User defined compare functions

int32_t compare_str(void *a, void *b) {
  int32_t result = strcmp(*(char **)a, *(char **)b);
  if (result == 0) return 0;
  else if (result < 0) return -1;
  else return 1;
}

char *rand_string(int32_t length, bool special) {
  char *str = malloc(sizeof(char) * length + 1);
  const char set[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  const char special_set[] = "!#$%&()*+,-./:;<=>?@[]^_`{|}~";
  int32_t size_set = special ? sizeof(special_set) - 1 : sizeof(set) - 1;
  const char *active_set = special ? special_set : set;
  for (int32_t i = 0; i < length; ++i) {
    str[i] = active_set[rand() % size_set];
  }
  str[length] = '\0';
  return str;
}


void check_list(ll *list, char *arr[], int32_t length) {
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
    char *value;
    if (val) {
      value = *(char **)val;
    }
    else {
      printf("Linked list data incorrect. Expected %s, got NULL.\n", arr[i]);
      exit(2);
    }
    if (strcmp(value, arr[i]) != 0) {
      printf("Linked list data incorrect. Expected %s, got %s.\n", arr[i], value);
      exit(3);
    }
  }
}

void llsearch_test(ll *myLL, char *arr[], int length) {
  ll *result;
  char *target;
  int32_t first;
  for (int32_t i = 0; i < length; ++i) {
    target = arr[i];
    first = -1;
    for (int32_t j = 0; j < length; ++j) {
      if (strcmp(arr[j], target) == 0) {
        first = j;
        break;
      }
    }
    result = llsearch(myLL, &target, compare_str);
    if (first == -1) {  // impossible because target selected from array
      if (result != NULL) {
        printf("llsearch returned non-NULL for missing value %s.\n", target);
        exit(5);
      }
    }
    else {
      if (result == NULL) {
        printf("llsearch returned null for existing value %s.\n", target);
        exit(6);
      }
      void *data = llgetdata(result);
      if (data == NULL) {
        printf("llsearch returned node with null data for value %s.\n", target);
        exit(7);
      }
      char *value = *(char **)data;
      if (strcmp(value, target) != 0) {
        printf("llsearch returned incorrect node. Expected %s, got %s.\n",
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
        printf("llsearch did not return first occurrence of %s.\n", target);
        exit(9);
      }
    }
  }
  char *missing = "";
  ll *missing_result = llsearch(myLL, &missing, compare_str);
  if (missing_result != NULL) {
    printf("llsearch returned non-null for missing value %s.\n", missing);
    exit(10);
  }
  // check that llsearch returns every occurrence
  ll *start = llgetnext(result);
  for (int32_t k = first + 1; k < length; ++k) {
    if (strcmp(arr[k], target) == 0) {
      ll *next_result = llsearch(start, &target, compare_str);
      if (next_result == NULL) {
        printf("llsearch failed to find later occurrence of %s.\n", target);
        exit(10);
      }
      ll *expected = myLL;
      for (int32_t idx = 0; idx < k; ++idx) {
        expected = llgetnext(expected);
      }
      if (next_result != expected) {
        printf("llsearch returned incorrect later occurrence of %s.\n", target);
        exit(11);
      }
      start = llgetnext(next_result);
    }
  }

  ll *extra = llsearch(start, &target, compare_str);
  if (extra != NULL) {
    printf("llsearch found non-existent extra occurrence of %s.\n", target);
    exit(12);
  }
}

void lldelete_test(ll *myLL, char *arr[], int length) {
  int32_t original_length = length;
  char *original_arr[original_length];
  char *track_arr[original_length];
  int32_t track_length = length;
  for (int32_t i = 0; i < original_length; ++i) {
    original_arr[i] = arr[i];
    track_arr[i] = arr[i];
  }
  for (int32_t i = 0; i < original_length; ++i) {
    char *target = original_arr[i];
    // count current occurrences of target in track_arr
    int32_t occur_count = 0;
    for (int32_t j = 0; j < track_length; ++j) {
      if (strcmp(track_arr[j], target) == 0) {
        occur_count++;
      }
    }
    // delete occurrences one by one
    for (int32_t occ = 0; occ < occur_count; ++occ) {
      int32_t first = -1;
      for (int32_t j = 0; j < track_length; ++j) {
        if (strcmp(track_arr[j], target) == 0) {
          first = j;
          break;
        }
      }
      myLL = lldelete(myLL, &target, compare_str, NULL);
      // remove first occurrence from track_arr
      for (int32_t j = first; j < track_length - 1; ++j) {
        track_arr[j] = track_arr[j + 1];
      }
      track_length--;
      check_list(myLL, track_arr, track_length);
      // verify remaining occurrences still searchable
      int remaining = 0;
      for (int32_t j = 0; j < track_length; ++j) {
        if (strcmp(track_arr[j], target) == 0) {
          remaining = 1;
          break;
        }
      }
      ll *search_result = llsearch(myLL, &target, compare_str);
      if (remaining && search_result == NULL) {
        printf("lldelete removed too many occurrences of %s.\n", target);
        exit(20);
      }
      if (!remaining && search_result != NULL) {
        printf("lldelete failed to remove all occurrences of %s.\n", target);
        exit(21);
      }
      // verify first occurrence deletion 
      if (remaining) {
        int32_t next_first = -1;
        for (int32_t j = 0; j < track_length; ++j) {
          if (strcmp(track_arr[j], target) == 0) {
            next_first = j;
            break;
          }
        }
        ll *expected = myLL;
        for (int32_t j = 0; j < next_first; ++j) {
          expected = llgetnext(expected);
        }
        if (search_result != expected) {
          printf("lldelete did not delete first occurrence of %s correctly.\n",
                 target);
          exit(22);
        }
      }
    }

    // ensure deleting non-existent value changes nothing
    ll *before = myLL;
    myLL = lldelete(myLL, &target, compare_str, NULL);
    check_list(myLL, track_arr, track_length);
    if (before != myLL && track_length > 0) {
      printf("lldelete modified list for non-existent value %s.\n", target);
      exit(24);
    }
    ll *should_be_null = llsearch(myLL, &target, compare_str);
    if (should_be_null != NULL) {
      printf("Value %s still exists after all deletions.\n", target);
      exit(25);
    }
  }
}

void llinsert_test(ll *myLL, char *arr[], int32_t length) {
  int32_t max_inserts = length * 2 + 10;
  char *insert_store[max_inserts];
  int32_t store_idx = 0;
  char *track_arr[length + max_inserts];
  int32_t track_length = length;
  for (int32_t i = 0; i < length; ++i) {
    track_arr[i] = arr[i];
  }
  //before_head = false
  for (int32_t i = 0; i < length; ++i) {
    char *target = arr[i];
    int32_t first = -1;
    for (int32_t j = 0; j < track_length; ++j) {
      if (strcmp(track_arr[j], target) == 0) {
        first = j;
        break;
      }
    }
    if (first == -1) { // impossible as target is chosen from array
      continue;
    }
    insert_store[store_idx] = rand_string((rand() % 10) + 1, true);
    char **new_val = &insert_store[store_idx++];
    myLL = llinsert(myLL, &target, new_val, compare_str, false);
    for (int32_t j = track_length; j > first + 1; --j) {
      track_arr[j] = track_arr[j - 1];
    }
    track_arr[first + 1] = *new_val;
    track_length++;
    check_list(myLL, track_arr, track_length);
    ll *inserted_node = myLL;
    for (int32_t j = 0; j < first + 1; ++j) {
      inserted_node = llgetnext(inserted_node);
    }
    if (strcmp(*(char **)llgetdata(inserted_node), *new_val) != 0) {
      printf("llinsert did not insert %s at correct position.\n", *new_val);
      exit(32);
    }
    ll *before_node = myLL;
    for (int32_t j = 0; j < first; ++j) {
      before_node = llgetnext(before_node);
    }
    if (strcmp(*(char **)llgetdata(before_node), target) != 0) {
      printf("llinsert corrupted predecessor of inserted value %s.\n", *new_val);
      exit(33);
    }
  }

  // before_head = true
  insert_store[store_idx] = rand_string((rand() % 10) + 1, true);
  char **bh_val = &insert_store[store_idx++];
  ll *old_head = myLL;
  myLL = llinsert(myLL, NULL, bh_val, compare_str, true);
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

  if (strcmp(*(char **)llgetdata(myLL), *bh_val) != 0) {
    printf("llinsert before head did not place %s at head.\n", *bh_val);
    exit(34);
  }

  ll *second = llgetnext(myLL);
  if (strcmp(*(char **)llgetdata(second), track_arr[1]) != 0) {
    printf("llinsert before head corrupted second node.\n");
    exit(35);
  }

  // val not found, appends to end
  char *nonexistent = "NONEXISTENT_SENTINEL";
  insert_store[store_idx] = rand_string((rand() % 10) + 1, true);
  char **nf_val = &insert_store[store_idx++];
  myLL = llinsert(myLL, &nonexistent, nf_val, compare_str, false);
  track_arr[track_length] = *nf_val;
  track_length++;
  check_list(myLL, track_arr, track_length);
  ll *tail = myLL;
  while (llgetnext(tail) != NULL) {
    tail = llgetnext(tail);
  }
  if (strcmp(*(char **)llgetdata(tail), *nf_val) != 0) {
    printf("llinsert did not append %s to end for non-existent val.\n", *nf_val);
    exit(36);
  }
  check_list(myLL, track_arr, track_length);
}

void llsplit_test(ll *myLL, char *arr[], int32_t length) {
  char *track_arr[length];
  for (int32_t i = 0; i < length; ++i) {
    track_arr[i] = arr[i];
  }
  // split in the middle
  char *mid_val = arr[length / 2];
  int32_t split_idx = -1;
  for (int32_t i = 0; i < length; ++i) {
    if (strcmp(track_arr[i], mid_val) == 0) {
      split_idx = i;
      break;
    }
  }
  ll *second = llsplit(myLL, &mid_val, compare_str);
  check_list(myLL, track_arr, split_idx);
  check_list(second, track_arr + split_idx, length - split_idx);

  // rebuild
  myLL = NULL;
  for (int32_t i = 0; i < length; ++i) {
    myLL = lladd(myLL, &arr[i]);
  }
  // split at head
  char *head_val = arr[0];
  ll *second_head = llsplit(myLL, &head_val, compare_str);
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
  char *nonexistent = "NONEXISTENT_SENTINEL";
  ll *second_nf = llsplit(myLL, &nonexistent, compare_str);
  if (second_nf != NULL) {
    printf("llsplit returned non-NULL for non-existent val.\n");
    exit(41);
  }
  check_list(myLL, track_arr, length);
}

void llappend_test(ll *myLL, char *arr[], int32_t length) {
  char *track_arr[length * 2];
  for (int32_t i = 0; i < length; ++i) {
    track_arr[i] = arr[i];
  }
  // build second list
  char *arr2[length];
  ll *list2 = NULL;
  for (int32_t i = 0; i < length; ++i) {
    arr2[i] = rand_string((rand() % 10) + 1, false);
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

int compare_str_qsort(const void *a, const void *b) {
  return strcmp(*(char **)a, *(char **)b);
}

void llsort_test(ll *myLL, char *arr[], int32_t length,
                 ll *(*sortfn)(ll *, int (*)(void *, void *))) {
  // build sorted reference array
  char *sorted_arr[length];
  for (int32_t i = 0; i < length; ++i) {
    sorted_arr[i] = arr[i];
  }
  // sorted array for reference
  qsort(sorted_arr, length, sizeof(char *), compare_str_qsort);

  // sorted order correct, no elements lost or gained
  myLL = sortfn(myLL, compare_str);
  check_list(myLL, sorted_arr, length);

  // already sorted input, sort again, verify unchanged
  myLL = sortfn(myLL, compare_str);
  check_list(myLL, sorted_arr, length);

  // all duplicates
  char *dup_arr[length];
  char *dup_str = rand_string((rand() % 10) + 1, false);
  ll *dup_list = NULL;
  for (int32_t i = 0; i < length; ++i) {
    dup_arr[i] = dup_str;
    dup_list = lladd(dup_list, &dup_arr[i]);
  }
  dup_list = sortfn(dup_list, compare_str);
  check_list(dup_list, dup_arr, length);
}

int main(void) {
  ll *myLL = NULL;
  int32_t length = (rand() % 50) + 1;
  char *arr[length];
  int32_t buf_size = 0;
  for (int i = 0; i < length; ++i) {
    arr[i] = rand_string((rand() % 10) + 1, false);
    myLL = lladd(myLL, &arr[i]);
    buf_size += strlen(arr[i]) + 2;
  }
  char *buffer1 = calloc(buf_size + 1, sizeof(char));
  for (int i = 0; i < length; ++i) {
    sprintf(buffer1 + strlen(buffer1), "%s ", arr[i]);
  }

  printf("Testing Suite for String Datatype:\n");
  printf("----------------------------------------\n");
  

  // testing lladd
  printf("Testing lladd\n");
  check_list(myLL, arr, length);
  printf("lladd passed all tests\n");

  // testing llprint
  printf("\n");
  buffer = calloc(buf_size + 1, sizeof(char));
  printf("Testing llprint\n");
  llprint(myLL, print_str_str);
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

