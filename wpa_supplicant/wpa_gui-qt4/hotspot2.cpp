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
  // TODO
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
}
