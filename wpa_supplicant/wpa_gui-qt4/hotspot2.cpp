#include <cstdio>

#include "hotspot2.h"
#include "signalbar.h"
#include "wpagui.h"
#include "networkconfig.h"

Hotspot2::Hotspot2(QWidget *parent, const char *, bool, Qt::WFlags)
	: QDialog(parent)
{
	setupUi(this);
	// TODO
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


