#ifndef _ARBITER_FILTER_
#define _ARBITER_FILTER_

#include "includes.h"
#include "common.h"
#include "utils/list.h"

#define MAX_ARBITER_CANDIDATE_NAME_LEN 256

typedef struct{
  struct dl_list list;
  char name[MAX_ARBITER_CANDIDATE_NAME_LEN];
  u8 sa[ETH_ALEN];
} filter_candidate;

typedef struct dl_list* (*arbiter_filter)(struct dl_list *candidates, void* context);

void remove_candidate(filter_candidate *candidate);

void add_candidate(struct dl_list *list, filter_candidate *candidate);

#endif
