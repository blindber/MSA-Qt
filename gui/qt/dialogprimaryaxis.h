#ifndef DIALOGPRIMARYAXIS_H
#define DIALOGPRIMARYAXIS_H

#include <QDialog>

namespace Ui {
class dialogPrimaryAxis;
}

class dialogPrimaryAxis : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogPrimaryAxis(QWidget *parent = 0);
  ~dialogPrimaryAxis();
  void menuPrimaryAxis();
  
private:
  Ui::dialogPrimaryAxis *ui;
  void ChoosePrimaryAxis();
  void primarySelY1();
  void primarySelY2();
  void primaryDone();
  void primaryCancel();

};

#endif // DIALOGPRIMARYAXIS_H
