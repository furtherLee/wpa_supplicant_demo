#include "includes.h"
#include "common.h"
#include "common/ieee802_11_defs.h"
#include "utils/list.h"
#include "../wpa_supplicant_i.h"
#include "../bss.h"
#include "../interworking.h"
#include "arbiter.h"
#include "util.h"

static void event_init(struct wpa_supplicant *wpa_s){
  // TODO
}

static void event_deinit(struct wpa_supplicant *wpa_s){
  // TODO
}

arbiter *arbiter_init(struct wpa_supplicant *wpa_s){
  arbiter* res = (arbiter *)os_malloc(sizeof(arbiter));
  if (res == NULL)
    return NULL;

  res->state = ARBITER_IDLE;
  res->wpa_s = wpa_s;
  res->filter_num = 2;
  res->filters[0] = access_internet_filter;
  res->filters[1] = random_filter;
  res->set_auto = 0;
  event_init(wpa_s);
  return res;
}

void arbiter_deinit(arbiter *arb){
  event_deinit(arb->wpa_s);
  os_free(arb);
}

struct wpa_bss *arbiter_select(struct dl_list *list, struct wpa_supplicant *wpa_s){

  int i;
  struct dl_list *ret = list;
  arbiter *arbiter = wpa_s->arbiter;
  arbiter->state = ARBITER_DECIDING;
  arbiter_message(wpa_s, "All information retrieved, start selecting process.");
  arbiter_message(wpa_s, "All interworking networks available are:");
  display_candidates(wpa_s, ret);

  for(i = 0; i < arbiter->filter_num; ++i){
    ret = arbiter->filters[i](list, wpa_s);
    arbiter_message(wpa_s, "After this filter, candidates left are:");
    display_candidates(wpa_s, ret);
    if(dl_list_empty(ret))
      break;
  }

  arbiter->state = ARBITER_IDLE;
  if (dl_list_empty(ret)){
    arbiter_message(wpa_s, "No network can be selected after all filters!");
    return NULL;
  }

  filter_candidate *candidate = dl_list_first(ret, filter_candidate, list);
  struct wpa_bss *ans = candidate->bss;
  os_free(candidate);

  return ans;
}

void arbiter_disconnect_occur(struct wpa_supplicant *wpa_s){

  if(!wpa_s->arbiter->set_auto)
    return;

  wpa_s->arbiter->state = ARBITER_QUERYING;
  arbiter_message(wpa_s, "Detect DISCONNECT! Arbiter start to handle network selection!");
  arbiter_message(wpa_s, "Arbiter starts to fetch all anqp information");
  interworking_select(wpa_s, 1);
}

int arbiter_set_auto(struct wpa_supplicant *wpa_s, char *buf, char* reply){
  if (os_strncmp(buf, "TRUE", 4) == 0){
    wpa_s->arbiter->set_auto = 1;
    os_memcpy(reply, "Arbiter: AUTO_SET\n", 18);
    return 18;
  }
  else if (os_strncmp(buf, "FALSE", 5) == 0){
    wpa_s->arbiter->set_auto = 1;
    os_memcpy(reply, "Arbiter: AUTO_RESET\n", 20);
    return 20;    
  }
  else
    return -1;
}
