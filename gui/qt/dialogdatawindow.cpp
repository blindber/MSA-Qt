#include "dialogdatawindow.h"
#include "ui_dialogdatawindow.h"

dialogDataWindow::dialogDataWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::dialogDataWindow)
{
  ui->setupUi(this);
}

dialogDataWindow::~dialogDataWindow()
{
  delete ui;
}

void dialogDataWindow::clear()
{
  ui->plainTextEdit->clear();
}

void dialogDataWindow::addLine(QString line)
{
  ui->plainTextEdit->appendPlainText(line);
}

void dialogDataWindow::moveCursor(QTextCursor::MoveOperation operation, QTextCursor::MoveMode mode)
{
  ui->plainTextEdit->moveCursor(operation, mode);
}
