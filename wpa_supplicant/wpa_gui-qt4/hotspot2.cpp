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
  //  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(autoSelectButton, SIGNAL(clicked()), this, SLOT(interworkingSelect()));
  //  connect(freshButton, SIGNAL(clicked()), this, SLOT(fresh()));
  //  connect(fetchANQPButton, SIGNAL(clicked()), this, SLOT(fetch()));
}

void Hotspot2::addMap(){
  OUIMap.insert("00071c", new QString("AT&T Wireless"));
  OUIMap.insert("001bc504bd", new QString("Silicon Controls"));
  OUIMap.insert("506f9a", new QString("Wi-Fi Alliance"));
  OUIMap.insert("0050f2", new QString("Microsoft"));

  accessTypeMap.insert("UNSET", new QString("UNSET"));
  accessTypeMap.insert("0", new QString("Private Network"));
  accessTypeMap.insert("1", new QString("Private Network with Guest Access"));
  accessTypeMap.insert("2", new QString("Chargable Public Network"));
  accessTypeMap.insert("3", new QString("Free Public Network"));
  accessTypeMap.insert("4", new QString("Personal Device Network"));
  accessTypeMap.insert("5", new QString("Emergency Service Only Network"));
  accessTypeMap.insert("14", new QString("Test or Experimental"));
  accessTypeMap.insert("15", new QString("Wildcardx"));

  stageColorMap.insert(0, new QString("gainsboro"));
  stageColorMap.insert(1, new QString("purple"));
  stageColorMap.insert(2, new QString("orange"));
  stageColorMap.insert(3, new QString("red"));
  stageColorMap.insert(4, new QString("lightgreen"));  
}

QString Hotspot2::getAccessNetworkType(QString query){
  if (!accessTypeMap.contains(query))
    return query;
  return *accessTypeMap.value(query);
}

QString Hotspot2::getConsortium(QString query){
  if (!OUIMap.contains(query))
    return query;
  return *OUIMap.value(query);
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
  addMap();
  colored = false;
}

void Hotspot2::setOUI(const QString &oui){
  ouiLab->setText("Home Carrier: " + oui);
}

void Hotspot2::interworkingSelect(){
  char reply[64];
  size_t reply_len;
  colored = false;
  filterStage = 0;
  wpagui->disconnect();
  wpagui->showTrayMessage(QSystemTrayIcon::Information, 5, "Start Interworking Select");
  wpagui->ctrlRequest("INTERWORKING_SELECT auto", reply, &reply_len);
  arbiterInfoText->clear();
  fresh();
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

void Hotspot2::highlight(QString str){
  QList<QTreeWidgetItem *> list = hs20APWidget->findItems(str, Qt::MatchExactly, 0);
  if (list.size() != 1)
    return;
  QTreeWidgetItem *item = list.first();
  QColor color(*stageColorMap.value(filterStage));
  QBrush b (color);
  for (int i = 0; i < hs20APWidget->columnCount(); ++i)
    item->setBackground(i, b);

}

void Hotspot2::notify(WpaMsg msg){
  QString str = msg.getMsg();
  if (str.startsWith("ANQP fetch completed") || str.startsWith("Arbiter: ANQP Information Received"))
    fresh();
  bool stageChange = false;

  if (str.startsWith("Arbiter: Get ANQP Answers from the following APs: ")){
    filterStage = 0;
    stageChange = true;
    colored = true;
  }
    
  if (str.startsWith("Arbiter: Further Filter out AP")){
    stageChange =true;
    filterStage++;
    colored = true;
  }
  
  if (str.startsWith("Arbiter: AP - "))
    highlight(str.mid(str.indexOf("Arbiter: AP - ") + 14));  
  /*
   * Deal with Arbiter Message
   */

  if(!str.startsWith("Arbiter: "))
    return;

  QString output = str.mid(str.indexOf("Arbiter: ") + 9);

  if (output.startsWith("Decide "))
    output = output + " because it is free, public, using [EAP-TTLS], and contracted with AT&T Wireless";

  if (colored)
    output = "<font color=\"" + *stageColorMap.value(filterStage) + "\">" +  output + "</font>";
  else
    output = "<font color=\"black\">" + output + "</font>";

  append(output);
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
	setOUI(wpagui->getOUI());
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

		//		printf("%s\n", reply);

		QString bss(reply);
		if (bss.isEmpty() || bss.startsWith("FAIL"))
			break;

		/* Helper String */
		QString ie;

		/* Display String */
		QString ssid, bssid, hs20, internet, chargable, authMethod, roamingConsortium, consortiumList;

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
			  consortiumList = (*it).mid(pos);
		}

		QStringList consortiums = consortiumList.split(QRegExp(","));
		for (QStringList::Iterator con = consortiums.begin();
		     con != consortiums.end(); con++){
		  if ((*con).isEmpty())
		    continue;
		  roamingConsortium.append("[" + getConsortium(*con) + "]");
		}

		chargable = getAccessNetworkType(chargable);

		QTreeWidgetItem *item = new QTreeWidgetItem(hs20APWidget);
		if (item) {
			item->setText(0, ssid);
			//			item->setText(1, bssid);
			item->setText(1, hs20);
			item->setText(2, internet);
			item->setText(3, chargable);
			item->setText(4, authMethod);
			item->setText(5, roamingConsortium);
			for(int i = 0; i < hs20APWidget->columnCount(); ++i)
			  item->setTextAlignment(i, Qt::AlignHCenter);
		}
		if (bssid.isEmpty())
			break;
	}
	for (int i = 0; i < hs20APWidget->columnCount(); ++i)
	  hs20APWidget->resizeColumnToContents(i);
	hs20APWidget->sortByColumn(1, Qt::DescendingOrder);
}
