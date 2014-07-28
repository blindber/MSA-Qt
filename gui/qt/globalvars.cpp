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
#include "globalvars.h"
#include "constants.h"

globalVars::globalVars()
{
  zeroVars();

  VNAData.mresize(2,3);

  resizeArrays(802);

  //The following constant values are used to specify graph data,
  //and assigned in such a way that the first ones can be
  //used as the index into ReflectArray(), which holds a bunch of pre-calculated values for reflection mode.
  //Note if these are changed, FilterDataType may have to be changed. //ver115-1b altered these
  //We start with 1, because entry 0 of ReflectArray is frequency

  maxNumSteps=40000;       //ver114-3e
  maxPointExtraLines=100;  //ver116-4k


  doCycleTraceColors = 0;
  cycleNumber = 0;

  multiscanMaxNum=4;   //This is fixed

  constMaxValue=1e12;

  calfigModuleVersion = 0;
  bUseUsb = 0;

  thisstep = 0;
  test = 0;

  doingInitialization = 1;

  steps = 400;
  globalSteps = 400;

  msaMode = modeSA;
  menuMode = modeSA;

}
int globalVars::DataTypeIsAngle(int dataType)
{
  int retVal;
  //Return 1 if dataType is an angle; otherwise 0
  if (dataType == constGraphS11Ang
      || dataType == constImpedAng
      || dataType == constAdmitAng
      || dataType == constAngle
      || dataType == constRawAngle)
  {
    retVal = 1;
  }
  else if(dataType == constAux0
          || dataType == constAux1
          || dataType == constAux2
          || dataType == constAux3
          || dataType == constAux4
          || dataType == constAux5)
  {
    retVal=auxGraphDataInfo[dataType-constAux0][0];    //This has info on auxiliary data ver116-1b
  }
  else //non-angles
  {
    retVal=0;
  }
  return retVal;
}

void globalVars::zeroVars()
{
  //****************************
  // clear all variables because thats what Liberty basic did
  centfreq = 0;
  sweepwidth = 0;
  glitchtime = 0;
  Y1Top = 0;
  Y1Bot = 0;
  Y2Top = 0;
  Y2Bot = 0;
  PLL = 0;
  doingPDMCal = 0;
  CalInvDeg = 0;
  cftest = 0;
  steps = 0;
  thisstep = 0;
  globalSteps = 0;
  //varwindow = 0;
  datawindow = 0;
  doSpecialGraph = 0;
  doSpecialRandom = 0;
  //LO2 = 0;
  suppressPhase = 0;
  menuOperatingCalShowing = 0;
  menuMultiscanShowing = 0;
  menuTwoPortShowing = 0;
  menuOptionsPosition = 0;
  menuDataPosition = 0;
  menuFunctionsPosition = 0;
  menuOperatingCalPosition = 0;
  menuMultiscanPosition = 0;
  menuTwoPortPosition = 0;
  menuModePosition = 0;
  menuDataS21ID = 0;
  menuDataLineCalID = 0;
  menuDataS11ID = 0;
  menuDataS11DerivedID = 0;
  menuDataLineCalRefID = 0;
  menuDataLineCalOSLID = 0;
  menuOptionsSmithID = 0;
  menuFunctionsFilterID = 0;
  menuFunctionsCrystalID = 0;
  menuFunctionsMeterID = 0;
  menuFunctionsRLCID = 0;
  menuFunctionsCoaxID = 0;
  menuFunctionsGenerateS21ID = 0;
  menuFunctionsGroupDelayID = 0;
  crystalLastUsedID = 0;
  sweepDir = 0;
  sweepStartStep = 0;
  sweepEndStep = 0;
  alternateSweep = 0;
  RefRLCLastNumPoints = 0;
  analyzeQLastNumPoints = 0;
  GDLastNumPoints = 0;
  doingInitialization = 0;
  Y1DisplayMode = 0;
  Y2DisplayMode = 0;
  isStickMode = 0;
  specialOneSweep = 0;
  returnBeforeFirstStep = 0;
  haltedAfterPartialRestart = 0;
  primaryAxisNum = 0;
  Y1DataType = 0;
  Y2DataType = 0;

  doFilterAnalysis = 0;
  x1DBDown = 0;
  x2DBDown = 0;
  maxNumSteps = 0;
  maxPointExtraLines = 0;
  VNADataNumSteps = 0;
  VNADataLinear = 0;
  VNADataZ0 = 0;
  VNARestoreDoR0AndPlaneExt = 0;
  startfreq = 0;
  endfreq = 0;
  baseFrequency = 0;
  wate = 0;
  planeadj = 0;
  thisfreq = 0;
  freqBand = 0;
  bandEnd1G = 0;
  bandEnd2G = 0;
  lastSetBand = 0;
  sgout = 0;
  test = 0;
  spurcheck = 0;
  gentrk = 0;
  normrev = 0;
  offset = 0;
  FiltA0 = 0;
  FiltA1 = 0;
  suppressPDMInversion = 0;
  leftstep = 0;
  userFreqPref = 0;
  validPhaseThreshold = 0;
  calCanUseAutoWait = 0;
  useAutoWait = 0;
  calADCofLowFringe = 0;
  calLowADCofCenterSlope = 0;
  calHighADCofCenterSlope = 0;
  calLowEndSlope = 0;
  calCenterSlope = 0;
  calHighEndSlope = 0;
  autoWaitTC = 0;
  autoWaitMaxChangeLowEndADC = 0;
  autoWaitMaxChangeCenterADC = 0;
  autoWaitMaxChangeHighEndADC = 0;
  autoWaitMaxChangePhaseADC = 0;
  transLastSteps = 0;
  transLastStartFreq = 0;
  transLastEndFreq = 0;
  transLastIsLinear = 0;
  transLastGraphR0 = 0;
  transLastY1Type = 0;
  transLastY1Top = 0;
  transLastY1Bot = 0;
  transLastY1AutoScale = 0;
  transLastY2Type = 0;
  transLastY2Top = 0;
  transLastY2Bot = 0;
  transLastY2AutoScale = 0;
  refLastSteps = 0;
  refLastStartFreq = 0;
  refLastEndFreq = 0;
  refLastIsLinear = 0;
  refLastGraphR0 = 0;
  refLastY1Type = 0;
  refLastY1Top = 0;
  refLastY1Bot = 0;
  refLastY1AutoScale = 0;
  refLastY2Type = 0;
  refLastY2Top = 0;
  refLastY2Bot = 0;
  refLastY2AutoScale = 0;
  restoreIsValidation = 0;
  restoreLastLineNum = 0;
  videoFilterAddress = 0;
  videoMagCap = 0;
  videoPhaseCap = 0;
  videoMagTC = 0;
  videoPhaseTC = 0;
  switchFR = 0;
  switchTR = 0;
  prevFreqMode = 0;
  prevStartF = 0;
  prevEndF = 0;
  prevBaseF = 0;
  prevXIsLinear = 0;
  prevY1IsLinear = 0;
  prevY2IsLinear = 0;
  prevSteps = 0;
  prevSweepDir = 0;
  prevAlternate = 0;
  prevStartY1 = 0;
  prevEndY1 = 0;
  prevStartY2 = 0;
  prevEndY2 = 0;
  prevHorDiv = 0;
  prevVertDiv = 0;
  prevY1Disp = 0;
  prevY2Disp = 0;
  prevGenTrk = 0;
  prevSpurCheck = 0;
  prevTGOff = 0;
  prevSGFreq = 0;
  prevPlaneAdj = 0;
  prevY1DataType = 0;
  prevY2DataType = 0;
  prevAutoScaleY1 = 0;
  prevAutoScaleY2 = 0;
  prevDataChanged = 0;
  prevS21JigR0 = 0;
  prevS21JigShuntDelay = 0;
  prevS11BridgeR0 = 0;
  prevS11GraphR0 = 0;
  prevSwitchFR = 0;
  functSaveAlternate = 0;
  functSaveSweepDir = 0;
  functSavePlaneAdj = 0;
  functSaveWate = 0;
  functSaveAutoWait = 0;
  lineCalThroughDelay = 0;
  OSLdoneO = 0;
  OSLdoneS = 0;
  OSLdoneL = 0;
  OSLcalLastUsedFull = 0;
  OSLLastSelectedCalSet = 0;
  OSLCalSetNumber = 0;
  OSLError = 0;
  OSLBaseStartFreq = 0;
  OSLBaseEndFreq = 0;
  OSLBaseNumSteps = 0;
  OSLBaseLinear = 0;
  OSLBandStartFreq = 0;
  OSLBandEndFreq = 0;
  OSLBandNumSteps = 0;
  OSLBandLinear = 0;
  OSLBaseS11BridgeR0 = 0;
  OSLBandS11BridgeR0 = 0;
  OSLBaseS21JigR0 = 0;
  OSLBandS21JigR0 = 0;
  installedOSLBaseLinear = 0;
  doCycleTraceColors = 0;
  cycleNumber = 0;
  calInProgress = 0;
  constMaxValue = 0;
  frontEndCalNumPoints = 0;
  suppressHardware = 0;
  suppressHardwareInitOnRestart = 0;
  multiscanCurrNum = 0;
  multiscanMaxNum = 0;
  multiscanIsOpen = 0;
  multiscanInProgress = 0;
  multiscanHaltAtEnd = 0;
  multiscanSaveRefreshEachScan = 0;

}

