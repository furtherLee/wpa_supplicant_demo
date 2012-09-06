#include <cstdio>

#include "hotspot2.h"
#include "signalbar.h"
#include "wpagui.h"
#include "networkconfig.h"
#include "eventhistory.h"
#include "wpamsg.h"
#include <QTextEdit>
#include <QScrollBar>

void Hotspot2::binding(){
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(autoSelectButton, SIGNAL(clicked()), this, SLOT(interworkingSelect()));
  connect(freshButton, SIGNAL(clicked()), this, SLOT(fresh()));
  connect(fetchANQPButton, SIGNAL(clicked()), this, SLOT(fetch()));
}

void Hotspot2::fetch(){
  char reply[64];
  size_t reply_len;
  wpagui->ctrlRequest("FETCH_ANQP", reply, &reply_len);
}

void Hotspot2::update(){
  // TODO
}

Hotspot2::Hotspot2(QWidget *parent, const char *, bool, Qt::WFlags)
	: QDialog(parent)
{
  setupUi(this);
  binding();
  // TODO
}

void Hotspot2::interworkingSelect(){
  char reply[64];
  size_t reply_len;
  wpagui->ctrlRequest("INTERWORKING_SELECT auto", reply, &reply_len);
}

void Hotspot2::append(QString str){
  bool scroll = true;
  
  arbiterInfoText->append(str);
  
  QScrollBar *sb = arbiterInfoText->verticalScrollBar();

  if (sb->value() <
      sb->maximum())
    scroll = false;
  
  if (scroll)
    sb->setValue(sb->maximum());

}

void Hotspot2::notify(WpaMsg msg){
  QString str = msg.getMsg();
  if (str.startsWith("ANQP fetch completed"))
    fresh();
  
  /*
   * Deal with Arbiter Message
   */

  if(!str.startsWith("Arbiter: "))
    return;

  append(str);
}

Hotspot2::~Hotspot2()
{
}

void Hotspot2::languageChange()
{
	retranslateUi(this);
}

void Hotspot2::setWpaGui(WpaGui *_wpagui)
{
	wpagui = _wpagui;
	fresh();
}

QString Hotspot2::hs20Support(QString flags){
  if(flags.contains("HS20"))
    return "YES";
  else 
    return "NO";
}

void Hotspot2::fresh(){
	char reply[4096];
	size_t reply_len;
	int index;
	char cmd[20];

	hs20APWidget->clear();

	index = 0;
	while (wpagui) {
		snprintf(cmd, sizeof(cmd), "ANQP_INFO %d", index++);
		if (index > 1000)
			break;

		reply_len = sizeof(reply) - 1;
		if (wpagui->ctrlRequest(cmd, reply, &reply_len) < 0)
			break;
		reply[reply_len] = '\0';

		QString bss(reply);
		if (bss.isEmpty() || bss.startsWith("FAIL"))
			break;

		/* Helper String */
		QString ie;

		/* Display String */
		QString ssid, bssid, hs20, internet, chargable, authMethod, roamingConsortium;

		QStringList lines = bss.split(QRegExp("\\n"));
		for (QStringList::Iterator it = lines.begin();
		     it != lines.end(); it++) {
			int pos = (*it).indexOf('=') + 1;
			if (pos < 1)
				continue;
			if ((*it).startsWith("bssid="))
			  bssid = (*it).mid(pos);
			if ((*it).startsWith("ssid="))
			  ssid = (*it).mid(pos);
			if ((*it).startsWith("hs20="))
			  hs20 = (*it).mid(pos);
			if ((*it).startsWith("internet="))
			  internet = (*it).mid(pos);
			if ((*it).startsWith("chargable="))
			  chargable = (*it).mid(pos);
			if ((*it).startsWith("authMethod="))
			  authMethod = (*it).mid(pos);
			if ((*it).startsWith("roamingConsortium="))
			  roamingConsortium = (*it).mid(pos);

		}

		QTreeWidgetItem *item = new QTreeWidgetItem(hs20APWidget);
		if (item) {
			item->setText(0, ssid);
			item->setText(1, bssid);
			item->setText(2, hs20);
			item->setText(3, internet);
			item->setText(4, chargable);
			item->setText(5, authMethod);
			item->setText(6, roamingConsortium);
		}
		if (bssid.isEmpty())
			break;
	}
}

QString Hotspot2::calcRoamingConsortium(QString str){
  return str;
}
