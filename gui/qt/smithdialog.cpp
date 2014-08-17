#include "smithdialog.h"

#include "ui_smithdialog.h"
#include <QGraphicsTextItem>
#include <math.h>
#include <qdebug.h>
#include "constants.h"

#define PI 3.14159265

class QGraphicsArcItem : public QGraphicsEllipseItem {
public:
    QGraphicsArcItem ( qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0 ) :
        QGraphicsEllipseItem(x, y, width, height, parent) {
    }

protected:
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
        painter->setPen(pen());
        painter->setBrush(brush());
        painter->drawArc(rect(), startAngle(), spanAngle());

        // fix me, need to adjust the rect size so that it's only as big as the arc

//        if (option->state & QStyle::State_Selected)
//            qt_graphicsItem_highlightSelected(this, painter, option);
    }
};


smithDialog::smithDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::smithDialog)
{
  ui->setupUi(this);
  connect(this, SIGNAL(smithRefreshMain(int)), parent, SLOT(smithRefreshMain(int)));
  connect(this, SIGNAL(gGetMinMaxPointNum(int &, int &)), parent, SLOT(gGetMinMaxPointNum(int &, int &)));

  scene = new QGraphicsScene();
  scene->setSceneRect(scene->itemsBoundingRect());
  ui->graphicsView->fitInView(scene->sceneRect());
  ui->graphicsView->setScene(scene);
  scene->clear();
  gGetTitleLine[0] = "Title Line 0";
  gGetTitleLine[1] = "Title Line 1";
  gGetTitleLine[2] = "Title Line 2";
  gGetTitleLine[3] = "Title Line 3";

  smithUseLines = 1;
}

smithDialog::~smithDialog()
{
  delete scene;
  delete ui;
}

void smithDialog::doit()
{
  S11GraphR0 = 50;
  S11BridgeR0 = 50;

  smithR0 = S11GraphR0;
  smithRadius = qMin(width(), height()) / 2   - 5;

  smithRightX = smithRadius ;
  smithLeftX = -smithRadius ;

  smithTop = -smithRadius;
  smithBottom = smithRadius;

  smithCenterY = 0;
  smithCenterX = 0;

  smithDrawChart();

  show();
}

void smithDialog::smithDrawChart()
{
  scene->clear();
  scene->setBackgroundBrush(smithBackColor);
  QFont font = scene->font();
  font.setWeight(7);
  scene->setFont(font);

  // draw constant resistance arcs
  smithDrawResist(4*S11GraphR0, true);
  smithDrawResist(2*S11GraphR0, true);
  smithDrawResist(S11GraphR0, true);
  smithDrawResist(S11GraphR0/2, true);
  smithDrawResist(S11GraphR0/5, true);
  smithDrawResist(0, false);

  smithDrawTitle();
  smithDrawFooter();


  // we set the scene rect here as the smithDrawReact breaks the itemsBoundingRect
  QRectF rect = scene->itemsBoundingRect();
  scene->setSceneRect(scene->itemsBoundingRect());
  scene->setSceneRect(rect);
  ui->graphicsView->fitInView(scene->sceneRect(),Qt::KeepAspectRatio);


  // draw constant reactance arcs
  smithDrawReact(S11GraphR0, true);

  smithDrawReact(0-S11GraphR0, true);
  smithDrawReact(0, true);

  smithDrawReact(S11GraphR0/5, true);
  smithDrawReact(S11GraphR0/2, true);
  smithDrawReact(2*S11GraphR0, true);
  smithDrawReact(4*S11GraphR0, true);

  smithDrawReact(0-S11GraphR0/5, true);
  smithDrawReact(0-S11GraphR0/2, true);
  smithDrawReact(0-2*S11GraphR0, true);
  smithDrawReact(0-4*S11GraphR0, true);
}

void smithDialog::smithSetChartColors(QColor backCol, QColor lineCol, QColor boundsCol)
{
  smithBackColor = backCol;
  smithLineColor = lineCol;
  smithBoundsColor = boundsCol;
}

