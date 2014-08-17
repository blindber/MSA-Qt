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
#include "graph.h"

#include <QtCore>
#if (QT_VERSION < 0x050000)
#include <QtGui>
#else
#include <QtWidgets>
#endif

#include "constants.h"
#include "complexmaths.h"

msagraph::msagraph(QWidget *parent)
{
  graphScene = new QGraphicsScene();

  connect(this, SIGNAL(ChangeMode()), parent, SLOT(ChangeMode()));
  connect(this, SIGNAL(RequireRestart()), parent, SLOT(RequireRestart()));
  connect(this, SIGNAL(updatevar(int)), parent, SLOT(updatevar(int)));
  connect(this, SIGNAL(smithRefreshMain(int)), parent, SLOT(smithRefreshMain(int)));


//****************************
// clear all variables because thats what Liberty basic did
  gDoY1 = 0;
  gDoY2 = 0;
  gWindowWidth = 0;
  gWindowHeight = 0;
  gMarginLeft = 0;
  gMarginRight = 0;
  gMarginBot = 0;
  gMarginTop = 0;
  gOriginY= 0;
  gSweepDir = 0;
  gHorDiv = 0;
  gVertDiv = 0;
  gGridWidth = 0;
  gGridHeight = 0;
  gPrimaryAxis = 0;
  gXAxisMin = 0;
  gXAxisMax = 0;
  gXGridStyle = "";
  gY1GridStyle = "";
  gY2GridStyle = "";
  gXAxisLabel = "";
  gY1AxisLabel = "";
  gY2AxisLabel = "";
  gNumPoints = 0;
  gDoHist= 0;
  gGraphY1 = 0;
  gGraphY2 = 0;
  gTrace1Width = 0;
  gTrace2Width = 0;
  gXAxisForm = "";
  gY1AxisForm = "";
  gY2AxisForm = "";
  gNumMarkers = 0;
  referenceColor1 = "";
  referenceColor2 = "";
  gIsFirstDynamicScan = 0;
  gIsDynamic = 0;
  gGridBmp = "";
  gGridString = "";
  gPrevPointNum = 0;
  referenceLineType = 0;
  referenceTrace = 0;
  referenceSourceNumPoints = 0;
  referenceWidth1 = 0;
  referenceWidth2 = 0;
  referenceColorSmith = "";
  referenceWidthSmith = 0;
  referenceLineSpec = "";
  refreshGridDirty = 0;
  refreshTracesDirty = 0;
  refreshMarkersDirty = 0;
  refreshAutoScale = 0;
  refreshRedrawFromScratch = 0;
  haltsweep = 0;
  refreshEachScan = 0;
  refreshForceRefresh = 0;
  refreshOnHalt = 0;
  referenceDoMath = 0;
  referenceOpA = 0;
  referenceOpB = 0;
  autoScaleY1 = 0;
  autoScaleY2 = 0;
  firstScan = 0;
  currGraphBoxHeight = 0;
  currGraphBoxWidth = 0;
  clientHeightOffset = 0;
  clientWidthOffset = 0;
  smithLastWindowHeight = 0;
  smithLastWindowWidth = 0;
  graphMarLeft = 0;
  graphMarRight = 0;
  graphMarTop = 0;
  graphMarBot = 0;
  haltAtEnd = 0;
  hasMarkPeakPos = 0;
  hasMarkPeakNeg = 0;
  hasMarkL = 0;
  hasMarkR = 0;
  hasAnyMark = 0;
  selMarkerID = "";
  doGraphMarkers = 0;
  doPeaksBounded = 0;
  doLRRelativeTo = "";
  doLRRelativeAmount = 0;
  doLRAbsolute = 0;
  continueCode = 0;
  axisPrefHandle = "";
  TwoPortGraphBox = "";
  displaySweepTime = 0;
  interpolateMarkerClicks = 0;
  componConst = 0;
  gEraseLead = 0;
  y1IsPhase = 0;
  y2IsPhase = 0;

  gClearMarkers();

  haltMarker = NULL;


//****************************



  gGraphPix.mresize(805,3);
  gTrace1.resize(805);
  gTrace2.resize(805);
  gErase1.resize(805);
  gErase2.resize(805);
  referenceSource.mresize(802, 3);
  referenceTransform.mresize(802, 3);
  referenceLineType = 0;
  referenceWidth1 = 0;
  referenceWidth2 = 0;

  referenceLineSpec="";
  referenceDoMath = 0;

  referenceOpA = 0;
  referenceOpB = 0;


  interpolateMarkerClicks = 0;

  markerIDs[0] = "1"; markerIDs[1] = "2"; markerIDs[2] = "3"; markerIDs[3] = "4";
  markerIDs[4] = "5"; markerIDs[5] = "6"; markerIDs[6] = "L"; markerIDs[7] = "R";
  markerIDs[8] = "P+"; markerIDs[9] = "P-";

  refreshOnHalt=0;     //We normally redraw the graph when we halt.
  refreshEachScan=1;
  smithLastWindowHeight=430 ;
  smithLastWindowWidth=400;
  graphMarLeft=70;
  graphMarRight=180;
  graphMarTop=55;
  graphMarBot=140;   //Graph margins from edge of graphicbox


}

msagraph::~msagraph()
{
}

void msagraph::setGlobalVars(globalVars *newVars)
{
  vars = newVars;
  inter.setGlobalVars(newVars);
  gSetNumDynamicSteps(vars->steps);
}

void msagraph::setActiveConfig(msaConfig *newActiveConfig)
{
  activeConfig = newActiveConfig;
}
void msagraph::gSetYAxisRange(int axisNum, int yMin, int yMax)
{
  //Change range of specified y axis
  //gCalcGraphParams, which must be called before graphing.
  if (axisNum==1)
  {
    gY1AxisMin=yMin; gY1AxisMax=yMax;
  }
  else
  {
    gY2AxisMin=yMin; gY2AxisMax=yMax;
  }
}

void msagraph::gGetYAxisRange(int axisNum, int &yMin, int &yMax)
{
  //Get specified axis min and max
  //This is the full axis min/max, not the min and max actual y values
  if (axisNum == 1)
  {
    yMin=gY1AxisMin;
    yMax=gY1AxisMax;
  }
  else
  {
    yMin=gY2AxisMin;
    yMax=gY2AxisMax;
  }
}
void msagraph::gGetXAxisRange(float &xMin, float &xMax)
{
  //These are the min/max for the entire axis
  xMin=gXAxisMin;
  xMax=gXAxisMax;
}
void msagraph::gSetXAxisRange(float xMin, float xMax)
{
  //Set min and max x values to be graphed
  //may be changed in gCalcGraphParams, which must be called before graphing.
  gXAxisMin=xMin;
  gXAxisMax=xMax;
}

void msagraph::gSetIsLinear(int linX, int linY1, int linY2)
{
  //Set linearity for each axis; lin=1 for linear, 0 for log.
  //If changing from log X scale to linear, force horizontal divisions to 10 in case user doesn't adjust
  if (gXIsLinear == 0 && linX == 1)
  {
    gHorDiv=10;
  }
  gXIsLinear=linX;
  gY1IsLinear=linY1;
  gY2IsLinear=linY2;
}
void msagraph::gGetIsLinear(int &linX, int &linY1, int &linY2)
{
  //Set linearity for each axis; lin=1 for linear, 0 for log.
  linX=gXIsLinear;
  linY1=gY1IsLinear;
  linY2=gY2IsLinear;
}
int msagraph::gGetXIsLinear()
{
  //Return 1 if x axis is linear, 0 if log
  return gXIsLinear;
}

void msagraph::gSetXIsLinear(int lin)
{
  //lin= 1 if x axis is linear, 0 if log
  if (lin==1)
  {
    //If changing from log X scale to linear, force horizontal divisions to 10 in case user doesn't adjust
    if (gXIsLinear==0)
    {
      gHorDiv=10;
    }
    gXIsLinear=1;    //changing to linear
  }
  else
  {
    gXIsLinear=0;    //changing to log
  }
}
void msagraph::gSetNumDivisions(int xDiv, int yDiv)
{
  //Set horizonal and vert number of divisions (overridden for log)
  //Note: User must call gCalcGraphParams before doing any actual graphing
  gHorDiv=qMax(1,xDiv);
  gVertDiv=qMax(1,yDiv);
}
void msagraph::gGetNumDivisions(int &xDiv, int &yDiv)
{
  xDiv = gHorDiv;
  yDiv = gVertDiv;
}
void msagraph::gSetTraceWidth(int wid1, int wid2)
{
  //Set width of graph traces
  if (wid1<=0)
     gTrace1Width=0;
  else
    gTrace1Width=wid1;

  if (wid2<=0)
    gTrace2Width=0;
  else
    gTrace2Width=wid2;
}
void msagraph::gGetTraceWidth(int &wid1, int &wid2)
{
  //Get width of graph traces
  wid1=gTrace1Width;
  wid2=gTrace2Width;
}
void msagraph::gSetDataLabels(QString y1Label, QString y2Label)
{
  gY1DataLabel=y1Label;
  gY2DataLabel=y2Label;
}
void msagraph::gSetAxisLabels(QString xLabel, QString y1Label, QString y2Label)
{
  //Set labels to print below x axis and above y axes
  gXAxisLabel=xLabel;
  gY1AxisLabel=y1Label;
  gY2AxisLabel=y2Label;
}

void msagraph::gGetAxisLabels(QString &xLabel, QString &y1Label, QString &y2Label)
{
  //Get labels to print below x axis and above y axes ver116-4h
  xLabel=gXAxisLabel;
  y1Label=gY1AxisLabel;
  y2Label=gY2AxisLabel;
}
QString msagraph::gGetTitleLine(int N)
{
  //Return one of 4 lines of title to print above top grid line. Numbered 1,2,3,4. Only 3 are to print
  if (N<1)
    N=1;
  else if (N>4)
    N=4;
  return gTitle[N];
}
void msagraph::gSetTitleLine(int N, QString t)
{
  //Set one of 4 lines of title to print above top grid line. Numbered 1,2,3,4
  if (N<0)
    N=0;
  else if (N>3)
    N=3;
  gTitle[N]=t;
}

void msagraph::gSetGridStyles(QString xStyle, QString y1Style, QString y2Style)
{
  //Set styles for labeling grid lines
  //See PrintGridLabels for meanings. Case doesn't matter because we convert
  //to upper case here.
  gXGridStyle=xStyle.trimmed().toUpper();
  gY1GridStyle=y1Style.trimmed().toUpper();
  gY2GridStyle=y2Style.trimmed().toUpper();
}
void msagraph::gSetAxisFormats(QString xForm, QString y1Form, QString y2Form)
{
  //Formatters for numbers at gridlines
  gXAxisForm=xForm;
  gY1AxisForm=y1Form;
  gY2AxisForm=y2Form;
}

void msagraph::gGetAxisFormats(QString &xForm, QString &y1Form, QString &y2Form)
{
  //Formatters for numbers at gridlines
  xForm=gXAxisForm;;
  y1Form=gY1AxisForm;
  y2Form=gY2AxisForm;
}
void msagraph::gSetMaxPoints(int n)
{
  //Set max number of allowed points
  //gGraphVal will be redimensioned. No need to use this if the number of points has not increased
  //Existing points are erased if we resize. Number will always be 800 or more
  //changed so dimension is never reduced

  if (n > vars->gMaxPoints)
  {
    vars->gMaxPoints=n;
    vars->gGraphVal.mresize(vars->gMaxPoints+5,3);
    gGraphPix.mresize(vars->gMaxPoints+5,3);
    gTrace1.resize(vars->gMaxPoints+5);
    gTrace2.resize(vars->gMaxPoints+5);
    gErase1.resize(vars->gMaxPoints+5);
    gErase2.resize(vars->gMaxPoints+5);
    gNumPoints=0;
  }
}
int msagraph::gMaxNumPoints()
{
  //max number of points given size of arrays. We fudge a little to allow safety margin
  return vars->gMaxPoints;
}
void msagraph::gSetDoHist(int doHist)
{
  //Set gDoHist to doHist. =1 to do histogram; =0 for normal trace
  gDoHist=doHist;
}

int msagraph::gGetDoHist()
{
  //Return gDoHist; =1 to do histogram; =0 for normal trace ver114-4d
  return gDoHist;
}

void msagraph::gSetPrimaryAxis(int prim)
{
 //Set primary axis to 1 or 2
 if (prim==1)
   gPrimaryAxis=1;
 else
   gPrimaryAxis=2;
}
int msagraph::gGetPrimaryAxis()
{
  //Return gPrimaryAxis
  return gPrimaryAxis;
}
void msagraph::gActivateGraphs(int graphY1, int graphY2)
{
  //If doYn=1, turn on the corresponding graph
  if (gDoY1==0)
    gGraphY1=0;
  else
    gGraphY1=graphY1;

  if (gDoY2==0)
    gGraphY2=0;
  else
    gGraphY2=graphY2;
}

void msagraph::gGetActiveGraphs(int &graphY1, int &graphY2)
{
  //If doY=1, turn on the corresponding graph
  graphY1=gGraphY1;
  graphY2=gGraphY2;
}
void msagraph::gGetPointVal(int N, int &x, int &y1, int &y2)
{
  //Return x, y1 and y2 for specified point (1...gDynamicSteps+1)
  //N may have a fractional part, so we do linear interpolation
  if (N>=0 && N<=gDynamicSteps)
  {
    int whole=(int)(N);
    float fract=N-whole;
    x=vars->gGraphVal[whole][0];
    y1=vars->gGraphVal[whole][1];
    y2=vars->gGraphVal[whole][2];
    //Note that angles in the main program are kept in the range -180 to +180, but
    //in gGraphVal they have been adjusted to fit graph range
    if (fract>0)
    {
      x=x+fract*(vars->gGraphVal[whole+1][0]-x);
      if (gY1IsPhase)
      {
        //created special interpolation for angles to deal with wrap-around
        y1=inter.intLinearInterpolateDegrees(fract, y1, vars->gGraphVal[whole+1][1], gY1AxisMin, gY1AxisMax);
      }
      else
      {
        y1=y1+fract*(vars->gGraphVal[whole+1][1]-y1);
      }
      if (gY2IsPhase)
      {
        //created special interpolation for angles to deal with wrap-around
        y2=inter.intLinearInterpolateDegrees(fract, y2, vars->gGraphVal[whole+1][2], gY2AxisMin, gY2AxisMax);
      }
      else
      {
        y2=y2+fract*(vars->gGraphVal[whole+1][2]-y2);
      }
    }
  }
  else
  {
    x=-1;y1=0;y2=0;
  }
}

float msagraph::gGetPointYVal(int N, int yNum)
{
  float y;
  //Get y1 or y2 (indicated by yNum=1 or 2) for point N, possibly non-integral
  //Angles will have values in the graph range, not necessarily -180 to +180
  if (N > 0 && N <= gDynamicSteps+1)
  {
    int whole=(int)(N);
    float fract=N-whole;
    y=vars->gGraphVal[whole][yNum];
    if (fract>0)
    {
      float yMin, yMax;
      int isPhase;
      if (yNum==1)
      {
        isPhase=gY1IsPhase;
        yMin=gY1AxisMin;
        yMax=gY1AxisMax;
      }
      else
      {
        isPhase=gY2IsPhase;
        yMin=gY2AxisMin;
        yMax=gY2AxisMax;
      }
      if (isPhase)
      {
        //Note that angles in the main program are kept in the range -180 to +180, but
        //in gGraphVal they have been adjusted to fit graph range
        //created special interpolation for angles to deal with wrap-around
        y=inter.intLinearInterpolateDegrees(fract, y, vars->gGraphVal[whole+1][yNum], yMin, yMax);
      }
      else
      {
        y=y+fract*(vars->gGraphVal[whole+1][yNum]-y);
      }
    }
  }
  else
  {
    y=0;
  }
  return y;
}
float msagraph::gGetPointXVal(float N)
{
  float x;
  //Return x for specified point (1...)
  //We don't verify that N is in bounds, because its value may have been created with gGenerateXValues
  //and the actual point data may not have been added yet.
  //N may have a fractional part, so we do linear interpolation
  if (N>0 && N<=vars->gMaxPoints)
  {
    int whole=int(N);
    float fract=N-whole;
    x=vars->gGraphVal[whole][0];
    if (fract>0)
    {
      x=x+fract*(vars->gGraphVal[whole+1][0]-x);
    }
  }
  else
  {
    x=-1;
  }
  return x;
}

float msagraph::gGetPointXPix(float N)
{
  float x;
  //Return x pixel coord for  specified point (1...)
  //We don't verify that N is in bounds, because its value may have been created with gGenerateXValues
  //and the actual point data may not have been added yet.
  //N may have a fractional part, so we do linear interpolation
  if (N>0 && N<=vars->gMaxPoints)
  {
    int whole=(int)N;
    float fract=N-whole;
    x=gGraphPix[whole][0];
    if (fract>0)
    {
      x=x+fract*(gGraphPix[whole+1][0]-x);
    }
  }
  else
  {
    x=-1;
  }
  return x;
}
void msagraph::gSetNumPoints(int nPts)
{
  //Set number of points to be considered valid. Does not affect values ver116-1b
  if (nPts < 0)
    nPts=0;
  if (nPts >= vars->gMaxPoints)
    gSetMaxPoints(nPts);
  gNumPoints=nPts;
}

void msagraph::gClearYValues()
{
  //Zero Y values of gNumPoints points
  for (int i=1; i <= gNumPoints;i++)
  {
    vars->gGraphVal[i][1] = 0;
    vars->gGraphVal[i][2] = 0;
  }
}
int msagraph::gPointCount()
{
  //Number of points in gGraphVal()
  return gNumPoints;
}
void msagraph::gChangePoints(int pNum, int y1, int y2)
{
  //change points at pNum, values y1, y2; keep old x
  if (pNum<0 || pNum>gDynamicSteps)
  {
    QMessageBox::warning(0,"Error", QString("Invalid point number: %1").arg(pNum));    //debugging
  }
  vars->gGraphVal[pNum][1] = y1;
  vars->gGraphVal[pNum][2] = y2;
}

int msagraph::gAddPoints(float x, float y1, float y2)
{
  //Add points to end. Must be done with non-decreasing x
  //return 1 if too many points
  if (gNumPoints>=vars->gMaxPoints)
    return 1;
  gNumPoints=gNumPoints+1;
  //enter new point
  vars->gGraphVal[gNumPoints][0]=x;
  vars->gGraphVal[gNumPoints][1]=y1;
  vars->gGraphVal[gNumPoints][2]=y2;
  return 0;
}
void msagraph::gGenerateXValues(int numValidPoints)
{
  //Create array with X values for log or linear sweep
  //Fill gGraphVal() with x values starting at gXAxisMin, ending at gXAxisMax, and creating gDynamicSteps+1 points
  //Set gNumPoints to numValidPoints. Thus, to recalculate x pixels for
  //an existing partial graph, nValidPoints would be the number of completed points in the
  //Also fills gGraphPix() with the pixel coords for the x values.
  //Does not affect y values or y pixel coords.
  //Log sweeps may now be for negative X values. The X values are not allowed to contain or cross zero.
  //If user specifies a zero endpoint, it gets changed to a small value.
  int numPoints=gDynamicSteps+1;
  //X axis starts at gXAxisMin and ends at gXAxisMax
  float pixStart=gMarginLeft;
  float pixEnd=gMarginLeft+gGridWidth;
  float pixInterval=(pixEnd-pixStart)/(numPoints-1);
  float xInterval;
  if (gXIsLinear==1)
  {
    //Set up for even spacing
    xInterval=(gXAxisMax-gXAxisMin)/(numPoints-1); //Add to one x value to get the next
  }
  else
  {
    //Here for log X sweep.
    xInterval=pow((gXAxisMax/gXAxisMin),(1/(numPoints-1)));    //multiply times one x value to get next
  }
  float x=gXAxisMin;
  float xPix=pixStart;
  for (int i=1; i <= numPoints; i++)
  {
    if (i==numPoints)
    {
      vars->gGraphVal[i][0]=gXAxisMax;
      gGraphPix[i][0]=pixEnd;  //to get it exact
    }
    else
    {
      if (x>0)
      {
        vars->gGraphVal[i][0]=(double)( (int)(1000000*x+0.5) )/1000000 ;
      }
      else
      {
        vars->gGraphVal[i][0]=0-(double)( (int)(1000000*(0-x)+0.5) )/1000000;    //Round to nearest Hz
      }
      //We round pixels to the nearest tenth
      //gGraphPix[i][0]=(double)( (int)(10*xPix+0.5) )/10;
      gGraphPix[i][0]=xPix;
    }
    xPix=xPix+pixInterval;
    if (gXIsLinear)
      x=x+xInterval;
    else
      x=x*xInterval;
  }
  gNumPoints=numValidPoints;
}

void msagraph::gFindMinMax()
{  
  //find min and max x and y values in gGraphVal ver115-1d deleted arguments
  //must call gCalcGraphParams after this
  int pMin, pMax;
  gGetMinMaxPointNum(pMin, pMax);
  for (int i=pMin; i <= pMax; i++)
  {
    int x=vars->gGraphVal[i][0];
    int y1=vars->gGraphVal[i][1];
    int y2=vars->gGraphVal[i][2];
    if (i==1)
    {
      gXAxisMin=x;
      gXAxisMax=x;
      gY1AxisMin=y1;
      gY1AxisMax=y1;
      gY2AxisMin=y2;
      gY2AxisMax=y2;
    }
    else
    {
      if (x<gXAxisMin)
      {
        gXAxisMin=x;
      }
      else if (x>gXAxisMax)
      {
        gXAxisMax=x;
      }
      if (y1<gY1AxisMin)
      {
        gY1AxisMin=y1;
      }
      else if (y1>gY1AxisMax)
      {
        gY1AxisMax=y1;
      }

      if (y2<gY2AxisMin)
      {
        gY2AxisMin=y2;
      }
      else if (y2>gY2AxisMax)
      {
        gY2AxisMax=y2;
      }
    }
  }
}
void msagraph::gGetMinMaxPointNum(int &minNum, int &maxNum)
{
  //Return min and max point numbers actually graphed
  //Note for gSweepDir=-1, the first points are put into the upper part of the arrays, so we
  //use the higher point numbers first
  //If this is the first scan, gNumPoints is the number of points for which we have valid
  //data; otherwise the number is gDynamicSteps+1
  if (gSweepDir==1)
  {
    minNum=1;
    maxNum=gNumPoints;
  }
  else
  {
   maxNum=gDynamicSteps+1;
   minNum=maxNum-gNumPoints+1;
  }
}
void msagraph::gFindPeaks(int traceNum, int p1, int p2, int &minNum, int &maxNum, int &minY, int &maxY)
{
  //find positive and negative peak
  //Search includes points from p1 to p2, inclusive. traceNum(1 or 2) indicates which trace to examine.
  //minNum and maxNum will be set to the point numbers (1...gDynamicSteps+1) where the peaks occur;
  //minY and maxY will be the peak value
  int pMin, pMax;
  gGetMinMaxPointNum(pMin, pMax);
  if (p1<pMin) p1=pMin;
  if (p2>pMax) p2=pMax;
  int minPeakEnded=1 ; int maxPeakEnded=1;
  int minNumStart, maxNumStart, minNumEnd,maxNumEnd;
  for (int i=p1; i < p2; i++)
  {
    int y=vars->gGraphVal[i][traceNum];
    if (i==p1)
    {
      minNumStart=p1; maxNumStart=p1;
      minNumEnd=p1; maxNumEnd=p1;
      minY=y ; maxY=y; //Start with values at first point
    }
    else
    {
     //See if peak is found. Once found, so long as we remain at that level, continue
     //to record maxPeakEnded
      if (y>maxY)
      {
        maxY=y;
        maxNumStart=i;
        maxPeakEnded=0;
      }
      if (maxPeakEnded==0 && y>=maxY)
        maxNumEnd=i;
      else
        maxPeakEnded=1;
      if (y<minY)
      {
        minY=y;
        minNumStart=i;
        minNumEnd=i;
        minPeakEnded=0;
      }
      if (minPeakEnded==0 && y<=minY)
        minNumEnd=i;
      else
        minPeakEnded=1;
    }
  }
  //Here the min or max start and end numbers indicate where the peak started and ended; we consider
  //the actual peak to be in the middle.
  maxNum=(int)((maxNumEnd+maxNumStart)/2);
  minNum=(int)((minNumEnd+minNumStart)/2);
}

void msagraph::gGetGridCorner(QString corner, float &xPix, float &yPix)
{
  /*
  'Get pixel coord of specified corner
      corner$=Trim$(Upper$(corner$))
      select case corner$
          case "LL"   'lower left
              xPix=gMarginLeft: yPix=gOriginY
          case "UL"   'upper left
              xPix=gMarginLeft: yPix=gMarginTop
          case "UR"   'upper right
              xPix=gMarginLeft+gGridWidth: yPix=gMarginTop
          case "LR"   'lower right
              xPix=gMarginLeft+gGridWidth: yPix=gOriginY
          case else
              xPix=0: yPix=0
      end select
  end sub*/
}
void msagraph::gGetGraphicsSize(int &width, int &height)
{
  //Get pixel pixel size of entire graphics window
  height=gWindowHeight;
  width=gWindowWidth;
}
void msagraph::gGetMargins(int &marLeft, int &marRight, int &marTop, int &marBot)
{
  //Get graph margins
  marLeft=gMarginLeft;
  marRight=gMarginRight;
  marTop=gMarginTop;
  marBot=gMarginBot;
}

int msagraph::gGetMarkerInfoTop()
{
  //Return top of marker info area
  return gMarkerInfoTop;
}

int msagraph::gGetMarkerInfoRight()
{
  //Return right edge of marker info box. Valid only after gDrawMarkerInfo
  return gMarkerInfoRight;
}
void msagraph::gSetIsPhase(int y1, int y2)
{
  //Set flags to indicate whether y1, y2 contain phase data
  if (y1==1)
    gY1IsPhase=1;
  else
    gY1IsPhase=0;
  if (y2==1)
    gY2IsPhase=1;
  else
    gY2IsPhase=0;
}
void msagraph::gGetIsPhase(int &y1, int &y2)
{
  //Return phase flags
 y1=gY1IsPhase;
 y2=gY2IsPhase;
}
void msagraph::gSetDoAxis(int doY1, int doY2)
{
  //Set variables indicating whether data exists for Y1 and Y2
  //We don't necessarily graph it just because we have it, but we will label the axis and put
  //the data in the marker info.
  gDoY1=doY1;
  gDoY2=doY2;
  if (doY1==0)
    gGraphY1=0;   //Can't graph it if we don't have it
  if (doY2==0)
    gGraphY2=0;
    //Clear accumulated draw commands. We use "down" as the empty command
  for (int i=0; i < gDynamicSteps; i++)
  {
    if (doY1==0)
    {
      gTrace1[i]= QPointF(0,0);  //"down";
      vars->gGraphVal[i][1]=0;
    }
    if (doY2==0)
    {
      gTrace2[i]= QPointF(0,0);
      vars->gGraphVal[i][2]=0;
    }
  }
}
void msagraph::gInitDefaults()
{
  //Initialize graphing variables to default values
  gXIsLinear=1;
  gY1IsLinear=1;
  gY2IsLinear=1;
  gY1IsPhase=1;
  gY2IsPhase=0;
  gActivateGraphs(1,1);
  gHorDiv=10;
  gVertDiv=10;
  gridappearance->gUsePresetColors("LIGHT", gPrimaryAxis);
  gUsePresetText("BasicText");
  gDoHist=0;
  gPrimaryAxis=1;
  gSetGridStyles("ENDSANDCENTER", "All", "All"); //Label these grid lines
  vars->S11GraphR0 = 50;
  vars->S11BridgeR0 = 50;
}
void msagraph::gSetSweepDir(int dir)
{
  //sub gSetSweepDir dir    //Set sweep direction to 1 or -1
  if (dir==1)
  {
    gSweepDir=1; //Forward
    //make erase lead sign match direction sign
    if (gEraseLead<0)
      gEraseLead=0-gEraseLead;
    gSweepStart=1;
    gSweepEnd=gDynamicSteps+1;
  }
  else
  {
    gSweepDir=-1; //Reverse
    //make erase lead sign match direction sign
    if (gEraseLead>0)
      gEraseLead=0-gEraseLead;
    gSweepStart=gDynamicSteps+1;
    gSweepEnd=1;
  }
}

void msagraph::gGetSweepStartAndEndPointNum(int &pStart, int &pEnd)
{
  pStart=gSweepStart;
  pEnd=gSweepEnd;
}
int msagraph::gGetSweepDir()
{
  //Return 1 for forward sweep; -1 for reverse
  return gSweepDir;
}
void msagraph::gUpdateGraphObject(int winWidth, int winHt, int marLeft, int marRight, int marTop, int marBot)
{
  //Note the width and height are labeled as window width and height, but are the dimensions
  //of the graphic box.
  //set up Grid parameters. Parameters get assigned directly to our variables,
  //so the parameters can be identified by the variable to which they are assigned
  gWindowHeight=winHt;
  gWindowWidth=winWidth;
  gMarginLeft=marLeft;
  gMarginRight=marRight;
  gMarginTop=marTop;
  gMarginBot=marBot;

  //set the size of the area we are going to be drawing in
  graphScene->clear();

  for (int i = 0; i < gErase1.count(); i++)
  {
    gErase1[i] = 0;
  }
  for (int i = 0; i < gErase2.count(); i++)
  {
    gErase2[i] = 0;
  }
  haltMarker = NULL;

  graphScene->addRect(0, 0, winWidth, winHt, QPen(QColor(Qt::magenta)));
  //graphScene->addRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height(), QPen(QColor(Qt::magenta)));
  graphScene->setSceneRect(graphScene->itemsBoundingRect());

  //Calculate a couple of additional parameters from those above
  gGridHeight=gWindowHeight-gMarginTop-gMarginBot;     //height of graph grid
  gGridWidth=gWindowWidth-gMarginLeft-gMarginRight;    //width of graph grid
  gOriginY=gWindowHeight-gMarginBot;   //pixel number of bottom grid line

  gMarkerInfoTop=gOriginY+40;      //Top of area below x axis labels
  gMarkerInfoLeft=10;              //Near left of graphic box
  gMarkerInfoRight=gMarkerInfoLeft;    //Recalc in gDrawMarkerInfo
  gIsFirstDynamicScan=0;
  gIsDynamic=0;
  gGridString="";
}
void msagraph::gDrawReferences()
{
  //Draw reference traces
  for (int i=0; i < 10; i++)
  {

    QPen Pen(Qt::green);
    //QString t=gRefTrace[i];
    if (i == 1)
    {
      //Pen.setBrush(QColor(util.fixColor(referenceColor1)));
      Pen.setBrush(referenceColor1);
      Pen.setWidth(referenceWidth1);
    }
    else if (i == 2)
    {
      //Pen.setBrush(QColor(util.fixColor(referenceColor2)));
      Pen.setBrush(referenceColor2);
      Pen.setWidth(referenceWidth2);
    }
    graphScene->addPath(refLine[i],Pen);
  }
}
void msagraph::gAddReference(int N, QColor t)
{
  if (N>0 && N<11)
  {
    gRefTrace[N]=t;
  }
}
void msagraph::gAddReference(int N, float x, float y)
{
  if (N>0 && N<11)
  {
    gRefTrace[N]=x;
  }
}

void msagraph::gClearReference(int N)
{
  if (N>0 && N<11)
  {
    gRefTrace[N]="";
    refLine[N] = QPainterPath();
  }
}
void msagraph::gClearAllReferences()
{
  for (int i=0; i < 11;i++)
  {
    gRefTrace[i]="";
    refLine[i] = QPainterPath();
  }
}
QString msagraph::gGridBoundaryLabel(float v, QString form)
{
  //Format number for grid line label
  //form$ is either a format string such as ##.## that can be used directly
  //by the using() function, or a series of 3 numbers (separated by spaces or commas)
  //indicating the max number of whole digits, the max number of decimal digits, and
  //the max number of significant digits. The latter is used only to restrict the actual
  //number of decimal places so the max sig dig won't be exceeded.
  QString s=util.uFormatted(v, form);
  int L=s.length();
  int dec=s.indexOf(".");
  if (dec>-1)
  {
    while (L>dec)   //Trim trailing zeroes
    {
      if (s.right(1)!="0") break;
      s=s.left(L-1);
      L=L-1;
    }
  }
  if (s.right(1)==".") s=s.left(s.length()-1);
  return s.trimmed();
}
QGraphicsTextItem *msagraph::gPrintText(QString text, int x, int y, QColor col)
{
  if (col == Qt::transparent)
  {
    col = Qt::magenta;
  }
  QFontMetrics fm(graphScene->font());
  QGraphicsTextItem *item;
  item = graphScene->addText(text);
  item->setDefaultTextColor(col);
  item->setPos(x,y);
  return item;
}
QGraphicsTextItem *msagraph::gPrintTextCentered(QString t, int x, int y, QColor col)
{
  //Print text t$ centered at x,y on graph
  QFontMetrics fm(graphScene->font());
  int pixelsWide = fm.width(t);
  int pixelHeight = fm.height();
  return gPrintText(t,x-pixelsWide/2,y-pixelHeight/2-fm.height(), col);

}
QGraphicsTextItem *msagraph::gPrintTextRightJust(QString t, int x, int y, QColor col)
{
  //Print text t$ right justified at x,y on graph
  QFontMetrics fm(graphScene->font());
  int pixelsWide = fm.width(t);
  int pixelHeight = fm.height();
  return gPrintText(t,x-pixelsWide,y-pixelHeight/2, col);
}
void msagraph::gDrawVertTick(int x, int y, int L)
{
  /*
'Draw vertical tick mark of length L whose top is at (x,y)
    #gGraphHandle$, "size 2"
    #gGraphHandle$, "line "; x;" ";y;" ";x;" ";y+L*/
}

void msagraph::gDrawHorTick(int x, int y, int L)
{/*
'Draw horizontal tick mark of length L whose left is at (x,y)
    #gGraphHandle$, "size 2"
    #gGraphHandle$, "line "; x;" ";y;" ";x+L;" ";y
end sub*/
}
QGraphicsTextItem *msagraph::gPrintMessage(QString msg)
{
  //Print message above top of marker info area; Limited to 75 characters; don't print if blank
  //The message is in msg$
  int y=gGetMarkerInfoTop()-5;
  int x=30;
  QColor textColor;
  QColor backColor;
  gridappearance->gGetInfoColors(textColor, backColor);
  //cmd$="font Tahoma 8 bold;color ";textColor$;";backcolor ";backColor$
  gPrintText(util.Space(140), x, y);   //note spaces are smaller than letters
  if (msg!="")
  {
    return gPrintText("MESSAGE: "+msg.left(75), x, y,textColor);
  } //don't print if blank
  return NULL;
}
int msagraph::gDoPrintLabel(QString style, int num, int nLines)
{
  //Return 1 if grid label is to be printed
  //The grid lines are numbered from 1 to nLines; the current one is num.
  //style$ is the grid labeling style:
  //(In gSetGridStyles, all are converted to upper case, lower case is used here for clarity)
  //None--No labels
  //Ends--Label axis ends
  //All--Label all grid lines
  //EndsAndCenter--Label ends and center

  int doPrint=0;
  int center=1 + (int)((nLines-1)/2);
  if (style == "ALL")
  {
    doPrint=1;
  }
  else if (style == "CENTER")
  {
    if (num==center) doPrint=1;
  }
  else if (style == "ENDS")
  {
    if (num==1 || num==nLines) doPrint=1;
  }
  else if (style == "ENDSANDCENTER")
  {
    if (num==1 || num==nLines || num==center) doPrint=1;
  }
  return doPrint;
}
void msagraph::gPrintGridLabels()
{
  //Print numeric values of grid lines
  //The number of gridlines labeled on any axis depends of gXGridStyle$
  //gY1GridStyle$, and gY2GridStyle$, see gDoPrintLabel for meanings
  //The format of the printed number depends on gXAxisForm$, etc.

  QString thisLabel;
  //Print X labels
  if (gXGridStyle!="NONE")
  {
    //    #gGraphHandle$, "font "; gXAxisFont$     //sets font
    //Note grid line  locations are in gXGridLines() and labels are in gXGridLabels$()
    // fix me check index
    for (int i=0; i <= gHorDiv+1; i++)
    {
      thisLabel = gXGridLabels[i];
      if (thisLabel!="")
      {
        gPrintTextCentered(thisLabel, gMarginLeft+gXGridLines[i], gOriginY+26, gridappearance->gXTextColor);
      }
    }
  }


  //Print Y1 labels
  if (gDoY1 && gY1GridStyle!="NONE")
  {
    // #gGraphHandle$, "font "; gY1AxisFont$     //sets font
    for (int i=0; i <= gVertDiv+1; i++)
    {
      thisLabel=gY1GridLabels[i];
      if (thisLabel!="")
      {
        gPrintTextRightJust(thisLabel, gMarginLeft-6,gOriginY-gY1GridLines[i]-10, gridappearance->gY1TextColor);
      }
    }
  }

  //Print Y2 labels
  if (gDoY2 && gY2GridStyle!="NONE")
  {
    //#gGraphHandle$, "font "; gY2AxisFont$     //sets font
    for (int i=0; i <= gVertDiv+1; i++)
    {
      thisLabel=gY2GridLabels[i];
      if (thisLabel!="")
      {
        gPrintText(thisLabel, gMarginLeft+gGridWidth+6, gOriginY-gY2GridLines[i]-10, gridappearance->gY2TextColor);
      }
    }
  }
}

