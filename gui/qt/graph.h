//*******************************************************************************
//
//  -- Modular Spectrum Analyzer , in QT.
//
// Copyright (c) 2013 Bill Lindbergs
//
// The majority of this code is from spectrumanalyzer.bas, written by
// Scotty Sprowls and modified by Sam Wetterlin and Dave Roberts
//
// This file may be distributed and/or modified under the terms of the
// GNU General Public License version 2 as published by the Free Software
// Foundation. (See COPYING.GPL for details.)
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//******************************************************************************
#ifndef GRAPH_H
#define GRAPH_H

#include <QGraphicsScene>
#include "dialogvnacal.h"
#include "interpolation.h"
#include "dialoggridappearance.h"
#include "msautilities.h"
#include "globalvars.h"
#include "dialogconfigman.h"
#include "uwork.h"
#include "twoport.h"
#include "hwdinterface.h"



class msagraph  : public QObject
{
   Q_OBJECT
public:
  msagraph(QWidget *parent);
  ~msagraph();
  void setGlobalVars(globalVars *newVars);
  void setActiveConfig(msaConfig *newActiveConfig);
  void setUwork(cWorkArray *newuWork);
  void setVna(dialogVNACal *newVnaCal);
  void setHwdIf(hwdInterface *newHwdIf);
  void setAppearance(dialogGridappearance *newGridappearance);

  void gInitFirstUse(QGraphicsView *view, int winWidth, int winHt, int marLeft, int marRight, int marTop, int marBot);
  void gInitDynamicDraw();
  QString TraceContext();
  QString RestoreTraceContext(QString s, int &startPos, int isValidation);
  QString RestoreGridContext(QString s, int &startPos, int isValidation);
  QString GridContext();
  void GraphDataContextToFile(QStringList &fHndl);
  void UpdateGraphDataFormat(int doTwoPort);
  void DetermineGraphDataFormat(int componConst, QString &yAxisLabel, QString &yLabel, int &yIsPhase, QString &yForm);
  void PlotDataToScreen();
  void mMarkToCenter();
  void Expand();
  void RedrawGraph(int restoreErase);
  void RememberState();
  void ToggleTransmissionReflection();
  void gSetMarkerNum(int markNum, int pointNum, QString ID, QString trace, QString style);
  void mUserMarkSelect(QString btn);

  void ResizeGraphHandler();

  int mMarkerNum(QString markID);
  void mDeleteMarker(QString markID);
  void mAddMarker(QString markID, int pointNum, QString trace);
  void mAddMarkerAndRedraw(QString markID, int ptNum, int traceNum);
  void mAddMarkerFromKeyboard();
  void mClearMarkers();

  void DetectChanges();
  void DetectChanges(int doRestart);



