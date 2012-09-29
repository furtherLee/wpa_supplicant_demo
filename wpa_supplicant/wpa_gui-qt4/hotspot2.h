#ifndef _HS20_H_
#define _HS20_H_

#include <QObject>
#include "ui_hotspot2.h"

class WpaGui;
class WpaMsg;

class Hotspot2: public QDialog, public Ui::Hotspot2Dialog{
  Q_OBJECT
  
public:
  Hotspot2(QWidget *parent = 0, const char *name = 0,
	      bool modal = false, Qt::WFlags fl = 0);
  ~Hotspot2();
		
public slots:
  virtual void setWpaGui(WpaGui *_wpagui);
  virtual void fresh();
  virtual void setOUI(const QString &oui);
protected slots:
  virtual void languageChange();

private slots:
  virtual void interworkingSelect();
  virtual void fetch();
  
private:
  void binding();
  void update();
  void append(QString str);
  void addMap();
  QString getConsortium(QString query);
  QString getAccessNetworkType(QString query);
  QString buildAlgOptions();
  QString getOuiCode(const QString &name);
  void highlight(QString str);
public:
  virtual void notify(WpaMsg msg);
private:
  WpaGui *wpagui;
  QMap<QString, QString*> OUIMap;
  QMap<QString, QString*> accessTypeMap;
  int filterStage;
  QMap<int, QString*> stageColorMap;
  bool colored;
};

#endif
