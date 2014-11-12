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
#include "dialogvnacal.h"
#include "dialogoslcal.h"
#include "interpolation.h"
#include "libusbfunctions.h"
#include "usbfunc.h"
#include "dialogspecialtests.h"

#define USELIBUSB 1

//(0-39)sw0-sw39,(40-44)w0-w4,(45)base,(46)actualdds1output
class cDDS
{
public:
  quint32 array[45];
  quint32  base;
  double freq;
};

class cPLL
{
public:
  //(0-23)N23thruN0,(24-39)notused,(40)pdf1,(43)LO1freq,(45)ncounter,(46)Fcounter,(47)Acounter,(48)Bcounter. ver111-30a
  quint32 array[40];
  double pdf;
  double freq;
  quint32 ncounter;
  quint32 fcounter;
  quint32 Acounter;
  quint32 Bcounter;
};

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
  void resizeArrays(int newSize);
  void initVars();

  //usbFunctions *usb;
  usbFunc *usb;
  touchStone touch;
  dialogCalManager *calMan;
//  coaxAnalysis coax;
  dialogOSLCal oslCal;

  void RecalcPlaneExtendAndR0AndRedraw();
  void DisplayTitleWindow(int doTwoPort);
  void titleCancel();
  void titleFinished();
  void ClearAuxData();
  void finished();
  void SpecialTests();
  void CloseSpecial();
  void CavityFilterTest();
  void CloseCavityFilterTest();
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

  void CommandCurrentStep(int step);
  void SelectVideoFilter();
  void SelectLatchedSwitches(int desiredFreqBand);
  int switchLatchBits(int desiredFreqBand);
  void SelectFilter(int &fbank);
  void CommandFilter(int &fbank);

  void AutoGlitchtime();
  void ReadMagnitude();
  void InvertPDmodule(int step);
  void VideoGlitchPDM();
  void CalPDMinvdeg();
  void ReadADCviaUSB();
  float Equiv1GFreq(float f, int aBand);
  float ActualSignalFrequency(float f, int aBand);
  void CalculateAllStepsForLO1Synth();
  void CalculateAllStepsForLO3Synth();
  void FillPLL1array(int step);
  void FillPLL3array(int step);
  void FillDDS1array(int step);
  void FillDDS3array(int step);
  void CreateCmdAllArray();
  void CommandPLL(int step);

  void CommandPLLslimUSB();

  void DetermineModule(int step);
  void CommandPDMonly();
  void CommandPDMSlimUSB() ;
  void CommandAllSlimsUSB() ;


  void ConvertPhadata();
  void ConvertMagPhaseData();
  void DoSpecialGraph(float &power);
  float gGetPointXVal(Q2DfloatVector &GraphVal, int MaxPoints, float N);


  void frontEndInterpolateToScan();
  void CopyModeDataToVNAData(int doIntermed);
  void autoWaitPrecalculate();
  void ReadStep();
  void ProcessAndPrintLastStep();

  void specTestToggleVisible();



  int port;
  int status;
  int control;

  int globalSTRB, globalINIT, globalSELT, globalContClear;
  int swclk;
  int LEPLL;
  int sfqud;

  int pdmlowlim, pdmhighlim;
  int bUseUsb;
  int glitch, glitchp1, glitchd1, glitchp3, glitchd3, glitchpdm, glitchhlt;

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

  float appxpdf;
  int datavalue;
  int levalue;

  QVector <cPLL> pll_1;

  double LO1;
  double pdf;
  int ncounter;
  int Bcounter;
  int Acounter;
  int fcounter;


  int ncounter1;
  int fcounter1;
  int rcounter;
  int rcounter1;



  QVector <cPLL> pll_2;
  double LO2;
  double pdf2;
  int ncounter2;
  int Bcounter2;
  int Acounter2;
  int fcounter2;
  int rcounter2;
  //float appxLO2;



  QVector <cPLL> pll_3;
  double LO3;
  double pdf3;
  int ncounter3;
  int fcounter3;
  int lastfcounter3;
  int lastncounter3;
  int rcounter3;

  int lastfcounter1;
  int lastncounter1;
  float appxVCO;


  QVector <cDDS> dds_1;
  QVector <cDDS> dds_3;


  int phasepolarity;
  int fractional;
  int preselector;
  int Jcontrol;


  int filtbank;

  int pdmcmd;


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
  dialogSpecialTests *specTests;


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
  void ConvertSync(int step);
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
  float freqerror;

  float ddsoutput;
  float ddsclock;
  int PLL;
  quint32 base;
  float power;

  UsbAllSlimsAndLoadData data;


  nValues n;



  int nb0;
  int nb1;
  int nb2;
  int nb3;
  int nb4;
  int nb5;
  int nb6;
  int nb7;
  int nb8;
  int nb9;
  int nb10;
  int nb11;
  int nb12;
  int nb13;
  int nb14;
  int nb15;
  int nb16;
  int nb17;
  int nb18;
  int nb19;
  int nb20;
  int nb21;
  int nb22;
  int nb23;

  int na0;
  int na1;
  int na2;
  int na3;
  int na4;


  int ra0;
  int ra1;
  int ra2;
  int ra3;
  int ra4;
  int ra5;
  int ra6;
  int ra7;
  int ra8;
  int ra9;
  int ra10;
  int ra11;
  int ra12;
  int ra13;

  quint32 sw0, sw1, sw2, sw3, sw4, sw5, sw6, sw7, sw8, sw9, sw10;
  quint32 sw11, sw12, sw13, sw14, sw15, sw16, sw17, sw18, sw19, sw20;
  quint32 sw21, sw22, sw23, sw24, sw25, sw26, sw27, sw28, sw29, sw30;
  quint32 sw31, sw32, sw33, sw34, sw35, sw36, sw37, sw38, sw39;

  quint32 b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10;
  quint32 b11, b12, b13, b14, b15, b16, b17, b18, b19, b20;
  quint32 b21, b22, b23, b24, b25, b26, b27, b28, b29, b30;
  quint32 b31, b32, b33, b34, b35, b36, b37, b38, b39;


  quint32 w0;
  quint32 w1;
  quint32 w2;
  quint32 w3;
  quint32 w4;

  int le1, le2,le3,fqud1,fqud3;



public slots:
  void setSwitches(int );
  void CommandFilterSlimCBUSB(int);
  void setDDS1(double );


signals:
  void DisplayButtonsForHalted();
  void DisplayButtonsForRunning();
  void RequireRestart();
  void CleanupAfterSweep();
  void ChangeMode();
  void SkipHardwareInitialization();
  void ResizeArrays(int);
  void ProcessAndPrint();
  void PrintMessage(QString);
  void Halted();
};

#endif // HWDINTERFACE_H
