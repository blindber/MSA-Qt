#ifndef DIALOGRLC_H
#define DIALOGRLC_H

#include <QDialog>
#include "constants.h"
#include "msautilities.h"

namespace Ui {
class dialogRLC;
}
class dialogRLCStruct
{
public:
  int msaMode;
  QString dialogRLCConnect;
  QString dialogCoaxSpecs;
  QString dialogCoaxName;
  float S21JigR0;
  QString S21JigAttach;
  double dialogRValue;
  double dialogLValue;
  double dialogQLValue;
  double dialogCValue;
  double dialogQCValue;
};
class dialogRLC : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogRLC(QWidget *parent = 0);
  ~dialogRLC();
  void RLCDialog();
  void setSettings(dialogRLCStruct settings);
  void getSettings(dialogRLCStruct &settings);

  void DialogRLCConnect( QString t);
  void DialogRValue( float t);
  void DialogLValue( float t);
  void DialogQLValue( float t);
  void DialogQCValue( float t);
  void DialogCValue( float t);
  void DialogCoaxSpecs( QString t);
  void DialogCoaxName(QString t);

  QString DialogRLCConnect();
  float DialogRValue();
  float DialogLValue();
  float DialogQLValue();
  float DialogQCValue();
  float DialogCValue();
  QString DialogCoaxSpecs();
  QString DialogCoaxName();
  
private:
  Ui::dialogRLC *ui;
  //globalVars *vars;
  msaUtilities util;

  void RLCSetTermStatus(QString stat);

  dialogRLCStruct settings;

private slots:
  void RLCSpecHelp();
  void on_checkBox_R_clicked(bool checked);
  void on_checkBox_L_clicked(bool checked);
  void on_checkBox_C_clicked(bool checked);
  void on_comboBoxCoaxTypes_currentIndexChanged(const QString &cName);
  void RLCDialogFinished();

};

#endif // DIALOGRLC_H
