#ifndef _ARBITER_FILTER_
#define _ARBITER_FILTER_

#include "includes.h"
#include "common.h"
#include "utils/list.h"
#include "../wpa_supplicant_i.h"
#include "../bss.h"

#define MAX_ARBITER_CANDIDATE_NAME_LEN 256

typedef struct{
  struct dl_list list;
  struct wpa_bss *bss;
} filter_candidate;

typedef struct dl_list* (*arbiter_filter)(struct dl_list *candidates, void* context);

void remove_candidate(filter_candidate *candidate);

void add_candidate(struct dl_list *list, filter_candidate *candidate);

struct dl_list* consortium_filter(struct dl_list *candidates, void *context);

struct dl_list* random_filter(struct dl_list *candidates, void *context);

filter_candidate* build_candidate(struct wpa_bss *bss);

#endif
