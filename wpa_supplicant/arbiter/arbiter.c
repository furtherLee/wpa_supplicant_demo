#include "includes.h"
#include "common.h"
#include "common/ieee802_11_defs.h"
#include "utils/list.h"
#include "../wpa_supplicant_i.h"
#include "../bss.h"
#include "../interworking.h"
#include "arbiter.h"
#include "util.h"
#include "../config.h"

static void event_init(struct wpa_supplicant *wpa_s){
  // TODO
}

static void event_deinit(struct wpa_supplicant *wpa_s){
  // TODO
}

arbiter *arbiter_init(struct wpa_supplicant *wpa_s){
  arbiter* res = (arbiter *)os_malloc(sizeof(arbiter));
  if (res == NULL)
    return NULL;

  res->state = ARBITER_IDLE;
  res->wpa_s = wpa_s;
  res->filter_num = 4;
  res->filters[0] = oui_filter;
  res->filters[1] = free_public_filter;
  res->filters[2] = access_internet_filter;
  res->filters[3] = random_filter;
  res->set_auto = 0;
  event_init(wpa_s);
  return res;
}

void arbiter_deinit(arbiter *arb){
  event_deinit(arb->wpa_s);
  os_free(arb);
}

struct wpa_bss *arbiter_select(struct dl_list *list, struct wpa_supplicant *wpa_s){

  int i;
  struct dl_list *ret = list;
  arbiter *arbiter = wpa_s->arbiter;
  arbiter->state = ARBITER_DECIDING;
  arbiter_message(wpa_s, "All information retrieved, start selecting process.");
  arbiter_message(wpa_s, "All interworking networks available are:");
  display_candidates(wpa_s, ret);

  for(i = 0; i < arbiter->filter_num; ++i){
    ret = arbiter->filters[i](list, wpa_s);
    arbiter_message(wpa_s, "After this filter, candidates left are:");
    display_candidates(wpa_s, ret);
    if(dl_list_empty(ret))
      break;
  }

  arbiter->state = ARBITER_IDLE;
  if (dl_list_empty(ret)){
    arbiter_message(wpa_s, "No network can be selected after all filters!");
    return NULL;
  }

  filter_candidate *candidate = dl_list_first(ret, filter_candidate, list);
  struct wpa_bss *ans = candidate->bss;
  os_free(candidate);

  return ans;
}

void arbiter_disconnect_occur(struct wpa_supplicant *wpa_s){

  wpa_printf(MSG_INFO, "set_auto: %d", wpa_s->arbiter->set_auto);

  if(!wpa_s->arbiter->set_auto)
    return;

  wpa_s->arbiter->state = ARBITER_QUERYING;
  arbiter_message(wpa_s, "Detect DISCONNECT! Arbiter start to handle network selection!");
  arbiter_message(wpa_s, "Arbiter starts to fetch all anqp information");
  interworking_select(wpa_s, 1);
}

int arbiter_set_auto(struct wpa_supplicant *wpa_s, char *buf, char* reply){
  wpa_printf(MSG_INFO, "%s", buf);

  if (os_strncmp(buf, "TRUE", 4) == 0){
    wpa_s->arbiter->set_auto = 1;
    os_memcpy(reply, "Arbiter: AUTO_SET\n", 18);
    return 18;
  }
  else if (os_strncmp(buf, "FALSE", 5) == 0){
    wpa_s->arbiter->set_auto = 0;
    os_memcpy(reply, "Arbiter: AUTO_RESET\n", 20);
    return 20;    
  }
  else
    return -1;
}

static void getHS20(char *hs20, struct wpa_bss *bss){
  if(wpa_bss_get_vendor_ie(bss, HS20_IE_VENDOR_TYPE))
    os_memcpy(hs20, "YES\0", 4);
  else
    os_memcpy(hs20, "NO\0", 3);
}


static void getInternet(char *internet, struct wpa_bss *bss){
  if(ie_interworking_internet(bss))
    os_memcpy(internet, "YES\0", 4);
  else
    os_memcpy(internet, "NO\0", 3);
}

