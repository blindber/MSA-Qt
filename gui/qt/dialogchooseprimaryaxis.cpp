#include "dialogchooseprimaryaxis.h"
#include "ui_dialogchooseprimaryaxis.h"

dialogChoosePrimaryAxis::dialogChoosePrimaryAxis(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogChoosePrimaryAxis)
{
  ui->setupUi(this);
  cancelled = true;
}

dialogChoosePrimaryAxis::~dialogChoosePrimaryAxis()
{
  delete ui;
}

int dialogChoosePrimaryAxis::getPrimaryAxis()
{
 if (ui->Y1->isChecked())
 {
   return 1;
 }
 else
 {
   return 2;
 }
}

void dialogChoosePrimaryAxis::setPrimaryAxis(int axis)
{
  if (axis == 1)
  {
    ui->Y1->setChecked(true);
  }
  else
  {
    ui->Y2->setChecked(true);
  }
}

void dialogChoosePrimaryAxis::on_Done_clicked()
{
  cancelled = false;
  close();
}

void dialogChoosePrimaryAxis::on_Cancel_clicked()
{
  cancelled = true;
  close();
}
