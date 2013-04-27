#ifndef DIALOGRLC_H
#define DIALOGRLC_H

#include <QDialog>

namespace Ui {
class dialogRLC;
}

class dialogRLC : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogRLC(QWidget *parent = 0);
  ~dialogRLC();
  void RLCDialog();
  
private:
  Ui::dialogRLC *ui;
  void RLCDialogSetSeries();
  void RLCDialogSetShunt();
  void RLCCancel();
  void RLCParOn();
  void RLCParReset();
  void RLCSerOn();
  void RLCSerReset();
  void RLCROn();
  void RLCROff();
  void RLCLOn();
  void RLCLOff();
  void RLCCOn();
  void RLCCOff();
  void RLCAdjustItemsToCoax();
  void RLCSelectCoax();
  void RLCDialogFinished();
  void RLCSetTermStatus(QString stat);
  void RLCSpecHelp(QString h);
  void RLCSpecHelpEnd();

};

#endif // DIALOGRLC_H
