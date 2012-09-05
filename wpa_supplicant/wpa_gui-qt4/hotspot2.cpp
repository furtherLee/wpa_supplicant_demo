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

void Hotspot2::notify(WpaMsg msg){
  bool scroll = true;

  arbiterInfoText->append(msg.getMsg());
  
  QScrollBar *sb = arbiterInfoText->verticalScrollBar();

  if (sb->value() <
      sb->maximum())
    scroll = false;
  
  if (scroll)
    sb->setValue(sb->maximum());
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
