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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#if (QT_VERSION < 0x050000)
#include <QtGui>
#else
#include <QtWidgets>
#endif
#include <QVariant>

#include <QGraphicsScene>

#include "dialogconfigman.h"
#include "usbfunctions.h"
#include "globalvars.h"
#include "referencedialog.h"
#include "msautilities.h"
#include "touchstone.h"
#include "uwork.h"
#include "dialogcalmanager.h"
#include "interpolation.h"
#include "dialogoslcal.h"
#include "lptfunctions.h"
#include "dialoggridappearance.h"
#include "coaxanalysis.h"
//#include "dialogcomponentmeasure.h"
//#include "dialogcrystal.h"
#include "dialogvnacal.h"
#include "graph.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  


private:
  Ui::MainWindow *ui;
  QGraphicsScene scene;
  dialogConfigMan *winConfigMan;
  usbFunctions *usb;
  globalVars *vars;
  QTimer *timerStart;
  QTimer *timerStart2;
  msaUtilities util;
  touchStone touch;
  cWorkArray uWork;
  dialogCalManager calMan;
  interpolation inter;
  dialogOSLCal oslCal;
  lptFunctions lpt;
  dialogGridappearance gridappearance;
  coaxAnalysis coax;
  //dialogCrystal xtal;
  dialogVNACal vnaCal;
  msagraph graph;




  void InitializeHardware();
  void SkipHardwareInitialization();
  void StartSweep();
  void CommandThisStep();
  void PostScan();
  void IncrementOneStep();
  void Halted();
  void FinishSweeping();
  void ReverseSweepDirection();
  void CommandCurrentStep();
  void FindClientOffsets();
  void ResizeGraphHandler();
  void ImplementDisplayModes();
  void UpdateGraphParams();
  void UpdateGraphDataFormat(int doTwoPort);
  void DetermineGraphDataFormat(int componConst, QString &yAxisLabel, QString &yLabel, int &yIsPhase, QString &yForm);
  void InitGraphParams();
  void ReadStep();
  void ProcessAndPrintLastStep();
  void WaitStatement();
  void AutoGlitchtime();
  void ReadMagnitude();
  void ReadPhase();
  void InvertPDmodule();
  void VideoGlitchPDM();
  void CalPDMinvdeg();
  void ReadADCviaUSB();
  void ProcessAndPrint();
  void ProcessDataArrays();
  void TransferToDataArrays();
  void CalcFreqCorrection();
  void ConvertPhadata();
  void ConvertMagPhaseData();
  void CalcMagpowerPixel();
  void DoSpecialGraph();
  void ConvertRawDataToReflection(int currStep);
  void ApplyExtensionAndTransformR0(float freq, float &db, float &ang);
  void PlotDataToScreen();
  void CreateRcounter();
  void CommandPLL1R();
  void CommandPLL2R();
  void CommandPLL3R();
  void CommandRBuffer();
  void CreateIntegerNcounter();
  void CreateFractionalNcounter();
  void AutoSpur();
  void ManSpur();
  void CreatePLL1N();
  void CreatePLL2N();
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
  void ResetDDS1serUSB();

  void ResetDDS3serUSB();
  void CommandDDS1();
  void CommandDDS3();
  void DDS3Track();
  void DDS1Sweep();
  void ChangePDM();
  void PdmNorm();
  void PdmInv();
  void SyncTestPDM();
  void SyncSweep();
  void ConvertDataToPower();
  void ConvertSync();
  void PresetVNAlin();
  void VNAlinTest();
  void ConvertPDMlin();
  void RequireRestart();
  void DisplayButtonsForRunning();
  void DisplayButtonsForHalted();
  void GetDialogPlacement();


  void InstallSelectedLineCal();
  int CreateOperatingCalFolder();
  void SaveBaseLineCalFile();
  QFile *OpenBaseLineCalFile();
  int LoadBaseLineCalFile();
  void FunctChangeAndSaveSweepParams();
  void FunctSetVideoAndAutoWait();
  void FunctRestoreVideoAndAutoWait();
  void FunctRestoreSweepParams();

  void menuRunConfig();
  void menuRunCal();
  void CreateGraphWindow();
  void menuQuitMultiscan();
  void ConformMenusToMode();
  void mMarkToCenter();
  void menuExpandSweep();
  void Expand();
  void btnRedraw();
  void RedrawGraph(int restoreErase);
  void RememberState();
  void DetectFullChanges();
  void ToggleTransmissionReflection();
  void CopyModeDataToVNAData(int doIntermed);
  void ChangeMode();
  void RestartSATGmode();
  void RestartPlainSAmode();
  void RestartSAmodes();
  void GoSAmode();
  void RestartTransmissionMode();
  void GoTransmissionMode();
  void RestartReflectionMode();
  void GoReflectionMode();
  void SetYAxes(int data1, int top1, int bot1, int auto1, int data2, int top2, int bot2, int auto2);
  void SetDefaultGraphData();
  void GetDefaultGraphData(int axisNum, int &axisType, int &axisMin, int &axisMax);
  void ResizeArrays(int nPoints);
  void LeftButDouble();
  void preupdatevar();
  void RightButDown();
  void MarkerClick();
  void StartingLimits();




  void FillRegularGraphData(int axisNum);
  void FillAppearancesArray();

  void SetCenterSpanFreq(float cent, float span);
  void RecalcPlaneExtendAndR0AndRedraw();
  void DisplayTitleWindow(int doTwoPort);
  void titleCancel();
  void titleFinished();
  int mMarkerNum(QString markID);
  void mDeleteMarker(QString markID);
  void mAddMarker(QString markID, int pointNum, QString trace);
  void mAddMarkerAndRedraw(QString markID, int ptNum, int traceNum);
  void mAddMarkerFromKeyboard();
  void mClearMarkers();

  void mUpdateMarkerEditButtons();
  void mUserMarkSelect(QString btn);
  void mMarkSelect(QString markID);
  void mBtnMarkClear(QString markID);
  void mBtnMarkEdit(QString markID);
  void mBtnMarkDelete(QString markID);
  void btnIncPoint();
  void btnDecPoint();
  void IncDecPoint(QString btn);
  void mEnterMarker(QString btn);
  void mDisplaySelectedMarker();
  void autoWaitPrecalculate();
  void SelectVideoFilter();
  void SelectLatchedSwitches(int desiredFreqBand);
  int switchLatchBits(int desiredFreqBand);
  void SelectFilter(int &fbank);
  void CommandFilter(int &fbank);
  void CommandFilterSlimCBUSB(int &fbank);
  void OneStep();
  void Continue();
  void FocusKeyBox();
  void PartialRestart();
  void Restart();
  void ClearAuxData();
  void Showvar();
  void updatevar();
  void Closevarwin();
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
  void finished();
  void SpecialTests();
  void CloseSpecial();
  void CavityFilterTest();
  void CloseCavityFilterTest();
  void CommandLO2forCavTest();
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