void msagraph::gPrintTitle(int doClear)
{
  QGraphicsItem *item;
  //Print title; if doClear=1 then first clear the title area
  //if (doClear==1)
  {
    QList<QGraphicsItem *> allItems = graphScene->items();

    QListIterator<QGraphicsItem *> i(allItems);
    while(i.hasNext())
    {
      item = i.next();
      QString data = item->data(0).toString();
      if (data == "gtitle3"
          || data == "gtitle2"
          || data == "gtitle1")
      {
        graphScene->removeItem(item);
        delete item;
      }
    }
  }
  int titleCenter = gMarginLeft+gGridWidth/2;
  //Print from bottom to top so bottom of characters don't get cut off
  item = gPrintTextCentered(gTitle[3], titleCenter, 45, gridappearance->gGridTextColor);
  item->setData(0,"gtitle3");
  item = gPrintTextCentered(gTitle[2], titleCenter, 30, gridappearance->gGridTextColor);
  item->setData(0,"gtitle2");
  item = gPrintTextCentered(gTitle[1], titleCenter, 15, gridappearance->gGridTextColor);
  item->setData(0,"gtitle1");

  //There is a fourth title line for sweep info, but we don't print it
}
void msagraph::gPrintAxisLabels()
{
  //Print axis labels
  //#gGraphHandle$, "font "; gXAxisFont$;";color "; gXTextColor$
  gPrintTextCentered(gXAxisLabel, gMarginLeft+gGridWidth/2, gOriginY+45, gridappearance->gXTextColor);

  if (gDoY1)
  {
//    #gGraphHandle$, "font "; gY1AxisFont$
    gPrintTextCentered(gY1AxisLabel, gMarginLeft-15, gMarginTop-15, gridappearance->gY1TextColor);
  }

  if (gDoY2)
  {
//    #gGraphHandle$, "font "; gY2AxisFont$
    gPrintTextCentered(gY2AxisLabel, gMarginLeft+gGridWidth+15, gMarginTop-15, gridappearance->gY2TextColor);
  }
}

void msagraph::gPrintSupplementalAxisLabels(int nSupp, int axisNum, QString col, QString lab)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  'Print supplemental axis labels above the main labels ver116-4b
      'nSupp is the supplement number. 1 goes right above main label. 2 goes above 1. No room for more
      #gGraphHandle$, "color "; col$
      if axisNum=1 then
          #gGraphHandle$, "font "; gY1AxisFont$
          call gPrintTextCentered lab$, gMarginLeft-15, gMarginTop-15-14*nSupp
      else
          #gGraphHandle$, "font "; gY2AxisFont$
          call gPrintTextCentered lab$, gMarginLeft+gGridWidth+15, gMarginTop-15-14*nSupp
      end if
  end sub
          */
}

void msagraph::gPrintAxisAnnotation(int axisNum, QString annotText, QString annotFont, QColor annotColor)
{
  //Print annotation above axis labels
  //The specified text is printed above the axis label of the specified axis (1 or 2)
  //in the specified font and color.
  //#gGraphHandle$, "font "; gXAxisFont$;";color "; gXTextColor$
  gPrintTextCentered(gXAxisLabel, gMarginLeft+gGridWidth/2, gOriginY+45, gridappearance->gXTextColor);
  //#gGraphHandle$, "font "; annotFont$
  if (axisNum==1)
    gPrintTextCentered(annotText, gMarginLeft-15, gMarginTop-30, annotColor);
  if (axisNum==2)
    gPrintTextCentered(annotText, gMarginLeft+gGridWidth+15, gMarginTop-30, annotColor);
}
void msagraph::gInitGraphRange(float minX, float maxX, float minY1, float maxY1, float minY2, float maxY2)
{
  //Initialize parameters needed for graphing. Linear/log must be set before this.
  //This is used when graphing will be done from a foreign source, point by point
  //If graphing from gGraphVal, create gGraphVal with gSetNumPoints 0 and gAddPoints and
  //then call gFindMinMax.
  //Then call gCalcGraphParams
  gXAxisMin=minX; gXAxisMax=maxX;
  gY1AxisMin=minY1; gY1AxisMax=maxY1;
  gY2AxisMin=minY2; gY2AxisMax=maxY2;
  gCalcGraphParams();
}

void msagraph::gShiftRefLevel(int axisNum, int nDiv)
{/*
'shift Y1 or Y2 reference level by nDiv divisions; return amount of shift
    'This means the min and max for the axis are shifted up or down an amount that
    'depends on the size of a vertical division. This does not affect the y scale, but we have to call
    'gCalcGraphParams to recalculate the grid label values
    if axisNum=1 then
        shiftSize=nDiv*(gY1AxisMax-gY1AxisMin)/gVertDiv
        gY1AxisMin=gY1AxisMin+shiftSize : gY1AxisMax=gY1AxisMax+shiftSize
    else
        shiftSize=nDiv*(gY2AxisMax-gY2AxisMin)/gVertDiv
        gY2AxisMin=gY2AxisMin+shiftSize : gY2AxisMax=gY2AxisMax+shiftSize
    end if
    gShiftRefLevel=shiftSize
    call gCalcGraphParams   'Need to recalculate grid label values*/
}
void msagraph::gCalcAxis(int isX, QString axisStyle, int &isLin, int &numDiv
                           , float &axisMin, float &axisMax, float &scale, float &blackHoleRadius, float &blackHolePix, QString &gridLines)
{

  //gCalcAxis was heavily modified by ver116-4k to allow negtive and zero values in log scans.
  //Calculates value at axis max, and scale for graphing, given  max and min
  //coordinate values. isX=1 to indicate we are using X axis. isLin=1 for linear, 0 for log.
  //If using log, then numDiv will be the number of divisions for the axis to give one
  //division per decade.  If linear, we do not calculate numDiv.
  //For log sweeps that cross zero (not allowed for x-axis), we calculate blackHoleRadius; anything within that
  //radius of zero is graphed as zero, whose grid line is placed at blackHolePix. If the log sweep ends at zero at either end
  //(and thus does not cross zero), we do a similar calculation for blackHoleRadius and make that or its negative the end point,
  //after which the blackHoleRadius variable is no longer needed.
  //We create in gridLines$ a comma delimited list of values and pixel locations at which grid lines
  //are located. Value is first, then pixel location. numDiv+1
  //is the number of such grid lines. (There is no grid line at the beginning or end, where the grid
  //boundary would cover them.)
  float axisLen;
  float span;
  if (isX==1)
  {
    axisLen=gGridWidth;
  }
  else
  {
    axisLen=gGridHeight;
  }
  float gridStart;
  float gridStartPix;
  int doLabel;
  //Zero span can cause problems
  if (axisMax<=axisMin)
  {
    if (isX)
    {
      //For X axis, we leave the span at zero; the user will have to use
      //a method other than multiplying by a scale factor to step through
      //the x axis. And we force to linear scaling of the x axis, with scale of 1
      axisMax=axisMin;
      if (isLin==0)    //Change log to linear with 10 divisions for zero span ver116-4a
      {
        isLin=1;
        numDiv=10;
      }
    }
    else
    {
      //For Y axes, pick min and max to center the graph
      axisMin=axisMin-1;
      axisMax=axisMax+1;
      isLin=1;
    }
  }
  int includesZero=(axisMin<=0) && (axisMax>=0);
  int crossesZero=axisMin*axisMax<0;
  if (isLin==0)  //log sweep ver116-4k
  {
    if (includesZero)    //Includes zero; may or may not cross zero
    {
      if (isX)
      {
        if (crossesZero)   //don't let x axis cross zero
        {
          axisMin=0;
          crossesZero=0;
        }
        if (qMax(fabs(axisMin), fabs(axisMax))<(float)0.0001)
        {
          isLin=1; //small frequency values; make it linear
        }
        else
        {
          float blackHolePower=-4;
          blackHoleRadius=0.0001;    //Use 100 Hz as blackHoleRadius for X axis if feasible
          float maxMag;
          if (axisMin==0)
          {
            axisMin=blackHoleRadius;
            blackHolePix=0;
            maxMag=axisMax;
          }
          else
          {
            axisMax=0-blackHoleRadius;
            blackHolePix=axisLen;
            maxMag=0-axisMin;
          }
          span=util.uSafeLog10(maxMag)-blackHolePower;
        }
      }
      else //Y axis
      {
        //We choose a blackHoleRadius to provide a reasonable number of decades
        if (crossesZero)
        {
          float gridTopPower=util.uRoundUp(util.uSafeLog10(axisMax));
          float gridBotPower=util.uRoundUp(util.uSafeLog10(0-axisMin));
          float commonPower=qMin(gridTopPower, gridBotPower);
          //if we used 10^commonPower as blackHoleRadius, we would have (gridTopPower-commonPower)+ (gridBotPower-commonPower)
          //whole or partial decades.
          float blackHolePower=commonPower-1;    //Would provide at least 1 decade of graph on each side.
          float minDecades=(gridTopPower-commonPower)+ (gridBotPower-commonPower);
          if (minDecades>6)
          {
            if (minDecades>9)
            {
              blackHolePower=minDecades-9;
            }
            else
            {
              blackHolePower=commonPower;
            }
          }
          else
          {
            blackHolePower=commonPower-(int)((6-minDecades)/2);
          }
          blackHoleRadius=util.uTenPower(blackHolePower);
          float negativeSpan=util.uSafeLog10(0-axisMin)-blackHolePower;
          float positiveSpan=util.uSafeLog10(axisMax)-blackHolePower;
          span=negativeSpan+positiveSpan;   //Total whole or partial decades
          blackHolePix=(axisLen*negativeSpan/span); //negative span as proportion of total span, times axis len
        }
        else //Y axis equals zero at one end
        {
          //float absMin;
          float absMax;
          if (axisMin<0)
          {
            //absMin=0 ;
            absMax=fabs(axisMin);
          }
          else
          {
            //absMin=axisMin;
            absMax=0;
          }
          float maxPower=util.uSafeLog10(absMax);
          float blackHolePower=(int)(maxPower)-6;  //Provide for approx six decades
          blackHoleRadius=util.uTenPower(blackHolePower);
          //grid line for blackHoleRadius will be at one end of axis
          if (axisMin==0)
          {
            axisMin=blackHoleRadius;
            blackHolePix=0;
          }
          else
          {
            axisMax=0-blackHoleRadius;
            blackHolePix=axisLen;
          }
          span=maxPower-blackHolePower;    //number of decades in range
          if (span<0.7)     //Too narrow a range--may cause trouble with grid lines
          {
            isLin=1;
          }
          if (span>12)  //don't allow huge span--too many grid lines
          {
            float excessSpan=span-12;
            blackHolePower=blackHolePower+excessSpan ; span=12;
            blackHoleRadius=util.uTenPower(blackHolePower);
            if (axisMax>0)
            {
              axisMin=blackHoleRadius;
            }
            else
            {
              axisMax=0-blackHoleRadius;
            }
          }
          includesZero=0;  //no longer includes zero
        }
      }
    }
    else    //X or Y axis that does not include zero--check for acceptable span
    {
      float absMax=qMax(fabs(axisMax), fabs(axisMin));
      float absMin=qMin(fabs(axisMax), fabs(axisMin));
      if (isX && absMax>0.0001 && absMin<0.0001)
      {
        absMin=0.0001; //Min for x axis should be at least 100 Hz if feasible
        if (axisMin<0)
        {
          axisMax=0-absMin;
        }
        else
        {
          axisMin=absMin;
        }
      }
      span=util.uSafeLog10(absMax/absMin);    //number of decades in range
      if (span<0.7)     //Too narrow a range--may cause trouble with grid lines
      {
        isLin=1;
      }
      if (span>12)  //don't allow huge span--too many grid lines, so adjust range
      {
        absMin=absMax/util.uTenPower(12); //Change the range limit closest to zero
        if (axisMin<0)
        {
          axisMax=0-absMin;
        }
        else
        {
          axisMin=absMin;
        }
        span=util.uSafeLog10(absMax/absMin);
      }
    }
  }
  if (isLin)
  {
    //int blackHoleRadius=0 ; int blackHolePix=0;  //not relevant to linear sweeps
    //Set up for even spacing
    span=axisMax-axisMin;
    //If zero span (allowed only for x axis), scale is multiplied by the step number to get the pixel value
    //Otherwise, scale is multiplied by the x value minus the leftmost x value
    if (span==0)
    {
      scale=axisLen/gDynamicSteps;
    }
    else
    {
      scale=axisLen/span;
    }
    float divWidth=axisLen/numDiv;
    float valInterval=(axisMax-axisMin)/numDiv;
    float thisPix=0;
    gridLines="";
    //int firstLine=divWidth;
    QString joint="";
    //Construct a string with pixel values for grid line locations, with the axis start being pixel zero
    for (int i=0; i <= numDiv; i++)
    {
      float thisVal=axisMin+valInterval*i;
      doLabel=gDoPrintLabel(axisStyle,i+1, numDiv+1);
      //Append comma, value, comma, pixel,doLabel,comma
      gridLines=gridLines + joint + QString::number(thisVal) + "," + QString::number(thisPix) + "," + QString::number(doLabel);
      thisPix=thisPix+divWidth;    //To next line
      joint=",";
    }
    if (isX==0)  //Scale is negative so higher Y moves up on graph ver116-4k
    {
      scale=0-scale;
    }
    return;
  }

  //Here for log sweep.
  //We draw grid lines on the ends and at powers of ten and proper multiples of powers of ten.
  //If mult=1, those include all multiples 2...9. If mult=2 they include only even multiples 2,4,6,8.
  //For any other value of mult, we only draw powers of ten.
  //Normally, gridStart is the biggest ten power less than or equal to axisMin, so it would be the leftmost
  //grid line we would draw if we started with a whole decade, but it may be off the left of
  //If negative values are included, the grid lines for negative values are effectively flipped horizontally
  //from their equivalent positive values, and gridStart is moved to blackHoleRadius, if included, or to the rightmost whole
  //power of ten (which may be off the right side of the graph)
  //If zero is included, values <=blackHoleRadius from zero will be forced to zero, and the zero grid
  //line is effectively a grid line for all powers of ten that get forced to zero. We could let the user
  //specify blackHoleRadius, but we will choose it in a way that produces a reasonable number of decades.

  scale=axisLen/span; //"scale" here is the number of pixels per decade

  int mult=10;
  if (scale>60)     //feasible density of grid lines depends on scale (pixels per decade)
  {
    mult=2;
  }
  if (scale>=120)
  {
    mult=1;
  }

  if (isX==0)  //Scale is negative so higher Y moves up on graph
  {
    scale=0-scale;
  }
  //gridStart will be the power of ten at which we begin. For all positive, it is at or to left of chart; for all negative
  //it is at or to right of chart. If zero is included, gridStart is at blackHoleRadius, which is actually labeled 0 and
  //is also equivalent to -blackHoleRadius.
  if (crossesZero)
  {
    //For log scans that include zero, we calculate the positive side starting at blackHoleRadius
    //and mirror those to the negative side.
    gridStart=blackHoleRadius;
    gridStartPix=blackHolePix;
  }
  else
  {
    if (axisMin<0)
    {
      gridStart=axisMax;
      gridStartPix=axisLen;
    }
    else
    {
      gridStart=axisMin;
      gridStartPix=0;
    }
  }
  numDiv=0 ;
  float absGridStart=abs(gridStart);
  float currTenPower=util.uRoundDownToPower(absGridStart,10);  //We run through positive currTenPower starting here
  //First grid line is for the axis beginning at axisMin, 0 pixel val, doLabel=1
  gridLines=QString::number(axisMin) + ",0,1";
  float currVal;
  for (int decadeNum=1; decadeNum <= 200; decadeNum++)  //we will never do all 200
  {
    for (int i=1; i<= 9; i++)    //one for each multiple of a power of ten.
    {
      //We run through increasing positive grid values, starting at or below |gridStart|. Once we are at or above
      //|gridStart| we start "drawing" the grid line for positive values, and if appropriate for the negative
      //of that value. We keep going until we are past both the postive and negative ends of the chart.
      if (i==1 || mult==1 || (mult==2 && (int)((i/2)*2)==i))   //do only for lines at desired multiples
      {
        currVal=currTenPower*i;
        if (i==1 && decadeNum==1 && crossesZero)
        {
          //If zero is crossed, we label the starting point 0.
          gridLines=gridLines+",0,"+QString::number(gridStartPix)+","+QString::number(doLabel);    //enter a grid line
        }
        else
        {
          if (currVal>=absGridStart)
          {
            float currPosPix=gridStartPix+scale*(util.uSafeLog10(currVal/absGridStart));
            float currNegPix=int(0.5+10*(2*gridStartPix-currPosPix))/10;   //reflect through gridStartPix and round to tenth of pixel
            currPosPix=((int)(0.5+10*currPosPix))/10;   //round to tenth of pixel
            if ((axisMax>0) && (currVal<=axisMax))
            {
              if (currPosPix>=2 && currPosPix<=axisLen-2)  //don't want to do very ends; we do that below
              {
                doLabel=(i==1 && (isX==0 || (currPosPix>=40 && currPosPix<=axisLen-40)));   //label powers of ten if not too close to end of x-axis
                gridLines=gridLines+","+QString::number(currVal)+","+QString::number(currPosPix)+","+QString::number(doLabel);    //enter a grid line
                numDiv=numDiv+1;
              }
            }
            if ((includesZero || (axisMax<0)) && (0-currVal>=axisMin))  //need to do negative grid line
            {
              if (currNegPix>=2 && currNegPix<axisLen-2)  //don't want to do very ends; we do that below
              {
                doLabel=(i==1 && (isX==0 || (currNegPix>=40 && currNegPix<=axisLen-40)));   //label powers of ten if not too close to end of x-axis
                gridLines=gridLines+","+QString::number(0-currVal)+","+QString::number(currNegPix)+","+QString::number(doLabel);    //enter a grid line
                numDiv=numDiv+1;
              }
            }
          }
        }
      }
    }
    if (currVal>=axisMax && 0-currVal<=axisMin)
    {
      break;    //End loop if at or outside both ends of chart
    }
    currTenPower=10*currTenPower;
  }
  gridLines=gridLines+","+QString::number(axisMax)+","+QString::number(axisLen)+",1";    //Final grid line
  numDiv=numDiv+1; //1 less than number of grid lines
}
void msagraph::gCalcGraphParams()
{
  //Calculate scaling and, for log, number of divisions
  //min and max X and Y values, number of divisions, log/linear, and grid style must already be set up
  //For log scans, also calculates and replaces gHorDiv or gVertDiv
  //Will force any axis scale to linear if log is inappropriate for some reason.
  //Also calculates grid line locations and labels

  gHorDiv=qMax(1,gHorDiv);
  gVertDiv=qMax(1,gVertDiv);

  //gCalcAxis may change gXIsLinear and gHorDiv
  //fix me
  QString gridLines;
  float dumBlackHolePix, dumRadius;

  gCalcAxis(1, gXGridStyle, gXIsLinear, gHorDiv, gXAxisMin, gXAxisMax
            , gXScale, dumRadius, dumBlackHolePix, gridLines);

  for (int i=1; i<= gHorDiv+1; i++)  //Save grid line locations and labels
  {
    float gridVal, gridPix, doLabel;
    util.uExtractNumericItems(3,gridLines, ",", gridVal, gridPix, doLabel);
    gXGridLines[i]=gridPix;
    if (doLabel==1)   //gCalcAxis determined whether a label is needed
    {
      gXGridLabels[i]=gGridBoundaryLabel(gridVal, gXAxisForm);   //format the grid line//s value
    }
    else
    {
      gXGridLabels[i] =""; //If no label, enter blank
    }
  }

  //Y parameters are set to accommodate Y1 unless only Y2 is graphed
  //gCalcAxis may change gY1IsLinear and gVertDiv
  if (gDoY1==1)
  {
    gCalcAxis(0, gY1GridStyle,gY1IsLinear, gVertDiv, gY1AxisMin, gY1AxisMax
              , gY1Scale, gY1BlackHoleRadius, gY1BlackHolePix, gridLines); //ver116-4k
    for (int i=1; i <= gVertDiv+1; i++)  //Save grid line locations and labels
    {
      float gridVal, gridPix, doLabel;
      util.uExtractNumericItems(3,gridLines, ",", gridVal, gridPix, doLabel);
      gY1GridLines[i]=gridPix;
      if (doLabel==1)
      {
        gY1GridLabels[i]=gGridBoundaryLabel(gridVal, gY1AxisForm);
      }
      else
      {
        gY1GridLabels[i] ="";
      }
    }
  }
  if (gDoY2==1)
  {
    //If Y1 is graphed, Y2 will go with gVertDiv calculated for Y1. We do this by
    //forcing the isLinear param to 1. The only situation that doesn't work is if Y1
    //is linear and Y2 is log, in which case Y2 would like to recalculate gVertDiv
    int lin;
    if (gDoY1==0)
    {
      lin=gY2IsLinear;
    }
    else
    {
      lin=1;
    }
    //gCalcAxis may change gY2IsLinear and gVertDiv
    gCalcAxis(0, gY2GridStyle, lin, gVertDiv, gY2AxisMin, gY2AxisMax
              , gY2Scale, gY2BlackHoleRadius, gY2BlackHolePix, gridLines); //ver116-4k
    for (int i=1; i <= gVertDiv+1; i++)  //Save grid line locations and labels
    {
      float gridVal, gridPix, doLabel;
      util.uExtractNumericItems(3,gridLines, ",", gridVal, gridPix, doLabel);
      gY2GridLines[i]=gridPix;
      if (doLabel==1)
      {
        gY2GridLabels[i]=gGridBoundaryLabel(gridVal, gY2AxisForm);
      }
      else
      {
        gY2GridLabels[i] ="";
      }
    }
    if (gDoY1==0)
    {
      gY2IsLinear=lin;     //May have been forced to linear by gCalcAxis
    }
  }
  //if gDoY1=0 and gDoY2=0 then gVertDiv=0 //delver115-2c
  //Y parameters are set to accommodate Y1; Y2 will be scaled to fit those
}

int msagraph::gPixIsInGrid(int xPix, int yPix)
{
  //Return 1 if (xPix, yPix) is within grid bounds; otherwise 0
  //We allow an extra pixel boundary to be part of the grid
  if (xPix < gMarginLeft-1 || xPix > gMarginLeft+gGridWidth+1)
    return 0;

  if (yPix < gMarginTop-1 || yPix > gOriginY+1)
    return 0;

  return 1;
}
void msagraph::gFindClickedPoint(int xPix, int &yPix, int &pointNum, int &traceNum)
{
  //Return data for clicked point
  //xPix, yPix originally represent the pixel coordinates of a mouse click. We determine what
  //graph point was clicked and return its point number and trace number, and the Y-pixel value at the clicked trace.
  //pointNum will be the number (1...), possibly non-integral, of the
  //clicked point, and traceNum (1 or 2) will be the number of the trace that was clicked.
  //We return pointNum=0 if no graph point was near the click. pointNum may be fractional
  if (gNumPoints<2)
  {
    pointNum=0;
    return;
  }
  if (yPix > gOriginY + 2 || yPix < gMarginTop - 2)
  {
    pointNum=0;
    return;  //click not in grid
  }
  pointNum=gPointNumOfXPix(xPix);  //Point number for xPix
  int pMin, pMax;
  gGetMinMaxPointNum(pMin, pMax);
  //Allow click to be a bit outside graph to mark end point
  if (pointNum<pMin)
  {
   if (pointNum>=0)
     pointNum=pMin;
   else
     pointNum=0;   //zero indicates outside graph
  }
  else
  {
    if (pointNum>pMax)
    {
     if (pointNum<=pMax+1)
       pointNum=pMax;
     else
       pointNum=0;
    }
  }
  //We determine the closest trace //ver114-7d changed to use actual pixel values, not nearest point values
  xPix=gGetPointXPix(pointNum);
  float y1Pix=gGetPointYVal(pointNum,1);
  float y2Pix=gGetPointYVal(pointNum,2);   //These aren't pixel values yet
  gConvertY1ToPix(y1Pix);
  gConvertY2ToPix(y2Pix);
  int d1, d2;
  if (y1Pix>yPix)
      d1=y1Pix-yPix;
  else
    d1=yPix-y1Pix; //distance from trace 1
  if (y2Pix>yPix)
    d2=y2Pix-yPix;
  else
    d2=yPix-y2Pix; //distance from trace 2
  if (gGraphY2==0 || (gGraphY1=1 && d1<d2 ))
  {
    traceNum=1;
    yPix=y1Pix;   //Closest to trace 1; or we are not doing trace 2
  }
  else
  {
    traceNum=2;
    yPix=y2Pix;   //Closest to trace 2; or we are not doing trace 1
  }
}
int msagraph::gPixelsPerStep()  //return number of pixels per step    'ver116-4k
{
    if (gDynamicSteps<=0)
      return gGridWidth;
    else
      return gGridWidth/gDynamicSteps;
}
float msagraph::gPointNumOfX(int x)
{
  //Return point num (1...), possibly non-integral, for the X value x
  //Note this can yield point numbers above or below the actual graph range, for x values outside that range
  int pMin, pMax;
  gGetMinMaxPointNum(pMin, pMax);    //ver114-6d
  if (gXAxisMin==gXAxisMax)
    return (pMin+pMax)/2;   //All have same X value; use middle one

  if (gXIsLinear==1)
  {
    float interval=(gXAxisMax-gXAxisMin)/gDynamicSteps;      //MHz per step
    return 1+(x-gXAxisMin)/interval;
  }
  else
  {
    return 1+gDynamicSteps*util.uSafeLog10(x/gXAxisMin)/util.uSafeLog10(gXAxisMax/gXAxisMin);
  }
}

float msagraph::gPointNumOfXPix(float xPix)
{
  //Return point num (1...), possibly non-integral, for the X pixel xPix
  //Note this can yield point numbers above or below the actual graph range, for pixels outside that range
  //ver114-6d used gDynamicSteps in place of gNumPoints to find the final point; this is necessary
  //for reverse sweeps.
  float x1=gGraphPix[1][0];
  float x2=gGraphPix[gDynamicSteps+1][0];    //First and last graph points; x pixel values
  int pMin, pMax;
  gGetMinMaxPointNum(pMin, pMax);
  if (gNumPoints<2)
  {
    return pMin;
  }
  float interval=(x2-x1)/(gDynamicSteps); //Pixels from point to point
  if (interval==0)  //Weird situation; entire graph at one x pixel value; may happen with halted graph
  {
    return pMin;
  }
  else
  {
    float p=1+(xPix-x1)/interval;
            //If point number is very close to an integer, make it one.
    int nearInt=int(p+0.01); //e.g. p=2.99 and p=3.01 both produce nearInt=3
    float dif = p - nearInt;
    if (dif<0.01 && dif>-0.01)
    {
      p=nearInt; //If close, make p=nearInt
    }
    return p;
  }
}

void msagraph::gConvertXToPix(float &x)
{
  //Convert x numeric value to pixel coordinates mod ver116-4k
  if (gXIsLinear==1)
  {
    x=(x-gXAxisMin)*gXScale+gMarginLeft;
  }
  else    //log sweep
  {
    //Note we presently do not allow the x axis to cross zero, and if it originally equalled zero at one end
    //that value has been made non-zero. So zero is never included at this point
    float gridStart;
    if (gXAxisMin<0) gridStart=gXAxisMax; else gridStart=gXAxisMin;
    float ratio=(x-gridStart)/gridStart;
    if (ratio<0) { x=-1; return; }  //x is out of bounds; return pixel=-1 ver116-4k
    float pixDist=util.uSafeLog10(ratio)*gXScale; //Num of pixels from start to value x
    if (gXAxisMin>0) //all positive values with grid starting at left
    {
      x=pixDist+gMarginLeft;
    }
    else //all negative values with grid starting at right
    {
      x=gGridWidth-pixDist+gMarginLeft;
    }
  }
  x=((int)(x*10+0.5))/10;    //Round to tenth of a pixel    */
}

void msagraph::gConvertY1ToPix(float &y1)
{
  //Convert Y1 numeric values to pixel coordinates. Y values should already be in bounds
  //Modified by ver116-4k to allow negative values in log sweeps
  if (gDoY1==0)
  {
    y1=-10;
    return;
  }
  if (gY1IsLinear==1)
  {
    y1=(y1-gY1AxisMin)*gY1Scale+gOriginY;
  }
  else
  {
    if (gY1AxisMin<0 && gY1AxisMax>0)  //axis crosses zero
    {
      if (y1>=0-gY1BlackHoleRadius && y1<=gY1BlackHoleRadius)
      {
        y1=gOriginY+int(gY1BlackHoleRadius*10+0.5)/10;
        return; //within black hole
      }
      if (y1>=gY1BlackHoleRadius)
      {
        y1=util.uSafeLog10((y1-gY1BlackHoleRadius)/gY1BlackHoleRadius)*gY1Scale
            +gY1BlackHolePix+gOriginY;
      }
      else
      {
        y1=gY1BlackHolePix-util.uSafeLog10((gY1BlackHoleRadius-y1)/gY1BlackHoleRadius)*gY1Scale+gOriginY;
      }
      y1=((int)(y1*10+0.5))/10.0;
      return;  //Round to tenth and exit
    }
    //Here for no zero crossing. If all positive, axis starts at gXGridStart; if all negative it ends at gXGridStart.
    float gridStart;
    if (gY1AxisMin<0)
      gridStart=gY1AxisMax;
    else
      gridStart=gY1AxisMin;
    float ratio=(y1-gridStart)/gridStart;
    if (ratio<0)
    {
      y1=-1;
      return;
    }  //y is out of bounds; return pixel=-1 ver116-4k
    float pixDist=util.uSafeLog10(ratio)*gY1Scale; //Num of pixels from start to value y
    if (gY1AxisMin>0)  //all positive values with grid starting at bottom
    {
      y1=pixDist+gOriginY;
    }
    else //all negative values with grid starting at top
    {
      y1=gGridHeight-pixDist+gOriginY;
    }
  }
  y1=((int)(y1*10+0.5))/10;    //Round to tenth of a pixel
  if (y1<gMarginTop)
    y1=gMarginTop; //don't let it go off the top
  if (y1>gOriginY)
    y1=gOriginY;     //don't let it go off the bottom
}

void msagraph::gConvertY2ToPix(float &y2)
{
  //Convert Y2 numeric values to pixel coordinates. Y values should already be in bounds
  //Modified by ver116-4k to allow negative values in log sweeps
  if (gDoY2==0) { y2=-10; return; }
  if (gY2IsLinear==1)
  {
    y2=(y2-gY2AxisMin)*gY2Scale+gOriginY;
  }
  else
  {
    if (gY2AxisMin<0 && gY2AxisMax>0) //axis crosses zero
    {
        if (y2>=0-gY2BlackHoleRadius && y2<=gY2BlackHoleRadius)
        {
          y2=gOriginY+(int)(gY2BlackHoleRadius*10+0.5)/10;
          return; //within black hole
        }
        if (y2>=gY2BlackHoleRadius)
        {
            y2=util.uSafeLog10((y2-gY2BlackHoleRadius)/gY2BlackHoleRadius)*gY2Scale+gY2BlackHolePix+gOriginY;
        }
        else
        {
            y2=gY2BlackHolePix-util.uSafeLog10((gY2BlackHoleRadius-y2)/gY2BlackHoleRadius)*gY2Scale+gOriginY;
        }
        y2=((int)(y2*10+0.5))/10;
        return; //Round to tenth and exit
    }
    //Here for no zero crossing. If all positive, axis starts at gXGridStart; if all negative it ends at gXGridStart.
    float gridStart;
    if (gY2AxisMin<0)
      gridStart=gY2AxisMax;
    else
      gridStart=gY2AxisMin;
    float ratio=(y2-gridStart)/gridStart;
    if (ratio<0)
    {
      y2=-1;
      return;
    }  //y is out of bounds; return pixel=-1
    float pixDist=util.uSafeLog10(ratio)*gY2Scale; //Num of pixels from start to value y
    if (gY2AxisMin>0) //all positive values with grid starting at bottom
    {
      y2=pixDist+gOriginY;
    }
    else //all negative values with grid starting at top
    {
      y2=gGridHeight-pixDist+gOriginY;
    }
  }
  y2=((int)(y2*10+0.5))/10;    //Round to tenth of a pixel
  if (y2<gMarginTop)
    y2=gMarginTop; //don't let it go off the top
  if (y2>gOriginY)
    y2=gOriginY;     //don't let it go off the bottom
}

float msagraph::gAdjustPhaseToDisplay(int axisNum, int pointNum, int useWorkArray)
{
  //Adjust phase of point pointNum and return
  //Assumes axis axisNum is phase. Returns the adjusted phase.
  //if useWorkArray=1, the phase is taken from uWorkArray(); otherwise from gGraphVal()
  //Adjust the phase as follows: first, if possible put it within the bounds
  //of gAxisYNMin and gAxisYNMax. Second, if gAxisYNMax-gAxisYNMin is more than a 360 degree range, there may
  //be more than one possible value. Choose the one that is closest to the previous point//s phase (previous based
  //on gSweepDir).
  //The idea is that a continuous sweep should normally have continuous phase. If our axes allow a range
  //from 0 to 1000 degrees and the measured phase moves from 350 at one point to 10 at the next point,
  //the best guess as to the true phase change is 20 degrees, and we should display 370 degrees.
  float axisMin;
  float axisMax;
  if (axisNum==1)
  {
    axisMax=gY1AxisMax;
    axisMin=gY1AxisMin;
  }
  else
  {
    axisMax=gY2AxisMax;
    axisMin=gY2AxisMin;
  }
  float phaseSpan=axisMax-axisMin;
  if (phaseSpan<0)
  {
    return 0;
    //Error if negative span ver116-1b
  }
  float phase;
  if (useWorkArray)
  {
    phase=uWork->uWorkArray[pointNum][axisNum];
  }
  else
  {
    phase=vars->gGraphVal[pointNum][axisNum];
  }
  float origPhase=phase;
  //Put phase in bounds from gY1AxisMin to gY1AxisMax. In general case, phase can
  //equal either bounds.
  //Add or Subtract 360 until phase becomes in range
  if (phase>axisMax)
  {
    while (phase>axisMax)
    {
      phase=phase-360;
    }
  }
  else
  {
    while (phase<axisMin)
    {
      phase=phase+360;
    }
  }

  if (phase<axisMin || phase>axisMax)
  {
    //Couldn't find in-bounds value; put in normal range. Can happen only if phaseRange<360
    phase=origPhase;
    while (phase>180)
    {
      phase=phase-360;
    }
    while (phase<=-180)
    {
      phase=phase+360;
    }
  }
  else
  {
    //Found in-bounds value; maybe there is more than one in-bounds value (if phaseSpan>=360)
    //If this is the first point, we just go with the angle as is
    float lastPhase;
    if (useWorkArray)
    {
      lastPhase=uWork->uWorkArray[pointNum-gSweepDir][axisNum];
    }
    else
    {
      lastPhase=vars->gGraphVal[pointNum-gSweepDir][axisNum];
    }
    if (pointNum!=gSweepStart && phaseSpan>=360)
    {
      if (lastPhase<phase)
      {
        while (phase>lastPhase && phase>=axisMin)
        {
          phase=phase-360;
        }
        //If phase is in bounds then phase and phase+360 now bracket lastPhase
        //In that case, use the closest value. If phase is now out-of-bounds,
        //use the last in-bounds value
        if (phase<axisMin || lastPhase-phase>180)
        {
          phase=phase+360;
        }
      }
      else
      {
        while (phase<lastPhase && phase<=axisMax)
        {
          phase=phase+360;
        }
        //If phase is in bounds then phase and phase-360 now bracket lastPhase
        //In that case, use the closest value. If phase is now out-of-bounds,
        //use the last in-bounds value
        if (phase>axisMax || phase-lastPhase>180)
        {
          phase=phase-360;
        }
      }
    }
  }
  return phase; //Return adjusted phase*/

}
int msagraph::gNumDynamicSteps()
{
  //Return number of steps, which is one less than number of points
  return gDynamicSteps;
}
void msagraph::gSetNumDynamicSteps(int nSteps)
{
  //Set number of steps in dynamic graph
  //if (nSteps<=2) nSweepPoints=2;
  gDynamicSteps=nSteps ;
  int nPoints=nSteps+1;
  //Our existing points probably aren't any good anyway, but in case they get graphed...
  if (gNumPoints>nSteps+1)
    gNumPoints=nSteps+1;
  gSetMaxPoints(nPoints);  //Be sure we have room
  gSetSweepDir(gSweepDir);  //To set sweep start and end point numbers
}
void msagraph::gSetErasure(int doErase1, int doErase2, int nEraseLead)
{
  //Set erasure parameters
  //nEraseLead specifies the number of points by which the erasure leads the drawing.
  //doEraseN indicates whether trace N should be erased as it is redrawn.
  if (doErase1==0)
    gDoErase1=0;
  else
    gDoErase1=1;
  if (doErase2==0)
    gDoErase2=0;
  else
    gDoErase2=1;
  if (nEraseLead<=0)
    nEraseLead=0-nEraseLead;   //make it positive
  gEraseLead=nEraseLead*gSweepDir;   //Set its sign based on sweep direction
  gInitDraw();  //Creates draw and erase prefixes
}

void msagraph::gGetErasure(int &doErase1, int &doErase2, int &nEraseLead)
{
  //Get erasure parameters
  doErase1=gDoErase1;
  doErase2=gDoErase2;
  nEraseLead=gEraseLead;
}

