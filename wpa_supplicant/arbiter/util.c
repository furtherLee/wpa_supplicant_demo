#include "includes.h"
#include "util.h"
#include "filter.h"
#include "../bss.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

static FILE *msg_file = NULL;

static const char msg_file_path[] = "/tmp/arbiter_msg.log";

static const char *PIPE = "/tmp/arbiter_fifo";

static int PIPE_FD = -1;

static void local_message(struct wpa_supplicant *wpa_s, char *content){
  if (msg_file == NULL)
    msg_file = fopen(msg_file_path, "w");

  struct os_time now;
  struct os_tm time;
  os_get_time(&now);
  os_gmtime(now.sec, &time);
  fprintf(msg_file, MACSTR "[%d-%d-%d %d:%d:%d] %s\n", MAC2STR(wpa_s->own_addr), time.year, time.month, time.day, time.hour, time.min, time.sec, content);
  fflush(msg_file);
}

static void fifo_message(struct wpa_supplicant *wpa_s, char *content){
  if(PIPE_FD == -1)
    PIPE_FD = open(PIPE, O_WRONLY);
  write(PIPE_FD, content, os_strlen(content));
  wpa_printf(MSG_INFO, "Arbiter fifo Write:%s", content);

}

void arbiter_message(struct wpa_supplicant *wpa_s, char* content){
  local_message(wpa_s, content);
  // fifo_message(wpa_s, content);
  wpa_msg(wpa_s, MSG_INFO, "Arbiter: %s", content);
  // TODO transfer to dbus messager
}

int parse_oui(int **ans, struct wpabuf* buf){
  // TODO
  return 0;
}

int ie_enable_interworking(struct wpa_bss *bss){
  const u8* pos = wpa_bss_get_ie(bss, EXTENDED_CAPABILITIES_IE_ID);
  if (pos == NULL)
    return 0;
  const u8* ie = pos + 2;
  return pos[1] >= 4 && (ie[3] & 0x80);
}

int ie_interworking_internet(struct wpa_bss *bss){
  const u8* pos = wpa_bss_get_ie(bss, INTERWORKING_IE_ID);
  if (pos == NULL)
    return 0;
  const u8* ie = pos + 2;
  return pos[1] >= 1 && (ie[0] & 0x10);
}

void display_candidates(struct wpa_supplicant *wpa_s, struct dl_list *list){
  filter_candidate *item = NULL;
  char ret[128];
  if (dl_list_empty(list))
    arbiter_message(wpa_s, "No network Left!");
  dl_list_for_each(item, list, filter_candidate, list){
    os_snprintf(ret, 128, "AP - %s", (char *)item->bss->ssid);
    arbiter_message(wpa_s, ret);
  }
}

int insert_string(char **pos, char **end, int num){
  if (num < 0 || num >= *end-*pos)
    return 0;
  else{
    *pos += num;
    return 1;
  }
}

static char* methodMap[] = {
  "EAP-NONE", "EAP-IDENTITY", "EAP-NOTIFICATION", "EAP-NAK", "EAP-MD5",
  "EAP-OTP", "EAP-GTC", NULL, NULL, NULL,
  NULL, NULL, NULL, "EAP-TLS", NULL,
  NULL, NULL, "EAP-LEAP", "EAP-SIM", NULL,
  NULL, "EAP-TTLS", NULL, "EAP-AKA", NULL,
  "EAP-PEAP", "EAP-MSCHAPV2", NULL, NULL, NULL,
  NULL, NULL, NULL, "EAP-TLV", NULL,
  NULL, NULL, NULL, "EAP-TNC", NULL,
  NULL, NULL, NULL, "EAP-FAST" , NULL,
  NULL, "EAP-PAX", "EAP-PSK", "EAP-SAKE", "EAP-IKEV2",
  "EAP-AKA-PRIME", "EAP-GPSK", "EAP-PWD", NULL, NULL
};

int abiter_append_eap_method(char **pos, char **end, u8 method){
  int ret;

  if (method == 254)
    ret = os_snprintf(*pos, *end-*pos, "[EPA-EXPANDED]");
  else if (method > 52)
    ret = os_snprintf(*pos, *end-*pos, "[UNDEFINED-EAP-METHOD]");
  else
    ret = os_snprintf(*pos, *end-*pos, "[%s]", methodMap[method]);

  return insert_string(pos, end, ret);
}

int is_free_public(struct wpa_bss *bss){
  const u8* pos = wpa_bss_get_ie(bss, INTERWORKING_IE_ID);
  if (pos == NULL || pos[1] < 1){
    return 0;
  }

  const u8* ie = pos + 2;
  int type = ie[0] & 0x0f;
  return (type == 3)? 1:0;
}
