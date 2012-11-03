#include "includes.h"
#include "filter.h"
#include "utils/list.h"
#include "util.h"

void remove_candidate(filter_candidate *candidate){
  // TODO
}

void add_candidate(struct dl_list *list, filter_candidate *candidate){
  dl_list_add_tail(list, &candidate->list);
}

struct dl_list* consortium_filter(struct dl_list *candidates, void *context){
  // TODO
  return candidates;
}

struct dl_list* random_filter(struct dl_list *candidates, void *context){
  unsigned int num = dl_list_len(candidates);
  unsigned int decide = os_random() % num;
  filter_candidate *ret = NULL;
  filter_candidate *pointer = NULL;
  int i = 0;
  
  arbiter_message((struct wpa_supplicant *)context, "Random Filter starts working...");

  while(!dl_list_empty(candidates)){
    pointer = dl_list_first(candidates, filter_candidate, list);
    dl_list_del(&pointer->list);
    if (i == decide)
      ret = pointer;
    else
      os_free(pointer);
    
    i++;
  }

  add_candidate(candidates, ret);
  return candidates;
}

struct dl_list* access_internet_filter(struct dl_list *candidates, void *context){
  filter_candidate *item = NULL;
  filter_candidate *wait_to_delete = NULL;

  arbiter_message((struct wpa_supplicant *)context, "Accessing Internet Filter starts working...");

  dl_list_for_each(item, candidates, filter_candidate, list){
    if(wait_to_delete){
      dl_list_del(&wait_to_delete->list);
      os_free(wait_to_delete);
      wait_to_delete = NULL;
    }
    if (!ie_interworking_internet(item->bss))
      wait_to_delete = item;
  }

  if(wait_to_delete){
    dl_list_del(&wait_to_delete->list);
    os_free(wait_to_delete);
    wait_to_delete = NULL;
  }

  return candidates;
}

struct dl_list* free_public_filter(struct dl_list *candidates, void *context){
  filter_candidate *item = NULL;
  filter_candidate *wait_to_delete = NULL;

  arbiter_message((struct wpa_supplicant *)context, "Free-Public Filter starts working...");

  dl_list_for_each(item, candidates, filter_candidate, list){
    if(wait_to_delete){
      dl_list_del(&wait_to_delete->list);
      os_free(wait_to_delete);
      wait_to_delete = NULL;
    }
    if (!is_free_public(item->bss))
      wait_to_delete = item;
  }

  if(wait_to_delete){
    dl_list_del(&wait_to_delete->list);
    os_free(wait_to_delete);
    wait_to_delete = NULL;
  }

  return candidates;  
}

filter_candidate* build_candidate(struct wpa_bss *bss){
  filter_candidate *ret = (filter_candidate *)os_malloc(sizeof(filter_candidate));
  if (ret == NULL)
    return NULL;
  ret->bss = bss;
  return ret;
}