  void gSetYAxisRange(int axisNum, int yMin, int yMax);
  void gGetYAxisRange(int axisNum, int &yMin, int &yMax);
  void gGetXAxisRange(float &xMin, float &xMax);
  void gSetXAxisRange(float xMin, float xMax);
  void gSetIsLinear(int linX, int linY1, int linY2);
  void gGetIsLinear(int &linX, int &linY1, int &linY2);
  int gGetXIsLinear();
  void gSetXIsLinear(int lin);
  void gSetNumDivisions(int xDiv, int yDiv);
  void gGetNumDivisions(int &xDiv, int &yDiv);
  void gSetTraceWidth(int wid1, int wid2);
  void gGetTraceWidth(int &wid1, int &wid2);
  void gSetDataLabels(QString y1Label, QString y2Label);
  void gSetAxisLabels(QString xLabel, QString y1Label, QString y2Label);
  void gGetAxisLabels(QString &xLabel, QString &y1Label, QString &y2Label);
  QString gGetTitleLine(int N);
  void gSetTitleLine(int N, QString t);
  void gSetGridStyles(QString xStyle, QString y1Style, QString y2Style);
  void gSetAxisFormats( QString xForm, QString y1Form, QString y2Form);
  void gGetAxisFormats(QString &xForm, QString &y1Form, QString &y2Form);
  void gSetMaxPoints(int n);
  int gMaxNumPoints();
  void gSetDoHist(int doHist);
  int gGetDoHist();
  void gSetPrimaryAxis(int prim);
  int gGetPrimaryAxis();
  void gActivateGraphs(int graphY1, int graphY2);
  void gGetActiveGraphs(int &graphY1, int &graphY2);
  void gGetPointVal(int N, int &x, int &y1, int &y2);
  float gGetPointYVal(int N,int yNum);
  int gGetPointXVal(float N);
  float gGetPointXPix(float N);
  void gSetNumPoints(int nPts);
  void gClearYValues();
  int gPointCount();
  void gChangePoints(int pNum, int y1, int y2);
  int gAddPoints(float x, float y1, float y2 );
  void gGenerateXValues(int numValidPoints);
  void gFindMinMax();
  void gGetMinMaxPointNum(int &minNum, int &maxNum);
  void gFindPeaks(int traceNum, int p1, int p2, int &minNum, int &maxNum, int &minY, int &maxY);
  void gGetGridCorner(QString corner, float &xPix, float &yPix);
  void gGetGraphicsSize(int &width, int &height);
  void gGetMargins(int &marLeft, int &marRight, int &marTop, int &marBot);
  int gGetMarkerInfoTop();
  int gGetMarkerInfoRight();
  void gGetIsPhase(int &y1, int &y2);
  void gSetDoAxis(int doY1, int doY2);
  void gInitDefaults();
  void gSetIsPhase(int y1, int y2);
  void gSetSweepDir(int dir);
  void gGetSweepStartAndEndPointNum(int &pStart, int &pEnd);
  int gGetSweepDir();
  void gUpdateGraphObject(int winWidth, int winHt, int marLeft, int marRight, int marTop, int marBot);
  void gDrawReferences();
  void gAddReference(int N, QString t);
  void gAddReference(int N, float x, float y);
  void gClearReference(int N);
  void gClearAllReferences();
  QString gGridBoundaryLabel(float v, QString form);
  QGraphicsTextItem *gPrintText(QString text, int x, int y, QColor col = Qt::transparent);
  QGraphicsTextItem *gPrintTextCentered(QString t, int x, int y, QColor col);
  QGraphicsTextItem *gPrintTextRightJust(QString t, int x, int y, QColor col);
  void gDrawVertTick(int x,int y, int L);
  void gDrawHorTick(int x, int y, int L);
  void gPrintMessage(QString msg);
  int gDoPrintLabel(QString style, int num, int nLines);
  void gPrintGridLabels();
  void gPrintTitle(int doClear);
  void gPrintAxisLabels();
  void gPrintSupplementalAxisLabels(int nSupp, int axisNum, QString col, QString lab);
  void gPrintAxisAnnotation(int axisNum, QString annotText, QString annotFont, QString annotColor);
  void gInitGraphRange(float minX, float maxX, float minY1, float maxY1, float minY2, float maxY2);
  void gShiftRefLevel(int axisNum, int nDiv);
  void gCalcAxis(int isX, QString axisStyle, int &isLin, int &numDiv, float &axisMin
                , float &axisMax, float &scale, float &blackHoleRadius, float &blackHolePix, QString &gridLines);
  void gCalcGraphParams();
  int gPixIsInGrid(int xPix, int yPix);
  void gFindClickedPoint(int xPix, int &yPix, int &pointNum, int &traceNum);
  float gPointNumOfX(int x);
  float gPointNumOfXPix(float xPix);
  void gConvertXToPix(float &x);
  void gConvertY1ToPix(float &y1);
  void gConvertY2ToPix(float &y2);

