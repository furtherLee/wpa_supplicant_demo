#include "includes.h"
#include "common.h"
#include "common/ieee802_11_defs.h"
#include "utils/list.h"
#include "wpa_supplicant_i.h"
#include "bss.h"
#include "interworking.h"
#include "config.h"
#include "osu.h"

int osu_fetch_credentials (struct wpa_supplicant *wpa_s, char* buf, char *reply){
  
  char prodApBssid[50], osuApBssid[50], uri[512];
  
  sscanf(buf, "%s %s %s", prodApBssid, osuApBssid, uri);
  wpa_printf(MSG_INFO, "%s\n%s\n%s\n", prodApBssid, osuApBssid, uri);
  
  os_memcpy(reply, "FETCH_CREDENTIALS STARTS!\n", 26);

  return 26;
}
