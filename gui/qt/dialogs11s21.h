#ifndef DIALOGS11S21_H
#define DIALOGS11S21_H

#include <QDialog>

namespace Ui {
class dialogS11S21;
}

class dialogS11S21 : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogS11S21(QWidget *parent = 0);
  ~dialogS11S21();
  
private:
  Ui::dialogS11S21 *ui;
  void S11ToS21();
  void S11S21Cancel();
  void S11S21Series();
  void S11S21Shunt();
  void S11S21Generate();
  void S11ToS21Help(QString h);
  void S11S21ExplainEnd();

};

#endif // DIALOGS11S21_H
