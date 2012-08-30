#include "includes.h"
#include "common.h"
#include "common/ieee802_11_defs.h"
#include "utils/list.h"
#include "../wpa_supplicant_i.h"
#include "../bss.h"
#include "arbiter.h"

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
  res->filters[0] = consortium_filter;
  res->filters[1] = random_filter;
  event_init(wpa_s);
  return res;
}

void arbiter_deinit(arbiter *arb){
  os_free(arb);
}

struct wpa_bss *arbiter_select(struct dl_list *list, int *count){
  // TODO
  return NULL;
}
