#ifndef SMITHDIALOG_H
#define SMITHDIALOG_H

#include <QDialog>
#include <QGraphicsScene>

#include "globalvars.h"
#include "msautilities.h"


namespace Ui {
class smithDialog;
}

class smithDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit smithDialog(QWidget *parent = 0);
  ~smithDialog();
  void setGlobalVars(globalVars *newVars) {vars = newVars;}
  void doit();
  void smithDrawChart();
  void smithSetChartColors(QColor backCol, QColor lineCol, QColor boundsCol);
  void smithSetTraceColor(QColor trace1Col) {smithTraceColor = trace1Col;}
  void smithSetTraceWidth(int w1) {smithTraceWidth = w1;}
  void smithSetReferenceColor(QColor referenceColor) {smithReferenceColor = referenceColor;}
  void smithSetReferenceWidth(int referenceWidth) {smithReferenceWidth = referenceWidth;}
  void smithSetMarkerColor(QColor gridTextCol) {smithMarkerColor = gridTextCol;}
  void setR0(double R0) {smithR0 = R0;}
  void smithRefresh(int doReference);

  int smithUseLines;
  
private:
  Ui::smithDialog *ui;
  QGraphicsScene *scene;
  globalVars *vars;
  msaUtilities util;

  void smithDrawResist(float resist, bool doLabel = false);
  void smithDrawReact(double react, bool doLabel = false);
  void smithDrawTitle();
  void smithDrawFooter();
  void smithPrintText(QString t, double x, double y);
  void smithPrintTextCentered(QString t, double x, double y);
  void smithPrintTextRightJust(QString t, double x, double y);

  void smithDrawReferenceSource();
  void smithDrawReflectArray();
  void smithDisplayMarkerInfo();
  void smithDrawMarkers();
  void smithSetPointPix(float xPix, float yPix);
  void smithDrawLinePix(float xPix, float yPix);
  void smithSetPointS11DB(float S11DB, float S11Ang, int width);
  void smithDrawLineS11DB(float S11DB, float S11Ang, int width);


  double S11GraphR0;
  double S11BridgeR0;
  double smithR0;
  double smithRadius;
  int smithRightX;
  int smithLeftX;
  int smithCenterY;
  int smithCenterX;
  int smithTop;
  int smithBottom;


  QColor smithBackColor, smithLineColor, smithBoundsColor;
  QColor smithMarkerColor; //Color for markers; intended for high contrast
  QColor smithTraceColor; //Color for main trace
  QColor smithReferenceColor; //Color for reference


  int smithTraceWidth;      //Size of trace
  int smithReferenceWidth;  //Size of reference trace

  float smithLastPixX;
  float smithLastPixY;
/*
  global smithLastPixX, smithLastPixY     'Last drawn point; used as starting point for lines
  global smithPrevBackCol$, smithPrevLineCol$, smithPrevBoundsCol$, smithPrevGridTextCol$ 'Used to detect changes
  global smithGraphMarkers    '=1 to draw markers on graph. Does not affect marker info ver115-2c
  global smithUseLines    '=1 to draw lines; 0 to draw circles. ver115-7a
  global smithWindH       'Windows handle to smith chart window ver115-7a
  global smithMenuBarH    'Windows handle to menu bar in smith window ver115-7a
  global smithOptionsH    'Windows handle to Options submenu ver115-7a
  global smithDoingRef    '=1 if chart currently shows reference line; set in smithRefresh ver115-7a
  global smithMouseFreq   'freq (MHz) used for displaying info when mouse left button is down over chart.
*/

  QString gGetTitleLine[5];
protected:
  virtual void showEvent(QShowEvent *event);
  virtual void resizeEvent(QResizeEvent *event);

signals:
  void gGetMinMaxPointNum(int &, int &);
  void smithRefreshMain(int);
};

#endif // SMITHDIALOG_H
