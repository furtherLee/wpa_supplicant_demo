#ifndef _HS20_CONFIG_H_
#define _HS20_CONFIG_H_

#include <QObject>
#include "ui_hotspot2config.h"

class WpaGui;
class WpaMsg;

class Hotspot2Config: public QDialog, public Ui::Hotspot2ConfigDialog{
  Q_OBJECT
  
public:
  Hotspot2Config(QWidget *parent = 0, const char *name = 0,
	      bool modal = false, Qt::WFlags fl = 0);
  ~Hotspot2Config();
		
public slots:
  virtual void setWpaGui(WpaGui *_wpagui);
  virtual void search(const QString &);
					 
protected slots:
  virtual void languageChange();

private slots:
  virtual void save();
  virtual void uploadCert();
private:
  WpaGui *wpagui;
  QMap<QString, QString*> OUIMap;

private:
  void binding();
  void addMap();
};

#endif