void globalVars::resizeArrays(int newSize)
{
  datatable.mresize(newSize,5);    //added element for band ver116-4s
  magarray.mresize(newSize,4);
  phaarray.mresize(newSize,5);
  lineCalArray.mresize(newSize,3);



  datatable.mresize(newSize,5);    //added element for band ver116-4s
  magarray.mresize(newSize,4);
  phaarray.mresize(newSize,5);
  lineCalArray.mresize(newSize,3);
//  PLL1array.mresize(newSize,49);
  //PLL3array.mresize(newSize,49);
  //DDS1array.mresize(newSize,49);
  //DDS3array.mresize(newSize,49);
  //cmdallarray.mresize(newSize,40);


  freqCorrection.resize(newSize);    //SEWgraph1
  frontEndCorrection.resize(newSize); //ver115-9c
  ReflectArray.mresize(newSize,17);   //ver115-2d
  S21DataArray.mresize(newSize, 4);  //ver116-1b
  bandLineCal.mresize(newSize, 3);  //ver114-5f
  OSLa.mresize(newSize, 2);  //ver115-1b
  OSLb.mresize(newSize, 2);  //ver115-1b
  OSLc.mresize(newSize, 2);  //ver115-1b
  OSLstdOpen.mresize(newSize,2);  //ver115-1b
  OSLstdLoad.mresize(newSize,2);  //ver115-1b
  OSLcalOpen.mresize(newSize,2);  //ver115-1b
  OSLcalLoad.mresize(newSize,2);  //ver115-1b
  OSLcalShort.mresize(newSize,2);  //ver115-1b
  OSLBandA.mresize(newSize,2); OSLBandB.mresize(800,1); OSLBandC.mresize(800,1);       //ver115-4a
  OSLBandRef.mresize(newSize,3);        //ver115-4a
  auxGraphData.mresize(newSize, 6);    //ver115-4a

  frontEndCorrection.reserve(newSize);

}