  float gAdjustPhaseToDisplay(int axisNum, int pointNum, int useWorkArray);
  int gNumDynamicSteps();
  void gSetNumDynamicSteps(int nSteps);
  void gSetErasure(int doErase1, int doErase2, int nEraseLead);
  void gGetErasure(int &doErase1, int &doErase2, int &nEraseLead);
  void gPauseDynamicScan();
  void gResumeDynamicScan();
  void gRestoreErasure();
  void gStartNextDynamicScan();
  void gDynamicDrawPoint(float y1, float y2);
  void gDynamicComboDrawPoint(float y1, float y2);
  int gCanUseExpeditedDraw();
  void gDrawSingleTrace();
  void gDrawNextPointValue(float y1, float y2);
  void gDrawNextPoint();
  void gDrawNextPointPix(float x, float y1, float y2);
  void gEraseNextPoint();
  void gInitDraw();
  void gInitErase();
  void gClearMarkers();
  int gMaxMarkers();
  int gValidMarkerCount();
  void gDetermineMarkerPointNumbers();
  void gUpdateMarkerXVal();
  void gRecalcPix(int calcXPix);
  void RefreshGraph(int restoreErase);
  void gRecreateTraces(bool doDraw);
  void PerformAutoScale();
  void DrawSetupInfo();
  void PrintMessage();
  QString gSweepContext();
  void gGetMarkerByNum(int markNum, int &pointNum, QString &ID, QString &trace, QString &style);
  void gUsePresetText(QString btn);
  void gDrawMarkerInfo();
  void PrintReferenceHeading();
  void RecalcYValues();
  void SetY1Range(int bot, int top);
  void SetY2Range(int bot, int top);
  void CalcAutoScale(int axisNum, int &axisMin, int &axisMax);
  void mDrawMarkerInfo();
  void mUpdateMarkerLocations();
  void gDrawGrid();

  void CreateReferenceSource();
  void CreateReferenceTransform();
  void CalcReferencesWholeStep(int stepNum, float &ref1, float &ref2);
  void CalcReferences();
  QString CreateReferenceTraces(QString tCol, int tSize, int traceNum, QPainterPath *path);
  QString PrivateCreateReferenceTrace(int traceNum, int startPoint, int endPoint, QPainterPath *path);

  void CalcGraphData(int currStep, float &y1, float &y2, int useWorkArray);
  void CalcGraphDataType(int currStep, int dataType1, int dataType2, float &y1, float &y2, int useWorkArray);
  void CalcReflectDerivedData(int currStep);
  void CalcReflectGraphData(int currStep, float &y1, float &y2, int useWorkArray);
  void SetStartStopFreq(float startF, float stopF);
  void gDrawMarkers();
  void gEraseMarkers();
  void gDrawMarkerPix(QString style, QString markLabel, float x, float y);
  void gRefreshTraces();
  void gRefreshGridLinesOnly();
  void ImplementDisplayModes();
  void UpdateGraphParams();
  void InitGraphParams();
  void SetYAxes(int data1, int top1, int bot1, int auto1, int data2, int top2, int bot2, int auto2);
  void SetDefaultGraphData();
  void SetCenterSpanFreq(float cent, float span);
  void mEnterMarker(QString btn);
  void mDisplaySelectedMarker();
  QString mMarkerContext();
  QString  mRestoreMarkerContext(QString s, int &startPos, int isValidation);
  QString gRestoreSweepContext(QString &s, int &startPos, int isValidation);



  QGraphicsScene *getScene() { return graphScene; }



