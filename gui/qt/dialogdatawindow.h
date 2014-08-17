#ifndef DIALOGDATAWINDOW_H
#define DIALOGDATAWINDOW_H

#include <QMainWindow>
#include <QTextCursor>

namespace Ui {
class dialogDataWindow;
}

class dialogDataWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit dialogDataWindow(QWidget *parent = 0);
  ~dialogDataWindow();
  void clear();
  void addLine(QString line);
  void moveCursor(QTextCursor::MoveOperation operation, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);

private:
  Ui::dialogDataWindow *ui;
};

#endif // DIALOGDATAWINDOW_H
