//--------------------------------------------------------------------------------
// Module: libll
// File: libll.c
// Author: Durga V. Nebhrajani
// Description:
//      Implementation file for a generic linked list library that allows
//      user-specific datatype for payload.
// Copyright:
//      Copyright (c) 2026 Durga V. Nebhrajani
//      All rights reserved.
//--------------------------------------------------------------------------------

#include "libll.h"

//--------------------------------------------------------------------------------
// Library Semantics:
//      - The library has no knowledge of the payload datatype
//      - NULL payloads are disallowed
//      - Payload ownership remains with the caller unless explicitly
//        freed through lldestroy()
//      - Compare callbacks must follow strcmp semantics:
//          <0 : first < second
//           0 : first == second
//          >0 : first > second
//--------------------------------------------------------------------------------

typedef enum {
    LL_NOTE,
    LL_WARNING,
    LL_ERROR,
    LL_FATAL
} ll_error_level;

typedef struct {
  const char *code;
  const char *message;
  const char  severity;
} ll_error;

static const ll_error LL_ERR_NULL_DATA = {
  "L02",
  "Invalid null data pointer."
};

static const ll_error LL_ERR_NULL_COMPARE = {
  "L03",
  "Invalid null compare function."
};

static const ll_error LL_ERR_NULL_PRINTER = {
  "L04",
  "Invalid null print function."
};

static const ll_error LL_ERR_ALLOC = {
  "M01",
  "Unable to allocate memory."
};


void _printerr(const ll_error *err,
               ll_error_level level,
               const char *func) {

  const char *severity;
  switch(level) {
  case LL_NOTE:
    severity = "NOTE";
    break;
  case LL_WARNING:
    severity = "WARNING";
    break;
  case LL_FATAL:
    severity = "FATAL";
    break;
  default:
    severity = "UNKNOWN";
  }
  fprintf(stderr,
          "[%s] [%s] %s(): %s\n",
          severity,
          err->code,
          func,
          err->message);
}


//--------------------------------------------------------------------------------
// Internal Function: _swap
//
// Description:
//      Swaps two payload pointers.
//
// Parameters:
//      a:   Pointer to first payload pointer.
//      b:   Pointer to second payload pointer.
//
// Returns:
//      None.
//--------------------------------------------------------------------------------

static void _swap(void **a, void **b) {
  void *temp = *a;
  *a = *b;
  *b = temp;
}

//--------------------------------------------------------------------------------
// Function: lladd
//
// Description:
//      Adds a new node containing the provided payload pointer to the
//      end of the linked list. If the provided list is NULL, a new
//      single-node list with provided payload is created and returned.
//
// Parameters:
//      list:   Pointer to the head of the linked list.
//      d:      Generic payload pointer to insert into the list.
//
// Returns:
//      Pointer to the head of the updated linked list.
//
// Error Handling:
//      - Returns original list if payload pointer is NULL.
//      - Terminates program on allocation failure.
//
// Time Complexity:
//      O(n)
//
// Usage Example:
//      int x = 42;
//      list = lladd(list, &x);
//--------------------------------------------------------------------------------

ll *lladd(ll *list, void *d) {
  if (!d) {
    _printerr(&LL_ERR_NULL_DATA, LL_WARNING, "lladd");
    return list;
  }
  ll *i;
  ll *new = (ll *)malloc(sizeof(ll));
  if (!new) {
    _printerr(&LL_ERR_ALLOC, LL_FATAL, "lladd");
    exit(1);
  }
  new->data = d;
  new->next = NULL;

  if (!list) {
    return new;
  }

  else {
    for (i = list; i->next != NULL; i = i->next);
    i->next = new;
    return list;
  }
}

//--------------------------------------------------------------------------------
// Function: llprint
//
// Description:
//      Traverses the linked list and prints each payload using the
//      provided printer function.
//
// Parameters:
//      list:     Pointer to the head of the linked list.
//      printer:  Callback function responsible for printing payload data.
//
// Returns:
//      0 on successful traversal.
//     -1 if list is NULL.
//
// Error Handling:
//      Terminates program if printer callback is NULL.
//
// Time Complexity:
//      O(n)
//
// Usage Example:
//      llprint(list, printInt);
//--------------------------------------------------------------------------------