//=====================Start Routines to Save/Copy Image===========================
  void SaveImage();
  QString FullGraphBmp();
  void CopyImage();
//==================End of Save/Copy Image================

  void HideButtonsOnGraph();
  void ShowButtonsOnGraph();
  void menuRLCAnalysis();



  void menuS11ToS21();

  void ImpedToRLC(); // connect$,centerStep, nLeft,nRight, resonStep, byref R, byref L, byref C
  void DetermineLCEquiv(); // connect$,startStep, endStep, resonStep
  void menuQ();
  void ChangeGraphsToAuxData();// aux1, aux2
  void QFactors(int nPoints);
  void menuSaveDataFile();
  void SaveDataFile(); //dataFileName$, doContexts
  void RestoreVNAData();
  void regraphDatatable();
  void menuLoadDataFile();
  void LoadDataFileWithContext(QString dataFileName);



//===================START CONTEXTS MODULE=======================
  QString SweepContext();
  void RestoreSweepContext();
  QString GridContext();
  void FilterDataType();
  QString TraceContext();
  QString RestoreTraceContext(QString s, int &startPos, int isValidation);
  QString RestoreGridContext(QString s, int &startPos, int isValidation);
  int BandLineCalContextAsTextArray();
  QString BandLineCalContext();
  void BandLineCalContextToFile(QStringList &fHndl);
  int BaseLineCalContextAsTextArray();
  QString BaseLineCalContext();
  void BaseLineCalContextToFile(QStringList &fHndl);
  void RestoreBandLineCalContext(QString &s, int &startpos);
  int GetBandLineCalContextFromFile(QString fHndl);
  int RestoreBaseLineCalContext(QString &s, int &startPos);
  int GetBaseLineCalContextFromFile(QFile *fHndl);
  int OSLCalContextAsTextArray(int isBand);
  void OSLCalContextToFile(QFile *fHndl, int isBand );
  void OSLGetCalContextFromFile(QFile *fHndl, int isBand);
  int GraphDataContextAsTextArray();
  QString GraphDataContext();
  void GraphDataContextToFile(QStringList &fHndl);
  void RestoreGraphDataContext(QString &s, int &startPos, int doTitle);
  void GetGraphDataContextFromFile(QFile *fHndl, int doTitle);
  void DataTableContextAsTextArray();
  QString DataTableContext();
  void DataTableContextToFile(QFile *fHndl);
  void RestoreDataTableContext(QString &s, int &startPos, int doTitle);
  void GetDataTableContextFromFile(QFile *fHndl, int doTitle);
  QString mMarkerContext();
  QString  mRestoreMarkerContext(QString s, int &startPos, int isValidation);
  void LoadBasicContextsFromFile();
  QFile *OpenContextFile(QString fName, QString inout);

  QString SaveContextFile(QString fName);
  void SavePreferenceFile(QString fName);
  void LoadPreferenceFile(QString fileName);
