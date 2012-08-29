/*
 * wpa_supplicant - Wi-Fi Display
 * Copyright (c) 2011, Atheros Communications
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

#ifndef WIFI_DISPLAY_H
#define WIFI_DISPLAY_H

int wifi_display_init(struct wpa_global *global);
void wifi_display_deinit(struct wpa_global *global);
void wifi_display_enable(struct wpa_global *global, int enabled);
int wifi_display_subelem_set(struct wpa_global *global, char *cmd);
int wifi_display_subelem_get(struct wpa_global *global, char *cmd,
			     char *buf, size_t buflen);

#endif /* WIFI_DISPLAY_H */