void msagraph::gPauseDynamicScan()
{
  //Suspend until next gResumeDynamicScan or gInitDynamicDraw
  //When the user halts the scanning, the screen may be redrawn, and we don't want any
  //of those draw commands mixed into gTrace1$() or gTrace2$(). In addition, we want to save
  //the last graphed point for proper Resuming.
  gIsDynamic=0;    //Signal not to accumulate drawing commands
  gLastPointBeforePause=gPrevPointNum;     //Save number of last point drawn (1...)
  gWasFirstScanAtPause=gIsFirstDynamicScan; //Save whether we were in the first scan.

}

void msagraph::gResumeDynamicScan()
{
  //Resume scanning after screen has been fully redrawn
  //We set gPrevPointNum and other things that got changed when the screen was redrawn.
  gPrevPointNum=gLastPointBeforePause; //Start where we left off
  gIsFirstDynamicScan=gWasFirstScanAtPause;
  gIsDynamic=1;    //Signal to accumulate drawing commands
}

void msagraph::gRestoreErasure()
{
  //Restore the erasure gEraseLead points in front of the traces. This is called
  //after redrawing the graphs.
  //Erasures occurring immediately adacent to the current draw point can affect pixels for prior draw
  //points (which may actually be at the same rounded off pixel x-value as the current point. So after
  //erasing we redraw a couple of points.
  //gPrevPointNum is the last point previously drawn, and thus is the point from which
  //the erasure is to begin, and the final point that we will redraw.
  if (gIsFirstDynamicScan==1 || (gDoErase1==0 && gDoErase2==0))
      return;

  int saveNum=gPrevPointNum;
  gInitErase();
  //Redraw some retroactive points if possible to eliminate erasure artifacts
  //The best indicator of how many to redraw is gEraseLead.
  int nRedraw;
  if (gSweepDir==1)
  {
    nRedraw=qMin(gPrevPointNum, gEraseLead);
    gPrevPointNum=gPrevPointNum-nRedraw;
  }
  else
  {
    if (gPrevPointNum == gNumPoints)
    {
      nRedraw=1;
      gPrevPointNum=0;     //zero signals we are at first point
    }
    else
    {
      nRedraw=qMin(gNumPoints-gPrevPointNum, 0-gEraseLead);   //note gEraseLead is negative here
      gPrevPointNum=gPrevPointNum+nRedraw;
    }
  }
  for (int i=1; i <= nRedraw; i++)
  {
    gDrawNextPoint();
  }
  gPrevPointNum=saveNum;

}

void msagraph::gStartNextDynamicScan()
{
 //Prepare for next pass of dynamic draw
    //#gGraphHandle$, "discard"   'Discard last scan info stored in Liberty Basic
    gIsFirstDynamicScan=0;
    gPrevPointNum=0;
    gInitDraw();  //implements current color and width in case they were changed at end of prior scan ver116-4s
    gInitErase(); //Erase first group of segments if required
}

void msagraph::gDynamicDrawPoint(float y1, float y2)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Do segment erase and redraw for dynamic drawing
    'This is called repeatedly to dynamically erase (if necessary) the prior line and draw a new one.
    'Each call draws the segment from gPrevPointNum to gPrevPointNum+1, using y1 and y2
    'as the new values for point gPrevPointNum+1; those values are also saved.
    'x values with pixel coords must already be in place via gGenerateXValues.
    'Erasure is not done on the very first scan of points, signalled by gIsFirstDynamicScan=1.
    'On the first scan, we save the x value; thereafter we reuse them. We always save the y values.
    'After the first scan, if gDoErase for the trace is 1, then before we draw a segment we erase the segment
    'from point gPrevPointNum+gEraseLead to the point after it.
    'This subroutine is called for each point except the first point of each scan, for which gStartNextDynamicPass
    'is called.
    'The data for the prior line, is already in gGraphVal(); we reuse the X-values.
    if gIsFirstDynamicScan=1 then
        if gNumPoints>vars->gMaxPoints then notice "Too many points to " : exit sub
        gNumPoints=gNumPoints+1
        'Do Draw.
        call gDrawNextPointValue y1,y2  'Draw segment and save values
    else
        call gEraseNextPoint
            'Do Draw.
        call gDrawNextPointValue y1,y2  'draw new segment and save values
    end if
    currPoint=gPrevPointNum 'gPrevPointNum has been updated to this point*/
}

void msagraph::gDynamicComboDrawPoint(float y1, float y2)
{
  //Do segment erase and redraw for dynamic drawing
  //This is called repeatedly to dynamically erase (if necessary) the prior line and draw a new one.
  //Each call draws the segment from gPrevPointNum to gPrevPointNum+1, using y1 and y2
  //as the new values for point gPrevPointNum+1; those values are also saved.
  //Erasure is not done on the very first scan of points, signalled by gIsFirstDynamicScan=1.
  //On the first scan, we save the x value; thereafter we reuse them. We always save the y values.
  //After the first scan, if gDoErase for the trace is 1, then before we draw a segment we erase the segment
  //from point gPrevPointNum+gEraseLead to the point after it.
  //This subroutine is called for each point except the first point of each scan, for which gStartNextDynamicPass
  //is called.
  //The data for the prior line, is already in gGraphVal(); we reuse the X-values.
  int prevPoint=gPrevPointNum;
  if (prevPoint == 399)
  {
    qDebug() << "399";
  }
  //1A. Erase prior segments if required
  // created this if... block to deal with reverse sweeps
  // modified to use gSweepStart and gSweepEnd
  int oneBeforeStart = gSweepStart - gSweepDir;    //The point before the sweep start
  if (prevPoint==0)
    prevPoint=oneBeforeStart;

  int isFirstPoint;
  int currPoint;
  int isFinalDrawPoint;
  if (prevPoint==oneBeforeStart)
    isFirstPoint=1;
  else
    isFirstPoint=0;
  currPoint=prevPoint+gSweepDir;

  if (currPoint == gSweepEnd)
    isFinalDrawPoint=1;
  else
    isFinalDrawPoint=0;
  int prevErasePoint=prevPoint+gEraseLead; //Erase is ahead of draw by gEraseLead points
  int thisErasePoint=prevErasePoint+gSweepDir;
  int eraseInBounds=0;

  if (gSweepDir==1)
  {
    if (thisErasePoint>=gSweepStart && thisErasePoint<=gSweepEnd)
      eraseInBounds=1;
  }
  else
  {
    if (thisErasePoint<=gSweepStart && thisErasePoint>=gSweepEnd)
      eraseInBounds=1;
  }

  if (gIsFirstDynamicScan==0 && eraseInBounds)
  {
    //1. Erase a segment on each trace
    if (gErase1[thisErasePoint] != 0)
    {
      QGraphicsItem *temp = gErase1[thisErasePoint];
      graphScene->removeItem(temp);
      delete temp;
      gErase1[thisErasePoint] = 0;
    }
    if (gErase2[thisErasePoint] != 0)
    {
      QGraphicsItem *temp = gErase2[thisErasePoint];
      graphScene->removeItem(temp);
      delete temp;
      gErase2[thisErasePoint] = 0;
    }
  }  //End Erase

  //2. Draw the new Segments
  //created isFirstPoint and isFinalPoint, and added the gSweepDir tests
  if (currPoint > vars->gGraphVal.count() || currPoint > 410)
  {
    qDebug() << "over max";
  }
  vars->gGraphVal[currPoint][1]=y1;
  vars->gGraphVal[currPoint][2]=y2;
  //2A. Save Y1 and convert to pixel coords
  if (gDoY1==1)
  {
    //Adjust phase by multiples of 360 to conform to graph bounds
    if (gY1IsPhase)
    {
      y1=gAdjustPhaseToDisplay(1, currPoint, 0);
      vars->gGraphVal[currPoint][1]=y1;
    }
    if (gY1IsLinear==1)
      y1=(y1-gY1AxisMin)*gY1Scale+gOriginY;
    else
      gConvertY1ToPix(y1);

    // disbale rounding as we are not using trings for graphics
    //We round pixels to the nearest tenth just to keep the accumulated draw commands shorter
    //y1=int(10*y1+0.5)/10;

    if (y1<gMarginTop)
      y1=gMarginTop; //don't let it go off the top
    if (y1>gOriginY)
      y1=gOriginY;     //don't let it go off the bottom
  }
                //2B. Save y2 and convert to pixel coords
  if (gDoY2==1)
  {
    if (gY2IsPhase)
    {
      y2=gAdjustPhaseToDisplay(2, currPoint,0);
      vars->gGraphVal[currPoint][2]=y2;
    }
    if (gY2IsLinear==1)
        y2=(y2-gY2AxisMin)*gY2Scale+gOriginY;
    else
        gConvertY2ToPix(y2);

    if (y2<gMarginTop)
      y2=gMarginTop; //don't let it go off the top
    if (y2>gOriginY)
      y2=gOriginY;     //don't let it go off the bottom

    // disbale rounding as we are not using trings for graphics
    //We round pixels to the nearest tenth just to keep the accumulated draw commands shorter
    //y2=int(10*y2+0.5)/10;
  }
        //2C. If first scan, we need to count the points
  if (gIsFirstDynamicScan==1)
  {
    if (gNumPoints>vars->gMaxPoints)
    {
      QMessageBox::about(0, "Notice", "Too many points to ");
      return;
    }
    gNumPoints=gNumPoints+1;
  }
  int x=gGraphPix[currPoint][0];    //Use old x pixel coord; it's still good

  //2D. For every scan, save the y  pixel coords
  gGraphPix[currPoint][1]=y1;
  gGraphPix[currPoint][2]=y2;   //save y pixel coords
  //2E. Find point from which to draw
  //created isFirstPoint and isFinalPoint, and added the gSweepDir tests

  int lastX;
  int lastY1;
  int lastY2;
  if (gDoHist==1)
  {
    //For histogram draw vertical line.
    lastX=x;
    lastY1=gOriginY;
    lastY2=gOriginY;
  }
  else
  {
    //For normal trace, draw from last point if there is one
    if (isFirstPoint==0)
    {
      lastX=gGraphPix[prevPoint][0];
      lastY1=gGraphPix[prevPoint][1];
      lastY2=gGraphPix[prevPoint][2];
    }
  }
  //2F. Construct total draw command as one string and send it
  //The "line" command does not draw the final endpoint of the line, which normally
  //gets drawn as the start point of the next line. But for the last point, or for
  //any point at which a halt occurs, that final pixel does not get drawn.
  //ver114-6d modified to accumulate all draw commands
  if (gDoY1)
  {
    //We normally draw a line, but in special cases we use "set" to set a point
    //We do so for the very first point of non-histogram, and for any histogram point
    //consisting of a single point, which "line" will not draw.
    //ver114-6d modified to accumulate all draw commands when gIsDynamic=1
    if ((isFirstPoint && (gDoHist==0)) || (gDoHist && int(y1)==gOriginY))
    {
      //thisCmd$="set ";x;" ";y1;   //just set the point
    }
    else
    {
      if (gGraphY1 && gTrace1Width!=0)
      {
        QPen pen(gridappearance->gTrace1Color);
        pen.setWidth(gTrace1Width);
        gErase1[vars->thisstep] = graphScene->addLine(lastX, lastY1, x, y1, pen);
      }
    }
    if (gIsDynamic==1)
    {
      gTrace1[currPoint]=QPointF(x,y1);
    }
  }
  if (gDoY2==1)
  {
    if ((isFirstPoint && (gDoHist==0)) || (gDoHist && int(y2)==gOriginY))
    {
        //thisCmd$="set ";x;" ";y2;   //just set the point
    }
    else
    {
      if (gGraphY2 && gTrace2Width!=0)
      {
        QPen pen(gridappearance->gTrace2Color);
        pen.setWidth(gTrace2Width);
        gErase2[vars->thisstep] = graphScene->addLine(lastX, lastY2, x, y2, pen);
      }
    }
    if (gIsDynamic==1)
    {
      gTrace2[currPoint]=QPointF(x,y2);
    }
  }
  gPrevPointNum=currPoint;
}

int msagraph::gCanUseExpeditedDraw()
{
  //Returns 1 if [gDrawSingleTrace] can be used; otherwise 0
  int retVal;

  retVal =(gGraphY2==1
      && gTrace2Width!=0
      && gDoY1==0
      && gY2IsLinear==1
      && gDoHist==0
      && gY2IsPhase==0);
  return retVal;
}

void msagraph::gDrawSingleTrace()
{
  //Simplified draw routine for Liberty Basic for single linear normal mag trace.
  //Requires thispointy2
  //Do segment erase and redraw for dynamic drawing of a single Normal trace for magnitude
  //This is a streamlined version intended to deal with speed issues in Liberty Basic, and is
  //not necessary in other languages.
  //This routine is a substitute for gDynamicComboDrawPoint in "gosub" form. It does not handle trace 1,
  //histograms or log scaling of the y axis. It is thus suitable for Normal traces in spectrum analyzer mode.
  //This is called repeatedly to dynamically erase (if necessary) the prior line and draw a new one.
  //Each call draws the segment from gPrevPointNum to gPrevPointNum+1, using yVal
  //as the new values for point gPrevPointNum+1; that value is also saved.
  //Erasure is not done on the very first scan of points, signalled by gIsFirstDynamicScan=1.
  //X values and x pixel coords have been precalculated. We save the y values and pix coords.
  //After the first scan, if gDoErase for the trace is 1, then before we draw a segment we erase the segment
  //from point gPrevPointNum+gEraseLead to the point after it.
  //This subroutine is called for each point except the first point of each scan, for which gStartNextDynamicPass
  //is called.
  //The data for the prior line, is already in gGraphVal(); we reuse the X-values.

  //Draws single Normal trace 2; not histogram; not log Y values; not phase trace
  int prevPoint = vars->thisstep - vars->sweepDir + 1;      //thisstep runs from 0; here 1 is first point.
  int currPoint=prevPoint+vars->sweepDir;
  int prevErasePoint=prevPoint+gEraseLead;     //Erase is ahead of draw by gEraseLead points
  int thisErasePoint=prevErasePoint+vars->sweepDir;
  if (gIsFirstDynamicScan)
  {
    //1. If first scan, we need to count the points, but not erase
    if (gNumPoints > vars->gMaxPoints)
    {
      QMessageBox::warning(0, "Error", "Too many points to graph.");
      return;
    }
    gNumPoints=gNumPoints+1;
  }
  else
  {
    //2. Erase prior segment if required
    //ver114-4k modified to allow for reverse sweeps
    //ver114-6d: The commands used to draw to a step N are in gTrace1$(N) and gTrace2$(N),
    //without the size/color info. To erase those steps we just add the erasure size/color
    //prefix to those recorded steps.
    int doErase=0;
    if (vars->sweepDir==1)
    {
      if (gDoErase2 && thisErasePoint<=vars->sweepEndStep+1)
        doErase=1;
    }
    else
    {
      if (gDoErase2 && thisErasePoint>=vars->sweepEndStep+1)
        doErase=1;
    }
    if (doErase)
    {
      if (gErase2[thisErasePoint] != 0)
      {
        QGraphicsItem *temp = gErase2[thisErasePoint];
        graphScene->removeItem(temp);
        delete temp;
        gErase2[thisErasePoint] = 0;
      }

    }  //End Erase
  }

  //3. Draw the new Segments
  //3A. Save Y and convert to pixel coord
  vars->gGraphVal[currPoint][2]=thispointy2; //save new y values
  float yPix=(thispointy2-gY2AxisMin)*gY2Scale+gOriginY;
  if (yPix<gMarginTop)
    yPix=gMarginTop; //don't let it go off the top
  if (yPix>gOriginY)
    yPix=gOriginY;     //don't let it go off the bottom
  //We round pixels to the nearest tenth just to keep the accumulated draw commands shorter
  //yPix=((int)(10*yPix+0.5))/10;
  gGraphPix[currPoint][2]=yPix;  //save y pixel coords

  //3B. Construct total draw command as one string and send it
  //ver114-4d modified the following to set the first point
  float xPix=gGraphPix[currPoint][0]; //ver114-4k
  //ver114-4n We set a point by drawing the line backward; except first point of non-histo is directly set.
  //For wide traces, "set" can create artifacts, but they are OK on first point.
  float lastX=gGraphPix[prevPoint][0];
  float lastY2=gGraphPix[prevPoint][2];
  //The "line" command does not draw the full final endpoint of the line, which normally
  //gets drawn as the start point of the next line anyway. But for the last point or histo or for
  //any point at which a halt occurs, the final pixel does not get drawn.
  //ver114-4n We normally draw a line, but in special cases we use "set" to set a point
  //We do so for the very first point of non-histogram, and for any histogram point
  //consisting of a single point, which "line" will not draw.
  if (vars->thisstep==vars->sweepStartStep)
  {
    //   thisCmd$="set ";xPix;" ";yPix
  }
  else
  {
    QPen pen(gridappearance->gTrace2Color);
    pen.setWidth(gTrace2Width);

    gErase2[vars->thisstep] = graphScene->addLine(lastX, lastY2, xPix, yPix,pen);
  }

  gTrace2[currPoint] = QPointF(xPix, yPix);
  gPrevPointNum=currPoint;
}

void msagraph::gDrawNextPointValue(float y1, float y2)
{
  //draw lines from last pix to these values, for y=y1 and y2
  //Note previous point pixel values are already in gGraphPix()
  //X value and pixel coords are alredy in place in gGraphVal() and gGraphPix().
  //For speed, we do things here that could be done by calling other subroutines
  //This is not purely a dynamic draw routine and thus does not assume x values have
  //been precalculated

  int prevPoint=gPrevPointNum;
  if (prevPoint==0)
    prevPoint=gSweepStart-gSweepDir;
  int currPoint=prevPoint+gSweepDir;   //Increment to next point, which is us
  if (gSweepDir==1)
  {
    if (currPoint>gSweepEnd)
      return;
  }
  else
  {
    if (currPoint<gSweepEnd)
      return;
  }

  vars->gGraphVal[currPoint][1]=y1;
  vars->gGraphVal[currPoint][2]=y2;
  int x=gGraphPix[currPoint][0];    //Get previously calculated x pixel value for current point

  //Draw to Y1 if necessary
  if (gDoY1==1)
  {
    //ver114-5e Adjust phase by multiples of 360 to conform to graph bounds
    if (gY1IsPhase)
    {
      y1=gAdjustPhaseToDisplay(1, currPoint, 0);
      vars->gGraphVal[currPoint][1]=y1;
    }
    if (gY1IsLinear==1)
    {
      y1=(y1-gY1AxisMin)*gY1Scale+gOriginY;
    }
    else
    {
      gConvertY1ToPix(y1); //ver116-4k
    }
    //We round pixels to the nearest tenth just to keep the accumulated draw commands shorter
    //y1=((int)(10*y1+0.5))/10;
    if (y1<gMarginTop)
      y1=gMarginTop; //don't let it go off the top
    if (y1>gOriginY)
      y1=gOriginY;     //don't let it go off the bottom
  }
  //Draw to Y2 if necessary
  if (gDoY2==1)
  {
    if (gY2IsPhase)
    {
      y2=gAdjustPhaseToDisplay(2, currPoint, 0);
      vars->gGraphVal[currPoint][2]=y2;
    }
    if (gY2IsLinear==1)
    {
      y2=(y2-gY2AxisMin)*gY2Scale+gOriginY;
    }
    else
    {
      gConvertY2ToPix(y2);
    }
    //We round pixels to the nearest tenth just to keep the accumulated draw commands shorter
    //y2=((int)(10*y2+0.5))/10;
    if (y2<gMarginTop)
      y2=gMarginTop; //don't let it go off the top
    if (y2>gOriginY)
      y2=gOriginY;     //don't let it go off the bottom
  }
  gGraphPix[currPoint][1]=y1;
  gGraphPix[currPoint][2]=y2;
  gDrawNextPoint();
}

void msagraph::gDrawNextPoint()
{
  //Draw from gPrevPointNum to the next point using pix coords in gGraphPix()
  //Also increments gPrevPointNum. If gGraphHandle$="" we just go thru the motions and create gTraceN$()
  //For each Normal trace a command of the form "line x startY x endY;" is appended to its gTraceN$,
  //LB does not completely draw the very end of a line, which is no problem if the end is going
  //to be the starting point of another segment. To be safe, we do it for all points. Except the first
  //point of a Normal trace, where we just set a point.
  //For histograms, we always do the double draw at the end of the line.
  //where x and y are the actual pixel coordinates of the target point.
  //ver114-4m created isFirstPoint and isFinalPoint, and added the gSweepDir tests
  //ver114-5e modified to use gSweepStart and gSweepEnd
  int prevPoint=gPrevPointNum;
  int oneBeforeStart=gSweepStart-gSweepDir;    //The point before the sweep start
  int isFirstPoint;
  if (prevPoint==0)
    prevPoint=oneBeforeStart;
  if (prevPoint==oneBeforeStart)
    isFirstPoint=1;
  else
    isFirstPoint=0;

  int currPoint=prevPoint+gSweepDir;
  int isFinalPoint;
  if (currPoint==gSweepEnd)
    isFinalPoint=1;
  else
    isFinalPoint=0;

  float x=gGraphPix[currPoint][0];
  float y1=gGraphPix[currPoint][1];
  float y2=gGraphPix[currPoint][2];
  float lastX;
  float lastY1;
  float lastY2;
  if (gDoHist==1)   //For histogram draw vertical line
  {
    lastX=x;
    lastY1=gOriginY;
    lastY2=gOriginY;    //ver114-4m eliminated underdraw
  }
  else
  {
    //For normal trace, draw from last point
    if (isFirstPoint==0)
    {
      lastX=gGraphPix[prevPoint][0];
      lastY1=gGraphPix[prevPoint][1];
      lastY2=gGraphPix[prevPoint][2];
    }
  }
  QString cmd="";
  QString thisCmd="";
  //The "line" command does not draw the final endpoint of the line, which normally
  //gets drawn as the start point of the next line. But for the last point, or for
  //any point at which a halt occurs, that final pixel does not get drawn.
  //ver114-6d modified to accumulate all draw commands
  if (gDoY1)
  {
    QString j;
    if (gGraphY2)
    {
      j=";";
    }
    else
    {
      j="";
    }
    //ver114-4n We normally draw a line, but in special cases we use "set" to set a point
    //We do so for the very first point of non-histogram, and for any histogram point
    //consisting of a single point, which "line" will not draw.
    //ver114-6d modified to accumulate all draw commands when gIsDynamic=1
    if ((isFirstPoint && (gDoHist==0)) || ((gDoHist && (int)y1==gOriginY)))
    {
      //thisCmd="set "+x+" "+y1;   //just set the point
    }
    else
    {
      QPen pen(gridappearance->gTrace1Color);
      pen.setWidth(gTrace1Width);
      gErase1[currPoint] = graphScene->addLine(lastX, lastY1, x, y1, pen);
      //thisCmd="line "+lastX+ " "+lastY1+" "+x+" "+y1=";line "+x+" "+y1=" "+lastX+ " "+lastY1;
    }
    if (gIsDynamic==1)
    {
      gTrace1[currPoint]=QPointF(x,y1);
    }
    if (gGraphY1 && gTrace1Width!=0)
    {
      cmd=gDraw1+thisCmd+j;
    }
  }
  if (gDoY2==1)
  {
    if ((isFirstPoint && (gDoHist==0)) || ((gDoHist && (int)y2==gOriginY)))
    {
      //thisCmd="set "+x+" "+y2;   //just set the point
    }
    else
    {
      QPen pen(gridappearance->gTrace2Color);
      pen.setWidth(gTrace2Width);
      gErase2[currPoint] = graphScene->addLine(lastX, lastY2, x, y2, pen);

      //thisCmd="line ";lastX; " ";lastY2;" ";x;" ";y2;";line ";x;" ";y2;" ";lastX; " ";lastY2
    }

    if (gIsDynamic==1)
    {
      gTrace2[currPoint]=QPointF(x,y2);
    }
    if (gGraphY2 && gTrace2Width!=0)
    {
      // cmd$=cmd$;gDraw2$;thisCmd$
    }
  }
  if (cmd!="")
  {
    //and gGraphHandle$!="" then #gGraphHandle$, cmd$   //Draw lines
  }
  gPrevPointNum=currPoint;
}

void msagraph::gDrawNextPointPix(float x, float y1, float y2)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'draw lines from last pix to these pix for y1, y2
    'All parameters are specified as pixel coordinates, which the caller can derive
    'from gConvertXToPix, gConvertY1ToPix, and gConvertY2ToPix
            'We round pixels to the nearest tenth just to keep the accumulated draw commands shorter
    y1=int(10*y1+0.5)/10 : y2=int(10*y2+0.5)/10
    prevPoint=gPrevPointNum  'ver114-5e
    if prevPoint=0 then prevPoint=gSweepStart-gSweepDir 'ver114-5e
    currPoint=prevPoint+gSweepDir   'Increment to next point, which is us
    if gSweepDir=1 then
        if currPoint>gSweepEnd then exit sub  'ver114-5e
    else
        if currPoint<gSweepEnd then exit sub  'ver114-5e
    end if
    gGraphPix(currPoint,0)=x : gGraphPix(currPoint,1)=y1 : gGraphPix(currPoint,2)=y2
    call gDrawNextPoint     'also increments gPrevPointNum
end sub*/
}

void msagraph::gEraseNextPoint()
{
  //erase line for the next segment due for erasure
  //Note gPrevPointNum is negative when we erase the first gEraseLead points when
  //starting a scan.
  if (gDoErase1==0 && gDoErase2==0)
    return;
  //Note gEraseLead is negative for reverse sweep
  int prevPoint=gPrevPointNum;
  if (prevPoint==0)
    prevPoint=gSweepStart-gSweepDir; //The point before the sweep start
  int prevErasePoint=prevPoint+gEraseLead; //Erase is ahead of draw by gEraseLead points
  int thisErasePoint=prevErasePoint+gSweepDir;

  if (gSweepDir==1)
  {
    if (thisErasePoint>gSweepEnd || thisErasePoint<gSweepStart)
      return;
  }
  else
  {
    if (thisErasePoint<gSweepEnd || thisErasePoint>gSweepStart)
      return;
  }

  //Erase a segment on each trace

  if (gDoErase1 && gTrace1Width!=0)
  {
    QGraphicsItem *temp = gErase1[thisErasePoint];
    if (temp != 0)
    {
      graphScene->removeItem(temp);
      delete temp;
    }
    gErase1[thisErasePoint] = 0;
  }

  if (gDoErase2 && gTrace2Width!=0)
  {
    QGraphicsItem *temp = gErase2[thisErasePoint];
    if (temp != 0)
    {
      graphScene->removeItem(temp);
      delete temp;
    }
    gErase2[thisErasePoint] = 0;
  }
}
void msagraph::gInitDraw()
{
  //initialize width and color commands for drawing new
  //Create initial specs for drawing and erasing traces.
  gDraw1="size "+QString::number(gTrace1Width)+";color "+gridappearance->gTrace1Color.name() + ";";
  gDraw2="size "+QString::number(gTrace2Width)+";color "+gridappearance->gTrace2Color.name() + ";";
}

void msagraph::gInitErase()
{
  //Initialize Erase.
  //Erases from point gPrevPointNum to point gPrevPointNum+gEraseLead
  //When a scan is restarted, gPrevPointNum is 0. If the scan is resumed in the middle,
  //gPrevPointNum is the last point that was drawn.
  if (gDoErase1==0 && gDoErase2==0)
    return;
  int saveNum=gPrevPointNum;
  if (gPrevPointNum==0)
    gPrevPointNum=gSweepStart-gSweepDir; //The point before the starting point ver114-5e

  int extra = 0;
  int nErase;
  if (gPrevPointNum==gSweepStart-gSweepDir)
    extra=1;
  else
    extra=0;
  if (gEraseLead>0)
  {
    nErase=extra+gEraseLead;
  }
  else    //reverse sweep
  {
    nErase=extra-gEraseLead; //gEraseLead is negative
    if (gPrevPointNum==0)
      gPrevPointNum=gNumPoints+1;  //zero signals start at beginning
  }
  gPrevPointNum=gPrevPointNum-gEraseLead;   //gEraseNextPoint will use this
  for (int i=1; i <= nErase; i++)
  {
    //Note gEraseNextPoint does nothing if the point number gets out of bounds
    gEraseNextPoint();    //Erase gPrevPointNum+gEraseLead to gPrevPointNum+gEraseLead+1
    gPrevPointNum=gPrevPointNum+gSweepDir;
  }
  gPrevPointNum=saveNum;   //Restore

}
void msagraph::gClearMarkers()
{
  //Clear all markers
  //point number of -1 indicates that marker is absent
  for (int i=0; i < 20; i++)
  {
    gMarkerPoints[i][0] = -1;
    gMarkerPoints[i][1] = 0 - util.uTenPower(9);
  }
}

int msagraph::gMaxMarkers()
{
  //Return max number of markers
  return gNumMarkers;
}

int msagraph::gValidMarkerCount()
{
  //Return number of markers with non-negative point numbers
  int count=0;
  for (int i=1; i <= gNumMarkers; i++)
  {
      if (gMarkerPoints[i][0] >= 0)
          count=count+1;
  }
  return count;
}

void msagraph::gDetermineMarkerPointNumbers()
{
  //Reassign point numbers to maintain current frequencies
  //If the sweep center or width changes when there are markers in place, they
  //need to be relocated to new points to keep their frequency, as much as possible.
  //The best place to do this is after the new scaling parameters have been established
  //on a Restart.
  int pMin;
  int pMax;
  gGetMinMaxPointNum(pMin, pMax);
  for (int i=1; i <= 20; i++)
  {
    int pNum=gMarkerPoints[i][0];
    if (pNum>0)  //proceed if marker exists
    {
      //If scan has zero width, put all markers in the center
      if (gXAxisMin==gXAxisMax)
      {
        pNum=(pMin+pMax)/2;  //zero span; put all markers at center
      }
      else
      {
        pNum=gPointNumOfX(gMarkerPoints[i][1]);  //Point number, possibly non-integral
        if (pNum>pMax)
          pNum=pMax;
        else if (pNum<pMin)
          pNum=pMin;
      }
      gMarkerPoints[i][0]=pNum; //Enter new point number
    }
  }
}

void msagraph::gUpdateMarkerXVal()
{
  //Record current marker X values (freq) in gMarkerPoints(N,1)
  for (int i=1; i <= 20; i++)
  {
    int pNum=gMarkerPoints[i][0];
    if (pNum>0) //If marker exists, update it
    {
       gMarkerPoints[i][1]=gGetPointXVal(pNum);    //Copy current graph X value for this point to gMarkerPoints
    }
  }
}

void msagraph::gRecalcPix(int calcXPix)
{
  //Recalc pixel coords; put into gGraphPix()
  //Because of resizing or other changes, we need to recalculate the pixel coords from
  //the x, y1 and y2 values in gGraphVal(). If the x pixels are recalculated with
  //gGenerateXValues, then calcXPix=0 tells us not to recalculate them here, but rather
  //use gGraphPix(N,0).
  int pMin, pMax;
  gGetMinMaxPointNum(pMin, pMax);    //ver114-6d
  for (int i=1; i <= gDynamicSteps+1; i++) //ver114-6d
  {
    float x;
    if (calcXPix) {x=vars->gGraphVal[i][0]; gConvertXToPix(x); gGraphPix[i][0]=x;}
    if (i>=pMin && i<=pMax)      //Only do for actual graph points ver114-6d
    {
      float y1=vars->gGraphVal[i][1]; float y2=vars->gGraphVal[i][2];
      if (gDoY1) gConvertY1ToPix(y1); else y1=0; //ver116-4k
      if (gDoY2) gConvertY2ToPix(y2); else y2=0; //ver116-4k
      gGraphPix[i][1]=y1; gGraphPix[i][2]=y2;
    }
  }
}

void msagraph::gDrawMarkerInfo()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;

  QGraphicsItem *item;
  QList<QGraphicsItem *> allItems = graphScene->items();

  QListIterator<QGraphicsItem *> i(allItems);
  while(i.hasNext())
  {
    item = i.next();
    QString data = item->data(0).toString();
    if (data == "MarkerText")
    {
      graphScene->removeItem(item);
      delete item;
    }
  }

