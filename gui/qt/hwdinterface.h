#ifndef HWDINTERFACE_H
#define HWDINTERFACE_H

#include "dialogconfigman.h"
#include "globalvars.h"
#include "msautilities.h"
#include "uwork.h"
#include "lptfunctions.h"
#include "usbfunctions.h"
#include "touchstone.h"
#include "dialogcalmanager.h"
#include "coaxanalysis.h"
#include "dialogvnacal.h"
#include "dialogoslcal.h"
#include "interpolation.h"


class hwdInterface  : public QObject
{
   Q_OBJECT
public:
  hwdInterface(QWidget *parent);
  ~hwdInterface();
  void setActiveConfig(msaConfig *newActiveConfig);
  void setGlobalVars(globalVars *newVar);
  void setUwork(cWorkArray *newuWork);
  void setFilePath(QString Dir);
  void setVna(dialogVNACal *newVnaCal);



  usbFunctions *usb;
  touchStone touch;
  dialogCalManager *calMan;
  coaxAnalysis coax;
  dialogOSLCal oslCal;

  void RecalcPlaneExtendAndR0AndRedraw();
  void DisplayTitleWindow(int doTwoPort);
  void titleCancel();
  void titleFinished();
  void ClearAuxData();
  void Showvar();
  void updatevar();
  void Closevarwin();
  void finished();
  void SpecialTests();
  void CloseSpecial();
  void CavityFilterTest();
  void CloseCavityFilterTest();
  void OpenDataWindow();
  void CloseDataWindow(QString hndl);
  void MSAinputData();
  void MagnitudePhaseMSAinput();
  void MagPhaS21();
  void MagPhaS11();
  void DataWin_GraphData();
  void LineCalArray();
  void DataWin_OSL();
  void ReflectDerivedData();
  QString AlignedReflectData(int currStep);
  void FindPeakSteps(int dataType, int startStep, int endStep, int &minNum, int &maxNum, int &minY, int &maxY);    //find positive and negative peak



  //void setGraph(msagraph *graphNew);

  void CreatePLL2N();
  void CreateRcounter();
  void CommandPLL3R();
  void CreateIntegerNcounter();
  void ResetDDS1serUSB();
  void ResetDDS3serUSB();
  void SyncSweep();
  void CommandPLL1R();
  void CommandPLL2R();

  void InitializeHardware();

  void CommandCurrentStep();
  void SelectVideoFilter();
  void SelectLatchedSwitches(int desiredFreqBand);
  int switchLatchBits(int desiredFreqBand);
  void SelectFilter(int &fbank);
  void CommandFilter(int &fbank);
  void CommandFilterSlimCBUSB(int &fbank);
  void AutoGlitchtime();
  void ReadMagnitude();
  void InvertPDmodule();
  void VideoGlitchPDM();
  void CalPDMinvdeg();
  void ReadADCviaUSB();
  float Equiv1GFreq(float f, int aBand);
  float ActualSignalFrequency(float f, int aBand);
  void CalculateAllStepsForLO1Synth();
  void CalculateAllStepsForLO3Synth();
  void FillPLL1array();
  void FillPLL3array();
  void FillDDS1array();
  void FillDDS3array();
  void CreateCmdAllArray();
  void CommandPLL();

  void CommandPLLslimUSB();

  void DetermineModule();
  void CommandPDMonly();
  void CommandPDMSlimUSB() ;
  void CommandAllSlimsUSB() ;
  void ProcessDataArrays();
  void TransferToDataArrays();
  void ConvertPhadata();
  void ConvertMagPhaseData();
  void CalcMagpowerPixel();
  void DoSpecialGraph();
  void ConvertRawDataToReflection(int currStep);
  void ApplyExtensionAndTransformR0(float freq, float &db, float &ang);
  void frontEndInterpolateToScan();
  void CopyModeDataToVNAData(int doIntermed);
  void autoWaitPrecalculate();
  void ReadStep();
  void ProcessAndPrintLastStep();



  int port;
  int status;
  int control;

  int globalSTRB, globalINIT, globalSELT, globalContClear;  //ver116-1b
  int swclk;
  int LEPLL;
  int sfqud;

  int le1, le2,le3,fqud1,fqud3; //ver111-31b

  int pdmlowlim, pdmhighlim;
  int bUseUsb;
  int glitch, glitchp1, glitchd1, glitchp3, glitchd3, glitchpdm, glitchhlt; //ver111-27

  int haltWasAtEnd;
  int syncsweep;
  int setpdm;
  int convdatapwr;
  int vnalintest;
  int test;
  int cftest;

  int magdata;
  int phadata;
  int onestep;
  int startTime;

  int scanResumed; //used to indicate whether we start with a new scan(0) or resume where we left off(1)SEW
  int suppressSweepTime;

  int pdf;
  int rcounter;
  float appxpdf;
  int pdf3;
  int datavalue;
  int levalue;
  float pdf2;
  float LO1;
  float appxVCO;
  float appxLO2;
  int ncounter2;
  int ncounter;
  int fcounter;
  int fcounter1;
  int lastfcounter1;
  int fcounter2;
  int fcounter3;
  int lastfcounter3;

  int lastncounter1;
  int lastncounter3;
  int ncounter1;
  int ncounter3;
  int rcounter3;
  int rcounter2;
  int rcounter1;
  int phasepolarity;
  int fractional;
  int preselector;
  int Jcontrol;


  int filtbank;

  int pdmcmd;

  int Bcounter;
  int Bcounter2;
  int Acounter;
  int Acounter2;
  int LO2;

  QString error;
  QString errora;




  //===================START CONTEXTS MODULE=======================
    QFile *OpenContextFile(QString fName, QString inout);

  //===================END CONTEXTS MODULE=========================

private:

  msaConfig *activeConfig;
  globalVars *vars;
  msaUtilities util;
  cWorkArray *uWork;
  lptFunctions lpt;
  dialogVNACal *vnaCal;

  interpolation inter;

  void CommandRBuffer();
  void CreateFractionalNcounter();
  void AutoSpur();
  void ManSpur();
  void CreatePLL1N();
  void CreatePLL3N();
  void CreateNBuffer();
  void Command2325R();
  void Create2325N();
  void Command2326R();
  void Create2326N();
  void Command2350R();
  void Create2350N();
  void Command2353R();
  void Create2353N();
  void Command4112R();
  void Create4112N();
  void CreateBaseForDDSarray();

  void CommandDDS1();
  void CommandDDS3();
  void DDS3Track();
  void DDS1Sweep();
  void ChangePDM();
  void PdmNorm();
  void PdmInv();
  void SyncTestPDM();

  void ConvertDataToPower();
  void ConvertSync();
  void PresetVNAlin();
  void VNAlinTest();
  void ConvertPDMlin();
  void ReadPhase();

  void WaitStatement();
  void CommandLO2forCavTest();
  void FunctChangeAndSaveSweepParams();
  void FunctSetVideoAndAutoWait();
  void FunctRestoreVideoAndAutoWait();
  void FunctRestoreSweepParams();
  void FillRegularGraphData(int axisNum);



  QString DefaultDir;
  float enterPLL2phasefreq;
  float difPhase;
  Int64N cmdForUsb;
  float ddsoutput;
  float ddsclock;

public slots:


signals:
  void DisplayButtonsForHalted();
  void DisplayButtonsForRunning();
  void RequireRestart();
  void CleanupAfterSweep();
  void ChangeMode();
  void SkipHardwareInitialization();
  void ResizeArrays(int);
  void ProcessAndPrint();
  void PrintMessage();
};

#endif // HWDINTERFACE_H