void smithDialog::smithRefresh(int doReference)
{
  //Refresh the graph using current chart bitmap and current ReflectArray() or TwoPortArray() ver115-6b
  //We also draw a reference line if doReference=1
  //call smithCopyParams    //Trace width, etc.
  smithDrawChart();

  if (doReference==1)
    smithDrawReferenceSource();
  int haveData;
  // fix me smithDoingRef=doReference;
  /*pNum=TwoPortGetSelectedParam();
      if twoPortWinHndl$<>"" then //ver116-4b
          //Two port window is open so we draw with its data
          haveData=0
          if pNum=1 or pNum=4 or pNum=5 or pNum=8 then haveData=1
          if haveData then call smithDrawTwoPortData //Draw selected param from TwoPort Module
      else*/
  {
    haveData=1;
    smithDrawReflectArray();
  }
  if (haveData)
  {
    smithDisplayMarkerInfo();
    smithDrawTitle();
//    fixme fix me if (smithGraphMarkers)
  //    smithDrawMarkers();
  }

}

void smithDialog::smithDrawResist(float resist, bool doLabel)
{
  if (resist < 0)
    return;

  double normResist = resist / smithR0;
  double arcRadius = (int)(smithRadius * (1 - (normResist - 1)/(normResist + 1))/2);
  float centX = smithRightX - arcRadius;
  centX++;

  if (resist == 0)
  {
    arcRadius++;
  }

  int penWidth = 1;
  if (resist == 0)
  {
    penWidth = 2;
  }
  QPen pen(smithLineColor,penWidth);
  QPainterPath path;
  path.addEllipse(QPointF(centX, 0), arcRadius, arcRadius);
  scene->addPath(path, pen);

  if (doLabel)
  {
    QString text = QString::number(resist);
    QGraphicsTextItem *txt = scene->addText(text);
    txt->setDefaultTextColor(smithLineColor);
    QFontMetrics fm(scene->font());
    QRectF rect = fm.tightBoundingRect(text);
    float x, y;
    x = smithRightX - 2 *arcRadius +3;
    y = rect.top();
    txt->setPos(x,y*2);
  }
}

void smithDialog::smithDrawReact(double react, bool doLabel)
{
  QPen pen(smithLineColor,1);
  if (react == 0)
  {
    QPainterPath path;
    path.moveTo(smithLeftX, smithCenterY);
    path.lineTo(smithRightX, smithCenterY);
    scene->addPath(path, pen);

    QString text = "0";
    QGraphicsTextItem *txt = scene->addText(text);
    QFontMetrics fm(scene->font());
    txt->setDefaultTextColor(smithLineColor);
    QRectF rect = fm.tightBoundingRect(text);
    float x, y;
    x = smithLeftX - rect.width() *2;
    y = rect.top();
    txt->setPos(x,y*2);
    return;
  }

  double angle;
  double arcRadius=(fabs(smithR0/react)*smithRadius) * 2;
  int startAngle = 270;

  angle = smithRadius / arcRadius * 2;
  angle = atan(angle) * 180 / PI;
  angle = angle * 2;

  QGraphicsArcItem* item;
  if (react < 0)
  {
    startAngle = 90;
    item = new QGraphicsArcItem(smithRightX - arcRadius/2, 0, arcRadius, arcRadius);
    item->setStartAngle(startAngle * 16);
    item->setSpanAngle(angle * 16);
  }
  else
  {
    item = new QGraphicsArcItem(smithRightX - arcRadius/2, 0 - arcRadius, arcRadius, arcRadius);
    item->setStartAngle(startAngle * 16);
    item->setSpanAngle(-angle * 16);
  }
  item->setPen(smithLineColor);

  scene->addItem(item);

  QString text = QString::number(react);
  QGraphicsTextItem *txt = scene->addText(text);
  txt->setDefaultTextColor(smithLineColor);
  QFontMetrics fm(scene->font());
  QRectF rect = fm.tightBoundingRect(text);
  float x, y;
  x = smithRightX - arcRadius * sin((angle) / 180.0 * PI) / 2;
  y = arcRadius * sin((90.0-angle) / 180.0 * PI)/2 - arcRadius/2;

  if (fabs(react) == 50)
  {
    x = -rect.width() / 2;
    y=y-5;
  }
  else if (x < 0)
  {
    x = x - rect.width() - 10;
  }
  if (react < 0)
  {
    y = -y - rect.height();
  }
  else
  {
    y = y - rect.height() * 2;
  }
  txt->setPos(x,y);
}

void smithDialog::smithDrawTitle()
{
  QFontMetrics fm(scene->font());
  QRectF rect = fm.tightBoundingRect("w");
  double height = rect.height();

  smithPrintTextCentered(gGetTitleLine[1], smithCenterX, smithTop - height * 11);
  smithPrintTextCentered(gGetTitleLine[2], smithCenterX, smithTop - height * 9);
  smithPrintTextCentered(gGetTitleLine[3], smithCenterX, smithTop - height * 7);
  //gGetXAxisRange(fMin, fMax);     //Note these are off if a full scan has not been done
  QString fMin = "min";
  QString fMax = "max";
  smithPrintTextCentered( fMin+" MHz to "+fMax+" MHz", smithCenterX, smithTop - height * 5);
}