//sub gDrawMarkerInfo //Draw marker info in specified area
  //(gMarkerInfoLeft, gMarkerInfoTop) is the upper left for the actual marker info, which will be printed
  //one line per marker. The heading for the marker info will be printed at the top.
  //The total display area is 280 pixels wide. markerX will be updated to the rightmost
  //pixel that is drawn (which will be the bounding box of the marker info.)
  //Markers whose point number in gMarkerPoints is <=0 will be skipped
  //The marker info will be in columns, labeled Mark, Freq, Mag and Phase
  //They will all be right aligned at specific points. If printed in a monotype font, the
  //decimals, where applicable, will also align. We print each column separately, rather than
  //using spaces for alignment, because some monotype fonts do not have spaces of the same
  //width as their characters.
  int nMarkers = gValidMarkerCount();  //Don't count those with negative value for point num
        //ver115-1b made various changes to be sure marker area is cleared no matter what.
        //Set drawing color
  //#gGraphHandle$, "font Tahoma 8 bold";       //Set drawing font
  //#gGraphHandle$, "backcolor ";gBackColor$
  ///#gGraphHandle$, "size 1"
  int headWidth=120;
  int headHeight=14;
  if (gDoY1)
    headWidth=headWidth+55;
  if (gDoY2)
    headWidth=headWidth+60;
  markerX=gMarkerInfoLeft;
  markerY=gMarkerInfoTop;
  int a=gWindowHeight;
  int b=gMarkerInfoTop;
  int markPerCol=int((gWindowHeight-5-gMarkerInfoTop-headHeight)/13);
  int maxBoxWidth=2*headWidth+14;  //widest possible box area
  int boxWidth;
  if (nMarkers>markPerCol)
  {
    boxWidth=maxBoxWidth;
  }
  else
  {
    boxWidth=headWidth+4;
  }
  int boxHt= 4 + headHeight + (markPerCol)*13;
      //First clear entire marker area
  //#gGraphHandle$, "color ";gBackColor$
  //#gGraphHandle$, "place ";markerX;" "; markerY //locate pen at upper left
  //#gGraphHandle$, "boxfilled "; maxBoxWidth+boxWidth; " "; markerY+boxHt   //This is lower right corner of max area
  //#gGraphHandle$, "color ";gGridTextColor$
  if (nMarkers<1)
  {
    return;
    //then exit sub   //None to display
  }
  int firstEnd;
  gPrivateDrawMarkerInfo(1, markPerCol, markerX, markerY, firstEnd);
  if (nMarkers>markPerCol)
    gPrivateDrawMarkerInfo(firstEnd+1, markPerCol, markerX+headWidth+10, markerY, firstEnd);
  gMarkerInfoRight=markerX+boxWidth;    //Rightmost pixel drawn, so area to right is still available

}
void msagraph::gPrivateDrawMarkerInfo(int startNum, int maxLines, int markerX, int markerY, int &endNum)
{
  QGraphicsTextItem *item;
//sub gPrivateDrawMarkerInfo startNum, maxLines, markerX, markerY, byref endNum
    //This is an internal helper routine to draw up to maxLines lines of markers
    //in an area whose upper left is (markerX, markerY), except that the heading
    //goes above that area. We start with entry number startNum. We put the final
    //one we examined into endNum, so further processing can start at endNum+1.
    //The drawn area extends from markerX to markerX+240. We assume the color and
    //text have been set.
    //We also update the marker list to reflect current frequencies.

    int markX=markerX+40;
    int freqX=markerX+120;
    int primX;
    int secX;
    int magX = 0;
    if (gPrimaryAxis==1)    //Print primary axis first ver115-3b
    {
        if (gDoY1)
          primX=markerX+180;
        else
          primX=freqX;

        if (gDoY2)
          secX=primX+55;
        else
          secX=magX;  //column positions
    }
    else
    {
        if (gDoY2)
          primX=markerX+180;
        else
          primX=freqX;
        if (gDoY1)
          secX=primX+55;
        else
          secX=magX;  //column positions
    }
    int drawCount=0;
    int headY=markerY+12;    //ver115-1b reduced by 2
    markerY=headY+13;
    QString markY1;
    QString markY2;
    for (int i=startNum; i < gNumMarkers; i++)
    {
        endNum=i;
        int markPointNum=gMarkerPoints[i][0];
        if (markPointNum>0)
        {
            QString markID =gMarkers[i][0];

            //markF=gGetPointXVal(markPointNum);
            QString markF=util.usingF("####.######",gGetPointXVal(markPointNum));
               //ver114-6f use axis label formats to format the marker values
            item = gPrintTextRightJust(markID, markX-5, markerY, QColor(Qt::white));
            item->setData(0,"MarkerText");
            item = gPrintTextRightJust(markF, freqX, markerY, QColor(Qt::white));
            item->setData(0,"MarkerText");
            if (gDoY1==0)
              markY1="";
            else
              markY1=util.uCompact(util.uFormatted(gGetPointYVal(markPointNum,1), gY1AxisForm)); //Y1
            if (gDoY2==0)
              markY2="";
            else
              markY2=util.uCompact(util.uFormatted(gGetPointYVal(markPointNum,2), gY2AxisForm)); //Y2
            if (gPrimaryAxis==1)
            {
                if (gDoY1==1)
                {
                  item = gPrintTextRightJust(markY1, primX, markerY, QColor(Qt::white));  //Print primary axis first ver115-3b
                  item->setData(0,"MarkerText");
                }
                if (gDoY2)
                {
                  item = gPrintTextRightJust(markY2, secX, markerY, QColor(Qt::white));
                  item->setData(0,"MarkerText");
                }
            }
            else
            {
                if (gDoY2)
                {
                  item = gPrintTextRightJust(markY2, primX, markerY, QColor(Qt::white));
                  item->setData(0,"MarkerText");
                }
                if (gDoY1==1)
                {
                  item = gPrintTextRightJust(markY1, secX, markerY, QColor(Qt::white));
                  item->setData(0,"MarkerText");
                }
            }
            markerY=markerY+13;  //To next line
            drawCount=drawCount+1;
            if (drawCount==maxLines)
              break;
        }
    }  //To next marker
        //Draw heading last, because we don't need it if we
        //didn't draw any markers
    if (drawCount==0)
    {
      return;
      //then exit sub
    }
    item = gPrintTextRightJust("Mark", markX, headY, QColor(Qt::white));
    item->setData(0,"MarkerText");
    item = gPrintTextRightJust("Freq (MHz)", freqX-5, headY, QColor(Qt::white));
    item->setData(0,"MarkerText");
    if (gPrimaryAxis==1)
    {
        if (gDoY1)
        {
          item = gPrintTextRightJust(gY1DataLabel, primX-2, headY, QColor(Qt::white));
          item->setData(0,"MarkerText");
        }
        if (gDoY2)
        {
          item = gPrintTextRightJust(gY2DataLabel, secX-2, headY, QColor(Qt::white));
          item->setData(0,"MarkerText");
        }
    }
    else
    {
        if (gDoY2)
        {
          item = gPrintTextRightJust(gY2DataLabel, primX-2, headY, QColor(Qt::white));
          item->setData(0,"MarkerText");
        }
        if (gDoY1)
        {
          item = gPrintTextRightJust(gY1DataLabel, secX-2, headY, QColor(Qt::white));
          item->setData(0,"MarkerText");
        }
    }

}
void msagraph::RefreshGraph(int restoreErase)
{

  //Quick redraw of graph area
  //This uses saved information from the most recent gDrawGrid (called in RedrawGraph)
  //to quickly redraw the graph area and about 20 pixels around it to effectively erase
  //the traces and markers, and also uses such saved info to quickly redraw the traces.
  //We then redraw the markers at their current locations. The marker info box is also
  //updated; the first step in that update is to draw a filled box with a border, which
  //erases everything underneath. We don't flush, so if the window gets covered the markers
  //and traces will disappear. We discard to clear some memory.
  //This may be called as a result of some user action in the middle of scanning. To avoid
  //interference with the dynamic scan process, we Pause dynamic scanning and then resume it.

  //In a language faster than Liberty Basic we could just use RedrawGraph to redraw from scratch.

  //The following globals determine whether we redraw various components from scratch or by a faster method.
  //global refreshGridDirty    //Forces grid (and labels, title) and setup info and references to redraw from scratch in RefreshGraphs
  //global refreshTracesDirty    //Forces traces and references to be redrawn from raw Y1 and Y2 values in RefreshGraph
  //global refreshMarkersDirty     //Forces marker relocation based on frequency
  //global refreshAutoScale        //Forces autoscaling of axes; implies refreshRedrawFromScratch
  //global refreshRedrawFromScratch  //Forces complete redraw from scratch in RefreshGraph

  /*  call gMouseQueryClear   //Should already be clear, but right-clicking during MouseOver can confuse
*/
  if(refreshAutoScale)
  {
    PerformAutoScale();   //Recalculates scaling in graph module
    refreshRedrawFromScratch=1;
  }

  int recreateReferences=0;
  if (refreshRedrawFromScratch || refreshGridDirty)
  {
    gDrawGrid();
    DrawSetupInfo();
    recreateReferences=1;
  }
  else
  {
    //gRefreshGrid();
  }

  if (referenceLineType!=0)
  {
    if (recreateReferences || refreshTracesDirty==1)
    {
      if (referenceLineType>1)
      {
        CreateReferenceSource();
      }
      CreateReferenceTransform();
      gClearAllReferences();
      if (referenceDoMath==0)   //don't draw if we are using ref for math
      {
        if (referenceTrace & 2)
        {
          gAddReference(1,CreateReferenceTraces(referenceColor2,referenceWidth2,2, &refLine[2]));  //Do Y2 reference
        }
        if (referenceTrace & 1)
        {
          gAddReference(2,CreateReferenceTraces(referenceColor1,referenceWidth1,1, &refLine[1]));
        }
      }
    }
    PrintReferenceHeading();
    if (referenceDoMath==0)
    {
      gDrawReferences();  //don't draw if we are using ref for math
    }
  }

  gPauseDynamicScan();  //Keeps the trace redraws from accumulating
  if (refreshTracesDirty==1 || refreshRedrawFromScratch)
  {
    gRecreateTraces(1);   //Recreate and draw trace draw commands
  }
  else
  {
    gRefreshTraces(); //Draw from accumulated trace draw commands
  }
  if (restoreErase)
  {
    gRestoreErasure();
  }
  QCoreApplication::processEvents();
  gResumeDynamicScan();
  //Discard draw commands if scan is still in progress; otherwise we flush a bit later
  if (haltsweep==1)
  {
   //   then #graphBox$, "discard"
  }
    //In case frequency axis has changed, we want to update the marker point numbers
    //to maintain their prior frequencies, if possible. We do this on every Refresh or Redraw
    //through the end of the first scan.
  if (refreshMarkersDirty || refreshRedrawFromScratch)
  {
    gDetermineMarkerPointNumbers();   //locate floating markers
  }
  gUpdateMarkerXVal();  //Save frequency values for markers
  mDrawMarkerInfo(); //Draw marker info under the x axis; also updates marker locations
  if (doGraphMarkers)
  {
    gDrawMarkers();
  }
  if (vars->multiscanIsOpen)
  {
    //multiscanMarkCurrentWindow(); //mark current graph //ver115-9a
  }
    //Discard draw commands if scan is still in progress; otherwise flush
  if (haltsweep==1)
  {
   // then #graphBox$, "discard" else #graphBox$, "flush"
  }

  if (vars->smithWindowVisible)
  {
    int doSmithRef;
    // fix me smithSetGraphMarkers(doGraphMarkers);    //Tell smith whether to draw markers on graph ver115-2c
    if (referenceLineType!=0 && (referenceTrace & 4))
    {
      // fix me what does this code actually
      //doSmithRef=(referenceDoMath=0);
      doSmithRef= 0;
      referenceDoMath=0;
    }
    else
      doSmithRef=0;   //draw non-fixed value reference ver115-7a

    smithRefreshMain(doSmithRef); //Draw chart and graph, with possible reference line
  }
  refreshForceRefresh=0; //Clear refresh flags, since we just redrew
  refreshGridDirty=0;
  refreshTracesDirty=0;
  refreshMarkersDirty=0;
  refreshAutoScale=0;
  refreshRedrawFromScratch=0;
}

void msagraph::gRecreateTraces(bool doDraw)
{
  //Recreate gTrace1) and gTrace2 from raw Y values; do not draw unless doDraw=1 ver114-7e
  if (gNumPoints==0)
  {
    return;
  }
  int pMin, pMax;
  gGetMinMaxPointNum(pMin, pMax);
  //saveGraph$=gGraphHandle$
  int saveDyn=gIsDynamic;
  int saveFirst=gIsFirstDynamicScan;
  int savePrev=gPrevPointNum;
  gIsDynamic=1;    //Signal to accumulate drawing commands
  gIsFirstDynamicScan=1;
  gPrevPointNum=0;
  gInitDraw();
  if (doDraw==0)
  {
    qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
    //no code seems to call with doDraw = 1

    // gGraphHandle$=""    //Suppresses actual drawing ver114-7e
  }
  if (gSweepDir==1)
  {
    for (int i=pMin; i<= pMax; i++)
    {
      //create segment from prior point to this point, for both y1 and y2 as appropriate
      gDrawNextPointValue(vars->gGraphVal[i][1], vars->gGraphVal[i][2]);   //create command for this draw
    }
  }
  else
  {
    for (int i=pMax; i!= pMin; i--)
    {
      //create segment from prior point to this point, for both y1 and y2 as appropriate
      gDrawNextPointValue(vars->gGraphVal[i][1], vars->gGraphVal[i][2]);   //create command for this draw
    }
  }
  //gGraphHandle$=saveGraph$
  gIsDynamic=saveDyn;
  gIsFirstDynamicScan=saveFirst;
  gPrevPointNum=savePrev;

}
void msagraph::PerformAutoScale()
{
  int newMin, newMax;
  //do autoscaling of axes as required, but no redraw
  //If autoscale is on  then calculate the scale and redraw from raw values
  if (autoScaleY1)
  {
    CalcAutoScale(1, newMin, newMax);
    SetY1Range(newMin, newMax);
  }
  if (autoScaleY2)
  {
    CalcAutoScale(2, newMin, newMax);
    SetY2Range(newMin, newMax);
  }
  gCalcGraphParams();   //Calculate new scaling. May change min or max.
  float xMin, xMax;
  gGetXAxisRange(xMin, xMax);
  if (vars->startfreq!=xMin || vars->endfreq!=xMax)
    SetStartStopFreq(xMin, xMax);
}
void msagraph::DrawSetupInfo()
{
  //Draw info about MSA setup at right side of graph box
  int InfoY;
  int InfoX;
  gGetGraphicsSize(InfoX, InfoY);
  InfoX = InfoX - 100;
  InfoY = graphMarTop+2;
  QColor textColor;
  QColor backColor;
  gridappearance->gGetInfoColors(textColor, backColor);
  //#graphBox$, "font Tahoma 8 bold;color ";textColor$;";backcolor ";backColor$
  QFont Tahoma("Tahoma", 8, QFont::Bold);
  graphScene->setFont(Tahoma);

  if (vars->msaMode==modeSA) //ver115-1b rewrote this if... block
  {
    //SA mode, either with or without TG
    if (vars->gentrk==0)
    {
      gPrintText("Spect. Analyzer", InfoX-10,0,textColor);
    }
    else
    {
      gPrintText("Spect. Analyzer with TG", InfoX-40,0,textColor);
    }
  }
  else
  {
    if (vars->msaMode==modeScalarTrans)
    {
      gPrintText("SNA Transmission", InfoX-10,0,textColor);
    }
    if (vars->msaMode==modeVectorTrans)
    {
      gPrintText("VNA Transmission", InfoX-10,0,textColor);
    }
        //ver115-1c revised the printing of the cal level
    if (vars->msaMode==modeReflection)
      gPrintText("VNA Reflection", InfoX-10,0,textColor);
    QColor col;
    if (vnaCal->applyCalLevel < vnaCal->desiredCalLevel)
    {
      col = Qt::red;   //if downgraded cal, print in red
    }
    if (vnaCal->applyCalLevel==0)
    {
        if (vars->msaMode==modeReflection)
        {
          col = Qt::red;  //no reflection cal, print in red
        }
        vnaCal->calLevel="None";
    }
    else
    {
        if (vnaCal->applyCalLevel==1)
        {
          vnaCal->calLevel="Base";
        }
        else
        {
          vnaCal->calLevel="Band";
        }
    }
    gPrintText("Cal=" + vnaCal->calLevel,InfoX, InfoY, col);
    InfoY=InfoY+16;
    //#graphBox$, "color ";textColor$ //restore
  }

  if (vars->freqBand!=0)
  {
    gPrintText(QString::number(vars->freqBand) + "G", InfoX, 16,textColor);
  }
  if (vars->suppressHardware)
  {
    gPrintText( "No MSA", InfoX, 44,textColor);
  }

            //Base frequency, if not zero
  if (vars->baseFrequency!=0)
  {
    gPrintText("Base Freq(MHz)=", InfoX, InfoY,textColor);
    InfoY=InfoY+16;
    gPrintText(QString::number(vars->baseFrequency,'g',3), InfoX+5, InfoY,textColor);
    InfoY=InfoY+16;
  }

  gPrintText( "RBW=" + QString::number(activeConfig->finalbw,'g',6)+"kHz",InfoX, InfoY,textColor);
  InfoY=InfoY+16;
  gPrintText( "Vid="+vars->videoFilter,InfoX, InfoY,textColor);
  InfoY=InfoY+16;

  QString wait;
  if (vars->useAutoWait) wait=vars->autoWaitPrecision; else wait=QString::number(vars->wate)+" ms";
  gPrintText("Wait="+wait, InfoX, InfoY,textColor); InfoY=InfoY+16;
  //Print number of steps
  gPrintText("Steps="+QString::number(vars->globalSteps), InfoX, InfoY,textColor); InfoY=InfoY+16;

  if (gGetXIsLinear())
  {
    //Print stepfreq as Hz, KHz, etc. with up to 3 whole places, max 4 decimals
    //and max 5 total significant digits. We don't do this for log sweeps because
    //the freq/step is not constant
    float stepfreq=(vars->endfreq - vars->startfreq)/vars->globalSteps;    //stepfreq is only calculated for printing
    QString stepSize = util.uFormatted(1000000*stepfreq,"3,4,5//UseMultiplier//suffix=Hz");
    gPrintText(util.uCompact(stepSize)+"/Step", InfoX, InfoY,textColor); InfoY=InfoY+16;
        //Print Freq/div. also done only for linear sweep
    int nHorDiv, nVertDiv;
    gGetNumDivisions(nHorDiv, nVertDiv);
    stepSize=util.uFormatted(1000000*vars->sweepwidth/nHorDiv,"3,4,5//UseMultiplier//suffix=Hz");
    gPrintText(util.uCompact(stepSize)+"/Div", InfoX, InfoY,textColor); InfoY=InfoY+16;
  }
  if (vars->msaMode==modeSA && activeConfig->TGtop>0)
  {
    if (vars->gentrk==0)
    {
        QString freq=QString::number(vars->sgout);
        if (freq.length()<7)
        {
            gPrintText("SG="+freq, InfoX, InfoY,textColor); InfoY=InfoY+16;
        }
        else
        {
          gPrintText("SG=", InfoX, InfoY,textColor); InfoY=InfoY+14;
          gPrintText("  "+freq, InfoX, InfoY,textColor); InfoY=InfoY+16;
        }
    }
    else
    {
      QString nr;
      if (vars->normrev==0) nr="TG=Normal"; else nr="TG=Reverse";
      gPrintText(nr, InfoX, InfoY,textColor); InfoY=InfoY+16;
      QString freq = QString::number(vars->offset);
      if (freq.length()<2)
      {
        gPrintText("Offset="+freq, InfoX, InfoY,textColor); InfoY=InfoY+16;
      }
      else
      {
        gPrintText("Offset=", InfoX, InfoY,textColor); InfoY=InfoY+14;
        gPrintText("  "+freq, InfoX, InfoY,textColor); InfoY=InfoY+16;
      }
    }
  }
  if (vars->msaMode!=modeSA && vars->msaMode!=modeScalarTrans)
  {
    gPrintText("Exten="+QString::number(vars->planeadj)+" ns", InfoX, InfoY,textColor);
    InfoY=InfoY+16;
  }
  if (vars->msaMode==modeReflection)
  {
    gPrintText("Z0="+util.uFormatted(vars->S11GraphR0, "3,4,5//UseMultiplier//DoCompact//SuppressMilli"), InfoX, InfoY,textColor);
    InfoY=InfoY+16;
  }
  if (vars->msaMode!=modeSA)
  {
  //  #graphBox$, "font Tahoma 8 bold;color ";textColor$;";backcolor ";backColor$
    QString directText;
    if (vars->switchFR==0) directText=">DUT"; else directText="DUT<";
    gPrintText(directText, InfoX, InfoY,textColor); InfoY=InfoY+16;
  }

  QString s="";
  if (referenceLineType!=0 && referenceDoMath!=0)  //If doing math with reference line, so indicate ver116-1b
  {
    if (referenceOpA==1 && referenceOpB==1)
      s="Data+Ref";
    if (referenceOpA==1 && referenceOpB==-1)
      s="Ref-Data";
    if (referenceOpA==-1 && referenceOpB==1)
      s="Data-Ref";
    gPrintText(s, InfoX, InfoY,textColor);
    InfoY=InfoY+16;
  }
}
void msagraph::PrintMessage(QString message)
{
  //Print message above top of marker info area; Limited to 75 characters; don't print if blank
  //The message is in message$
  QList<QGraphicsItem *> allItems = graphScene->items();
  QGraphicsItem *item;
  QListIterator<QGraphicsItem *> i(allItems);
  while(i.hasNext())
  {
    item = i.next();
    QString data = item->data(0).toString();
    if (data == "message")
    {
      graphScene->removeItem(item);
      delete item;
    }
  }

  item = gPrintMessage(message);
  item->setData(0,"message");
}
QString msagraph::gSweepContext()
{
  //Return string with context info on trace graphing
  //successive lines are separated by chr(13)
  QString newLine="\r";
  QString aSpace=" ";
  //QString sep=";;";   //delimits text items on one line

  QString s1= "MinMaxXAxis="+QString::number(gXAxisMin)+aSpace+QString::number(gXAxisMax);         //X-axis range
  s1= s1+ newLine+ "MinMaxY1Axis="+QString::number(gY1AxisMin)+aSpace+QString::number(gY1AxisMax); //Y1 axis range
  s1= s1+ newLine+ "MinMaxY2Axis="+QString::number(gY2AxisMin)+aSpace+QString::number(gY2AxisMax); //Y2 axis range
  s1= s1+ newLine+ "IsLinear="+QString::number(gXIsLinear)+aSpace+QString::number(gY1IsLinear)+aSpace+QString::number(gY2IsLinear);    //Is Linear
  s1= s1+ newLine+ "NumSteps="+ QString::number(gDynamicSteps);    //Number of steps
  s1= s1+ newLine+ "SweepDir="+ QString::number(gSweepDir);    //Sweep direction
  //Note gMode is handled by saving msaMode ver114-6f
  return s1;
}
void msagraph::gGetMarkerByNum(int markNum, int &pointNum, QString &ID, QString &trace, QString &style)
{
  //markNum is the ordinal
  //Get info for marker at entry number entryNum (1...)
  //Return negative pointNum if none found
  if (markNum<0 || markNum>gNumMarkers)
  {
    pointNum=-1;
    ID="";
    return;
  }
  pointNum=gMarkerPoints[markNum][0];
  ID=gMarkers[markNum][0];
  trace=gMarkers[markNum][1];
  style=gMarkers[markNum][2];
}

void msagraph::gUsePresetText(QString btn)
{
  //btn specifies the preset to be used. It may be in the form of a
  //handle, so we drop everything through the period, if there is one.
  int pos=btn.indexOf(".");
  if (pos>-1) btn=btn.mid(pos+1);
  btn=btn.trimmed().toUpper();
  gridappearance->gGraphTextPreset=btn;  //Save for later reference
  if (btn == "BASICTEXT")
  {
    gridappearance->gXAxisFont="Arial bold 9";
    gridappearance->gY1AxisFont="Arial bold 9";
    gridappearance->gY2AxisFont="Arial bold 9";
    gridappearance->gGridFont="Arial bold 9";
  }
}
void msagraph::PrintReferenceHeading()
{
  //Print above axis to indicate which line matches which axis
  QString v1;
  QString v2;
  if (referenceLineType!=0)
  {
    if (referenceDoMath==0)
    {
       vars->refHeadingColor1=referenceColor1;
       vars->refHeadingColor2=referenceColor2; //Use ref color if ref line is drawn
    }
    else
    {
       gridappearance->gGetTraceColors(vars->refHeadingColor1, vars->refHeadingColor2); //Use trace colors for "REF" if math is used
    }
    if (referenceLineType==3) //For fixed value ref, print the value
    {
       int semiPos=referenceLineSpec.indexOf(";");
       v1= "=" + referenceLineSpec.left(semiPos-1);    //Up to semicolon is first value
       v2= "=" + referenceLineSpec.mid(semiPos+1);     //After semicolon is second value
    }
    else
    {
       v1="";
       v2="";
    }
    if (referenceTrace & 2)
    {
        gPrintAxisAnnotation(2, "Ref"+v2, "Tahoma 8 bold", vars->refHeadingColor2);
    }

    if (referenceTrace & 1)
    {
      gPrintAxisAnnotation(1, "Ref"+v1, "Tahoma 8 bold", vars->refHeadingColor1);
    }
  }
}
void msagraph::RecalcYValues()
{
  //Recalculate Y values for existing graph points, and change in the graph module
  int pMin, pMax;
  gGetMinMaxPointNum(pMin, pMax);
  for (int i=pMin; i <= pMax; i++)
  {
    double y1, y2;
    CalcGraphData(i-1, y1, y2, 0);  //i-1 to get step number from point number ver114-6h
    if (referenceDoMath==2) //ref math is to be done on graph values ver115-5d
    {
        if (referenceTrace & 1)
                y1=referenceOpA*referenceTransform[vars->thisstep+1][1]+referenceOpB*y1;
        if (referenceTrace & 2)
                y2=referenceOpA*referenceTransform[vars->thisstep+1][2]+referenceOpB*y2;
    }
    gChangePoints(i, y1, y2);    //Change point values in graph module
  }
}
void msagraph::SetY1Range(int bot, int top)
{
  //Set range of Y1 axis
  //Note gCalcGraphParams must be called to fully update graph module
  gSetYAxisRange(1, bot, top);
  vars->Y1Bot=bot;
  vars->Y1Top=top;
}

void msagraph::SetY2Range(int bot, int top)
{
  //Set range of Y2 axis
  //Note gCalcGraphParams must be called to fully update graph module
  gSetYAxisRange(2, bot, top);
  vars->Y2Bot=bot;
  vars->Y2Top=top;
}
void msagraph::CalcAutoScale(int axisNum, int &axisMin, int &axisMax)
{
  //Calculate min and max for autoscaling axis axisNum
  if (axisNum==1)
    componConst=vars->Y1DataType;
  else
    componConst=vars->Y2DataType;
  int pMin, pMax;
  gGetMinMaxPointNum(pMin, pMax);
  if (componConst==constGD)
  {
    //For group delay, the first point is invalid so don't include it
    if (gGetSweepDir()==1) pMin=pMin+1; else pMax=pMax-1;
  }
  //Find min and max values between points pMin and pMax
  int dum1, dum2;
  gFindPeaks(axisNum, pMin, pMax, dum1, dum2, axisMin, axisMax);

  if (componConst == constGraphS11DB || componConst == constMagDBM || componConst ==  constMagDB
      || componConst == constReturnLoss || componConst == constInsertionLoss) //Round db/dbm to multiple of 5 ver114-8d
  {
    axisMin=util.uRoundDownToMultiple(axisMin-0.5, 5);
    axisMax=util.uRoundUpToMultiple(axisMax+0.5, 5);
  }
  else if (componConst == constGraphS11Ang || componConst == constAngle || componConst == constRawAngle
           || componConst == constTheta || componConst == constImpedAng || componConst == constAdmitAng)   //ver115-4a
  {
    //Round angles to multiple of 15 ver115-1i added raw angle
    axisMin=util.uRoundDownToMultiple(axisMin-5, 15); if (axisMin<-180) axisMin=-180; //ver114-8c
    axisMax=util.uRoundUpToMultiple(axisMax+5, 15) ; if (axisMax>180) axisMax=180;
    if (axisMax-axisMin>120) { axisMin=-180 ; axisMax=180; }  //Expand to full range  //ver114-8c
  }
  else if (componConst == constSerR || componConst == constParR || componConst == constImpedMag)    //Round non-negative impedances to multiple of 25
  {
    if (axisMax>1000000) axisMax=1000;    //don't scale to screwy values, such as divide by zero might produce
    axisMin=util.uRoundDownToMultiple(axisMin-5, 25);
    axisMax=util.uRoundUpToMultiple(axisMax+5, 25);
    if (axisMin<0) axisMin=0; //No resistance below 0
    if (axisMin>0)
    {
      if (axisMax/axisMin>=2) axisMin=0; //Min is zero unless range very small ver115-4h
    }
  }
  else if (componConst == constSerReact || componConst == constParReact)    //Round reactances to multiple of 25; allow negative
  {
    if (axisMax>1000000) axisMax=1000;    //don't scale to screwy values, such as divide by zero might produce
    int tentMin=util.uRoundDownToMultiple(axisMin-5, 25);
    if (tentMin<0 && axisMin>0) tentMin=0; //don't make axis<0 if axisMin>0
    axisMin=tentMin;
    int tentMax=util.uRoundUpToMultiple(axisMax+5, 25);
    if (tentMax>0 && axisMax<0) tentMax=0; //don't make axis>0 if axisMax<0
    axisMax=tentMax;
    if (axisMin*axisMax>0)
    {
      //Pin one end to zero unless range very small ver115-4h
      if (axisMax/axisMin>=2 || axisMin/axisMax>=2)  //ver115-4h
      {
        if (axisMax<0) axisMax=0; else axisMin=0;
      }
    }
  }
  else if (componConst == constMagWatts || componConst == constMagRatio || componConst == constMagV || componConst == constRho) //Fractional values that won't exceed 1 or be negative
  {
    if (axisMax>1) axisMax=1;
    axisMin=util.uRoundDownToPower(axisMin, 10);
    axisMax=util.uRoundUpToPower(axisMax,10);
    if (axisMin>0) { if (axisMax/axisMin>=2) axisMin=0;}
  }
  else if (componConst == constSerC || componConst == constSerL || componConst == constParC || componConst == constParL)     //Capacitance and Inductance
  {
    if (axisMin<0) axisMin=axisMin*1.1; else axisMin=axisMin*0.9;
    if (axisMax>0) axisMax=axisMax*1.1; else axisMax=axisMax*0.9;
    axisMin=util.uRoundDownToPower(axisMin, 10);
    axisMax=util.uRoundUpToPower(axisMax, 10);
    if (axisMax>1) axisMax=1 ; if (axisMin<-1) axisMin=-1;
    //If the range does not cross the axis and is more than a factor of 2, pin one end to zero
    if (axisMin*axisMax>0)
    {
      if (axisMax/axisMin>=2 || axisMin/axisMax>=2)   //ver115-4h
      {
        if (axisMax<0) axisMax=0; else axisMin=0;
      }
    }
  }
  else if (componConst == constSWR)   //VSWR >=1 and we don't care about huge values ver114-8d
  {
    axisMin=1;
    if (axisMax<2) axisMax=2; else axisMax=(int)(axisMax)+2;
    if (axisMax>50) axisMax=50;
  }
  else if (componConst == constComponentQ)   //>=0 and we don't care about huge values ver115-2d
  {
    axisMin=0;
    if (axisMax<10) axisMax=10; else axisMax=(int)(axisMax)+5;
    if (axisMax>10000) axisMax=10000;
  }
  else if (componConst == constReflectPower)   //a percentage ver115-2d
  {
    axisMin=0;
    axisMax=100;
  }
  else if (componConst == constAdmitMag)  //ver115-4a
  {
    axisMin=0;
    axisMax=util.uRoundUpToPower(axisMax, 10);
  }
  else if (componConst == constConductance || componConst == constSusceptance)  //ver115-4a
  {
    if (axisMin>0) axisMin=util.uRoundDownToPower(axisMin, 10); else axisMin=util.uRoundUpToPower(axisMin, 10);
    if (axisMax>0) axisMax=util.uRoundUpToPower(axisMax, 10); else axisMax=util.uRoundDownToPower(axisMax, 10);
  }
  else if (componConst == constAux0 || componConst == constAux1 || componConst == constAux2
           || componConst == constAux3 || componConst == constAux4 || componConst == constAux5)   //ver115-4a
  {
    int auxNum=componConst-constAux0;    //e.g. constAux4 produces 4
    axisMin=vars->auxGraphDataInfo[auxNum][1]  ; axisMax=vars->auxGraphDataInfo[auxNum][2];
  }
  else if (componConst == constNoGraph)
  {
    //Do nothing
  }
  else       //constGD and anything we missed
  {
    //These items can have a broad range of values, positive and negative
    if (axisMax>1000000) axisMax=1; //don't scale to screwy values, such as divide by zero might produce
    if (axisMin<0) axisMin=axisMin*1.1; else axisMin=axisMin*0.9;
    if (axisMax>0) axisMax=axisMax*1.1; else axisMax=axisMax*0.9;
    axisMin=util.uRoundDownToPower(axisMin, 10);
    axisMax=util.uRoundUpToPower(axisMax, 10);
  }
}

void msagraph::ConvertRawDataToReflection(int currStep)
{
  //For the current step in reflection mode, calculate S11, referenced to S11GraphR0 ver115-5f mod by ver116-4n
  //Calculate reflection in db, angle format and puts results in ReflectArray, which already contains the raw data.
  //Also calculates the various items in ReflectArray() from the final reflection value.
  //We need to adjust the data for calibration
  //      Reference calibration
  //The simplest reflection calibration is to use the Open or Short as a reference. In that case, we still calculate
  //OSL coefficients as though we did full OSL, using Ideal results for the missing data.
  //    Full OSL
  //More extensive calibration would include the Open, Short and Load, from which we calculated the a, b, c OSL
  //coefficients during calibration. If we have full OSL coefficients, we apply them here.
  //We identify the type of jig used with S11JigType$, which the user sets during calibration.
  //S11JigType$ is always set to "Reflect" when doing full OSL, since we don't even know the nature of the actual jig.
  //In addition, S21JigR0 is set to S11BridgeR0.
  //Note that S21 or S11 are now referenced to the S21JigR0 or S11BridgeR0, not the graph R0. We do the
  //conversion here. But we also save S11 as an intermediate value before applying the R0 coversion or plane extension
  //(but after applying cal) to make recalculations easier. It is saved with constIntermedS11DB and constIntermedS11Ang.

      //First get the raw reflection data. This is the measured data, adjusted by subtracting the reference.
      //planeadj has not been applied; it is applied after applying calibration
      //S21JigShuntDelay has not yet been applied. It will be applied here via the OSL coefficients.

  double trueFreq=vars->ReflectArray[currStep][0]*1000000;
  double db=vars->ReflectArray[currStep][constGraphS11DB];
  double ang=vars->ReflectArray[currStep][constGraphS11Ang];
  if (vars->calInProgress)    //If calibrating we don't adjust anything here, or calculate anything other than S11
  {
    vars->ReflectArray[currStep][constIntermedS11DB]=db;
    vars->ReflectArray[currStep][constIntermedS11Ang]=ang;
    return;
  }

  double rho=util.uTenPower(db/20);    //mag made linear
  //db, rho, and ang (degrees) now have the raw reflection data
  //If necessary, we apply full OSL to the reflection data, whether it was derived
  //from a reflection bridge or a transmission jig.
  //If doing OSL cal, then we don't want to apply whatever coefficients we happen to have now.
  //If doSpecialGraph<>0 we don't want to mess with the internally generated data
  if (vars->doSpecialGraph==0 && vnaCal->applyCalLevel!=0)
  {
    double rads=ang*util.uRadsPerDegree();   //angle in radians
    double mR=rho*cos(rads);
    double mI=rho*sin(rads);     //measured S11, real and imaginary
    double aR=vars->OSLa[currStep][0];
    double aI=vars->OSLa[currStep][1];   //coefficient a, real and imaginary
    double bR=vars->OSLb[currStep][0];
    double bI=vars->OSLb[currStep][1];   //coefficient b, real and imaginary
    double cR=vars->OSLc[currStep][0];
    double cI=vars->OSLc[currStep][1];   //coefficient c, real and imaginary
    double refR, refI;

    //calculate adjusted db, ang via OSL. Note OSL must be referenced to S11BridgeR0
    int calcMethod=1;    //For debugging, we have two different methods
    if (calcMethod==1)
    {
      //The first method uses  the following formula, and corresponds to CalcOSLCoeff
      //       S = (M ? b) / (ac*M)
      //where S is the actual reflection coefficient and M is the measured reflection coefficient.
      //S and M are in rectangular form in this equation.
      double RealCM=(cR*mR-cI*mI);
      double ImagCM=cR*mI+cI*mR;     //c*M, real and imaginary
      ComplexMaths a;
      a.cxDivide(mR-bR, mI-bI, aR-RealCM,aI-ImagCM,refR, refI);   //Divide M-b by a-c*M
    }
    else
    {
      //The second method uses  the following formula, and corresponds to CalcOSLCoeff1
      //       S = (a - cM)/(bM - 1)
      //where S is the actual reflection coefficient and M is the measured reflection coefficient.
      //S and M are in rectangular form in this equation.

      double RealCM=cR*mR-cI*mI;
      double ImagCM=cR*mI+cI*mR;     //c*M, real and imaginary
      double RealBM=bR*mR-bI*mI;
      double ImagBM=bR*mI+bI*mR;     //b*M, real and imaginary
      double numR=aR-RealCM;
      double numI=aI-ImagCM;             //numerator, real and imaginary
      double denR=RealBM-1;
      double denI=ImagBM;                  //denominator, real and imaginary
      ComplexMaths a;
      a.cxDivide(numR, numI, denR, denI, refR, refI);     //Divide numerator by denominator; result is reflection coeff.
    }
    //separated the following common calculations from the above if...else block
    double magSquared=pow(refR,2)+pow(refI,2);        //mag of S, squared
    db=10*util.uSafeLog10(magSquared);    //S mag in db; multiply by 10 not 20 because mag is squared
    if (db>0)
      db=0;   //Shouldn't happen
    ang=util.uATan2(refR, refI);      //angle of S in degrees
    //db, ang (degrees) now have S11 data produced by applying OSL calibration.
  }

  //Save the angle prior to applying plane extension or Z0 transform, to make it easier to recalculate with a new values
  vars->ReflectArray[currStep][constIntermedS11DB]=db;
  vars->ReflectArray[currStep][constIntermedS11Ang]=ang;
      //Note we do apply plane extension even when doSpecialGraph<>0
  if (vars->planeadj!=0 || vars->S11BridgeR0!=vars->S11GraphR0)
    ApplyExtensionAndTransformR0(vars->ReflectArray[currStep][0], db, ang);

      //Note we do not put the reflection data in datatable, which retains the original raw data
  vars->ReflectArray[currStep][constGraphS11DB]=db;   //Save final S11 in db, angle format (in Graph R0, after plane ext)
  while (ang>180)
  {
    ang=ang-360;
  }
  while (ang<=-180)
  {
    ang=ang+360;
  }
  vars->ReflectArray[currStep][constGraphS11Ang]=ang;
  //We now compute the various items in ReflectArray() from S11, but if we are doing calibration we don't need this
  //other data, and it probably doesn//t make sense anyway.
  if (vars->calInProgress==0)
    CalcReflectDerivedData(currStep);  //Calc other ReflectArray() data from S11.
}

void msagraph::ApplyExtensionAndTransformR0(double freq, double &db, double &ang)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
 /*
//ver115-2d created ApplyExtensionAndR0Transform so it can be called from a couple of places
sub ApplyExtensionAndTransformR0 freq, byref db, byref ang   //Apply reflection mode plane extension and transform from bridge R0 to graph R0 for reflection
    //freq is in MHz
    //apply plane extension. We do this after applying calibration.
    //For reflection mode with S21 series jig, plane extension makes no sense, so we don//t do it
    //For Transmission mode, we don//t get here.
    //We don//t do the adjustment when calibrating,
    //because plane extension is used to extend the plane after calibration, and we don//t need to do S11GraphR0
    //ver115-2b modified this procedure

    if calInProgress=1 then exit sub
    if planeadj<>0 then
        //Do the extension, but not if series fixtures is used
        if S11JigType$="Reflect" or S21JigAttach$="Shunt" then call uExtendCalPlane freq, ang, planeadj,1  //1 means reflection mode  ver116-4j
    end if

    //Convert into new R0 if necessary   //ver115-1e moved this here from CalcReflectDerivedData
    //We don//t convert if calibrating
    if S11BridgeR0<>S11GraphR0 then  //ver115-1e
            //Transform to graph reference impedance
        call uS11DBToImpedance S11BridgeR0, db, ang, impR, impX       //calc impedance : R, X
        call uImpedanceToRefco S11GraphR0, impR, impX, rho, ang   //calc S11
        db=20*uSafeLog10(rho) //put S11 in db form   ver115-1b fixed typo
    end if
    refLastGraphR0=S11GraphR0   //ver116-1b
end sub
*/
}