int llprint(ll *list, void (*printer)(void *)) {
  if (!printer) {
    _printerr(&LL_ERR_NULL_PRINTER, LL_FATAL, "llprint");
    exit(2);
  }
  if (list) {
    for (ll *i = list; i != NULL; i = i->next) {
      if (!i->data) {
        continue;
      }
      printer(i->data);
    }
    return 0; 
  }
  return -1;
}   

//--------------------------------------------------------------------------------
// Function: llsearch
//
// Description:
//      Searches the linked list for the first node containing a payload
//      matching the target value according to the provided compare function.
//
// Parameters:
//      list:      Pointer to the head of the linked list.
//      d:         Pointer to target payload.
//      compare:   User-defined comparison callback.
//
// Returns:
//      Pointer to matching node if found.
//      NULL otherwise.
//
// Error Handling:
//      - Returns NULL if target payload is NULL.
//      - Terminates program if compare callback is NULL.
//
// Time Complexity:
//      O(n)
//
// Usage Example:
//      int target = 25;
//      ll *result = llsearch(list, &target, compareInt);
//--------------------------------------------------------------------------------

ll *llsearch(ll *list, void *d, int (*compare)(void *, void *)) { 
  if (!compare) {
    _printerr(&LL_ERR_NULL_COMPARE, LL_FATAL, "llsearch");
    exit(2);
  }
  if (!d) {
    _printerr(&LL_ERR_NULL_DATA, LL_WARNING, "llsearch");
    return NULL;
  }
  ll *i;
  for (i = list; i != NULL; i = i->next) {
    if (compare(i->data, d) == 0) {
      return i;
    }
  }
  return NULL;
}

//--------------------------------------------------------------------------------
// Function: lldelete
//
// Description:
//      Deletes the first node whose payload matches the provided target
//      value according to the comparison callback.
//
// Parameters:
//      list:      Pointer to the head of the linked list.
//      d:         Pointer to target payload.
//      compare:   User-defined comparison callback.
//
// Returns:
//      Pointer to the head of the updated linked list.
//
// Error Handling:
//      - Returns original list if payload pointer is NULL.
//      - Terminates program if compare callback is NULL.
//
// Notes:
//      Node memory is freed. Payload memory is unfreed.
//
// Time Complexity:
//      O(n)
//
// Usage Example:
//      int target = 25;
//      list = lldelete(list, &target, compareInt);
//--------------------------------------------------------------------------------

ll *lldelete(ll *list, void *d, int (*compare)(void *, void *),
                                void (*destroy)(void *)) {
  if (!compare) {
    _printerr(&LL_ERR_NULL_COMPARE, LL_FATAL, "lldelete");
    exit(2);
  }
  if (!d) {
    _printerr(&LL_ERR_NULL_DATA, LL_WARNING, "lldelete");
    return list;
  }
  if (!list) {
    return NULL;
  }
  // Head matches
  if (compare(list->data, d) == 0) {
    // Single-node list
    if (list->next == NULL) {
      if (destroy) {
        destroy(list->data);
      }
      free(list);
      return NULL;
    }
    ll *temp = list->next;
    list->data = temp->data;
    list->next = temp->next;
    free(temp);
    return list;
  }
  ll *curr;
  ll *prev = list;
  for (curr = list->next; curr != NULL; curr = curr->next) {
    if (compare(curr->data, d) == 0) {
      prev->next = curr->next;
      if (destroy) {
        destroy(curr->data);
      }
      free(curr);
      return list;
    }
    prev = curr;
  }
  return list;
}

