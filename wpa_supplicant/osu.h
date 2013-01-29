#ifndef OSU_H
#define OSU_H 

struct wpa_supplicant;

struct osu_priv {
  u8 osuApBssid[ETH_ALEN];
  u8 prodApBssid[ETH_ALEN];
  char uri[512];
  int fetchCredentialsStarted;
  // TODO
};

int osu_fetch_credentials (struct wpa_supplicant *wpa_s, char* buf, char *reply);
int osu_credentials_fetched (struct wpa_supplicant *wpa_s, char* buf, char *reply);
struct osu_priv *osu_init(struct wpa_supplicant *wpa_s);
void osu_deinit(struct osu_priv *osu);
void on_osu_ap_connected(struct wpa_supplicant *wpa_s);

#endif
