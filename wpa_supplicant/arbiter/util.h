#ifndef _ARBITER_UTIL_
#define _ARBITER_UTIL_

#include "includes.h"
#include "common.h"
#include "../wpa_supplicant_i.h"
#include "utils/wpabuf.h"

void arbiter_message(struct wpa_supplicant *wpa_s, char* content);
int parse_oui(int **ans, struct wpabuf* buf);

#endif
