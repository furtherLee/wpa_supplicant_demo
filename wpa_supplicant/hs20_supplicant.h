/*
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

#ifndef HS20_SUPPLICANT_H
#define HS20_SUPPLICANT_H

void wpas_hs20_add_indication(struct wpabuf *buf);

int hs20_anqp_send_req(struct wpa_supplicant *wpa_s, const u8 *dst, u32 stypes,
		       const u8 *payload, size_t payload_len);
struct wpabuf * hs20_build_anqp_req(u32 stypes, const u8 *payload,
				    size_t payload_len);
void hs20_parse_rx_hs20_anqp_resp(struct wpa_supplicant *wpa_s,
				  const u8 *sa, const u8 *data, size_t slen);

#endif /* HS20_SUPPLICANT_H */
