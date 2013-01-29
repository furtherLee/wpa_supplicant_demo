#ifndef OSU_H
#define OSU_H 

struct wpa_supplicant;

int osu_fetch_credentials (struct wpa_supplicant *wpa_s, char* buf, char *reply);

#endif
