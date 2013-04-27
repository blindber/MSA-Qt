#ifndef DIALOGSMITHCHART_H
#define DIALOGSMITHCHART_H

#include <QDialog>
#include <QtCore>
#include <QWidget>
#include <QVariant>
#include <QMessageBox>
#include <QGraphicsScene>

namespace Ui {
class dialogSmithChart;
}

class dialogSmithChart : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogSmithChart(QWidget *parent = 0);
  ~dialogSmithChart();
  void smithAdjustSize();
  
private:
  Ui::dialogSmithChart *ui;
  QGraphicsScene *scene;
  QGraphicsEllipseItem *ell;

   int smithCenterX, smithCenterY, smithRadius;  //Center and radius of smith chart, in pixels
   QVariant smithLeftX, smithRightX, smithTopY, smithBottomY; //Coordinates of extreme points
   QVariant smithR0;      //Reference impedance
   QVariant smithBackColor, smithLineColor, smithBoundsColor;  //chart colors
   QVariant smithMarkerColor;    //Color for markers; intended for high contrast
   QVariant smithTraceColor;     //Color for main trace
   QVariant smithReferenceColor; //Color for reference
   QVariant smithTraceWidth;      //Size of trace
   QVariant smithReferenceWidth;  //Size of reference trace
   QVariant smithHndl;       //Text handle to smith graphicbox
   QVariant smithChartBmp, smithHasChartBmp; //Bitmap of chart (no trace); smithHasChartBmp=1 if bitmap exists
   QVariant smithBmpHt, smithBmpWidth;        //Height and width of smithChartBmp
   QVariant smithLastPixX, smithLastPixY;     //Last drawn point; used as starting point for lines
   QVariant smithPrevBackCol, smithPrevLineCol, smithPrevBoundsCol, smithPrevGridTextCol; //Used to detect changes
   QVariant smithGraphMarkers;    //=1 to draw markers on graph. Does not affect marker info ver115-2c
   QVariant smithUseLines;    //=1 to draw lines; 0 to draw circles. ver115-7a
   QVariant smithWindH;       //Windows handle to smith chart window ver115-7a
   QVariant smithMenuBarH;    //Windows handle to menu bar in smith window ver115-7a
   QVariant smithOptionsH;    //Windows handle to Options submenu ver115-7a
   QVariant smithDoingRef;    //=1 if chart currently shows reference line; set in smithRefresh ver115-7a
   QVariant smithMouseFreq;   //freq (MHz) used for displaying info when mouse left button is down over chart.




};

#endif // DIALOGSMITHCHART_H
