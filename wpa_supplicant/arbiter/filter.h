#ifndef _ARBITER_FILTER_
#define _ARBITER_FILTER_

#define MAX_ARBITER_CANDIDATE_NAME_LEN 256

typedef struct{
  struct dlist list;
  char[MAX_ARBITER_CANDIDATE_NAME_LEN] name;
  u8[ETH_ALEN] sa;
} filter_candidate;

struct dlist* (*arbiter_filter)(struct dlist *candidates, void* context);

void remove_candidate(filter_candidate *candidate);

void add_candidate(struct dlist *list, filter_candidate *candidate);

#endif
