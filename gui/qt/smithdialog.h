#ifndef SMITHDIALOG_H
#define SMITHDIALOG_H

#include <QDialog>
#include <QGraphicsScene>

namespace Ui {
class smithDialog;
}

class smithDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit smithDialog(QWidget *parent = 0);
  ~smithDialog();
  void drawGrid();
  
private:
  Ui::smithDialog *ui;
  QGraphicsScene *scene;

  void drawArcs(float R);

protected:
  virtual void showEvent(QShowEvent *event);
  virtual void resizeEvent(QResizeEvent *event);
};

#endif // SMITHDIALOG_H
