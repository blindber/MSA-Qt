#ifndef DIALOGMARKEROPTIONS_H
#define DIALOGMARKEROPTIONS_H

#include <QDialog>

namespace Ui {
class dialogMarkerOptions;
}

class dialogMarkerOptions : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogMarkerOptions(QWidget *parent = 0);
  ~dialogMarkerOptions();
  void mMenuMarkerOptions();
  
private:
  Ui::dialogMarkerOptions *ui;
  void markNil();
  void markBtnCancel();
  void markFinished();

};

#endif // DIALOGMARKEROPTIONS_H