//--------------------------------------------------------------------------------
// Function: llinsert
//
// Description:
//      Inserts a new node immediately after the first node whose payload
//      matches the provided target value if before_head flag is not set.
//      If the flag is set, inserts value before the head. If value is not
//      found, inserts at the end of the list. If null list is provided,
//      returns a new single node list with provided insertion data.
//
// Parameters:
//      list:         Pointer to the head of the linked list.
//      val:          Pointer to target payload.
//      d:            Pointer to payload to insert.
//      compare:      User-defined comparison callback.
//      before_head:  Boolean flag indicating insertion
//                    being before head of list 
//
// Returns:
//      Pointer to the head of the updated linked list. 
//
// Error Handling:
//      - Returns original list if payload pointer is NULL.
//      - Terminates program if compare callback is NULL.
//      - Terminates program on allocation failure.
//
// Time Complexity:
//      O(n)
//
// Usage Example:
//      int target = 25;
//      int new_node = 50;
//      list = llinsert(list, &new_node, &target, compareInt);
//--------------------------------------------------------------------------------

ll *llinsert(ll *list, void *val, void *d, int (*compare)(void *, void *),
                                           bool before_head) {
  if (!compare) {
    _printerr(&LL_ERR_NULL_COMPARE, LL_FATAL, "llinsert");
    exit(2);
  }
  if (!d) {
    _printerr(&LL_ERR_NULL_DATA, LL_WARNING, "llinsert");
    return list;
  }
  if (!val && !before_head) {
    _printerr(&LL_ERR_NULL_DATA, LL_WARNING, "llinsert");
    return list;
  }
  if (before_head) {
    ll *new = (ll *)malloc(sizeof(ll));
    if (!new) {
      _printerr(&LL_ERR_ALLOC, LL_FATAL, "llinsert");
      exit(1);
    }
    new->data = list->data;
    new->next = list->next;
    list->data = d;
    list->next = new;
    return list;
  }
  ll *curr;
  ll *prev = NULL;
  for (curr = list; curr != NULL; curr = curr->next) {
    if (compare(curr->data, val) == 0) {
      ll *new = (ll *)malloc(sizeof(ll));
      if (!new) {
        _printerr(&LL_ERR_ALLOC, LL_FATAL, "llinsert");
        exit(1);
      }
      new->data = d;
      new->next = curr->next;
      curr->next = new;
      return list;
    }
    prev = curr;
  }
  // val not found, append to end
  ll *new = (ll *)malloc(sizeof(ll));
  if (!new) {
    _printerr(&LL_ERR_ALLOC, LL_FATAL, "llinsert");
    exit(1);
  }
  new->data = d;
  new->next = NULL;
  if (!prev) {
    return new;
  }
  prev->next = new;
  return list;
}

//--------------------------------------------------------------------------------
// Function: llsplit
//
// Description:
//      Splits the linked list at the first node matching the target
//      value and returns the head of the second resulting list.
//
// Parameters:
//      list:      Pointer to the head of the linked list.
//      d:         Pointer to target payload.
//      compare:   User-defined comparison callback.
//
// Returns:
//      Pointer to the head of the second split list. If value is head, returns
//      head. If val not found, returns NULL.
//
// Error Handling:
//      - Returns original list if payload pointer is NULL.
//      - Terminates program if compare callback is NULL.
//
// Notes:
//      - If the target is the first node, the original list is returned.
//      - The original list structure is modified.
//
// Time Complexity:
//      O(n)
//
// Usage Example:
//      int target = 25;
//      ll *target_list = llsplit(list, &target, compareInt);
//--------------------------------------------------------------------------------

ll *llsplit(ll *list, void *d, int (*compare)(void *, void *)) {
  if (!compare) {
    _printerr(&LL_ERR_NULL_COMPARE, LL_FATAL, "llsplit");
    exit(2);
  }
  if (!d) {
    _printerr(&LL_ERR_NULL_DATA, LL_WARNING, "llsplit");
    return list;
  }
  ll *curr;
  ll *prev = NULL;
  for (curr = list; curr != NULL; curr = curr->next) {
    if (compare(curr->data, d) == 0) {
      if (curr == list) {
        return list;
      }
      else {
        prev->next = NULL;
        return curr;
      }
    }
    prev = curr;
  }
  return NULL;
}

