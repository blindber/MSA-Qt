#include "smithdialog.h"
#include "ui_smithdialog.h"
#include <QGraphicsTextItem>

smithDialog::smithDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::smithDialog)
{
  ui->setupUi(this);
  scene = new QGraphicsScene();
  ui->graphicsView->setScene(scene);
  //scene->addRect(-500,-500,1000,1000);
  drawGrid();
  scene->setSceneRect(scene->itemsBoundingRect());
  ui->graphicsView->fitInView(scene->sceneRect(),Qt::KeepAspectRatio);
}

smithDialog::~smithDialog()
{
  delete scene;
  delete ui;
}

void smithDialog::drawGrid()
{
  drawArcs(0.2);
  drawArcs(0.5);
  drawArcs(1.0);
  drawArcs(2.0);
  drawArcs(4.0);
}

void smithDialog::drawArcs(float R)
{
  float R0 = 50.0;
  QPen pen(QColor(Qt::black),1);
  // resistance arcs
  float s11r = (R-1) / (R+1);
  float xcent = (s11r + 1) / 2;
  float radius = 1 - xcent;
  float smRad = qMin(width(), height())/2 - 5;
  QPainterPath path;
  //path.moveTo(smRad, 0);
  path.addEllipse(QPointF(xcent*smRad, 0), radius*smRad, radius*smRad);
  scene->addPath(path, pen);
  QString text = QString::number(R*R0);
  QGraphicsTextItem *txt = scene->addText(text);
  QFontMetrics fm(scene->font());
  QRectF rect = fm.tightBoundingRect(text);
  float x, y;
  x = s11r*smRad + rect.width()/2 +2;
  y = rect.top();
  txt->setPos(x,y);

  //path.arcTo();
  // reactance arcs
  /*
  float s11 = (1j*R-1) / (1j*R+1);
  theta = angle(s11)
  ycent = tan(theta/2)
  theta2 = atan2(s11.imag-ycent, s11.real-1)
  ycent *= smRad
  path = gc.CreatePath()
  path.MoveToPoint(smRad, 0)
  path.AddArc((smRad, -ycent), ycent, pi/2, -theta2)
  path.MoveToPoint(s11.real*smRad, s11.imag*smRad)
  path.AddArc((smRad,  ycent), ycent, theta2, -pi/2)
  gc.StrokePath(path)
  txrad = smRad + sqrt(tw**2 + th**2)/2 + 1
  gc.DrawText(text, s11.real*txrad-tw/2, -s11.imag*txrad-th/2)
  text = si(-R*R0)
  tw, th = gc.GetTextExtent(text)
  gc.DrawText(text, s11.real*txrad-tw/2,  s11.imag*txrad-th/2)
*/
}

void smithDialog::showEvent(QShowEvent *event)
{
  ui->graphicsView->fitInView(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
  QWidget::showEvent(event);
}

void smithDialog::resizeEvent(QResizeEvent *event)
{
  ui->graphicsView->fitInView(scene->sceneRect(),Qt::KeepAspectRatio);
}
