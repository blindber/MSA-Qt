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

msagraph::msagraph()
{
  graphScene = new QGraphicsScene();

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



//****************************


  gGraphVal.mresize(805,3);
  gGraphPix.mresize(805,3);
  gTrace1.resize(805);
  gTrace2.resize(805);
  referenceSource.mresize(802, 3);
  referenceTransform.mresize(802, 3);
  referenceLineType = 0;
  referenceWidth1 = 0;
  referenceWidth2 = 0;

  referenceLineSpec="";
  referenceDoMath = 0;
  gMaxPoints = 0;
  referenceOpA = 0;
  referenceOpB = 0;

}

void msagraph::setGlobalVars(globalVars *newVars)
{
  vars = newVars;
  inter.setGlobalVars(newVars);
}

void msagraph::setActiveConfig(msaConfig *newActiveConfig)
{
  activeConfig = newActiveConfig;
}
void msagraph::gSetYAxisRange(int axisNum, int yMin, int yMax)
{
  //Change range of specified y axis
  //'gCalcGraphParams, which must be called before graphing.
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
  if (axisNum==1)
  {
    yMin=gY1AxisMin; yMax=gY1AxisMax;
  }
  else
  {
    yMin=gY2AxisMin; yMax=gY2AxisMax;
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
  if (gXIsLinear==0 && linX==1) gHorDiv=10;  //ver116-4a
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
    if (gXIsLinear==0) gHorDiv=10;  //ver116-4a
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
  //Set width of graph traces 'mod ver116-4b
  if (wid1<=0) gTrace1Width=0; else gTrace1Width=wid1;
  if (wid2<=0) gTrace2Width=0; else gTrace2Width=wid2;
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
  xLabel=gXAxisLabel; y1Label=gY1AxisLabel; y2Label=gY2AxisLabel;
}
QString msagraph::gGetTitleLine(int N)
{
  //Return one of 4 lines of title to print above top grid line. Numbered 1,2,3,4. Only 3 are to print
  if (N<0)
    N=0;
  else if (N>3)
    N=3;    //ver114-5m
  return gTitle[N];
}
void msagraph::gSetTitleLine(int N, QString t)
{
  //Set one of 4 lines of title to print above top grid line. Numbered 1,2,3,4
  if (N<0)
    N=0;
  else if (N>3) N=3;   //ver114-5m
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
  //ver114-1e changed so dimension is never reduced

  if (n>gMaxPoints)
  {
    gMaxPoints=n;
    gGraphVal.mresize(gMaxPoints+5,3);
    gGraphPix.mresize(gMaxPoints+5,3);
    gTrace1.resize(gMaxPoints+5);
    gTrace2.resize(gMaxPoints+5); //ver114-6b
    gNumPoints=0;
  }
}
int msagraph::gMaxNumPoints()
{
  //max number of points given size of arrays. We fudge a little to allow safety margin
  return gMaxPoints;
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
  if (gDoY1==0) gGraphY1=0; else gGraphY1=graphY1;
  if (gDoY2==0) gGraphY2=0; else gGraphY2=graphY2;
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
   int whole=(int)(N); float fract=N-whole;
   x=gGraphVal[whole][0];y1=gGraphVal[whole][1];y2=gGraphVal[whole][2];
   //Note that angles in the main program are kept in the range -180 to +180, but
   //in gGraphVal they have been adjusted to fit graph range
   if (fract>0)
   {
       x=x+fract*(gGraphVal[whole+1][0]-x);
       if (gY1IsPhase)
       {
           //ver114-7d created special interpolation for angles to deal with wrap-around
           y1=inter.intLinearInterpolateDegrees(fract, y1, gGraphVal[whole+1][1], gY1AxisMin, gY1AxisMax);   //ver116-4k
       }
       else
       {
           y1=y1+fract*(gGraphVal[whole+1][1]-y1);
       }
       if (gY2IsPhase)
       {
           //ver114-7d created special interpolation for angles to deal with wrap-around
           y2=inter.intLinearInterpolateDegrees(fract, y2, gGraphVal[whole+1][2], gY2AxisMin, gY2AxisMax);   //ver116-4k
       }
       else
       {
           y2=y2+fract*(gGraphVal[whole+1][2]-y2);
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
  if (N>0 && N<=gDynamicSteps+1)
  {
    int whole=(int)(N); float fract=N-whole;
    y=gGraphVal[whole][yNum];
    if (fract>0)
    {
      float yMin, yMax;
      int isPhase;
      if (yNum==1) {isPhase=gY1IsPhase; yMin=gY1AxisMin; yMax=gY1AxisMax;} else {isPhase=gY2IsPhase; yMin=gY2AxisMin; yMax=gY2AxisMax;}
      if (isPhase)
      {
        //Note that angles in the main program are kept in the range -180 to +180, but
        //in gGraphVal they have been adjusted to fit graph range
        //ver114-7d created special interpolation for angles to deal with wrap-around
        y=inter.intLinearInterpolateDegrees(fract, y, gGraphVal[whole+1][yNum], yMin, yMax);   //ver116-4k
      }
      else
      {
        y=y+fract*(gGraphVal[whole+1][yNum]-y);
      }
    }
  }
  else
  {
    y=0;
  }
  return y;
}
int msagraph::gGetPointXVal(float N)
{
  int x;
  //Return x for specified point (1...)
  //We don't verify that N is in bounds, because its value may have been created with gGenerateXValues
  //and the actual point data may not have been added yet.
  //N may have a fractional part, so we do linear interpolation
  if (N>0 && N<=gMaxPoints)
  {
    int whole=int(N); float fract=N-whole;
    x=gGraphVal[whole][0];
    if (fract>0) x=x+fract*(gGraphVal[whole+1][0]-x);
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
  if (N>0 && N<=gMaxPoints)
  {
    int whole=(int)N; float fract=N-whole;
    x=gGraphPix[whole][0];
    if (fract>0) x=x+fract*(gGraphPix[whole+1][0]-x);
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
  if (nPts < 0) nPts=0;
  if (nPts >= gMaxPoints) gSetMaxPoints(nPts);
  gNumPoints=nPts;
}

void msagraph::gClearYValues()
{
  //Zero Y values of gNumPoints points
  for (int i=1; i <= gNumPoints;i++) { gGraphVal[i][1]=0; gGraphVal[i][2]= 0; }
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
    QMessageBox::warning(0,"Error", QString("Invalid point number: %1").arg(pNum));    //debugging

  gGraphVal[pNum][1]=y1; gGraphVal[pNum][2]=y2;
}

int msagraph::gAddPoints(float x, float y1, float y2)
{
  //Add points to end. Must be done with non-decreasing x
  //return 1 if too many points
  if (gNumPoints>=gMaxPoints) return 1;
  gNumPoints=gNumPoints+1;
  //enter new point
  gGraphVal[gNumPoints][0]=x; gGraphVal[gNumPoints][1]=y1; gGraphVal[gNumPoints][2]=y2;
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
  //ver116-4k: Log sweeps may now be for negative X values. The X values are not allowed to contain or cross zero.
  //If user specifies a zero endpoint, it gets changed to a small value.
  int numPoints=gDynamicSteps+1;    //ver114-1f
  //X axis starts at gXAxisMin and ends at gXAxisMax
  int pixStart=gMarginLeft; int pixEnd=gMarginLeft+gGridWidth;
  int pixInterval=(pixEnd-pixStart)/(numPoints-1);
  int xInterval;
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
  int x=gXAxisMin; int xPix=pixStart;
  for (int i=1; i <= numPoints; i++)
  {
      if (i==numPoints)
      {
          gGraphVal[i][0]=gXAxisMax;
          gGraphPix[i][0]=pixEnd;  //to get it exact; ver114-4m
      }
      else    //ver114-4k
      {
          if (x>0)
            gGraphVal[i][0]=((int)(1000000*x+0.5))/1000000 ;
          else
            gGraphVal[i][0]=0-((int)(1000000*(0-x)+0.5))/1000000;    //Round to nearest Hz ver114-4k
              //We round pixels to the nearest tenth
          gGraphPix[i][0]=((int)(10*xPix+0.5))/10;
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
  /*
  sub gFindMinMax   'find min and max x and y values in gGraphVal ver115-1d deleted arguments
      'must call gCalcGraphParams after this
      call gGetMinMaxPointNum pMin, pMax    'ver114-6d
      for i=pMin to pMax    'ver114-6d
          x=gGraphVal(i, 0):y1=gGraphVal(i,1):y2=gGraphVal(i,2)
          if i=1 then
              gXAxisMin=x : gXAxisMax=x
              gY1AxisMin=y1 : gY1AxisMax=y1 :gY2AxisMin=y2 : gY2AxisMax=y2
          else
              if x<gXAxisMin then gXAxisMin=x else if x>gXAxisMax then gXAxisMax=x
              if y1<gY1AxisMin then gY1AxisMin=y1 else if y1>gY1AxisMax then gY1AxisMax=y1
              if y2<gY2AxisMin then gY2AxisMin=y2 else if y2>gY2AxisMax then gY2AxisMax=y2
          end if
      next i
  end sub
      */
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
  gGetMinMaxPointNum(pMin, pMax);    //ver114-6d
  if (p1<pMin) p1=pMin;
  if (p2>pMax) p2=pMax;
  int minPeakEnded=1 ; int maxPeakEnded=1;
  int minNumStart, maxNumStart, minNumEnd,maxNumEnd;
  for (int i=p1; i < p2; i++)
  {
    int y=gGraphVal[i][traceNum];
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
      if (y>maxY) {maxY=y ; maxNumStart=i ; maxPeakEnded=0;}
      if (maxPeakEnded==0 && y>=maxY) maxNumEnd=i; else maxPeakEnded=1;
      if (y<minY) { minY=y ; minNumStart=i ; minNumEnd=i ; minPeakEnded=0;}
      if (minPeakEnded==0 && y<=minY) minNumEnd=i; else minPeakEnded=1;
    }
  }
  //Here the min or max start and end numbers indicate where the peak started and ended; we consider
  //the actual peak to be in the middle.
  maxNum=(int)((maxNumEnd+maxNumStart)/2);   //ver115-4b
  minNum=(int)((minNumEnd+minNumStart)/2);   //ver115-4b
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
  //'Get pixel pixel size of entire graphics window
  height=gWindowHeight;
  width=gWindowWidth;
}
void msagraph::gGetMargins(int &marLeft, int &marRight, int &marTop, int &marBot)
{
  //Get graph margins
  marLeft=gMarginLeft; marRight=gMarginRight; marTop=gMarginTop; marBot=gMarginBot;
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
  if (y1==1) gY1IsPhase=1; else gY1IsPhase=0;
  if (y2==1) gY2IsPhase=1; else gY2IsPhase=0;
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
  gDoY1=doY1; gDoY2=doY2;
  if (doY1==0) gGraphY1=0;   //Can't graph it if we don't have it
  if (doY2==0) gGraphY2=0;
    //Clear accumulated draw commands. We use "down" as the empty command
  for (int i=0; i < gDynamicSteps; i++)
  {
    if (doY1==0)
    {
      gTrace1[i]= QPoint(0,0);  //"down";
      gGraphVal[i][1]=0;
    }
    if (doY2==0)
    {
      gTrace2[i]= QPoint(0,0);  //"down";
      gGraphVal[i][2]=0;
    }
  }
}
void msagraph::gInitDefaults()
{
  //Initialize graphing variables to default values
  gXIsLinear=1;
  gY1IsLinear=1;
  gY2IsLinear=1;
  gY1IsPhase=1;  //ver114-5e
  gY2IsPhase=0;    //ver114-6f
  gActivateGraphs(1,1);
  gHorDiv=10; gVertDiv=10;
  gUsePresetColors("LIGHT");   //ver115-3c
  gUsePresetText("BasicText");
  gDoHist=0;
  gPrimaryAxis=1;  //ver115-3c
  gSetGridStyles("ENDSANDCENTER", "All", "All"); //Label these grid lines ver114-5f
}
void msagraph::gSetSweepDir(int dir)
{
  //sub gSetSweepDir dir    //Set sweep direction to 1 or -1
  if (dir==1)
  {
      gSweepDir=1; //Forward
          //make erase lead sign match direction sign
      if (gEraseLead<0) gEraseLead=0-gEraseLead;
      gSweepStart=1; gSweepEnd=gDynamicSteps+1; //ver114-5e
  }
  else
  {
      gSweepDir=-1; //Reverse
          //make erase lead sign match direction sign
      if (gEraseLead>0) gEraseLead=0-gEraseLead;
      gSweepStart=gDynamicSteps+1; gSweepEnd=1; //ver114-5e
  }
}

void msagraph::gGetSweepStartAndEndPointNum(int &pStart, int &pEnd)
{
  pStart=gSweepStart; pEnd=gSweepEnd;
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
  gWindowHeight=winHt; gWindowWidth=winWidth;
  gMarginLeft=marLeft; gMarginRight=marRight;
  gMarginTop=marTop; gMarginBot=marBot;

  //set the size of the area we are going to be drawing in
  graphScene->clear();
  graphScene->addRect(0, 0, winWidth, winHt, QPen(QColor(Qt::magenta)));
  //graphScene->addRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height(), QPen(QColor(Qt::magenta)));
  graphScene->setSceneRect(graphScene->itemsBoundingRect());




  //Calculate a couple of additional parameters from those above
  gGridHeight=gWindowHeight-gMarginTop-gMarginBot;     //height of graph grid
  gGridWidth=gWindowWidth-gMarginLeft-gMarginRight;    //width of graph grid
  gOriginY=gWindowHeight-gMarginBot;   //pixel number of bottom grid line

  gMarkerInfoTop=gOriginY+40;      //Top of area below x axis labels ver114-4e
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
      Pen.setBrush(QColor(referenceColor1));
      Pen.setWidth(referenceWidth1);
    }
    else if (i == 2)
    {
      Pen.setBrush(QColor(referenceColor2));
      Pen.setWidth(referenceWidth2);
    }
    graphScene->addPath(refLine[i],Pen);
  }
}
void msagraph::gAddReference(int N, QString t)
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
  //number of decimal places so the max sig dig won//t be exceeded.
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
void msagraph::gPrintMessage(QString msg)
{
  //Print message above top of marker info area; Limited to 75 characters; don't print if blank
  //The message is in msg$
  int y=gGetMarkerInfoTop()-5;
  int x=30;
  QString textColor, backColor;
  gridappearance->gGetInfoColors(textColor, backColor);
  //cmd$="font Tahoma 8 bold;color ";textColor$;";backcolor ";backColor$    //ver116-4i
  gPrintText(util.Space(140), x, y);   //note spaces are smaller than letters ver116-4j
  if (msg!="")
  {
    gPrintText("MESSAGE: "+msg.left(75), x, y);
  } //don't print if blank
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
    for (int i=0; i <= gHorDiv+1; i++)
    {
      thisLabel = gXGridLabels[i];
      if (thisLabel!="")
      {
        gPrintTextCentered(thisLabel, gMarginLeft+gXGridLines[i], gOriginY+18, gridappearance->gXTextColor);
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
        gPrintTextRightJust(thisLabel, gMarginLeft-6,gOriginY+3-gY1GridLines[i], gridappearance->gY1TextColor);
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
        gPrintText(thisLabel, gMarginLeft+gGridWidth+6, gOriginY+(-10)-gY2GridLines[i], gridappearance->gY2TextColor);
      }
    }
  }
}



void msagraph::gPrintTitle(int doClear)
{
  QGraphicsItem * item;
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

  //There is a fourth title line for sweep info, but we don't print it ver114-5m
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

void msagraph::gPrintAxisAnnotation(int axisNum, QString annotText, QString annotFont, QString annotColor)
{
  //Print annotation above axis labels
  //The specified text is printed above the axis label of the specified axis (1 or 2)
  //in the specified font and color.
  //#gGraphHandle$, "font "; gXAxisFont$;";color "; gXTextColor$
  gPrintTextCentered(gXAxisLabel, gMarginLeft+gGridWidth/2, gOriginY+45, gridappearance->gXTextColor);
  //#gGraphHandle$, "font "; annotFont$
  if (axisNum==1) gPrintTextCentered(annotText, gMarginLeft-15, gMarginTop-30, annotColor);
  if (axisNum==2) gPrintTextCentered(annotText, gMarginLeft+gGridWidth+15, gMarginTop-30, annotColor);
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
  if (isX==1) axisLen=gGridWidth; else axisLen=gGridHeight;
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
      if (isLin==0) {isLin=1; numDiv=10;} //Change log to linear with 10 divisions for zero span ver116-4a
    }
    else
    {
      //For Y axes, pick min and max to center the graph
      axisMin=axisMin-1; axisMax=axisMax+1 ; isLin=1;
    }
  }
  float includesZero=(axisMin<=0) && (axisMax>=0);
  float crossesZero=axisMin*axisMax<0;
  if (isLin==0)  //log sweep ver116-4k
  {
    if (includesZero)    //Includes zero; may or may not cross zero
    {
      if (isX)
      {
        if (crossesZero) {axisMin=0 ; crossesZero=0;}  //don't let x axis cross zero
        if (qMax(abs(axisMin), abs(axisMax))<0.0001)
        {
          isLin=1; //small frequency values; make it linear
        }
        else
        {
          float blackHolePower=-4 ; blackHoleRadius=0.0001;    //Use 100 Hz as blackHoleRadius for X axis if feasible
          float maxMag;
          if (axisMin==0)
          {
            axisMin=blackHoleRadius ; blackHolePix=0 ; maxMag=axisMax;
          }
          else
          {
            axisMax=0-blackHoleRadius ; blackHolePix=axisLen ; maxMag=0-axisMin;
          }
          span=util.uSafeLog10(maxMag)-blackHolePower;
        }
      }
      else //Y axis
      {
        //We choose a blackHoleRadius to provide a reasonable number of decades
        if (crossesZero)
        {
          float gridTopPower=util.uRoundUp(util.uSafeLog10(axisMax)) ; float gridBotPower=util.uRoundUp(util.uSafeLog10(0-axisMin));
          float commonPower=qMin(gridTopPower, gridBotPower);
          //if we used 10^commonPower as blackHoleRadius, we would have (gridTopPower-commonPower)+ (gridBotPower-commonPower)
          //whole or partial decades.
          float blackHolePower=commonPower-1;    //Would provide at least 1 decade of graph on each side.
          float minDecades=(gridTopPower-commonPower)+ (gridBotPower-commonPower);
          if (minDecades>6)
          {
            if (minDecades>9) blackHolePower=minDecades-9; else blackHolePower=commonPower;
          }
          else
          {
            blackHolePower=commonPower-(int)((6-minDecades)/2);
          }
          blackHoleRadius=util.uTenPower(blackHolePower);
          float negativeSpan=util.uSafeLog10(0-axisMin)-blackHolePower  ; int positiveSpan=util.uSafeLog10(axisMax)-blackHolePower;
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
            absMax=abs(axisMin);
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
          if (axisMin==0) {axisMin=blackHoleRadius ; blackHolePix=0;} else {axisMax=0-blackHoleRadius ; blackHolePix=axisLen;}
          span=maxPower-blackHolePower;    //number of decades in range
          if (span<0.7) isLin=1;    //Too narrow a range--may cause trouble with grid lines
          if (span>12)  //don't allow huge span--too many grid lines
          {
            int excessSpan=span-12 ; blackHolePower=blackHolePower+excessSpan ; span=12;
            blackHoleRadius=util.uTenPower(blackHolePower);
            if (axisMax>0) axisMin=blackHoleRadius; else axisMax=0-blackHoleRadius;
          }
          includesZero=0;  //no longer includes zero
        }
      }
    }
    else    //X or Y axis that does not include zero--check for acceptable span
    {
      float absMax=qMax(abs(axisMax), abs(axisMin)) ; float absMin=qMin(abs(axisMax), abs(axisMin));
      if (isX && absMax>0.0001 && absMin<0.0001)
      {
        absMin=0.0001; //Min for x axis should be at least 100 Hz if feasible
        if (axisMin<0) axisMax=0-absMin; else axisMin=absMin;
      }
      span=util.uSafeLog10(absMax/absMin);    //number of decades in range
      if (span<0.7) isLin=1;    //Too narrow a range--may cause trouble with grid lines
      if (span>12)  //don't allow huge span--too many grid lines, so adjust range
      {
        absMin=absMax/util.uTenPower(12); //Change the range limit closest to zero
        if (axisMin<0) axisMax=0-absMin; else axisMin=absMin;
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
    if (span==0) scale=axisLen/gDynamicSteps; else scale=axisLen/span;  //ver116-1b
    float divWidth=axisLen/numDiv ; float valInterval=(axisMax-axisMin)/numDiv;
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
    if (isX==0) scale=0-scale; //Scale is negative so higher Y moves up on graph ver116-4k
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

  int mult=10 ; if (scale>60)  mult=2;   //feasible density of grid lines depends on scale (pixels per decade)
  if (scale>=120) mult=1;

  if (isX==0) scale=0-scale; //Scale is negative so higher Y moves up on graph
  //gridStart will be the power of ten at which we begin. For all positive, it is at or to left of chart; for all negative
  //it is at or to right of chart. If zero is included, gridStart is at blackHoleRadius, which is actually labeled 0 and
  //is also equivalent to -blackHoleRadius.
  if (crossesZero)
  {
    //For log scans that include zero, we calculate the positive side starting at blackHoleRadius
    //and mirror those to the negative side.
    gridStart=blackHoleRadius ; gridStartPix=blackHolePix;
  }
  else
  {
    if (axisMin<0) {gridStart=axisMax ; gridStartPix=axisLen;}  else {gridStart=axisMin ; gridStartPix=0;}
  }
  numDiv=0 ; int absGridStart=abs(gridStart);
  float currTenPower=util.uRoundDownToPower(absGridStart,10);  //We run through positive currTenPower starting here
  //First grid line is for the axis beginning at axisMin, 0 pixel val, doLabel=1
  gridLines=QString::number(axisMin) + ",0,1";
  int currVal;
  for (int decadeNum=1; decadeNum < 200;decadeNum++)  //we will never do all 200
  {
    for (int i=1; i< 9; i++)    //one for each multiple of a power of ten.
    {
      //We run through increasing positive grid values, starting at or below |gridStart|. Once we are at or above
      //|gridStart| we start "drawing" the grid line for positive values, and if appropriate for the negative
      //of that value. We keep going until we are past both the postive and negative ends of the chart.
      if (i==1 || mult==1 || (mult==2 and int(i/2)*2==i))   //do only for lines at desired multiples
      {
        currVal=currTenPower*i;
        if (i==1 && decadeNum==1 && crossesZero)
        {
          //If zero is crossed, we label the starting point 0.
          gridLines=gridLines+",0,"+gridStartPix+","+doLabel;    //enter a grid line
        }
        else
        {
          if (currVal>=absGridStart)
          {
            int currPosPix=gridStartPix+scale*(util.uSafeLog10(currVal/absGridStart));
            int currNegPix=int(0.5+10*(2*gridStartPix-currPosPix))/10;   //reflect through gridStartPix and round to tenth of pixel
            currPosPix=int(0.5+10*currPosPix)/10;   //round to tenth of pixel
            if ((axisMax>0) && (currVal<=axisMax))
            {
              if (currPosPix>=2 && currPosPix<=axisLen-2)  //don't want to do very ends; we do that below
              {
                doLabel=(i==1 && (isX==0 || (currPosPix>=40 && currPosPix<=axisLen-40)));   //label powers of ten if not too close to end of x-axis
                gridLines=gridLines+","+currVal+","+currPosPix+","+doLabel;    //enter a grid line
                numDiv=numDiv+1;
              }
            }
            if ((includesZero || (axisMax<0)) && (0-currVal>=axisMin))  //need to do negative grid line
            {
              if (currNegPix>=2 && currNegPix<axisLen-2)  //don't want to do very ends; we do that below
              {
                doLabel=(i==1 && (isX==0 || (currNegPix>=40 && currNegPix<=axisLen-40)));   //label powers of ten if not too close to end of x-axis
                gridLines=gridLines+","+(0-currVal)+","+currNegPix+","+doLabel;    //enter a grid line
                numDiv=numDiv+1;
              }
            }
          }
        }
      }
    }
    if (currVal>=axisMax && 0-currVal<=axisMin) break;    //End loop if at or outside both ends of chart
    currTenPower=10*currTenPower;
  }
  gridLines=gridLines+","+axisMax+","+axisLen+",1";    //Final grid line
  numDiv=numDiv+1; //1 less than number of grid lines
}
void msagraph::gCalcGraphParams()
{
  //Calculate scaling and, for log, number of divisions
  //min and max X and Y values, number of divisions, log/linear, and grid style must already be set up
  //For log scans, also calculates and replaces gHorDiv or gVertDiv
  //Will force any axis scale to linear if log is inappropriate for some reason.
  //Also calculates grid line locations and labels

  gHorDiv=qMax(1,gHorDiv);gVertDiv=qMax(1,gVertDiv);

  //gCalcAxis may change gXIsLinear and gHorDiv
  //fix me
  QString gridLines;
  float dumBlackHolePix, dumRadius;

  gCalcAxis(1, gXGridStyle, gXIsLinear, gHorDiv, gXAxisMin, gXAxisMax
            , gXScale, dumRadius, dumBlackHolePix, gridLines); //ver116-4k

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
    QString gridLines;
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
    //forcing the isLinear param to 1. The only situation that doesn//t work is if Y1
    //is linear and Y2 is log, in which case Y2 would like to recalculate gVertDiv
    int lin;
    if (gDoY1==0) lin=gY2IsLinear; else lin=1;
        //gCalcAxis may change gY2IsLinear and gVertDiv
    gCalcAxis(0, gY2GridStyle, lin, gVertDiv, gY2AxisMin, gY2AxisMax
              , gY2Scale, gY2BlackHoleRadius, gY2BlackHolePix, gridLines); //ver116-4k
    for (int i=1; i <= gVertDiv+1; i++)  //Save grid line locations and labels
    {
      float gridVal, gridPix, doLabel;
      util.uExtractNumericItems(3,gridLines, ",", gridVal, gridPix, doLabel);
      gY2GridLines[i]=gridPix;;
      if (doLabel==1)
          gY2GridLabels[i]=gGridBoundaryLabel(gridVal, gY2AxisForm);
      else
          gY2GridLabels[i] ="";
    }
    if (gDoY1==0)  gY2IsLinear=lin;     //May have been forced to linear by gCalcAxis
  }
  //if gDoY1=0 and gDoY2=0 then gVertDiv=0 //delver115-2c
  //Y parameters are set to accommodate Y1; Y2 will be scaled to fit those
}

int msagraph::gPixIsInGrid(int xPix, int yPix)
{
  //Return 1 if (xPix, yPix) is within grid bounds; otherwise 0
  //We allow an extra pixel boundary to be part of the grid
  if (xPix < gMarginLeft-1 || xPix > gMarginLeft+gGridWidth+1) return 0;
  if (yPix < gMarginTop-1 || yPix > gOriginY+1) return 0;
  return 1;
}
void msagraph::gFindClickedPoint(int xPix, int &yPix, int &pointNum, int &traceNum)
{
  /*
'Return data for clicked point
    'xPix, yPix originally represent the pixel coordinates of a mouse click. We determine what
    'graph point was clicked and return its point number and trace number, and the Y-pixel value at the clicked trace.
    'pointNum will be the number (1...), possibly non-integral, of the
    'clicked point, and traceNum (1 or 2) will be the number of the trace that was clicked.
    'We return pointNum=0 if no graph point was near the click. pointNum may be fractional
    if gNumPoints<2 then pointNum=0 : exit sub  'To avoid problems
    if yPix>gOriginY+2 or yPix<gMarginTop-2 then pointNum=0 : exit sub  'click not in grid
    pointNum=gPointNumOfXPix(xPix)  'Point number for xPix
    call gGetMinMaxPointNum pMin, pMax    'ver114-6d
    'Allow click to be a bit outside graph to mark end point
    if pointNum<pMin then
        if pointNum>=0 then pointNum=pMin else pointNum=0   'zero indicates outside graph
    else
        if pointNum>pMax then
            if pointNum<=pMax+1 then pointNum=pMax else pointNum=0
        end if
    end if
        'We determine the closest trace 'ver114-7d changed to use actual pixel values, not nearest point values
    xPix=gGetPointXPix(pointNum)
    y1Pix=gGetPointYVal(pointNum,1) : y2Pix=gGetPointYVal(pointNum,2)   'These aren't pixel values yet
    call gConvertY1ToPix y1Pix : call gConvertY2ToPix y2Pix   'ver116-4k
    if y1Pix>yPix then d1=y1Pix-yPix else d1=yPix-y1Pix 'distance from trace 1
    if y2Pix>yPix then d2=y2Pix-yPix else d2=yPix-y2Pix 'distance from trace 2
    if gGraphY2=0 or (gGraphY1=1 and d1<d2 )then
        traceNum=1 : yPix=y1Pix   'Closest to trace 1; or we are not doing trace 2
    else
        traceNum=2 : yPix=y2Pix   'Closest to trace 2; or we are not doing trace 1
    end if*/
}

float msagraph::gPointNumOfX(int x)
{
  //Return point num (1...), possibly non-integral, for the X value x
  //Note this can yield point numbers above or below the actual graph range, for x values outside that range
  int pMin, pMax;
  gGetMinMaxPointNum(pMin, pMax);    //ver114-6d
  if (gXAxisMin==gXAxisMax) return (pMin+pMax)/2;   //All have same X value; use middle one

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
{/*
'Return point num (1...), possibly non-integral, for the X pixel xPix
    'Note this can yield point numbers above or below the actual graph range, for pixels outside that range
    'ver114-6d used gDynamicSteps in place of gNumPoints to find the final point; this is necessary
    'for reverse sweeps.
    x1=gGraphPix(1,0) : x2=gGraphPix(gDynamicSteps+1,0)    'First and last graph points; x pixel values
    call gGetMinMaxPointNum pMin, pMax    'ver114-6d
    if gNumPoints<2 then gPointNumOfXPix=pMin : exit function
    interval=(x2-x1)/(gDynamicSteps) 'Pixels from point to point
    if interval=0 then  'Weird situation; entire graph at one x pixel value; may happen with halted graph
        gPointNumOfXPix=pMin    'ver116-4f
    else
        p=1+(xPix-x1)/interval
                'If point number is very close to an integer, make it one.
        nearInt=int(p+0.01) 'e.g. p=2.99 and p=3.01 both produce nearInt=3
        dif=p-nearInt
        if dif<0.01 and dif>-0.01 then p=nearInt 'If close, make p=nearInt  'ver116-4f
        gPointNumOfXPix=p
    end if
end function*/
  return 0;
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
  x=((int)(x*10+0.5))/10;    //Round to tenth of a pixel    //ver116-4b*/
}

void msagraph::gConvertY1ToPix(float &y1)
{
  //Convert Y1 numeric values to pixel coordinates. Y values should already be in bounds
  //Modified by ver116-4k to allow negative values in log sweeps
  if (gDoY1==0) { y1=-10; return;}
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
          y1=((int)(y1*10+0.5))/10;
          return;  //Round to tenth and exit
      }
      //Here for no zero crossing. If all positive, axis starts at gXGridStart; if all negative it ends at gXGridStart.
      float gridStart;
      if (gY1AxisMin<0) gridStart=gY1AxisMax; else gridStart=gY1AxisMin;
      float ratio=(y1-gridStart)/gridStart;
      if (ratio<0) { y1=-1; return;}  //y is out of bounds; return pixel=-1 ver116-4k
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
  y1=((int)(y1*10+0.5))/10;    //Round to tenth of a pixel    //ver116-4b
  if (y1<gMarginTop) y1=gMarginTop; //don//t let it go off the top
  if (y1>gOriginY) y1=gOriginY;     //don//t let it go off the bottom
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
    if (gY2AxisMin<0) gridStart=gY2AxisMax; else gridStart=gY2AxisMin;
    float ratio=(y2-gridStart)/gridStart;
    if (ratio<0) { y2=-1; return;}  //y is out of bounds; return pixel=-1 ver116-4k
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
  y2=((int)(y2*10+0.5))/10;    //Round to tenth of a pixel    //ver116-4b
  if (y2<gMarginTop) y2=gMarginTop; //don//t let it go off the top
  if (y2>gOriginY) y2=gOriginY;     //don//t let it go off the bottom
}
float msagraph::gNormalizePhase(float p)
{
  //Return phase in range -180<phase<=180
  while (p>180) { p=p-360; }
  while (p<=-180) { p=p+360; }
  return p;
}

float msagraph::gAdjustPhaseToDisplay(int axisNum, int pointNum, int useWorkArray)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  'Adjust phase of point pointNum and return
    'Assumes axis axisNum is phase. Returns the adjusted phase.
    'if useWorkArray=1, the phase is taken from uWorkArray(); otherwise from gGraphVal()
    'Adjust the phase as follows: first, if possible put it within the bounds
    'of gAxisYNMin and gAxisYNMax. Second, if gAxisYNMax-gAxisYNMin is more than a 360 degree range, there may
    'be more than one possible value. Choose the one that is closest to the previous point's phase (previous based
    'on gSweepDir).
    'The idea is that a continuous sweep should normally have continuous phase. If our axes allow a range
    'from 0 to 1000 degrees and the measured phase moves from 350 at one point to 10 at the next point,
    'the best guess as to the true phase change is 20 degrees, and we should display 370 degrees.
    if axisNum=1 then
        axisMax=gY1AxisMax : axisMin=gY1AxisMin
    else
        axisMax=gY2AxisMax : axisMin=gY2AxisMin
    end if
    phaseSpan=axisMax-axisMin
    if phaseSpan<0 then gAdjustPhaseToDisplay=0 : exit function   'Error if negative span ver116-1b
    if useWorkArray then phase=uWorkArray(pointNum, axisNum) else phase=gGraphVal(pointNum, axisNum)
    origPhase=phase
    'Put phase in bounds from gY1AxisMin to gY1AxisMax. In general case, phase can
    'equal either bounds.
        'Add or Subtract 360 until phase becomes in range
    if phase>axisMax then
        while phase>axisMax : phase=phase-360 : wend
    else
        while phase<axisMin : phase=phase+360 : wend
    end if

    if phase<axisMin or phase>axisMax then
        'Couldn't find in-bounds value; put in normal range. Can happen only if phaseRange<360
        phase=origPhase    'ver114-5o
        while phase>180 : phase=phase-360 : wend    'ver115-1d
        while phase<=-180 : phase=phase+360 : wend    'ver115-1d
    else
            'Found in-bounds value; maybe there is more than one in-bounds value (if phaseSpan>=360)
            'If this is the first point, we just go with the angle as is
        if useWorkArray then lastPhase=uWorkArray(pointNum-gSweepDir, axisNum) else lastPhase=gGraphVal(pointNum-gSweepDir, axisNum)
        if pointNum<>gSweepStart and phaseSpan>=360 then
            if lastPhase<phase then
                while phase>lastPhase and phase>=axisMin : phase=phase-360 : wend
                'If phase is in bounds then phase and phase+360 now bracket lastPhase
                'In that case, use the closest value. If phase is now out-of-bounds,
                'use the last in-bounds value
                if phase<axisMin or lastPhase-phase>180 then phase=phase+360
            else
                while phase<lastPhase and phase<=axisMax : phase=phase+360 : wend
                'If phase is in bounds then phase and phase-360 now bracket lastPhase
                'In that case, use the closest value. If phase is now out-of-bounds,
                'use the last in-bounds value
                if phase>axisMax or phase-lastPhase>180 then phase=phase-360
            end if
        end if
    end if
    gAdjustPhaseToDisplay=phase 'Return adjusted phase*/
  return 0;
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
  gDynamicSteps=nSteps ; int nPoints=nSteps+1;
  //Our existing points probably aren//t any good anyway, but in case they get graphed...
  if (gNumPoints>nSteps+1) gNumPoints=nSteps+1; //ver114-6d
  gSetMaxPoints(nPoints);  //Be sure we have room
  gSetSweepDir(gSweepDir);  //ver114-5e To set sweep start and end point numbers
}
void msagraph::gSetErasure(int doErase1, int doErase2, int nEraseLead)
{
  //Set erasure parameters
  //nEraseLead specifies the number of points by which the erasure leads the drawing.
  //doEraseN indicates whether trace N should be erased as it is redrawn.
  if (doErase1==0) gDoErase1=0; else gDoErase1=1;
  if (doErase2==0) gDoErase2=0; else gDoErase2=1;
  if (nEraseLead<=0) nEraseLead=0-nEraseLead;   //make it positive
  gEraseLead=nEraseLead*gSweepDir;   //Set its sign based on sweep direction
  gInitDraw();  //Creates draw and erase prefixes
}

void msagraph::gGetErasure(int &doErase1, int &doErase2, int &nEraseLead)
{
  //Get erasure parameters
  doErase1=gDoErase1; doErase2=gDoErase2; nEraseLead=gEraseLead;
}

void msagraph::gPauseDynamicScan()
{
  //Suspend until next gResumeDynamicScan or gInitDynamicDraw
  //When the user halts the scanning, the screen may be redrawn, and we don//t want any
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

  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*

    'Restore the erasure gEraseLead points in front of the traces. This is called
    'after redrawing the graphs.
    'Erasures occurring immediately adacent to the current draw point can affect pixels for prior draw
    'points (which may actually be at the same rounded off pixel x-value as the current point. So after
    'erasing we redraw a couple of points.
    'gPrevPointNum is the last point previously drawn, and thus is the point from which
    'the erasure is to begin, and the final point that we will redraw.
    if gIsFirstDynamicScan=1 or (gDoErase1=0 and gDoErase2=0) then exit sub
    saveNum=gPrevPointNum
    call gInitErase
        'Redraw some retroactive points if possible to eliminate erasure artifacts
        'The best indicator of how many to redraw is gEraseLead.
    if gSweepDir=1 then
        nRedraw=min(gPrevPointNum, gEraseLead)
        gPrevPointNum=gPrevPointNum-nRedraw
    else    'ver114-4m modified this else... block
        if gPrevPointNum=gNumPoints then
            nRedraw=1 : gPrevPointNum=0     'zero signals we are at first point
        else
            nRedraw=min(gNumPoints-gPrevPointNum, 0-gEraseLead)   'note gEraseLead is negative here
            gPrevPointNum=gPrevPointNum+nRedraw
        end if
    end if
    for i=1 to nRedraw : call gDrawNextPoint: next i
    gPrevPointNum=saveNum
end sub*/
}

void msagraph::gStartNextDynamicScan()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
 'Prepare for next pass of dynamic draw
    #gGraphHandle$, "discard"   'Discard last scan info stored in Liberty Basic
    gIsFirstDynamicScan=0
    gPrevPointNum=0
    call gInitDraw  'implements current color and width in case they were changed at end of prior scan ver116-4s
    call gInitErase 'Erase first group of segments if required
end sub*/
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
        if gNumPoints>gMaxPoints then notice "Too many points to " : exit sub
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
qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Do segment erase and redraw for dynamic drawing
    'This is called repeatedly to dynamically erase (if necessary) the prior line and draw a new one.
    'Each call draws the segment from gPrevPointNum to gPrevPointNum+1, using y1 and y2
    'as the new values for point gPrevPointNum+1; those values are also saved.
    'Erasure is not done on the very first scan of points, signalled by gIsFirstDynamicScan=1.
    'On the first scan, we save the x value; thereafter we reuse them. We always save the y values.
    'After the first scan, if gDoErase for the trace is 1, then before we draw a segment we erase the segment
    'from point gPrevPointNum+gEraseLead to the point after it.
    'This subroutine is called for each point except the first point of each scan, for which gStartNextDynamicPass
    'is called.
    'The data for the prior line, is already in gGraphVal(); we reuse the X-values.
    prevPoint=gPrevPointNum 'ver114-4m
        '1A. Erase prior segments if required
        'ver114-4m created this if... block to deal with reverse sweeps
    'ver114-5e modified to use gSweepStart and gSweepEnd
    oneBeforeStart=gSweepStart-gSweepDir    'The point before the sweep start
    if prevPoint=0 then prevPoint=oneBeforeStart
    if prevPoint=oneBeforeStart then isFirstPoint=1 else isFirstPoint=0
    currPoint=prevPoint+gSweepDir
    if currPoint=gSweepEnd then isFinalDrawPoint=1 else isFinalDrawPoint=0
    prevErasePoint=prevPoint+gEraseLead 'Erase is ahead of draw by gEraseLead points
    thisErasePoint=prevErasePoint+gSweepDir
    eraseInBounds=0
    if gSweepDir=1 then
        if thisErasePoint>=gSweepStart and thisErasePoint<=gSweepEnd then eraseInBounds=1
    else
        if thisErasePoint<=gSweepStart and thisErasePoint>=gSweepEnd then eraseInBounds=1
    end if

    if gIsFirstDynamicScan=0 and eraseInBounds then  'ver114-5e
            '1. Erase a segment on each trace
        cmd$=""
        'ver114-6d We erase by using the same command used to draw the segment, which is
        'in gTrace1$() or gTrace2$(). We prefix with our size/color info. ver116-4b added trace width tests
        if gDoErase1 and gTrace1Width<>0 then cmd$=gErase1$;gTrace1$(thisErasePoint) else cmd$="down"   'down is our NOP command
        if gDoErase2 and gTrace2Width<>0 then  cmd$=cmd$;";";gErase2$;gTrace2$(thisErasePoint)
        if cmd$<>"" then #gGraphHandle$, cmd$    'Send command to erase
    end if  'End Erase

        '2. Draw the new Segments
         'ver114-4m created isFirstPoint and isFinalPoint, and added the gSweepDir tests
    gGraphVal(currPoint, 1)=y1 : gGraphVal(currPoint, 2)=y2
            '2A. Save Y1 and convert to pixel coords
    if gDoY1=1 then
        'ver114-5e Adjust phase by multiples of 360 to conform to graph bounds
        if gY1IsPhase then y1=gAdjustPhaseToDisplay(1, currPoint, 0) :gGraphVal(currPoint, 1)=y1
        if gY1IsLinear=1 then
            y1=(y1-gY1AxisMin)*gY1Scale+gOriginY
        else
            call gConvertY1ToPix y1 'ver116-4k
        end if
            'We round pixels to the nearest tenth just to keep the accumulated draw commands shorter
        y1=int(10*y1+0.5)/10

        if y1<gMarginTop then y1=gMarginTop 'don't let it go off the top
        if y1>gOriginY then y1=gOriginY     'don't let it go off the bottom
    end if
                '2B. Save y2 and convert to pixel coords
    if gDoY2=1 then
        if gY2IsPhase then y2=gAdjustPhaseToDisplay(2, currPoint,0) : gGraphVal(currPoint, 2)=y2
        if gY2IsLinear=1 then
            y2=(y2-gY2AxisMin)*gY2Scale+gOriginY
        else
            call gConvertY2ToPix y2 'ver116-4k
        end if
        if y2<gMarginTop then y2=gMarginTop 'don't let it go off the top
        if y2>gOriginY then y2=gOriginY     'don't let it go off the bottom
                    'We round pixels to the nearest tenth just to keep the accumulated draw commands shorter
        y2=int(10*y2+0.5)/10
    end if
            '2C. If first scan, we need to count the points
    if gIsFirstDynamicScan=1 then
        if gNumPoints>gMaxPoints then notice "Too many points to " : exit sub
        gNumPoints=gNumPoints+1
    end if
    x=gGraphPix(currPoint,0)    'Use old x pixel coord; it's still good

            '2D. For every scan, save the y  pixel coords
    gGraphPix(currPoint,1)=y1 : gGraphPix(currPoint,2)=y2   'save y pixel coords
            '2E. Find point from which to draw
          'ver114-4m created isFirstPoint and isFinalPoint, and added the gSweepDir tests

    if gDoHist=1 then
            'For histogram draw vertical line.
            lastX=x :lastY1=gOriginY : lastY2=gOriginY
    else
                'For normal trace, draw from last point if there is one
        if isFirstPoint=0 then 'ver114-4m
            lastX=gGraphPix(prevPoint,0)
            lastY1=gGraphPix(prevPoint,1)
            lastY2=gGraphPix(prevPoint,2)
        end if
    end if
            '2F. Construct total draw command as one string and send it
    cmd$="" : thisCmd$=""
         'The "line" command does not draw the final endpoint of the line, which normally
        'gets drawn as the start point of the next line. But for the last point, or for
        'any point at which a halt occurs, that final pixel does not get drawn.
        'ver114-6d modified to accumulate all draw commands
    if gDoY1 then 'ver115-1a
        if gGraphY2 then j$=";" else j$=""
        'ver114-4n We normally draw a line, but in special cases we use "set" to set a point
        'We do so for the very first point of non-histogram, and for any histogram point
        'consisting of a single point, which "line" will not draw.
        'ver114-6d modified to accumulate all draw commands when gIsDynamic=1
        if isFirstPoint and (gDoHist=0) or (gDoHist and int(y1)=gOriginY) then
            thisCmd$="set ";x;" ";y1   'just set the point
        else
            thisCmd$="line ";lastX; " ";lastY1;" ";x;" ";y1;";line ";x;" ";y1;" ";lastX; " ";lastY1
        end if
        if gIsDynamic=1 then  gTrace1$(currPoint)=thisCmd$
        if gGraphY1 and gTrace1Width<>0 then cmd$=gDraw1$;thisCmd$;j$  'ver116-4b
    end if
    if gDoY2=1 then  'ver115-1a
        if isFirstPoint and (gDoHist=0) or (gDoHist and int(y2)=gOriginY) then
            thisCmd$="set ";x;" ";y2   'just set the point
        else
            thisCmd$="line ";lastX; " ";lastY2;" ";x;" ";y2;";line ";x;" ";y2;" ";lastX; " ";lastY2
        end if
        if gIsDynamic=1 then  gTrace2$(currPoint)=thisCmd$
        if gGraphY2 and gTrace2Width<>0 then cmd$=cmd$;gDraw2$;thisCmd$  'ver116-4b
    end if
    if cmd$<>"" and gGraphHandle$<>"" then #gGraphHandle$, cmd$   'Draw lines
    gPrevPointNum=currPoint
end sub*/
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
  int prevPoint = vars->thisstep - vars->sweepDir + 1;      //thisstep runs from 0; here 1 is first point. //ver114-4k
  int currPoint=prevPoint+vars->sweepDir;   //ver114-4k
  int prevErasePoint=prevPoint+gEraseLead;     //Erase is ahead of draw by gEraseLead points
  int thisErasePoint=prevErasePoint+vars->sweepDir;    //ver114-4k
  if (gIsFirstDynamicScan)
  {
    //1. If first scan, we need to count the points, but not erase
    if (gNumPoints>gMaxPoints)
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
      //cmd$=gErase2$;gTrace2$(thisErasePoint)
      //#graphBox$, cmd$    //Send command to erase
    }  //End Erase
  }

  //3. Draw the new Segments
  //3A. Save Y and convert to pixel coord
  gGraphVal[currPoint][2]=thispointy2; //save new y values
  int yPix=(thispointy2-gY2AxisMin)*gY2Scale+gOriginY;
  if (yPix<gMarginTop)
    yPix=gMarginTop; //don//t let it go off the top
  if (yPix>gOriginY)
    yPix=gOriginY;     //don//t let it go off the bottom
  //We round pixels to the nearest tenth just to keep the accumulated draw commands shorter
  yPix=((int)(10*yPix+0.5))/10;
  gGraphPix[currPoint][2]=yPix;  //save y pixel coords

  //3B. Construct total draw command as one string and send it
  //ver114-4d modified the following to set the first point
  int xPix=gGraphPix[currPoint][0]; //ver114-4k
  //ver114-4n We set a point by drawing the line backward; except first point of non-histo is directly set.
  //For wide traces, "set" can create artifacts, but they are OK on first point.
  int lastX=gGraphPix[prevPoint][0];
  int lastY2=gGraphPix[prevPoint][2];
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
    //   thisCmd$="line ";lastX; " ";lastY2;" "; xPix;" ";yPix;"line ";xPix;" ";yPix;" ";lastX; " ";lastY2

    QPen pen(QColor(gridappearance->gTrace2Color));
    pen.setWidth(gTrace2Width);

    graphScene->addLine(lastX, lastY2, xPix, yPix,pen);
  }

  //ver114-6d permanently enable accum
  //      gTrace2[currPoint]=thisCmd;
  gTrace2[currPoint] = QPoint(xPix, yPix);
  //cmd$=gDraw2$;thisCmd$
  //#graphBox$, cmd$   //Draw lines
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

  gGraphVal[currPoint][1]=y1;
  gGraphVal[currPoint][2]=y2; //ver114-5e
  int x=gGraphPix[currPoint][0];    //Get previously calculated x pixel value for current point

  //Draw to Y1 if necessary
  if (gDoY1==1)
  {
    //ver114-5e Adjust phase by multiples of 360 to conform to graph bounds
    if (gY1IsPhase)
    {
      y1=gAdjustPhaseToDisplay(1, currPoint, 0);
      gGraphVal[currPoint][1]=y1;
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
    y1=((int)(10*y1+0.5))/10;
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
      gGraphVal[currPoint][2]=y2;
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
    y2=((int)(10*y2+0.5))/10;
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

  int x=gGraphPix[currPoint][0];
  int y1=gGraphPix[currPoint][1];
  int y2=gGraphPix[currPoint][2];
  int lastX;
  int lastY1;
  int lastY2;
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
      //thisCmd="line "+lastX+ " "+lastY1+" "+x+" "+y1=";line "+x+" "+y1=" "+lastX+ " "+lastY1;
    }
    if (gIsDynamic==1)
    {
      gTrace1[currPoint]=QPoint(x,y1);
    }
    if (gGraphY1 && gTrace1Width!=0)
    {
      cmd=gDraw1+thisCmd+j;  //ver116-4b
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
      //thisCmd="line ";lastX; " ";lastY2;" ";x;" ";y2;";line ";x;" ";y2;" ";lastX; " ";lastY2
    }

    if (gIsDynamic==1)
    {
      gTrace2[currPoint]=QPoint(x,y2);
    }
    if (gGraphY2 && gTrace2Width!=0)
    {
      // cmd$=cmd$;gDraw2$;thisCmd$  //ver116-4b
    }
  }
  if (cmd!="")
  {
    //and gGraphHandle$<>"" then #gGraphHandle$, cmd$   //Draw lines
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
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'erase line for the next segment due for erasure
    'Note gPrevPointNum is negative when we erase the first gEraseLead points when
    'starting a scan.
    if (gDoErase1=0 and gDoErase2=0) then exit sub   'ver114-2f
        'Note gEraseLead is negative for reverse sweep
    prevPoint=gPrevPointNum
    if prevPoint=0 then prevPoint=gSweepStart-gSweepDir 'The point before the sweep start
    prevErasePoint=prevPoint+gEraseLead 'Erase is ahead of draw by gEraseLead points
    thisErasePoint=prevErasePoint+gSweepDir

    if gSweepDir=1 then         'ver114-4k allows for reverse
        if thisErasePoint>gSweepEnd or thisErasePoint<gSweepStart then exit sub 'ver114-5e
    else
        if thisErasePoint<gSweepEnd or thisErasePoint>gSweepStart then exit sub  'ver114-5e
    end if

        'Erase a segment on each trace
    cmd$=""
    'ver114-6d We erase by using the same command used to draw the segment, which is
    'in gTrace1$() or gTrace2$(). We prefix with our size/color info
    if gDoErase1 and gTrace1Width<>0 then cmd$=gErase1$;gTrace1$(thisErasePoint) else cmd$="down"   'down is our NOP command
    if gDoErase2 and gTrace2Width<>0 then  cmd$=cmd$;";";gErase2$;gTrace2$(thisErasePoint)  ' ver116-4b
    if cmd$<>"" then #gGraphHandle$, cmd$    'Send command to erase
end sub*/
}
void msagraph::gInitDraw()
{
  //initialize width and color commands for drawing new
  //gPrevPointNum=0 delver114-6d
  //Create initial specs for drawing and erasing traces.
  gDraw1="size "+QString::number(gTrace1Width)+";color "+gridappearance->gTrace1Color + ";";
  gDraw2="size "+QString::number(gTrace2Width)+";color "+gridappearance->gTrace2Color + ";";
  gErase1="size "+QString::number(gTrace1Width)+";color "+gridappearance->gBackColor + ";";
  gErase2="size "+QString::number(gTrace2Width)+";color "+gridappearance->gBackColor + ";";
}

void msagraph::gInitErase()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  'Initialize Erase.
      'Erases from point gPrevPointNum to point gPrevPointNum+gEraseLead
      'When a scan is restarted, gPrevPointNum is 0. If the scan is resumed in the middle,
      'gPrevPointNum is the last point that was drawn.
      if (gDoErase1=0 and gDoErase2=0) then exit sub
      saveNum=gPrevPointNum
      if gPrevPointNum=0 then gPrevPointNum=gSweepStart-gSweepDir 'The point before the starting point ver114-5e
      if gPrevPointNum=gSweepStart-gSweepDir then extra=1 else extra=0    'ver114-5e
      if gEraseLead>0 then
          nErase=extra+gEraseLead 'ver114-5c
      else    'reverse sweep
          nErase=extra-gEraseLead 'gEraseLead is negative 'ver114-5c
          if gPrevPointNum=0 then gPrevPointNum=gNumPoints+1  'zero signals start at beginning
      end if
      gPrevPointNum=gPrevPointNum-gEraseLead   'gEraseNextPoint will use this
      for i=1 to nErase
              'Note gEraseNextPoint does nothing if the point number gets out of bounds
          call gEraseNextPoint    'Erase gPrevPointNum+gEraseLead to gPrevPointNum+gEraseLead+1
          gPrevPointNum=gPrevPointNum+gSweepDir   'ver114-4k
      next
      gPrevPointNum=saveNum   'Restore
  end sub*/
}
void msagraph::gClearMarkers()
{
  //Clear all markers
  //point number of -1 indicates that marker is absent
  for (int i=0; i < 20; i++)
  {
    gMarkerPoints[i][0]=-1; gMarkerPoints[i][1]=0-util.uTenPower(9);
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
      if (gMarkerPoints[i][0]>=0) count=count+1;
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
    if (calcXPix) {x=gGraphVal[i][0]; gConvertXToPix(x); gGraphPix[i][0]=x;}
    if (i>=pMin && i<=pMax)      //Only do for actual graph points ver114-6d
    {
      float y1=gGraphVal[i][1]; float y2=gGraphVal[i][2];
      if (gDoY1) gConvertY1ToPix(y1); else y1=0; //ver116-4k
      if (gDoY2) gConvertY2ToPix(y2); else y2=0; //ver116-4k
      gGraphPix[i][1]=y1; gGraphPix[i][2]=y2;
    }
  }
}

void msagraph::gDrawMarkerInfo()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub gDrawMarkerInfo 'Draw marker info in specified area
    '(gMarkerInfoLeft, gMarkerInfoTop) is the upper left for the actual marker info, which will be printed
    'one line per marker. The heading for the marker info will be printed at the top.
    'The total display area is 280 pixels wide. markerX will be updated to the rightmost
    'pixel that is drawn (which will be the bounding box of the marker info.)
    'Markers whose point number in gMarkerPoints is <=0 will be skipped
    'The marker info will be in columns, labeled Mark, Freq, Mag and Phase
    'They will all be right aligned at specific points. If printed in a monotype font, the
    'decimals, where applicable, will also align. We print each column separately, rather than
    'using spaces for alignment, because some monotype fonts do not have spaces of the same
    'width as their characters.
    nMarkers=gValidMarkerCount()  'Don't count those with negative value for point num
          'ver115-1b made various changes to be sure marker area is cleared no matter what.
          'Set drawing color
    #gGraphHandle$, "font Tahoma 8 bold";       'Set drawing font
    #gGraphHandle$, "backcolor ";gBackColor$
    #gGraphHandle$, "size 1"
    headWidth=120 : headHeight=14
    if gDoY1 then headWidth=headWidth+55
    if gDoY2 then headWidth=headWidth+60
    markerX=gMarkerInfoLeft
    markerY=gMarkerInfoTop
    a=gWindowHeight: b=gMarkerInfoTop
    markPerCol=int((gWindowHeight-5-gMarkerInfoTop-headHeight)/13)
    maxBoxWidth=2*headWidth+14  'widest possible box area 'ver115-1b
    if nMarkers>markPerCol then _
            boxWidth=maxBoxWidth else boxWidth=headWidth+4      'ver115-1b
    boxHt= 4+ headHeight + (markPerCol)*13
        'First clear entire marker area
    #gGraphHandle$, "color ";gBackColor$
    #gGraphHandle$, "place ";markerX;" "; markerY 'locate pen at upper left
    #gGraphHandle$, "boxfilled "; maxBoxWidth+boxWidth; " "; markerY+boxHt   'This is lower right corner of max area
    #gGraphHandle$, "color ";gGridTextColor$
    if nMarkers<1 then exit sub   'None to display

    call gPrivateDrawMarkerInfo 1, markPerCol, markerX, markerY, firstEnd
    if nMarkers>markPerCol then call gPrivateDrawMarkerInfo firstEnd+1, markPerCol, markerX+headWidth+10, markerY, firstEnd
    gMarkerInfoRight=markerX+boxWidth    'Rightmost pixel drawn, so area to right is still available
end sub
*/
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

  /*  call gMouseQueryClear   //Should already be clear, but right-clicking during MouseOver can confuse ver116-4k
*/
  if(refreshAutoScale)  //ver114-7a
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

  if (referenceLineType!=0)    //ver114-7f
  {
    if (recreateReferences || refreshTracesDirty==1)
    {
      if (referenceLineType>1)
        CreateReferenceSource();
      CreateReferenceTransform();
      gClearAllReferences();
      if (referenceDoMath==0)   //don't draw if we are using ref for math ver114-8b
      {
        if (referenceTrace & 2)
          gAddReference(1,CreateReferenceTraces(referenceColor2,referenceWidth2,2, &refLine[2]));  //Do Y2 reference
        if (referenceTrace & 1)
          gAddReference(2,CreateReferenceTraces(referenceColor1,referenceWidth1,1, &refLine[1]));
      }
    }
    PrintReferenceHeading();
    if (referenceDoMath==0)
      gDrawReferences();  //don't draw if we are using ref for math ver115-5d
  }

  gPauseDynamicScan();  //Keeps the trace redraws from accumulating
  if (refreshTracesDirty==1 || refreshRedrawFromScratch)
  {
    //gRecreateTraces(1);   //Recreate and draw trace draw commands //ver114-6e //ver114-7e
  }
  else
  {
    gRefreshTraces(); //Draw from accumulated trace draw commands  //ver114-6e
  }
  if (restoreErase)
    gRestoreErasure();
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
    gDetermineMarkerPointNumbers();   //locate floating markers
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
  /*
  if smithGraphHndl$()<>"" then  //ver115-1b draw smith chart if we have one
    call smithSetGraphMarkers doGraphMarkers    //Tell smith whether to draw markers on graph ver115-2c
    if referenceLineType<>0 and (referenceTrace and 4=4) then _
                    doSmithRef=(referenceDoMath=0) else doSmithRef=0   //draw non-fixed value reference ver115-7a
    call smithRefresh doSmithRef //Draw chart and graph, with possible reference line
  end if
  */
  refreshForceRefresh=0; //Clear refresh flags, since we just redrew
  refreshGridDirty=0;
  refreshTracesDirty=0;
  refreshMarkersDirty=0;
  refreshAutoScale=0;      //ver114-7b
  refreshRedrawFromScratch=0;
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
  QString textColor, backColor;
  gridappearance->gGetInfoColors(textColor, backColor);
  //#graphBox$, "font Tahoma 8 bold;color ";textColor$;";backcolor ";backColor$
  QFont Tahoma("Tahoma", 8, QFont::Bold);
  graphScene->setFont(Tahoma);

  if (vars->msaMode=="SA") //ver115-1b rewrote this if... block
  {
    //SA mode, either with or without TG
    if (vars->gentrk==0) //ver115-4f
    {
      gPrintText("Spect. Analyzer", InfoX-10,16,textColor);
    }
    else
    {
      gPrintText("Spect. Analyzer with TG", InfoX-40,16,textColor);
    }
  }
  else
  {

    if (vars->msaMode=="ScalarTrans")
      gPrintText("SNA Transmission", InfoX-10,16,textColor);   //ver115-4e
    if (vars->msaMode=="VectorTrans")
      gPrintText("VNA Transmission", InfoX-10,16,textColor);
        //ver115-1c revised the printing of the cal level
    if (vars->msaMode=="Reflection")
      gPrintText("VNA Reflection", InfoX-10,16,textColor);
    QColor col;
    if (vnaCal->applyCalLevel < vnaCal->desiredCalLevel) col = Qt::red;   //ver115-1b if downgraded cal, print in red
    if (vnaCal->applyCalLevel==0)
    {
        if (vars->msaMode=="Reflection") col = Qt::red;  //no reflection cal, print in red
          vnaCal->calLevel="None";
    }
    else
    {
        if (vnaCal->applyCalLevel==1)
          vnaCal->calLevel="Base";
        else
          vnaCal->calLevel="Band";  //ver115-1d
    }
    gPrintText("Cal=" + vnaCal->calLevel,InfoX, InfoY, col);
    InfoY=InfoY+16;
    //#graphBox$, "color ";textColor$ //restore
  }

  if (vars->freqBand!=0)
    gPrintText(QString::number(vars->freqBand) + "G", InfoX, 30,textColor);   //ver116-4s
  if (vars->suppressHardware)
    gPrintText( "No MSA", InfoX, 44,textColor);   //ver115-6c

            //Base frequency, if not zero
  if (vars->baseFrequency!=0)
  {
    gPrintText("Base Freq(MHz)=", InfoX, InfoY,textColor);
    InfoY=InfoY+16;     //ver116-4k
    gPrintText(QString::number(vars->baseFrequency,'g',3), InfoX+5, InfoY,textColor);
    InfoY=InfoY+16; //ver116-4k
  }

  gPrintText( "RBW=" + QString::number(activeConfig->finalbw,'g',6)+"kHz",InfoX, InfoY,textColor);
  InfoY=InfoY+16;    //ver116-1b
  gPrintText( "Vid="+vars->videoFilter,InfoX, InfoY,textColor);
  InfoY=InfoY+16; //ver116-1b put this back in

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
    float stepfreq=(vars->endfreq-vars->startfreq)/vars->globalSteps;    //stepfreq is only calculated for printing ver115-1b
    QString stepSize = util.uFormatted(1000000*stepfreq,"3,4,5//UseMultiplier//suffix=Hz");
    gPrintText(util.uCompact(stepSize)+"/Step", InfoX, InfoY,textColor); InfoY=InfoY+16;
        //Print Freq/div. also done only for linear sweep
    int nHorDiv, nVertDiv;
    gGetNumDivisions(nHorDiv, nVertDiv);
    stepSize=util.uFormatted(1000000*vars->sweepwidth/nHorDiv,"3,4,5//UseMultiplier//suffix=Hz");
    gPrintText(util.uCompact(stepSize)+"/Div", InfoX, InfoY,textColor); InfoY=InfoY+16;
  }
  if (vars->msaMode=="SA" && activeConfig->TGtop>0)   //ver115-4f
  {
    if (vars->gentrk==0)
    {
        QString freq=QString::number(vars->sgout);
        if (freq.length()<7)  //ver116-4d
        {
            gPrintText("SG="+freq, InfoX, InfoY,textColor); InfoY=InfoY+16;
        }
        else
        {
          gPrintText("SG=", InfoX, InfoY,textColor); InfoY=InfoY+14;  //ver116-4d
          gPrintText("  "+freq, InfoX, InfoY,textColor); InfoY=InfoY+16;
        }
    }
    else
    {
      QString nr;
      if (vars->normrev==0) nr="TG=Normal"; else nr="TG=Reverse";
      gPrintText(nr, InfoX, InfoY,textColor); InfoY=InfoY+16;
      QString freq = QString::number(vars->offset);
      if (freq.length()<2)  //ver115-4g
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
  if (vars->msaMode!="SA" && vars->msaMode!="ScalarTrans")
  {
    gPrintText("Exten="+QString::number(vars->planeadj)+" ns", InfoX, InfoY,textColor);
    InfoY=InfoY+16;  //ver114-5f
  }
  if (vars->msaMode=="Reflection") //ver116-4k
  {
    //gPrintText("Z0=";uFormatted$(S11GraphR0, "3,4,5//UseMultiplier//DoCompact//SuppressMilli"), InfoX, InfoY); InfoY=InfoY+16;
  }
  if (vars->msaMode!="SA")
  {
  //  #graphBox$, "font Tahoma 8 bold;color ";textColor$;";backcolor ";backColor$
    QString directText;
    if (vars->switchFR==0) directText=">DUT"; else directText="DUT<";
    gPrintText(directText, InfoX, InfoY,textColor); InfoY=InfoY+16;
  }

  QString s="";
  if (referenceLineType!=0 && referenceDoMath!=0)  //If doing math with reference line, so indicate ver116-1b
  {
    if (referenceOpA==1 && referenceOpB==1) s="Data+Ref";
    if (referenceOpA==1 && referenceOpB==-1) s="Ref-Data";
    if (referenceOpA==-1 && referenceOpB==1) s="Data-Ref";
    gPrintText(s, InfoX, InfoY,textColor); InfoY=InfoY+16;
  }
}
void msagraph::PrintMessage()
{
  //Print message above top of marker info area; Limited to 75 characters; don't print if blank
  //The message is in message$
  gPrintMessage(vars->message); //ver116-4i
}
QString msagraph::gSweepContext()
{
  //Return string with context info on trace graphing
  //successive lines are separated by chr(13)
  QString newLine="\r";
  QString aSpace=" ";
  //QString sep=";;";   //delimits text items on one line
  //ver114-2d cleaned up the following

  QString s1= "MinMaxXAxis="+QString::number(gXAxisMin)+aSpace+QString::number(gXAxisMax);         //X-axis range
  s1= s1+ newLine+ "MinMaxY1Axis="+QString::number(gY1AxisMin)+aSpace+QString::number(gY1AxisMax); //Y1 axis range
  s1= s1+ newLine+ "MinMaxY2Axis="+QString::number(gY2AxisMin)+aSpace+QString::number(gY2AxisMax); //Y2 axis range
  s1= s1+ newLine+ "IsLinear="+QString::number(gXIsLinear)+aSpace+QString::number(gY1IsLinear)+aSpace+QString::number(gY2IsLinear);    //Is Linear
  s1= s1+ newLine+ "NumSteps="+ QString::number(gDynamicSteps);    //Number of steps
  s1= s1+ newLine+ "SweepDir="+ QString::number(gSweepDir);    //Sweep direction ver114-4k
  //Note gMode is handled by saving msaMode ver114-6f
  return s1;
}
void msagraph::gGetMarkerByNum(int markNum, int &pointNum, QString &ID, QString &trace, QString &style)
{
  //markNum is the ordinal
  //Get info for marker at entry number entryNum (1...)
  //Return negative pointNum if none found
  if (markNum<0 || markNum>gNumMarkers) {pointNum=-1; ID=""; return;}
  pointNum=gMarkerPoints[markNum][0];
  ID=gMarkers[markNum][0];
  trace=gMarkers[markNum][1]; style=gMarkers[markNum][2];
}
void msagraph::gUsePresetColors(QString btn)
{

  //ver116-4b added extra trace colors
  //Light, Dark and Sepia are hard-wired. In addition, 5 custom colors may be used, and referred to as Custom1, ....
  //btn$ specifies the preset to be used. It may be in the form of a
  //button handle, so we drop everything before the period, if there is one.
  int pos=btn.indexOf(".");
  if (pos>-1) btn=btn.mid(pos+1);
  if (btn.left(6).toUpper()=="CUSTOM")   //ver115-2a
  {
      //We need to use a numbered custom color set. btn$ is in the form "CustomN" where N is 1-5
    //fix me gUseCustomPresetColors(btn.mid(7).toInt());
    return;
  }

  gridappearance->gTrace1Color = QColor(250,40,20).name();
  gridappearance->gTrace2Color = QColor(30,30, 230).name();
  btn=btn.trimmed().toUpper();
  gridappearance->gGraphColorPreset=btn;     //ver114-2a Save for later use
  if (btn == "LIGHT")       //Black on white
  {
    gridappearance->gGridLineColor = QColor(200, 200, 200).name();
    gridappearance->gGridBoundsColor=QColor(Qt::black).name();
    gridappearance->gBackColor = QColor(Qt::white).name();
    gridappearance->gXTextColor=QColor(Qt::black).name();
    gridappearance->gGridTextColor=QColor(Qt::black).name();
    if (gPrimaryAxis==1)  //ver115-3c
    {
      gridappearance->gTrace1Color = QColor(30, 30, 230).name();
      gridappearance->gTrace2Color = QColor(250, 40, 20).name();   //blue, red
      gridappearance->gTrace1AColor = QColor(20, 160, 175).name();
      gridappearance->gTrace2AColor = QColor(230, 140, 20).name();   //dark cyan, orange
      gridappearance->gTrace1BColor = QColor(0, 150, 50).name();
      gridappearance->gTrace2BColor = QColor(200, 175, 20).name();   //green, gold
    }
    else
    {
      gridappearance->gTrace1Color = QColor(250, 40, 20).name();
      gridappearance->gTrace2Color = QColor(30, 30, 230).name();   //red, blue
      gridappearance->gTrace1AColor = QColor(230, 140, 20).name();
      gridappearance->gTrace2AColor = QColor(20, 160, 175).name();   //orange, dark cyan
      gridappearance->gTrace1BColor = QColor(200, 175, 20).name();
      gridappearance->gTrace2BColor = QColor(0, 150, 50).name();   //gold, green
    }
    gridappearance->gY1TextColor=gridappearance->gTrace1Color;
    gridappearance->gY2TextColor=gridappearance->gTrace2Color;
  }
  else if (btn == "DARK")       //White On black
  {
    gridappearance->gGridLineColor = QColor(150, 150, 140).name();
    gridappearance->gGridBoundsColor = QColor(190, 190, 180).name();
    gridappearance->gBackColor = QColor(0, 0, 0).name();
    if (gPrimaryAxis==1)  //ver115-3c
    {
      gridappearance->gTrace1Color = QColor(0, 255, 255).name();
      gridappearance->gTrace2Color = QColor(255, 200, 90).name();  //cyan,orange
      gridappearance->gTrace1AColor = QColor(100, 190, 255).name();
      gridappearance->gTrace2AColor = QColor(255, 50, 50).name();   //light blue, red
      gridappearance->gTrace1BColor = QColor(0, 255, 150).name();
      gridappearance->gTrace2BColor = QColor(255, 255, 50).name();   //green, yellow
    }
    else
    {
      gridappearance->gTrace1Color = QColor(255, 200, 90).name();
      gridappearance->gTrace2Color = QColor(0, 255, 255).name();  //orange, cyan
      gridappearance->gTrace1AColor = QColor(255, 50, 50).name();
      gridappearance->gTrace2AColor = QColor(100, 190, 255).name();   //red, light blue
      gridappearance->gTrace1BColor = QColor(255, 255, 50).name();
      gridappearance->gTrace2BColor = QColor(0, 255, 150).name();   //yellow, green
    }
    gridappearance->gXTextColor = QColor(245, 245, 240).name();
    gridappearance->gGridTextColor = QColor(245, 245, 240).name();
    gridappearance->gY1TextColor=gridappearance->gTrace1Color;
    gridappearance->gY2TextColor=gridappearance->gTrace2Color;
  }
  else if (btn == "SEPIA")    //Sepia
  {
    gridappearance->gGridLineColor = QColor(255, 230, 180).name();
    gridappearance->gGridBoundsColor = QColor(115, 85, 0).name();
    gridappearance->gBackColor = QColor(255, 255, 255).name();
    gridappearance->gXTextColor=QColor(Qt::black).name();
    gridappearance->gGridTextColor=QColor(Qt::black).name();
    if (gPrimaryAxis==1)  //ver115-3c
    {
      gridappearance->gTrace1Color = QColor(30, 30, 230).name();
      gridappearance->gTrace2Color = QColor(250, 40, 20).name();   //blue, red
      gridappearance->gTrace1AColor = QColor(20, 160, 175).name();
      gridappearance->gTrace2AColor = QColor(230, 140, 20).name();   //dark cyan, orange
      gridappearance->gTrace1BColor = QColor(200, 175, 20).name();
      gridappearance->gTrace2BColor = QColor(220, 220, 50).name();   //green, yellow
    }
    else
    {
      gridappearance->gTrace1Color = QColor(250, 40, 20).name();
      gridappearance->gTrace2Color = QColor(30, 30, 230).name();   //red, blue
      gridappearance->gTrace1AColor = QColor(230, 140, 20).name();
      gridappearance->gTrace2AColor = QColor(20, 160, 175).name();   //orange, dark cyan
      gridappearance->gTrace1BColor = QColor(220, 220, 50).name();
      gridappearance->gTrace2BColor = QColor(200, 175, 20).name();   //yellow, green
    }
    gridappearance->gY1TextColor=gridappearance->gTrace1Color;
    gridappearance->gY2TextColor=gridappearance->gTrace2Color;
  }
}

void msagraph::gUseCustomPresetColors(int N)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  sub gUseCustomPresetColors N    'Use preset color set N (1...5) 'ver116-4b added extra trace colors
      if N<1 then N=1
      if N>5 then N=5
      gGridLineColor$=gCustomColors$(N,0)
      gGridBoundsColor$=gCustomColors$(N,1)
      gBackColor$=gCustomColors$(N,2)
      gGridTextColor$=gCustomColors$(N,3)
      gXTextColor$=gCustomColors$(N,4)
      gY1TextColor$=gCustomColors$(N,5)
      gY2TextColor$=gCustomColors$(N,6)
      gTrace1Color$=gCustomColors$(N,7)
      gTrace2Color$=gCustomColors$(N,8)
      gTrace1AColor$=gCustomColors$(N,9)
      gTrace2AColor$=gCustomColors$(N,10)
      gTrace1BColor$=gCustomColors$(N,11)
      gTrace2BColor$=gCustomColors$(N,12)
      gGraphColorPreset$="Custom";N       'So we can identify what set of colors we last started with
  end sub*/
}
void msagraph::gSetCustomPresetColors(int N, QString grid, QString bounds, QString back, QString gridText, QString XText, QString Y1Text, QString Y2Text, QString trace1, QString trace2, QString trace1A, QString trace2A, QString trace1B, QString trace2B)
{
  //Set Nth (1-5) preset color set; don't actually implement this set yet
  if (N<1) N=1;
  if (N>5) N=5;
  gridappearance->gCustomColors[N][0]=grid;
  gridappearance->gCustomColors[N][1]=bounds;
  gridappearance->gCustomColors[N][2]=back;
  gridappearance->gCustomColors[N][3]=gridText;
  gridappearance->gCustomColors[N][4]=XText;
  gridappearance->gCustomColors[N][5]=Y1Text;
  gridappearance->gCustomColors[N][6]=Y2Text;
  gridappearance->gCustomColors[N][7]=trace1;
  gridappearance->gCustomColors[N][8]=trace2;
  gridappearance->gCustomColors[N][9]=trace1A;
  gridappearance->gCustomColors[N][10]=trace2A;
  gridappearance->gCustomColors[N][11]=trace1B;
  gridappearance->gCustomColors[N][12]=trace2B;
}
void msagraph::gGetCustomPresetColors(int N, QString &grid, QString &bounds, QString &back, QString &gridText, QString &XText, QString &Y1Text, QString &Y2Text, QString &trace1, QString &trace2, QString &trace1A, QString &trace2A, QString &trace1B, QString &trace2B)
{
  //Get Nth (1-5) preset color set
  if (N<1) N=1;
  if (N>5) N=5;
  N = N -1;
  grid=gridappearance->gCustomColors[N][0];
  bounds=gridappearance->gCustomColors[N][1]; //ver116-4b fixed second index of all these
  back=gridappearance->gCustomColors[N][2];
  gridText=gridappearance->gCustomColors[N][3];
  XText=gridappearance->gCustomColors[N][4];
  Y1Text=gridappearance->gCustomColors[N][5];
  Y2Text=gridappearance->gCustomColors[N][6];
  trace1=gridappearance->gCustomColors[N][7];trace2=gridappearance->gCustomColors[N][8];
  trace1A=gridappearance->gCustomColors[N][9];trace2A=gridappearance->gCustomColors[N][10];
  trace1B=gridappearance->gCustomColors[N][11];trace2B=gridappearance->gCustomColors[N][12];
}
void msagraph::gInitCustomColors()
{
//Initialize the custom colors to default values just to be valid
  for (int N=0; N < 5; N++)
  {
    //White background, dark foreground
    gridappearance->gCustomColors[N][0]="200 200 200";   //Line
    gridappearance->gCustomColors[N][1]="black";         //Bounds
    gridappearance->gCustomColors[N][2]="white";         //Background
    gridappearance->gCustomColors[N][3]="black";         //GridText
    gridappearance->gCustomColors[N][4]="black";         //XText
    gridappearance->gCustomColors[N][5]="250 40 20";     //Y1Text off-red
    gridappearance->gCustomColors[N][6]="30 30 230";     //Y2Text off-blue
    gridappearance->gCustomColors[N][7]="250 40 20";     //Trace1 off-red
    gridappearance->gCustomColors[N][8]="30 30 230";     //Trace2 off-blue
    gridappearance->gTrace1AColor="230 140 20";         //Trace1A orange
    gridappearance->gTrace2AColor="20 160 175";         //Trace 2A dark cyan
    gridappearance->gTrace1BColor="220 220 50";         //Trace1B yellow
    gridappearance->gTrace2BColor="200 175 20";         //Trace2B green
  }
}
void msagraph::gUsePresetText(QString btn)
{
  //@gUsePresetText
  //btn$ specifies the preset to be used. It may be in the form of a
  //handle, so we drop everything through the period, if there is one.
  int pos=btn.indexOf(".");
  if (pos>-1) btn=btn.mid(pos+1);
  btn=btn.trimmed().toUpper();
  gridappearance->gGraphTextPreset=btn;  //ver114-2a Save for later reference
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
    int y1, y2;
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
  if (axisNum==1) componConst=vars->Y1DataType; else componConst=vars->Y2DataType;
  int pMin, pMax;
  gGetMinMaxPointNum(pMin, pMax);
  if (componConst==vars->constGD)
  {
    //For group delay, the first point is invalid so don't include it
    if (gGetSweepDir()==1) pMin=pMin+1; else pMax=pMax-1;
  }
  //Find min and max values between points pMin and pMax
  int dum1, dum2;
  gFindPeaks(axisNum, pMin, pMax, dum1, dum2, axisMin, axisMax);

  if (componConst == vars->constGraphS11DB || componConst == vars->constMagDBM || componConst ==  vars->constMagDB
      || componConst == vars->constReturnLoss || componConst == vars->constInsertionLoss) //Round db/dbm to multiple of 5 ver114-8d
  {
    axisMin=util.uRoundDownToMultiple(axisMin-0.5, 5);
    axisMax=util.uRoundUpToMultiple(axisMax+0.5, 5);
  }
  else if (componConst == vars->constGraphS11Ang || componConst == vars->constAngle || componConst == vars->constRawAngle
           || componConst == vars->constTheta || componConst == vars->constImpedAng || componConst == vars->constAdmitAng)   //ver115-4a
  {
    //Round angles to multiple of 15 ver115-1i added raw angle
    axisMin=util.uRoundDownToMultiple(axisMin-5, 15); if (axisMin<-180) axisMin=-180; //ver114-8c
    axisMax=util.uRoundUpToMultiple(axisMax+5, 15) ; if (axisMax>180) axisMax=180;
    if (axisMax-axisMin>120) { axisMin=-180 ; axisMax=180; }  //Expand to full range  //ver114-8c
  }
  else if (componConst == vars->constSerR || componConst == vars->constParR || componConst == vars->constImpedMag)    //Round non-negative impedances to multiple of 25
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
  else if (componConst == vars->constSerReact || componConst == vars->constParReact)    //Round reactances to multiple of 25; allow negative
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
  else if (componConst == vars->constMagWatts || componConst == vars->constMagRatio || componConst == vars->constMagV || componConst == vars->constRho) //Fractional values that won//t exceed 1 or be negative
  {
    if (axisMax>1) axisMax=1;
    axisMin=util.uRoundDownToPower(axisMin, 10);
    axisMax=util.uRoundUpToPower(axisMax,10);
    if (axisMin>0) { if (axisMax/axisMin>=2) axisMin=0;}
  }
  else if (componConst == vars->constSerC || componConst == vars->constSerL || componConst == vars->constParC || componConst == vars->constParL)     //Capacitance and Inductance
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
  else if (componConst == vars->constSWR)   //VSWR >=1 and we don't care about huge values ver114-8d
  {
    axisMin=1;
    if (axisMax<2) axisMax=2; else axisMax=(int)(axisMax)+2;
    if (axisMax>50) axisMax=50;
  }
  else if (componConst == vars->constComponentQ)   //>=0 and we don't care about huge values ver115-2d
  {
    axisMin=0;
    if (axisMax<10) axisMax=10; else axisMax=(int)(axisMax)+5;
    if (axisMax>10000) axisMax=10000;
  }
  else if (componConst == vars->constReflectPower)   //a percentage ver115-2d
  {
    axisMin=0;
    axisMax=100;
  }
  else if (componConst == vars->constAdmitMag)  //ver115-4a
  {
    axisMin=0;
    axisMax=util.uRoundUpToPower(axisMax, 10);
  }
  else if (componConst == vars->constConductance || componConst == vars->constSusceptance)  //ver115-4a
  {
    if (axisMin>0) axisMin=util.uRoundDownToPower(axisMin, 10); else axisMin=util.uRoundUpToPower(axisMin, 10);
    if (axisMax>0) axisMax=util.uRoundUpToPower(axisMax, 10); else axisMax=util.uRoundDownToPower(axisMax, 10);
  }
  else if (componConst == vars->constAux0 || componConst == vars->constAux1 || componConst == vars->constAux2
           || componConst == vars->constAux3 || componConst == vars->constAux4 || componConst == vars->constAux5)   //ver115-4a
  {
    int auxNum=componConst-vars->constAux0;    //e.g. constAux4 produces 4
    axisMin=vars->auxGraphDataInfo[auxNum][1]  ; axisMax=vars->auxGraphDataInfo[auxNum][2];
  }
  else if (componConst == vars->constNoGraph)   //ver115-2c
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

void msagraph::mUpdateMarkerLocations()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  //Find point numbers for peak markers and for L and R if relative to the peaks
      //We are called from mDrawMarkerInfo, which also has the ability to move markers.
      saveSel$=selMarkerID$   //We want to save and restore the current selected marker
      axisNum$=str$(primaryAxisNum)
      if hasMarkPeakPos or hasMarkPeakNeg then    //Locate peaks
          if doPeaksBounded=1 then
              if hasMarkL=0 then pStart=1 else pStart=gMarkerPointNum(mMarkerNum("L"))
              if hasMarkR=0 then pEnd=gPointCount() else pEnd=gMarkerPointNum(mMarkerNum("R"))
          else
              pStart=1 : pEnd=gPointCount()   //Signals to search all points
          end if
          call gFindPeaks primaryAxisNum,pStart, pEnd, minNum, maxNum, minY, maxY //ver115-3b
          if hasMarkPeakPos then call gUpdateMarkerPointNum mMarkerNum("P+"),maxNum
          if hasMarkPeakNeg then call gUpdateMarkerPointNum mMarkerNum("P-"),minNum
      end if
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
}
void msagraph::gDrawGrid()
{
  QString startTime = util.time("ms");
  QString stopTime;
  graphScene->clear();


  //Clears screen and draws background grid
  //Also accumulates all the draw commands for the grid, other than the initial
  //cls and fill commands, into the string gGridString$. Text commands are not included,
  //because once the "\" character is found, everything after that is considered one piece
  //of text to be printed.
  //ver115-1b caused gGridString$ to be created even if gIsDynamic=0

  graphScene->setBackgroundBrush(QColor(gridappearance->gBackColor)); // set background to black

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

  //#gGraphHandle$, cmd$       //draw horizontal grid lines
  //gGridString$=cmd$   //ver115-1b
  //cmd$=""
  for (int i=2; i <= gHorDiv; i++)  //draw vertical lines; skip those under the boundary
  {
      //Pixel locations are in gXGridLines
      int x=gMarginLeft+gXGridLines[i];
      //cmd$=cmd$+"line "; x; " ";gOriginY; " "; x; " ";gOriginY-gGridHeight ;";"
      graphScene->addLine(x, gOriginY, x, gOriginY-gGridHeight, QPen(gridappearance->gGridLineColor));
  }

  //#gGraphHandle$, cmd$    //Draw vertical grid lines
  //gGridString$=gGridString$+cmd$  //ver115-1b

  int gridRight=gMarginLeft+gGridWidth;
  QPen pen(QColor(gridappearance->gGridBoundsColor), 3);
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
    for (unsigned int i=0; i <= uWork->uWorkNumPoints; i++) {uWork->uWorkArray[i+1][0]=vars->datatable[i][1]; }   //set up for uRLCComboResponse
      //Calc response in whatever S11 or S21 setup the user has chosen
    int doSpecialR0; QString doSpecialJig;
    if (vars->msaMode=="Reflection")
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
    for (unsigned int i=1; i <= uWork->uWorkNumPoints; i++)
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
      if (vars->msaMode=="SA") { source1=vars->constMagDBM; source2=vars->constNoGraph;}
      if (vars->msaMode=="ScalarTrans") { source1=vars->constMagDB; source2=vars->constNoGraph;}
      if (vars->msaMode=="VectorTrans") { source1=vars->constMagDB; source2=vars->constAngle;}
      if (vars->msaMode=="Reflection") { source1=vars->constGraphS11DB; source2=vars->constGraphS11Ang;}
      for (int i=1; i <= referenceSourceNumPoints; i++)
      {
        referenceSource[i][0]=gGetPointXVal(i);   //Actual tuning freq, in MHz
        //No matter what we are doing with the reference, we save dB(m)/angle data as the "source"
        //and later do any necessary "transform".
        int y1, y2;
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
    int ref1, ref2;
    CalcReferencesWholeStep(i, ref1, ref2);
    referenceTransform[i+1][0]=referenceSource[i+1][0];  //freq
    referenceTransform[i+1][1]=ref1;  //Trace 1
    referenceTransform[i+1][2]=ref2; //Trace 2
  }
}
void msagraph::CalcReferencesWholeStep(int stepNum, int &ref1, int &ref2)
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
  if (vars->msaMode=="Reflection")
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
QString msagraph::CreateReferenceTraces(QString tCol, int tSize, int traceNum, QPainterPath *path)
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
  for (unsigned int i=1; i <= uWork->uWorkNumPoints; i++)
  {
    uWork->uWorkArray[i][0]=referenceTransform[i][0];  //freq
    uWork->uWorkArray[i][1]=referenceTransform[i][1];  //Trace 1 data
    uWork->uWorkArray[i][2]=referenceTransform[i][2]; //Trace 2 data
  }

  retVal = "color "+tCol+";size "+tSize+";"+PrivateCreateReferenceTrace(traceNum, 1, gNumDynamicSteps()+1, path);
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
void msagraph::CalcGraphData(int currStep, int &y1, int &y2, int useWorkArray)
{
  //Calculate y1,y2 per user request
  //If useWorkArray=1 then the data source is uWorkArray() or uWorkReflectData;
  //otherwise it is datatable()or ReflectArray()
  //If data type is an auxiliary type, we retrieve the data from auxGraphData()
  //currStep may have a fractional part, in which case we need to interpolate.
  y1=0;
  y2=0; //ver115-4a
  if (vars->Y1DataType>=vars->constAux0 && vars->Y1DataType<=vars->constAux5)
  {
    y1=vars->auxGraphData[currStep][vars->Y1DataType-vars->constAux0];
  }
  if (vars->Y2DataType>=vars->constAux0 && vars->Y2DataType<=vars->constAux5)
  {
    y2=vars->auxGraphData[currStep][vars->Y2DataType-vars->constAux0];
  }
  if (vars->msaMode=="Reflection")
  {
    CalcReflectGraphData(currStep, y1, y2, useWorkArray);
  }
  else
  {
    if (vars->msaMode!="SA")
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
        if (c == vars->constMagDBM)
        {
          y=db;
        }
        else if (c == vars->constMagWatts)
        {
          fpow = pow(10,(db/10)); //ver115-9f
          y=fpow/1000;
        }
        else if (c == vars->constMagV)
        {
          fpow=pow(10,(db/10));  //ver115-9f
          y=sqrt(fpow/20);    //square root of 50* mw/1000
        }
        else if (c == vars->constAux0
                 || c == vars->constAux1
                 || c == vars->constAux2
                 || c == vars->constAux3
                 || c == vars->constAux4
                 || c == vars->constAux5)   //ver115-4a
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
  int nextY1;
  int nextY2;
  CalcGraphData(wholeStep+1, nextY1, nextY2, useWorkArray); //Get values at next step

  if (vars->Y1DataType!=vars->constNoGraph)
    y1=inter.LinearInterpolateDataType(vars->Y1DataType, fract, y1, nextY1);

  if (vars->Y2DataType!=vars->constNoGraph)
    y2=inter.LinearInterpolateDataType(vars->Y2DataType, fract, y2, nextY2);
}
void msagraph::CalcGraphDataType(int currStep, int dataType1, int dataType2, int &y1, int &y2, int useWorkArray)
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

void msagraph::setAppearance(dialogGridappearance *newGridappearance)
{
  gridappearance = newGridappearance;
}
void msagraph::CalcReflectDerivedData(int currStep)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Calc ReflectArray() data from S11. frequency and S11 must already be in place.
    'if currStep<0 then we get the data from uWorkReflectData(x) and put results into uWorkReflectData(x)
    'otherwise we get the data from ReflectArray() and put results into ReflectArray(currStep,x)
    'S11 is assumed to be re S11GraphR0

    if currStep<0 then  'Get frequency and S11; we calculate everything else
        trueFreq=1000000*uWorkReflectData(0)    'ver115-1f
        db=uWorkReflectData(constGraphS11DB)
        ang=uWorkReflectData(constGraphS11Ang)
    else
        trueFreq=1000000*ReflectArray(currStep,0)
        db=ReflectArray(currStep,constGraphS11DB)
        ang=ReflectArray(currStep,constGraphS11Ang)
    end if

    rho=10^(db/20)

    call uRefcoToImpedance S11GraphR0, rho, ang, serR, serReact
    call uEquivSeriesLC trueFreq, serR, serReact, serL, serC
    if abs(serReact)<0.001 then serReact=0  'ver115-5d
    if serR<0.001 then serR=0 'ver115-5d
    call uEquivParallelImped serR, serReact, parR, parReact 'Convert imped to equivalent parallel resistance and reactance ver114-7b
    if abs(parReact)<0.001 then parReact=0 'ver115-5d
    if parR<0.001 then parR=0 'ver115-5d
    if trueFreq=0 then
        parR=constMaxValue: parL=constMaxValue: parC=0   'Set for max impedance
    else
        twoPiF = 2.0*uPi() * trueFreq
        if parReact>=constMaxValue then parL=constMaxValue else  parL = parReact/twoPiF
        if parReact=0 then parC=constMaxValue else parC = -1.0/(twoPiF * parReact)
    end if

    if rho>0.999999 then swr=9999 else swr=(1+rho)/(1-rho)
        'Impose a max of 1F or 1H
    if serC>1 then serC=1
    if serL>1 then serL=1
    if parC>1 then parC=1
    if serC>1 then serC=1
    if serC<-1 then serC=-1
    if serL<-1 then serL=-1
    if parC<-1 then parC=-1
    if serC<-1 then serC=-1
    minC=1e-15 : minL=1e-12 'impose min of 1 fF and 1 pH ver115-2d
    if abs(serC)<minC then serC=0
    if abs(parC)<minC then parC=0
    if abs(serL)<minL then serL=0
    if abs(parL)<minL then parL=0

    if currStep<0 then    'Store the data in the appropriate place
        uWorkReflectData(constRho)=rho
        uWorkReflectData(constImpedMag)=sqr(serR^2+serReact^2)  'mag of impedance
        uWorkReflectData(constImpedAng)=uATan2(serR, serReact)  'angle of impedance
        uWorkReflectData(constSerR)=serR
        uWorkReflectData(constSerReact)=serReact
        uWorkReflectData(constSerC)=serC
        uWorkReflectData(constSerL)=serL
        uWorkReflectData(constParR)=parR
        uWorkReflectData(constParReact)=parReact
        uWorkReflectData(constParC)=parC
        uWorkReflectData(constParL)=parL
        uWorkReflectData(constSWR)=swr
    else
        ReflectArray(currStep,constRho)=rho
        ReflectArray(currStep,constImpedMag)=sqr(serR^2+serReact^2)  'mag of impedance
        ReflectArray(currStep,constImpedAng)=uATan2(serR, serReact)  'angle of impedance
        ReflectArray(currStep,constSerR)=serR
        ReflectArray(currStep,constSerReact)=serReact
        ReflectArray(currStep,constSerC)=serC
        ReflectArray(currStep,constSerL)=serL
        ReflectArray(currStep,constParR)=parR
        ReflectArray(currStep,constParReact)=parReact
        ReflectArray(currStep,constParC)=parC
        ReflectArray(currStep,constParL)=parL
        ReflectArray(currStep,constSWR)=swr
    end if
    */
}
void msagraph::CalcReflectGraphData(int currStep, int &y1, int &y2, int useWorkArray)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
 'Calculate y1,y2 per user request
    'If useWorkArray=1 then the data source is uWorkReflectData(); otherwise it is ReflectArray()
    'Power is in uWorkArray(currStep+1,1), phase in uWorkArray(currStep+1,2)
    'S11 db is in ReflectArray(currStep,2), phase in ReflectArray(currStep,3)

    for dataNum=2 to 1 step -1
        if dataNum=1 then
            if msaMode$="SA" or msaMode$="ScalarTrans" then exit for    'don't have phase
            graph.componConst=Y1DataType
        else
            graph.componConst=Y2DataType
        end if
        select graph.componConst
            case constGraphS11DB, constGraphS11Ang, constRho, constImpedMag, constImpedAng, constSerR,constSerReact,constParR,constParReact,_
                        constSerC,constSerL,constParC,constParL,constSWR
                'All these have already been computed
                if useWorkArray then y=uWorkReflectData(graph.componConst) else y=ReflectArray(currStep,graph.componConst)  'ver115-1e
            case constTheta     'Same as angle
                if useWorkArray then y=uWorkReflectData(constGraphS11Ang) else y=ReflectArray(currStep,constGraphS11Ang)   'ver115-1e
            case constReturnLoss
                if useWorkArray then y=0-uWorkReflectData(constGraphS11DB) else y=0-ReflectArray(currStep,constGraphS11DB) 'ver115-1e
            case constReflectPower
                if useWorkArray then y=100*uWorkReflectData(constRho)^2 else y=100*ReflectArray(currStep,constRho)^2    'ver115-2d
            case constComponentQ    'ver115-2d
                'Note that this formula works only for a single L or C. For LC combos, we would need the reactance
                'of the individual components
                if useWorkArray then
                    X=uWorkReflectData(constSerReact) : R=uWorkReflectData(constSerR)
                else
                    X=ReflectArray(currStep,constSerReact) : R=ReflectArray(currStep,constSerR)
                end if
                if R=0 then y=99999 else y=abs(X)/R 'Q=X/R

            case constAdmitMag, constAdmitAng, constConductance, constSusceptance   'ver115-4a
                select case graph.componConst
                    case constAdmitMag
                        if useWorkArray then mag=uWorkReflectData(constImpedMag) else mag=ReflectArray(currStep,constImpedMag)
                        if mag=0 then y=constMaxValue else y=1/mag
                    case constAdmitAng
                        if useWorkArray then ang=uWorkReflectData(constImpedAng) else ang=ReflectArray(currStep,constImpedAng)
                        y=0-ang
                    case else 'constConductance, constSusceptance
                        if useWorkArray then R=uWorkReflectData(constSerR) : X=uWorkReflectData(constSerReact) _
                            else R=ReflectArray(currStep,constSerR) : X=ReflectArray(currStep,constSerReact)
                        call cxInvert R, X, G, S
                        if graph.componConst=constConductance then y=G else y=S
                end select
            case constAux0, constAux1, constAux2, constAux3, constAux4, constAux5   'ver115-4a
                if dataNum=1 then y=y1 else y=y2  'Auxiliary data has already been calculated, so keep it
            case else
                y=0    'invalid data, or None
        end select
        if dataNum=1 then  'Put data into y1 or y2
            y1=y
        else
            y2=y
        end if
    next dataNum