//===================END CONTEXTS MODULE=========================
  void DebugSaveData();
  void DebugCopyFile(QString source, QString dest);
  void DebugCopyDirectory(QString sourcePath, QString dirName, QString destPath);
  void DebugLoadData();
  void DebugSaveArrays(QString folder);
  void DebugLoadArrays(QString folder);
  void  DebugArrayAsTextArray(QString arrayID);
  void  DebugArrayToFile(QString arrayID, QString fullName);
  void DebugOpenInputFile$(QString arrayID, QString folder);
  void DebugOpenOutputFile$(QString arrayID, QString folder);
  void fileError();
  void DebugGetArrayFromFile(QString arrayID, QString folder);
//================================START DEBUG MODULE=======================================



//===============Configuration Manager Module=======================

  msaConfig activeConfig;


  //configDisplayedFilters$ is a list of final filters displayed in the Manager window.
  //zero entry of first dimension is used. Limit is 38, but more than 10 is highly unlikely.
  //QString configDisplayedFilters[41];
  int configNumDisplayedFilters; //Number of filters in configDisplayedFilters$
  //delver113.7g    global configWindHndl$      //Handle to our main window SEWcal3 moved to beginning

  //The following arrays are to initialize listboxes or comboboxes
  QString configPLLtypes[6];
  QString configPLLpol[2];
  QString configPLLmodes[2];
  QString configParsers[2];
  QString configADCs[4];
  QString configTGtops[3];
  QString configControlBoards[4]; //ver116-1b


  int uWorkArrayFromTextArray(int nLines, int nPerLine);
  int  uArrayFromFile(QFile *fHndl, int nPerLine);   //get points from file into uWorkArray();
  void uArrayFromString(QString s, int startN, int &startPos, int nPerLine);   //get points from string into uWorkArray();
  void uHighlightText(int handle);  //handle$ is handle variable for the target text box to highlight
  void uSleep(int ms);
  void uTickCount();   //Return windows tick count ver116-1b
  void uParsePath(QString fullPath, QString &folder, QString &file); //Parse full path name of file into the file name and folder name (no slash at end);
  int uConfirmSave(QString fullPath); //if file fullPath$ exists, then ask user to confirm saving file. Return 1 if save, 0 if cancel.
  QString uAddExtension(QString name, QString exten);    //Add extension to path or file name if there is no extension
  void uDeleteFile(QString fullName);   //Kill file if it exists; no error if it doesn//t
  int uFileOrFolderExists(QString Path); // checks for the existence of the given file or folder
  QString  uSaveFileDialog(QString filter, QString defaultExt, QString initialDir, QString initialFile, QString windTitle);
  QString uOpenFileDialog(QString filter, QString defaultExt, QString initialDir, QString initialFile, QString windTitle);
  bool uVerifyDLL(QString dllName);
  QString uExtractFontColor(QString font);  //Return the color from a font specification
  void FindPeakSteps(int dataType, int startStep, int endStep, int &minNum, int &maxNum, int &minY, int &maxY);    //find positive and negative peak
  float StepWithValue(int dataType, int startStep, int dir, int targVal);   //Return first step number (maybe fractional); with the specified value
  void uSeriesRLCFromPoints(float Z1r, float Z1i, float f1, float Z2r, float Z2i, float f2, float &R, float &L, float &C);   //From Z at two points, calculate series RLC components
  void uParallelRLCFromPoints(float Z1r, float Z1i, float f1, float Z2r, float Z2i, float f2, float &R, float &L, float &C);   //From Z at two points, calculate parallel RLC components









  //==========================Two-Port Module  ver116-1b============================
  /*
  //Individual scans can be assembled into two-port S-Params by adding them to the appropriate
  //entries in TwoPortArray, whose first index specifies the step number, and second index indicates
  //Frequency(0), S11(1=db, 2=angle), S21(3=db, 4=angle), S12(5=db, 6=angle) and S22(7=db, 8=angle)
  //Frequency is the true frequency in MHz, not the equivalent 1G frequency.
  //This array is resized when necessary, not in ResizeArrays
  dim TwoPortArray(100,8)

  //TwoPortABCD contains the parameters converted to ABCD format, stored in the order
  //A(1=Real, 2=Imag), B(3=Real, 4=Imag), C(5=Real, 6=Imag), D(7=Real, 8=Imag).
  //They are used to speed calculations of TwoPortMatchedSParam when impedance matching is changed.
  //TwoPortMatchParamValid=1 when these arrays have valid values, and =0 when they need recalculation.
  dim TwoPortABCD(100,8)  //Used to convert TwoPortArray to ABCD params ver116-2a
  //TwoPortMatchedSParam is derived from TwoPortABCD and the impedance match specifications,
  //and holds the S-Params of the matched DUT.
  dim TwoPortMatchedSParam(100,8) //S params in same order as TwoPortArray
  */
  int  TwoPortMatchParamValid;   //=1 when TwoPortABCD and TwoPortMatchedSParam have valid values, and =0 when they need recalculation.
  /*
  //Whenever one of the 8 parameters is drawn, its drawing commands are saved in TwoPortParamTraces$ for future draws.
  //TwoPortParamTraces$(p,1) holds the axis 1 commands; TwoPortParamTraces$(p,2) holds axis 2 commands.
  dim TwoPortParamTraces$(8,2)    //Traces for drawing; zero not used for second index
                        //zero for first index means individual graphs; not a param pair

  //A data type may be described as an S-parameter, but we may be graphing reflection parameters as
  //something else, such as impedance. The form in which we are graphing is specified in TwoPortS11S22Form$,
  //which is: "S"(S-param), "Z"(Z mag/angle), "SerResReact"(series resistance/reactance),
  // "ParResReact"(parallel resistance/reactance),"RhoTheta"(rho/theta), "RLSWR"(return loss/SWR),
  // "ParRL" (parallel RL), "ParRC"(parallel RC), "SerRL"(series RL) or "SerRC"(series RC)
  */
  QString TwoPortS11S22Form;   //indicates whether S11 and S22 should be graphed as S-param or in another form

  int TwoPortReflectTypeDB;  //type that replaces reflection DB when TwoPortS11S22Form$<>"S"
  int TwoPortReflectTypeAng; //type that replaces reflection Ang when TwoPortS11S22Form$<>"S"

  int TwoPortTermWindowOpen;    //=1 when #twoPortTermWin is open for altering impedance match values

  int TwoPortValidEntries[5];   //Items 1-4 are 1 if corresponding S parameters have been entered
                        //1=S11, 2=S21, 3=S12, 4=S22
  int TwoPortParamShowing[9];  //1 if parameter is showing from click of parameter button 1-4 are S params 5-8 are terminated gain and RL
  int TwoPortMaxSteps;   //max number of steps for current size of TwoPortArray
  /*global TwoPortZ0        //Reference impedance of two port parameters
  global TwoPortGraphBoxWidth, TwoPortGraphBoxHeight   //width and height of our graph box; adusted when resizing occurs

  //Impedance matching can be specified in several ways. The simplest (and at the moment the only) is to
  //specify fixed impedances to which the source and load are matched--i.e. they are the impedances seen by
  //the DUT. The impedance seen by the actual source and load (and the actual impedances of source and load) are TwoPortZ0.
  //The reactance is specified by a capacitor or inductor, in series or shunt. For narrow band matching, this is
  //a fairly fixed reactance.
  */
  int TwoPortLoadIsShunt, TwoPortSourceIsShunt, TwoPortSourceIsCap, TwoPortLoadIsCap; //config of reactive components, and whether they are cap or inductor
  int TwoPortSourceComponValue, TwoPortLoadComponValue; //value of source and load capacitors (in pF) or inductors (in uH)
  int TwoPortSourceZR, TwoPortLoadZR;   //In and out termination resistances //ver116-2a
  //TwoPortTermCalcFreq is the freq(MHz) for calculating auto match and converting negative caps and inductors
  //into positive inductors and caps before doing match graphs. TwoPortTermCalcStep is the step number of that freq
  float TwoPortTermCalcFreq,;
  int TwoPortTermCalcStep;
  /*
  dim TwoPortTitle$(4)       //Title of two port params. 4 lines; zero entry not used
  dim TwoPortSaveTitle$(4)    //Saved title from main graph when two-port window was opened
  //A specific parameter is sometimes selected for some action. The parmeter being graphed is always selected.
  global TwoPortSelectedParam    //1(S11), 2(S21), 3(S12), 4(S22), 5(matched S11), 6(matched S21), 7(matched S12) or 8(matched S22)
  //The following x and Y axis values are preserved when two port is closed
  global TwoPortStartFreq, TwoPortEndFreq, TwoPortNumSteps, TwoPortLinear //Info on sweep settings of current data
  */
  int TwoPortY1Bot, TwoPortY1Top, TwoPortY1Type, TwoPortY2Bot, TwoPortY2Top, TwoPortY2Type;
  /*
  //The following are used to save/restore the main graph settings
  global TwoPortSaveY1Top, TwoPortSaveY1Bot, TwoPortSaveY1Auto, TwoPortSaveY1Type
  global TwoPortSaveY2Top, TwoPortSaveY2Bot, TwoPortSaveY2Auto, TwoPortSaveY2Type
  global TwoPortSaveIsLinear
  global TwoPortSaveHorDiv, TwoPortSaveVertDiv    //ver116-4a
  */
  int TwoPortZ0Left, TwoPortZ0Top;  //top left coordinate of "Z0=..." text. Used to check for clicks.

  int TwoPortTransDBTop, TwoPortTransDBBot; //Most recent axis top and bottom for transmission params DB
  int TwoPortRefDBTop, TwoPortRefDBBot; //Most recent axis top and bottom for reflection params DB
  int TwoPortPhaseTop, TwoPortPhaseBot;     //Most recent phase top and bottom for any parameters
  int TwoPortStabilityTop, TwoPortStabilityBot;      //Most recent top and bottom for stability factors.ver116-2a
  int TwoPortZMagTop, TwoPortZMagBot;       //Most recent top and bottom for S11 or S22 as impedance  mag  //ver116-4b
  int TwoPortReactTop, TwoPortReactBot;     //Most recent top and bottom for S11 or S22 as reactance  //ver116-4e
  int TwoPortRTop, TwoPortRBot;    //Most recent top and bottom for S11 or S22 as resistance  //ver116-4e
  float TwoPortCTop, TwoPortCBot;    //Most recent top and bottom for S11 or S22 as capacitance  //ver116-4e
  float TwoPortLTop, TwoPortLBot;    //Most recent top and bottom for S11 or S22 as inductance  //ver116-4e
  int TwoPortReturnLossTop, TwoPortReturnLossBot;    //Most recent top and bottom for S11 or S22 as return loss  //ver116-4e

  QString twoPortSaveMarkers, twoPortOurMarkers;      //saved main graph marker context when opening, and two port markers saved when closing

  int TwoPortWindH, TwoPortMenuBarH, TwoPortOptionsH;    //Windows handles to #twoPortWin, its menu bar and options submenu  //ver116-4b

  QString TwoPortFileDataForm, TwoPortFileFreqForm; //Output file form for frequency (HZ, KHZ, MHZ, GHZ) and data (RA, MI, DB) ver116-4m



  void TwoPortInitVariables();
  void TwoPortResize(int maxSteps);

  int TwoPortGetY1Type();
  int TwoPortGetY2Type();
  void TwoPortDetermineGraphDataFormat(int componConst, QString &yAxisLabel, QString yLabel, int &yIsPhase, QString &yForm);