void msagraph::mUpdateMarkerLocations()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;

  //Find point numbers for peak markers and for L and R if relative to the peaks
      //We are called from mDrawMarkerInfo, which also has the ability to move markers.
      //saveSel$=selMarkerID$   //We want to save and restore the current selected marker
  /*    //axisNum$=str$(primaryAxisNum)
      if (hasMarkPeakPos || hasMarkPeakNeg)    //Locate peaks
      {
          if (doPeaksBounded==1)
          {
              if hasMarkL=0 then pStart=1 else pStart=gMarkerPointNum(mMarkerNum("L"))
              if hasMarkR=0 then pEnd=gPointCount() else pEnd=gMarkerPointNum(mMarkerNum("R"))
          }
          else
          {
              pStart=1 : pEnd=gPointCount()   //Signals to search all points
          }
          gFindPeaks(primaryAxisNum,pStart, pEnd, minNum, maxNum, minY, maxY);
          if hasMarkPeakPos then call gUpdateMarkerPointNum mMarkerNum("P+"),maxNum
          if hasMarkPeakNeg then call gUpdateMarkerPointNum mMarkerNum("P-"),minNum
      }
      if doLRRelativeTo$<>"" then  //Locate LR relative to another marker
          markNum=mMarkerNum(doLRRelativeTo$)
          if markNum<1 then notice "Invalid Marker Number"    //For debugging
          pNum=gMarkerPointNum(markNum)
          if pNum=0 then  //Only proceed if the marker exists
              call mDeleteMarker "L" : call mDeleteMarker "R" //ver115-1e
          else
              call gFindDBOffset primaryAxisNum,pNum, doLRRelativeAmount, doLRAbsolute,leftPoint, rightPoint //ver115-3f
              if leftPoint>=1 then call mAddMarker "L", leftPoint, axisNum$ else call mDeleteMarker "L"  //ver115-3b
              if rightPoint>=1 then call mAddMarker "R", rightPoint, axisNum$ else call mDeleteMarker "R"  //ver115-3b
          end if
      end if
      if doFilterAnalysis then
          peakPoint=gMarkerPointNum(mMarkerNum(filterPeakMarkID$)) //Get point number of marker that is the peak reference
          if peakPoint<0 then doFilterAnalysis=0    //Our peak marker has been deleted, so terminate the analysis
      end if

          //If we are placing markers as part of filter analysis, we do it now.
       if doFilterAnalysis then
          peakPoint=gMarkerPointNum(mMarkerNum(filterPeakMarkID$)) //Get point number of marker that is the peak reference
          if peakPoint<0 then doFilterAnalysis=0    //Our peak marker has been deleted, so terminate the analysis
      end if
      if doFilterAnalysis then
          //Locate the points required for filter analysis. It is possible the points
          //don't exist on the graph, in which case their point numbers will end up as -1.
          call gSetPeakAnalysis x1DBDown, x2DBDown, ripHighFreq, ripLowFreq
              //For now we do auto determination of ripple search limits in gPeakAnalysis
          call gPeakAnalysis primaryAxisNum, peakPoint, 1, 1  //Find points down 3db, x1DBDown and x2DBDown, and ripple min and max; ver115-3b
          call gGetPeakAnalysisPoints  db3LowPoint, db3HighPoint, x1LowPoint, x1HighPoint, x2LowPoint, x2HighPoint
              //Place markers 2,3,4 and 5 as needed on the x1 and x2 points. Note that mAddMarker for
              //an existing marker just relocates it.
          if x1DBDown<>0 then _
                  call mAddMarker "2", x1LowPoint, axisNum$ : call mAddMarker "3", x1HighPoint, axisNum$  //ver115-3b
          if x2DBDown<>0 then _
                  call mAddMarker "4", x2LowPoint, axisNum$ : call mAddMarker "5", x2HighPoint, axisNum$  //ver115-3b
      end if
      selMarkerID$=saveSel$   //ver114-5L deleted call to mDisplaySelectedMarker
  end sub
*/
}
void msagraph::mDrawMarkerInfo()
{
  //Draw marker info at bottom of graph
  //gGetGraphicsSize(graphwide, graphigh);
  mUpdateMarkerLocations();  //Determines locations of peak markers and L,R if they are relative to the peaks

    //We will draw marker info in a rectangular area below the labels of the x axis.
    //This is the frequency and graph values
    gDrawMarkerInfo();
/*
    'Set InfoX and InfoY where additional info can be printed.
    InfoX=gGetMarkerInfoRight()+5
    InfoY=gGetMarkerInfoTop()
    if selMarkerID$<>"" then call mDisplaySelectedMarker 'info may have changed
        'We may have to print some additional info from an analysis
    if doFilterAnalysis then
        peakPoint=gMarkerPointNum(mMarkerNum(filterPeakMarkID$)) 'Get point number of marker that is the peak reference
        'Note we know peakPoint is a valid point because mUpdateMarkerLocations set doFilterAnalysis=0 if it was not.
        peakFreq=gGetPointXVal(peakPoint)   'Frequency of peak
        'gPeakAnalysis has found the points that are down the required number of db. It is possible the points
        'don't exist on the graph, in which case their point numbers will end up as -1.

        call gGetPeakAnalysisPoints db3LowPoint, db3HighPoint, x1LowPoint, x1HighPoint, x2LowPoint, x2HighPoint
        call gGetPeakAnalysisRipple ripLowFreq, ripHighFreq, ripMinVal, ripMaxVal
            'Get frequencies at each of the points
        db3HighFreq=gGetPointXVal(db3HighPoint)
        db3LowFreq=gGetPointXVal(db3LowPoint)
        x1HighFreq=gGetPointXVal(x1HighPoint)
        x1LowFreq=gGetPointXVal(x1LowPoint)
        x2HighFreq=gGetPointXVal(x2HighPoint)
        x2LowFreq=gGetPointXVal(x2LowPoint)
        db3BW=db3HighFreq-db3LowFreq

        'If any of the required db values does not have both its points, we set a flag to zero.
        if db3LowPoint>0 and db3HighPoint>0 then hasDB3=1 else hasDB3=0
        if x1LowPoint>0 and x1HighPoint>0 then hasX1=1 else hasX1=0
        if x2LowPoint>0 and x2HighPoint>0 then hasX2=1 else hasX2=0

        call gGetInfoColors textColor$, backColor$
        if twoPortWinHndl$="" then gBox$="#handle.g" else gBox$="#twoPortGraphBox" 'main win or two port win 'ver116-4e
        #gBox$, "color ";textColor$      'Set drawing color
        #gBox$, "font Tahoma 8 bold";       'Set drawing font
        #gBox$, "backcolor ";backColor$     'Set background color
        #gBox$, "size 1"
        'We draw a filled box to get an outline and to clear any existing data
        boxHt= 5 + 7*13     'Assume 7 lines of height 13 each
        boxWidth=150
        #gBox$, "place ";InfoX;" "; InfoY 'locate pen at upper left
        #gBox$, "boxfilled "; InfoX+boxWidth; " "; InfoY+boxHt   'This is lower right corner
        InfoX=InfoX+7
        InfoY=InfoY+15  'Set to bottom of first line (matches marker heading)
        'Now print the filter information on screen
        if x1DBDown<>3 and x2DBDown<>3 and hasDB3 then
            'If neither x1 nor x2 is 3 db down, then print the 3db bandwidth if we have it
            bw$=uFormatted$(db3BW*1000000,"3,3,4//UseMultiplier//suffix=Hz")
            call gPrintText "BW(3dB)=";bw$, InfoX, InfoY :InfoY=InfoY+13   'e.g. BW(3dB)=123 KHz
        end if
        if hasX1 then
            x1BW=x1HighFreq-x1LowFreq
            bw$=uFormatted$(x1BW*1000000,"3,3,4//UseMultiplier//suffix=Hz")
            call gPrintText "BW(";str$(x1DBDown);"dB)=";bw$, InfoX, InfoY :InfoY=InfoY+13   'e.g. BW(3dB)=123 KHz
        end if
        if hasX2<>0 then
            x2BW=x2HighFreq-x2LowFreq
            bw$=uFormatted$(x2BW*1000000,"3,3,4//UseMultiplier//suffix=Hz")
            call gPrintText "BW("; str$(x2DBDown);"dB)=";bw$, InfoX, InfoY :InfoY=InfoY+13   'e.g. BW(3dB)=123 KHz
        end if

        'Print Q if we have the necessary data
        if hasDB3 and db3BW>0 then
            Q$=Trim$(using("#####.#", peakFreq/db3BW))
            call gPrintText "Q=";Q$, InfoX, InfoY :InfoY=InfoY+13  'e.g. Q=345.1
        end if

        if hasX1 and hasX2 and x1BW>0 then  'Print shape factor if we have needed data
            shape$=Trim$(using("###.##", x2BW/x1BW))
            call gPrintText "SF(";x2DBDown; "dB/";x1DBDown;"dB)=";shape$, InfoX, InfoY :InfoY=InfoY+13  'e.g. Shape=2.6 ver115-1a
        end if
        if msaMode$="ScalarTrans" or msaMode$="VectorTrans" then    'ver114-5n
            IL=0-gGetPointYVal(peakPoint,primaryAxisNum)  'ver116-4s allowed IL to be negative
            'Note TO DO: we could also do this if in TG mode
            IL$=Trim$(using("###.###", IL))       'Insertion loss, which we only do if in transmission 'ver115-1e
            call gPrintText "IL=";IL$, InfoX, InfoY : InfoY=InfoY+13
        end if
            'Print ripple. Ideally we would print the ripple search bounds, but we don't have room
            'Maybe we should delete insertion loss, because it is apparent from the peak value
        if hasDB3 then
            rip$=Trim$(using("####.###", ripMaxVal-ripMinVal))  'ver115-1e
            call gPrintText "Ripple=";rip$;" dB", InfoX, InfoY : InfoY=InfoY+13
            InfoX=InfoX+boxWidth    'End by setting InfoX to the far right of what we just drew
        end if
    end if

   */
}
void msagraph::gDrawGrid()
{
//  QString startTime = util.time("ms");
  QString stopTime;
  graphScene->clear();

  for (int i = 0; i < gErase1.count(); i++)
  {
    gErase1[i] = 0;
  }
  for (int i = 0; i < gErase2.count(); i++)
  {
    gErase2[i] = 0;
  }
  haltMarker = NULL;


  //Clears screen and draws background grid
  //Also accumulates all the draw commands for the grid, other than the initial
  //cls and fill commands, into the string gGridString$. Text commands are not included,
  //because once the "\" character is found, everything after that is considered one piece
  //of text to be printed.
  //ver115-1b caused gGridString$ to be created even if gIsDynamic=0

  graphScene->setBackgroundBrush(gridappearance->gBackColor); // set background to black

  //cmd$="down;size 1;color ";gGridLineColor$;";"   //Set color and size 1
  if (gDoY1 || gDoY2)
  {
    int y;
    for (int i=2; i <= gVertDiv; i++)   //draw horizontal lines; skip those under the boundary
    {
      //Pixel locations are in gY1GridLines or gY2GridLines; if in both they are the same
      if (gDoY1)
        y=gOriginY - gY1GridLines[i];
      else
        y=gOriginY - gY2GridLines[i];
      graphScene->addLine(gMarginLeft, y, gMarginLeft+gGridWidth, y, QPen(gridappearance->gGridLineColor));
    }
  }

  for (int i=2; i <= gHorDiv; i++)  //draw vertical lines; skip those under the boundary
  {
      //Pixel locations are in gXGridLines
      int x=gMarginLeft+gXGridLines[i];
      //cmd$=cmd$+"line "; x; " ";gOriginY; " "; x; " ";gOriginY-gGridHeight ;";"
      graphScene->addLine(x, gOriginY, x, gOriginY-gGridHeight, QPen(gridappearance->gGridLineColor));
  }

  int gridRight=gMarginLeft+gGridWidth;
  QPen pen(gridappearance->gGridBoundsColor, 3);
  graphScene->addLine(gMarginLeft, gOriginY+1, gridRight, gOriginY+1, pen);  //bottom
  graphScene->addLine(gMarginLeft-1, gMarginTop, gMarginLeft-1, gOriginY, pen); // left
  graphScene->addLine(gMarginLeft,gMarginTop-1,gridRight,gMarginTop-1,pen);  //top
   //ver114-4m changed below to gridRight+1 now that final point is being set.
  graphScene->addLine(gridRight+1,gMarginTop, gridRight+1, gOriginY,pen);    //right
  gPrintGridLabels();   //Print numeric values at appropriate grid lines
  gPrintTitle(0);
  gPrintAxisLabels();   //Print names of axes and the title, all above the grid
  /*
  //Flush to make graphics stick. Note there is no danger of repeated calls to gDrawGrid
  //causing an accumulation of flushed segments, since we start with "cls" which clears
  //the graphics memory. Also creates a segment called gGridLabels for quickly redrawing
  //items that are wholly or partially erased when we erase and redraw just the grid.
  //Note gGridLabels is not a "variable"; it is the literal name of the flushed segment.
*/
//  scene->setSceneRect(scene->itemsBoundingRect());
//  ui->graphicsView->fitInView(scene->sceneRect());
  stopTime = util.time("ms");
  //QMessageBox::warning(this, startTime, stopTime);
  //QCoreApplication::processEvents();
}
void msagraph::CreateReferenceSource()
{
  //Return reference trace ver114-7e
  //Create referenceSource() data in accordance with referenceLineType and referenceLineSpec$
  //referenceSource entries are numbered from 1
  //refType describes the reference type:
  //   referenceLineType=0   No Reference lines
  //   referenceLineType=1   Use existing datatable() data at time reference was created
  //   referenceLineType=2   Calculate trace for RLC combo specified in spec$, per uParseRLC
  //   referenceLineType=3   use fixed value references
  //This routine is specific to the test setup. In the case of RLC combos, it can calculate impedance at a freq, and
  //then determine the S21 that the impedance would generate in the S21 fixture, or it can generate S11 data
  //based on the current ref impedance.
  if (referenceLineType==0)
  {
    referenceSourceNumPoints=0;
    return;
  }   //No reference lines needed
  if (referenceLineType==2)  //2=RLC
  {
    uWork->uSetMaxWorkPoints(gNumDynamicSteps()+1,3);
    uWork->uWorkNumPoints=gNumDynamicSteps()+1;
    for (int i=0; i <= uWork->uWorkNumPoints; i++)
    {
      uWork->uWorkArray[i+1][0] = vars->datatable[i][1];
    }   //set up for uRLCComboResponse
      //Calc response in whatever S11 or S21 setup the user has chosen
    int doSpecialR0; QString doSpecialJig;
    if (vars->msaMode == modeReflection)
    {
      doSpecialR0=vnaCal->S11BridgeR0; doSpecialJig="S11";   //ver115-2a
    }
    else
    {
      doSpecialR0=vnaCal->S21JigR0;
      if (vnaCal->S21JigAttach=="Series") doSpecialJig="S21Series"; else doSpecialJig="S21Shunt";
    }
    //TO DO--Note uRLCComboResponse does not adjust for S21JigShuntDelay. If we need to do so, do it here.
    int isErr= util.uRLCComboResponse(referenceLineSpec, doSpecialR0, doSpecialJig);
    if (isErr) { QMessageBox::about(0,"", "Invalid RLC reference specification"); return; }

    //uWorkArray now contains the db, degrees response for each frequency
    //Transfer to referenceSource()
    referenceSourceNumPoints=uWork->uWorkNumPoints;
    for (int i=1; i <= uWork->uWorkNumPoints; i++)
    {
      referenceSource[i][0]=uWork->uWorkArray[i][0];  //freq
      referenceSource[i][1]=uWork->uWorkArray[i][1];  //db
      referenceSource[i][2]=uWork->uWorkArray[i][2];  //ang
    }
  }
  else
  {
    if (referenceLineType==3) //3=Fixed value
    {
      QString s=referenceLineSpec;
      float v1, v2, v3;
      util.uExtractNumericItems(2, s, ";",v1, v2, v3);
      referenceSourceNumPoints=gNumDynamicSteps()+1;
      for (int i=1; i <= referenceSourceNumPoints; i++)
      {
        referenceSource[i][0]=gGetPointXVal(i);   //Actual tuning freq, in MHz
        referenceSource[i][1]=v1;    //Fixed val for trace 1
        referenceSource[i][2]=v2;    //Fixed val for trace 2
      }
    }
    else
    {
      //referenceLineType=1; use current data
      //ver115-7a modified this
      int source1, source2;
      referenceSourceNumPoints=gNumDynamicSteps()+1;
      if (vars->msaMode==modeSA)
      {
        source1=constMagDBM;
        source2=constNoGraph;
      }
      if (vars->msaMode==modeScalarTrans)
      {
        source1=constMagDB;
        source2=constNoGraph;
      }
      if (vars->msaMode==modeVectorTrans)
      {
        source1=constMagDB;
        source2=constAngle;
      }
      if (vars->msaMode==modeReflection)
      {
        source1=constGraphS11DB;
        source2=constGraphS11Ang;
      }
      for (int i=1; i <= referenceSourceNumPoints; i++)
      {
        referenceSource[i][0]=gGetPointXVal(i);   //Actual tuning freq, in MHz
        //No matter what we are doing with the reference, we save dB(m)/angle data as the "source"
        //and later do any necessary "transform".
        double y1, y2;
        CalcGraphDataType(i-1, source1, source2, y1, y2,0);  //calc db/angle info
        referenceSource[i][1] = y1;   //dB or dBm
        referenceSource[i][2] = y2;    //angle or 0
      }
    }
  }
}

void msagraph::CreateReferenceTransform()
{
//Transform referenceSource() data into actual graph data; put it into referenceTransform
  for (int i=0; i < referenceSourceNumPoints; i++)  //iterate by step num, though referenceTransform starts at 1 (point num)
  {
    double ref1, ref2;
    CalcReferencesWholeStep(i, ref1, ref2);
    referenceTransform[i+1][0]=referenceSource[i+1][0];  //freq
    referenceTransform[i+1][1]=ref1;  //Trace 1
    referenceTransform[i+1][2]=ref2; //Trace 2
  }
}
void msagraph::CalcReferencesWholeStep(int stepNum, double &ref1, double &ref2)
{
  //Calculate reference line data at whole step stepNum
  int pointNum = stepNum+1;
  if (referenceLineType==3)
  {
    //Here we have a fixed value reference, which we don't need to transform.
    //We just transfer the values.
    ref1=referenceSource[pointNum][1];
    ref2=referenceSource[pointNum][2];
    return;
  }
  if (vars->msaMode==modeReflection)
  {
    //For reflection we have to calculate a bunch of derived data, such as impedance,
    //which may be needed to calculate the graph data. This is done frequency by frequency
    //using uWorkReflectData() as an intermediary
    vars->uWorkReflectData[0]=referenceSource[pointNum][0];  //freq
    vars->uWorkReflectData[1]=referenceSource[pointNum][1];  //db
    vars->uWorkReflectData[2]=referenceSource[pointNum][2];  //ang
    CalcReflectDerivedData(-1);  //Calculate other reflection derived data from the above
    //Calc graph data from info just put into uWorkReflectData
    //fix me int thispointy1=0 : thispointy2=0
    CalcReflectGraphData(-1, ref1, ref2, 1);
  }
  else
  {
    //For non-reflection modes, the only source data we need for the calculations are
    //freq, db and ang (or dBm as the case may be). We transfer all the data to uWorkArray
    //and then calculate from there
    uWork->uWorkArray[1][0]=referenceSource[pointNum][0];  //freq
    uWork->uWorkArray[1][1]=referenceSource[pointNum][1];  //db
    uWork->uWorkArray[1][2]=referenceSource[pointNum][2];  //ang
    CalcGraphData(0, ref1, ref2, 1);     //calc values from uWorkArray
  }
}

void msagraph::CalcReferences()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub CalcReferences stepNum, ref1IsAngle, byref ref1, ref2IsAngle, byref ref2   'Return value of reference lines at step stepNum, possibly fractional
    'We interpolate if necessary. ref1IsAngle=1 means reference 1 is an angle and we have to
    'deal with possible wraparound when interpolating. Likewise for ref2IsAngle
    if stepNum<0 then stepNum=0
    if stepNum>globalSteps then stepNum=globalSteps
    'The point may not be integral, so we need to interpolate
    whole=int(stepNum) : fract=stepNum-whole
    if fract=0 then
        call CalcReferencesWholeStep stepNum, ref1, ref2
        exit sub
    else
        'If we are not at a whole step, we need to interpolate. The regular graph will interpolate the value
        'being graphed, not the underlying S11, so we do the same
        call CalcReferencesWholeStep whole, refA1, refA2  'ref values at wholeStep
        call CalcReferencesWholeStep whole+1, refB1, refB2 'ref values at wholeStep+1
        if (referenceTrace and 1)=0 then
            ref1=0  'Don't have ref line for axis Y1
        else
            if ref1IsAngle then 'angles--must worry about wraparound
                'Note angles are kept in range -180 to +180; will be adjusted in gGraphVal to fit range
                ref1=intLinearInterpolateDegrees(fract, refA1, refB1, -180, 180)   'ver116-4k
            else
                ref1=refA1 + fract*(refB1-refA1)
            end if
        end if
        if (referenceTrace and 2)=0 then
            ref2=0  'Don't have ref line for axis Y2
        else
            if ref2IsAngle then 'angles--must worry about wraparound
                'Note angles are kept in range -180 to +180; will be adjusted in gGraphVal to fit range
                ref2=intLinearInterpolateDegrees(fract, refA2, refB2, -180, 180)   'ver116-4k
            else
                ref2=refA2 + fract*(refB2-refA2)
            end if
        end if
    end if
end sub
*/
}
QString msagraph::CreateReferenceTraces(QColor tCol, int tSize, int traceNum, QPainterPath *path)
{
  QString retVal;
  //Return reference trace ver114-7e
  //The trace is created from the data in referenceTransform().
  //referenceTrace indicates which traces we want
  if ((traceNum && referenceTrace)==0)
  {
    retVal="down";
    return retVal;
  }
  uWork->uSetMaxWorkPoints(referenceSourceNumPoints,3);
  uWork->uWorkNumPoints=referenceSourceNumPoints;
  for (int i=1; i <= uWork->uWorkNumPoints; i++)
  {
    uWork->uWorkArray[i][0]=referenceTransform[i][0];  //freq
    uWork->uWorkArray[i][1]=referenceTransform[i][1];  //Trace 1 data
    uWork->uWorkArray[i][2]=referenceTransform[i][2]; //Trace 2 data
  }

  retVal = "color "+tCol.name()+";size "+tSize+";"+PrivateCreateReferenceTrace(traceNum, 1, gNumDynamicSteps()+1, path);
  return retVal;
}
QString msagraph::PrivateCreateReferenceTrace(int traceNum, int startPoint, int endPoint, QPainterPath *path)
{

  //Return reference line for RLC combo
  //We generate values from startPoint to endPoint for a reference trace, using data in uWorkArray()
  //traceNum=1 for Y1 and 2 for Y2.
  //color and size prefix not included
  QString t;
  int span=endPoint-startPoint;
  if (span==0) return "down";     //down is a NOP in order to have a valid command

  int y1IsPhase, y2IsPhase;
  gGetIsPhase(y1IsPhase, y2IsPhase);
  float lastY;
  float lastX;
  float x, y;
  if (startPoint!=1)
  {
    //If not very first point of trace, the starting point is the point before startPoint
    if (traceNum==1)
    {
      if (y1IsPhase)
        lastY=gAdjustPhaseToDisplay(1, startPoint-1, 1);
      else
        lastY=uWork->uWorkArray[startPoint-1][1];
      gConvertY1ToPix(lastY);
    }
    else
    {
      if (y2IsPhase)
        lastY=gAdjustPhaseToDisplay(2, startPoint-1, 1);
      else
        lastY=uWork->uWorkArray[startPoint-1][2];
      gConvertY2ToPix(lastY);
    }
    lastX=gGetPointXPix(startPoint-1);   //Ref has same x pixel as main graph
  }
  for (int currPoint=startPoint;  currPoint <= endPoint; currPoint++)
  {
      //create segment from prior point to this point, for both y1 and y2 as appropriate
    if (traceNum==1)
    {
      if (y1IsPhase)
        y=gAdjustPhaseToDisplay(1, currPoint, 1);
      else
        y=uWork->uWorkArray[currPoint][1];
      gConvertY1ToPix(y);
    }
    else
    {
      if (y2IsPhase)
        y=gAdjustPhaseToDisplay(2, currPoint, 1);
      else
        y=uWork->uWorkArray[currPoint][2];
      gConvertY2ToPix(y);
    }
    x=gGetPointXPix(currPoint);   //Ref has same x pixel as main graph
    if (currPoint==1)
    {
      path->moveTo(x,y);
    }
    else
    {
        path->lineTo(x,y);
        //draw final segment (of this larger segment) in reverse to be sure the very last pixel gets drawn.
        if (currPoint==endPoint)
        {
          path->lineTo(lastX,lastY);
        }
    }
    lastX=x; lastY=y;
  }
  //Result is what we have put together so far, plus the final line drawn backward to
  //be sure final point is fully drawn

  path->lineTo(lastX,lastY);
  return t;
}
void msagraph::CalcGraphData(int currStep, double &y1, double &y2, int useWorkArray)
{
  //Calculate y1,y2 per user request
  //If useWorkArray=1 then the data source is uWorkArray() or uWorkReflectData;
  //otherwise it is datatable()or ReflectArray()
  //If data type is an auxiliary type, we retrieve the data from auxGraphData()
  //currStep may have a fractional part, in which case we need to interpolate.
  y1=0;
  y2=0;
  if (vars->Y1DataType>=constAux0 && vars->Y1DataType<=constAux5)
  {
    y1=vars->auxGraphData[currStep][vars->Y1DataType-constAux0];
  }
  if (vars->Y2DataType>=constAux0 && vars->Y2DataType<=constAux5)
  {
    y2=vars->auxGraphData[currStep][vars->Y2DataType-constAux0];
  }
  if (vars->msaMode==modeReflection)
  {
    CalcReflectGraphData(currStep, y1, y2, useWorkArray);
  }
  else
  {
    if (vars->msaMode!=modeSA)
    {
      CalcTransmitGraphData(currStep, y1, y2, useWorkArray);
    }
    else
    {
      //Here for spectrum analyzer mode
      float db;
      if (useWorkArray)
        db=uWork->uWorkArray[currStep+1][1];
      else
        db=vars->datatable[currStep][2];

      if (referenceDoMath==1) //ref math is to be done on db values--only allowed for SA mode ver115-5d
      {
        db=referenceOpA*referenceTransform[currStep+1][1]+referenceOpB*db;
      }
      for (int i=1; i <= 2; i++)
      {
        int c;
        if (i==1)
          c=vars->Y1DataType;
        else
          c=vars->Y2DataType;
        float fpow;
        float y;
        if (c == constMagDBM)
        {
          y=db;
        }
        else if (c == constMagWatts)
        {
          fpow = pow(10,(db/10));
          y=fpow/1000;
        }
        else if (c == constMagV)
        {
          fpow=pow(10,(db/10));
          y=sqrt(fpow/20);    //square root of 50* mw/1000
        }
        else if (c == constAux0
                 || c == constAux1
                 || c == constAux2
                 || c == constAux3
                 || c == constAux4
                 || c == constAux5)
        {
          if (i==1)
            y=y1;
          else
            y=y2;  //Auxiliary data has already been calculated, so keep it
        }
        else   //constNoGraph, or invalid data type
          y=0;
        if (i==1)
          y1=y;
        else
          y2=y;
      }
    }  //end SA mode
  }  //end all modes

  //The point may not be integral, so we need to interpolate
  int wholeStep=(int)currStep;
  float fract=currStep-wholeStep;
  if (wholeStep==vars->globalSteps || fract<0.0000001)
    return;    //No need to interpolate; we are done

  //We interpolate the required data types, not the raw S21, S11, etc.
  double nextY1;
  double nextY2;
  CalcGraphData(wholeStep+1, nextY1, nextY2, useWorkArray); //Get values at next step

  if (vars->Y1DataType!=constNoGraph)
    y1=inter.LinearInterpolateDataType(vars->Y1DataType, fract, y1, nextY1);

  if (vars->Y2DataType!=constNoGraph)
    y2=inter.LinearInterpolateDataType(vars->Y2DataType, fract, y2, nextY2);
}
void msagraph::CalcGraphDataType(int currStep, int dataType1, int dataType2, double &y1, double &y2, int useWorkArray)
{
  //Calculate specified data type (constRho, etc.) for step currStep
  //This just calls CalcGraphData, after forcing data types to the dataTypes we want.
  int saveY1Type=vars->Y1DataType;
  int saveY2DataType=vars->Y2DataType;   //Save original types

  vars->Y1DataType=dataType1;
  vars->Y2DataType=dataType2;   //Set the types we want

  CalcGraphData(currStep, y1, y2, useWorkArray);   //Calculate them

  vars->Y1DataType=saveY1Type;
  vars->Y2DataType=saveY2DataType;   //Restore
}

void msagraph::setUwork(cWorkArray *newuWork)
{
  uWork = newuWork;
}

void msagraph::setVna(dialogVNACal *newVnaCal)
{
  vnaCal = newVnaCal;
}

void msagraph::setHwdIf(hwdInterface *newHwdIf)
{
  hwdIf = newHwdIf;
}

void msagraph::setAppearance(dialogGridappearance *newGridappearance)
{
  gridappearance = newGridappearance;
}
void msagraph::gInitFirstUse(QGraphicsView *view, int winWidth, int winHt, int marLeft, int marRight, int marTop, int marBot)
{
  graphicsView = view;
  gNumMarkers=20;  //ver114-4d Some may not be in use at any given time
  gSetMaxPoints(800);
  gUpdateGraphObject(winWidth, winHt, marLeft, marRight, marTop, marBot);
  graphicsView->fitInView(graphScene->sceneRect());
  gInitDefaults();
  gClearMarkers();
  gIsFirstDynamicScan=0;
  gIsDynamic=0;
  gGridString="";
  gGridBmp="";
  gridappearance->gInitCustomColors(); //Initialize custom color sets ver116-4b
}
void msagraph::gInitDynamicDraw()
{
  //Initialize to commence dynamic draw process
  //nEraseLead specifies the number of points by which the erasure leads the drawing.
  //doEraseN indicates whether trace N should be erased as it is redrawn.
  //A dynamic draw involves erasing prior lines while drawing new ones.
  //As each trace in a dynamic graph is drawn, it is necessary to "discard" occasionally to
  //clear accumulated graphics info. To avoid affecting user drawn graphics somehow, we start
  //with a flush which avoids discarding that prior graphics.
  //#gGraphHandle$, "flush"
  gIsDynamic=1;    //Signal to accumulate drawing commands
  gIsFirstDynamicScan=1;
  gNumPoints=0;        //Clear existing points

  gSetErasure(1,1,1);  //Default values. also calls gInitDraw   ver114-6e
  gPrevPointNum=0; //ver114-6d
  //gTrace1$=gDraw1$
  //gTrace2$=gDraw2$
  //Start over accumulating draw commands. We use "down" as the empty command
  for (int i=1; i <= gDynamicSteps+1; i++)
  {
    gTrace1[i]=QPointF(0,0);
    gTrace2[i]=QPointF(0,0);
  }
}

void msagraph::CalcReflectDerivedData(int currStep)
{
  //Calc ReflectArray() data from S11. frequency and S11 must already be in place.
  //if currStep<0 then we get the data from uWorkReflectData(x) and put results into uWorkReflectData(x)
  //otherwise we get the data from ReflectArray() and put results into ReflectArray(currStep,x)
  //S11 is assumed to be re S11GraphR0

  double trueFreq;
  double db;
  double ang;
  if (currStep<0)  //Get frequency and S11; we calculate everything else
  {
    trueFreq=1000000*vars->uWorkReflectData[0];
    db=vars->uWorkReflectData[constGraphS11DB];
    ang=vars->uWorkReflectData[constGraphS11Ang];
  }
  else
  {
    trueFreq=1000000*vars->ReflectArray[currStep][0];
    db=vars->ReflectArray[currStep][constGraphS11DB];
    ang=vars->ReflectArray[currStep][constGraphS11Ang];
  }

  double rho=pow(10,(db/20));

  double serL, serC, serR, serReact;
  double parL, parC, parR, parReact;
  util.uRefcoToImpedance(vars->S11GraphR0, rho, ang, serR, serReact);
  util.uEquivSeriesLC(trueFreq, serR, serReact, serL, serC);
  if (fabs(serReact)<0.001)
    serReact=0  ;
  if (serR<0.001)
    serR=0;
  util.uEquivParallelImped(serR, serReact, parR, parReact); //Convert imped to equivalent parallel resistance and reactance
  if (fabs(parReact)<0.001)
    parReact=0;
  if (parR<0.001)
    parR=0;
  if (trueFreq==0)
  {
    parR=1e12;//constMaxValue;
    parL=1e12;//constMaxValue;
    parC=0;   //Set for max impedance
  }
  else
  {
    double twoPiF = 2.0*util.uPi() * trueFreq;
    if (parReact>=1e12)//constMaxValue)
      parL=1e12;//constMaxValue;
    else
      parL = parReact/twoPiF;
    if (parReact==0)
      parC=1e12;//constMaxValue;
    else
      parC = -1.0/(twoPiF * parReact);
  }

  double swr;
  if (rho>0.999999)
    swr=9999;
  else
    swr=(1+rho)/(1-rho);
  //Impose a max of 1F or 1H
  if (serC>1) serC=1;
  if (serL>1) serL=1;
  if (parC>1) parC=1;
  if (serC>1) serC=1;
  if (serC<-1) serC=-1;
  if (serL<-1) serL=-1;
  if (parC<-1) parC=-1;
  if (serC<-1) serC=-1;
  double minC=1e-15;
  double minL=1e-12; //impose min of 1 fF and 1 pH ver115-2d
  if (abs(serC)<minC) serC=0;
  if (abs(parC)<minC) parC=0;
  if (abs(serL)<minL) serL=0;
  if (abs(parL)<minL) parL=0;

  if (currStep<0)    //Store the data in the appropriate place
  {
    vars->uWorkReflectData[constRho]=rho;
    vars->uWorkReflectData[constImpedMag]=sqrt(pow(serR,2)+pow(serReact,2));  //mag of impedance
    vars->uWorkReflectData[constImpedAng]=util.uATan2(serR, serReact);  //angle of impedance
    vars->uWorkReflectData[constSerR]=serR;
    vars->uWorkReflectData[constSerReact]=serReact;
    vars->uWorkReflectData[constSerC]=serC;
    vars->uWorkReflectData[constSerL]=serL;
    vars->uWorkReflectData[constParR]=parR;
    vars->uWorkReflectData[constParReact]=parReact;
    vars->uWorkReflectData[constParC]=parC;
    vars->uWorkReflectData[constParL]=parL;
    vars->uWorkReflectData[constSWR]=swr;
  }
  else
  {
    vars->ReflectArray[currStep][constRho]=rho;
    vars->ReflectArray[currStep][constImpedMag]=sqrt(pow(serR,2)+pow(serReact,2));  //mag of impedance
    vars->ReflectArray[currStep][constImpedAng]=util.uATan2(serR, serReact);  //angle of impedance
    vars->ReflectArray[currStep][constSerR]=serR;
    vars->ReflectArray[currStep][constSerReact]=serReact;
    vars->ReflectArray[currStep][constSerC]=serC;
    vars->ReflectArray[currStep][constSerL]=serL;
    vars->ReflectArray[currStep][constParR]=parR;
    vars->ReflectArray[currStep][constParReact]=parReact;
    vars->ReflectArray[currStep][constParC]=parC;
    vars->ReflectArray[currStep][constParL]=parL;
    vars->ReflectArray[currStep][constSWR]=swr;
  }
}
void msagraph::CalcReflectGraphData(int currStep, double &y1, double &y2, int useWorkArray)
{
  double y;
  double X;
  double R;

  //Calculate y1,y2 per user request
  //If useWorkArray=1 then the data source is uWorkReflectData(); otherwise it is ReflectArray()
  //Power is in uWorkArray(currStep+1,1), phase in uWorkArray(currStep+1,2)
  //S11 db is in ReflectArray(currStep,2), phase in ReflectArray(currStep,3)

  for (int dataNum=2 ; dataNum >= 1; dataNum--)
  {
    if (dataNum==1)
    {
      if (vars->msaMode==modeSA || vars->msaMode==modeScalarTrans)
        break;  //exit for don't have phase
      componConst=vars->Y1DataType;
    }
    else
    {
      componConst=vars->Y2DataType;
    }
    switch (componConst)
    {
    case constGraphS11DB:
    case constGraphS11Ang:
    case constRho:
    case constImpedMag:
    case constImpedAng:
    case constSerR:
    case constSerReact:
    case constParR:
    case constParReact:
    case constSerC:
    case constSerL:
    case constParC:
    case constParL:
    case constSWR:
      //All these have already been computed
      if (useWorkArray)
        y=vars->uWorkReflectData[componConst];
      else
        y=vars->ReflectArray[currStep][componConst];
      break;
    case constTheta:     //Same as angle
      if (useWorkArray)
        y=vars->uWorkReflectData[constGraphS11Ang];
      else
        y=vars->ReflectArray[currStep][constGraphS11Ang];
      break;
    case constReturnLoss:
      if (useWorkArray)
        y=0-vars->uWorkReflectData[constGraphS11DB];
      else
        y=0-vars->ReflectArray[currStep][constGraphS11DB];
      break;
    case constReflectPower:
      if (useWorkArray)
        y=100*pow(vars->uWorkReflectData[constRho],2);
      else
        y=100*pow(vars->ReflectArray[currStep][constRho],2);
      break;
    case constComponentQ:
      //Note that this formula works only for a single L or C. For LC combos][ we would need the reactance
      //of the individual components
      if (useWorkArray)
      {
        X=vars->uWorkReflectData[constSerReact];
        R=vars->uWorkReflectData[constSerR];
      }
      else
      {
        X=vars->ReflectArray[currStep][constSerReact];
        R=vars->ReflectArray[currStep][constSerR];
      }

      if (R==0)
        y=99999;
      else
        y=abs(X)/R; //Q=X/R
      break;
    case constAdmitMag:
    case constAdmitAng:
    case constConductance:
    case constSusceptance:
      switch (componConst)
      {
      case constAdmitMag:
        double mag;
        if (useWorkArray)
          mag=vars->uWorkReflectData[constImpedMag];
        else
          mag=vars->ReflectArray[currStep][constImpedMag];
        if (mag==0)
          y=vars->constMaxValue;
        else
          y=1/mag;
        break;
      case constAdmitAng:
        double ang;
        if (useWorkArray)
          ang=vars->uWorkReflectData[constImpedAng];
        else
          ang=vars->ReflectArray[currStep][constImpedAng];

        y=0-ang;
        break;
      default: //constConductance, constSusceptance
        if (useWorkArray)
        {
          R=vars->uWorkReflectData[constSerR];
          X=vars->uWorkReflectData[constSerReact];
        }
        else
        {
          R=vars->ReflectArray[currStep][constSerR];
          X=vars->ReflectArray[currStep][constSerReact];
        }
        double G, S;
        ComplexMaths a;
        a.cxInvert(R, X, G, S);
        if (componConst==constConductance)
          y=G;
        else
          y=S;
      }
      break;
    case constAux0:
    case constAux1:
    case constAux2:
    case constAux3:
    case constAux4:
    case constAux5:
      if (dataNum==1)
        y=y1;
      else y=y2 ; //Auxiliary data has already been calculated, so keep it
      break;
    default:
      y=0;    //invalid data, or None
    }
    if (dataNum==1)   //Put data into y1 or y2
      y1=y;
    else
      y2=y;
  }

}

