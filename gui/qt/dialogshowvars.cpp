#include "dialogshowvars.h"
#include "ui_dialogshowvars.h"

dialogShowVars::dialogShowVars(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogShowVars)
{
  ui->setupUi(this);
}

dialogShowVars::~dialogShowVars()
{
  delete ui;
}

void dialogShowVars::update(const QStringList vars)
{
  text = vars.join("\n");

  QWidget::update();

}

void dialogShowVars::paintEvent(QPaintEvent *e)
{
  QPainter painter(this);
  QFont font = painter.font();
  font.setBold(true);
  font.setPointSize(font.pointSize() + 1);
  font.setFixedPitch(true);

  painter.setFont(font);
  painter.drawText(rect(), Qt::AlignLeft, text);
}