//--------------------------------------------------------------------------------
// Function: llappend
//
// Description:
//      Appends the second linked list to the end of the first linked
//      list.
//
// Parameters:
//      list1:   Pointer to the head of the first linked list.
//      list2:   Pointer to the head of the second linked list.
//
// Returns:
//      Pointer to the head of the combined linked list.
//
// Error Handling:
//      Prevents appending a list to itself.
//
// Time Complexity:
//      O(n)
//
// Usage Example:
//      list1 = llappend(list1, list2);
//--------------------------------------------------------------------------------

ll *llappend(ll *list1, ll *list2) {
  static const ll_error LL_ERR_SELF_APPEND = {
    "L01",
    "Cannot append list to itself."
  };

  if (!list1) return list2;
  if (!list2) return list1;
  if (list1 == list2) {
    _printerr(&LL_ERR_SELF_APPEND, LL_WARNING, "llappend");
    return list1;
  }
  ll *curr;
  for (curr = list1; curr->next != NULL; curr = curr->next);
  curr->next = list2;
  return list1;
}

//--------------------------------------------------------------------------------
// Internal Function: _merge
//
// Description:
//      Recursively merges two sorted linked lists into a single
//      sorted linked list.
//
// Parameters:
//      left:      Pointer to first sorted list.
//      right:     Pointer to second sorted list.
//      compare:   User-defined comparison callback.
//
// Returns:
//      Pointer to merged sorted list.
//--------------------------------------------------------------------------------

static ll *_merge(ll *left, ll *right, int (*compare)(void *, void *)) {
  if (left == NULL) {
    return right;
  }
  if (right == NULL) {
    return left;
  }
  ll *final = NULL;
  if (compare(left->data, right->data) <= 0) {
    final = left;
    final->next = _merge(left->next, right, compare);
  }
  else {
    final = right;
    final->next = _merge(left, right->next, compare);
  }
  return final;
}

//--------------------------------------------------------------------------------
// Internal Function: _find_middle
//
// Description:
//      Finds and returns the middle node of the linked list.
//
// Parameters:
//      list:   Pointer to the head of the linked list.
//
// Returns:
//      Pointer to middle node.
//--------------------------------------------------------------------------------

static ll *_find_middle(ll *list) {
  int count = 0;
  ll *curr;
  for (curr = list; curr != NULL; curr = curr->next) {
    count++;
  }
  int n = count/2;
  if (count % 2 == 0) n -= 1;
  ll *middle = list;
  for (int i = 0; i < n; ++i) {
    middle = middle->next;
  }
  return middle;
}

//--------------------------------------------------------------------------------
// Function: llmergesort
//
// Description:
//      Sorts the linked list using the Merge Sort algorithm.
//
// Parameters:
//      list
//          Pointer to the head of the linked list.
//
//      compare
//          User-defined comparison callback function.
//
// Returns:
//      Pointer to the head of the sorted linked list.
//
// Error Handling:
//      Terminates program if compare callback is NULL.
//
// Notes:
//      - Sorting is performed by rearranging node links.
//      - Payload pointers themselves are NOT modified.
//      - Stable ordering is preserved for equal elements.
//
// Time Complexity:
//      O(n log n)
//
// Usage Example:
//      list = llmergesort(list, compareInt);
//--------------------------------------------------------------------------------

ll *llmergesort(ll *list, int (*compare)(void *, void *)) {
  if (!compare) {
    _printerr(&LL_ERR_NULL_COMPARE, LL_FATAL, "llmergesort");
    exit(2);
  }
  if (!list || !list->next) return list;
  ll *middle = _find_middle(list);
  ll *right = middle->next;
  middle->next = NULL;
  ll *left = list;
  left = llmergesort(left, compare); 
  right = llmergesort(right, compare);
  return _merge(left, right, compare);
}  


