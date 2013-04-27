#ifndef DIALOGCHOOSEPRIMARYAXIS_H
#define DIALOGCHOOSEPRIMARYAXIS_H

#include <QDialog>

namespace Ui {
class dialogChoosePrimaryAxis;
}

class dialogChoosePrimaryAxis : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogChoosePrimaryAxis(QWidget *parent = 0);
  ~dialogChoosePrimaryAxis();
  int getPrimaryAxis();
  void setPrimaryAxis(int axis);
  bool cancelled;
  
private slots:
  void on_Done_clicked();

  void on_Cancel_clicked();

private:
  Ui::dialogChoosePrimaryAxis *ui;
};

#endif // DIALOGCHOOSEPRIMARYAXIS_H
