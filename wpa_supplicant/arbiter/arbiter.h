#ifndef _ARBITER_
#define _ARBITER_

#include "wpa_supplicant_i.h"
#include "arbiter/filter.h"

enum arbiter_state{
  ARBITER_IDLE,
  ARBITER_QUERYING,
  ARBITER_DECIDING,
  ARBITER_STATE_LAST,
};

typedef struct{
  enum arbiter_state state;
  struct wpa_supplicant *wpa_s;
  arbiter_filter* filters;
} arbiter;

arbiter *arbiter_init(struct wpa_supplicant *wpa_s);
void arbiter_deinit(arbiter *arb);

#endif
