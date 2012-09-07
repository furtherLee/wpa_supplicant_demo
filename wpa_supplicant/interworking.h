/*
 * Interworking (IEEE 802.11u)
 * Copyright (c) 2011, Qualcomm Atheros
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef INTERWORKING_H
#define INTERWORKING_H

enum gas_query_result;

int anqp_send_req(struct wpa_supplicant *wpa_s, const u8 *dst,
		  u16 info_ids[], size_t num_ids);
void anqp_resp_cb(void *ctx, const u8 *dst, u8 dialog_token,
		  enum gas_query_result result,
		  const struct wpabuf *adv_proto,
		  const struct wpabuf *resp, u16 status_code);
int interworking_fetch_anqp(struct wpa_supplicant *wpa_s);
void interworking_stop_fetch_anqp(struct wpa_supplicant *wpa_s);
int interworking_select(struct wpa_supplicant *wpa_s, int auto_select);
int interworking_connect(struct wpa_supplicant *wpa_s, struct wpa_bss *bss);

struct nai_realm_eap {
	u8 method;
	u8 inner_method;
	enum nai_realm_eap_auth_inner_non_eap inner_non_eap;
	u8 cred_type;
	u8 tunneled_cred_type;
};

struct nai_realm {
	u8 encoding;
	char *realm;
	u8 eap_count;
	struct nai_realm_eap *eap;
};

struct nai_realm * nai_realm_parse(struct wpabuf *anqp, u16 *count);
void nai_realm_free(struct nai_realm *realms, u16 count);

#endif /* INTERWORKING_H */