  //gDoY1 and gDoY2 indicate whether we should process info for each axis and calculate graph commands,
  //even though we may not be currently actually drawing the graph because the display mode is Off.
  //These are based on whether the data type is constNoGraph, whereas, whereas gGraphY1 and gGraphY2 are based
  //on whether the display mode is off.
  int gDoY1, gDoY2;     //=1 if Y1 and Y2 data exist
  QString referenceColor1, referenceColor2;
  QPainterPath refLine[12];
  int referenceLineType;    //Spec and type of reference line    ver114-7f
  //type: 0=none; 1=use data when ref was selected; 2=use RLC in spec; 3=use fixed value ver115-5d
  int referenceTrace;       //which ref lines, based on bits: 1 bit=do trace 1; 2 bit=do trace 2; 4 bit=do Smith trace   //ver115-6b
  int referenceWidth1;    //Reference Trace color and width
  int referenceWidth2;    //Reference Trace color and width
  QString referenceColorSmith;
  int referenceWidthSmith;     //color/width for smith chart reference line
  //The following globals determine whether we redraw various components from scratch or by a faster method.
  int refreshGridDirty;    //Forces grid (and labels, title) and setup info to redraw from scratch in RefreshGraphs
  int refreshTracesDirty;    //Forces traces to be redrawn from raw Y1 and Y2 values in RefreshGraph
  int refreshMarkersDirty;    //Forces recalc of marker positions based on their frequency
  int refreshAutoScale;     //Do auto scaling on refresh; also implies refreshRedrawFromScratch ver114-7a
  int refreshRedrawFromScratch;    //Forces complete redraw from scratch in RefreshGraph
  int haltsweep;    //Set to 1 when scan is running
  int refreshEachScan; //Set/cleared by user to control screen refresh. =1 means refresh each scan.
  int refreshForceRefresh;   //Forces refresh at end of scan even though refreshEachScan=0
  int refreshOnHalt;     //normally =1 to redraw when we halt. Set to 0 internally for some purposes.
  //The reference line may be graphed or combined with the scan data.
  //The math transform is in the form A*Ref + B*Data, A and B generally being 1 or -1
  //Math is done if referenceDoMath=1 or 2; otherwise reference is just graphed.
  //If referenceDoMath=1 then math is done on the dBm values that would be graphed with the default graphs.
  //referenceDoMath=1 is allowed only for SA mode; too complicated and not very useful for VNA modes, which use calibration instead.
  //If referenceDoMath=2 then math is done on the current graph values (e.g. capacitance)
  int referenceDoMath, referenceOpA, referenceOpB; //ver114-7b
  int autoScaleY1, autoScaleY2; //=1 to autoscale the axes
  float thispointy1, thispointy2;

  //------SEWgraph globals for graph params
  int firstScan;   //Set to 1 for first scan after background grid for graph is drawn
  int currGraphBoxHeight, currGraphBoxWidth;    //Actual current height, width, adjusted when resizing //ver115-1c
  int clientHeightOffset, clientWidthOffset;    //Difference between window size and client area size; set from test window //ver115-1c
  int smithLastWindowHeight, smithLastWindowWidth;  //Determines size of smith chart; initialized and later adjusted when resizing
  int graphMarLeft, graphMarRight, graphMarTop, graphMarBot;    //margins from graph box edge to grid
  int haltAtEnd;    //Flag set to 1 to cause a halt at end of current sweep  //SEWgraph
  QString markerIDs[10];    //IDs of markers, used to fill combo box. marker numbers run from 1 so ID of marker N is markerIDs(N-1)
  int doGraphMarkers;   //Set/cleared by user to show or hide markers on graph
  int continueCode; //Checked after "scan" command; 0=continue; 1=halt via [Halted]; 2=immediate wait; 3=restart.
  //QString graphBox;       //handle variable containing handle for current graph box. ver 114-3c
  int displaySweepTime; //=1 to display sweep time in message area  //ver114-4f
  int interpolateMarkerClicks;  //=1 to enable placing marker at exact click point; =0 to round to nearest step. ver115-1a
  int componConst;


  int gWindowWidth,gWindowHeight; //Number of pixels wide and high for entire graph window or graphics box
  int gMarginLeft, gMarginRight, gMarginBot, gMarginTop;    //Margins from window edge to graph grid
  int gOriginY;     //Y value of bottom grid line=gWindowHeight-gMarginBot
  int gSweepDir;    //1 for forward sweep -1 for reverse //ver114-4k
  int gHorDiv, gVertDiv;    //Number of hor and vert divisions in graph
  int gGridWidth, gGridHeight;     //graph grid width and height in pixels



  int gPrimaryAxis; //Used for preset colors ver115-3c
  float gXAxisMin, gXAxisMax;     //Axis max and min are the values at the end of each axis
  QString gXGridStyle, gY1GridStyle, gY2GridStyle;   //style of labeling grid lines
  QString gXAxisLabel, gY1AxisLabel, gY2AxisLabel;  //Indentifier which prints by axis in axis color
  //gNumPoints is the number of points for which we have valid graph data, though it may
  //come partly from this scan and partly from prior scans. For the first scan, it is the
  //number of points processed in that scan; thereafter it is always equal to gDynamicSteps+1.
  //Note that for the first scan with reverse sweeps, if gNumPoints=10, those 10 points
  //are the highest 10 point numbers in gGraphVal(), etc., which allow for gDynamicSteps+1 points
  int gNumPoints;   //Number of points actually entered in gGraphVal
  int gDynamicSteps;  //Number of steps (number of points-1) in a complete sweep; for dynamic graphing
  //QVector<QString> gTrace1, gTrace2; //Array of accumulated draw commands, one per point ver114-6a
  QVector<QPointF> gTrace1;
  QVector<QPointF> gTrace2;
  int  gDoHist;  //=1 to graph as histogram; 0 for normal trace
  int gGraphY1, gGraphY2;     //=1 to cause Y1 and Y2 to be graphed ver114-5L
  //gXIsLinear, gYIsLinear = 1 for linear graph, = for log. If value being graphed
  int gTrace1Width, gTrace2Width;  //Width of graph trace (0,1,2 or 3)
  QString gXAxisForm, gY1AxisForm, gY2AxisForm;      //Formatter for grid line values. e.g. "##.##"

