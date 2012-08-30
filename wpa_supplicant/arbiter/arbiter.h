#ifndef _ARBITER_
#define _ARBITER_

#include "../wpa_supplicant_i.h"
#include "filter.h"

#define ROAM_WAIT_TIME 5
#define MAX_FILTER_NUM 5

enum arbiter_state{
  ARBITER_IDLE,
  ARBITER_QUERYING,
  ARBITER_DECIDING,
  ARBITER_STATE_LAST,
};

typedef struct{
  enum arbiter_state state;
  struct wpa_supplicant *wpa_s;
  int filter_num;
  arbiter_filter filters[MAX_FILTER_NUM];
} arbiter;

arbiter *arbiter_init(struct wpa_supplicant *wpa_s);
void arbiter_deinit(arbiter *arb);

#endif
