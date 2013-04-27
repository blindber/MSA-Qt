#include "dialogsmithchart.h"
#include "ui_dialogsmithchart.h"

dialogSmithChart::dialogSmithChart(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogSmithChart)
{
  ui->setupUi(this);
  resize(430,400);

  scene = new QGraphicsScene(this);
  ui->graphicsView->setScene(scene);

  QBrush redBrush(Qt::red);
  QBrush blueBrush(Qt::blue);
  QPen blackPen(Qt::black);

  //ell = scene->addEllipse(10,10,100,100,blackPen,redBrush);
  QPainterPath* path = new QPainterPath();
 path->arcMoveTo(0,0,50,50,20);
 path->arcTo(0,0,50,50,20, 90);

 scene->addPath(*path);
 //ui->graphicsView->sc
  //h = 430; w = 400
//  centerX = 195;
//  CenterY = 191;

}

dialogSmithChart::~dialogSmithChart()
{
  delete ui;
}
void dialogSmithChart::smithAdjustSize()
{
  int CenterX = ui->graphicsView->width() / 2;
  int CenterY = ui->graphicsView->height() / 2;
//  'Note the graphics box itself auto resizes
//   #smithHndl$ "home"
//   #smithHndl$ "posxy CenterX CenterY"
   smithCenterX = CenterX;
   smithCenterY = CenterY;

   smithRadius = 4*(int(qMin(CenterX, CenterY)-65)/4);
   // 'Make radius a multiple of 4 'ver115-2c
   smithLeftX = smithCenterX - smithRadius;
   smithRightX = smithCenterX + smithRadius;
   smithTopY = smithCenterY - smithRadius;
   smithBottomY = smithCenterY + smithRadius;
}