  int gMarkerPoints[21][2]; //Up to 20 markers; point number (0) and frequency (1)

  QString gMarkers[21][3];     //Marker IDs, trace and style
  int  gNumMarkers;        //Number of markers in gMarkers; some may not be in use at any given time

  QString gXGridLabels[121], gY1GridLabels[121], gY2GridLabels[121];    //Labels to print at each grid line
  int  gIsFirstDynamicScan;   //=1 if first dynamic scan; 0 thereafter
  int gIsDynamic;   //Signal =1 not to accumulate drawing commands
  QString gGridBmp;        //Name of saved bitmap of grid lines ver114-6a
  QString gGridString;     //Cumulative LB drawing commands for grid lines without labels
  int gXGridLines[121], gY1GridLines[121], gY2GridLines[121]; //Pixel locations of grid lines;
  int gPrevPointNum;    //Index in gGraphVal() of last dynamically drawn point
  int referenceSourceNumPoints;     //Number of valid points in referenceSource()


  Q2DfloatVector referenceSource;  //freq(0), db(1) and angle() of source data for reference lines. First entry is 1
  Q2DfloatVector referenceTransform;  //Actual graph data for reference lines  Entries are by point number, 1... (not step num)

  QString referenceLineSpec;


  QString selMarkerID;  //ID of marker selected by user
  int hasMarkPeakPos, hasMarkPeakNeg, hasMarkL, hasMarkR, hasAnyMark;       //Marker flags
  int doPeaksBounded;   //=1 to limit peak search between L and R markers; otherwise 0
  QString doLRRelativeTo;  //marker ID of reference marker when L and R are relative to another marker; otherwise blank
  float doLRRelativeAmount;   //db offset when L,R are relative to another marker; otherwise 0
  int doLRAbsolute;     //=1 when LR are placed around another marker, but at an absolute db level ver115-3f
  QString axisPrefHandle;  //handle variable for axis preference window; non-blank when window is open
  QString TwoPortGraphBox;     //Handle to graph box for two-port graphs, or blank if no window open ver116-1b

  Q2DfloatVector gGraphVal;      //Can be used to hold graph. frequency(0), Y1(1), Y2(2)


private:
  QGraphicsScene *graphScene;
  QGraphicsView *graphicsView;
  interpolation inter;
  dialogGridappearance *gridappearance;
  msaUtilities util;
  globalVars *vars;
  msaConfig *activeConfig;
  cWorkArray *uWork;
  dialogVNACal *vnaCal;
  twoPortModule twoPort;
  hwdInterface *hwdIf;

  void CalcTransmitGraphData(int currStep, float &y1, float &y2, int useWorkArray);
  QString gRestoreTraceContext(QString &s, int &startPos, int isValidation);
  QString gGridContext();
  QString gTraceContext();
  QString gRestoreGridContext(QString &s, int &startPos, int isValidation);

  int GraphDataContextAsTextArray();
  QString GraphDataContext();
  void RestoreGraphDataContext(QString &s, int &startPos, int doTitle);
  void GetGraphDataContextFromFile(QFile *fHndl, int doTitle);





  void GetDefaultGraphData(int axisNum, int &axisType, int &axisMin, int &axisMax);

  int y1IsPhase, y2IsPhase;
  QString yAxisLabel;
  QString y1AxisLabel;
  QString y2AxisLabel;
  QString yLabel, y1Label, y2Label;



