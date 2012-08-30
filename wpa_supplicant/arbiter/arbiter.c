#include "includes.h"
#include "common.h"
#include "common/ieee802_11_defs.h"
#include "arbiter.h"

arbiter *arbiter_init(struct wpa_supplicant *wpa_s){
  arbiter* res = (arbiter *)os_malloc(sizeof(arbiter));
  if (res == NULL)
    return NULL;

  res->state = ARBITER_IDLE;
  res->wpa_s = wpa_s;
  res->filter_num = 2;
  res->filters[0] = consortium_filter;
  res->filters[1] = random_filter;
  return res;
}

void arbiter_deinit(arbiter *arb){
  os_free(arb);
}