//--------------------------------------------------------------------------------
// Internal Function: _join_lists
//
// Description:
//      Concatenates the lesser, equal, and greater partitions produced
//      during quicksort partitioning.
//
// Parameters:
//      lesser:    Pointer to lesser partition list.
//      equal:     Pointer to equal partition list.
//      greater:   Pointer to greater partition list.
//
// Returns:
//      Pointer to combined linked list.
//--------------------------------------------------------------------------------

static ll *_join_lists(ll *lesser, ll *equal, ll *greater) {
  ll *head = NULL;
  ll *curr = NULL;
  // overall head
  if (lesser) {
    head = lesser;
  }
  else if (equal) {
    head = equal;
  }
  else {
    head = greater;
  }
  // attach equal after lesser
  if (lesser) {
    for (curr = lesser; curr->next != NULL; curr = curr->next);
    curr->next = equal;
    if (!equal) {
      curr->next = greater;
      return head;
    }
  }
  // attach greater after equal (or lesser if equal is NULL)
  if (equal) {
    for (curr = equal; curr->next != NULL; curr = curr->next);
    curr->next = greater;
  }
  return head;
}

//--------------------------------------------------------------------------------
// Function: llquicksort
//
// Description:
//      Sorts the linked list using a recursive three-way Quick Sort
//      implementation.
//
// Parameters:
//      list:      Pointer to the head of the linked list.
//      compare:   User-defined comparison callback function.
//
// Returns:
//      Pointer to the head of the sorted linked list.
//
// Error Handling:
//      Terminates program if compare callback is NULL.
//
// Notes:
//      - Sorting is performed by rearranging node links.
//      - Payload pointers themselves are not modified.
//      - Uses the first node payload as pivot.
//
// Time Complexity:
//      Average Case:   O(n log n)
//      Worst Case:     O(n^2)
//
// Usage Example:
//      list = llquicksort(list, compareInt);
//--------------------------------------------------------------------------------

ll *llquicksort(ll *list, int (*compare)(void *, void *)) {
  if (!compare) {
    _printerr(&LL_ERR_NULL_COMPARE, LL_FATAL, "llquicksort");
    exit(2);
  }
  if (!list || !list->next) {
    return list;
  }
  void *pivot = list->data; // head of the list is the default pivot
  // break list into lesser, equal, greater
  ll *lesser_iter = NULL;
  ll *equal_iter = NULL;
  ll *greater_iter = NULL;
  ll *lesser = NULL;

  ll *equal = NULL;
  ll *greater = NULL;
  ll *curr;
  ll *next;
  for (curr = list; curr != NULL; curr = next) {
    next = curr->next;
    curr->next = NULL;
    if (compare(curr->data, pivot) < 0) {
      if (!lesser) {
        lesser = curr;
        lesser_iter = lesser;
      }
      else {
        lesser_iter->next = curr;
        lesser_iter = lesser_iter->next;
      }
    }
    else if (compare(curr->data, pivot) == 0) {
      if (!equal) {
        equal = curr;
        equal_iter = equal;
      }
      else {
        equal_iter->next = curr;
        equal_iter = equal_iter->next;
      }
    }
    else {
      if (!greater) {
        greater = curr;
        greater_iter = greater;
      }
      else {
        greater_iter->next = curr;
        greater_iter = greater_iter->next;
      }
    }
  }
  ll *less = llquicksort(lesser, compare);
  ll *great = llquicksort(greater, compare);
  return _join_lists(less, equal, great);
}

//--------------------------------------------------------------------------------
// Internal Function: _part
//
// Description:
//      Partitions a linked list segment around a pivot element for
//      in-place quicksort processing.
//
// Parameters:
//      list:       Pointer to first node of partition range.
//      tail:      Pointer to final node of partition range.
//      compare:   User-defined comparison callback.
//
// Returns:
//      Pointer to pivot node after partitioning.
//--------------------------------------------------------------------------------

