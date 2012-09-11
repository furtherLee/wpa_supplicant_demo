#ifndef _ARBITER_UTIL_
#define _ARBITER_UTIL_

#define EXTENDED_CAPABILITIES_IE_ID 127 
#define INTERWORKING_IE_ID 107

#include "includes.h"
#include "common.h"
#include "../wpa_supplicant_i.h"
#include "utils/wpabuf.h"
#include "utils/list.h"

void arbiter_message(struct wpa_supplicant *wpa_s, char* content);

int parse_oui(int **ans, struct wpabuf* buf);

int ie_enable_interworking(struct wpa_bss *bss);

int ie_interworking_internet(struct wpa_bss *bss);

void display_candidates(struct wpa_supplicant *wpa_s, struct dl_list *list);

int insert_string(char **pos, char **end, int num);

int abiter_append_eap_method(char **pos, char **end, u8 method);

int is_free_public(struct wpa_bss *bss);

int oui_contains(struct wpa_bss *bss, char *oui);

#endif