void smithDialog::smithDrawFooter()
{
  QFontMetrics fm(scene->font());
  QRectF rect = fm.tightBoundingRect("w");
  double height = rect.height();

  smithPrintTextCentered(gGetTitleLine[3], smithCenterX, smithBottom + height * 7);
  QString fMin = "min";
  QString fMax = "max";
  smithPrintTextCentered( fMin+" MHz to "+fMax+" MHz", smithCenterX, smithBottom + height * 5);
}
void smithDialog::smithPrintText(QString t, double x, double y)     //Print text t$ at position x,y on graph; font and color are preset
{
  QFontMetrics fm(scene->font());
  QGraphicsTextItem *item;
  item = scene->addText(t);
  item->setDefaultTextColor(smithMarkerColor);
  item->setPos(x,y);
}

void smithDialog::smithPrintTextCentered(QString t, double x, double y)     //Print text t$ centered at x,y on graph
{
  QFontMetrics fm(scene->font());
  int w = fm.width(t);
  smithPrintText(t, x-w/2,y);
}

void smithDialog::smithPrintTextRightJust(QString t, double x, double y)     //Print text t$ right adjusted at x,y on graph
{
  QFontMetrics fm(scene->font());
  int w = fm.width(t);
  smithPrintText(t, x-w,y);
}

void smithDialog::smithDrawReferenceSource()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Draw trace from referenceSource()
    'Note referenceSource is done by points, starting at 1, not steps starting at 0.
    call gGetMinMaxPointNum startPt, endPt  'ver115-1e
    if endPt<2 then exit sub

    #smithHndl$, "color ";smithReferenceColor$  'Set color but don't add to accumulated trace
    #smithHndl$, "size ";smithTraceWidth

        'Set first point
    call smithSetPointS11DB referenceSource(startPt,1), referenceSource(startPt,2)     'Set first point
    for i=startPt+1 to endPt  'point startPt is already done; draw the remaining lines
        call smithDrawLineS11DB referenceSource(i,1), referenceSource(i,2)    'Draw to next point
    next i
    if smithUseLines then call smithDrawLineS11DB referenceSource(endPt-1,1), referenceSource(endPt-1,2)    'Draw final segment backwards to get final point right
    #smithHndl$, "flush"
   * */
}

void smithDialog::smithDrawReflectArray()
{
  //Draw trace from ReflectArray()
  int startPt, endPt;
  gGetMinMaxPointNum(startPt, endPt);
  int startStep=startPt-1;
  int endStep=endPt-1;
  if (endStep<1)
    return;

  int lineWidth = smithTraceWidth;
  if (smithUseLines)
  {
    lineWidth = smithTraceWidth+2;  //First point is extra wide, but not for circles
  }

  //Set first point and add the set command to smithCurrTrace$
  smithSetPointS11DB(vars->ReflectArray[startStep][constGraphS11DB], vars->ReflectArray[startStep][constGraphS11Ang], lineWidth);     //Set first point
  lineWidth = smithTraceWidth;

  for (int i=startStep+1; i < endStep; i++)  //step startStep is already done; draw the remaining lines
  {
    smithDrawLineS11DB(vars->ReflectArray[i][constGraphS11DB], vars->ReflectArray[i][constGraphS11Ang], lineWidth);    //Draw to next point

  }
  if (smithUseLines)
  {
    smithDrawLineS11DB(vars->ReflectArray[endStep-1][constGraphS11DB], vars->ReflectArray[endStep-1][constGraphS11Ang], lineWidth);    //Draw final segment backwards to get final point right
  }
}
void smithDialog::smithDisplayMarkerInfo()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;

  //For now, we display just the selected marker
  if (vars->calInProgress) //Data is garbage during cal
  {
    return;
  }
  /*
    if selMarkerID$=""  then exit sub    //No selected marker
    markPoint=gMarkerPointNum(mMarkerNum(selMarkerID$))
    call smithDisplayReflectInfo markPoint //Also sets font and color
    call smithPrintText "Marker ";selMarkerID$;"  ", 10, smithBottomY+14    //Couple extra blanks to clear anything there
    #smithHndl$, "flush"
    */
}