void msagraph::SetStartStopFreq(float startF, float stopF)
{
  //Use Start/Stop to determine centfreq, sweepwidth, startfreq and endfreq
  //A centralized routine is used so all these related variables can be kept in sync.
  //Note gCalcGraphParams must be called to fully update graph module
  vars->startfreq=startF;
  vars->endfreq=stopF;
  if (vars->startfreq>vars->endfreq)
  {
    float dum=vars->startfreq;
    vars->startfreq=vars->endfreq;
    vars->endfreq=dum;    //Swap to get in right order
  }
  vars->centfreq=(util.usingF("####.######",(vars->startfreq+vars->endfreq)/2)).toFloat(); //ver114-5n
  vars->sweepwidth=vars->endfreq-vars->startfreq;
  gSetXAxisRange(vars->startfreq, vars->endfreq);  //ver114-6d

}
void msagraph::gDrawMarkers()        //Draw all listed markers
{
qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;


  //For the moment, use the grid color and font
//  #gGraphHandle$, "color "; gGridTextColor$; ";font "; gGridFont$
//  #gGraphHandle$, "size 1"
  gMarkerString="";
  //QString join="";
  for (int i=1; i < gNumMarkers; i++)   //Send each marker to the draw routines
  {
    QString cmd=gDrawMarkerAtPointNum(gMarkerPoints[i][0], gMarkers[i][1], gMarkers[i][2], gMarkers[i][0]);
      //if cmd$<>"" then gMarkerString$=gMarkerString$+join$+cmd$ : join$=";"
  }
}
void msagraph::gEraseMarkers()        //Erase all listed markers ver116-4b
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    'We just draw them with the background color, using the info in gMarkerString$, which will have items
    'separated by semicolons. Each drawn marker is represented by four items :style, label, x pixel and y pixel.
   #gGraphHandle$, "color "; gBackColor$; ";font "; gGridFont$
   #gGraphHandle$, "size 1"
   s$=gMarkerString$
   for i=0 to 50 'process until all info is done
        offset=4*i
        style$=word$(s$, offset+1, ";") : if style$="" then exit for   'find next semicolon
        label$=word$(s$, offset+2, ";") : xPix=val(word$(s$, offset+3, ";")) : yPix=val(word$(s$, offset+4, ";"))
        call gDrawMarkerPix style$, label$, xPix, yPix
   next i
end sub
*/
}

QString msagraph::gDrawMarkerAtPointNum(float N,QString trace, QString style, QString markLabel)   //Draw marker at point N; return string of info
{
    //trace$ specifies which trace to mark. See the select block
    //markLabel$ is the label to be drawing with the marker, if the style provides for a label.
    //See gDrawMarkerPix for meaning of style$. N is the point number and may be fractional, indicating
    //that we are part way between two points
    //We return a string that can be later used for erasing the markers, containing one or two groups of four
    //items: style$, markLabel$, x pixel location, and y pixel location, separated by semicolons.
    if (N<1 || N > gDynamicSteps+1)
    {
      return "";    //Don't draw if out of bounds
    }
    trace=trace.trimmed().toUpper();
    int whole=int(N);
    float fract=N-whole;
    float xPix=gGetPointXPix(N);
    //ver114-7d moved all interpolation before the select statement, and used intLinearInterpolateDegrees
    //to handle phase wrap-around
    //First find the Y pixel values for each trace being graphed
    float y1 = 0;
    float y2 = 0;
    int y;
    if (gGraphY1==1)
    {
        float y1=gGraphPix[whole][1];
        if (fract>0)
        {
            float thisY=vars->gGraphVal[whole][1];
            float nextY= vars->gGraphVal[whole+1][1];
            //Note that angles in the main program are kept in the range -180 to +180, but
            //in gGraphVal they have been adjusted to fit graph range
            if (gY1IsPhase) //Special rule for phase
            {
                y1=inter.intLinearInterpolateDegrees(fract, thisY, nextY, gY1AxisMin, gY1AxisMax); //interpolate, dealing with wrap-around ver116-4k
                y1=(y1-gY1AxisMin)*gY1Scale+gOriginY;    //Convert to pixels
                if (y1<gMarginTop)
                  y1=gMarginTop;
                if (y1>gOriginY)
                  y1=gOriginY;
            }
            else
            {
                y1=y1+fract*(gGraphPix[whole+1][1]-y1);   //Interpolate non-phase between two pixel values
            }
        }
    }
    /*
    if gGraphY2=1 then
        y2=gGraphPix(whole,2)
        if fract>0 then
            thisY=gGraphVal(whole,2)
            nextY= gGraphVal(whole+1,2)
            if gY2IsPhase then //Special rule for phase
                //Note that angles in the main program are kept in the range -180 to +180, but
                //in gGraphVal they have been adjusted to fit graph range
                y2=intLinearInterpolateDegrees(fract, thisY, nextY, gY1AxisMin, gY1AxisMax) //interpolate, dealing with wrap-around ver116-4k
                y2=(y2-gY2AxisMin)*gY2Scale+gOriginY    //Convert to pixels
                if y2<gMarginTop then y2=gMarginTop
                if y2>gOriginY then y2=gOriginY
            else
                y2=y2+fract*(gGraphPix(whole+1,2)-y2)   //Interpolate non-phase between two pixel values
            end if
        end if
    end if
        */
    xPix=int(10*xPix+0.5)/10;
    y1=int(10*y1+0.5)/10;
    y2=int(10*y2+0.5)/10;
        //xPix, y1 and y2 now have the pixel values for the specified point
        //Use trace$ to determine the pixel location of the marker

        if (trace == "1")   //Marker on trace 1 (phase, if used)
        {
            if (gGraphY1==0)
              return "";
            y=y1;
        }
        else if (trace == "2")   //marker on trace 2 (dbm or db; right axis)
        {
            if (gGraphY2==0)
              return "";
            y=y2;
        }
        else if (trace == "HIGHPIX")   //mark the highest graph pixel at this point
        {
            y=gOriginY;
            //Note minimum pixel value is highest on the graph
            if (gGraphY1==1)
                y=y1;
            if (gGraphY2==1)
            {
                if (y2<y)
                  y=y2;
            }
        }
        else if (trace == "LOWPIX")   //mark the lowest graph pixel at this point
        {
            y=gMarginTop;
            //Note max pixel value is lowest on the graph
            if (gGraphY1==1)
                y=y1;

            if (gGraphY2==1)
            {
                if (y2>y)
                  y=y2;
            }

        }
        else if (trace == "XAXIS")  //Mark on x-axis   //ver114-4d
        {
            y=gOriginY;
        }
        else if (trace == "ALL")  //Mark on all traces   //ver114-5n
        {
            QString m="";
            //QString join="";
            if (gGraphY1)
            {
              gDrawMarkerPix(style, markLabel, xPix, y1 );
              m=style+";"+markLabel+";"+xPix+";"+y1;
             // join=";";
            }
//            if gGraphY2 then call gDrawMarkerPix style$, markLabel$, xPix, y2 _
//                        : m$=m$;join$;style$;";";markLabel$;";";xPix;";";y2
//            gDrawMarkerAtPointNum$=m$
            return "";
        }
        else if (trace == "NONE")  //invisible
            return "";
        else
            return "";    //invalid trace$

    gDrawMarkerPix(style, markLabel, xPix, y);    //Finally draw the marker
    //gDrawMarkerAtPointNum$=style$;";";markLabel$;";";xPix;";";y
    return style + ";" + markLabel + ";" + xPix +";" + y;
}
void msagraph::gDrawMarkerPix(QString style, QString markLabel, float x, float y)    //Draw marker at pix coord (x,y)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;

  //markLabel$ is the marker ID, for styles which draw the ID
  style=style.trimmed().toUpper();
  //We round pixels to the nearest tenth just to keep the accumulated draw commands shorter
  //If they have already been so rounded, this doesn't hurt
  //x=int(10*x+0.5)/10 : y=int(10*y+0.5)/10
  if (style == "WEDGE")
  {
    gDrawWedgePix(x,y-1);
  }
  else if (style == "LABELEDWEDGE") //If too close to top, invert it ver114-5n
  {
    if (y<gMarginTop+15)
    {
      gDrawMarkerPix("LabeledInvertedWedge",markLabel, x,y);
      return;
    }
    gPrintTextCentered(markLabel,x+1,y-9,QColor(Qt::magenta));
    gDrawWedgePix(x,y-1);
  }
  else if (style == "INVERTEDWEDGE")
  {
    gDrawInvertedWedgePix(x,y+1);
  }
  else if (style == "LABELEDINVERTEDWEDGE") //If too close to bottom, un-invert it ver114-5n
  {
    if (y>gOriginY-15)
    {
      gDrawMarkerPix("LabeledWedge",markLabel, x,y);
      return;
    }
    gPrintTextCentered(markLabel,x+1,y+19,QColor(Qt::magenta));
    gDrawInvertedWedgePix(x,y+1);
  }
  else if (style == "SMALLINVERTEDWEDGE")
  {
    gDrawSmallInvertedWedgePix(x, y+1);
  }
  else if (style == "HALTPOINTER")
  {
    gDrawHaltPointerPix(x, y+5);
  }
  else if (style == "XOR")  //Draw black box in XOR mode to invert colors; Do it twice to restore to original ver116-4h
  {
    //cmd$="rule xor; size 1; color black; place ";x-3;" ";y-3;";down;box ";x+3;" ";y+3;";rule over"
    //#gGraphHandle$, cmd$ : #gGraphHandle$, "discard"   //Actual draw, and discard so lots of these don't build up
  }
}

void msagraph::gDrawWedgePix(float x, float y)  //Draw wedge pointing down at pixel coord (x,y)
{
  QPainterPath mark;
  QPen pen;

  mark.moveTo(x,y);
  mark.lineTo(x-5,y-6);
  mark.lineTo(x+5,y-6);
  mark.lineTo(x,y);

  pen.setBrush(QColor(Qt::magenta));
  pen.setWidth(1);

  graphScene->addPath(mark, pen);
}
void msagraph::gDrawInvertedWedgePix(float x, float y)  // Draw wedge pointing up at pixel coord (x,y)
{
  QPainterPath mark;
  QPen pen;

  mark.moveTo(x,y);
  mark.lineTo(x-5,y+6);
  mark.lineTo(x+5,y+6);
  mark.lineTo(x,y);

  pen.setBrush(QColor(Qt::magenta));
  pen.setWidth(1);

  graphScene->addPath(mark, pen);
}
void msagraph::gDrawSmallInvertedWedgePix(float x, float y)   //Draw small wedge pointing up at pixel coord (x,y)
{
  QPainterPath mark;
  QPen pen;

  mark.moveTo(x,y);
  mark.lineTo(x-4,y+6);
  mark.lineTo(x+4,y+6);
  mark.lineTo(x,y);

  pen.setBrush(QColor(Qt::magenta));
  pen.setWidth(1);

  graphScene->addPath(mark, pen);
}
void msagraph::gDrawHaltPointerPix(float x, float y)    //Draw pointer at pixel coord (x,y) pointing in sweep direction
{

  if (haltMarker == NULL)
  {
    haltMarker = new marker((qreal)x, (qreal)y, marker::haltMarker);
    if (gSweepDir == -1)
    {
      haltMarker->setReverseHaltMarker(true);
    }
    else
    {
      haltMarker->setReverseHaltMarker(false);
    }

    graphicsView->scene()->addItem(haltMarker);
  }
  else
  {
    if (gSweepDir == -1)
    {
      haltMarker->setReverseHaltMarker(true);
    }
    else
    {
      haltMarker->setReverseHaltMarker(false);
    }
    haltMarker->setPos(x,y);
  }
}
void msagraph::gRefreshTraces()  //Redraw traces from gTrace1$() and gTrace2$()
{
  //gTrace1 and gTrace2 will each have gNumPoints points,

  if (gNumPoints<=0)
    return;

  gInitDraw();
  int pMin;
  int pMax;
  gGetMinMaxPointNum(pMin, pMax);    //ver114-6d

  QPen pen;
  if (gGraphY1 && gTrace1Width>0)
  {
    QPainterPath trace1;

    trace1.moveTo(gTrace1.at(pMin));
    for (int j=pMin+1; j <= pMax; j++)
    {
      trace1.lineTo(gTrace1.at(j));
    }
    pen.setBrush(QColor(gridappearance->gTrace1Color));
    pen.setWidth(gTrace1Width);

//    graphScene->addPath(trace1, pen);

  }
  if (gGraphY2 && gTrace2Width>0)
  {
    QPainterPath trace2;
    trace2.moveTo(gTrace2.at(pMin));
    for (int j=pMin+1; j <= pMax; j++)
    {
      trace2.lineTo(gTrace2.at(j));
    }
    pen.setBrush(gridappearance->gTrace2Color);
    pen.setWidth(gTrace2Width);

    //graphScene->addPath(trace2,pen);
   }

}
void msagraph::gRefreshGridLinesOnly()   //Redraw grid lines but no text, without first clearing the background
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    'This assumes the dynamic draw procedure has been used to accumulate grid drawing commands
    'into gGridString$, grid label draw commands into the segment gGridLabels.
    #gGraphHandle$, gGridString$    'Draws the grid lines
*/
}
void msagraph::CalcTransmitGraphData(int currStep, double &y1, double &y2, int useWorkArray)
{

  //Calculate y1,y2 per user request
  //If useWorkArray=1 then the data source is uWorkArray(); otherwise it is S21DataArray()
  //ver116-1b deleted calc of trueFreq, which was incorrect and not needed
  float db;
  float ang;
  if (useWorkArray)
  {
    db=uWork->uWorkArray[currStep+1][1];
    ang=uWork->uWorkArray[currStep+1][2];
  }
  else
  {
    db=vars->S21DataArray[currStep][1];
    ang=vars->S21DataArray[currStep][2];
  }
  //ver115-1e deleted impedance transform for transmission mode
  int componConst;
  for (int dataNum=1; dataNum <= 2;dataNum++)
  {
    if (dataNum==1)
    {
      componConst=vars->Y1DataType;
    }
    else
    {
      componConst=vars->Y2DataType;
    }
    float y;
    switch(componConst)
    {
    case constMagDB:
      y=db;
      break;
    case constMagDBM:    //This gives raw data before cal was applied
      //Used for transmission mode only
      if (vnaCal->applyCalLevel==0)
      {
        y=db;
      }
      else
      {
        y=db+vars->lineCalArray[currStep][1];
      }
      break;
    case constMagRatio:
      y=pow(10,(db/20));
      break;
    case constInsertionLoss:
      y=0-db;
      break;
    case constAngle:
      //Source data is raw
      y=ang;
      break;
    case constRawAngle:  // raw phase before cal. Used for transmission mode only
      if (vnaCal->applyCalLevel==0)
      {
        y=ang;
      }
      else
      {
        y=util.NormalizePhase(ang+vars->lineCalArray[currStep][2]);
      }
      break;

    case constGD:    //calc group delay
      if (vars->startfreq==vars->endfreq)
      {

        PrintMessage("Can't calculate Group Delay with zero sweep width.");
        y=-1;
      }
      else
      {
        int pStart, pEnd;
        gGetSweepStartAndEndPointNum(pStart, pEnd);
        int dir=gGetSweepDir();  //1 or -1
        if (currStep==pStart-1)
        {
          y=-1;  //Need two points; only have one at first point
        }
        else
        {
          if (useWorkArray)
          {
            y=(uWork->uWorkArray[currStep+1][2]-uWork->uWorkArray[currStep+1-dir][2])/360;
          }
          else
          {
            y=(vars->S21DataArray[currStep][2]-vars->S21DataArray[currStep-dir][2])/360;
          }//delta phase in cycles  ver116-1b
          if (y>0.5)
          {
            y=y-1;
          }
          else if (y<-0.5)
          {
            y=y+1; //Deal with wraparound ver114-6k
          }
          float deltaF;
          if (useWorkArray)
          {
            deltaF=1000000*(uWork->uWorkArray[currStep+1][0]-uWork->uWorkArray[currStep+1-dir][0]);
          }
          else
          {
            deltaF=1000000*(vars->S21DataArray[currStep][0]-vars->S21DataArray[currStep-dir][0]); //delta freq, cycles per second ver116-1b
          }
          y=0-y/deltaF;   //negative of delta phase over delta freq
        }
      }
      break;
    case constAux0:
    case constAux1:
    case constAux2:
    case constAux3:
    case constAux4:
    case constAux5:
      if (dataNum==1)
      {
        y=y1;
      }
      else
      {
        y=y2;  //Auxiliary data has already been calculated, so keep it
      }
      break;
    default:
      y=0;    //invalid data, or None
    }
    if (dataNum==1)  //Put data into y1 or y2
    {
      y1=y;
    }
    else
    {
      y2=y;
    }
  }

}
QString msagraph::gRestoreTraceContext(QString &s, int &startPos, int isValidation)
{
  //Restore context info on trace graphing
  //Return error message if error; otherwise "". We ignore data prior to startPos.
  //startPos is updated to one past the last line we process (normally EndContext or end of string)
  //successive lines are separated by chr$(13)
  //If isValidation=1, then we just run through the data for error checking, without changing anything.
  //We don't save/restore gDoErase1, gDoErase2, gEraseLead because user will reset them in gInitDynamicDraw
  QString newLine="\r";
  QString aSpace=" ";
  QString sep=";;"; //Used to separate text items
  QString nonTextDelims=" ,\t";    //space, comma and tab are delimiters
     //Get next line and increment startPos to start of the following line
  QString tLine=util.uGetLine(s, startPos);
  QString tag;
  while (tLine!="")
  {
    QString origLine=tLine; //ver115-1b
    if (tLine.left(10).toUpper()=="ENDCONTEXT") break;
    int isErr=0;
    int equalPos=tLine.indexOf("=");     //equal sign marks end of tag
    if (equalPos==-1)
    {
      tag=""; //We will skip this line; it does not have a tag
    }
    else
    {
      tag=tLine.left(equalPos).toUpper();  //tag is stuff before equal sign
      tLine=tLine.mid(equalPos+1).trimmed();  //contents is stuff after equal sign
    }
    //Each tag represents one or several data items. These are retrieved one at a time,
    //and as each is retrieved it is deleted from tLine$. Numeric items are delimited
    //by spaces, tabs or commas. Text items are delimited by the double character
    //contained in sep$, because they may contain spaces or commas. If this is just
    //a validation run, we do not enter any of the retrieved data into our variables.
    float v1, v2, v3;
    if (tag == "DOHIST")
    {
      isErr=util.uExtractNumericItems(1, tLine,nonTextDelims, v1, v2, v3);
      if (isValidation==0)
        gDoHist=v1;
    }
    else if (tag == "ACTIVEGRAPHS")
    {
      isErr=util.uExtractNumericItems(2, tLine,nonTextDelims, v1, v2, v3);
      int graphY1=v1;
      int graphY2=v2;
      if (isValidation==0)
      {
        gGraphY1=graphY1;
        gGraphY2=graphY2;
      }
    }
    else if (tag == "TRACEWIDTHS")
    {
      isErr=util.uExtractNumericItems(2, tLine,nonTextDelims, v1, v2, v3);
      if (isValidation==0)
      {
        gTrace1Width=v1;
        gTrace2Width=v2;
      }
    }
    else if (tag == "TRACECOLORS")
    {
      QString t1=util.uExtractTextItem(tLine,sep);
      QString t2=util.uExtractTextItem(tLine,sep);
      if (t1=="" || t2=="")
        isErr=1;
      //It was originally intended that trace colors and their grid labels be independent in the graph module,
      //but that is not how we use it, and the independence makes it awkward if the chosen Appearance has
      //one trace/label color and the preference file has a different trace color. So we tie them together.
      if (isValidation==0)
      {
        gridappearance->gTrace1Color = util.fixColor1(t1);
        gridappearance->gTrace2Color = util.fixColor1(t2);
        gridappearance->gY1TextColor = util.fixColor1(t1);
        gridappearance->gY2TextColor = util.fixColor1(t2);
      }
    }
    else
    {
      //Unrecognized tag. Skip it
    }
    if (isErr)
    {
      return "Trace Context Error in: " + origLine;
    }
    //Get next line and increment startPos to start of the following line
    tLine=util.uGetLine(s, startPos);
  }
  return "";
}
QString msagraph::TraceContext()
{
  //Return trace context as string
  //successive lines are separated by chr$(13)
  //First include variables used outside the graph module
  QString s1= "RefreshEachScan="+QString::number(refreshEachScan);
  s1= s1+"\rTraceModes="+QString::number(vars->Y1DisplayMode)+","+QString::number(vars->Y2DisplayMode);
  if (vars->doCycleTraceColors)
  {
    vars->cycleNumber=1;
    gridappearance->gSetTraceColors(gridappearance->cycleColorsAxis1[1], gridappearance->cycleColorsAxis2[1]);
  } //ver116-4s to save basic trace color
  return s1+"\r"+gTraceContext();   //Add items from graph module
}
QString msagraph::RestoreTraceContext(QString s, int &startPos, int isValidation)
{
  //Restore sweep context
  //Returns error message if error; otherwise 0. Ignores data prior to startPos.
  //startPos is updated to one past the last line we process (normally EndContext or end of string)
  //if isValidation=1, we merely check for errors
  //sep$ is a possibly multi-character separator to delimit text items on one line
  //successive lines are separated by chr$(13)
  //QString newLine="\r";
  //QString aSpace=" ";
  //QString sep$=";;"; //Used to separate text items
  QString nonTextDelims=" ,\t";    //space, comma and tab are delimiters
    //Get next line and increment startPos to start of the following line
  int oldStartPos=startPos;
  QString tLine=util.uGetLine(s, startPos);
  QString errMsg;
  oldStartPos=startPos;
  while (tLine!="")
  {
    QString tag;
    QString origLine=tLine;
    if (tLine.toUpper().left(10)=="ENDCONTEXT") break;
    int isErr=0;
    int equalPos=tLine.indexOf("=");     //equal sign marks end of tag
    if (equalPos==-1)
    {
      isErr=1; break;
    }
    else
    {
      tag=tLine.left(equalPos).toUpper();  //tag is stuff before equal sign
      tLine=tLine.mid(equalPos+1).trimmed();  //contents is stuff after equal sign
    }
    if (tag == "REFRESHEACHSCAN")
    {
      if (isValidation==0) refreshEachScan=tLine.toInt();
    }
    else if (tag == "TRACEMODES")
    {
      float v1, v2, v3;
      isErr=util.uExtractNumericItems(2, tLine, nonTextDelims, v1, v2, v3);
      if (isValidation==0 && isErr==0) vars->Y1DisplayMode=v1; vars->Y2DisplayMode=v2;
    }
    else
    {
      //Unrecognized tag. Must belong to the graph module, so we end here
      startPos=oldStartPos;    //reset to beginning of this line
      break;
    }
    if (isErr)
    {
      return "Trace Context Error in: "+ origLine;
    } //ver115-1b
    //Get next line and increment startPos to start of the following line
    oldStartPos=startPos;
    tLine=util.uGetLine(s, startPos);
  }
  //If no error so far, process the data for the graph module
  errMsg = gRestoreTraceContext(s, startPos, isValidation);
  gridappearance->SetCycleColors();     //Set trace colors for color cycling ver116-4s
  return errMsg;
}
QString msagraph::RestoreGridContext(QString s, int &startPos, int isValidation)
{
  //Restore grid context
  //Returns error message if error; otherwise 0. Ignores data prior to startPos.
  //startPos is updated to one past the last line we process (normally EndContext or end of string)
  //if isValidation=1, we merely check for errors

  //Return error message if error
  //startPos=0;
  QString tLine=util.uGetLine(s, startPos);
  int oldStartPos=startPos;
  //QString contextVersion="A";
  while (tLine!="")
  {
    QString tag;
    //QString origLine=tLine; //ver115-1b
    if (tLine.left(10).toUpper()=="ENDCONTEXT")
      break;
    int isErr=0;
    int equalPos=tLine.indexOf("=");     //equal sign marks end of tag
    if (equalPos==-1)
    {
       isErr=1;
       break;
    }
    else
    {
       tag=tLine.left(equalPos).toUpper();   //tag is stuff before equal sign
       tLine=tLine.mid(equalPos+1).trimmed();  //contents is stuff after equal sign
    }

    if (tag == "CUSTOMNAMES")  //List of custom color preset names
    {
      if (isValidation==0)
      {
        for (int i=1; i <= 5; i++)
        {
          gridappearance->customPresetNames[i]=util.uExtractTextItem(tLine, ";:");   //5 items,separated by ";:"
        }
      }
    }
    else
    {
       //Unrecognized tag. Must belong to the graph module, so we end here
       startPos=oldStartPos;    //reset to beginning of this line
       break;
    }
    //Get next line and increment startPos to start of the following line
    oldStartPos=startPos;
    tLine=util.uGetLine(s, startPos);
  }

   //process the data for the graph module
  int oldWidth, oldHeight;
  gGetGraphicsSize(oldWidth, oldHeight);   //save so we can restore
  QString errMsg=gRestoreGridContext(s, startPos, isValidation);

  if (errMsg!="" || isValidation==1) return errMsg;
  vars->primaryAxisNum=gGetPrimaryAxis(); //ver115-3c
  gGetMargins(graphMarLeft, graphMarRight, graphMarTop, graphMarBot);  //Get graph margins
  //We may need to resize the graph window to match the dimensions just retrieved
  //For now, we restore the previous size with the possibly new margins
  gUpdateGraphObject(oldWidth, oldHeight, graphMarLeft, graphMarRight, graphMarTop, graphMarBot);
  graphicsView->fitInView(getScene()->sceneRect());
  QColor referenceColor1, referenceColor2, dum1, dum2;
  gridappearance->gGetSupplementalTraceColors(referenceColor1, referenceColor2, dum1, dum2);
  if (vars->primaryAxisNum==1)
    referenceColorSmith=referenceColor1;
  else
    referenceColorSmith=referenceColor2;

  return errMsg;
}
QString msagraph::gGridContext()
{
  //Return string with context info on grid size and appearance
  //successive lines are separated by chr(13)
  QString newLine="\r";
  QString aSpace=" ";
  QString sep=";;";   //delimits text items on one line

  QString s1= "WindowSize="+ QString::number(gWindowWidth)+ aSpace+ QString::number(gWindowHeight);
  s1= s1+ newLine+ "Margins="+ QString::number(gMarginLeft)+ aSpace+ QString::number(gMarginRight)+
                        aSpace+ QString::number(gMarginTop)+ aSpace+ QString::number(gMarginBot);
  s1= s1+ newLine+ "GridDivisions="+ QString::number(gHorDiv)+ aSpace+ QString::number(gVertDiv);
  s1= s1+ newLine+ "PrimaryAxis="+ QString::number(gPrimaryAxis);
  s1= s1+ newLine+ "AxisLabels="+ gXAxisLabel+ sep+ gY1AxisLabel+ sep+ gY2AxisLabel;

  QString s2= "AxisTextColors="+ gridappearance->gXTextColor.name()+ sep+ gridappearance->gY1TextColor.name()+ sep+ gridappearance->gY2TextColor.name();
  s2= s2+ newLine+ "AxisFonts="+ gridappearance->gXAxisFont+ sep+ gridappearance->gY1AxisFont+ sep+ gridappearance->gY1AxisFont;
  s2= s2+ newLine+ "BackColor="+ gridappearance->gBackColor.name();
  s2= s2+ newLine+ "GridTextColor="+ gridappearance->gGridTextColor.name();
  s2= s2+ newLine+ "GridFont="+ gridappearance->gGridFont;
  s2= s2+ newLine+ "GridLineColor="+ gridappearance->gGridLineColor.name();
  s2= s2+ newLine+ "GridBoundsColor="+ gridappearance->gGridBoundsColor.name();
  s2= s2+ newLine+ "GridStyles="+ gXGridStyle+ sep+ gY1GridStyle+ sep+ gY2GridStyle;
  s2= s2+ newLine+ "AxisFormats="+ gXAxisForm+ sep+ gY1AxisForm+ sep+ gY2AxisForm;

  for (int i=1; i<= 5; i++)    //custom colors. One line per color set. Start with set number, then list of colors ver115-2a
  {
    QString s3="CustomColors="+QString::number(i)+",";

    QColor grid, bounds, back, gridText, XText, Y1Text, Y2Text, trace1, trace2, trace1A, trace2A, trace1B, trace2B;
    gridappearance->gGetCustomPresetColors( i, grid,bounds,back,gridText,XText,Y1Text,Y2Text, trace1, trace2,
            trace1A, trace2A, trace1B, trace2B);

    s3=s3+grid.name()+sep+bounds.name()+sep+back.name()+sep+ gridText.name()+sep+XText.name()+sep+Y1Text.name()+sep+
                Y2Text.name()+sep+trace1.name()+sep+trace2.name()+sep+trace1A.name()+sep+trace2A.name()+sep+trace1B.name()+sep+trace2B.name()+sep;

    s2=s2+newLine+s3; //Append this line to s1
  }
  return s1+ newLine+ s2+ newLine+ "LastPresets=" + gridappearance->gGraphColorPreset+ sep+ gridappearance->gGraphTextPreset;
}

QString msagraph::gTraceContext()
{
  //Return string with context info on trace graphing
  //successive lines are separated by chr$(13)
  //We don't save/restore gDoErase1, gDoErase2, gEraseLead because user will reset them in gInitDynamicDraw
  QString newLine="\r";
  QString aSpace=" ";
  QString sep=";;";   //delimits text items on one line
  QString s1= newLine+ "ActiveGraphs="+ QString::number(gGraphY1)+ aSpace+ QString::number(gGraphY2);
  s1="DoHist="+ QString::number(gDoHist);
  s1= s1+ newLine+ "TraceWidths="+ QString::number(gTrace1Width)+ aSpace+ QString::number(gTrace2Width);
  s1= s1+ newLine+ "TraceColors="+ gridappearance->gTrace1Color.name()+ sep+ gridappearance->gTrace2Color.name();
  return s1;
}

QString msagraph::GridContext()
{
  //Get grid context as string
  QString s="CustomNames=";
  for (int i=1; i <= 5; i++)
  {
    s=s+gridappearance->customPresetNames[i]+";:";   //use a goofy separator so user won't have used it
  }
  return s+"\r"+gGridContext();
}
QString msagraph::gRestoreGridContext(QString &s, int &startPos, int isValidation)
{
  //Restore info on grid size and appearance from string s$
  //If isValidation=1, then we just run through the data for error checking, without changing anything.
  //Return error message if error; otherwise "". We ignore data prior to startPos.
  //startPos is updated to one past the last line we process (normally EndContext or end of string)
  //successive lines are separated by chr$(13)
  //CalcGraphParams MUST BE CALLED at some point after this routine to adjust to the margins
  //and hor/vert divisions. This can be done by a subsequent call of gRestoreSweepContext or
  //by a direct call.
  //QString newLine="\r";
  //QString aSpace=" ";
  QString sep=";;"; //Used to separate text items
  QString nonTextDelims=" ,\t";    //space, comma and tab are delimiters
  //Get next line and increment startPos to start of the following line
  QString tLine=util.uGetLine(s, startPos);
  float v1, v2, v3;
  QString t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13;
  while (tLine != "")
  {
    QString origLine=tLine;
    if (tLine.left(10).toUpper()=="ENDCONTEXT")
      break;
    int isErr=0;
    int equalPos=tLine.indexOf("=");     //equal sign marks end of tag
    QString tag;
    if (equalPos==-1)
    {
      tag=""; //We will skip this line; it does not have a tag
    }
    else
    {
      tag=tLine.left(equalPos).toUpper();  //tag is stuff before equal sign
      tLine=tLine.mid(equalPos+1).trimmed();  //contents is stuff after equal sign
    }

    //Each tag represents one or several data items. These are retrieved one at a time,
    //and as each is retrieved it is deleted from tLine$. Numeric items are delimited
    //by spaces, tabs or commas. Text items are delimited by the double character
    //conotained in sep$, because they may contain spaces or commas. If this is just
    //a validation run, we do not enter any of the retrieved data into our variables.
    //ver114-3f deleted Context Version and Title. Title is now part of Data context
    if (tag == "WINDOWSIZE")
    {
      isErr=util.uExtractNumericItems(2, tLine,nonTextDelims, v1, v2, v3);
      if (isValidation==0)
      {
        gWindowWidth=v1;
        gWindowHeight=v2;
      }
    }
    else if (tag == "MARGINS")
    {
      isErr=util.uExtractNumericItems(2, tLine,nonTextDelims, v1, v2, v3);
      if (isValidation==0)
      {
        gMarginLeft=v1;
        gMarginRight=v2;
      }
      isErr=util.uExtractNumericItems(2, tLine,nonTextDelims, v1, v2, v3);
      if (isValidation==0)
      {
        gMarginTop=v1;
        gMarginBot=v2;
      }
    }
    else if (tag == "PRIMARYAXIS")
    {
      isErr=util.uExtractNumericItems(1, tLine,nonTextDelims, v1, v2, v3);
      if (v1!=1 && v1!=2)
        isErr=1;
      if (isValidation==0 && isErr==0)
      {
        gSetPrimaryAxis(v1);
      }
    }
    else if (tag == "AXISTEXTCOLORS")
    {
      t1=util.uExtractTextItem(tLine,sep);
      t2=util.uExtractTextItem(tLine,sep);
      t3=util.uExtractTextItem(tLine,sep);
      if (t1=="" || t2=="" || t3=="")
        isErr=1;
      if (isValidation==0)
      {
        gridappearance->gXTextColor=util.fixColor1(t1);
        gridappearance->gY1TextColor=util.fixColor1(t2);
        gridappearance->gY2TextColor=util.fixColor1(t3);
      }
    }
    else if (tag == "AXISFONTS")
    {
      t1=util.uExtractTextItem(tLine,sep);
      t2=util.uExtractTextItem(tLine,sep);
      t3=util.uExtractTextItem(tLine,sep);
      if (t1=="" || t2=="" || t3=="")
        isErr=1;
      if (isValidation==0)
      {
        gridappearance->gXAxisFont=t1;
        gridappearance->gY1AxisFont=t2;
        gridappearance->gY2AxisFont=t3;
      }
    }
    else if (tag == "AXISLABELS")
    {
      t1=util.uExtractTextItem(tLine,sep);
      t2=util.uExtractTextItem(tLine,sep);
      t3=util.uExtractTextItem(tLine,sep);
      if (isValidation==0)
      {
        gXAxisLabel=t1;
        gY1AxisLabel=t2;
        gY2AxisLabel=t3;
      }
    }
    else if (tag == "BACKCOLOR")
    {
      t1=util.uExtractTextItem(tLine,sep);
      if (t1=="")
        isErr=1;
      if (isValidation==0)
      {
        gridappearance->gBackColor = util.fixColor1(t1);
      }
    }
    else if (tag == "GRIDTEXTCOLOR")
    {
      t1=util.uExtractTextItem(tLine,sep);
      if (t1=="")
        isErr=1;
      if (isValidation==0)
      {
        gridappearance->gGridTextColor=util.fixColor1(t1);
      }
    }
    else if (tag == "GRIDFONT")
    {
      t1=util.uExtractTextItem(tLine,sep);
      if (t1=="")
        isErr=1;
      if (isValidation==0)
      {
        gridappearance->gGridFont=t1;
      }
    }
    else if (tag == "GRIDLINECOLOR")
    {
      t1=util.uExtractTextItem(tLine,sep);
      if (t1=="")
        isErr=1;
      if (isValidation==0)
      {
        gridappearance->gGridLineColor=util.fixColor1(t1);
      }
    }
    else if (tag == "GRIDBOUNDSCOLOR")
    {
      t1=util.uExtractTextItem(tLine,sep);
      if (t1=="")
        isErr=1;
      if (isValidation==0)
      {
        gridappearance->gGridBoundsColor=util.fixColor1(t1);
      }
    }
    else if (tag == "GRIDDIVISIONS")
    {
      isErr=util.uExtractNumericItems(2, tLine,nonTextDelims, v1, v2, v3);
      if (isValidation==0)
      {
        gHorDiv=v1;
        gVertDiv=v2;
      }
    }
    else if (tag == "GRIDSTYLES")
    {
      t1=util.uExtractTextItem(tLine,sep);
      t2=util.uExtractTextItem(tLine,sep);
      t3=util.uExtractTextItem(tLine,sep);
      if (t1=="" || t2=="" || t3=="")
        isErr=1;
      if (isValidation==0)
      {
        gXGridStyle=t1;
        gY1GridStyle=t2;
        gY2GridStyle=t3;
      }
    }
    else if (tag == "AXISFORMATS")
    {
      t1=util.uExtractTextItem(tLine,sep);
      t2=util.uExtractTextItem(tLine,sep);
      t3=util.uExtractTextItem(tLine,sep);
      if (t1=="" || t2=="" || t3=="")
        isErr=1;
      if (isValidation==0)
      {
        gXAxisForm=t1;
        gY1AxisForm=t2;
        gY2AxisForm=t3;
      }
    }
    else if (tag == "CUSTOMCOLORS")
    {
      isErr=util.uExtractNumericItems(1, tLine,nonTextDelims, v1, v2, v3);    //Get preset number
      if (v1<1 || v1>5)
        isErr=1;
      //Get the colors
      t1=util.uExtractTextItem(tLine,sep);
      t2=util.uExtractTextItem(tLine,sep);
      t3=util.uExtractTextItem(tLine,sep);
      t4=util.uExtractTextItem(tLine,sep);
      t5=util.uExtractTextItem(tLine,sep);
      t6=util.uExtractTextItem(tLine,sep);
      t7=util.uExtractTextItem(tLine,sep);
      t8=util.uExtractTextItem(tLine,sep);
      t9=util.uExtractTextItem(tLine,sep);
      //added colors for traces 1A, 1B, 2A and 2B. If not present, duplicate the 1 and 2 colors
      if (tLine=="")
      {
        t10=t8;
        t11=t9;
        t12=t8;
        t13=t9;
      }
      else
      {
        t10=util.uExtractTextItem(tLine,sep);
        t11=util.uExtractTextItem(tLine,sep);
        t12=util.uExtractTextItem(tLine,sep);
        t13=util.uExtractTextItem(tLine,sep);
      }
      if (isErr==0 && isValidation==0)
      {
        gridappearance->gSetCustomPresetColors(v1
                                               ,util.fixColor1(t1)
                                               ,util.fixColor1(t2)
                                               ,util.fixColor1(t3)
                                               ,util.fixColor1(t4)
                                               ,util.fixColor1(t5)
                                               ,util.fixColor1(t6)
                                               ,util.fixColor1(t7)
                                               ,util.fixColor1(t8)
                                               ,util.fixColor1(t9)
                                               ,util.fixColor1(t10)
                                               ,util.fixColor1(t11)
                                               ,util.fixColor1(t12)
                                               ,util.fixColor1(t13));
      }
    }
    else if (tag == "LASTPRESETS")
    {
      t1=util.uExtractTextItem(tLine,sep) ;
      t2=util.uExtractTextItem(tLine,sep);
      if (t1=="" || t2=="")
        isErr=1;
      if (isValidation==0)
      {
        gridappearance->gUsePresetColors(t1, gPrimaryAxis);
        gUsePresetText(t2);
        gridappearance->SetCycleColors();
      }
    }
    else
    {
      //Unrecognized tag. Skip it
    }
    if (isErr)
      return "Grid Context Error in: " + origLine;
    //Get next line and increment startPos to start of the following line
    tLine=util.uGetLine(s, startPos);

    //Calculate some variables from those just restored ver114-3a
  }
  gUpdateGraphObject(gWindowWidth, gWindowHeight, gMarginLeft
                           , gMarginRight, gMarginTop, gMarginBot);
  graphicsView->fitInView(graphScene->sceneRect());
  return "";
}