static void getChargable(char *chargable, struct wpa_bss *bss){
  const u8* pos = wpa_bss_get_ie(bss, INTERWORKING_IE_ID);
  if (pos == NULL || pos[1] < 1){
    os_memcpy(chargable, "UNSET\0", 6);
    return;
  }

  const u8* ie = pos + 2;
  int type = ie[0] & 0x0f;
  os_snprintf(chargable, 32, "%d", type);
}

static void getAuthMethod(struct wpa_supplicant *wpa_s, char *authMethod, struct wpa_bss *bss){
  if (bss->anqp_nai_realm == NULL){
    os_memcpy(authMethod, "UNSET\0", 6);
    return;
  }
  u16 count, i;
  char *pos = authMethod;
  char *end = pos + 256;
  struct nai_realm *realms = nai_realm_parse(bss->anqp_nai_realm, &count);
  int ret;
  int j;
  int flag = 1;

  for (i = 0; i < count; ++i)
    if(nai_realm_match(&realms[i], wpa_s->conf->home_realm)){
      for (j = 0; j < realms[i].eap_count; ++j)
	if(!abiter_append_eap_method(&pos, &end, realms[i].eap[j].method)){
	  flag = 0;
	  break;
	}

      if (!flag)
	break;
    }

  if (pos == authMethod){
    ret = os_snprintf(authMethod, end-pos, "No Matched Realm");
    insert_string(&pos, &end, ret); 
  }

  *pos = '\0';

  nai_realm_free(realms, count);
}

static void getRoamingConsortium(char *roamingConsortium, struct wpa_bss *bss){
  if (bss->anqp_roaming_consortium == NULL){
    os_memcpy(roamingConsortium, "UNSET\0", 6);
    return;
  }
  
  struct wpabuf *list = bss->anqp_roaming_consortium;

  const u8 *pos = wpabuf_head_u8(list);
  const u8 *end = pos + wpabuf_len(list);
  //  char fuck[256];
  //  wpa_snprintf_hex(fuck, 256, pos, 30);
  char *mark = roamingConsortium;
  char *mark_end = mark + 256;
  for(;pos != end; pos = pos + *pos + 1){
    wpa_snprintf_hex(mark, mark_end - mark, pos + 1, *pos);
    mark += 2 * (*pos);
    if (pos + *pos + 1 != end){
      *mark = ',';
      mark++;
    }
  }
  *mark = '\0';
}

int arbiter_get_anqp_info(struct wpa_supplicant *wpa_s, char *cmd, char *reply, size_t reply_size){
    size_t i;
    struct wpa_bss *bss;
    int ret;
    char *pos, *end;
    
    struct wpa_bss *tmp;
    i = atoi(cmd);
    bss = NULL;
    dl_list_for_each(tmp, &wpa_s->bss_id, struct wpa_bss, list_id)
      {
	if (i-- == 0) {
	  bss = tmp;
	  break;
	}
      }
    
  if (bss == NULL)
    return 0;
  
  char hs20[6], internet[6], chargable[64], authMethod[256], roamingConsortium[256];  
  
  getHS20(hs20, bss);
  getInternet(internet, bss);
  getChargable(chargable, bss);
  getAuthMethod(wpa_s, authMethod, bss);
  getRoamingConsortium(roamingConsortium, bss);

  pos = reply;
  end = reply + reply_size;
  ret = os_snprintf(pos, end - pos,
		    "ssid=%s\n"
		    "bssid=" MACSTR "\n"
		    "hs20=%s\n"
		    "internet=%s\n"
		    "chargable=%s\n"
		    "authMethod=%s\n"
		    "roamingConsortium=%s\n"
		    ,
		    bss->ssid,
		    MAC2STR(bss->bssid),
		    hs20,
		    internet,
		    chargable,
		    authMethod,
		    roamingConsortium
		    );

  return ret;
}

int arbiter_set_oui(struct wpa_supplicant *wpa_s, char* buf, char* reply){
  sscanf(buf, "%s", wpa_s->arbiter->oui);
  os_memcpy(reply, "SETTED\n", 7);
  return 7;
}

