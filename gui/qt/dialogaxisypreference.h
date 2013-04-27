#ifndef DIALOGAXISYPREFERENCE_H
#define DIALOGAXISYPREFERENCE_H

#include <QDialog>

namespace Ui {
class dialogAxisYPreference;
}

class dialogAxisYPreference : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogAxisYPreference(QWidget *parent = 0);
  ~dialogAxisYPreference();
  void DisplayAxisYPreference(int axisNum, int doTwoPort);
  
private:
  Ui::dialogAxisYPreference *ui;
  void SelectGraphType();
  void axisYDefaultOn();
  //void LeftButDouble();
  void TwoPortLeftButDouble();
  void axisYDoNothing();
  void axisYChangeGraph();
  void axisAutoscaleOn();
  void PickColor();
  void axisYCancel();
  void axisYFinished();

};

#endif // DIALOGAXISYPREFERENCE_H