static ll *_part(ll *list, ll *tail, int (*compare)(void *, void *)) {
  void *pivot = tail->data;
  ll *i = list;
  ll *j;

  for (j = list; j != tail; j = j->next) {
    if (compare(j->data, pivot) < 0) {
      _swap(&i->data, &j->data);
      i = i->next;
    }
  }
  _swap(&i->data, &tail->data); 
  return i;  
}

//--------------------------------------------------------------------------------
// Internal Function: _qsort
//
// Description:
//      Recursive helper function for in-place linked list quicksort.
//
// Parameters:
//      list:      Pointer to first node of current partition.
//      tail:      Pointer to final node of current partition.
//      compare:   User-defined comparison callback.
//
// Returns:
//      None.
//--------------------------------------------------------------------------------

static void _qsort(ll *list, ll *tail, int (*compare)(void *, void *)) {
  if (!list || !tail || list == tail) {
    return;
  }
  ll *pivot = _part(list, tail, compare);
  if (pivot != list) {
    ll *curr;
    for (curr = list; curr->next != pivot; curr = curr->next);
    _qsort(list, curr, compare);
  }
  if (pivot != tail) {
    _qsort(pivot->next, tail, compare);
  }
}

//--------------------------------------------------------------------------------
// Function: llqsort
//
// Description:
//      Sorts the linked list using an in-place Quick Sort algorithm.
//
// Parameters:
//      list:      Pointer to the head of the linked list.
//      compare:   User-defined comparison callback function.
//
// Returns:
//      Pointer to the head of the sorted linked list.
//
// Error Handling:
//      Terminates program if compare callback is NULL.
//
// Notes:
//      - Sorting is performed through payload pointer swapping.
//      - Node structure and connectivity remain unchanged.
//      - This implementation is unstable.
//
// Time Complexity:
//      Average Case:   O(n log n)
//      Worst Case:     O(n^2)
//
// Usage Example:
//      list = llqsort(list, compareInt);
//--------------------------------------------------------------------------------


ll *llqsort(ll *list, int (*compare)(void *, void *)) {
  if (!compare) {
    _printerr(&LL_ERR_NULL_COMPARE, LL_FATAL, "llqsort");
    exit(2);
  }
  if (!list || !list->next) {
    return list;
  }
  ll *tail;
  for (tail = list; tail->next != NULL; tail = tail->next);
  _qsort(list, tail, compare);
  return list;
}


//--------------------------------------------------------------------------------
// Function: lldestroy
//
// Description:
//      Frees all nodes and payloads in the linked list.
//
// Parameters:
//      list:      Pointer to the head of the linked list.
//      destroy:   Payload destructor callback
//
// Returns:
//      None.
//
// Time Complexity:
//      O(n)
//
// Usage Example:
//      lldestroy(list, free);
//--------------------------------------------------------------------------------

void lldestroy(ll *list, void (*destroy)(void *)) {
  ll *curr;
  ll *next;
  for (curr = list; curr != NULL; curr = next) {
    next = curr->next;
    if (destroy) {
      destroy(curr->data);
    }
    free(curr);
  }
  return;
}

//--------------------------------------------------------------------------------
// Function: llgetdata
//
// Description:
//      Returns the payload of the node provided
//
// Parameters:
//      list:      Pointer to the head of the linked list.
//
// Returns:
//      Payload of the provided node.
//
// Time Complexity:
//      O(1)
//
// Usage Example:
//      void *val = llgetdata(list);
//--------------------------------------------------------------------------------

void *llgetdata(ll *list) {
  if (list) {
    return list->data;
  }
  else {
    return NULL;
  }
  
}

//--------------------------------------------------------------------------------
// Function: llgetnext
//
// Description:
//      Returns the next pointer of the node provided
//
// Parameters:
//      list:      Pointer to the head of the linked list.
//
// Returns:
//      Next pointer of the provided node.
//
// Time Complexity:
//      O(1)
//
// Usage Example:
//      ll *next = llgetnext(list);
//--------------------------------------------------------------------------------

ll *llgetnext(ll *list) {
  if (list) {
    return list->next;
  }
  else {
    return NULL;
  }
  
}