*/
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
  //ver115-1b deleted calc of stepfreq
  gSetXAxisRange(vars->startfreq, vars->endfreq);  //ver114-6d

}
void msagraph::gDrawMarkers()        //Draw all listed markers
{
qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
/*

    'For the moment, use the grid color and font
   #gGraphHandle$, "color "; gGridTextColor$; ";font "; gGridFont$
   #gGraphHandle$, "size 1"
   gMarkerString$=""  : join$=""   'ver116-4b added gMarkerString$ to cumulate info on markers that are drawn
   for i=1 to gNumMarkers   'Send each marker to the draw routines
        cmd$=gDrawMarkerAtPointNum$(gMarkerPoints(i,0), _
                        gMarkers$(i,1), gMarkers$(i,2), gMarkers$(i,0))
        if cmd$<>"" then gMarkerString$=gMarkerString$+join$+cmd$ : join$=";"
   next i
   */
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
void msagraph::gDrawMarkerPix(QString style, QString markLabel, float x, float y)    //Draw marker at pix coord (x,y)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    'markLabel$ is the marker ID, for styles which draw the ID
    style$=Trim$(Upper$(style$))
        'We round pixels to the nearest tenth just to keep the accumulated draw commands shorter
        'If they have already been so rounded, this doesn't hurt
    x=int(10*x+0.5)/10 : y=int(10*y+0.5)/10
    #gGraphHandle$, "size 1"
    select case style$
        case "WEDGE"
            call gDrawWedgePix x,y-1
        case "LABELEDWEDGE" 'If too close to top, invert it ver114-5n
            if y<gMarginTop+15 then call gDrawMarkerPix "LabeledInvertedWedge",markLabel$, x,y : exit sub
            call gPrintTextCentered markLabel$,x+1,y-9
            call gDrawWedgePix x,y-1
        case "INVERTEDWEDGE"
            call gDrawInvertedWedgePix x,y+1
        case "LABELEDINVERTEDWEDGE" 'If too close to bottom, un-invert it ver114-5n
            if y>gOriginY-15 then call gDrawMarkerPix "LabeledWedge",markLabel$, x,y : exit sub
            call gPrintTextCentered markLabel$,x+1,y+19
            call gDrawInvertedWedgePix x,y+1
        case "SMALLINVERTEDWEDGE"
            call gDrawSmallInvertedWedgePix, x, y+1
        case "HALTPOINTER"  'ver114-5m
            call gDrawHaltPointerPix, x, y+1 'ver114-5m
        case "XOR"  'Draw black box in XOR mode to invert colors; Do it twice to restore to original ver116-4h
            cmd$="rule xor; size 1; color black; place ";x-3;" ";y-3;";down;box ";x+3;" ";y+3;";rule over"
            #gGraphHandle$, cmd$ : #gGraphHandle$, "discard"   'Actual draw, and discard so lots of these don't build up
        case else
    end select
end sub
*/
}
void msagraph::gRefreshTraces()  //Redraw traces from gTrace1$() and gTrace2$()
{
  //gTrace1$() and gTrace2$() will each have gNumPoints points, some
  //of which may be just "down".
  //ver114-6d: Grouping the commands into groups of six per string took slightly longer
  //than going one-by-one, so we do the latter.
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

    graphScene->addPath(trace1, pen);

  }
  if (gGraphY2 && gTrace2Width>0)
  {
    QPainterPath trace2;
    trace2.moveTo(gTrace2.at(pMin));
    for (int j=pMin+1; j <= pMax; j++)
    {
      trace2.lineTo(gTrace2.at(j));
    }
    pen.setBrush(QColor(gridappearance->gTrace2Color));
    pen.setWidth(gTrace2Width);

    graphScene->addPath(trace2,pen);
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
void msagraph::CalcTransmitGraphData(int currStep, int &y1, int &y2, int useWorkArray)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  sub CalcTransmitGraphData currStep, byref y1, byref y2, useWorkArray  'Calculate y1,y2 per user request
      'If useWorkArray=1 then the data source is uWorkArray(); otherwise it is S21DataArray()
          'ver116-1b deleted calc of trueFreq, which was incorrect and not needed
      if useWorkArray then
          db=uWorkArray(currStep+1,1)
          ang=uWorkArray(currStep+1,2)
      else
          db=S21DataArray(currStep,1)     'ver115-2b
          ang=S21DataArray(currStep,2)     'ver115-2b
      end if
          'ver115-1e deleted impedance transform for transmission mode
      for dataNum=1 to 2  'ver115-3b
          if dataNum=1 then
              graph.componConst=Y1DataType
          else
              graph.componConst=Y2DataType
          end if
          select graph.componConst
              case constMagDB
                  y=db
              case constMagDBM    'This gives raw data before cal was applied
                    'Used for transmission mode only
                  if applyCalLevel=0 then y=db else y=db+lineCalArray(currStep,1)
              case constMagRatio
                  y=10^(db/20)
              case constInsertionLoss
                  y=0-db
              case constAngle
                  'Source data is raw
                  y=ang
              case constRawAngle  ' raw phase before cal. Used for transmission mode only
                  if applyCalLevel=0 then y=ang else y=gNormalizePhase(ang+lineCalArray(currStep,2)) 'ver116-4b

              case constGD    'calc group delay
                  if startfreq=endfreq then
                      message$= "Can't calculate Group Delay with zero sweep width." : call PrintMessage
                      y=-1
                  else
                      call gGetSweepStartAndEndPointNum pStart, pEnd
                      dir=gGetSweepDir()  '1 or -1
                      if currStep=pStart-1 then
                          y=-1  'Need two points; only have one at first point
                      else
                          if useWorkArray then y=(uWorkArray(currStep+1,2)-uWorkArray(currStep+1-dir,2))/360 _
                                  else y=(S21DataArray(currStep,2)-S21DataArray(currStep-dir,2))/360  'delta phase in cycles  ver116-1b
                          if y>0.5 then y=y-1 else if y<-0.5 then y=y+1     'Deal with wraparound ver114-6k
                          if useWorkArray then deltaF=1000000*(uWorkArray(currStep+1,0)-uWorkArray(currStep+1-dir,0)) _
                                  else deltaF=1000000*(S21DataArray(currStep,0)-S21DataArray(currStep-dir,0)) 'delta freq, cycles per second ver116-1b
                          y=0-y/deltaF   'negative of delta phase over delta freq
                      end if
                  end if
              case constAux0, constAux1, constAux2, constAux3, constAux4, constAux5   'ver115-4a
                  if dataNum=1 then y=y1 else y=y2  'Auxiliary data has already been calculated, so keep it
              case else
                  y=0    'invalid data, or None
          end select
          if dataNum=1 then  'Put data into y1 or y2
              y1=y
          else
              y2=y
          end if
      next dataNum
  end sub
      */
}