//==========================End Two-Port Module=================================

//===========================Real-Time Graphing Module=====================================
  // see the global file for the variables

  void gInitFirstUse(QString hnd, int winWidth, int winHt, int marLeft, int marRight, int marTop, int marBot);
  QString gGetLastPresetColors();
  void gChangeTextSpecs(QString btn, QString &newTxt);






  void gInitDynamicDraw();



















  QString gRestoreTraceContext(QString &s, int &startPos, int isValidation);
  QString gGridContext();
  QString gTraceContext();




  bool CoaxCreateFile();

  void smithFinished(QString );
  void multiscanCloseAll();





  void DetectChanges();
  void DetectChanges(int doRestart);








  QString smithGraphHndl();





  void frontEndInterpolateToScan();

  void CleanupAfterSweep();



  void gSetMarkerNum(int markNum, int pointNum, QString ID, QString trace, QString style);

  QString gRestoreGridContext(QString &s, int &startPos, int isValidation);









  void CloseSpecial(int returnflag);



  QString DefaultDir;

  int port;
  int status;
  int control;

  int globalSTRB, globalINIT, globalSELT, globalContClear;  //ver116-1b

  int contclear;
  int STRB;
  int AUTO;
  int INIT;
  int SELT;
  int INITSELT;
  int STRBAUTO;
  int Jcontrol;
  int swclk;
  int sfqud;

  int le1, le2,le3,fqud1,fqud3; //ver111-31b

  int pdmlowlim, pdmhighlim;
  int bUseUsb;
  int glitch, glitchp1, glitchd1, glitchp3, glitchd3, glitchpdm, glitchhlt; //ver111-27
  //int waittime;

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

  QString yAxisLabel;
  QString y1AxisLabel;
  QString y2AxisLabel;
  QString yLabel, y1Label, y2Label;
  int y1IsPhase, y2IsPhase;


  int filtbank;
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


  int pdmcmd;

  int Bcounter;
  int Bcounter2;
  int Acounter;
  int Acounter2;
  int LO2;
  int preselector;
  int rcounter1;
  int LEPLL;

  QString error;
  QString errora;

  int phasepolarity;
  int fractional;

  //QString msaVersion, msaRevision;  //Version and revision numbers of this release

  int special;
  float enterPLL2phasefreq;

  int scanResumed; //used to indicate whether we start with a new scan(0) or resume where we left off(1)SEW
  int suppressSweepTime;






