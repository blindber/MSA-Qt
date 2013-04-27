#ifndef DIALOGCALPDM_H
#define DIALOGCALPDM_H

#include <QDialog>

namespace Ui {
class dialogCalPDM;
}

class dialogCalPDM : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogCalPDM(QWidget *parent = 0);
  ~dialogCalPDM();
  void menuCalPDM();
  
private:
  Ui::dialogCalPDM *ui;
  void PDMCalWait();
  void PDMCalCancel();
  void PDMCalFinished();
  void DoPDMCal();

};

#endif // DIALOGCALPDM_H
