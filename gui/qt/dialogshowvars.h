#ifndef DIALOGSHOWVARS_H
#define DIALOGSHOWVARS_H

#include <QDialog>
#include <QPainter>
#include <QPaintEvent>

namespace Ui {
class dialogShowVars;
}

class dialogShowVars : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogShowVars(QWidget *parent = 0);
  ~dialogShowVars();
  void update(const QStringList vars);
private:
  Ui::dialogShowVars *ui;
  QString text;

protected:
  void paintEvent(QPaintEvent *e);
};

#endif // DIALOGSHOWVARS_H
