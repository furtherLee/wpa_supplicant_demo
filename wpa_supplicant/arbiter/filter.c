#include "includes.h"
#include "filter.h"

void remove_candidate(filter_candidate *candidate){
  // TODO
}

void add_candidate(struct dl_list *list, filter_candidate *candidate){
  // TODO
}

struct dl_list* consortium_filter(struct dl_list *candidates, void *context){
  // TODO
  return NULL;
}

struct dl_list* random_filter(struct dl_list *candidates, void *context){
  // TODO
  return NULL;
}

filter_candidate* build_candidate(struct wpa_bss *bss){
  filter_candidate *ret = (filter_candidate *)os_malloc(sizeof(filter_candidate));
  if (ret == NULL)
    return NULL;
  ret->bss = bss;
  return ret;
}