void smithDialog::smithDrawMarkers()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  //Draw all listed markers except halt marker
     //modified by ver116-4b
     //For the moment, use the grid color and font
    #smithHndl$, "color "; smithMarkerColor$
    #smithHndl$, "font Arial bold 8"
    #smithHndl$, "size 1"
     for i=1 to gNumMarkers   //Send each marker to the draw routines
         N=gMarkerPoints(i,0) //Point num (possible fractional)
         if smithUseLines=0 then N=int(N+0.5)    //round to integral point if not doing line graph ver116-4k
         if N>0 and N<=gDynamicSteps+1 and gMarkers$(i,0)<>"Halt" then    //draw all valid markers except Halt
             //ver116-4k changed the interpolation to match the way data is displayed at the bottom of the chart. For widely
             //separated points, this will not align with the graph, which linearly interpolates pixels between two adjacent points.
             call smithPointS11 N, db, ang //Get S11; interpolate if necessary ver116-4k
             call smithS11DBtoPix db, ang, xPix, yPix //ver116-4k
             call smithDrawMarkerPix xPix, yPix,gMarkers$(i,0)    //Pix coord and label
        end if
    next i
    #smithHndl$, "flush"
         */
}
void smithDialog::smithSetPointPix(float xPix, float yPix)
{
  //Set point at specified pixel coordinates
  //If drawing lines, we set a point; otherwise we draw a circle
  smithLastPixX=xPix;
  smithLastPixY=yPix;
  if (smithUseLines)
  {
    scene->addEllipse(xPix-1, yPix-1,2,2);
  }
  else
  {
    scene->addEllipse(xPix, yPix, 3, 3);
  }
}

void smithDialog::smithDrawLinePix(float xPix, float yPix)
{
  QPen pen(smithTraceColor,smithTraceWidth);
  //Draw a line from last point to a point defined by pixel coords
  if (smithUseLines)
  {
    //cmd$="line ";smithLastPixX;" ";smithLastPixY;" ";xPix;" ";yPix;";line ";xPix;" ";yPix;" ";smithLastPixX;" ";smithLastPixY
    scene->addLine(smithLastPixX, smithLastPixY, xPix, yPix,pen);
  }
  else
  {
    //cmd$="place ";xPix;" "; yPix;";circle 3"
    scene->addEllipse(xPix, yPix, 3, 3);
  }

  //#smithHndl$, cmd$
  //smithDrawLinePix$=cmd$
  smithLastPixX=xPix;
  smithLastPixY=yPix;
}
void smithDialog::smithSetPointS11DB(float S11DB, float S11Ang, int width)
{
  //Set point defined by S11 in db, angle format
  if (S11DB>0)
    S11DB=0;   //Keep in bounds
  float RefcoMag=util.uTenPower(S11DB/20);
  float ang=S11Ang*util.uRadsPerDegree(); //convert to radians
  if (RefcoMag>1)
    RefcoMag=1;   //Keep in bounds
  if (RefcoMag<-1)
    RefcoMag=-1; //Should actually be no smaller than 0, but this is just a safety test
  float RefR=RefcoMag*cos(ang);  //Real part of reflect coeff.
  float RefI=RefcoMag*sin(ang);  //Imag part of reflect coeff.
  float xPix=smithCenterX + int(0.5+RefR*smithRadius);
  float yPix=smithCenterY - int(0.5+RefI*smithRadius);
  smithSetPointPix(xPix, yPix);
}

void smithDialog::smithDrawLineS11DB(float S11DB, float S11Ang, int width)
{
  //Draw a line from last point to a point defined by S11 in db, angle format
  if (S11DB > 0)
    S11DB=0; //Keep everything in bounds
  double RefcoMag=util.uTenPower(S11DB/20);
  double ang=S11Ang*util.uRadsPerDegree(); //convert to radians
  if (RefcoMag > 1)
    RefcoMag = 1;   //Keep in bounds
  if (RefcoMag < -1)
    RefcoMag = -1; //Should actually be no smaller than 0, but this is just a safety test
  double RefR=RefcoMag*cos(ang);  //Real part of reflect coeff.
  double RefI=RefcoMag*sin(ang);  //Imag part of reflect coeff.
  double xPixB=smithCenterX + int(0.5+RefR*smithRadius);
  double yPixB=smithCenterY - int(0.5+RefI*smithRadius);
  smithDrawLinePix(xPixB,yPixB);
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