int msagraph::GraphDataContextAsTextArray()
{
  //Put graph data points into uTextPointArray, with header info
   //return number of lines placed into uTextPointArray
   //First 3 lines are title, each preceded by !
   //Next line is sweep info
   //Next is Touchstone options line
   //Next is comment data headings
   //Then comes each point as its own string
   uWork->uTextPointArray[1]="!"+gGetTitleLine(1);
   uWork->uTextPointArray[2]="!"+gGetTitleLine(2);
   uWork->uTextPointArray[3]="!"+gGetTitleLine(3);
   uWork->uTextPointArray[4]="!"+gGetTitleLine(4);  //This line has sweep info set by user+ not printed as part of title ver114-5m
   uWork->uTextPointArray[5]="# MHz S DB R 50";   //Freq in MHz, data in DB/angle format
   uWork->uTextPointArray[6]="! MHz  S21_DB  S21_Degrees";
   //We save frequency plus two data per point even if we only have one trace.
   QString aSpace=" ";
   int pMin, pMax;
   gGetMinMaxPointNum(pMin, pMax);    //ver114-6d
   int minStep=pMin-1 ; int maxStep=pMax-1;
   for (int i=minStep; i < maxStep;i++)    //ver114-6d
   {
     int f, y1, y2;
     gGetPointVal(i+1, f, y1, y2);    //point num is one more than step num.
     uWork->uTextPointArray[i+7]=QString::number(f)+aSpace+QString::number(y2)+aSpace+QString::number(y1);  //Save freq, Y2, Y1
   }
   return maxStep+7;
}
QString msagraph::GraphDataContext()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function GraphDataContext$()    'Return data points as string, with title in first 3 lines
    'We do not include !StartContext or !EndContext lines
    nLines=GraphDataContextAsTextArray()   'Assemble strings into uTextPointArray$
    GraphDataContext$=uTextArrayToString$(1,nLines) 'Assemble array of strings into one string
end function
*/
  return "fix me";
}
void msagraph::GraphDataContextToFile(QStringList &fHndl)
{
  //save data points to file
  //We do not include StartContext or EndContext lines
  //fHndl$ is the handle of an already open file. We output our data
  //but do not close the file.
  int nLines=GraphDataContextAsTextArray();   //Assemble strings into uTextPointArray$
  for (int i=1; i<= nLines;i++)
  {
    fHndl.append(uWork->uTextPointArray[i]);
  }
}
void msagraph::RestoreGraphDataContext(QString &s, int &startPos, int doTitle)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function RestoreGraphDataContext(byref s$, byref startPos, doTitle)   'Restore data points from s$
    'Return number of points; -1 if error
    'Restore data; restore title only if doTitle=1
    'We ignore data prior to startPos. We update startPos to the start of the next line after
    'the point data, per uArrayFromString

    isErr=uArrayFromString(s$, 1, startPos, 3)   'Get data into uWorkArray; 3 data per line (freq+mag+phase)
    if isErr then RestoreGraphDataContext=-1 : exit function

        'Now transfer retrieved data from uWorkArray() to gGraphVal().
        'TO DO--This resizing does not help other arrays, and to resize them we would need to erase them.
        'If we don't have room, we should notify the user that he needs to do a sweep with more points.
    if uWorkNumPoints>gMaxNumPoints() then call ResizeArrays uWorkNumPoints
    call gSetNumDynamicSteps uWorkNumPoints-1
    call gSetNumPoints 0
    for i=1 to uWorkNumPoints
        overflow=gAddPoints(uWorkArray(i, 0), uWorkArray(i, 2), uWorkArray(i, 1)) 'Freq, Phase, Mag
    next i
    if doTitle then
        for i=1 to 4
            call gSetTitleLine i, uWorkTitle$(i)  'Save title, which uArrayFromString retrieved ver114-5i
        next i
    end if
    'Note title line 4 has info on linear/log. We don't change to match, so on a Restart we will
    'not conform. But that should be taken care of in restoring the sweep context. Likewise with path info.
    'Note we do not call gSetNumDynamicSteps; that must be set elsewhere.
    'Caller must also separately conform datatable() with the graph data
    RestoreGraphDataContext=uWorkNumPoints
    prevDataChanged=1
end function
*/
}
void msagraph::GetGraphDataContextFromFile(QFile *fHndl, int doTitle)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function GetGraphDataContextFromFile(fHndl$, doTitle)   'get points from file; return number of points or -1 if error
    'Restore data; restore title only if doTitle=1
    'fHndl$ is the handle of an already open file. We read our data
    'but do not close the file. The last line we read will be "!EndContext"
    isErr=uArrayFromFile(fHndl$,3) 'Get data, 3 per line
    if isErr then GetGraphDataContextFromFile=-1 : exit function
        'Move the data from uWorkArray to gGraphVal
    if uWorkNumPoints>gMaxNumPoints() then call ResizeArrays uWorkNumPoints   'Make sure we have enough room
    call gSetNumPoints 0
    for i=1 to uWorkNumPoints
        overflow=gAddPoints(uWorkArray(i, 0), uWorkArray(i, 2), uWorkArray(i, 1)) 'Freq, Phase, Mag
    next i
    if doTitle then
        for i=1 to 4
            call gSetTitleLine i, uWorkTitle$(i)  'Save title, which uArrayFromString retrieved ver114-5i
        next i
    end if
    'Note title line 4 has info on linear/log. We don't change to match, so on a Restart we will
    'not conform. But that should be taken care of in restoring the sweep context. Likewise with path info.
    'Note we do not call gSetNumDynamicSteps; that must be set elsewhere.
    'Caller must also separately conform datatable() with the graph data
    GetGraphDataContextFromFile=uWorkNumPoints
    prevDataChanged=1
end function
*/
}


void msagraph::ImplementDisplayModes()
{
  int doErase1, doErase2, eraseLead;
  //calculate the various items from Y1DisplayMode and Y2DisplayMode
  //Y1DisplayMode, Y2DisplayMode: 0=off  1=NormErase  2=NormStick  3=HistoErase  4=HistoStick
  //ver115-2c added checks for constNoGraph
  if ((vars->Y1DataType!=constNoGraph && (vars->Y1DisplayMode==2 || vars->Y1DisplayMode==4))
    || (vars->Y2DataType!=constNoGraph && (vars->Y2DisplayMode==2 || vars->Y2DisplayMode==4)))
  {
    vars->isStickMode=1;
  }
  else
  {
    vars->isStickMode=0;
  }
  gSetDoAxis(vars->Y1DataType!=constNoGraph, vars->Y2DataType!=constNoGraph); //Turn graph data on or off ver115-3b
    //Note that gActivateGraphs won't activate a graph if we just set its data existence to zero ver115-3b
  gActivateGraphs(vars->Y1DisplayMode!=0,vars->Y2DisplayMode!=0);   //Turn actual graphing on or off ver115-3b
  if ((vars->Y1DataType!=constNoGraph && vars->Y1DisplayMode>2) || (vars->Y2DataType!=constNoGraph && vars->Y2DisplayMode>2))
  {
    gSetDoHist(1);
  }
  else
  {
    gSetDoHist(0);  //Set histogram or normal trace ver115-3b
  }
  int t1Width, t2Width;
  gGetTraceWidth(t1Width, t2Width);
    //ver114-4n Erase eraseLead points ahead of drawing. The more steps, the larger eraseLead
  if (vars->globalSteps<=50) //ver114-4n reduced eraseLead
  {
    eraseLead=1;
  }
  else
  {
    eraseLead=2+int(vars->steps/400);
    if (((vars->Y1DataType!=constNoGraph && t1Width>2) || (vars->Y2DataType!=constNoGraph && t2Width>2)) && vars->globalSteps > 200)
    {
        eraseLead=eraseLead+1;
    }
  }
  if (vars->Y2DisplayMode!=1 && vars->Y2DisplayMode!=3)
    doErase2=0;
  else
    doErase2=1;
  if (vars->Y1DisplayMode!=1 && vars->Y1DisplayMode!=3)
    doErase1=0;
  else
    doErase1=1;
  gSetErasure(doErase1, doErase2, eraseLead);
}
void msagraph::UpdateGraphParams()
{
  //added UpdateGraphParams; ver114-4n made it a gosub to allow use of non-globals
  //Set up graphs for drawing, but don't draw anything
  if (vars->alternateSweep)
  {
    vars->sweepDir=1;
    gSetSweepDir(1);
  }      //Start out forward if alternating ver114-5a
  int sweepDir=gGetSweepDir();  //ver114-4k
  if (sweepDir==1)   //ver114-4k added this if... block
  {
    //Forward direction
    vars->sweepStartStep=0; vars->sweepEndStep=vars->steps;
  }
  else
  {
    //Reverse direction
    vars->sweepStartStep=vars->steps; vars->sweepEndStep=0;
  }

  gInitGraphRange(vars->startfreq, vars->endfreq, vars->Y1Bot, vars->Y1Top, vars->Y2Bot, vars->Y2Top);  //min and max values for x, y1 and y2; calls gCalcGraphParams
  gCalcGraphParams();   //Calculate new scaling. May change min or max.
  float xMin, xMax;
  gGetXAxisRange(xMin, xMax);   //in case gCalcGraphParams changed axis limits ver116-4k
  if (vars->startfreq != xMin || vars->endfreq != xMax) SetStartStopFreq(xMin, xMax);
  //ver114-5f moved the following here from step 12
  gGenerateXValues(0);   //Precalculate x values for steps+1 points; reset number of points to 0; ver114-1f deleted parameter
  UpdateGraphDataFormat(0);
}

void msagraph::UpdateGraphDataFormat(int doTwoPort)
{
  //Update graph module for the type of data we are graphing, and set data source and component
  //If doTwoPort, we are dealing with two-port graphs ver116-1b
  gSetGridStyles("ENDSANDCENTER", "All", "All");
  //For linear sweep we display frequency in MHz; for log we do 1, 1 K, 1 M, or 1 G
  QString xForm, yForm, y1Form, y2Form;
  if (gGetXIsLinear())     //ver114-6d modified this block to use startfreq/endfreq for log sweeps
    xForm= "4,6,9//suffix= M";
  else
    xForm= "3,4,5//UseMultiplier//DoCompact//Scale=1000000";    //ver115-1e


  //ver115-2c caused the full procedure to be executed for both dataNum.
  //Also eliminated default setting of yForm$
  //ver115-3a moved the select block to DetermineGraphDataFormat so others can use it
  for (int dataNum=1; dataNum <= 2;dataNum++)
  {
    if (doTwoPort) //ver116-1b
    {
      if (dataNum==1)
        componConst=twoPort.TwoPortGetY1Type();
      else
        componConst=twoPort.TwoPortGetY2Type();
    }
    else
    {
      if (dataNum==1) componConst=vars->Y1DataType; else componConst=vars->Y2DataType;
    }

    int doData;
    if (componConst==constNoGraph)
    {
      doData=0; //Indicates whether there is a graph ver115-2c
      yAxisLabel ="None";
      yLabel="None";
      yForm="####.##";    //Something valid, in case it gets mistakenly used
      if (dataNum==1)
      {
        y1AxisLabel="None";
        y1Label="None";
        y1Form="####.##";    //Something valid, in case it gets mistakenly used
      }
      else
      {
        y2AxisLabel="None";
        y2Label="None";
        y2Form="####.##";    //Something valid, in case it gets mistakenly used
      }
    }
    else
    {
      doData=1;
      if (dataNum==1)
      {
        if (doTwoPort)
        {
          twoPort.TwoPortDetermineGraphDataFormat(componConst, y1AxisLabel,y1Label, y1IsPhase,y1Form);
        }
        else
        {
          DetermineGraphDataFormat(componConst, y1AxisLabel,y1Label, y1IsPhase, y1Form);
        }
      }
      else
      {
        if (doTwoPort)
        {
          twoPort.TwoPortDetermineGraphDataFormat(componConst, y2AxisLabel,y2Label, y2IsPhase,y2Form);
        }
        else
        {
          DetermineGraphDataFormat(componConst, y2AxisLabel,y2Label, y2IsPhase,y2Form);
        }
      }
    }
    if (dataNum==1)
      gDoY1=doData;
    else
      gDoY2=doData;
  }

  gSetIsPhase(y1IsPhase, y2IsPhase);   //Tell graph module whether data is phase
  gSetAxisFormats(xForm, y1Form, y2Form);   //Formats for displaying the data values
  gSetAxisLabels("", y1AxisLabel, y2AxisLabel);    //Labels for the axes; No label for freq
  gSetDataLabels(y1Label, y2Label);      //Shorter labels for marker info
  if (doTwoPort) //ver116-1b
  {
    //gSetDoAxis specifies whether data for the axis even exists. gActivateGraphs specifies whether
    //to actually graph the data, based on display mode, which for two port is always On.
    gSetDoAxis(twoPort.TwoPortGetY1Type()!=constNoGraph, twoPort.TwoPortGetY2Type()!=constNoGraph); //Turn graph data on or off ver115-3b
    //Note that gActivateGraphs won't activate a graph if we just set its data existence to zero ver115-3b
    gActivateGraphs(1, 1);   //Turn actual graphing on
  }
  else
  {
    ImplementDisplayModes();  //give effect to Y2DisplayMode and Y1DisplayMode
  }
}

