#include <cstdio>

#include "hotspot2.h"
#include "signalbar.h"
#include "wpagui.h"
#include "networkconfig.h"
#include "eventhistory.h"
#include "wpamsg.h"
#include <QTextEdit>
#include <QScrollBar>

Hotspot2Config::Hotspot2Config(QWidget *parent, const char *, bool, Qt::WFlags)
  : QDialog(parent){
  setupUi(this);
  binding();
  addMap();
}

void Hotspot2Config::binding(){
  connect(ouiEdit, SIGNAL(textChanged(const QString &)), this, SLOT(search(const QString &)));
}

void Hotspot2Config::addMap(){
  OUIMap.insert("00071c", new QString("AT&T Wireless"));
  OUIMap.insert("001bc504bd", new QString("Silicon Controls"));
  OUIMap.insert("506f9a", new QString("Wi-Fi Alliance"));
  OUIMap.insert("0050f2", new QString("Microsoft"));
}

Hotspot2Config::~Hotspot2Config(){

}

void Hotspot2Config::languageChange()
{
	retranslateUi(this);
}

void Hotspot2Config::setWpaGui(WpaGui *_wpagui){
	wpagui = _wpagui;
}

void Hotspot2Config::save(){
  done(0);
}

void Hotspot2Config::search(const QString &text){
  QString ans;
  if (OUIMap.contains(text))
    ans = *OUIMap.value(text);
  else
    ans = "Unknown";
    
  carrierNameLab->setText(ans);
  
}

void Hotspot2Config::uploadCert(){

}




