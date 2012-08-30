#include "includes.h"
#include "util.h"

static FILE *msg_file = NULL;

static const char msg_file_path[] = "/tmp/arbiter_msg.log";

static void local_message(struct wpa_supplicant *wpa_s, char *content){
  if (msg_file == NULL)
    msg_file = fopen(msg_file_path, "w");

  struct os_time now;
  struct os_tm time;
  os_get_time(&now);
  os_gmtime(os_get_time(&now), &time);
  fprintf(msg_file, MACSTR "[%d-%d-%d %d:%d:%d] %s\n", MAC2STR(wpa_s->own_addr), time.year, time.month, time.day, time.hour, time.min, time.sec, content);
  fflush(msg_file);
}

void arbiter_message(struct wpa_supplicant *wpa_s, char* content){
  local_message(wpa_s, content);
  // TODO transfer to dbus messager
}

int parse_oui(int **ans, struct wpabuf* buf){
  // TODO
  return 0;
}