void msagraph::DetermineGraphDataFormat(int componConst, QString &yAxisLabel, QString &yLabel, int &yIsPhase, QString &yForm)
{
  //Return format info
  //componConst indicates the data type. We return
  //yAxisLabel$  The label to use at the top of the Y axis
  //yLabel$   A typically shorter label for the marker info table
  //yIsPhase$ =1 if the value represents phase. This indicates whether we have wraparound issues.
  //yForm$    A formatting string to send to uFormatted$() to format the data
  //
  //ver116-1b added code to display S12 or S22 instead of S21 or S11 when DUT is reversed.
  QString Sref, Strans;
  if (vars->switchFR==0)
  {
    Sref="S11";
    Strans="S21"; //Forward DUT
  }
  else
  {
    Sref="S22";
    Strans="S12";  //ReverseDUT
  }
  yIsPhase=0;  //Default, since most are not phase
  //select case componConst  shortened some labels
  if (componConst == constGraphS11DB)
  {
    yAxisLabel=Sref+" Mag(dB)";
    yLabel=Sref+" dB";
    yForm="####.###";   //ver115-5d
  }
  else if (componConst == constRawAngle) //Used for transmission mode only  //added by ver115-1i
  {
    yAxisLabel="Raw Deg";
    yLabel="Raw Deg";
    yIsPhase=1;
    yForm="#####.##";
  }
  else if (componConst == constAngle || componConst == constGraphS11Ang || componConst == constTheta || componConst == constImpedAng)
  {
    if (componConst==constAngle)
    {
      yAxisLabel=Strans+" Deg";
      yLabel=Strans+" Deg";
    }
    if (componConst==constTheta)
    {
      yAxisLabel="Theta";
      yLabel="Theta";
    }
    if (componConst==constGraphS11Ang)
    {
      yAxisLabel=Sref+" Deg";
      yLabel=Sref+" Deg";
    }
    if (componConst==constImpedAng)
    {
      yAxisLabel="Z Deg";
      yLabel="Z Deg";
    }
    yIsPhase=1;
    yForm="#####.##";     //ver115-5d
  }
  else if (componConst == constGD)    //calc group delay
  {
    yAxisLabel="Grp Delay (sec)";
    yLabel="G.D.";
    yForm="3,2,4//UseMultiplier//DoCompact";
  }
  else if (componConst == constSerReact)
  {
    yAxisLabel="Xs";
    yLabel="Xs";
    yForm="3,3,4//UseMultiplier//SuppressMilli//DoCompact";
  }
  else if (componConst == constParReact)
  {
    yAxisLabel="Xp";
    yLabel="Xp";
    yForm="3,3,4//UseMultiplier//SuppressMilli//DoCompact";
  }
  else if (componConst == constImpedMag)
  {
    yAxisLabel="Z ohms";
    yLabel="Z ohms";
    yForm="3,3,4//UseMultiplier//SuppressMilli//DoCompact";
  }
  else if (componConst == constSerR)
  {
    yAxisLabel="Rs" ; yLabel="Rs";
    yForm="3,3,4//UseMultiplier//SuppressMilli//DoCompact";
  }
  else if (componConst == constParR)
  {
    yAxisLabel="Rp" ; yLabel="Rp";
    yForm="3,3,4//UseMultiplier//SuppressMilli//DoCompact";
  }
  else if (componConst == constSerC)
  {
    yAxisLabel="Cs" ; yLabel="Cs";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (componConst == constParC)
  {
    yAxisLabel="Cp" ; yLabel="Cp";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (componConst == constSerL)
  {
    yAxisLabel="Ls" ; yLabel="Ls";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (componConst == constParL)
  {
    yAxisLabel="Lp" ; yLabel="Lp";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (componConst == constMagDBM)
  {
    if (vars->msaMode==modeSA)
    {
      yAxisLabel="Magnitude (dBm)" ; yLabel="dBm";
    }
    else
    {
      yAxisLabel="Power (dBm)" ; yLabel="dBm"; //ver115-1i
    }
    yForm="####.###";    //ver115-5d
  }
  else if (componConst == constMagWatts)
  {
    yAxisLabel="Magnitude (Watts)" ; yLabel="Watts";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (componConst == constMagDB)         //Only done for Transmission
  {
    if (vars->msaMode==modeScalarTrans)  //ver115-1a
    {
      yAxisLabel="Transmission (dB)"  ; yLabel="dB";
    }
    else
    {
      yAxisLabel=Strans+" dB"  ; yLabel=Strans+" dB";
    }
    yForm="####.###";    //ver115-1e
  }
  else if (componConst == constMagRatio)  //Only done for TG mode transmission
  {
    if (vars->msaMode==modeScalarTrans)   //ver115-4f
    {
      yAxisLabel="Trans (Ratio)" ; yLabel="Ratio";
    }
    else
    {
      yAxisLabel="Mag (Ratio)" ; yLabel="Ratio";
    }
    yForm="3,3,4//UseMultiplier//SuppressMilli//DoCompact";
  }
  else if (componConst == constMagV)
  {
    yAxisLabel="Mag (Volts)"  ; yLabel="Volts";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (componConst == constRho)
  {
    yAxisLabel="Rho"  ;
    yLabel="Rho";
    yForm="#.###";
  }
  else if (componConst == constReturnLoss) //ver114-8d
  {
    yAxisLabel="RL"  ;
    yLabel="RL";
    yForm="###.###";    //ver115-1e
  }
  else if (componConst == constInsertionLoss)  //ver114-8d
  {
    yAxisLabel="Insertion Loss(dB)"  ; yLabel="IL";
    yForm="###.###";    //ver115-1e
  }
  else if (componConst == constReflectPower)  //ver115-2d
  {
    yAxisLabel="Reflect Pow(%)"  ; yLabel="Ref%";
    yForm="###.##";
  }
  else if (componConst == constComponentQ)     //ver115-2d
  {
    yAxisLabel="Component Q"  ; yLabel="Q";
    yForm="#####.#";
  }
  else if (componConst == constSWR)  //ver114-8d
  {
    yAxisLabel="SWR"  ; yLabel="SWR";
    yForm="####.##";
  }
  else if (componConst == constAdmitMag)  //ver115-4a
  {
    yAxisLabel="Admit. (S)" ; yLabel="Y";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (componConst == constAdmitAng)   //ver115-4a
  {
    yAxisLabel="Admit Deg" ; yLabel="Admit Deg";
    yIsPhase=1;
    yForm="#####.##";
  }
  else if (componConst == constConductance)  //ver115-4a
  {
    yAxisLabel="Conduct. (S)" ; yLabel="Conduct";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (componConst == constSusceptance)  //ver115-4a
  {
    yAxisLabel="Suscep. (S)" ; yLabel="Suscep";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (componConst == constNoGraph)   //ver115-2c
  {
    yAxisLabel="None"  ; yLabel="None";
    yForm="####.##";    //Something valid, in case it gets mistakenly used
  }
  else if (componConst == constAux0 || componConst == constAux1 || componConst == constAux2
           || componConst == constAux3 || componConst == constAux4 || componConst == constAux5)
  {
    int auxNum=componConst-constAux0;    //e.g. constAux4 produces 4
    yAxisLabel=vars->auxGraphDataFormatInfo[auxNum][2]  ; yLabel=vars->auxGraphDataFormatInfo[auxNum][3];
    yForm=vars->auxGraphDataFormatInfo[auxNum][1];
  }
  else
  {
    yForm="###.##";
    yAxisLabel="Invalid"  ; yLabel="Invalid";
  }

}
void msagraph::InitGraphParams()
{
  //added InitGraphParams; ver114-4n made it a gosub to allow use of non-globals
  //Initial setup for graphs
  //This is called one time early on, or to re-initialize default values
  //The following intialize a lot of variables that the user may subsequently override
  vars->wate=0 ;
  vars->offset=0 ;
  vars->sgout=10;
  vars->gentrk=0 ;
  vars->normrev=0;
  vars->msaMode=modeSA;
  vars->primaryAxisNum=2;   //Primary Y axis. Eg. on restart in SA mode, mag dBm goes here moved ver116-4m
  SetDefaultGraphData();    //ver115-3b
  vnaCal->S21JigAttach="Series";  //ver115-1b
  vnaCal->S21JigR0=50 ; vnaCal->S21JigShuntDelay=0;
  vnaCal->S11BridgeR0=50 ; vnaCal->S11GraphR0=50;
  vnaCal->S11JigType="Reflect";   //ver115-1b
  UpdateGraphDataFormat(0);  //Updates from graphDataType$
  vars->planeadj=0;    //ver 114-4k
  //freqBand=1      //delver116-1b this is done elsewhere on startup
  //call gSetMode msaMode$ delver116-1b
  SetCenterSpanFreq(0, activeConfig->finalbw/100); //Note finalbw is in kHz, others are in MHz
  gSetNumDivisions(10,10); //Number of Hor and Vert divisions
  gSetTraceWidth(1, 1);     //Set width of both graph traces
  gridappearance->gUsePresetColors("DARK", gPrimaryAxis);    //Activate the appearance ver114-3c
  gSetIsLinear(1, 1, 1);     //default is linear sweep ver114-3c
  gSetNumDynamicSteps(vars->globalSteps);
  vars->alternateSweep=0; vars->sweepDir=1;
  gSetSweepDir(1);   //set sweep to forward and set start/end point nums
  vars->specialOneSweep=0;   //No special sweep in progress
  doGraphMarkers=1;
  refreshEachScan=1;
  referenceTrace=3;
  QColor dum1, dum2;
  gridappearance->gGetSupplementalTraceColors(referenceColor1, referenceColor2, dum1, dum2);
  referenceWidth1=1;
  referenceWidth2=1;
  referenceWidthSmith=1;
  referenceColorSmith=referenceColor2;
  vars->lineCalThroughDelay=0;
  for (int i=1; i <= 5; i++)    //clear custom names
  {
    gridappearance->customPresetNames[i]="Empty";
  }
  //gridappearance->FillAppearancesArray();
  gSetPrimaryAxis(vars->primaryAxisNum);
  UpdateGraphParams();

}

void msagraph::SetYAxes(int data1, int top1, int bot1, int auto1, int data2, int top2, int bot2, int auto2)
{
  //Set datatypes, axis top and bottom, and autoscale //ver116-1b
  //Note gCalcGraphParams must be called to fully update graph module
  autoScaleY1=auto1;
  autoScaleY2=auto2;
  vars->Y1DataType=data1;
  vars->Y2DataType=data2;
  SetY1Range(bot1, top1);
  SetY2Range(bot2, top2);
  vars->Y1DisplayMode=1;
  vars->Y2DisplayMode=1;
  gSetDoAxis((vars->Y1DataType!=constNoGraph), (vars->Y2DataType!=constNoGraph));  //Tell graph module whether we have graphs ver115-2c
  ImplementDisplayModes();

}
void msagraph::SetDefaultGraphData()
{
//  Set data type and range for default data, and Y2DisplayMode and Y1DisplayMode based on msaMode$
//  mod by 116-4h
  autoScaleY1=0;
  autoScaleY2=0;
  int min1, max1, min2, max2;
  GetDefaultGraphData(1, vars->Y1DataType, min1, max1);
  GetDefaultGraphData(2, vars->Y2DataType, min2, max2);
  SetY1Range(min1, max1);
  SetY2Range(min2, max2);
  vars->Y1DisplayMode=1;
  vars->Y2DisplayMode=1;
  ImplementDisplayModes();

}
void msagraph::SetCenterSpanFreq(float cent, float span)
{

  //Use Center/Span to determine centfreq, sweepwidth, startfreq, endfreq
  //A centralized routine is used so all these related variables can be kept in sync.
  //Note gCalcGraphParams must be called to fully update graph module
  vars->centfreq=cent;
  vars->sweepwidth=span;
  if (vars->sweepwidth<0)
    vars->sweepwidth=0-vars->sweepwidth;
  vars->startfreq=(util.usingF("####.######",vars->centfreq-vars->sweepwidth/2)).toFloat(); //ver114-5n
  vars->endfreq=(util.usingF("####.######",vars->centfreq+vars->sweepwidth/2)).toFloat(); //ver114-5n
  //ver115-1b deleted calc of stepfreq
  gSetXAxisRange(vars->startfreq, vars->endfreq);  //ver114-6d

}

void msagraph::GetDefaultGraphData(int axisNum, int &axisType, int &axisMin, int &axisMax)
{
  //get axis default data type and range for this mode ver116-4h
  if (vars->msaMode== modeReflection)
  {
    if (vars->primaryAxisNum==axisNum)
    {
      axisType=constGraphS11DB ; axisMin=-100 ; axisMax=0;
    }
    else
    {
      axisType=constGraphS11Ang ; axisMin=-180 ; axisMax=180;
    }
  }
  else if (vars->msaMode== modeScalarTrans)
  {
    if (vars->primaryAxisNum==axisNum)
    {
      axisType=constMagDB ; axisMin=-100 ; axisMax=0;
    }
    else
    {
      axisType=constNoGraph ; axisMin=-180 ; axisMax=180;
    }
  }
  else if (vars->msaMode== modeVectorTrans)
  {
    if (vars->primaryAxisNum==axisNum)
    {
      axisType=constMagDB ; axisMin=-100 ; axisMax=0;
    }
    else
    {
      axisType=constAngle ; axisMin=-180 ; axisMax=180;
    }
  }
  else   // "SA"
  {
    if (vars->primaryAxisNum==axisNum)
    {
      axisType=constMagDBM ; axisMin=-100 ; axisMax=0;
    }
    else
    {
      axisType=constNoGraph ; axisMin=-1 ; axisMax=0;
    }
  }
}

void msagraph::PlotDataToScreen()
{
  //heavily revised PlotDataToScreen to utilize new graphing module.
  //Initialization for a series of sweeps is done in the sweep loop. During the first scan we draw traces.
  //At the start of each subsequent scan we call gStartNextDynamicScan which prepares for the erase-and-draw.
  //At the end of each scan, if the user so selects, the screen is "refreshed", meaning that is quickly redrawn
  //from strings or "flush" segments saved during the drawing process. However, if there are more than 1200 steps,
  //the refresh is actually a complete redraw from scratch, because it is too time consuming to accumulate the large
  //strings needed for the refresh procedure. Repeatedly adding short segments to very large strings is time consuming.
  //When the scan is halted, the screen is redrawn
  //from scratch using draw commands saved from the scans. However, redrawing and refreshing does not occur
  //if we are graphing in one of the "stick" modes. At the end of each sweep we also figure out where the markers go,
  //draw them, and display their info below the graph. The user has the option not to show the markers on the
  //graph, in which case their info is still displayed. Data for the first, last and center points is displayed
  //below the frequency axis (no center point for log sweeps), but those are not considered "markers".
  //Data values, and their pixel coordinates, are saved in the
  //graphing module. The same values are also saved in the arrays used in software versions prior to the graphing module.
  //Pixel coordinates are no longer saved in those pre-existing arrays, but the slots where the pixels were saved
  //still exist.
  //Trace segments are discarded at the end of a scan, and also every time 1000 have accumulated. Traces are not
  //"flushed" until the graph is redrawn upon halting.


  CalcGraphData(vars->thisstep, thispointy1, thispointy2, 0);   //0 means use regular data arrays ver114-7f
  if (referenceDoMath==2) //ref math is to be done on graph values ver115-5d
  {
    if (referenceTrace & 1)
      thispointy1=referenceOpA*referenceTransform[vars->thisstep+1][1]+referenceOpB*thispointy1;
    if (referenceTrace & 2)
      thispointy2=referenceOpA*referenceTransform[vars->thisstep+1][2]+referenceOpB*thispointy2;
  }
  //We draw a point after erasing a point (if required).
  //We also saves the data to gGraphVal() and the pixel values to gGraphPix()
  //X values have been precalculated in gGenerateXValues
  if (vars->thisstep==vars->sweepStartStep)
  {
    //ver114-6e added the refreshXXX flags
    refreshForceRefresh=0;
    //We normally want refreshGridDirty=0 because we redrew the grid when restarting. But
    //for mulitscans, that step is skipped.
    refreshGridDirty=vars->multiscanInProgress;
    refreshTracesDirty=0;
    refreshAutoScale=0;
    refreshRedrawFromScratch=0;
    if (firstScan)
    {
      refreshMarkersDirty=1;   //Signals to relocate any markers to correspond to their frequency.
      //If required, do auto scaling of axes at end of first scan
      if (autoScaleY2 || autoScaleY1)
        refreshAutoScale=1;
    }
    else
    {
      gStartNextDynamicScan();    //Initializes for erase/draw passes; done at start of scans 2...
    }

    //ver114-5c moved setting of isStickMode to UpdateGraphParams
  }

  //useExpeditedDraw is set to 1 for single trace (mag) in non-histo mode for linear y scaling
  //The expedited draw procedure is slightly faster than normal drawing, and will almost always be
  //used when in SA mode.
  int useExpeditedDraw = gCanUseExpeditedDraw();
  if (useExpeditedDraw)
  {
    gDrawSingleTrace();    //Normal SA drawing occurs with this
  }
  else
  {
    //Enter new Y values and draw from last point
    //Comment out all but one; two choices allowed for testing
    gDynamicComboDrawPoint(thispointy1,thispointy2);     //This is the full-blown drawing procedure
  }
  if (vars->steps>=1000)
  {
    //Discard at least every 1000 points to avoid a slowdown.
    //oneThousandthThisStep=vars->thisstep/1000;
    //if (int(oneThousandthThisStep)==oneThousandthThisStep) then #graphBox$, "discard"
  }
  //ver114-4e deleted drawing of point values at start, center and end
  if (vars->thisstep == vars->sweepEndStep) //just processed final point of a sweep //ver114-4k
  {
    //If autoscale is on for either axis then calculate the scale and redraw from raw values
    //We only do this for the first scan. ver114-7a added this autoscale material
    if (firstScan && (autoScaleY2 || autoScaleY1))
    {
      if (haltAtEnd==1) //Halt will redraw, so we just autoscale here
      {
        PerformAutoScale();   //Recalculates scaling in graph module
      }
      else
      {
        refreshForceRefresh=1;
        refreshAutoScale=1;
      }
    }
    if (refreshForceRefresh || refreshEachScan)
    {
      //force full refresh unless we are in stick mode; and don't redraw if haltAtEnd=1 because
      //a redraw will occur when we halt.
      if (vars->isStickMode==0 && haltAtEnd==0)
        RefreshGraph(1);
    }
    else
    {//modver116-4b
      //Even if we don't do a full refresh, we refresh the grid lines and traces except on first scan.
      //This restores any grid lines that got erased
      //If we draw markers, their movement from scan to scan can make a mess.
      //If we don't draw them, the grid lines will cross them.
      //So we skip this altogether if we have markers; we could do it if we had a way to erase markers.
      int partialRefresh;
      if (firstScan==0 && vars->isStickMode==0 && haltAtEnd==0)
      {
        if (doGraphMarkers)
          gEraseMarkers();   //Erase prior markers before they move
        gRefreshGridLinesOnly();
        gRefreshTraces();
        partialRefresh=1;
      }
      else
      {
        partialRefresh=0;
      }
      if (firstScan==1)
        gDetermineMarkerPointNumbers(); //Finds new point numbers for old markers if freq changed
      //Update marker info at end of every scan no matter what
      if (hasAnyMark==1)
        mDrawMarkerInfo(); //also updates marker locations

      //If no refresh, draw markers on first scan only, and only if user wants them drawn
      if ((firstScan==1 || partialRefresh) && doGraphMarkers==1)
        gDrawMarkers();
      //#graphBox$, "discard"    //Get rid of marker draw commands in memory
    }
    if ((vars->doCycleTraceColors==1) && (vars->isStickMode==1))  //ver116-4s  cycleTrace colors if necessary
    {
      vars->cycleNumber=vars->cycleNumber+1;
      if (vars->cycleNumber>3)
        vars->cycleNumber=1;
      gridappearance->gSetTraceColors(gridappearance->cycleColorsAxis1[vars->cycleNumber], gridappearance->cycleColorsAxis2[vars->cycleNumber]);  //ver116-4s
    }
    firstScan=0;  //scan has ended; next point is not in first scan since restart
    vars->doSpecialRandom=(rand() % 100) / 100.0;  //Random number for doSpecialGraph ver115-1b
  }

  updatevar(vars->thisstep); //moved here from [ProcessAndPrint]
}

void msagraph::mMarkToCenter()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub mMarkToCenter btn$ 'Recenter around marker frequency
    if selMarkerID$="" then
        notice "No marker is selected."
        exit sub  'No marker selected so nothing to do
    end if
    'Get frequency from from marker number from marker ID
    newCenterFreq=gMarkerCurrXVal(mMarkerNum(selMarkerID$))
    newCenterFreq=int(1000000*newCenterFreq+0.5)/1000000 'Round to nearest Hz.
    call SetCenterSpanFreq newCenterFreq, sweepwidth   'Set new center with old span; also updates startfreq and endfreq
    if haltsweep=1 then
        continueCode=3 'Forces restart if in midsweep.
    else
        call RequireRestart
    end if
end sub
*/
}

void msagraph::Expand()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub Expand  'Expand L to R markers to be the bounds of the sweep. Called from [menuExpandSweep]
    if hasMarkL=0 and hasMarkR=0 then exit sub   'No L or R; nothing to do.
    if hasMarkL=1 then
        newLowerFreq=gMarkerCurrXVal(mMarkerNum("L"))   'Get L frequency
    else
        newLowerFreq=startfreq
    end if
    if hasMarkR=1 then
        newUpperFreq=gMarkerCurrXVal(mMarkerNum("R"))   'Get R frequency
    else
        call gGetPointVal globalSteps+1, newUpperFreq, dum1, dum2  'Use final frequency ver114-7d
    end if
    'Note that when the first scan is completed, the marker point numbers will be
    'redetermined from their current frequencies, so L and R will be moved to the
    'edges of the scan.
    call SetStartStopFreq newLowerFreq, newUpperFreq    'Enter new frequencies in globals
end sub
*/
}
void msagraph::RedrawGraph(int restoreErase)
{
  //Redraw all layers of the graph
  //Redraw entire graph from scratch, using the point values previously accumulated.

  refreshRedrawFromScratch=1;
  RefreshGraph(restoreErase);
}

void msagraph::gSetMarkerNum(int markNum, int pointNum, QString ID, QString trace, QString style)
{
  //set marker by position in the list
  //Enter marker data and update gNumMarkers to have the max entryNum of any entered marker
  if (markNum<1 || markNum>20)
  {
    QMessageBox::about(0,"", "Invalid marker number");
    return;
  } //for debugging
  gMarkerPoints[markNum][0]=pointNum;
  gMarkerPoints[markNum][1]=gGetPointXVal(pointNum); //Copy current x value (freq)
  gMarkers[markNum][0]=ID;
  gMarkers[markNum][1]=trace;
  gMarkers[markNum][2]=style;
  if (markNum>gNumMarkers)
    gNumMarkers=markNum;
}
void msagraph::ResizeGraphHandler()
{
  //Called when graph window resizes

  int CenterX = graphicsView->width() / 2;
  int CenterY = graphicsView->height() / 2;

  currGraphBoxWidth = CenterX * 2-1;
  currGraphBoxHeight = CenterY * 2-1;

  //Note: On resizing, all non-buttons seem to end up a few pixels higher than the original spec,
  //so the Y locations are adjusted accordingly via markTop
  //Note WindowHeight when window is created is entire height; on resizing, it is the client area only
  //int markTop=currGraphBoxHeight+15;
  //int markSelLeft=5;
  //int markEditLeft=markSelLeft+55;
  //int markMiscLeft=markEditLeft+185;
  //int configLeft=markMiscLeft+80;


  gUpdateGraphObject(currGraphBoxWidth, currGraphBoxHeight
                     , graphMarLeft, graphMarRight, graphMarTop, graphMarBot);
  graphicsView->fitInView(getScene()->sceneRect());
  gCalcGraphParams();   //Calculate new scaling. May change min or max.
  float xMin, xMax;
  gGetXAxisRange(xMin, xMax);
  if (vars->startfreq!=xMin || vars->endfreq!=xMax)
    SetStartStopFreq(xMin, xMax);

  gGenerateXValues(gPointCount()); //recreate x values and x pixel locations; keep same number of points
  gRecalcPix(0);   //0 signals not to recalc x pixel coords, which we just did in gGenerateXValues.
  //If a sweep is in progress, we don't want to redraw from here, because that can cause a crash.
  //So we just clear the graph and signal to wait for the user to redraw. This crash may have to
  //do with the fact that we don't know where we are in the sweep process when resizing is invoked,
  //because it is not synchronous with the scan command. Or it may simply have something to do with
  //the fact that no button has yet been pushed on the graph window, which somehow affects the
  //LB resizing process. The crash still sometimes occurs, so it is best to halt before resizing.
  if (haltsweep==1)
  {
    RequireRestart();
  }
  else
  {
    refreshRedrawFromScratch=1; //To redraw from scratch ver115-1b
    RedrawGraph(0);  //Redraw at new size
  }
}

void msagraph::RememberState()
{

  //Record state before loading a context, to allow detection of certain changes
  //We want to record enough to determine whether we have to restart, do a full redraw or just
  //refresh and redraw setup info.
  //Some of these changes can be made during a scan without an immediate redraw, but when loading
  //a context we will force a halt.

  //Change to msaMode$ may reqire a new graph window
  vars->prevMSAMode=vars->msaMode;

    //Changes to these will require a complete Restart
  vars->prevFreqMode=vars->freqBand;
  vars->prevPath=vars->path;
  gGetXAxisRange(vars->prevStartF, vars->prevEndF);
  vars->prevBaseF=vars->baseFrequency;
  gGetIsLinear(vars->prevXIsLinear, vars->prevY1IsLinear, vars->prevY2IsLinear);
  vars->prevSteps=vars->globalSteps;
  vars->prevSweepDir=gGetSweepDir();
  vars->prevAlternate=vars->alternateSweep;
  vars->prevGenTrk=vars->gentrk ;
  vars->prevSpurCheck=vars->spurcheck; //ver114-6k
  vars->prevTGOff=vars->offset ;
  vars->prevSGFreq=vars->sgout; //ver115-1a
  vars->prevPath=vars->path; //ver115-1a

    //Changes to these will require calling gCalcGraphParams and then a full Redraw
  gGetYAxisRange(1, vars->prevStartY1, vars->prevEndY1);
  gGetYAxisRange(2, vars->prevStartY2, vars->prevEndY2);
  gGetNumDivisions(vars->prevHorDiv, vars->prevVertDiv);
  vars->prevSwitchFR=vars->switchFR;   //forward/reverse switch affects axis labeling ver116-1b

    //Changes to graph data type will require a replotting to get the proper transform,
    //and redrawing from scratch to get axis labeling correct.
  vars->prevY1DataType=vars->Y1DataType;  //ver115-1b deleted source constants
  vars->prevY2DataType=vars->Y2DataType;

  //If in a mode where these are relevant, changes to the following are treated
  //the same as a change to the data source.   //ver114-7e added these
  vars->prevS21JigAttach=vnaCal->S21JigAttach;
  vars->prevS21JigR0=vnaCal->S21JigR0;   //ver115-1e
  vars->prevS21JigShuntDelay=vnaCal->S21JigShuntDelay;   //ver115-1f
  vars->prevS11BridgeR0=vnaCal->S11BridgeR0;
  vars->prevS11GraphR0=vnaCal->S11GraphR0;
  vars->prevS11JigType=vnaCal->S11JigType; //ver115-1b

    //Changes to these require a full Redraw from existing Y values
    //Note this refers to magtable data, not graph data
  vars->prevDataChanged=0;   //If changed, this has to be set elsewhere

     //Changes to these may require redraw from scratch or just normal refresh
  vars->prevY1Disp=vars->Y1DisplayMode;
  vars->prevY2Disp=vars->Y2DisplayMode;

            //If auto scale gets turned on, we will signal to do autoscale at end of sweep,
            //or do it immediately if we are halted.
  vars->prevAutoScaleY1=autoScaleY1;
  vars->prevAutoScaleY2=autoScaleY2;
    //Changing planeadj could require immediate recalc of existing data
  vars->prevPlaneAdj=vars->planeadj;   //ver114-7f
    //If none of the above are changed, we will do a RefreshGraph with refreshGridDirty=1
  //Items that reach here include;
  //Marker Options, Graph Appearance, trace width and trace color
  //RBW, video filter, SigGen freq, TG offset, spur test, wait time and plane extension

}


void msagraph::ToggleTransmissionReflection()
{
  //Load and graph reflection data if now in transmission, or transmission data if now in reflection
  //This is used in full VNA modes only, for switching between reflection and transmission. They should have been
  //scanned with the same parameters, because we don't restore any parameters here. For example, they must have the
  //same number of steps. It is actually possible for the frequency ranges to differ, but that is not how this would
  //normally be used.
  //This will not disrupt the current data, which can be displayed by calling here again.
  //Switching between VectorTrans and Reflection modes is specially handled, to preserve the data and Y-axis
  //settings that were last in effect for the new mode. This special treatment is done for menu-driven change
  //or by certain internally generated changes that call [ToggleTransmissionReflection], but not for changes
  //resulting from loading of preference files.   ver116-1b



  int restoreSettingsAfterChange=0;
  if (vars->menuMode==modeVectorTrans)  //menuMode$ has prior mode ver116-1b
  {
    //If changing from vector trans mode and sweep frequencies are the same, we preserve some settings
    if (vars->refLastSteps!=0 && vars->refLastSteps==vars->steps && vars->refLastStartFreq==vars->startfreq
        && vars->refLastEndFreq==vars->endfreq && vars->refLastIsLinear==gGetXIsLinear()) restoreSettingsAfterChange=1;
  }
  else    //changing from reflection to transmission
  {
    if (vars->transLastSteps!=0 && vars->transLastSteps==vars->steps && vars->transLastStartFreq==vars->startfreq
        && vars->transLastEndFreq==vars->endfreq && vars->transLastIsLinear==gGetXIsLinear()) restoreSettingsAfterChange=1;
  }

  if (vars->msaMode==modeReflection) vars->msaMode=modeVectorTrans; else vars->msaMode=modeReflection; //switch mode
  ChangeMode();  //Conform to new mode
  if (restoreSettingsAfterChange==0)
  {
    return;
  }

  if (vars->msaMode==modeReflection)    //These changes will be fully implemented at PartialRestart
  {
    SetYAxes(vars->refLastY1Type, vars->refLastY1Top, vars->refLastY1Bot, vars->refLastY1AutoScale,
            vars->refLastY2Type, vars->refLastY2Top, vars->refLastY2Bot, vars->refLastY1AutoScale);
  }
  else
  {
    SetYAxes(vars->transLastY1Type, vars->transLastY1Top, vars->transLastY1Bot, vars->transLastY1AutoScale,
            vars->transLastY2Type, vars->transLastY2Top, vars->transLastY2Bot, vars->transLastY2AutoScale);
  }

  //put data to be restored into VNAData.

  //Note we don't have to worry about ResizeArrays, because any resizing would have been
  //done when number of steps was set to its current value. Note also that we only restore the data
  //if the old data had the same sweep settings as the current settings.

  //We want to apply any graph R0 transform and plane extension, if applicable, so we indicate to copy
  //the "intermediate" values.
  hwdIf->CopyModeDataToVNAData(1);  //Copy data to be restored to VNAData
  vars->VNARestoreDoR0AndPlaneExt=1;
  // fix me hwdIf->RestoreVNAData();   //load and graph the data ver116-4j

  vars->VNAData.mresize(2,3);  //save space
  vars->VNADataNumSteps=1;
  util.beep();

}
int msagraph::mMarkerNum(QString markID)
{
  //Return ordinal marker number for this marker ID$
  //This value corresponds to the entry in the list of markers (1...)
  //-1 for invalid ID$. This is used to convert meaningful ID//s into
  //arbitrary storage locations. When marker info is printed, it is
  //printed in the same order as the ordinal marker numbers. This is the only place
  //that ID//s are tied to specific ordinals, to make it easy to change.
  if (markID == "Halt")       //ver114-4c added Halt and renumbered
        return 1;
  else if (markID == "L")
        return 2;
  else if (markID == "R")
        return 3;
  else if (markID == "P+")
        return 4;
  else if (markID == "P-")
        return 5;
  else if (markID == "1")
        return 6;
  else if (markID == "2")
        return 7;
  else if (markID == "3")
        return 8;
  else if (markID == "4")
        return 9;
  else if (markID == "5")
        return 10;
  else if (markID == "6")
        return 11;
  else
  {
        return -1;
  }
}
void msagraph::mDeleteMarker(QString markID)
{
  if (markID == "L")
  {

  }
  else if (markID == "R")
  {

  }
  else if (markID == "P+")
  {

  }
  else if (markID == "P-")
  {

  }
  else if (markID == "1" || markID == "2" || markID == "3" ||
           markID == "4" || markID == "5" || markID == "6")
  {

  }
  else if (markID == "Halt")
  {
    if (haltMarker)
    {
      graphScene->removeItem(haltMarker);
      delete haltMarker;
    }
    haltMarker = NULL;
  }
  else
  {
    return;
  }
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  markNum=mMarkerNum(markID$) :
if markNum<1 then notice "Invalid Marker Number" : exit sub
   call gUpdateMarkerPointNum markNum,-1
       //Update the flags indicating whether we have the special markers
   select case markID$
       case "L"
           hasMarkL=0
       case "R"
           hasMarkR=0
       case "P+"
           hasMarkPeakPos=0
       case "P-"
           hasMarkPeakNeg=0
       case "1", "2","3","4","5", "6", "Halt"  //ver114-4c
           //valid markers but nothing special to do
       case else
           exit sub    //Not valid marker ID
   end select
   if gValidMarkerCount>0 then hasAnyMark=1 else hasAnyMark=0
   if markID$=selMarkerID$ then
       call mMarkSelect ""  //ver114-5L
   end if
      */
}
void msagraph::mAddMarker(QString markID, int pointNum, QString trace)
{

  //Add specified marker at specified point
  QString markTrace=trace;
  QString markStyle="LabeledWedge";
  int markNum=mMarkerNum(markID);
  if (markNum<0)
  {
    QMessageBox::warning(0,"Error", "Invalid Marker Number"); return;
  }
  if (pointNum<0)
  {
    mDeleteMarker(markID);
    return;
  }    //Adding with point num <0 is deleting
  if (markID == "L")
  {
    hasMarkL=1;
  }
  else if (markID == "R")
  {
    hasMarkR=1;
  }
  else if (markID == "P+")
  {
    hasMarkPeakPos=1;
    markTrace=QString::number(vars->primaryAxisNum);   //Always do peak markers on primary trace
  }
  else if (markID == "P-")
  {
    hasMarkPeakNeg=1;
    markTrace=QString::number(vars->primaryAxisNum);   //Always do peak markers on primary trace
    markStyle="LabeledInvertedWedge";
  }
  else if (markID == "Halt")
  {
        markTrace="Xaxis";
        markStyle="HaltPointer";
  }
  else if (markID == "1" || markID == "2" || markID == "3" || markID == "4" || markID == "5" || markID == "6")
  {
        //valid markers but nothing special to do
  }
  else
  {
    return;    //not valid marker
  }

  hasAnyMark=1;    //Indicate that we have at least one marker
  gSetMarkerNum(markNum, pointNum, markID, markTrace, markStyle);
  //Note we do not change the selected marker here
}

void msagraph::mAddMarkerAndRedraw(QString markID, int ptNum, int traceNum)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub mAddMarkerAndRedraw markID$,ptNum, traceNum
    call mAddMarker markID$,ptNum, str$(traceNum)
    call mMarkSelect markID$  'Select the marker we added ver114-5L
    if twoPortWinHndl$="" then
        call RefreshGraph 0     'ver114-7d
        'ver116-4j made calling [preupdatevar] responsibility of caller
    else
        call TwoPortDrawGraph 0
    end if
end sub
*/
}

void msagraph::mAddMarkerFromKeyboard()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[mAddMarkerFromKeyboard]  'Add marker when marker key was pressed ver116-4j
    if haltsweep=1 then gosub [FinishSweeping]
    key$=Inkey$ 'This is the key that was pressed
    if isStickMode then wait    'Don't do marker in stick modes
    key$=upper$(key$)   'to upper case
    if len(key$)<>1 or inStr("LR123456", key$)=0 then wait  'exit if invalid marker
    call gGetLastMouseQuery queryPt, queryTrace 'point and trace number of last mouse position
        'erase prior slider markers
    dum$=gDrawMarkerAtPointNum$(queryPt,"ALL","XOR","")    'Erase prior box
    if smithGraphHndl$()<>"" then call smithDrawSliderMarker queryPt   'erase in Smith chart too
    call mAddMarkerAndRedraw key$, queryPt, queryTrace  'record new marker and redraw, which clears the query info
    if twoPortWinHndl$="" and varwindow = 1 then leftstep=int(queryPt+0.5)-1 : gosub [preupdatevar] 'will update variables at marker point ver116-4j
wait
*/
}
void msagraph::mClearMarkers()
{
  hasMarkL=0;
  hasMarkR=0;
  hasMarkPeakPos=0;
  hasMarkPeakNeg=0;
  hasAnyMark=0;
  gClearMarkers();
  gDrawMarkerInfo();    //to clear info area ver114-7n
  // fix me mMarkSelect("");  //ver114-5L
}

void msagraph::mUserMarkSelect(QString btn)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  //Marker was selected in combobox.
      if twoPortWinHndl$="" then #handle.selMark, "selection? selMarkerID$"  _
                      else #twoPortWin.selMark, "selection? selMarkerID$" //ver116-2a
      if selMarkerID$="None" then selMarkerID$=""
          //Show marker editing info; ver114-4a revised
      call mUpdateMarkerEditButtons    //Enable/disable proper buttons
      call mDisplaySelectedMarker  //Display numeric info
          //Update Smith chart. But if sweep is in progress don't, because we may have
          //been called by program action. If sweeping, it will be updated at refresh time.
      if haltsweep=0 and smithGraphHndl$()<>"" then call smithRefreshMarkerInfo   //ver115-2c
  end sub
*/
}
void msagraph::mEnterMarker(QString btn)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub mEnterMarker btn$    'Marker Enter button was clicked
    'Enter new marker info based on the frequency. If the point num was changed,
    'the frequency was updated, but the user may have changed the frequency so
    'the point number may be outdated.
    if twoPortWinHndl$="" then  'ver116-4a
        #handle.markFreq, "!contents? markFreq$"    'get frequency from proper box
    else
        #twoPortWin.markFreq, "!contents? markFreq$"
    end if
    markFreq=val(markFreq$)
    markPoint=gPointNumOfX(markFreq)   'Find point number matching this frequency.
    if msaMode$="SA" then markPoint=int(0.5+markPoint)  'Round to integral point in SA mode ver115-2d
    if markPoint <1 then markPoint=1
    maxPoint=gPointCount()      'ver114-4a
    if markPoint>maxPoint then markPoint=maxPoint
    markFreq=gGetPointXVal(markPoint)
    if twoPortWinHndl$="" then  'ver116-2a
        print #handle.markFreq, using("####.######", markFreq)  'Enter frequency into box, in case we changed it ver116-2a
        call gUpdateMarkerPointNum mMarkerNum(selMarkerID$), markPoint
        if doGraphMarkers=1 then call RefreshGraph 0 else call mDrawMarkerInfo 'ver114-7d
    else
        print #twoPortWin.markFreq, using("####.######", markFreq)  'Enter frequency into box, in case we changed it ver116-2a
        call gUpdateMarkerPointNum mMarkerNum(selMarkerID$), markPoint
        call TwoPortDrawGraph 0   'ver116-2a
    end if
end sub
*/
}

void msagraph::mDisplaySelectedMarker()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub mDisplaySelectedMarker    'Update label, point number and freq for selected marker
    'ver114-5L deleted actual selection of marker in combobox, which is usually not necessary
    if selMarkerID$="" then #handle.markFreq, "" : exit sub    'No marker to update ver114-4a
    markPoint=gMarkerPointNum(mMarkerNum(selMarkerID$))
    if markPoint<0 then  #handle.markFreq, "" : exit sub    'No marker to update ver114-4a
    markFreq=gGetPointXVal(markPoint)
    if twoPortWinHndl$="" then #handle.markFreq, using("####.######",markFreq) _
                else #twoPortWin.markFreq, using("####.######",markFreq)   'ver116-2a

        'If we have reference lines, other than fixed value, print their values as a message
    if twoPortWinHndl$="" and referenceLineType>0 and referenceLineType<>3 then    'ver115-6d added display of reference values
        message$="Marker ";selMarkerID$;":   "
        call DetermineGraphDataFormat Y1DataType, axisLab1$, dataLab1$,isPhase1,y1Form$
        call DetermineGraphDataFormat Y2DataType, axisLab2$, dataLab2$,isPhase2,y2Form$
        call CalcReferences markPoint-1, isPhase1, ref1, isPhase2, ref2
        if (Y1DataType<>constNoGraph) and (referenceTrace and 1) then   'Do reference for Y1
            message$=message$; dataLab1$;" Ref=";uFormatted$(ref1, y1Form$);"     "
        end if
        if (Y2DataType<>constNoGraph) and (referenceTrace and 2)=2 then   'Do reference for Y2
            message$=message$; dataLab2$;" Ref=";uFormatted$(ref2, y2Form$);"  "
        end if
        call PrintMessage
    end if
end sub
*/
}

QString msagraph::mMarkerContext()
{

  //Return marker context as string
  //successive lines are separated by chr$(13)
  QString newLine="\r";
  QString s=",";  //comma to separate items
  QString j="";
  QString s1="";
  for (int i=0; i < 9; i++)
  {
    QString id=markerIDs[i];
    int markNum=mMarkerNum(id);
    int pointNum;
    QString trace, style;
    gGetMarkerByNum(markNum, pointNum, id, trace, style);
    if (pointNum>0)
    {
        s1=s1+j+"Marker=" + id+s+ QString::number(pointNum)+ s+ trace;     //Marker id, point number and trace
        j=newLine;
    }
  }
  s1= s1+j+"Selected="+selMarkerID;
  j=newLine;
  s1= s1+newLine+"DoGraph="+QString::number(doGraphMarkers);
  s1= s1+newLine+"LROptions="+QString::number(doPeaksBounded)+s+doLRRelativeTo
      +s+QString::number(doLRRelativeAmount)+s+QString::number(doLRAbsolute); //ver115-3f
  s1= s1+newLine+"FiltAnalysis="+QString::number(vars->doFilterAnalysis)+s+QString::number(vars->x1DBDown)+s+QString::number(vars->x2DBDown);
  return s1;

}
QString msagraph::mRestoreMarkerContext(QString s, int &startPos, int isValidation)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function mRestoreMarkerContext$(s$, byref startPos, isValidation)  'Restore marker info from context string
    'Returns error message if error; otherwise 0. Ignores data prior to startPos.
    'startPos is updated to one past the last line we process (normally EndContext or end of string)
    'if isValidation=1, we merely check for errors
    'sep$ is a possibly multi-character separator to delimit text items on one line
    'successive lines are separated by chr$(13)
    call mClearMarkers  'delete existing markers ver116-2a
    newLine$=chr$(13)
    nonTextDelims$=" ," + chr$(9)    'space, comma and tab are delimiters
        'Get next line and increment startPos to start of the following line
    oldStartPos=startPos
    tLine$=uGetLine$(s$, startPos)  'Get line of data
    oldStartPos=startPos
    while tLine$<>""
        origLine$=tLine$ 'ver115-1b
        if Upper$(Left$(tLine$,10))="ENDCONTEXT" then exit while
        isErr=0
        equalPos=instr(tLine$,"=")     'equal sign marks end of tag
        if equalPos=0 then
            isErr=1 : exit while
        else
            tag$=Upper$(Left$(tLine$, equalPos-1))  'tag is stuff before equal sign
            tLine$=Trim$(Mid$(tLine$, equalPos+1))  'contents is stuff after equal sign
        end if
        select case tag$
            case "MARKER"
                if isValidation=0 then _
                    call mAddMarker Word$(tLine$,1,","),val(Word$(tLine$,2,",")),Word$(tLine$,3,",")  'id, point num, trace
            case "SELECTED"
                if isValidation=0 then selMarkerID$=tLine$
            case "DOGRAPH"
                if isValidation=0 then doGraphMarkers=val(tLine$)
            case "LROPTIONS"
                if isValidation=0 then 'modver115-3f
                    isErr=uExtractNumericItems(1,tLine$, nonTextDelims$,v1, v2, v3)
                    doPeaksBounded=v1
                    doLRRelativeTo$=uExtractTextItem$(tLine$,",")   'ver116-2a
                    isErr=uExtractNumericItems(2,tLine$, nonTextDelims$,v1, v2, v3)
                    doLRRelativeAmount=v1
                    doLRAbsolute=v2
                end if
            case "FILTANALYSIS"
                if isValidation=0 then  'modver115-3f
                    isErr=uExtractNumericItems(1,tLine$, nonTextDelims$,v1, v2, v3)
                    doFilterAnalysis=v1
                    x1DBDown=v2
                    x2DBDown=v3
                end if
            case else
                'Unrecognized tag. Must belong to the graph module, so we end here
                startPos=oldStartPos    'reset to beginning of this line
                exit while
        end select
        if isErr then mRestoreMarkerContext$="Marker Context Error in: "; origLine$: exit function  'ver115-1b
        'Get next line and increment startPos to start of the following line
        oldStartPos=startPos
        tLine$=uGetLine$(s$, startPos)
    wend
    mRestoreMarkerContext$=""
    call mMarkSelect selMarkerID$ 'To display info  ver114-5L
end function
*/
  return "fix me";
}
void msagraph::DetectChanges()
{
  //Take appropriate redraw/restart action in response to certain changes
  //This is a gosub routine so it can access [ChangeMode]. This needs to be called only if msaMode$ may have changed;
  //otherwise call DetectChanges directly.
  int doRestart=0;
  if (vars->prevMSAMode != vars->msaMode)
  {
    ChangeMode();
    doRestart=1;
  }
  DetectChanges(doRestart);
}

void msagraph::DetectChanges(int doRestart)
{

  //ver114-6e added DetectChanges; modver114-7f to move mode change detection to gosub routine
  //Take appropriate redraw/restart action in response to certain changes
  //We want to record enough to determine whether we have to restart, do a full redraw or just
  //refresh and redraw setup info. If doRestart=1, we will specify a restart no matter what.
  //Some of these changes can be made during a scan without an immediate redraw, but when loading
  //a context we will force a halt.
  //We set continueCode to specify the user action required; 0=continue; 2=wait; 3=restart

  //TO DO--The concept has been to take the minimum action necessary, primarily for speed, but also to
  //preserve existing data when possible. Now that we have [PartialRestart], it might make more sense
  //to require [PartialRestart] whenever there is any doubt, and to zero out the data when it no longer
  //makes sense. The caller can then do [PartialRestart] and regraph the data. In fact, we could change
  //this subroutine to a gosub, and take all necessary action here. But it may be better for the user to do
  //the restart, in case a series of changes are being made--the restart can be done at the end of the series.
  //Some things we do here, such as recalculating graph params, would become unnecessary.

  continueCode=0;  //Assume we can continue when we are done here
  int doCalcAndRedraw=0;
  int doTransform=0;
  int dataTypeChanged=0;

  //For certain switch settings, we don't bother to determine whether a change was made; we just
  //implement the current setting.
  hwdIf->SelectVideoFilter();  //Also makes auto wait calculations if necessary ver116-1b
  hwdIf->SelectLatchedSwitches(vars->lastSetBand);  //transmit/reflect, forward/reverse; leaves band as it is (handled by restart)  //ver116-1b ver116-4s
  if (vars->calCanUseAutoWait==0 && vars->useAutoWait)
  {
    //If the path calibration had insufficient or bad data, auto wait was disabled when loading
    //the file. If preferences are loaded specifying to use it, we need to override that.
    vars->useAutoWait=0;
    vars->wate=100;
  }

  //Changes to graph data type will require recalculating grid labels with new format,
  //recalculating the transform and then
  //redrawing from scratch to get axis labeling correct.
  if (vars->prevY1DataType!=vars->Y1DataType || vars->prevY2DataType!=vars->Y2DataType)
  {
    gSetDoAxis ((vars->Y1DataType!=constNoGraph), (vars->Y2DataType!=constNoGraph));  //Tell graph module whether we have graphs ver115-2c
    ImplementDisplayModes();
    doCalcAndRedraw=1;
    dataTypeChanged=1;
    doTransform=1;
  }

  //Changes to these will require a complete Restart
  float currStartF, currEndF;
  gGetXAxisRange(currStartF, currEndF);
  int currXIsLinear, currY1IsLinear, currY2IsLinear;
  gGetIsLinear(currXIsLinear, currY1IsLinear, currY2IsLinear);
  if (vars->freqBand!=vars->prevFreqMode || currStartF!=vars->prevStartF
      || currEndF!=vars->prevEndF || vars->prevSteps!=vars->globalSteps || vars->baseFrequency!=vars->prevBaseF)
  {
    doRestart=1;
  }
  if (currXIsLinear!=vars->prevXIsLinear || currY1IsLinear!=vars->prevY1IsLinear
      || currY2IsLinear!=vars->prevY2IsLinear)
  {
    doRestart=1;
  }
  if (vars->prevGenTrk!=vars->gentrk || vars->prevSpurCheck!=vars->spurcheck)
  {
    doRestart=1;
  }
  if (vars->prevTGOff!=vars->offset || vars->prevSGFreq!=vars->sgout)
  {
    doRestart=1;
  }
  int filtbank = 0;
  if (vars->prevPath!=vars->path)
  {
    hwdIf->SelectFilter(filtbank);
    doRestart=1;
  }
  if (doRestart)
  {
    //If we are using fixed data as the reference source and any of the above changed,
    //we can no longer use it.
    if (referenceLineType==1)
    {
      referenceLineType=0 ;
      referenceTrace=0 ;
      gClearAllReferences();
    }
  }

  if (vars->prevSweepDir!=gGetSweepDir() || vars->prevAlternate!=vars->alternateSweep)
  {
    doRestart=1;
  }
  if (vars->prevPath!=vars->path)
  {
    doRestart=1;
  }
  //If in a mode where these are relevant, we need to require Restart,
  //but do not do anything to the existing data. Note that the calibration
  //will be invalidated elsewhere in these cases, so there is no meaningful way
  //to transform the data.
  if (vars->msaMode==modeReflection)
  {
    if (vars->prevS21JigAttach!=vnaCal->S21JigAttach) doRestart=1;
    if (vars->prevS21JigR0!=vnaCal->S21JigR0) doRestart=1;
    if (vars->prevS21JigShuntDelay!=vnaCal->S21JigShuntDelay) doRestart=1;
    if (vars->prevS11JigType!=vnaCal->S11JigType || vars->prevS11BridgeR0!=vnaCal->S11BridgeR0)
    {
      doRestart=1;
    }
  }

  if (doRestart)
  {
    //If the data changed, we should redraw it before we exit. We assume the frequency
    //points of the data are consistent with any changes to the graph module frequency points.
    //If the data did not change, we may not be able to redraw it with the new sweep parameters so
    //we don't try.
    continueCode=3;
    if (vars->prevDataChanged==0)
    {
      return;
    }
    gGenerateXValues(gPointCount());  //Make sure x values correspond to sweep parameters
    //For RLC or fixed value reference, recalculate reference data
    if (referenceLineType>1)
    {
      CreateReferenceSource();
      CreateReferenceTransform();
    }
    doCalcAndRedraw=1;
    doTransform=1;
  }

  //Changes to these will require calling gCalcGraphParams and then a full Redraw from existing Y values
  int rescaleReferences=0;
  int currStartY1, currEndY1;
  int currStartY2, currEndY2;
  int currHorDiv, currVertDiv;
  gGetYAxisRange(1, currStartY1, currEndY1);
  gGetYAxisRange(2, currStartY2, currEndY2);
  gGetNumDivisions(currHorDiv, currVertDiv);
  if (vars->prevSwitchFR!=vars->switchFR)
    doCalcAndRedraw=1;    //Forward/Reverse switch affects axis labels ver116-1b
  if (currStartY1!=vars->prevStartY1 || currEndY1!=vars->prevEndY1)
  {
    doCalcAndRedraw=1 ;
    rescaleReferences=1;
  }
  if (currStartY2!=vars->prevStartY2 || currEndY2!=vars->prevEndY2)
  {
    doCalcAndRedraw=1 ;
    rescaleReferences=1;
  }
  if (currHorDiv!=vars->prevHorDiv || currVertDiv!=vars->prevVertDiv)
  {
    doCalcAndRedraw=1;
  }
  if (rescaleReferences)
  {
    CreateReferenceTransform();
  }
  if (doCalcAndRedraw)
  {
    if (dataTypeChanged || vars->prevSwitchFR!=vars->switchFR)
    {
      UpdateGraphDataFormat(0);  //To get new data format ver116-1b
    }
    gCalcGraphParams();   //Calculate new scaling. May change min or max.
    float xMin, xMax;
    gGetXAxisRange(xMin, xMax);
    if (vars->startfreq!=xMin || vars->endfreq!=xMax)
    {
      SetStartStopFreq(xMin, xMax);
      continueCode=3;
    }
  }
  int doRedraw=doCalcAndRedraw;

  int autoScaleTurnedOn;
  if ((vars->prevAutoScaleY1==0 || autoScaleY1==1) || (vars->prevAutoScaleY2==0 || autoScaleY2==1))
  {
    autoScaleTurnedOn=1;
  }
  else
  {
    autoScaleTurnedOn=0;
  }
  int autoScaleIsOn;
  if (autoScaleY1==1 || autoScaleY2==1)
  {
    autoScaleIsOn=1;
  }
  else
  {
    autoScaleIsOn=0;
  }

  //If data changes, recalc x pixel values and require a restart, though proceeding will destroy the new data
  if (vars->msaMode==modeReflection && vars->prevS11GraphR0!=vnaCal->S11GraphR0)
  {
    continueCode=3 ;
    doTransform=1;
  }
  if (vars->prevDataChanged)
  {
    continueCode=3;
    doTransform=1;
  }

  if (doTransform)
  {
    UpdateGraphDataFormat(0);  //Update data formatting; may also affect data transform
    RecalcYValues();
    if (autoScaleIsOn)
    {
      PerformAutoScale();  //autoscale with the new data   //ver114-7a
      autoScaleTurnedOn=0;     //Since we just did autoscale ver114-7e
    }
    doRedraw=1;  //Signal to do complete redraw from scratch
  }

  //If auto scale has been turned on, we signal to do the autoscaling at the end of the sweep or on redraw
  if (autoScaleTurnedOn)
  {
    refreshAutoScale=1;
  }

  if (doRedraw)
  {
    RedrawGraph(0);
    return;
  }

  //Changing trace style requires recreating the traces and refreshing if it has just
  //been turned on (since we have no accumulated trace draw commands) or if it
  //was and remains on but the style changed (between histo/normal or erase/stick).
  int doRefreshTraces;
  if (vars->prevY1Disp!=vars->Y1DisplayMode && (vars->Y1DisplayMode!=0 || vars->prevY1Disp==0))
  {
    doRefreshTraces=1;
  }
  if (vars->prevY2Disp!=vars->Y2DisplayMode && (vars->Y2DisplayMode!=0 || vars->prevY2Disp==0))
  {
    doRefreshTraces=1;
  }
  if (doRefreshTraces)
  {
    refreshTracesDirty=1;
    refreshGridDirty=1;
    RefreshGraph(0);
    return;
  }

  //Everything else we redraw everything except the traces from scratch; traces
  //will be drawn from gTrace1$() and gTrace2$()
  //Marker Options, Graph Appearance, trace width and trace color will be in this category
  //Also includes primaryAxisNum, which has no immediate effects except the order of marker info
  refreshGridDirty=1;
  //We don't redraw if in stick mode, because the "stuck" traces will be erased.
  int doErasure;
  if (haltsweep==1)
  {
    doErasure=1;
  }
  else
  {
    doErasure=0;    //Do erasure gap if still sweeping
  }
  if (vars->isStickMode==0)
  {
    RefreshGraph(doErasure);
  }
}

QString msagraph::gRestoreSweepContext(QString &s, int &startPos, int isValidation) //Restore context info on sweep
{
  //Return error message if error; otherwise "". We ignore data prior to startPos.
  //successive lines are separated by chr$(13)
  //startPos is updated to one past the last line we process (normally EndContext or end of string)
  //If isValidation=1, then we just run through the data for error checking, without changing anything.
  QString newLine="\r";
  QString aSpace=" ";
  QString sep=";;"; //Used to separate text items
  QString nonTextDelims=" ," "\t";    //space, comma and tab are delimiters
  //Get next line and increment startPos to start of the following line
  QString tLine = util.uGetLine(s, startPos);
  QString tag;
  while (tLine!="")
  {
    float v1, v2, v3;
    QString origLine=tLine;
    if (tLine.left(10).toUpper() == "ENDCONTEXT")
    {
      break;
    }
    int isErr=0;
    int equalPos = tLine.indexOf("=");     //equal sign marks end of tag
    if (equalPos==-0)
    {
      tag=""; //We will skip this line; it does not have a tag
    }
    else
    {
      tag=tLine.left(equalPos).toUpper();  //tag is stuff before equal sign
      tLine=tLine.mid(equalPos+1).trimmed();  //contents is stuff after equal sign
    }
    //Each tag represents one or several data items. These are retrieved one at a time,
    //and as each is retrieved it is deleted from tLine$. Numeric items are delimited
    //by spaces, tabs or commas. Text items are delimited by the double character
    //contained in sep$, because they may contain spaces or commas. If this is just
    //a validation run, we do not enter any of the retrieved data into our variables.
    //ver114-2d rearranged
    if (tag == "MINMAXXAXIS")
    {
      isErr=util.uExtractNumericItems(2, tLine,nonTextDelims, v1, v2, v3);

      if (isErr==0 && isValidation==0)
      {
        gXAxisMin=v1;
        gXAxisMax=v2;
      }
    }
    else if (tag == "MINMAXY1AXIS")
    {
      isErr=util.uExtractNumericItems(2, tLine,nonTextDelims, v1, v2, v3);
      if (isErr==0 && isValidation==0)
      {
        gY1AxisMin=v1;
        gY1AxisMax=v2;
      }
    }
    else if (tag == "MINMAXY2AXIS")
    {
      isErr=util.uExtractNumericItems(2, tLine,nonTextDelims, v1, v2, v3);
      if (isErr==0 && isValidation==0)
      {
        gY2AxisMin=v1;
        gY2AxisMax=v2;
      }
    }
    else if (tag == "ISLINEAR")
    {
      isErr=util.uExtractNumericItems(3, tLine,nonTextDelims, v1, v2, v3);
      if (isErr==0 && isValidation==0)
      {
        gXIsLinear=v1;
        gY1IsLinear=v2;
        gY2IsLinear=v3;
      }
    }
    else if (tag == "NUMSTEPS")
    {
      isErr=util.uExtractNumericItems(1, tLine,nonTextDelims, v1, v2, v3);
      if (isErr==0 && isValidation==0)
      {
        gSetNumDynamicSteps(v1);
      }
    }
    else if (tag == "SWEEPDIR")
    {
      isErr=util.uExtractNumericItems(1, tLine,nonTextDelims, v1, v2, v3);
      if (isErr==0 && isValidation==0)
      {
        gSetSweepDir(v1);
      }
    }
    else
    {
      //Unrecognized tag. Skip it
    }
    if (isErr)
    {
      return "Sweep Context Error in: " + origLine;
    }
    //Get next line and increment startPos to start of the following line
    tLine=util.uGetLine(s, startPos);
  }
  gCalcGraphParams();   //Calculates various things derived from the above ver114-2d
  return "";
}