  int  gMarkerInfoTop;      //Y pixel coord of top of area for marker info; calculated from gMarginBot
  int  gMarkerInfoLeft;     //Y pix coord of left of marker info area; calculated at initialization
  int  gMarkerInfoRight;     //Rightmost pixel used for marker info; valid only after gDrawMarkerInfo


  int gY1IsPhase, gY2IsPhase;  //=1 if data contains phase info ver114-5f
  //are already in db, consider the graph linear
  int gXIsLinear;
  int gY1IsLinear, gY2IsLinear;
  int gSweepStart, gSweepEnd;   //Start and end points, based on gSweepDir and X axis bounds ver115-5e

  //------------Variables provided by user or calculated by subroutine call----
  float gY1AxisMin, gY1AxisMax;   //and are used for automatic calculation of axis and
  float gY2AxisMin, gY2AxisMax;   //grid characteristics


  //In dynamic draw, erase gEraseLead ahead of drawing

  int gDoErase1, gDoErase2, gEraseLead;


  QString gY1DataLabel, gY2DataLabel;       //Data label used for labeling marker values ver114-6f
  QString gTitle[5];      //Title printed above top grid line; up to 4 lines, excluding the zero entry ver114-5m
  //Fourth line is not printable title; it is sweep info set by user

  int gMaxPoints;       //Max number of points allowed. Starts at 1200
  //In dynamic graphing, the pixel values are saved in gGraphPix, and the commands to graph
  //the trace lines are saved in gTrace1() and gTrace2(). The commands to draw the background
  //are cumulated in gGridString; this includes the grid boundaries and everything inside, but not
  //the labels outside the grid. The latter can be used to reset the grid by covering all graphing
  //with fresh drawing.
  //           -------------Graph Data--------
  //gGraphVal can be used to hold a graph info, or the user can just call
  //the point-by-point routines. Second dimension specifies X(0), Y1(1) and Y2(2).
  //Points are numbered 1 ...gDynamicSteps+1
  Q2DfloatVector gGraphPix;  //Pix values for previous draw at this point: X(0), Y1(1) and Y2(2)



  //--------------Variables calculated internally----------------
  float gXScale, gY1Scale, gY2Scale;  //scale for converting from value to pixel coord
  //gXGridLines, etc., are used to specify the pixel locations of grid lines, relative to the axis start,
  //which is location 0. Zero entry of first dimension is not used. The number of entries is gHorDiv+1 or gVertDiv+1
  //gXGridLabels(), etc., contain the actual grid line label, or "" for no label.
  //If a y-axis has log scale and crosses zero, there is a "black hole" around zero that collapses to zero.
  //We keep track of the radius and pixel location of the black hole. If the axis is specified as ending
  //at zero, we simply change the endpoint to match the black hole radius (or its negative) and we have no
  //further use for this info. The x-axis is not allowed to cross zero, though it can be specified as ending
  //at zero (at either end), in which case the end will be changed in the same manner.
  float gY1BlackHoleRadius, gY2BlackHoleRadius;
  float gY1BlackHolePix, gY2BlackHolePix;



  //when drawing point by point
  QString gDraw1, gDraw2,gErase1, gErase2;  //color and size combined for drawing and erasing traces
  int gLastPointBeforePause;     //number of last point drawn (1...) in dynamic draw at time of pause
  int gWasFirstScanAtPause;  //whether we were in the first scan at time of pause
  int gLastQueryPointNum;    //Last point number at which mouse-move query was done, or -1 if none ver116-4h
  int gLastQueryTraceNum;   //Trace num (1 or 2) closest to point of last mouse query ver116-4h


  QString gMarkerString;   //for each drawn marker, contains four parameters for gDrawMarkerPix
  QString gMode;   //SA, ScalarTrans, VectorTrans or Reflection mode in which we are operating ver114-5L
  //global gIsTransMode     //=1 if ScalarTrans or VectorTrans Mode delver116-1b

  QString gRefTrace[12]; //Trace draw commands, as one string, for reference lines, numbered 1... ver114-6g



signals:
  void ChangeMode();
  void RequireRestart();


};

#endif // GRAPH_H
