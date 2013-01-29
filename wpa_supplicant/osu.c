#include "includes.h"
#include "common.h"
#include "common/ieee802_11_defs.h"
#include "utils/list.h"
#include "wpa_supplicant_i.h"
#include "bss.h"
#include "interworking.h"
#include "config.h"
#include "notify.h"
#include "rsn_supp/wpa.h"
#include "eapol_supp/eapol_supp_sm.h"
#include "osu.h"

static int add_network(struct wpa_supplicant *wpa_s) {
  struct wpa_ssid *ssid;
  
  ssid = wpa_config_add_network(wpa_s->conf);
  
  if (ssid == NULL)
    return -1;
  
  wpas_notify_network_added(wpa_s, ssid);
  
  ssid->disabled = 1;
  wpa_config_set_network_defaults(ssid);
  
  return ssid->id;
}

static int set_network(struct wpa_supplicant *wpa_s, int id, char *name, char* value) {
	struct wpa_ssid *ssid;

	ssid = wpa_config_get_network(wpa_s->conf, id);
	if (ssid == NULL) {
		wpa_printf(MSG_DEBUG, "CTRL_IFACE: Could not find network "
			   "id=%d", id);
		return -1;
	}

	if (wpa_config_set(ssid, name, value, 0) < 0) {
		wpa_printf(MSG_DEBUG, "CTRL_IFACE: Failed to set network "
			   "variable '%s'", name);
		return -1;
	}

	wpa_sm_pmksa_cache_flush(wpa_s->wpa, ssid);

	if (wpa_s->current_ssid == ssid || wpa_s->current_ssid == NULL) {
		/*
		 * Invalidate the EAP session cache if anything in the current
		 * or previously used configuration changes.
		 */
		eapol_sm_invalidate_cached_session(wpa_s->eapol);
	}

	if ((os_strcmp(name, "psk") == 0 &&
	     value[0] == '"' && ssid->ssid_len) ||
	    (os_strcmp(name, "ssid") == 0 && ssid->passphrase))
		wpa_config_update_psk(ssid);
	else if (os_strcmp(name, "priority") == 0)
		wpa_config_update_prio_list(wpa_s->conf);

	return 0;
}

static int select_network(struct wpa_supplicant *wpa_s, int id) {
  struct wpa_ssid* ssid = wpa_config_get_network(wpa_s->conf, id);
  if (ssid == NULL) {
    wpa_printf(MSG_DEBUG, "CTRL_IFACE: Could not find "
	       "network id=%d", id);
    return -1;
  }
  if (ssid->disabled == 2) {
    wpa_printf(MSG_DEBUG, "CTRL_IFACE: Cannot use "
	       "SELECT_NETWORK with persistent P2P group");
    return -1;
  }

  wpa_supplicant_select_network(wpa_s, ssid);
  
  return 0;
}


static void osu_connect_osu_ap (struct wpa_supplicant *wpa_s, char *osuApBssid) {
  int id = add_network(wpa_s);
  int ret = set_network(wpa_s, id, "bssid", osuApBssid);
  ret = set_network(wpa_s, id, "key_mgmt", "NONE");
  ret = select_network(wpa_s, id);
}

static void osu_client_fetch_credentials(struct wpa_supplicant *wpa_s, char *uri) {
  // TODO
  char cmd[1024];
  sprintf(cmd, "OSU_EVENT_FETCH_REQUEST %s", uri);
  wpa_printf(MSG_INFO, "FETCH CREDENTIALS %s", uri);
  wpa_msg_ctrl(wpa_s, MSG_INFO, cmd);
}

void on_osu_ap_connected(struct wpa_supplicant *wpa_s) {
  // TODO compare osuApBssid and wpa_s->current_ssid->bssid
  // TODO check whether fetchCredentialsStarted
  osu_client_fetch_credentials(wpa_s, wpa_s->osu->uri);
  
}

static void on_osu_client_credentials_fetched(struct wpa_supplicant *wpa_s, char *params) {
  
  if (!wpa_s->osu->fetchCredentialsStarted)
    return;
  
  char *p;

  wpa_printf(MSG_INFO, "params is: %s", params);
  
  int id = add_network(wpa_s);
  char prodApBssidStr[64];
  sprintf(prodApBssidStr, MACSTR, MAC2STR(wpa_s->osu->prodApBssid));
  
  int ret = set_network(wpa_s, id, "bssid", prodApBssidStr);
  char name[256];
  char value[256];

  for (p = strtok(params, ","); p != NULL; p = strtok(NULL, ",")){
    int i = 0;
    
    while (p[i] != '='){
      name[i] = p[i];
      i++;
    }
    name[i++]='\0';
    wpa_printf(MSG_INFO, "name: %s", name);

    int j = 0;
    while (p[i] != ',') {
      value[j++] = p[i];
      i++;
     }
    value[j] = '\0';

    wpa_printf(MSG_INFO, "key=%s, value=%s", name, value);
    ret = set_network(wpa_s, id, name, value);
  }

  select_network(wpa_s, id);

  wpa_s->osu->fetchCredentialsStarted = 0;
}


int osu_fetch_credentials (struct wpa_supplicant *wpa_s, char* buf, char *reply){

  if (wpa_s->osu->fetchCredentialsStarted)
    return -1;
  
  wpa_s->osu->fetchCredentialsStarted = 1;
  
  char prodApBssidStr[50], osuApBssidStr[50], uri[512];
  
  sscanf(buf, "%s %s %s", prodApBssidStr, osuApBssidStr, uri);
  
  hwaddr_aton2(prodApBssidStr, wpa_s->osu->prodApBssid);
  hwaddr_aton2(osuApBssidStr, wpa_s->osu->osuApBssid);
  memcpy(wpa_s->osu->uri, uri, strlen(uri));

  osu_connect_osu_ap(wpa_s, osuApBssidStr);
  
  os_memcpy(reply, "FETCH_CREDENTIALS STARTS!\n", 26);


  return 26;
}

int osu_credentials_fetched (struct wpa_supplicant *wpa_s, char* buf, char *reply){
  on_osu_client_credentials_fetched (wpa_s, buf);
  memcpy(reply, "OK\n", 3);
  return 3;
}


struct osu_priv *osu_init(struct wpa_supplicant *wpa_s) {
  struct osu_priv* ret = (struct osu_priv *) os_malloc(sizeof(struct osu_priv));
  ret->fetchCredentialsStarted = 0;
  return ret;
}

void osu_deinit(struct osu_priv *osu){
  free(osu);
}