protected:
  void resizeEvent(QResizeEvent *event);
  virtual void showEvent ( QShowEvent * event );

private slots:

  void on_actionMultiscan_Help_triggered();

  void on_actionVNA_Reflection_triggered();

  void on_actionHardware_Config_Manager_triggered();

  void on_actionShow_Variables_triggered();

  void on_actionReference_Lines_triggered();

  void on_actionVNA_TRansmission_triggered();

  void on_actionSpectrum_Analyzer_triggered();

  void on_actionSpectrum_Analyzer_with_TG_triggered();

  void on_actionCopy_Image_triggered();

  void on_actionLoad_Data_triggered();

  void on_actionInitial_Cal_Manager_triggered();

  void on_actionApperances_triggered();

  void on_actionSave_Image_triggered();

  void on_actionSave_Debug_Info_triggered();

  void on_btnContinue_clicked();

  void on_actionComponent_Meter_triggered();

  void on_actionCrystal_Analysis_triggered();

  void on_actionAbout_Qt_triggered();

  void on_actionAbout_triggered();

  void on_btnTestSetup_clicked();

  void on_btnRestart_clicked();

  void on_btnRedraw_clicked();

  void on_btnExpandLR_clicked();

  void on_btnmMarkToCenter_clicked();

  void on_btnMarkEnterFreq_clicked();

  void on_btnMarkClear_clicked();

  void on_btnOneStep_clicked();

  void on_actionSave_Prefs_triggered();

  void on_actionLoad_Prefs_triggered();

  void on_actionSave_Data_triggered();

public slots:
  void updateView();
  void delayedStart();


};

#endif // MAINWINDOW_H
