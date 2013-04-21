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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "math.h"


#include <qwaitcondition.h>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  winConfigMan = NULL;
  vars = NULL;
  usb = NULL;
  ui->setupUi(this);
  vars = new globalVars;



  vnaCal.setGlobalVars(vars);

  graph.setGlobalVars(vars);
  graph.setActiveConfig(&activeConfig);
  graph.setUwork(&uWork);
  graph.setAppearance(&gridappearance);
  graph.setVna(&vnaCal);

  usb = new usbFunctions;

  ui->statusBar->setVisible(false);
  ui->mainToolBar->setVisible(false);

  calMan.setMsaConfig(&activeConfig);

  DefaultDir = QApplication::applicationDirPath();
  coax.setFilePath(DefaultDir);

  touch.setUwork(&uWork);

  filtbank = 0;
  special = 0;
  scanResumed = 0;
  suppressSweepTime=0;

  ui->graphicsView->setScene(&scene);
  scene.clear();
  scene.addSimpleText("Please wait, starting up.");
  scene.setSceneRect(scene.itemsBoundingRect());
  ui->graphicsView->fitInView(scene.sceneRect());

  //ui->graphicsView->setScene(graph.getScene());

  if (winConfigMan)
  {
    delete winConfigMan;
  }
  winConfigMan = new dialogConfigMan(this);
  timerStart2 = new QTimer(this);
  connect(timerStart2, SIGNAL(timeout()), this, SLOT(delayedStart()));
  timerStart2->start(50);
}

MainWindow::~MainWindow()
{
  delete timerStart;
  delete timerStart2;
  delete usb;
  delete ui;
  if (winConfigMan)
  {
    delete winConfigMan;
  }
  delete vars;
}

void MainWindow::delayedStart()
{
  timerStart2->stop();

  //Suppress parallel port if we don't have the DLLs
  if (uVerifyDLL("ntport"))
    vars->suppressHardware=0;
  else
    vars->suppressHardware=1; //may change when we have cb info

  bool bUsbAvailable;
  if (uVerifyDLL("msadll"))
    bUsbAvailable = true;
  else
    bUsbAvailable = false; //USB:01-08-2010

  if (bUsbAvailable)
  {
    if (usb->usbInterfaceOpen(QApplication::applicationDirPath() + "/msadll.dll"))
    {
      int rev = usb->usbMSAGetVersions();
      if (rev / 256 < 2)
      {
        usb->usbCloseInterface();
        QMessageBox::critical(this, "MSADLL", tr("The version number of msadll is too old for me to use"));
      }
      if ((rev & 255) < 36)
      {
        usb->usbCloseInterface();
        QMessageBox::critical(this, "MSADLL", tr("The USB interface is either not plugged in or is too old a version for me"));
      }
    }

  }

  graph.interpolateMarkerClicks = 0;
  vars->steps = 400;
  vars->globalSteps = 400;
  graph.gSetNumDynamicSteps(vars->steps);

  graph.markerIDs[0] = "1"; graph.markerIDs[1] = "2"; graph.markerIDs[2] = "3"; graph.markerIDs[3] = "4";
  graph.markerIDs[4] = "5"; graph.markerIDs[5] = "6"; graph.markerIDs[6] = "L"; graph.markerIDs[7] = "R";
  graph.markerIDs[8] = "P+"; graph.markerIDs[9] = "P-";
  uWork.uWorkMaxPoints=800;  //Record size of uWorkArray(,); initially 800 points
  uWork.uWorkMaxPerPoint=9;  //Initially max 9 items per point

  winConfigMan->configInitFirstUse();

  if (!winConfigMan->configFileExists())
  {
    winConfigMan->configRunManager(1);
    winConfigMan->configReadFile(&activeConfig);
  }
  else
  {
    QString errStr = winConfigMan->configReadFile(&activeConfig);
    if (!errStr.isEmpty())
    {
      QMessageBox::critical(0,"Config file error","Configuration File Error; " + errStr + "; Default values used");
      winConfigMan->configInitializeDefaults(&activeConfig);
    }
    else
    {
      //Be sure we have the Wide filter. This line is needed for smooth transition from
      //ver115 to ver116. ver1116-4f
      if (activeConfig.videoFilterNames[1] != "Wide")
      {
        activeConfig.videoFilterNames[1] = "Wide";
        activeConfig.videoFilterCaps[1][0]=0.002;
        activeConfig.videoFilterCaps[1][1]=0.011;
      }
      winConfigMan->configSaveFile(activeConfig); //Save config file in current format ver114-5i
    }
  }
  port = activeConfig.globalPort;  //SEW5 add ver113-7c
  vars->glitchtime = 0;     //SEW5 add ver113-7c; ver114-5k deleted globalGlitchtime
  status = port + 1;        //SEW5 add ver113-7c
  control = port + 2;       //SEW5 add ver113-7c

  vars->bandEnd1G=1000; vars->bandEnd2G=2000;    //good for now ver116-4s

  if (activeConfig.cb == 3)
  {
    //            3 means USB. suppressHardware relates only to parallel port ver116-4b
    //            An initial low on the PS bit that controls latched switches may be draining switch capacitors
    //            Set it high and allow some recharge time. At this point we don't care what the switches get set to,
    //            and if the capacitors are discharging the switches won//t change state at all.
    vars->suppressHardware = 0;
  }
  vars->freqBand = 1;
  SelectLatchedSwitches(1);   //ver116-4h ver116-4s
  uSleep(1000);               //Wait 1 second for capacitor recharge. There will be additional software delays before PS is used.
  calMan.calInitFirstUse(201, 1001, activeConfig.hasVNA);    //Initialize Mag/Freq Calibration Module--201 max mag cal points; 1001 max freq cal points ver114-4b
  //ResizeArrays needs TGtop, so we do it after loading config file

  ResizeArrays(2001);   //Make all arrays big enough for 2001 points; also loads BaseLineCal file   'ver114-5m

  //---------Load path and freq calibration info------
  calMan.calInstallFile(0);   //Loads frequency calibration file; creates one if necessary
  for (int i = activeConfig.MSANumFilters; i == 1; i--)
  {
    //For each filter, create the file if necessary and load it
    //Each one loaded replaces the data from the previous one. We are just
    //trying to be sure they exist and are OK.
    //We do this in reverse order to path 1 will be the last one and stays in place
    //This also sets finalfreq and finalbw
    calMan.calInstallFile(i);
  }
  vars->path="Path 1";
  //Note physical selection of filter 1 is done in step 5 below
  for (int i=1; i <= activeConfig.MSANumFilters; i++)
  {
    //For each filter, combine freq and bw into nicely aligned string. Used to load #main.FiltList
    activeConfig.MSAFiltStrings[i-1]="P" + QString("%1").arg(i) + "   "
        + winConfigMan->configFormatFilter(activeConfig.MSAFilters[i][0]
        , activeConfig.MSAFilters[i][1]); //ver113-7c
  }

  //The below are not actually the desired states. See step 3 (initialization) for explanation.
  //The latched filter addresses will be asserted by SelectVideoFilter because the video filter
  //shares the same latch. But the PS bit will not be toggled, so this will not actually affect
  //latched switches that rely on PS, and won//t drain capacitors. But this will help initialize
  //latched switches that generate the latching pulse from a change of address.
  //This is done after loading config file so capacitor info is available, and after
  //loading cal files so auto wait info is available.
  vars->videoFilter="Wide";
  vars->freqBand=2;
  vars->switchTR=1;
  vars->switchFR=1;
  SelectVideoFilter(); //ver116-4s

  //---------Create OperatingCal Folder-------------
  if (activeConfig.TGtop>0)
  {
    int isErr=CreateOperatingCalFolder();  //Create OperatingCal folder if it does not exist
    if (isErr)
      QMessageBox::about(0,"Error", "Unable to create OperatingCal folder.");
  }

  //-----Load or create coax data file-----
  coax.CoaxLoadDataFile(); //ver115-4a

  //2.Establish hard "Global" variables
  //For speed, most of the following are not declared global, and they are not accessible
  //within true subroutines. But a couple have true global versions.
  contclear = 11;         //to take all LPT control lines low
  STRB = 10;              //to take LPT-pin 1 high. (SLIM Latch 4)
  AUTO = 9;               //to take LPT-pin 14 high.  (SLIM Latch 3)
  INIT = 15;              //to take LPT-pin 16 high. (SLIM Latch 2)
  SELT = 3;               //to take LPT-pin 17 high. (SLIM Latch 1)
  INITSELT =  7;          //to take both LPT-pins 16 & 17 high. (INIT,SELT)(was enapt) ver111-22
  STRBAUTO = 8;           //to take both LPT-pins 1 & 14 high. (FQUD,WCLK)(was wclkfqud) ver111-22
  /*
    global globalSTRB, globalINIT, globalSELT, globalContClear  //ver116-1b
    */
  globalSTRB=STRB ; globalINIT=INIT ; globalSELT=SELT ; globalContClear=contclear;   //ver116-1b

  if (activeConfig.cb==0)
  {
    le1=4;le2=8;le3=16;fqud1=STRB;fqud3=2; //ver111-31b
  }
  if (activeConfig.cb==1)
  {
    le1=1;le2=1;le3=4;fqud1=2;fqud3=8; //ver111-31b
  }
  if (activeConfig.cb==2)
  {
    le1=1;le2=16;le3=4;fqud1=2;fqud3=8; //ver111-31b
  }
  if (activeConfig.cb==3)
  {
    le1=1;le2=16;le3=4;fqud1=2;fqud3=8;vars->bUseUsb = 1;  //USB;01-08-2010
  }
  if (activeConfig.adconv == 8)
  {
    pdmlowlim = 51 ; pdmhighlim = 205; //establish boundries for 8 bit parallel A to D ver111-36f
  }
  if (activeConfig.adconv == 12)
  {
    pdmlowlim = 819 ; pdmhighlim = 3277; //establish boundries for 12 bit parallel A to D ver111-36f
  }
  if (activeConfig.adconv == 16)
  {
    pdmlowlim = 13107 ; pdmhighlim = 52429; //establish boundries for 16 bit serial A to D ver111-36f
  }
  if (activeConfig.adconv == 22)
  {
    pdmlowlim = 819 ; pdmhighlim = 3277; //establish boundries for 12 bit serial A to D ver111-37a
  }


  //3.Initialize for whatever mode we will start up in
  //Some of these initializations may be changed when the preferences file
  //is loaded in [LoadPreferenceFile]
  vars->doingInitialization=1; //ver114-3f
  vars->suppressPhase=0;     //Turns phase on ver116-1b
  vars->suppressHardwareInitOnRestart=0;     //Normally we do hardware initialization on each restart.
  graph.refreshOnHalt=1;     //We normally redraw the graph when we halt.
  vars->multiscanIsOpen=0;
  vars->multiscanInProgress=0;
  vars->baseFrequency=0;
  vars->cftest=0;    //cavity filter sweep test off ver116-4b
  vars->message="";  //ver115-1a
  vars->msaMode="SA";  //ver115-3b
  vars->planeadj=0;  //ver114-4i
  vars->gentrk=0 ; vars->normrev=0 ; vars->sweepDir=1;   //ver 114-4k
  graph.refreshEachScan=1;    //ver114-3f
  vars->videoFilter="Wide";  //ver116-1b
  vars->useAutoWait=0;    //ver116-1b
  vars->autoWaitPrecision="Normal";  //ver116-1b
  TwoPortInitVariables(); //initialize variables including default Y axis ranges for Two-Port ver116-1b

  SelectVideoFilter(); //Sets videoFilterAddress and outputs it ver116-1b
  //Note the latched switches--Band, FR and TR--may actually not be "latching" (because they may just
  //rely on the control board latch, and they may or may not use the Pulse Start (Latch Pulse) to trigger latching.
  //If they generate their own pulse for latching based on a change of required state, we have to be sure
  //that on startup they do so. For example, just setting the frequency band switch to 01 for 1G may not create
  //a latch pulse, if the control board latch already had that value. We need to initialize those switches to
  //two successive values. We have to wait in between to allow for some recharging of the capacitors.
  //The first state was set at the very beginning of the program, to utilize the time delays of initializtion.
  //Video Filter is not a latched switch, but it has to be output along with the other data
  //This should work if the switch capacitors discharge only about 10% and recharge time constant is 1 second or less
  //Note that SelectLatchedSwitches adds some time delay also
  vars->switchTR=0 ; vars->switchFR=0 ;
  SelectLatchedSwitches(vars->lastSetBand); //ver116-1b ver116-4s
  uSleep(500); //wait again because latching will occur again when preferences are loaded.

  vars->returnBeforeFirstStep=0; //ver115-1d
  vars->haltedAfterPartialRestart=0; //ver116-1b
  vars->specialOneSweep=0; //ver115-1d
  vars->crystalLastUsedID=0;
  vars->imageSaveLastFolder =DefaultDir;    //Folder in which image was last saved ver115-2a
  touch.touchLastFolder = DefaultDir;     //Folder from which param data was last loaded ver115-5f
  vars->doSpecialRLCSpec ="RLC[P, R1000,C1n,L1u]";   //default for doSpecialGraph of simulated RLC
  vars->RefRLCLastNumPoints=0;
  vars->RefRLCLastConnect ="";   //For continuity calling ReflectionRLC
  vars->analyzeQLastNumPoints=0;   //For continuity in AnalyzeQ
  vars->GDLastNumPoints=0;       //Number of points last used for group delay analysis
  vars->frontEndCalNumPoints=0;  //No front end adjustment
  vars->frontEndActiveFilePath="";
  vars->frontEndLastFolder=DefaultDir;

  //ver114-3f moved the call to gInitFirstUse here from [CreateGraphWindow]

  FindClientOffsets();   //set clientWidthOffset and clientHeightOffset from test window ver115-1b
  graph.smithLastWindowHeight=430 ; graph.smithLastWindowWidth=400;    //ver115-5d
  graph.currGraphBoxHeight=600-graph.clientHeightOffset-44;   //ver115-1b //ver115-1c 44 allows for button area below box?
  graph.currGraphBoxWidth=800-graph.clientWidthOffset;  //ver115-1b //ver115-1c
  graph.graphMarLeft=70 ; graph.graphMarRight=180 ; graph.graphMarTop=55 ; graph.graphMarBot=140;   //Graph margins from edge of graphicbox

  gInitFirstUse("#handle.g", graph.currGraphBoxWidth, graph.currGraphBoxHeight, graph.graphMarLeft, graph.graphMarRight, graph.graphMarTop, graph.graphMarBot);  //Initialize graphing module

  InitGraphParams();   //Initialize parameters to set up the graphing module ver114-3f moved
  ChangeMode(); //create Graph Window in mode of msaMode$
  vnaCal.desiredCalLevel=0;   //Desire no cal ver114-6b
  vnaCal.SignalNoCalInstalled();
  vnaCal.bandLineNumSteps=-1;   //Indicate cal does not exist ver114-5f; baseLine cal was handled above ver114-5mb
  vars->OSLBandNumSteps=-1;  //ver115-1b
  vars->OSLBaseNumSteps=-1;  //ver115-1b

  vars->OSLLastSelectedCalSet=0;  //Indicates there was no prior selection ver115-7a
  vars->OSLOpenSpec="" ; vars->OSLShortSpec="" ; vars->OSLLoadSpec="";    //ver116-4i
  //vars->OSLS11JigType="Reflect";   //ver115-1b
  //Defaults are now in place. Read the preferences file and save it. If there is no preference file
  //this has the effect of creating one with the default values. If there is one, saving it updates a
  //possibly old preferences file to the current format.

  LoadPreferenceFile(DefaultDir + "/MSA_Info/MSA_Prefs/Prefs.txt");

  //call uSleep 500     //Loading Preferences may re-latch switches; allow some recharge time ver116-1b delver116-4d
  if (graph.gGetXIsLinear()) { vars->userFreqPref=0; } else { vars->userFreqPref=1;}      //Start with Center/Span for linear, Start/Stop for log //ver115-1d
  mClearMarkers();   //Clear all graph markers

  //4.measure computer speed and update global, glitchtime
  //Determine speed of computer //ver111-37c
  if (vars->glitchtime == 0)
  {
    vars->glitchtime = 1;
    // fix me need to test the glitch time once there is hardware
    AutoGlitchtime();//ver111-37c
  }
  //return with glitchtime, number approximates 1 millisecond of computer processing speed with Liberty Basic
  //this is a "coarse" calculation.

  //5.Command Filter Bank
  InitializeHardware();

  timerStart = new QTimer(this);
  connect(timerStart, SIGNAL(timeout()), this, SLOT(updateView()));
  timerStart->start(50);
}

void MainWindow::InitializeHardware()
{

  //These hardware initializations are performed on startup and usually repeated on Restart. The reason
  //they are repeated on Restart is to fix any hardware glitches that might occur. Whenever it is known
  //that a hardware change is made, such as filter selection changing, it is best to take action immediately,
  //and not rely on the Restart process. In some cases, Restart skips these initializations for speed.
  if (vars->suppressHardware==0 && activeConfig.cb<3)   //ver115-6c USB:02-08-2010 added cb test
  {
    qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
    /*
      out port, 0                 //begin with all data lines low
      if (cb == 2)   //ver116-1b
      {
          out control, INITSELT //latch "0" into SLIM Control Board Buffers 1 and 2
          out control, AUTO //latch "0" into SLIM Control Board Buffers 3
          //We don't clear SLIM Buffer 4, because it controls among other things the latched switches
          //It was initialized near the beginning to make the PS line high.
      }
      out control, contclear      //begin with all control lines low
          */
  }
  if (activeConfig.cb == 3 && vars->bUseUsb !=0)  //USB:01-08-2010
  {
    QString USBwrbuf = "A5010000"; // reset all lines low //USB:01-08-2010
    usb->usbMSADeviceWriteString(USBwrbuf,4);    //USB:01-08-2010
  } //USB:01-08-2010
  //the following are meaningless values to guarantee first time commanding. Used in subroutine, [DetermineModule]
  vars->lastdds1output = activeConfig.appxdds1;vars->lastdds3output = activeConfig.appxdds3; vars->lastpdmstate = 2; //ver111-28
  lastncounter1 = 0; lastncounter3 = 0; //to guarantee Original MSA will command PLL//s after init. ver114-6c
  error=""; errora="";  //ver115-1c

  //Initialize Final Filter path.
  int filtbank;
  CommandFilter(filtbank);   //Commands and sets filtbank. Does nothing if suppressHardware=1. ver115-6c

  SelectVideoFilter();  //reselect video filter in case a glitch got it //ver116-1b
  //Note we don't reset the latched switches on Restart (for startup, they are set when prefs are loaded),
  //because it can get obnoxius and requires a time delay.
  //Plus, we don't want to set them when the user makes a change, and immediately set again on Restart.
  //These switches are properly set whenever DetectChanges is called, which should take care of them.
  //ver116-4d deleted call to SelectLatchedSwitches

  //6.if configured, initialize DDS3 by reseting to serial mode. Frequency is commanded to zero
  if (vars->suppressHardware)
  {
    SkipHardwareInitialization();  //In case there is no hardware ver115-6c
    return;
  }

  if (activeConfig.TGtop != 0) //goto endInitializeTrkGen;// there is no Tracking Generator ver111-22
  {
    //Initialize DDS 3
    if (activeConfig.cb == 0 && activeConfig.TGtop == 2) {Jcontrol = INIT;swclk = 32;sfqud = 2; lpt.ResetDDS3ser();} //ver111-7
    //[ResetDDS3ser]needs:port,control,Jcontrol,swclk,sfqud,contclear ; resets DDS3 into Serial mode
    if (activeConfig.cb == 2) lpt.ResetDDS3serSLIM(); //ver111-29
    if (activeConfig.cb == 3) ResetDDS3serUSB();  //USB:01-08-2010
    //7.if configured, initialize PLO3. No frequency command yet.
    //Initialize PLL 3. //CreatePLL3R,CommandPLL3R
    appxpdf=activeConfig.PLL3phasefreq; //ver111-4
    if (activeConfig.TGtop == 1) vars->reference=activeConfig.masterclock; //ver111-4
    if (activeConfig.TGtop == 2) vars->reference=activeConfig.appxdds3; //ver111-4
    CreateRcounter();//needs:reference,appxpdf ; creates:rcounter //ver111-14
    rcounter3=rcounter; pdf3=pdf; //ver111-7
    //CommandPLL3R and Init Buffers
    datavalue = 8;levalue = 4; //PLL3 data and le bit values ver111-28
    CommandPLL3R();//needs:PLL3mode,PLL3phasepolarity,INIT,PLL3 ; Initializes and commands PLL3 R Buffer(s) //ver111-7
  }


  //[endInitializeTrkGen]   //skips to here if no TG

  //8.initialize and command PLO2 to proper frequency
  //CreatePLL2R
  appxpdf=activeConfig.PLL2phasefreq; //ver111-4
  vars->reference=activeConfig.masterclock; //ver111-4
  CreateRcounter();//needed:reference,appxpdf ; creates:rcounter,pdf //ver111-14
  rcounter2 = rcounter; //ver111-7
  pdf2 = pdf;    //actual phase detector frequency of PLL 2 //ver111-7
  //CommandPLL2R and Init Buffers
  datavalue = 16; levalue = 16; //PLL2 data and le bit values ver111-28
  CommandPLL2R();//needs:PLL2phasepolarity,SELT,PLL2 ; Initializes and commands PLL2 R Buffer(s)
  //CreatePLL2N
  appxVCO = appxLO2; vars->reference = activeConfig.masterclock;
  CreateIntegerNcounter();//needs:appxVCO,reference,rcounter ; creates:ncounter,fcounter(0)
  ncounter2 = ncounter; fcounter2 = fcounter;
  CreatePLL2N();//needs:ncounter,fcounter,PLL2 ; returns with Bcounter,Acounter, and N Bits N0-N23
  Bcounter2=Bcounter; Acounter2=Acounter;
  LO2=((Bcounter*preselector)+Acounter+(fcounter/16))*pdf2; //actual LO2 frequency  //ver115-1c LO2 is now global
  //CommandPLL2N
  Jcontrol = SELT; LEPLL = 8;
  datavalue = 16; levalue = 16; //PLL2 data and le bit values ver111-28
  CommandPLL();//needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111-5

  //9.Initialize PLO 1. No frequency command yet.
  //[InitializePLL1]//set PLL1 to proper Rcount and initialize
  //        appxpdf=PLL1phasefreq //ver111-4
  //        reference=appxdds1 //ver111-4
  //        gosub [CreateRcounter]//needed:reference,appxpdf ; creates:rcounter,pdf //ver111-4
  //        rcounter1 = rcounter //ver111-4
  //Create rcounter1 ver114-2e
  rcounter1=(int)(activeConfig.appxdds1/activeConfig.PLL1phasefreq);   //ver114-2e
  if ((activeConfig.appxdds1/activeConfig.PLL1phasefreq) - rcounter1 >= 0.5) rcounter1 = rcounter1 + 1;   //rounds off rcounter  ver114-2e
  if (vars->spurcheck==1 && activeConfig.PLL1mode==0) rcounter1 = rcounter1 +1; //only do this for IntegerN PLL  ver114-2e

  //CommandPLL1R and Init Buffers
  datavalue = 2; levalue = 1; //PLL1 data and le bit values ver111-28
  CommandPLL1R();//needs:rcounter1,PLL1mode,PLL1phasepolarity,SELT,PLL1 ; Initializes and commands PLL1 R Buffer(s)

  //10.initialize DDS1 by resetting. Frequency is commanded to zero
  //It should power up in parallel mode, but could power up in a bogus condition.
  if (activeConfig.cb == 0 && activeConfig.dds1parser == 0) lpt.ResetDDS1par();//(Orig Control)//needs:control,STRBAUTO,contclear ; resets DDS1 on J5, parallel ver111-21
  if (activeConfig.cb == 0 && activeConfig.dds1parser == 1) lpt.ResetDDS1ser();//(Orig Control)//needed:control,AUTO,STRB,contclear  ; resets DDS1 on J5, into serial mode ver111-21
  if (activeConfig.cb == 2) lpt.ResetDDS1serSLIM();//reset serial DDS1 without disturbing Filter Bank or PDM //ver111-29
  if (activeConfig.cb == 3) ResetDDS1serUSB();//reset serial DDS1 without disturbing Filter Bank or PDM  //USB:01-08-2010
  SkipHardwareInitialization();
}
void MainWindow::SkipHardwareInitialization()    //Skips to here if there is no hardware (suppressHardware=1) ver115-6c
{
  activeConfig.cb=activeConfig.cb;   //to avoid two labels in a row

  //11.[BeginScanSeries] get info from windows and update variables
  //[BeginScanSeries]   //Start a new series of scans, which requires some initialization

  //12.[InitializeGraphModule]
  vars->suppressPDMInversion=0;  //ver115-1a
  //ver115-8d moved test for specialOneSweep to step 14
  UpdateGraphParams();  //Update graph module for any changes made by the user
  graph.firstScan=1;     //Signal that the next scan is the first after Restart
  //ver114-5f moved some items to UpdateGraphParams

  //Note x values must be calculated first (in [UpdateGraphParams]) ; modVer115-1c
  //If calInProgress=1, InstallSelectedxxx will just set applyCal=0 and installed base steps=-1    //ver116-4b
  if (vars->msaMode=="Reflection")
  {
    oslCal.InstallSelectedOSLCal();
  }
  else
  {
    if (vars->msaMode!="SA") InstallSelectedLineCal();  //ver115-8c
  }
  vars->cycleNumber=1;
  gridappearance.gSetTraceColors(gridappearance.cycleColorsAxis1[0],gridappearance.cycleColorsAxis2[0]);  //ver116-4s
  QString xText, dum1, dum2, gridText;
  gridappearance.gGetTextColors(xText, dum1, dum2, gridText);  //ver116-4s
  gridappearance.gSetTextColors(xText, gridappearance.cycleColorsAxis1[0],gridappearance.cycleColorsAxis2[0], gridText);    //match text to trace ver116-4s
  vars->doCycleTraceColors=0;    //start with cycling off. No preference file item for this. ver116-4s

  gInitDynamicDraw();   //Set up for first scan of dynamic draw/erase/redraw...
  ImplementDisplayModes();  //Done in [UpdateGraphParams] but gInitDynamicDraw overrode it   //ver115-4e

  //In multiscan, we don't want to update the time stamp on every redraw, which sometimes happens without scanning.
  if (vars->multiscanIsOpen==0 || vars->multiscanInProgress==1)  //ver115-9a
  {
    vars->restartTimeStamp=QDateTime::currentDateTime().toString("MM/dd/yy; hh:mm:ss"); //date$("mm/dd/yy"); "; ";time$()   //ver115-2c
    graph.gSetTitleLine(2, vars->restartTimeStamp);    //Put date and time in line 3 of title
    if (graph.gGetXIsLinear())
      graph.gSetTitleLine(3, "MSA Linear Sweep " + vars->path);
    else
      graph.gSetTitleLine(3, "MSA Log Sweep "+vars->path);      //Save linear/log and path info ver116-1b
  }

  //For multiscan, the redraw of the background is done prior to scanning via [PartialRestart], and on refresh
  if (vars->multiscanInProgress==0)
  {
    //Redraw background stuff on first scan of a series. ver115-8d
    //ver115-8d deleted calc of centerstep, which is no longer used
    graph.gDrawGrid();      // Clear graphics area and draw the background grid and labels. Wipes out all prior flushes.
    graph.DrawSetupInfo();    // Draw info describing the sweep setup

    /*
      //fix me no smith chart yet
      if (smithGraphHndl()!="")   //ver115-1b draw smith chart if we have one ver115-1e
      {
        //smithRedrawChart(); //Draw blank chart ver115-2c
      }
*/
    if (graph.referenceLineType!=0)    //Draw reference lines ver114-8a
    {
      if (graph.referenceLineType>1)
        graph.CreateReferenceSource();  //RLC or fixed value
      graph.CreateReferenceTransform();   //Generate actual reference graph data
      graph.gClearAllReferences();
      QString refHeadingColor1; QString refHeadingColor2; //ver115-5d
      if (graph.referenceDoMath==0)    //don't draw ref if we are using ref for math
      {
        if (graph.referenceTrace & 2)
        {
          graph.gAddReference(1,graph.CreateReferenceTraces(graph.referenceColor2,graph.referenceWidth2,2,&graph.refLine[2]));  //Do Y2 reference
        }
        if (graph.referenceTrace & 1)
        {
          graph.gAddReference(2,graph.CreateReferenceTraces(graph.referenceColor1,graph.referenceWidth1,1,&graph.refLine[1])); //Do Y1 reference
        }
        graph.gDrawReferences();
        refHeadingColor1=graph.referenceColor1; refHeadingColor2=graph.referenceColor2; //ver115-5d
      }
      else
      {
        gridappearance.gGetTraceColors(refHeadingColor1, refHeadingColor2); //Use trace colors for "REF" if math is used
      }
      graph.PrintReferenceHeading();  //Print above axis to indicate which line matches which axis //ver115-5d
    }
    //#graphBox$, "flush"  //Make the setup info stick
  }

  //fix me
  //useExpeditedDraw=gCanUseExpeditedDraw();   ; For normal SA use, [gDrawSingleTrace] will be used.
  //ver115-1a deleted printing of glitchtime
  vars->doingInitialization=0;   //We are done with initialization on startup //ver114-4g moved

  if (vars->calInProgress==1)  //ver114-5g
  {
    vars->message="Calibration in progress."; graph.PrintMessage(); //ver114-4g
  }
  else
  {
    vars->message="";
    graph.PrintMessage();    //ver114-4f
  }
  if (vars->msaMode=="SA" && vars->gentrk==0 && vars->multiscanInProgress==0)     //ver115-4f
  {
    if ((vars->endfreq-vars->startfreq)/vars->steps >activeConfig.finalbw/1000)      //compare as MHz
    {
      vars->message= "Frequency step size exceeds RBW; signals may be missed.";
      graph.PrintMessage();
    }
  }

  //13.Calculate the command information for first step through last step of the sweep and put in arrays
  setCursor(Qt::WaitCursor);

  //ver116-4s changed this so datatable  and phaarray are set up here whether or not suppresshardware=1.
  for (int i=0; i < vars->steps;i++)
  {
    int thisfreq=graph.gGetPointXVal(i+1);    //Point number is 1 greater than step number SEWgraph
    if (vars->msaMode!="SA")   //Store actual signal freq in VNA arrays ver116-1b
    {
      if (vars->msaMode!="Reflection")
        vars->ReflectArray[vars->thisstep][0]=thisfreq;
      else
        vars->S21DataArray[vars->thisstep][0]=thisfreq;
    }
    int thisBand;
    if (vars->freqBand==0)    //ver116-4s
    {
      thisBand=1;
      if (thisfreq > vars->bandEnd2G)
        thisBand=3;
      else if (thisfreq > vars->bandEnd1G)
        thisBand=2;    //set band if auto-band
    }
    else
    {
      thisBand=vars->freqBand;
    }
    if (thisBand!=1) thisfreq=Equiv1GFreq(thisfreq, thisBand);  //Convert from actual freq to equivalent 1G frequency ver116-4s
    vars->datatable[i][0] = vars->thisstep;    //put current step number into the array, row value= thisstep //moved ver111-18
    vars->datatable[i][1] = thisfreq;
    vars->datatable[i][4] = thisBand; //ver116-4s
    vars->phaarray[i][0] = 0;   //pdm state
  }
  if (vars->suppressHardware==0)    //Do these only if we are using the hardware //ver115-6c ver116-4s
  {
    CalculateAllStepsForLO1Synth(); //ver111-18
    if (activeConfig.TGtop > 0) CalculateAllStepsForLO3Synth(); //ver111-18
    CreateCmdAllArray(); //ver111-31b
  }
  CalcFreqCorrection();     //Calculate power correction at each frequency SEWgraph1
  setCursor(Qt::ArrowCursor);
  if (vars->msaMode=="SA" && vars->frontEndActiveFilePath!="") frontEndInterpolateToScan();  //Calculate corrections for front end ver115-9d
  graph.continueCode=0;     //Set to other values by subroutines to cause halt, wait or restart

  vars->haltedAfterPartialRestart=0; //May get set to 1 a few lines below. 116-1b
  //ver114-6e Normally, refresh will occur at end of scan only if halted or refreshEachScan=1,
  //and will be done by expedited methods. But if the user makes certain changes, the following
  //variables are used to force more extensive redrawing.
  mDeleteMarker("Halt");    //ver114-4h moved the -4d version
  suppressSweepTime=1;     //to suppress it for the first scan ver114-4h
  //if we just want to go through the initialization procedure we set returnBeforeFirstStep
  //and invoke [Restart] with a gosub; here we return to the caller

  //Save some sweep settings for reflection and transmission for use when changing
  //back to a previously used mode, so we know the nature of the last gathered data
  if (vars->msaMode=="Reflection")    //ver116-1b
  {
    vars->refLastSteps=vars->steps ; vars->refLastStartFreq=vars->startfreq ;
    vars->refLastEndFreq=vars->endfreq ; vars->refLastIsLinear=graph.gGetXIsLinear();
    vars->refLastGraphR0=vnaCal.S11GraphR0;
    vars->refLastY1Type=vars->Y1DataType ; vars->refLastY1Top=vars->Y1Top ; vars->refLastY1Bot=vars->Y1Bot ; vars->refLastY1AutoScale=graph.autoScaleY1;
    vars->refLastY2Type=vars->Y2DataType ; vars->refLastY2Top=vars->Y2Top ; vars->refLastY2Bot=vars->Y2Bot ; vars->refLastY2AutoScale=graph.autoScaleY2;
    for (int i=0; i < 4; i++) { vars->refLastTitle[i]=graph.gGetTitleLine(i);}
  }
  else
  {
    if (vars->msaMode=="VectorTrans")
    {
      vars->transLastSteps=vars->steps ; vars->transLastStartFreq=vars->startfreq ;
      vars->transLastEndFreq=vars->endfreq ; vars->transLastIsLinear=graph.gGetXIsLinear();
      vars->transLastGraphR0=vnaCal.S21JigR0;
      vars->transLastY1Type=vars->Y1DataType ; vars->transLastY1Top=vars->Y1Top ; vars->transLastY1Bot=vars->Y1Bot ; vars->transLastY1AutoScale=graph.autoScaleY1;
      vars->transLastY2Type=vars->Y2DataType ; vars->transLastY2Top=vars->Y2Top ; vars->transLastY2Bot=vars->Y2Bot ; vars->transLastY2AutoScale=graph.autoScaleY2;
      for (int i=0; 1 < 4; i++)
      {
        vars->transLastTitle[i]=graph.gGetTitleLine(i);
      }
    }
  }

  if (vars->returnBeforeFirstStep)   //ver115-2a
  {
    vars->thisstep=vars->sweepStartStep;
    vars->returnBeforeFirstStep=0;
    vars->haltedAfterPartialRestart=1; //ver116-1b
    CleanupAfterSweep();
    return;    //ver115-1d
  }
  StartSweep();
}

void MainWindow::StartSweep()
{
  //14.[StartSweep]//Begin sweeping from step 0
  //StartSweep begins the outer loop that repeats the entire scan process until halted.
  //The scan loop continues until a user action which aborts the scan, or in the case of
  //OneStep it continues only for a single point. If specialOneSweep=1 or HaltAtEnd=1, it
  //automatically stops at the end of a single sweep.
  //[StartSweep]//enters from above, or [IncrementOneStep]or[FocusKeyBox]([OneStep][Continue])
  if (vars->specialOneSweep) graph.haltAtEnd=1; else graph.haltAtEnd=0;   //ver115-8d moved this here
  if (vars->haltedAfterPartialRestart==0 && scanResumed==1)   //ver116-1b
  {
    //For a resumed scan, a halt occurred after the previous step and that step was fully processed.
    //haltsweep will equal 0. If alternateSweep=1 and the halt occurred at the end of a sweep, we need to
    //repeat the last point as the first point of the new sweep. But in the case where we are continuing
    //after a halt resulting from partial restart, we returned before the first step was taken and need to
    //start with that step.
    mDeleteMarker("Halt");    //ver114-4h moved the -4d version
    if (vars->thisstep == vars->sweepStartStep && syncsweep == 1) SyncSweep(); //ver112-2b; ver114-4k
    if (vars->alternateSweep==0 || haltWasAtEnd==0)   //ver114-5c Go to next step unless we need to repeat this one
    {
      if (vars->sweepDir==1)
      {
        if (vars->thisstep<vars->sweepEndStep) vars->thisstep = vars->thisstep + 1; else vars->thisstep=vars->sweepStartStep;
      }
      else
      {
        if (vars->thisstep > vars->sweepEndStep) vars->thisstep = vars->thisstep - 1; else vars->thisstep=vars->sweepStartStep;
      }
    }
  }
  else    //ver114-5c No longer need to retest scanResumed
  {
    vars->thisstep=vars->sweepStartStep;  //ver114-4k
  }
  vars->haltedAfterPartialRestart=0; //Reset. Will stay zero until next partial restart. 116-1b
  scanResumed=0;   //Reset flag
  CommandThisStep();
}

void MainWindow::CommandThisStep()
{
  //15.[CommandThisStep]. command relevant Control Board and modules
  //SEW CommandThisStep begins the inner loop that moves from step to step to complete a single
  //SEW scan.This branch label is accessed only from the end of the loop.
  //[CommandThisStep]//needs:thisstep ; commands PLL1,DDS1,PLL3,DDS3,PDM //ver111-7
  //a. first, check to see if any or all the 5 module commands are necessary [DetermineModule]
  //b. calculate how much delay is needed for each module[DetermineModule], but use only the largest one[WaitStatement].
  //c. send individual data, clocks, and latch commands that are necessary for[CommandOrigCB]
  //or for SLIM, use [CommandAllSlims] for commanding concurrently //ver111-31c
  CommandCurrentStep();  //ver116-4j made this a separate routine

  //16.Determine sequence of operations after commanding the modules
  if (onestep == 1)   //in the One Step mode
  {
    glitchhlt = 10; //add extra settling time
    ReadStep(); //read this step
    ProcessAndPrint(); //process and print this step
    DisplayButtonsForHalted();    //ver114-4f replaced call to [UpdateBoxes]
    mAddMarker("Halt", vars->thisstep+1, "1");   //ver114-4d
    //If marker is shown on graph, we need to redraw the whole graph
    //Otherwise just redraw the marker info
    if (graph.doGraphMarkers) graph.RefreshGraph(0); else graph.mDrawMarkerInfo();  //No erasure gap in redraw ver114-5m
    if (vars->thisstep == vars->sweepEndStep)
    {
      //Note reversal is after graph is redrawn
      if (vars->alternateSweep) ReverseSweepDirection(); //ver114-4m; ver114-5e
      haltWasAtEnd=1;  //ver114-5c
    }
    else
    {
      haltWasAtEnd=0;  //ver114-5c
    }
    // fix me , need to work out what to do with the wait
    //wait //wait here for next button push ver113-6d
  }

  if (graph.haltsweep == 0)  //in first step after a Halt
  {
    graph.haltsweep = 1; //change flag to say we are not in first step after a Halt, for future steps
    glitchhlt = 10;  //add extra settling time
    ReadStep(); //read this step //ver113-6d
  }
  else  //if in middle of sweep. process and print the previous step, then read this step
  {
    ProcessAndPrintLastStep();
    ReadStep();//read this step //ver113-6d
  }
  //moved sweep time here, so it prints after any refresh action from the prior scan
  if (graph.displaySweepTime && vars->thisstep == vars->sweepStartStep)
  {
    int currTime = util.time("ms").toInt();
    if (suppressSweepTime == 0)
    {
      vars->message= "Sweep Time="+util.usingF("####.##", (currTime-startTime)/1000)+" sec.";
      graph.PrintMessage();   //ver114-4h
    }
    suppressSweepTime=0; //Only suppress on first scan //ver114-4h
    startTime=currTime;        //timer for testing
  }
  PostScan();
}

void MainWindow::PostScan()
{
  //17.[Scan] Check to see if a button has been pushed
  // Note that on any user action, if haltsweep=1 the action must have been detected
  //during the following "scan". But if haltsweep=0 the action occurred during a wait state.
  //Exception: Window resizing is detected when it happens, not during "scan".
  //[Scan] //ver113-6d
  //scan    //check for any button push and go there. ver111-26
  //otherwise, continue sweeping.
  //[PostScan]  . Label is used to return here after a button action handled by a [xyz] routine.
  //    Note: after a button handler in the form of a true subroutine, control will exit sub back
  //to this point. We do not want a "wait" to occur in such a subroutine, because that will suspend
  //control in a non-global namespace, and the user will be unable to take actions that require access
  //to [xyz] routines. To cause a halt, wait or restart in such a subroutine, the subroutine should set
  //the global variable continueCode to 1, 2 or 3.
  if (graph.continueCode!=0)  // =0 means continue normally
  {
    if (graph.continueCode==1)
    {
      graph.continueCode=0;
      Halted();
      return;
    }   //=1 means halt immediately
    // fix me handle wait
    //if (graph.continueCode==2) {graph.continueCode=0; haltsweep=0; wait;}     //=2 means wait immediately
    // fix me handle restart
    //continueCode=0 : haltsweep=0 : goto [Restart]    //Anything else means restart
  }
  IncrementOneStep();
}

void MainWindow::IncrementOneStep()
{
  //18.[IncrementOneStep]
  //SEW IncrementOneStep is the end of both the inner loop over points and the outer loop
  //SEW over scans. goto [CommandThisStep] continues the inner loop with the next point.
  //SEW goto[StartSweep] continues the outer loop with the next scan.
  //SEW [IncrementOneStep] is commented out to be clear it is not used for any goto.
  //[IncrementOneStep]
  if (vars->thisstep == vars->sweepEndStep && syncsweep == 1) SyncSweep(); //ver112-2b //ver114-4k
  //ver114-5a modified the following
  if (vars->sweepDir==1)   //ver114-4k added this block to handle possible reverse sweeps
  {
    // fix me handle goto
    //if (vars->thisstep<vars->sweepEndStep) then thisstep = thisstep + 1 : goto [CommandThisStep]    //ver114-4k
  }
  else
  {
    // fix me handle goto
    //if thisstep>sweepEndStep then thisstep = thisstep - 1 :goto [CommandThisStep]    //ver114-4k
  }
  //If we are here, we have just read the final step of a sweep
  if (graph.haltAtEnd==0)
  {
    //Alternate sweep directions if required. When we switch direction, thisstep
    //was the final point of one sweep and becomes the first point of the next.
    //We process and print it  immediately as the last point of this sweep; then reverse
    //direction and start with the same point. To avoid re-processing it at the next step we
    //set haltsweep=0.
    if (vars->alternateSweep)  //ver114-5c
    {
      ProcessAndPrint();
      ReverseSweepDirection();
      graph.haltsweep=0;
    }
    // fix me handle goto
    //goto [StartSweep]  Repeat loop over scans if halt flag not set
  }
  Halted();
}
     //We fall out of this loop only when haltAtEnd=1 and we reach thisstep=sweepEndStep
//[EndSweepSeries] //This label marks the end of the scan loops.

//19.[Halted]
void MainWindow::Halted()    // moved guts of this to FinishSweeping, which can also be called from elsewhere if desired.
{
  FinishSweeping();//get raw data, process, print to the computer monitor ver111-22
  if (vars->specialOneSweep)
  {
    vars->specialOneSweep=0;
    return; //Sweep process was called by gosub; we return to caller.
  }
  //wait //wait for operator action
}

void MainWindow::FinishSweeping()
{
  //created FinishSweeping;
  //ver114-6e split the non-graphing cleanup into [CleanupAfterSweep]
  //0 Do cleanup to end sweeping but return for further actions
  //This is a modified version of the former [Halted], without the wait at the end
  ProcessAndPrint();//process, print to the computer monitor ver111-22
  if (graph.haltAtEnd==0) mAddMarker("Halt", vars->thisstep+1, "1"); //Add Halt marker ver114-4d
  graph.haltsweep=0; //do now so RefreshGraph will "flush" ver115-1a
  if (vars->isStickMode==0)
  {
    if (graph.refreshOnHalt)   //ver115-8c
    {
      graph.refreshGridDirty=1;
      graph.RefreshGraph(1);  //redraw and show erasure gap; don't do if stick mode ver114-7d
    }
    else
    {
      //We sometimes don't want to waste time redrawing, such as when we are loading a data file,
      //but we at least need to flush to make the graphics stick.
      //#graphBox$, "flush"
    }
  }
  if (vars->specialOneSweep && vars->thisstep != vars->sweepEndStep)
  {
    util.beep(); vars->message="Sweep Aborted"; //ver115-4b
  }
  else
  {
    if (vars->calInProgress)
    {
      util.beep();
      vars->message="Calibration Complete";
    } //ver115-4b
  }
  //test is used for troubleshooting. Coder can insert
  //test = (any variable) anywhere in the code, and it will get displayed in the Messages Box during Halt.
  if (vars->test!=0)
    vars->message=QString::number(vars->test);
  if (vars->message!="")
    graph.PrintMessage();
  //Alternate sweep directions if required; added by ver114-5a
  if (vars->thisstep==vars->sweepEndStep)
  {
    if (vars->alternateSweep)
      ReverseSweepDirection();
  }
  //goto [CleanupAfterSweep]

  //[CleanupAfterSweep] //Do cleanup after a sweep to be sure flags are set/reset properly
  //Called by [FinishSweeping]. Can also be called by other routines to immediately
  //terminate a sweep when they will be Restarting so they don't care about finishing the plotting.
  DisplayButtonsForHalted();    //ver114-4f replaced call to [UpdateBoxes]
  if (vars->thisstep==vars->sweepEndStep) haltWasAtEnd=1; else haltWasAtEnd=0;  //ver114-5c
  graph.haltAtEnd=0;      //In case we got here from auto halt at end of sweep
  vars->calInProgress=0;  //ver114-5h
  graph.haltsweep = 0; //this says the sweep has been halted, so don't print the first command of the next sweep step //ver111-20
}
void MainWindow::ReverseSweepDirection()
{
  //[ReverseSweepDirection] //Reverse direction of sweep
  //This is called after sweepEndStep has been fully processed, but only if alternateSweep=1
  if (vars->sweepDir==1)
  {
    vars->sweepDir=-1;
    vars->sweepStartStep=vars->steps; vars->sweepEndStep=0;
  }
  else
  {
    vars->sweepDir=1;
    vars->sweepStartStep=0; vars->sweepEndStep=vars->steps;
  }
  graph.gSetSweepDir(vars->sweepDir); //Notify graph module of new direction
}

void MainWindow::CommandCurrentStep()
{
  //a separate gosub from the old [CommandThisStep] so it can be called not only during regular scanning,
  //but on in combination with [ReadStep] to command and read a particular step, once all info is set up.
  //needs:thisstep ; commands PLL1,DDS1,PLL3,DDS3,PDM //ver111-7
  //a. first, check to see if any or all the 5 module commands are necessary [DetermineModule]
  //b. calculate how much delay is needed for each module[DetermineModule], but use only the largest one[WaitStatement].
  //c. send individual data, clocks, and latch commands that are necessary for[CommandOrigCB]
  //or for SLIM, use [CommandAllSlims] for commanding concurrently //ver111-31c
  if (vars->suppressHardware==0)
  {
    int thisBand=vars->datatable[vars->thisstep][4];
    if (thisBand!=vars->lastSetBand)
      SelectLatchedSwitches(thisBand);   //Set band switch ver116-4s
    DetermineModule(); //determine which, if any, module needs commanding.
    int cmdneeded = glitchp1 + glitchd1 + glitchp3 + glitchd3 + glitchpdm;
    if (cmdneeded > 0 && activeConfig.cb == 0)
      lpt.CommandOrigCB();//old Control (150 usec, 0 SW) //ver111-28ver111-38a
    //if cb = 1 then gosub [CommandRevB]//old Control looking like SLIM  //not created yet
    if (cmdneeded > 0 && activeConfig.cb == 2)
      lpt.CommandAllSlims();//ver111-38a
    if (cmdneeded > 0 && activeConfig.cb == 3)
      CommandAllSlimsUSB(); //USB:01-08-2010
    if (cftest==1)
      CommandLO2forCavTest(); //cav ver116-4c
  }
}
void MainWindow::FindClientOffsets()
{

  /*
  //set clientWidthOffset and clientHeightOffset from test window ver115-1b
      //Open a small test window so we can find the client area to determine how much
       //smaller it is than the full window size.
     WindowWidth = 150 ; WindowHeight = 150
     UpperLeftX = 1 ; UpperLeftY = 1
     menu #handle, "File", "Save Image", [SaveImage] //We need a menu to get the size right
     open "Test" for window as #handle
          //Now that we have a window, find the actual client area--ver114-7o
     hWind = hWnd(#handle)        //Windows handle of graph window
     STRUCT Rect, leftX as long, upperY as long, rightX as long, lowerY as long //To hold the returned data
     calldll #user32,"GetClientRect", hWind as ulong, Rect as struct, r as long  //Fill Rect with size info
         //The offsets will be the size difference between the full window and the client area
     clientWidthOffset = 150-(Rect.rightX.struct-Rect.leftX.struct)
     clientHeightOffset = 150-(Rect.lowerY.struct-Rect.upperY.struct)
     close #handle   //We don't need the test window anymore
         */
  // fix me, got these values from running the basic program
  graph.clientHeightOffset = 46;
  graph.clientWidthOffset = 8;
}

void MainWindow::ResizeGraphHandler()
{
  /*
  //Called when graph window resizes
  #handle, "hide"     //hide window to avoid multiple system redraws ver115-1b
  #graphBox$ "home"
  #graphBox$ "posxy CenterX CenterY"
  */
  int CenterX = ui->graphicsView->width() / 2;
  int CenterY = ui->graphicsView->height() / 2;

  graph.currGraphBoxWidth = CenterX * 2-1;   //ver115-1c
  graph.currGraphBoxHeight = CenterY * 2-1;  //ver115-1c

  //Note: On resizing, all non-buttons seem to end up a few pixels higher than the original spec,
  //so the Y locations are adjusted accordingly via markTop
  //Note WindowHeight when window is created is entire height; on resizing, it is the client area only
  //int markTop=graph.currGraphBoxHeight+15;
  //int markSelLeft=5; //ver115-1b   //ver115-1c
  //int markEditLeft=markSelLeft+55;
  //int markMiscLeft=markEditLeft+185;
  //int configLeft=markMiscLeft+80;

/*
  #handle, "refresh"
  #handle.Cover, "!show"      //Cover the crap that can appear from resizing
  #handle.Cover, "!hide"      //Uncover and the crap is gone
  #handle, "show"     //show window ver115-1b
*/
    //The graphicbox auto resizes but we have to update the graph module
    //to let it know the new size

  graph.gUpdateGraphObject(graph.currGraphBoxWidth, graph.currGraphBoxHeight
                     , graph.graphMarLeft, graph.graphMarRight, graph.graphMarTop, graph.graphMarBot);
  ui->graphicsView->fitInView(graph.getScene()->sceneRect());
  graph.gCalcGraphParams();   //Calculate new scaling. May change min or max.
  float xMin, xMax;
  graph.gGetXAxisRange(xMin, xMax); if (vars->startfreq!=xMin || vars->endfreq!=xMax) graph.SetStartStopFreq(xMin, xMax);
  graph.gGenerateXValues(graph.gPointCount()); //recreate x values and x pixel locations; keep same number of points
  graph.gRecalcPix(0);   //0 signals not to recalc x pixel coords, which we just did in gGenerateXValues.
  //If a sweep is in progress, we don't want to redraw from here, because that can cause a crash.
  //So we just clear the graph and signal to wait for the user to redraw. This crash may have to
  //do with the fact that we don't know where we are in the sweep process when resizing is invoked,
  //because it is not synchronous with the scan command. Or it may simply have something to do with
  //the fact that no button has yet been pushed on the graph window, which somehow affects the
  //LB resizing process. The crash still sometimes occurs, so it is best to halt before resizing.
  if (graph.haltsweep==1)
  {
//    #graphBox$, "cls"
//    notice "Warning: Halt before resizing to avoid LB bug."
    RequireRestart(); //ver115-9e  Otherwise old graph still appears, in wrong place.
  }
  else
  {
    graph.refreshRedrawFromScratch=1; //To redraw from scratch ver115-1b
    RedrawGraph(0);  //Redraw at new size
  }
}

void MainWindow::ImplementDisplayModes()
{
  int doErase1, doErase2, eraseLead;
  //calculate the various items from Y1DisplayMode and Y2DisplayMode
  //Y1DisplayMode, Y2DisplayMode: 0=off  1=NormErase  2=NormStick  3=HistoErase  4=HistoStick
  //ver115-2c added checks for constNoGraph
  if ((vars->Y1DataType!=vars->constNoGraph && (vars->Y1DisplayMode==2 || vars->Y1DisplayMode==4))
    || (vars->Y2DataType!=vars->constNoGraph && (vars->Y2DisplayMode==2 || vars->Y2DisplayMode==4)))
  {
    vars->isStickMode=1;
  }
  else
  {
    vars->isStickMode=0;
  }
  graph.gSetDoAxis(vars->Y1DataType!=vars->constNoGraph, vars->Y2DataType!=vars->constNoGraph); //Turn graph data on or off ver115-3b
    //Note that gActivateGraphs won//t activate a graph if we just set its data existence to zero ver115-3b
  graph.gActivateGraphs(vars->Y1DisplayMode!=0,vars->Y2DisplayMode!=0);   //Turn actual graphing on or off ver115-3b
  if ((vars->Y1DataType!=vars->constNoGraph && vars->Y1DisplayMode>2) || (vars->Y2DataType!=vars->constNoGraph && vars->Y2DisplayMode>2))
  {
    graph.gSetDoHist(1);
  }
  else
  {
    graph.gSetDoHist(0);  //Set histogram or normal trace ver115-3b
  }
  int t1Width, t2Width;
  graph.gGetTraceWidth(t1Width, t2Width);
    //ver114-4n Erase eraseLead points ahead of drawing. The more steps, the larger eraseLead
  if (vars->globalSteps<=50) //ver114-4n reduced eraseLead
  {
    eraseLead=1;
  }
  else
  {
    eraseLead=2+int(vars->steps/400);
    if (((vars->Y1DataType!=vars->constNoGraph && t1Width>2) || (vars->Y2DataType!=vars->constNoGraph && t2Width>2)) && vars->globalSteps > 200)
    {
        eraseLead=eraseLead+1;
    }
  }
  if (vars->Y2DisplayMode!=1 && vars->Y2DisplayMode!=3) doErase2=0; else doErase2=1; //ver114-2f
  if (vars->Y1DisplayMode!=1 && vars->Y1DisplayMode!=3) doErase1=0; else doErase1=1; //ver115-3b
  graph.gSetErasure(doErase1, doErase2, eraseLead);
}
void MainWindow::UpdateGraphParams()
{
  //added UpdateGraphParams; ver114-4n made it a gosub to allow use of non-globals
  //Set up graphs for drawing, but don't draw anything
  if (vars->alternateSweep) {vars->sweepDir=1; graph.gSetSweepDir(1);}      //Start out forward if alternating ver114-5a
  int sweepDir=graph.gGetSweepDir();  //ver114-4k
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

  //ver115-3b deleted settings related to Y2DisplayMode and Y1DisplayMode. They were overridden in ImplementMagPhaDisp,
  //which is called by UpdateGraphDataFormat
  //ver115-1d deleted the separate call for linear mode. startfreq and endfreq are now valid in all modes.
  graph.gInitGraphRange(vars->startfreq, vars->endfreq, vars->Y1Bot, vars->Y1Top, vars->Y2Bot, vars->Y2Top);  //min and max values for x, y1 and y2; calls gCalcGraphParams
  graph.gCalcGraphParams();   //Calculate new scaling. May change min or max.
  float xMin, xMax;
  graph.gGetXAxisRange(xMin, xMax);   //in case gCalcGraphParams changed axis limits ver116-4k
  if (vars->startfreq != xMin || vars->endfreq != xMax) graph.SetStartStopFreq(xMin, xMax);
  //ver114-5f moved the following here from step 12
  graph.gGenerateXValues(0);   //Precalculate x values for steps+1 points; reset number of points to 0; ver114-1f deleted parameter
  UpdateGraphDataFormat(0);
}

void MainWindow::UpdateGraphDataFormat(int doTwoPort)
{
  //Update graph module for the type of data we are graphing, and set data source and component
  //If doTwoPort, we are dealing with two-port graphs ver116-1b
  graph.gSetGridStyles("ENDSANDCENTER", "All", "All");
  //For linear sweep we display frequency in MHz; for log we do 1, 1 K, 1 M, or 1 G
  QString xForm, yForm, y1Form, y2Form;
  if (graph.gGetXIsLinear())     //ver114-6d modified this block to use startfreq/endfreq for log sweeps
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
      if (dataNum==1) graph.componConst=TwoPortGetY1Type(); else graph.componConst=TwoPortGetY2Type();
    }
    else
    {
      if (dataNum==1) graph.componConst=vars->Y1DataType; else graph.componConst=vars->Y2DataType;
    }

    int doData;
    if (graph.componConst==vars->constNoGraph)
    {
      doData=0; //Indicates whether there is a graph ver115-2c
      yAxisLabel ="None"; yLabel="None";
      yForm="####.##";    //Something valid, in case it gets mistakenly used
      if (dataNum==1)
      {
        y1AxisLabel="None"; y1Label="None";
        y1Form="####.##";    //Something valid, in case it gets mistakenly used
      }
      else
      {
        y2AxisLabel="None"; y2Label="None";
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
          TwoPortDetermineGraphDataFormat(graph.componConst, y1AxisLabel,y1Label, y1IsPhase,y1Form);
        }
        else
        {
          DetermineGraphDataFormat(graph.componConst, y1AxisLabel,y1Label, y1IsPhase, y1Form);
        }
      }
      else
      {
        if (doTwoPort)
        {
          TwoPortDetermineGraphDataFormat(graph.componConst, y2AxisLabel,y2Label, y2IsPhase,y2Form);
        }
        else
        {
          DetermineGraphDataFormat(graph.componConst, y2AxisLabel,y2Label, y2IsPhase,y2Form);
        }
      }
    }
    if (dataNum==1) graph.gDoY1=doData; else graph.gDoY2=doData;
  }

  graph.gSetIsPhase(y1IsPhase, y2IsPhase);   //Tell graph module whether data is phase
  graph.gSetAxisFormats(xForm, y1Form, y2Form);   //Formats for displaying the data values
  graph.gSetAxisLabels("", y1AxisLabel, y2AxisLabel);    //Labels for the axes; No label for freq
  graph.gSetDataLabels(y1Label, y2Label);      //Shorter labels for marker info
  if (doTwoPort) //ver116-1b
  {
    //gSetDoAxis specifies whether data for the axis even exists. gActivateGraphs specifies whether
    //to actually graph the data, based on display mode, which for two port is always On.
    graph.gSetDoAxis(TwoPortGetY1Type()!=vars->constNoGraph, TwoPortGetY2Type()!=vars->constNoGraph); //Turn graph data on or off ver115-3b
    //Note that gActivateGraphs won//t activate a graph if we just set its data existence to zero ver115-3b
    graph.gActivateGraphs(1, 1);   //Turn actual graphing on
  }
  else
  {
    ImplementDisplayModes();  //give effect to Y2DisplayMode and Y1DisplayMode
  }
}

void MainWindow::DetermineGraphDataFormat(int componConst, QString &yAxisLabel, QString &yLabel, int &yIsPhase, QString &yForm)
{
  //Return format info
  //graph.componConst indicates the data type. We return
  //yAxisLabel$  The label to use at the top of the Y axis
  //yLabel$   A typically shorter label for the marker info table
  //yIsPhase$ =1 if the value represents phase. This indicates whether we have wraparound issues.
  //yForm$    A formatting string to send to uFormatted$() to format the data
  //
  //ver116-1b added code to display S12 or S22 instead of S21 or S11 when DUT is reversed.
  QString Sref, Strans;
  if (vars->switchFR==0)
  {
    Sref="S11"; Strans="S21"; //Forward DUT
  }
  else
  {
    Sref="S22" ; Strans="S12";  //ReverseDUT
  }
  yIsPhase=0;  //Default, since most are not phase
  //select case graph.componConst //ver116-4b shortened some labels
  if (graph.componConst == vars->constGraphS11DB)
  {
    yAxisLabel=Sref+" Mag(dB)" ; yLabel=Sref+" dB";
    yForm="####.###";   //ver115-5d
  }
  else if (graph.componConst == vars->constRawAngle) //Used for transmission mode only  //added by ver115-1i
  {
    yAxisLabel="Raw Deg" ; yLabel="Raw Deg";
    yIsPhase=1;
    yForm="#####.##";   //ver115-5d
  }
  else if (graph.componConst == vars->constAngle || graph.componConst == vars->constGraphS11Ang || graph.componConst == vars->constTheta || graph.componConst == vars->constImpedAng)
  {
    if (graph.componConst==vars->constAngle) {yAxisLabel=Strans+" Deg" ; yLabel=Strans+" Deg";}
    if (graph.componConst==vars->constTheta) {yAxisLabel="Theta" ; yLabel="Theta";}
    if (graph.componConst==vars->constGraphS11Ang) {yAxisLabel=Sref+" Deg" ; yLabel=Sref+" Deg";}
    if (graph.componConst==vars->constImpedAng) {yAxisLabel="Z Deg" ; yLabel="Z Deg";}
    yIsPhase=1;
    yForm="#####.##";     //ver115-5d
  }
  else if (graph.componConst == vars->constGD)    //calc group delay
  {
    yAxisLabel="Grp Delay (sec)" ;  yLabel="G.D.";
    yForm="3,2,4//UseMultiplier//DoCompact";
  }
  else if (graph.componConst == vars->constSerReact)
  {
    yAxisLabel="Xs" ; yLabel="Xs";
    yForm="3,3,4//UseMultiplier//SuppressMilli//DoCompact"; //ver115-4e
  }
  else if (graph.componConst == vars->constParReact)
  {
    yAxisLabel="Xp" ; yLabel="Xp";
    yForm="3,3,4//UseMultiplier//SuppressMilli//DoCompact"; //ver115-4e
  }
  else if (graph.componConst == vars->constImpedMag)
  {
    yAxisLabel="Z ohms" ; yLabel="Z ohms";
    yForm="3,3,4//UseMultiplier//SuppressMilli//DoCompact"; //ver115-4e
  }
  else if (graph.componConst == vars->constSerR)
  {
    yAxisLabel="Rs" ; yLabel="Rs";
    yForm="3,3,4//UseMultiplier//SuppressMilli//DoCompact"; //ver115-4e
  }
  else if (graph.componConst == vars->constParR)
  {
    yAxisLabel="Rp" ; yLabel="Rp";
    yForm="3,3,4//UseMultiplier//SuppressMilli//DoCompact"; //ver115-4e
  }
  else if (graph.componConst == vars->constSerC)
  {
    yAxisLabel="Cs" ; yLabel="Cs";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (graph.componConst == vars->constParC)
  {
    yAxisLabel="Cp" ; yLabel="Cp";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (graph.componConst == vars->constSerL)
  {
    yAxisLabel="Ls" ; yLabel="Ls";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (graph.componConst == vars->constParL)
  {
    yAxisLabel="Lp" ; yLabel="Lp";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (graph.componConst == vars->constMagDBM)
  {
    if (vars->msaMode=="SA")
    {
      yAxisLabel="Magnitude (dBm)" ; yLabel="dBm";
    }
    else
    {
      yAxisLabel="Power (dBm)" ; yLabel="dBm"; //ver115-1i
    }
    yForm="####.###";    //ver115-5d
  }
  else if (graph.componConst == vars->constMagWatts)
  {
    yAxisLabel="Magnitude (Watts)" ; yLabel="Watts";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (graph.componConst == vars->constMagDB)         //Only done for Transmission
  {
    if (vars->msaMode=="ScalarTrans")  //ver115-1a
    {
      yAxisLabel="Transmission (dB)"  ; yLabel="dB";
    }
    else
    {
      yAxisLabel=Strans+" dB"  ; yLabel=Strans+" dB";
    }
    yForm="####.###";    //ver115-1e
  }
  else if (graph.componConst == vars->constMagRatio)  //Only done for TG mode transmission
  {
    if (vars->msaMode=="ScalarTrans")   //ver115-4f
    {
      yAxisLabel="Trans (Ratio)" ; yLabel="Ratio";
    }
    else
    {
      yAxisLabel="Mag (Ratio)" ; yLabel="Ratio";
    }
    yForm="3,3,4//UseMultiplier//SuppressMilli//DoCompact"; //ver115-4e
  }
  else if (graph.componConst == vars->constMagV)
  {
    yAxisLabel="Mag (Volts)"  ; yLabel="Volts";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (graph.componConst == vars->constRho)
  {
    yAxisLabel="Rho"  ; yLabel="Rho";
    yForm="#.###";
  }
  else if (graph.componConst == vars->constReturnLoss) //ver114-8d
  {
    yAxisLabel="RL"  ; yLabel="RL";
    yForm="###.###";    //ver115-1e
  }
  else if (graph.componConst == vars->constInsertionLoss)  //ver114-8d
  {
    yAxisLabel="Insertion Loss(dB)"  ; yLabel="IL";
    yForm="###.###";    //ver115-1e
  }
  else if (graph.componConst == vars->constReflectPower)  //ver115-2d
  {
    yAxisLabel="Reflect Pow(%)"  ; yLabel="Ref%";
    yForm="###.##";
  }
  else if (graph.componConst == vars->constComponentQ)     //ver115-2d
  {
    yAxisLabel="Component Q"  ; yLabel="Q";
    yForm="#####.#";
  }
  else if (graph.componConst == vars->constSWR)  //ver114-8d
  {
    yAxisLabel="SWR"  ; yLabel="SWR";
    yForm="####.##";
  }
  else if (graph.componConst == vars->constAdmitMag)  //ver115-4a
  {
    yAxisLabel="Admit. (S)" ; yLabel="Y";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (graph.componConst == vars->constAdmitAng)   //ver115-4a
  {
    yAxisLabel="Admit Deg" ; yLabel="Admit Deg";
    yIsPhase=1;
    yForm="#####.##";
  }
  else if (graph.componConst == vars->constConductance)  //ver115-4a
  {
    yAxisLabel="Conduct. (S)" ; yLabel="Conduct";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (graph.componConst == vars->constSusceptance)  //ver115-4a
  {
    yAxisLabel="Suscep. (S)" ; yLabel="Suscep";
    yForm="3,3,4//UseMultiplier//DoCompact";
  }
  else if (graph.componConst == vars->constNoGraph)   //ver115-2c
  {
    yAxisLabel="None"  ; yLabel="None";
    yForm="####.##";    //Something valid, in case it gets mistakenly used
  }
  else if (graph.componConst == vars->constAux0 || graph.componConst == vars->constAux1 || graph.componConst == vars->constAux2
           || graph.componConst == vars->constAux3 || graph.componConst == vars->constAux4 || graph.componConst == vars->constAux5)
  {
    int auxNum=graph.componConst-vars->constAux0;    //e.g. constAux4 produces 4
    yAxisLabel=vars->auxGraphDataFormatInfo[auxNum][2]  ; yLabel=vars->auxGraphDataFormatInfo[auxNum][3];
    yForm=vars->auxGraphDataFormatInfo[auxNum][1];
  }
  else
  {
    yForm="###.##";
    yAxisLabel="Invalid"  ; yLabel="Invalid";
  }

}
void MainWindow::InitGraphParams()
{
  //added InitGraphParams; ver114-4n made it a gosub to allow use of non-globals
  //Initial setup for graphs
  //This is called one time early on, or to re-initialize default values
  //The following intialize a lot of variables that the user may subsequently override
  vars->wate=0 ; vars->offset=0 ; vars->sgout=10; //ver114-3c ver114-4h
  vars->gentrk=0 ; vars->normrev=0;
  vars->msaMode="SA";   //ver114-5L
  vars->primaryAxisNum=2;   //Primary Y axis. Eg. on restart in SA mode, mag dBm goes here moved ver116-4m
  SetDefaultGraphData();    //ver115-3b
  vnaCal.S21JigAttach="Series";  //ver115-1b
  vnaCal.S21JigR0=50 ; vnaCal.S21JigShuntDelay=0;
  vnaCal.S11BridgeR0=50 ; vnaCal.S11GraphR0=50;
  vnaCal.S11JigType="Reflect";   //ver115-1b
  UpdateGraphDataFormat(0);  //Updates from graphDataType$
  vars->planeadj=0;    //ver 114-4k
  //freqBand=1      //delver116-1b this is done elsewhere on startup
  //call gSetMode msaMode$ delver116-1b
  SetCenterSpanFreq(0, activeConfig.finalbw/100); //Note finalbw is in kHz, others are in MHz
  graph.gSetNumDivisions(10,10); //Number of Hor and Vert divisions
  graph.gSetTraceWidth(1, 1);     //Set width of both graph traces
  graph.gUsePresetColors("DARK");    //Activate the appearance ver114-3c
  graph.gSetIsLinear(1, 1, 1);     //default is linear sweep ver114-3c
  graph.gSetNumDynamicSteps(vars->globalSteps);    //ver114-4k
  vars->alternateSweep=0; vars->sweepDir=1;   //ver 114-5a
  graph.gSetSweepDir(1);   //set sweep to forward and set start/end point nums
  vars->specialOneSweep=0;   //No special sweep in progress
  graph.doGraphMarkers=1;
  graph.refreshEachScan=1;
  graph.referenceTrace=3;
  QString dum1, dum2;
  gridappearance.gGetSupplementalTraceColors(graph.referenceColor1, graph.referenceColor2, dum1, dum2); //ver116-4b
  graph.referenceWidth1=1;
  graph.referenceWidth2=1;
  graph.referenceWidthSmith=1;
  graph.referenceColorSmith=graph.referenceColor2;  //ver116-4b
  vars->lineCalThroughDelay=0;
  for (int i=0; i < 5; i++)    //clear custom names  ver115-2a
  {
    vars->customPresetNames[i]="Empty";
  }
  FillAppearancesArray();   //ver115-2a
  graph.gSetPrimaryAxis(vars->primaryAxisNum); //ver115-3c
  UpdateGraphParams();

}
void MainWindow::ReadStep()
{

  //and put raw data bits into arrays. //heavily modified ver116-1b
  int nonPhaseMode=((vars->msaMode=="SA") || (vars->msaMode=="ScalarTrans"));   //ver116-4e
  int doingPhase= ((vars->suppressPhase==0) && (nonPhaseMode==0));   //ver116-4e
  int phaseIsStable;
  int magIsStable=0;
  int changePhaseADC=0;
  int changeMagADC=0;
  int repeatOnceMore=0;
  if (vars->useAutoWait)
  {
    vars->wate=(int)(vars->autoWaitTC+0.5);  //wait this much between measurements ver116-4j
    if (doingPhase) phaseIsStable=0; else phaseIsStable=1;
  }
  int prevReadPhaseData = 0;
  int prevReadMagData = 0;
  for (int readStepCount=1; readStepCount <= 25; readStepCount++)    //ver116-1b added auto wait time procedures
  {
    //If doing auto wait, repeat up to 25 times until readings become stable, as shown by comparing two
    //successive readings. If not doing auto wait, we bail out in the middle of the first pass.
    //if readStepCount=1 then readTime=uTickCount()  //DEBUG
    WaitStatement();//needs:wate,glitch variables,glitchtime ;slows program before reading data //ver111-20b
    prevReadMagData=magdata; //Note if we are starting a new step, but not first of sweep, this is last step final data
    magdata = 0; //reset this variable before reading data
    //Read phase even in non-phase modes unless suppressPhase=1; we just don't process it in non-phase modes
    if (vars->suppressPhase==0)
    {
      //ver116-4r deleted           UsbAdcControl.Adcs.struct = 3 // USB: 15/08/10
      prevReadPhaseData=phadata;
      ReadPhase();
      //and return with phadata(in bits). Also installed into pharray(thisstep,3).
      // If serial AtoD, magdata is returned, but not installed in any array
      //if magdata is collected during [ReadPhase], skip Read Magnitude
    }
    else
    {
      phadata=0;   //zero phase info if we are suppressing phase
      vars->phaarray[vars->thisstep][3]=0; //phadata
      vars->phaarray[vars->thisstep][4]=0; //pdm Read state
    }

    //prevReadTime=readTime  //DEBUG
    //readTime=uTickCount()  //DEBUG
    if (magdata == 0)
    {
      //ver116-4r deleted            UsbAdcControl.Adcs.struct = 1 // USB: 15/08/10
      ReadMagnitude();//and return with raw magdata bits
    } //USB:05/12/2010

    vars->magarray[vars->thisstep][3] = magdata; //put raw data into array
    //the phadata could be in dead zone, but magnitude is still valid.
    //if in VNA Mode and PDM is in automatic, check for phasedata (bits) for limits
    //If magnitude is so low that phase is not valid and will be forced to zero, don't do PDM inversion.
    int readStepDidInvert=0;
    if (doingPhase)
    {
      if (setpdm == 0 && vars->suppressPDMInversion==0
          && (phadata < pdmlowlim || phadata > pdmhighlim))
      {
        //Invert PDM and re-read after waiting. But if mag reading is too low for phase
        //to be valid, don't bother.
        if (magdata>=vars->validPhaseThreshold)
        {
          readStepDidInvert=1;
          InvertPDmodule(); //ver116-1b
        }
      }
    }
    //Note InvertPDmodule will impose some wait time, but not very much in auto wait mode.
    if (vars->useAutoWait==0) break; //exit for
    //The rest of the loop is just for determining whether to repeat when doing auto wait
    if (repeatOnceMore)
    {
      //            if thisstep>=45 and thisstep<=55 then    //For DEBUG
      //                print "Final Repeat: ms=";magIsStable;" ps=";phaseIsStable;" magChange=";magdata-prevReadMagData;" phaChange=";phadata-prevReadPhaseData
      //            end if
      break; //exit for //we just finished the extra repeat
    }
    //Decide whether we need to repeat
    //        if readStepCount=1 and thisstep>=45 and thisstep<=55 then    //For DEBUG
    //            print "-----------";thisstep;"--------------"
    //            print "First Read: wait=";wate;" ms=";magIsStable;" ps=";phaseIsStable;" mag=";magdata;" pha=";phadata; " Delay=";readTime-prevReadTime
    //        end if

    //We want to keep reading until two successive reads are close to each other, or until the direction reverses.
    //We initially waited one half time constant of the magnitude filter, took a reading, waited,
    //and took a second reading. We determine here whether the changes were less than a predetermined value
    //Once mag or phase is determined to be stable, we flag it as stable so we don't have
    //to re-evaluate after the next read.
    int directionReversal=0; //flag for reversal of sign of change
    int lowLevelADC=0;
    int evaluateThisRead = 0;
    if (vars->thisstep!=vars->sweepStartStep)
    {
      //For the first read, we generate the change in readings from the readings left over
      //from the previous step, but only if this is not the first step.
      evaluateThisRead=1;
    }
    else
    {
      //Always evaluate if not first read
      if (readStepCount>1) evaluateThisRead=1; else evaluateThisRead=0;
    }

    if (evaluateThisRead)
    {
      if (magIsStable==0)   //Evaluate mag change if mag not already deemed stable
      {
        int prevMagADCChange=changeMagADC;  //save previous change to compare direction
        changeMagADC=magdata-prevReadMagData;

        int maxADCChange = 0;
        if (magdata<vars->calADCofLowFringe)
        {
          //For very low level signals, just repeat once more, but not if Wide filter
          //We can never expect these to be perfectly stable.
          lowLevelADC=1; if (vars->videoFilter!="Wide") repeatOnceMore=1;
          magIsStable=1; phaseIsStable=1; //pretend. Note phase will be no good at this level so we don't evaluate it.
        }
        else if (magdata<vars->calLowADCofCenterSlope)
        {
          maxADCChange=vars->autoWaitMaxChangeLowEndADC;
        }
        else if (magdata>vars->calHighADCofCenterSlope)
        {
          maxADCChange=vars->autoWaitMaxChangeHighEndADC;
        }
        else   //in center region
        {
          if (changeMagADC<0)
          {
            //If in center on first read but headed for low end, we may end up
            //in the low end, so may want to use the minimum allowed change for center and low end
            //The low end allowed change is likely much smaller than that for center.
            if (abs(changeMagADC)>(magdata-vars->calLowADCofCenterSlope))    //See whether another change like this gets us to low end
            {
              maxADCChange=qMin(vars->autoWaitMaxChangeCenterADC,vars->autoWaitMaxChangeLowEndADC);
            }
            else
            {
              maxADCChange=vars->autoWaitMaxChangeCenterADC;
            }
          }
          else
          {
            maxADCChange=vars->autoWaitMaxChangeCenterADC;
          }
        }

        if (abs(changeMagADC)<=maxADCChange) magIsStable=1;
        if (magIsStable==0 && lowLevelADC==0 && readStepCount>1)
        {
          //Even if we don't have two close readings, we can stop when
          //the direction of change reverses, which is a sign the change is being dominated by noise.
          if ((prevMagADCChange<0 && changeMagADC>0) || (prevMagADCChange>0 && changeMagADC<0))
          {
            magIsStable=1;
            directionReversal=1;
          }
        }
      }

      //Now do phase
      if (phaseIsStable==0) //evaluate phase change if phase not already deemed stable
      {
        //Note that if we just inverted phase, it is still valid to compare this phadata to the
        //previous one, because settling time will still be based on that change.
        if (magdata<vars->validPhaseThreshold)
        {
          //In this case we have no valid phase reading. If this is likely a final reading, then just deem
          //phase to be stable.
          if (magIsStable) phaseIsStable=1; //can//t actually evaluate low level phase
        }
        else
        {
          int prevPhaseADCChange=changePhaseADC;
          changePhaseADC=phadata-prevReadPhaseData;
          if (abs(changePhaseADC)<=vars->autoWaitMaxChangePhaseADC)
          {
            phaseIsStable=1;
          }
          else
          {
            //Even if we don't have two close readings, we can stop when
            //the direction of change reverses, which is a sign the change is being dominated by noise.
            //Can//t do this if we just inverted the PDM for this read, because that might cause
            //reversal of sign of the change
            if (readStepDidInvert==0 && readStepCount>1
                && ((prevPhaseADCChange<0 && changePhaseADC>0)  || (prevPhaseADCChange>0 && changePhaseADC<0)))
            {
              phaseIsStable=1;
              directionReversal=1;
            }
          }
        }
      }
      //            if thisstep>=45 and thisstep<=55 then    //For DEBUG
      //                print "Analysis "; readStepCount; ": ms=";magIsStable;" ps=";phaseIsStable;" mag=";magdata;" pha=";phadata;" magChange=";changeMagADC;" phaChange=";changePhaseADC;" Delay=";readTime-prevReadTime
      //            end if
      if (magIsStable && phaseIsStable)
      {
        //If Precise, repeat one more
        //time and stop after that without comparing readings.
        //low level is already set to repeat once more unless Wide filter
        if (vars->autoWaitPrecision=="Precise") repeatOnceMore=1;
        //For lowLevelADC, we already set repeatOnceMore (and phase is meaningless)
        if (lowLevelADC==0 && repeatOnceMore==0) break; // then exit for
        //Also repeat once more if ending because of direction reversal, other than
        //direction reversal on second read.
        if (directionReversal && readStepCount<3) repeatOnceMore=1; else break; //exit for
      }
    }
    //No point repeating if we aren//t actually reading data. But we went through the motions above
    //for possible debugging use.
    if (vars->suppressHardware) break;// then exit for
  }
}
void MainWindow::ProcessAndPrintLastStep()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ProcessAndPrintLastStep]
    rememberstep = thisstep //remember where we were when entering this routine //ver111-19
    //since we are processing and printing the previous step, use raw data in array(thisstep - sweepDir,data)

    if thisstep=sweepStartStep then     //Added by ver114-4m
        thisstep=sweepEndStep   //back up one and wrap around
    else
        thisstep=thisstep-sweepDir  //Back up one step; no wraparound to worry about
    end if
    gosub [ProcessAndPrint]//get raw data, process, print to the computer monitor ver111-22
    thisstep = rememberstep //ver111-19
    return

*/
}
void MainWindow::WaitStatement()
{
  //needed;wate,glitch()(p1,d1,p3,d3,pdm,hlt),glitchtime ; this slows the program //ver111-27
  glitch = qMax(qMax(qMax(glitchp1, glitchd1),qMax(glitchp3, glitchd3)), qMax(glitchpdm, glitchhlt)); //ver111-27
    //glitchp1=PLL1;glitchd1=DDS1;glitchp3=PLL3;glitchd3=DDS3;glitchpdm=PDM(10);glitchhlt=halted(10)
  int waittime = vars->wate + glitch;   //number of ms we need to wait //ver115-1i
    //in my Toshiba, a waittime count of 80 gives a delay of approx, 1 millisecond
    //therefore, each increment of any "glitchXX" or "wate" (Wait Box) should add 1 ms of delay before a "read"
  if (waittime>0)   //ver115-1i added the use of the system sleep function
  {
    if (vars->doingInitialization || waittime<15)
    {
      //For short wait times we use our own timing loop
      //Also for initialization, when we are measuring glitchtime
      waittime=waittime*vars->glitchtime;
      int timecounter = 0; //ver111-27
      //[TimeLoop] //ver111-27
      while (timecounter < waittime)
      {
        timecounter = timecounter + 1;
      }//;goto [TimeLoop] //ver111-27
    }
    else
    {
        //It is preferable to use the system Sleep function, but it operates in increments of 10-15 ms.
        //So it is only suitable for long wait times
      uSleep(waittime);
    }
  }
  glitchp1=0;glitchd1=0;glitchp3=0;glitchd3=0;glitchpdm=0;glitchhlt=0; //reset glitch variables back to 0 //ver111-27
}
void MainWindow::AutoGlitchtime()
{

  int whatiswate;
  QString a;
  QString b;
  //ver111-37c
  vars->glitchtime = 10000;
  whatiswate = vars->wate;
  vars->wate = 1;
  a = util.time("ms"); //time of day, in milliseconds. This uses the computer//s internal clock
  WaitStatement();
  b = util.time("ms");
  int lasped = b.toInt()-a.toInt();
  if (lasped == 0)
    lasped = 1;
  vars->glitchtime = vars->glitchtime/lasped; //glitchtime is the value required for a 1 ms wait time
  vars->wate = whatiswate; //change wate back to it//s original global value

}
void MainWindow::ReadMagnitude()
{
  //needed: port,status ; creates: magdata (and phadata for serial A/D//s)
  if (vars->suppressHardware) { magdata=0; return;} //ver115-6c
  if (activeConfig.adconv ==  8) lpt.Read8Bitmag(); //and return here with magdata
  if (activeConfig.adconv == 12) lpt.Read12Bitmag(); //and return here with magdata
  if (activeConfig.cb == 3) ReadADCviaUSB(); // and return here with magdata and phadata //ver116-4r
  if (activeConfig.cb != 3 && activeConfig.adconv == 16) { lpt.ReadAD16Status(); lpt.Process16Mag();} //ver116-4r
    //and return here with just magdata //ver111-33b
    if (activeConfig.cb != 3 && activeConfig.adconv == 22) { lpt.ReadAD22Status(); lpt.Process22Mag();} //ver116-4r
    //and return here with just magdata //ver111-37a
}
void MainWindow::ReadPhase()
{

  //needed: port,status ; creates: phadata (and magdata for serial A/D//s)
  if (vars->suppressHardware)  //ver115-6c
  {
    phadata=0;
  }
  else
  {
    switch(activeConfig.adconv)  //ver116-1b
    {
    case 8:
      lpt.Read8Bitpha(); //and return here with phadata only
      break;
    case 12:
      lpt.Read12Bitpha(); //and return here with phadata only
      break;
    default:
      if (activeConfig.cb == 3) ReadADCviaUSB(); //ver116-4r
      if (activeConfig.cb != 3 && activeConfig.adconv == 16) { lpt.ReadAD16Status(); lpt.Process16MagPha();} //ver116-4r
      if (activeConfig.cb != 3 && activeConfig.adconv == 22) { lpt.ReadAD22Status(); lpt.Process22MagPha();} //ver116-4r
      // and return here with magdata and phadata
    }
  }
  if (vars->doSpecialGraph>0) phadata=activeConfig.maxpdmout/4 + vars->thisstep*30;    //Force to a value not requiring constant inversion
  //and return here with phadata (and magdata, if serial AtoD) //ver111-33b
  //if calibrating the PDM inversion, don't put raw data into arrays, used only in [CalPDMinvdeg]
  if (vars->doingPDMCal == 1) return; //to [CalPDMinvdeg] //ver111-29 ver114-5L
  vars->phaarray[vars->thisstep][3] = phadata; //put raw data into array //ver112-2a
  vars->phaarray[vars->thisstep][4] = vars->phaarray[vars->thisstep][0]; //PDM state at which this data is taken. ver112-2a
  //it is only used in Variables Windows to show state of PDM when data was collected.
  return; //to [ReadStep]

}
void MainWindow::InvertPDmodule()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[InvertPDmodule]//this will change the state of the PDM for this step and future steps //ver111-28
    //entered from [ReadStep], where it was determined that phadata was in, or close to "dead zone"
    //this subroutine will re-command PDM, and return to [ReadStep] and read the phase again,
      //but not test for dead zone again, just assumes data to be viable
    //determine what the pdmstate was when entering, and "flip" it
    if suppressHardware then return //ver115-6c
    if phaarray(thisstep,0) = 0 then newpdmstate = 1 //ver112-2a
    if phaarray(thisstep,0) = 1 then newpdmstate = 0 //ver112-2a
    //change the pdm state for thisstep to the newpdmstate
    phaarray(thisstep,0) = newpdmstate
    //now, go and command the PDM to the new state (Command PDM only!)
    gosub [CommandPDMonly] //command just the PDM ver111-28
    //this has just created a large glitch in the PDM output, so while it is settling down,
    //change future PDM commands for all subsequent steps to end of sweep.
    for i = thisstep to sweepEndStep step sweepDir //ver114-5a
        phaarray(i,0) = newpdmstate //inverts pdmcmd for thisstep and subsequent steps to end of sweep. ver111-28
    next i
    //add appropriate wait time before reading the phase again
    gosub [VideoGlitchPDM]//calculates glitchpdm, depending on Video Selection and auto wait state
    gosub [WaitStatement] //and use the new glitchpdm value
    //now, go and read Phase, again. Use its "return" to return to [ReadStep]
    goto [ReadPhase] //re-read the phase and return to [ReadStep]
    //when back in [ReadStep], it will not test for dead zone again. Assumes valid Phase.
*/
}
void MainWindow::VideoGlitchPDM()
{
//entered from [InvertPDmodule],[PDM] //changed by ver116-1b
  //We want to wait 10 ms plus 12 time constants when inverting the PDM, but max of 5 seconds.
  //If auto wait mode, we wait less time. For small time constants, we wait a certain minimum in auto wait because
  //we need the PDM to become stable before we can rely on steady settling per the time constant.
  if (vars->useAutoWait) glitchpdm=glitchpdm + (int)(20+vars->videoPhaseTC*5); else glitchpdm=glitchpdm + (int)(20+vars->videoPhaseTC*12);  //in ms ver116-4j
  if (glitchpdm>5000) glitchpdm=5000;   //5 sec max
  if (vars->useAutoWait==0 && glitchpdm<vars->wate) glitchpdm=vars->wate; //no less than the normal wate time
  //    if videoFilter$ = "Wide" then glitchpdm = glitchpdm + 10 // or maybe just glitchpdm=10, etc
  //    if videoFilter$ = "Mid" then glitchpdm = glitchpdm + 100
  //    if videoFilter$ = "Narrow" then glitchpdm =  glitchpdm + 1000

}
void MainWindow::CalPDMinvdeg()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CalPDMinvdeg] //to find the amount of phase shift when the PDM state is inverted
    //invdeg is a calibration value used in [ConvertPhadata], (phase of inverted PDM) - (invdeg) = real phase of PDM. ver113-7b
    //the VNA must be in "0" sweepwidth, freq close to the transition point.
    if suppressHardware then //ver115-6c
        message$=180;"  ";phase1 : call PrintMessage //ver114-4e
        return
    end if
    rememberpdmstate = phaarray(thisstep,0) //ver112-2e
    #handle.Restart, "Cal PDM"      //ver114-4c deleted print to #main.restart
    beep
    phaarray(thisstep,0) = 0 //command PDM to Normal //ver111-29
    gosub [CommandPDMonly] //ver111-29
    glitchpdm = 5000 //should equate to 5 seconds of delay //ver111-29
    gosub [WaitStatement] //ver111-29
    gosub [ReadPhase]//return here with phadata //ver111-29
    //the 16 bit serial has just been "hit" with a conv (D7)in Commanding the Orig PDM. OK to hit it again? Yes.
    //expect phadata to be either 80% or 20% of maxpdmout
    phase0 = 360*phadata/maxpdmout //convert to degrees
    phaarray(thisstep,0) = 1 //command PDM to Inverted //ver111-29
    gosub [CommandPDMonly] //ver111-29
    glitchpdm = 5000 //should equate to 5 seconds of delay //ver111-29
    gosub [WaitStatement] //ver111-29
    gosub [ReadPhase]//return here with phadata //ver111-29
    //expect phadata to be either 20% or 80% of maxpdmout
    phase1 = 360*phadata/maxpdmout //convert to degrees
    CalInvDeg = phase1 - phase0     //ver114-5L
    if CalInvDeg < 0 then CalInvDeg = CalInvDeg + 360
    //The inversion deviation from 180 degrees is really a fixed time period, so its value in degrees depends
    //on frequency. We store invdeg normalized to 10.7 MHz.
    CalInvDeg=180+(CalInvDeg-180)*10.7/finalfreq    //ver116-1b
    CalInvDeg = val(using("####.##",CalInvDeg)) //PDM//s phase shift when inverted, in .01 degree resolution
    call DisplayButtonsForHalted   //SEW8 replaced print #main.restart, "Restart"
    message$=phase0;"  ";phase1 : call PrintMessage //ver114-4e
    beep  //ver111-36d
    //put PDM into the state at which it entered this subroutine
    phaarray(thisstep,0) = rememberpdmstate //ver112-2e
    gosub [CommandPDMonly] //ver112-2e
    return      //ver114-5k changed wait to return

    */
}
void MainWindow::ReadADCviaUSB()
{
     //  USB:15/08/10
    // the following code works fine but the structure version below is quicker
    //    // Generic code for USB ADC input regardless of number of bits and ADC type //USB:01-08-2010
//[Read22wSlimCBUSB] //USB:01-08-2010
//    USBwrbuf$ = "B201040A01"
//    goto [ReadCommonwSlimCBUSB]
//
//[Read16wSlimCBUSB] //USB:01-08-2010
//    USBwrbuf$ = "B200021001"
//    // fall through
//[ReadCommonwSlimCBUSB] //USB:01-08-2010
//    if USBdevice = 0 then return
//    UsbAdcCount = 0
//    UsbAdcResult1 = 0
//    UsbAdcResult2 = 0
//    CALLDLL #USB, "UsbMSADeviceReadAdcs", USBdevice as long, USBwrbuf$ as ptr, 5 as short, USBrBuf as struct, result as boolean
//    if( result ) then
//        UsbAdcCount = USBrBuf.numreads.struct
//        UsbAdcResult1 = USBrBuf.magnitude.struct
//        UsbAdcResult2 = USBrBuf.phase.struct
//    end if
//    return //to [ReadMagnitude]or[ReadPhase]with status words

// the main code has already set up the structure that defines the ADC conversion so just go for it !
//This is new common subroutine for reading the ADC//s with USB. //ver116-4r
//This will read the ADC//s and return the values for magdata and phadata
//    if USBdevice = 0 then return
//ver116-4r deleted    UsbAdcCount = 0  //since this is not used anywhere in the SW anyway
//ver116-4r deleted    UsbAdcResult1 = 0
 //ver116-4r deleted   UsbAdcResult2 = 0
  magdata = 0;
  phadata = 0;
  unsigned long result;
  USBrBuf buf;
  usb->usbMSADeviceReadAdcsStruct((unsigned short*)(&buf), &result);

  if( result )
  {
//    UsbAdcCount = USBrBuf.numreads.struct   //this is not used anywhere, but I will leave it here as a reference //ver116-4r
    magdata = buf.magnitude;
    phadata = buf.phase;
    //Since the 12 bit serial ADC is read as 16 bits, throw away the last 4 bits //ver116-4r
    if (activeConfig.adconv == 22) magdata = (int)(magdata/16);
    if (activeConfig.adconv == 22) phadata = (int)(phadata/16);
  }
  return; //to [ReadMagnitude]or[ReadPhase]with status words
}


void MainWindow::ProcessAndPrint()
{
//process and print "thisstep" //ver111-22
//SEW3 changed the next few lines to have phase degrees adjusted for phase-change-over-signal-level
//The calculation of the phase adjustment, difPhase, is made in ConvertMagPhaseData (formerly ConvertMagData).
//That correction is then added to phase in ConvertPhadata.Note that ConvertPhadata must now be
//called after ConvertMagPhaseData so difPhase is valid when ConvertPhadata is executed.
  ConvertMagPhaseData(); //convert magdata (bits read) to magpower (dBm)
  if (vars->msaMode!="SA")   //modver115-1e
  {
    //convert phadata (bits read) to phase (degrees) if we have phase, but not for special graphs, which set phase directly
    if (vars->msaMode!="ScalarTrans" && vars->doSpecialGraph==0)
      ConvertPhadata(); //ver116-4h
    ProcessDataArrays();   //Enter data in S21DataArray or ReflectArray
  }
  PlotDataToScreen();
  return; //from [ProcessAndPrint]

}
void MainWindow::ProcessDataArrays()
{
  //process "thisstep" data for VNA/SNA, filling S21DataArray and/or ReflectArray
  //ver115-8b separated this from ProcessAndPrint so it can be called separately.
  //Data is transferred from datatable, stored as necessary and calculations made.
  //For reflection mode; do jig calc and/or apply OSL calibration ver115-1b
  //But data as is if we are doing calibration. ver115-1e
  TransferToDataArrays();
  if (vars->msaMode=="VectorTrans" || vars->msaMode=="ScalarTrans")
  {
    if (vars->calInProgress==0 && vars->planeadj!=0)
    {
      int phaseToExtend=vars->S21DataArray[vars->thisstep][2];  //ver116-4s
      // fix me uExtendCalPlane(thisfreq, phaseToExtend, vars->planeadj,0);  //Do plane adjustment ver116-1b //ver116-4s
      vars->S21DataArray[vars->thisstep][2]=phaseToExtend;  //ver116-4s
    }
  }
  else
  {
    if (vars->msaMode=="Reflection")
    {
      ConvertRawDataToReflection(vars->thisstep);    //Apply calibration and calculate all reflection related data; apply OSL if necessary ver116-4n
    }
  }
}

void MainWindow::TransferToDataArrays()
{
  //Transfer datatable data to transmission or reflection array for thisstep
  float thisfreq=vars->datatable[vars->thisstep][1];  //freq
  int thisBand=vars->datatable[vars->thisstep][4];  //ver116-4s
  if (thisBand!=1)
    thisfreq=ActualSignalFrequency(thisfreq,thisBand); //actual signal frequency, not equivalent 1G freq ver116-4s
  float thisDB=vars->datatable[vars->thisstep][2];     //mag db
  float thisAng=vars->datatable[vars->thisstep][3];  //phase

  //We save data in S21DataArray for VectorTrans and ScalarTrans modes
  //We save data in ReflectArray for Reflection Mode.
  //Note that the actual signal frequency, not equivalent 1G freq, gets saved in these arrays
  //as part of the restart process, but we repeat that here in case we are called outside the normal
  //scanning process.
  //Note data in datatable has no adjustment for planeadj, but these other arrays do

  if (vars->msaMode=="VectorTrans" || vars->msaMode=="ScalarTrans")
  {
    vars->S21DataArray[vars->thisstep][0]=thisfreq;   //actual signal freq
    vars->S21DataArray[vars->thisstep][1]=thisDB;   //mag
    vars->S21DataArray[vars->thisstep][2]=thisAng;  //phase--may be changed by plane extension
    vars->S21DataArray[vars->thisstep][3]=thisAng;  //phase before plane extension ver116-1b
  }
  if (vars->msaMode=="Reflection")
  {
    for (int i=1; i <= 16; i++)
    {
      vars->ReflectArray[vars->thisstep][i]=0;
    } //Clear all reflection data (except freq) for this point
    vars->ReflectArray[vars->thisstep][0]=thisfreq;   //actual signal freq
    //The following may be changed by applying OSL
    vars->ReflectArray[vars->thisstep][vars->constGraphS11DB]=thisDB;   //Save raw data in array that will hold reflection related data
    vars->ReflectArray[vars->thisstep][vars->constGraphS11Ang]=thisAng;
    vars->ReflectArray[vars->thisstep][vars->constIntermedS11DB]=thisDB;    //ConvertRawDataToReflection may override these intermed values
    vars->ReflectArray[vars->thisstep][vars->constIntermedS11Ang]=thisAng;
  }
}

void MainWindow::CalcFreqCorrection()
{
  //Calculate power correction factors for each frequency step.
  for (int s=0; s <= vars->globalSteps; s++)
  {
    float currFreq=vars->baseFrequency+graph.gGetPointXVal(s+1);    //Point number is one more than step num  ver116-4k added baseFrequency
    vars->freqCorrection[s]=calMan.calConvertFreqError(currFreq); //Put power correction into the array
  }
}
void MainWindow::ConvertPhadata()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ConvertPhadata]//needed: phadata,PDM polarity,difPhase ; creates "phaseofpdm" and "thispointphase", the pixel value
//retrieve phadata from array
//convert phadata to phase, round off to .01 deg
//compensate phase using "invdeg" (if PDM was inverted during the phase reading)
//compensate phase using "difPhase", Phase Error Correction Factor//
//   as determined in Path Calibration, (variation of phase readings over signal level)
//if normal sweep, process phase by removing line calibration phase
//put final phase into datatable
//Do not do Reference Plane Extension here
//convert phase to +180 to -180 format and round off to .01 degree

        //grab raw phase data bits from array
    phadata = phaarray(thisstep,3) //ver111-19
        //convert phadata to absolute phase lead, referenced to 0 degrees. maxpdmout/4 = 90 degrees lead,maxpdmout/2 = 180 degrees lead
    phase = 360*phadata/maxpdmout  //converts phadata bits to absolute phase. ver15-2d moved rounding to later
      //the absolute phase will normally be between limits of +288 and +72 degrees,
      //however, it can be between 360 and 0 degrees if the PDM is "forced" into a set state (setpdm=1)
    //ver116-4h prevented getting here when doSpecialGraph>0, so we don't need to test it here
        //if PDM was inverted, subtract the inverted phase change
        //The deviation in the PDM inversion from theoretical 180 deg is actually a fixed time period
        //invdeg is maintained as the actual inversion that occurs at 10.7 MHz. With a different finalfreq,
        //the deviation from 180 degrees will change.
    if phaarray(thisstep,0)=1 then  //ver116-1b
        PDMinversionDeviation=(invdeg-180)*finalfreq/10.7
        phase = phase - (180+PDMinversionDeviation)
    end if
    phase=phase-difPhase    //SEW3: subtract correction for change of phase over signal level.
    thisBand=datatable(thisstep,4)  //ver116-4s
    if thisBand=3 then phase=0-phase    //For 3G mode, true phase is negative of measured phase, due to low side LO1 ver116-4L ver116-4s
    //calculate phase with calibration table factored in
    //ver114-5f Apply lineCalArray only if LineCal or BaseLineCal is active, and not when calibrating
        //Note if calInProgress=1, applyCalLevel will have been set to 0 by cal installation routine
    if applyCalLevel>0 then phase = phase - lineCalArray(thisstep,2) //subtract reference.
    if phase>=0 then phase=int(phase*100+0.5)/100 else phase=int(phase*100-0.5)/100 //round to two decimal places ver115-2d
//ver115-2b moved the application of planeadj. The data in datatable() never contains that adjustment.

    //The phase correction is set to 180 degrees when the phase reading during initial calibration
    //is suspect. In that case, we override all the foregoing and set the final phase to 0.
    //validPhaseThreshold indicates the lowest magnitude level at which phase is valid.
    if magdata<validPhaseThreshold then phase=0 //ver116-1b

    //convert to standard +180 -180 format
    while phase >180 : phase = phase - 360 : wend //ver114-5d fixed typo
    while phase <=-180 : phase = phase + 360 : wend
    //write the processed phase into the memory array, +180 to -180 deg
    datatable(thisstep,3) = phase    //put current phase measurement into the array, line value= thisstep
    //Note that phase may have to be adjusted during graphing to fit the bounds of the display
    return

*/
}
//--SEW Replaced [ConvertMagData] and [ConvertFreq] with following combined routine,
//to utilize the calibration module to interpolate the necessary correction factors
//to convert the raw ADC reading into dbm, and then to correct that number for variations
//over frequency. Phase correction is also calculated so the routine name was changed.
//That phase correction is difPhase and is subtracted from phase in ConvertPhadata
//The phase correction for signal level has to be calculated here, because it
//is a function of magnitude ADC reading and so can be interpolated at the same time
//as that ADC reading is converted to mag power. This messes up the terminology a bit
//because we now have ConvertPhadata and ConvertMagPhaseData. But it works.
void MainWindow::ConvertMagPhaseData()
{
  //convert magnitude data bits to MSA input power(in dBm) and to pixels. ver111-39a
  //needed: magarray,calibration table
  //this converts magdata to MSA input power, using
  //a Magnitude Error Correction Factor, (determined in Frequency Calibration)
  //If in VNA mode, it also finds the phase correction for the power level indicated
  //by magdata, and put it into difPhase, to be subtracted from phase later.

  float magdata, doPhaseCor, power, difPhase;
  int freqerror;
  if (vars->doSpecialGraph==0)
  {
    //Normal scan. Apply the calibration
    magdata = vars->magarray[vars->thisstep][3];
    //Apply mag calibration to get power and phase correction

    if (vars->msaMode!="SA" && vars->msaMode!="ScalarTrans") doPhaseCor=1; else doPhaseCor=0; //ver115-1a
    calMan.calConvertMagPhase(magdata, doPhaseCor, power, difPhase);    //ver114-5n
    //int thisfreq = vars->datatable[vars->thisstep][1];
    freqerror=vars->freqCorrection[vars->thisstep]; //find freq cal adjustment SEWgraph1
    //In SA mode, if there is an active front end file, we add the front end correction factor
    if (vars->msaMode=="SA")
    {
      if (vars->frontEndActiveFilePath!="") freqerror=freqerror-vars->frontEndCorrection[vars->thisstep];    //ver115-9d
    }
  }
  else
  {
    if (vars->calInProgress && vars->msaMode!="Reflection")  //ver116-4b
    {
      //If calibrating transmission, we want to use ideal results so when we display the actual doSpecialGraph
      //it will come out the way we want.
      power=0;
      int phase=0;
      vars->datatable[vars->thisstep][3] = phase;
    }
    else
    {
      DoSpecialGraph();
    }
  }
  //goto [CalcMagpowerPixel]
  //--SEW End of new routine to make calibration adjustments

  //[CalcMagpowerPixel]
  power = power + freqerror;
  // fix me if (convdatapr == 1) ConvertDataToPower(); //ver112-2b
  //round off MSA input power to .01 dBm, magpower, no matter which AtoD is used
  int magpower = power; //ver115-2d
  //Note if calInProgress=1, applyCalLevel will have been set to 0 by cal installation routine
  if (vnaCal.applyCalLevel>0)
  {
    if (vars->msaMode!="SA")
      magpower = magpower - vars->lineCalArray[vars->thisstep][1];
  }  //ver116-4n  subtract reference.
  if (magpower>=0)
    magpower=(int)(magpower*100000+0.5)/100000;
  else
    magpower=(int)(magpower*100000-0.5)/100000; //round to five decimal places ver115-4d
  vars->datatable[vars->thisstep][2] = magpower;    //put current power measurement into the array
  return; //to [ProcessAndPrint]

}

void MainWindow::CalcMagpowerPixel()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    power = power + freqerror
    if convdatapwr = 1 then gosub [ConvertDataToPower] 'ver112-2b
    'round off MSA input power to .01 dBm, magpower, no matter which AtoD is used
    magpower = power 'ver115-2d
        'Note if calInProgress=1, applyCalLevel will have been set to 0 by cal installation routine
    if applyCalLevel>0 then if (msaMode$<>"SA") then  magpower = magpower - lineCalArray(thisstep,1)  'ver116-4n  subtract reference.
    if magpower>=0 then magpower=int(magpower*100000+0.5)/100000 else magpower=int(magpower*100000-0.5)/100000 'round to five decimal places ver115-4d
    datatable(thisstep,2) = magpower    'put current power measurement into the array
    return 'to [ProcessAndPrint]
*/
}

void MainWindow::DoSpecialGraph()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
difPhase=0 : freqerror=0  'ver114-7e
    if doSpecialGraph=1 then
        'Graph mag calibration table. Find the min and max ADC values
        'and make magdata run linearly from the minimum to the maximum.
        'Any non-linearities in the graph then reflect the calibration
        'We do nothing with phase
        call calGetMagPoint 1,minADC, calMag, calPhase    'ignore calMag and calPhase
        call calGetMagPoint calNumMagPoints(),maxADC, calMag, calPhase
        testSlope=(maxADC-minADC)/steps
        magdata=minADC+testSlope*thisstep
        'Apply mag calibration to get power, but forget phase correction
        call calConvertMagPhase magdata, 0, power, dum
        'skip freq cal
        return
    end if
    if doSpecialGraph=2 then
        'Force power to 0 dbm and then find the frequency compensation.
        'The resulting graph will show the shape of the frequency compensation
        'curve.
        power=0
        thisfreq = datatable(thisstep,1)
        freqerror=freqCorrection(thisstep) 'find freq cal adjustment SEWgraph1
        return
    end if
    if doSpecialGraph=3 then
        'Generate random values for magdata superimposed on a sine wave centered midway in ADC
        'Make Transmission values a little higher.
        call calGetMagPoint 1,minADC, calMag, calPhase    'ignore calMag and calPhase
        call calGetMagPoint calNumMagPoints(),maxADC, calMag, calPhase
        magdata=3*(1+RND(1)/20)*(maxADC)/4 +  3000*sin(10*datatable(thisstep,1)) 'ver114-7b
        if msaMode$="VectorTrans" or msaMode$="ScalarTrans" then magdata=1.1*magdata    'Increase transmission values ver116-1b
        'Note: without being connected to MSA, which is when DoSpecialGraph is useful,
        'phase will bounce between 0 and 180 and thus will show some graph action.
        call calConvertMagPhase magdata, 1, power, difPhase 'ver114-5n
        thisfreq = datatable(thisstep,1)
        freqerror=freqCorrection(thisstep) 'find freq cal adjustment SEWgraph1
        phase=180-10*datatable(thisstep,1)  'linear change over frequency
        datatable(thisstep,3) = phase   'ver116-4h
        return
    end if
    if doSpecialGraph=4 then
        'Generate a peak near 1 MHz
        currXVal=gGetPointXVal(thisstep+1)-1
        'Upside down parabola centered  near MHz ver 114-3g
        power=max(-6-((3000+doSpecialRandom*2000)*(currXVal+0.025-doSpecialRandom/20)^2),-100)
        phase=270-300*datatable(thisstep,1) 'ver115-1b
        datatable(thisstep,3) = phase   'ver116-4h
        return
    end if
    if doSpecialGraph=5 then
            'doSpecialGraph=5
            'For SA mode, do response of a 1 MHz square wave.
            'For VNA modes, Calc response of an RLC circuit with optional transmission line 'ver114-7e
       if msaMode$="SA" then    'ver115-4c added the 1 MHz square wave for SA mode
            currXVal=gGetPointXVal(thisstep+1)
            specialWholeFreq=int(currXVal+0.5) : specialFractFreq=currXVal-specialWholeFreq 'fract may be -0.5 to +0.5
            specialNoise=(1e-11)*finalbw*(1+4*Rnd(0))
            if specialWholeFreq=2*int(specialWholeFreq/2) then
                power=10*uSafeLog10(specialNoise)
            else
                '1 MHz square wave at odd multiples of 1 MHz has power of 1/N mw, where N is the multiple.
                'It tapers off per a parabola, which is wider for higher RBW.
                power=10*uSafeLog10(specialNoise+(1/specialWholeFreq^2)*max(0,(1-(1400*specialFractFreq/finalbw)^2)))
            end if
            phase=0 : datatable(thisstep,3) = 0
        else    'VNA modes
            uWorkNumPoints=1 : uWorkArray(1,0)=gGetPointXVal(thisstep+1) 'ver115-1c  'set up for uRLCComboResponse
            uWorkArray(1,1)=0:uWorkArray(1,2)=0 'Default in case of error
            'Calc response in whatever S11 or S21 setup the user has chosen
            if msaMode$="Reflection" then
                doSpecialR0=S11BridgeR0 : doSpecialJig$="S11"   'ver115-4a
            else
                doSpecialR0=S21JigR0
                if S21JigAttach$="Series" then doSpecialJig$="S21Series" else doSpecialJig$="S21Shunt"
            end if
                'Note we only have one point in uWorkArray for uRLCComboResponse to process
                'Note calibration will not be applied for doSpecialGraph=5, so uRLCComboResponse
                'calculates the actual final response. e.g. S21JigShuntDelay is not taken into account, because
                'it would be removed by a perfect calibration.
            isErr=uRLCComboResponse(doSpecialRLCSpec$, doSpecialR0, doSpecialJig$)  'ver115-4a
            power=uWorkArray(1,1)   'get results of uRLCComboResponse
            phase=uWorkArray(1,2)
            datatable(thisstep,3) = phase
        end if
        return
    end if
    'doSpecialGraph=6  There is currently no such thing
    power=0 : phase=0 : datatable(thisstep,3) = phase
    return
*/
}
void MainWindow::ConvertRawDataToReflection(int currStep)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
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
  /*
  trueFreq=ReflectArray(currStep,0)*1000000
  db=ReflectArray(currStep,constGraphS11DB) : ang=ReflectArray(currStep,constGraphS11Ang)
  if calInProgress then   //If calibrating we don't adjust anything here, or calculate anything other than S11
      ReflectArray(currStep, constIntermedS11DB)=db  //ver115-2d
      ReflectArray(currStep, constIntermedS11Ang)=ang  //ver115-2d
      exit sub
  end if

  rho=uTenPower(db/20)    //mag made linear
      //db, rho, and ang (degrees) now have the raw reflection data
      //If necessary, we apply full OSL to the reflection data, whether it was derived
      //from a reflection bridge or a transmission jig.
      //If doing OSL cal, then we don't want to apply whatever coefficients we happen to have now.
      //If doSpecialGraph<>0 we don't want to mess with the internally generated data
  if doSpecialGraph=0 and applyCalLevel<>0 then   //ver115-5f
      rads=ang*uRadsPerDegree()   //angle in radians
      mR=rho*cos(rads) : mI=rho*sin(rads)     //measured S11, real and imaginary
      aR=OSLa(currStep,0) : aI=OSLa(currStep,1)   //coefficient a, real and imaginary
      bR=OSLb(currStep,0) : bI=OSLb(currStep,1)   //coefficient b, real and imaginary
      cR=OSLc(currStep,0) : cI=OSLc(currStep,1)   //coefficient c, real and imaginary

      //calculate adjusted db, ang via OSL. Note OSL must be referenced to S11BridgeR0
      calcMethod=1    //For debugging, we have two different methods
      if calcMethod=1 then
              //The first method uses  the following formula, and corresponds to CalcOSLCoeff
              //       S = (M ? b) / (ac*M)
              //where S is the actual reflection coefficient and M is the measured reflection coefficient.
              //S and M are in rectangular form in this equation.
          RealCM=cR*mR-cI*mI : ImagCM=cR*mI+cI*mR     //c*M, real and imaginary
          call cxDivide mR-bR, mI-bI, aR-RealCM,aI-ImagCM,refR, refI   //Divide M-b by a-c*M
      else
              //The second method uses  the following formula, and corresponds to CalcOSLCoeff1
              //       S = (a - cM)/(bM - 1)
              //where S is the actual reflection coefficient and M is the measured reflection coefficient.
              //S and M are in rectangular form in this equation.

          RealCM=cR*mR-cI*mI : ImagCM=cR*mI+cI*mR     //c*M, real and imaginary
          RealBM=bR*mR-bI*mI : ImagBM=bR*mI+bI*mR     //b*M, real and imaginary
          numR=aR-RealCM : numI=aI-ImagCM             //numerator, real and imaginary
          denR=RealBM-1 :denI=ImagBM                  //denominator, real and imaginary
          call cxDivide numR, numI, denR, denI, refR, refI     //Divide numerator by denominator; result is reflection coeff.
      end if
       separated the following common calculations from the above if...else block
      magSquared=refR^2+refI^2        //mag of S, squared
      db=10*uSafeLog10(magSquared)    //S mag in db; multiply by 10 not 20 because mag is squared
      if db>0 then db=0   //Shouldn//t happen
      ang=uATan2(refR, refI)      //angle of S in degrees
          //db, ang (degrees) now have S11 data produced by applying OSL calibration.
  end if

  //Save the angle prior to applying plane extension or Z0 transform, to make it easier to recalculate with a new values
  ReflectArray(currStep, constIntermedS11DB)=db  //ver115-2d
  ReflectArray(currStep, constIntermedS11Ang)=ang  //ver115-2d
      //Note we do apply plane extension even when doSpecialGraph<>0
  if planeadj<>0 or S11BridgeR0<>S11GraphR0 then call ApplyExtensionAndTransformR0 ReflectArray(currStep,0), db, ang //ver115-2d

      //Note we do not put the reflection data in datatable, which retains the original raw data
  ReflectArray(currStep,constGraphS11DB)=db   //Save final S11 in db, angle format (in Graph R0, after plane ext)
  while ang>180 : ang=ang-360 : wend
  while ang<=-180 : ang=ang+360 : wend
  ReflectArray(currStep,constGraphS11Ang)=ang
  //We now compute the various items in ReflectArray() from S11, but if we are doing calibration we don't need this
  //other data, and it probably doesn//t make sense anyway.
  if calInProgress=0 then call CalcReflectDerivedData currStep  //Calc other ReflectArray() data from S11.
  */
}

void MainWindow::ApplyExtensionAndTransformR0(float freq, float &db, float &ang)
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
void MainWindow::PlotDataToScreen()
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


  graph.CalcGraphData(vars->thisstep, graph.thispointy1, graph.thispointy2, 0);   //0 means use regular data arrays ver114-7f
  if (graph.referenceDoMath==2) //ref math is to be done on graph values ver115-5d
  {
    if (graph.referenceTrace & 1)
      graph.thispointy1=graph.referenceOpA*graph.referenceTransform[vars->thisstep+1][1]+graph.referenceOpB*graph.thispointy1;
    if (graph.referenceTrace & 2)
      graph.thispointy2=graph.referenceOpA*graph.referenceTransform[vars->thisstep+1][2]+graph.referenceOpB*graph.thispointy2;
  }
  //We draw a point after erasing a point (if required).
  //We also saves the data to gGraphVal() and the pixel values to gGraphPix()
  //X values have been precalculated in gGenerateXValues
  if (vars->thisstep==vars->sweepStartStep)
  {
    //ver114-6e added the refreshXXX flags
    graph.refreshForceRefresh=0;
    //We normally want refreshGridDirty=0 because we redrew the grid when restarting. But
    //for mulitscans, that step is skipped.
    graph.refreshGridDirty=vars->multiscanInProgress;
    graph.refreshTracesDirty=0;
    graph.refreshAutoScale=0;
    graph.refreshRedrawFromScratch=0;
    if (graph.firstScan)
    {
      graph.refreshMarkersDirty=1;   //Signals to relocate any markers to correspond to their frequency.
      //If required, do auto scaling of axes at end of first scan
      if (graph.autoScaleY2 || graph.autoScaleY1)
        graph.refreshAutoScale=1; //ver115-3b
    }
    else
    {
      graph.gStartNextDynamicScan();    //Initializes for erase/draw passes; done at start of scans 2...
    }

    //ver114-5c moved setting of isStickMode to UpdateGraphParams
  }

  //useExpeditedDraw is set to 1 for single trace (mag) in non-histo mode for linear y scaling
  //The expedited draw procedure is slightly faster than normal drawing, and will almost always be
  //used when in SA mode.
  int useExpeditedDraw = graph.gCanUseExpeditedDraw();
  if (useExpeditedDraw)
  {
    graph.gDrawSingleTrace();    //Normal SA drawing occurs with this
  }
  else
  {
    //Enter new Y values and draw from last point
    //Comment out all but one; two choices allowed for testing
    //call gDynamicDrawPoint thispointy1,thispointy2
    graph.gDynamicComboDrawPoint(graph.thispointy1,graph.thispointy2);     //This is the full-blown drawing procedure
  }
  if (vars->steps>=1000)  //ver114-4k
  {
    //Discard at least every 1000 points to avoid a slowdown.
    //oneThousandthThisStep=vars->thisstep/1000;
    //if (int(oneThousandthThisStep)==oneThousandthThisStep) then #graphBox$, "discard"
  }
  //ver114-4e deleted drawing of point values at start, center and end
  if (vars->thisstep==vars->sweepEndStep) //just processed final point of a sweep //ver114-4k
  {
    //If autoscale is on for either axis then calculate the scale and redraw from raw values
    //We only do this for the first scan. ver114-7a added this autoscale material
    if (graph.firstScan && (graph.autoScaleY2 || graph.autoScaleY1))   //ver115-3b
    {
      if (graph.haltAtEnd==1) //Halt will redraw, so we just autoscale here
      {
        graph.PerformAutoScale();   //Recalculates scaling in graph module
      }
      else
      {
        graph.refreshForceRefresh=1;
        graph.refreshAutoScale=1;
      }
    }
    if (graph.refreshForceRefresh || graph.refreshEachScan)
    {
      //force full refresh unless we are in stick mode; and don't redraw if haltAtEnd=1 because
      //a redraw will occur when we halt.
      if (vars->isStickMode==0 && graph.haltAtEnd==0) graph.RefreshGraph(1);
    }
    else
    {//modver116-4b
      //Even if we don't do a full refresh, we refresh the grid lines and traces except on first scan.
      //This restores any grid lines that got erased
      //If we draw markers, their movement from scan to scan can make a mess.
      //If we don't draw them, the grid lines will cross them.
      //So we skip this altogether if we have markers; we could do it if we had a way to erase markers.
      int partialRefresh;
      if (graph.firstScan==0 && vars->isStickMode==0 && graph.haltAtEnd==0)
      {
        if (graph.doGraphMarkers)
          graph.gEraseMarkers();   //Erase prior markers before they move
        graph.gRefreshGridLinesOnly();
        graph.gRefreshTraces();
        partialRefresh=1;
      }
      else
      {
        partialRefresh=0;
      }
      if (graph.firstScan==1)
        graph.gDetermineMarkerPointNumbers(); //Finds new point numbers for old markers if freq changed
      //Update marker info at end of every scan no matter what
      if (graph.hasAnyMark==1)
        graph.mDrawMarkerInfo(); //also updates marker locations

      //If no refresh, draw markers on first scan only, and only if user wants them drawn
      if ((graph.firstScan==1 || partialRefresh) && graph.doGraphMarkers==1)
        graph.gDrawMarkers(); //ver116-4b
      //#graphBox$, "discard"    //Get rid of marker draw commands in memory
    }
    if ((vars->doCycleTraceColors==1) && (vars->isStickMode==1))  //ver116-4s  cycleTrace colors if necessary
    {
      vars->cycleNumber=vars->cycleNumber+1;
      if (vars->cycleNumber>3)
        vars->cycleNumber=1;
      gridappearance.gSetTraceColors(gridappearance.cycleColorsAxis1[vars->cycleNumber], gridappearance.cycleColorsAxis2[vars->cycleNumber]);  //ver116-4s
    }
    graph.firstScan=0;  //scan has ended; next point is not in first scan since restart
    vars->doSpecialRandom=(rand() % 100) / 100;  //Random number for doSpecialGraph ver115-1b
  }

  if (vars->varwindow == 1)
    updatevar(); //moved here from [ProcessAndPrint] ver111-34a
}
void MainWindow::CreateRcounter()
{
  //needed:reference,appxpdf ; creates:rcounter,pdf //ver111-4
  rcounter = (int)(vars->reference/appxpdf); //ver111-4
  if ((vars->reference/appxpdf) - rcounter >= .5) rcounter = rcounter + 1;   //rounds off rcounter //ver111-4
  pdf = vars->reference/rcounter; //ver111-4
  //to (Initialize PLL 3),[InitializePLL2],or[InitializePLL1]with rcounter,pdf //ver111-4
}
void MainWindow::CommandPLL1R()
{
  //needed:rcounter1,PLL1mode,PLL1phasepolarity,SELT,PLL1
  rcounter = rcounter1;
  preselector = 32; if (activeConfig.PLL1mode == 1) preselector = 16;
  phasepolarity = activeConfig.PLL1phasepolarity;    //inverting op amp is 0, non-inverting loop is 1
  fractional = activeConfig.PLL1mode;       //0 for Integer-N; 1 for Fractional-N
  Jcontrol = SELT;   //for PLL 1, on Control Board J1, the value is "3"
  LEPLL = 4;         //for PLL 1, on Control Board J1, the value is "4"
  vars->PLL = activeConfig.PLL1;
  CommandRBuffer();//needs:rcounter,preselector,phasepolarity,fractional,Jcontrol,LEPLL,PLL
  if (errora.length()>0)
  {
    error = "PLL 1, " + errora;
    vars->message=error; graph.PrintMessage(); //ver114-4e
    RequireRestart();   //ver115-1c
    //wait
  }
  return;
}

void MainWindow::CommandPLL2R()
{
  //needed:reference,appxpdf,PLL2phasepolarity,SELT,PLL2
  preselector = 32;
  phasepolarity = activeConfig.PLL2phasepolarity;    //inverting op amp is 0, non-inverting loop is 1
  fractional = 0;    //0 for Integer-N; PLL 2 should not be fractional due to increased noise
  Jcontrol = SELT;   //for PLL 2, on Control Board J2, the value is "3"
  LEPLL = 8;          //for PLL 2, on Control Board J2, the value is "8"
  vars->PLL = activeConfig.PLL2;
  CommandRBuffer();//needs:rcounter,preselector,phasepolarity,fractional,Jcontrol,LEPLL,PLL
  if (errora.length()>0)
  {
    error = "PLL 2, " + errora;
    vars->message=error; graph.PrintMessage(); //ver114-4e
    RequireRestart();   //ver115-1c
   // wait
  }
  return; //to //CommandPLL2R and Init Buffers
}

void MainWindow::CommandPLL3R()
{
  //needed:PLL3mode,PLL3phasepolarity,INIT,PLL3
  preselector = 32; if (activeConfig.PLL3mode == 1) preselector = 16;
  phasepolarity = activeConfig.PLL3phasepolarity;    //inverting op amp is 0, non-inverting loop is 1
  fractional = activeConfig.PLL3mode;       //0 for Integer-N; 1 for Fractional-N
  Jcontrol = INIT;   //for Tracking Gen PLL, on Control Board J3, the value is "15"
  LEPLL = 16;         //for Tracking Gen PLL, on Control Board J3, the value is "16"
  vars->PLL = activeConfig.PLL3;
  CommandRBuffer();//needs:rcounter,preselector,phasepolarity,fractional,Jcontrol,LEPLL,PLL
  if (errora.length()>0)
  {
    error = "PLL 3, " + errora;
    vars->message=error; graph.PrintMessage(); //ver114-4e
    RequireRestart();   //ver115-1c
   // wait
  }
  return; //to //CommandPLL3R and Init Buffers
}
void MainWindow::CommandRBuffer()
{
  //needed:rcounter,preselector,phasepolarity,fractional,Jcontrol,LEPLL,PLL
  if (vars->PLL == 2325) Command2325R();//needs:rcounter,preselector,Jcontrol,port,LEPLL,contclear ; commands LMX2325 rcounter and registers
  if (vars->PLL == 2326) Command2326R();//needs:rcounter,phasepolarity,Jcontrol,port,LEPLL,contclear ; commands LMX2326 rcounter and registers
  if (vars->PLL == 2350) Command2350R();//needs:rcounter,phasepolarity,Jcontrol,port,LEPLL,contclear,fractional ; commands LMX2350 rcounter
  if (vars->PLL == 2353) Command2353R();//needs:rcounter,phasepolarity,Jcontrol,port,LEPLL,contclear,fractional ; commands LMX2353 rcounter
  if (vars->PLL == 4112) Command4112R();//needs:rcounter,preselector,phasepolarity,Jcontrol,port,LEPLL,contclear ; commands AD4112 rcounter
  return;
}
void MainWindow::CreateIntegerNcounter()
{

  //needed:appxVCO,reference,rcounter ; creates:ncount,ncounter,fcounter(0),pdf
  int ncount = appxVCO/(vars->reference/rcounter);  //approximates the Ncounter for PLL
  ncounter = int(ncount);     //approximates the ncounter for PLL
  if (ncount - ncounter >= .5) ncounter = ncounter + 1;   //rounds off ncounter
  fcounter = 0;
  pdf = appxVCO/ncounter;        //actual phase freq of PLL
  return;  //to //CreatePLL2N,//[CalculateThisStepPLL1],or //[CalculateThisStepPLL3] with ncount, ncounter and fcounter(=0)

}

void MainWindow::CreateFractionalNcounter()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CreateFractionalNcounter]'needed:appxVCO,reference,rcounter ; creates:ncount,ncounter,fcounter,pdf
    ncount = appxVCO/(reference/rcounter)  'approximates the Ncounter for PLL
    ncounter = int(ncount)    'actual value for PLL Ncounter
    fcount = ncount - ncounter
    fcounter = int(fcount*16) 'ver111
    if (fcount*16) - fcounter >= .5 then fcounter = fcounter + 1 'rounds off fcounter  ver111
    if fcounter = 16 then ncounter = ncounter + 1:fcounter = 0
    pdf = appxVCO/(ncounter + (fcounter/16)) 'actual phase freq for PLL 'ver111-10
    return  'with ncount,ncounter,fcounter,pdf

*/
}

void MainWindow::AutoSpur()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[AutoSpur]'needed:LO1,LO2,finalfreq,appxdds1,dds1output,rcounter1,finalbw,fcounter,ncounter,spurcheck;changes pdf,dds1output
    '[AutoSpur] is a continuation of [CreateFractionalNcounter], used only in MSA when PLL 1 is Fractional
    spur = 0    'reset spur, and determine if there is potential for a spur
    firstif = LO2 - finalfreq
    fractionalfreq = dds1output/(rcounter1*16)
    harnonicb = int(firstif/fractionalfreq)
    if (firstif/fractionalfreq)-harnonicb >=.5 then harnonicb = harnonicb + 1  'rev108
    harnonica = harnonicb - 1
    harnonicc = harnonicb + 1
    firstiflow = LO2 - (finalfreq + finalbw/1000)
    firstifhigh = LO2 - (finalfreq - finalbw/1000)
    if harnonica*fractionalfreq > firstiflow and harnonica*fractionalfreq < firstifhigh then spur = 1
    if harnonicb*fractionalfreq > firstiflow and harnonicb*fractionalfreq < firstifhigh then spur = 1
    if harnonicc*fractionalfreq > firstiflow and harnonicc*fractionalfreq < firstifhigh then spur = 1
    if spur = 1 and (dds1output<appxdds1) then fcounter = fcounter - 1
    if spur = 1 and (dds1output>appxdds1) then fcounter = fcounter + 1
    if fcounter = 16 then ncounter = ncounter + 1:fcounter = 0  'rev108
    if fcounter <0 then ncounter = ncounter - 1:fcounter = 15  'rev108
    pdf = LO1/(ncounter + (fcounter/16))
    dds1output = pdf * rcounter1    'actual output of DDS1(input Ref to PLL1)
    return 'with possibly new ncounter,fcounter,pdf,dds1output
    */
}

void MainWindow::ManSpur()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ManSpur]'needed:spurcheck,dds1output,appxdds1,fcounter,ncounter
    '[ManSpur] is a continuation of [CreateFractionalNcounter], used only in MSA when PLL 1 is Fractional
    if spurcheck = 1 and (dds1output<appxdds1) then fcounter = fcounter - 1 'causes +shift in pdf1
    if spurcheck = 1 and (dds1output>appxdds1) then fcounter = fcounter + 1 'causes -shift in pdf1
    if fcounter = 16 then ncounter = ncounter + 1:fcounter = 0  'rev108
    if fcounter < 0 then ncounter = ncounter - 1:fcounter = 15  'rev108
    pdf = LO1/(ncounter + (fcounter/16))
    dds1output = pdf * rcounter1    'actual output of DDS1(input Ref to PLL1)
    return 'with possibly new:ncounter,fcounter,pdf,dds1output
    */
}

void MainWindow::CreatePLL1N()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CreatePLL1N]'needed:ncounter,fcounter,PLL1mode,PLL1
    preselector = 32 : if PLL1mode = 1 then preselector = 16
    PLL = PLL1
    gosub [CreateNBuffer]'needs:ncounter,fcounter,PLL,preselector;creates:Bcounter,Acounter, and N Bits N0-Nx
    if len(errora$)>0 then
        error$ = "PLL 1, " + errora$
        message$=error$ : call PrintMessage 'ver114-4e
        call RequireRestart   'ver115-1c
        wait
    end if
    Bcounter1=Bcounter: Acounter1=Acounter
    return 'returns with Bcounter1,Acounter1,N0thruNx
    */
}
void MainWindow::CreatePLL2N()
{
  //needed:ncounter,fcounter,PLL2
  preselector = 32;
  vars->PLL = activeConfig.PLL2;
  CreateNBuffer();//needs:ncounter,fcounter,PLL,preselector;creates:Bcounter,Acounter, and N Bits N0-Nx
  if (errora.length()>0)
  {
    error = "PLL 2, " + errora;
    vars->message=error; graph.PrintMessage(); //ver114-4e
    RequireRestart();   //ver115-1c
    //wait
  }
  return; //to //CreatePLL2N
}

void MainWindow::CreatePLL3N()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CreatePLL3N]'needed:ncounter,fcounter,PLL3mode,PLL3  ver111-14
    preselector = 32 : if PLL3mode = 1 then preselector = 16
    PLL = PLL3
    gosub [CreateNBuffer]'needs:ncounter,fcounter,PLL,preselector;creates:Bcounter,Acounter, and N Bits N0-Nx
    if len(errora$)>0 then
        error$ = "PLL 3, " + errora$
        message$=error$ : call PrintMessage 'ver114-4e
        call RequireRestart   'ver115-1c
        wait
    end if
    Bcounter3=Bcounter: Acounter3=Acounter
    return 'returns with Bcounter3,Acounter3,N0thruNx
*/
}


void MainWindow::CreateNBuffer()
{
  //needed:PLL,ncounter,fcounter,preselector
  if (vars->PLL == 2325) Create2325N();//needs:ncounter,preselector; creates LMX2325 N Buffer ver111
  if (vars->PLL == 2326) Create2326N();//needs:ncounter ; creates LMX2326 N Buffer ver111
  if (vars->PLL == 2350) Create2350N();//needs:ncounter,preselector,fcounter; creates LMX2350 RFN Buffer ver111
  if (vars->PLL == 2353) Create2353N();//needs: ncounter,preselector,fcounter; creates LMX2353 N Buffer ver111
  if (vars->PLL == 4112) Create4112N();//needs:ncounter,preselector; creates AD4112 N Buffer ver111
  return; //with Bcounter,Acounter, and N Bits N0-N23
}

void MainWindow::Create2325N()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed:ncounter,preselector; creates LMX2325 n buffer
    Bcounter = int(ncounter/preselector)
    Acounter = ncounter- (Bcounter * preselector)
    if Bcounter<3 then beep:errora$ = "2325 Bcounter < 3":return 'with errora$ ver111-37c
    if Bcounter>2047 then beep:errora$ = "2325 Bcounter > 2047":return 'with errora$ ver111-37c
    if Bcounter<Acounter then beep:errora$ = "2325 Bcounter<Acounter":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0 = 0    'address bit, 0 sets the N Buffer, 1 is for R Buffer
    na1 = int(Acounter/2):N1 = Acounter - 2*na1 'binary conversion from decimal
    na2 = int(na1/2):N2 = na1 - 2*na2
    na3 = int(na2/2):N3 = na2 - 2*na3
    na4 = int(na3/2):N4 = na3 - 2*na4
    na5 = int(na4/2):N5 = na4 - 2*na5
    na6 = int(na5/2):N6 = na5 - 2*na6
    na7 = int(na6/2):N7 = na6 - 2*na7
    nb8 = int(Bcounter/2):N8 = Bcounter - 2*nb8
    nb9 = int(nb8/2):N9 = nb8 - 2*nb9
    nb10 = int(nb9/2):N10 = nb9 - 2*nb10
    nb11 = int(nb10/2):N11 = nb10 - 2*nb11
    nb12 = int(nb11/2):N12 = nb11 - 2*nb12
    nb13 = int(nb12/2):N13 = nb12 - 2*nb13
    nb14 = int(nb13/2):N14 = nb13 - 2*nb14
    nb15 = int(nb14/2):N15 = nb14 - 2*nb15
    nb16 = int(nb15/2):N16 = nb15 - 2*nb16
    nb17 = int(nb16/2):N17 = nb16 - 2*nb17
    nb18 = int(nb17/2):N18 = nb17 - 2*nb18
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
    return
    */
}

void MainWindow::Create2326N()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed:ncounter ; creates LMX2326 n buffer  ver111
    Bcounter = int(ncounter/32)
    Acounter = int(ncounter-(Bcounter*32))
    if Bcounter < 3 then beep:errora$="2326 Bcounter <3":return 'with errora$ ver111-37c
    if Bcounter > 8191 then beep:errora$="2326 Bcounter >8191":return 'with errora$ ver111-37c
    if Bcounter < Acounter then beep:errora$="2326 Bcounter<Acounter":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0 = 1       'n address bit 0, must be 1
    N1 = 0       'n address bit 1, must be 0
    na0 = int(Acounter/2):N2 = Acounter- 2*na0      'Acounter bit 0 LSB
    na1 = int(na0/2):N3 = na0 - 2*na1
    na2 = int(na1/2):N4 = na1 - 2*na2
    na3 = int(na2/2):N5 = na2 - 2*na3
    na4 = int(na3/2):N6 = na3 - 2*na4               'Acounter bit 4 MSB
    nb0 = int(Bcounter/2):N7 = Bcounter- 2*nb0      'Bcounter bit 0 LSB
    nb1 = int(nb0/2):N8 = nb0 - 2*nb1
    nb2 = int(nb1/2):N9 = nb1 - 2*nb2
    nb3 = int(nb2/2):N10 = nb2 - 2*nb3
    nb4 = int(nb3/2):N11 = nb3 - 2*nb4
    nb5 = int(nb4/2):N12 = nb4 - 2*nb5
    nb6 = int(nb5/2):N13 = nb5 - 2*nb6
    nb7 = int(nb6/2):N14 = nb6 - 2*nb7
    nb8 = int(nb7/2):N15 = nb7 - 2*nb8
    nb9 = int(nb8/2):N16 = nb8 - 2*nb9
    nb10 = int(nb9/2):N17 = nb9 - 2*nb10
    nb11 = int(nb10/2):N18 = nb10 - 2*nb11
    nb12 = int(nb11/2):N19 = nb11 - 2*nb12          'Bcounter bit 12 MSB
    N20 = 1    'Phase Det Current, 1= 1 ma, 0= 250 ua
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
    return
*/
}

void MainWindow::Create2350N()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed: ncounter,preselector,fcounter; creates LMX2350 RFN Buffer
    Bcounter = int(ncounter/preselector)
    Acounter = int(ncounter-(Bcounter*preselector))
    if Bcounter < 3 then beep:errora$="2350 Bcounter <3":return 'with errora$ ver111-37c
    if Bcounter > 1023 then beep:errora$="2350 Bcounter >1023":return 'with errora$ ver111-37c
    if Bcounter < Acounter + 2 then beep:errora$="2350 Bcounter<Acounter+2":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0=1      '2350 RF_N register, must be 1
    N1=1      '2350 RF_N register, must be 1 'was N=1 ver113-7a
    f0 = int(fcounter/2):N2 = fcounter - 2*f0      'fcounter bit 0
    f1 = int(f0/2):N3 = f0 - 2*f1       'fcounter bit 1
    f2 = int(f1/2):N4 = f1 - 2*f2       'fcounter bit 2
    f3 = int(f2/2):N5 = f2 - 2*f3       'fcounter bit 3 (0 to 15)
    rfna6 = int(Acounter/2):N6 = Acounter- 2*rfna6
    rfna7 = int(rfna6/2):N7 = rfna6 - 2*rfna7
    rfna8 = int(rfna7/2):N8 = rfna7 - 2*rfna8
    rfna9 = int(rfna8/2):N9 = rfna8 - 2*rfna9
    rfna10 = int(rfna9/2):N10 = rfna9 - 2*rfna10
    rfnb11 = int(Bcounter/2):N11 = Bcounter- 2*rfnb11
    rfnb12 = int(rfnb11/2):N12 = rfnb11 - 2*rfnb12
    rfnb13 = int(rfnb12/2):N13 = rfnb12 - 2*rfnb13
    rfnb14 = int(rfnb13/2):N14 = rfnb13 - 2*rfnb14
    rfnb15 = int(rfnb14/2):N15 = rfnb14 - 2*rfnb15
    rfnb16 = int(rfnb15/2):N16 = rfnb15 - 2*rfnb16
    rfnb17 = int(rfnb16/2):N17 = rfnb16 - 2*rfnb17  'was rgb17 ver113-7a
    rfnb18 = int(rfnb17/2):N18 = rfnb17 - 2*rfnb18
    rfnb19 = int(rfnb18/2):N19 = rfnb18 - 2*rfnb19
    rfnb20 = int(rfnb19/2):N20 = rfnb19 - 2*rfnb20
    N21=0 :if preselector = 32 then N21 = 1  '0=16/17  1=32/33
    N22=0     'Pwr down RF,    0=normal  1=pwr down
    N23=0     'RF cntr reset,  0=normal  1=reset
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
    return
    */
}

void MainWindow::Create2353N()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  'needed: ncounter,preselector,fcounter; creates LMX2353 N Buffer
      Bcounter = int(ncounter/preselector)
      Acounter = int(ncounter-(Bcounter*preselector))
      if Bcounter < 3 then beep:errora$ = "2353 Bcounter is < 3":return 'with errora$ ver111-37c
      if Bcounter > 1023 then beep:errora$ = "2353 Bcounter is > 1023":return 'with errora$ ver111-37c
      if Bcounter < Acounter + 2 then beep:errora$ = "2353 Bcounter < Acounter+2":return 'with errora$ ver111-37c
      'ver116-4o deleted "if" block, per Lrev1
      N0 = 1       'n address bit 0
      N1 = 1       'n address bit 1
      f0 = int(fcounter/2):N2 = fcounter - 2*f0       'fcounter bit 0
      f1 = int(f0/2):N3 = f0 - 2*f1       'fcounter bit 1
      f2 = int(f1/2):N4 = f1 - 2*f2       'fcounter bit 2
      f3 = int(f2/2):N5 = f2 - 2*f3       'fcounter bit 3 (0 to 15)
      na0 = int(Acounter/2):N6 = Acounter- 2*na0      'Acounter bit 0 LSB
      na1 = int(na0/2):N7 = na0 - 2*na1
      na2 = int(na1/2):N8 = na1 - 2*na2
      na3 = int(na2/2):N9 = na2 - 2*na3
      na4 = int(na3/2):N10 = na3 - 2*na4      'Acounter bit 4 MSB
      nb0 = int(Bcounter/2):N11 = Bcounter- 2*nb0      'Bcounter bit 0 LSB
      nb1 = int(nb0/2):N12 = nb0 - 2*nb1
      nb2 = int(nb1/2):N13 = nb1 - 2*nb2
      nb3 = int(nb2/2):N14 = nb2 - 2*nb3
      nb4 = int(nb3/2):N15 = nb3 - 2*nb4
      nb5 = int(nb4/2):N16 = nb4 - 2*nb5
      nb6 = int(nb5/2):N17 = nb5 - 2*nb6
      nb7 = int(nb6/2):N18 = nb6 - 2*nb7
      nb8 = int(nb7/2):N19 = nb7 - 2*nb8
      nb9 = int(nb8/2):N20 = nb8 - 2*nb9      'Bcounter bit 9 MSB
      N21 = 0 :if preselector = 32 then N21 = 1  '0=16/17  1=32/33
      N22 = 0          'power down if 1
      N23 = 0          'counter reset if 1
      if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
              2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
              2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
      if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
      return
          */
}

void MainWindow::Create4112N()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed: ncounter,preselector; creates AD4112 N Buffer
    Bcounter = int(ncounter/preselector)
    Acounter = int(ncounter-(Bcounter*preselector))
    if Bcounter < 3 then beep:errora$="4112 N counter <3":return 'with errora$ ver111-37c
    if Bcounter > 8191 then beep:errora$="4112 N counter >8191":return 'with errora$ ver111-37c
    if Bcounter < Acounter then beep:errora$="4112 B counter<Acounter":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0 = 1       'N address bit 0, must be 1
    N1 = 0       'N address bit 1, must be 0
    na0 = int(Acounter/2):N2 = Acounter- 2*na0      'Acounter bit 0 LSB
    na1 = int(na0/2):N3 = na0 - 2*na1
    na2 = int(na1/2):N4 = na1 - 2*na2
    na3 = int(na2/2):N5 = na2 - 2*na3
    na4 = int(na3/2):N6 = na3 - 2*na4
    na5 = int(na4/2):N7 = na4 - 2*na5      'Acounter bit 5 MSB
    nb0 = int(Bcounter/2):N8 = Bcounter- 2*nb0      'Bcounter bit 0 LSB
    nb1 = int(nb0/2):N9 = nb0 - 2*nb1
    nb2 = int(nb1/2):N10 = nb1 - 2*nb2
    nb3 = int(nb2/2):N11 = nb2 - 2*nb3
    nb4 = int(nb3/2):N12 = nb3 - 2*nb4
    nb5 = int(nb4/2):N13 = nb4 - 2*nb5
    nb6 = int(nb5/2):N14 = nb5 - 2*nb6
    nb7 = int(nb6/2):N15 = nb6 - 2*nb7
    nb8 = int(nb7/2):N16 = nb7 - 2*nb8
    nb9 = int(nb8/2):N17 = nb8 - 2*nb9
    nb10 = int(nb9/2):N18 = nb9 - 2*nb10
    nb11 = int(nb10/2):N19 = nb10 - 2*nb11
    nb12 = int(nb11/2):N20 = nb11 - 2*nb12      'Bcounter bit 12 MSB
    N21 = 0    '0=ChargePump setting 1, 1=setting 2
    N22 = 0     'reserved
    N23 = 0     'reserved
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
    return
    */
}

void MainWindow::CreateBaseForDDSarray()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CreateBaseForDDSarray]'needed:ddsoutput,ddsclock ; creates: base,sw0thrusw30,w0thruw4
    'the formula for the frequency output of the DDS(AD9850, 9851, or any 32 bit DDS) is:
    'ddsoutput = base*ddsclock/2^32, where "base" is the decimal equivalent of command words
    'to find "base": first, use: fullbase = (ddsoutput*2^32/ddsclock)
        fullbase=(ddsoutput*2^32/ddsclock) 'decimal number, including fraction
    'then, round it off to the nearest whole bit
            '(the following has a problem) 11-03-08
            'if ddsoutput is greater than ddsclock/2, the program will error out. I don't know why but
                'halt and create an error message
    if ddsoutput >= ddsclock/2 then
        beep:message$="Error, ddsoutput > .5 ddsclock" : call PrintMessage :goto [Halted] 'ver114-4e
    end if
        base = int(fullbase) 'rounded down to whole number
        if fullbase - base >= .5 then base = base + 1 'rounded to nearest whole number
    'now, the actual ddsoutput can be determined by: ddsoutput = base*ddsclock/2^32
  'Create Parallel Words 'needed:base
        w0= 0 'a "1" here will activate the x4 internal multiplier, but not recommended
        w1= int(base/2^24)  'w1 thru w4 converts decimal base code to 4 words, each are 8 bit binary
        w2= int((base-(w1*2^24))/2^16)
        w3= int((base-(w1*2^24)-(w2*2^16))/2^8)
        w4= int(base-(w1*2^24)-(w2*2^16)-(w3*2^8))
    if cb = 3 then 'USB:05/12/2010
        Int64SW.msLong.struct = 0 'USB:05/12/2010
        Int64SW.lsLong.struct = int( base ) 'USB:05/12/2010
    else 'USB:05/12/2010
        'Create Serial Bits'needed:base ; creates serial word bits; sw0 thru sw39
        b0 = int(base/2):sw0 = base - 2*b0  'LSB, Freq-b0.  sw is serial word bit
        b1 = int(b0/2):sw1 = b0 - 2*b1:b2 = int(b1/2):sw2 = b1 - 2*b2
        b3 = int(b2/2):sw3 = b2 - 2*b3:b4 = int(b3/2):sw4 = b3 - 2*b4
        b5 = int(b4/2):sw5 = b4 - 2*b5:b6 = int(b5/2):sw6 = b5 - 2*b6
        b7 = int(b6/2):sw7 = b6 - 2*b7:b8 = int(b7/2):sw8 = b7 - 2*b8
        b9 = int(b8/2):sw9 = b8 - 2*b9:b10 = int(b9/2):sw10 = b9 - 2*b10
        b11 = int(b10/2):sw11 = b10 - 2*b11:b12 = int(b11/2):sw12 = b11 - 2*b12
        b13 = int(b12/2):sw13 = b12 - 2*b13:b14 = int(b13/2):sw14 = b13 - 2*b14
        b15 = int(b14/2):sw15 = b14 - 2*b15:b16 = int(b15/2):sw16 = b15 - 2*b16
        b17 = int(b16/2):sw17 = b16 - 2*b17:b18 = int(b17/2):sw18 = b17 - 2*b18
        b19 = int(b18/2):sw19 = b18 - 2*b19:b20 = int(b19/2):sw20 = b19 - 2*b20
        b21 = int(b20/2):sw21 = b20 - 2*b21:b22 = int(b21/2):sw22 = b21 - 2*b22
        b23 = int(b22/2):sw23 = b22 - 2*b23:b24 = int(b23/2):sw24 = b23 - 2*b24
        b25 = int(b24/2):sw25 = b24 - 2*b25:b26 = int(b25/2):sw26 = b25 - 2*b26
        b27 = int(b26/2):sw27 = b26 - 2*b27:b28 = int(b27/2):sw28 = b27 - 2*b28
        b29 = int(b28/2):sw29 = b28 - 2*b29:b30 = int(b29/2):sw30 = b29 - 2*b30
        b31 = int(b30/2):sw31 = b30 - 2*b31  'MSB, Freq-b31
        sw32 = 0 'x4 multiplier, 1=enable, but not recommended
        sw33 = 0 'control bit
        sw34 = 0 'power down bit
        sw35 = 0 'phase data
        sw36 = 0 'phase data
        sw37 = 0 'phase data
        sw38 = 0 'phase data
        sw39 = 0 'phase data
    end if 'USB:05/12/2010
    return
'[endCreateBaseForDDSarray]
    */
}
void MainWindow::ResetDDS1serUSB()
{

  //USB:01-08-2010
  int pdmcmd = vars->phaarray[vars->thisstep][0]; //ver111-39d

  //(reset DDS3 to parallel)WCLK up,WCLK up and FQUD up,WCLK up and FQUD down,WCLK down
  QString USBwrbuf = "A10100" + util.ToHex(filtbank + 1);
  QString USBwrbuf2;
  bool result = usb->usbMSADeviceWriteString(USBwrbuf,4);
  if (!result)
  {
    //if USBdevice <> 0 then CALLDLL #USB, "UsbMSAInit", USBdevice as long, result as boolean
    usb->usbMSAInit();
    usb->usbMSADeviceWriteString(USBwrbuf,4);
  }
  USBwrbuf2 = "A30200"+util.ToHex(pdmcmd*64 + 2)+util.ToHex(pdmcmd*64);
  usb->usbMSADeviceWriteString(USBwrbuf2,5);
  USBwrbuf = "A10300"+util.ToHex(filtbank)+util.ToHex(filtbank + 1)+util.ToHex(filtbank);
  usb->usbMSADeviceWriteString(USBwrbuf,6);
  usb->usbMSADeviceWriteString(USBwrbuf2,5);
  //(end involk serial mode DDS3)
  //(flush and command DDS3)D7,WCLK up,WCLK down,(repeat39more),FQUD up,FQUD down
  //present data to buffer,latch buffer,disable buffer,present data+clk to buffer,latch buffer,disable buffer
  USBwrbuf = "A12801";
  QString USBwrbuf3 = util.ToHex( filtbank );
  for (int thisLoop = 0; thisLoop <= 39; thisLoop++)
  {
    USBwrbuf = USBwrbuf + USBwrbuf3;
  }
  usb->usbMSADeviceWriteString(USBwrbuf,43);
  usb->usbMSADeviceWriteString(USBwrbuf2,5);
  //(end flush command DDS3)

  return;
}

void MainWindow::ResetDDS3serUSB()
{
  //USB:01-08-2010
  //reset serial DDS3 without disturbing Filter Bank or PDM. usb v1.0
  //must have DDS (AD9850/9851) hard wired. pin2=D2=0, pin3=D1=1,pin4=D0=1, D3-D7 are don't care.
  //this will reset DDS into parallel, involk serial mode, then command to 0 Hz.
  int pdmcmd = vars->phaarray[vars->thisstep][0]; //ver111-39d

  //(reset DDS3 to parallel)WCLK up,WCLK up and FQUD up,WCLK up and FQUD down,WCLK down
  QString USBwrbuf = "A10100"+util.ToHex(filtbank + 1);
  usb->usbMSADeviceWriteString(USBwrbuf, 4);
  QString USBwrbuf2 = "A30200"+util.ToHex(pdmcmd*64 + 8)+util.ToHex(pdmcmd*64);
  usb->usbMSADeviceWriteString(USBwrbuf2, 5);
  USBwrbuf = "A10300"+util.ToHex(filtbank)+util.ToHex(filtbank + 1)+util.ToHex(filtbank);
  usb->usbMSADeviceWriteString(USBwrbuf, 6);
  usb->usbMSADeviceWriteString(USBwrbuf2, 5);
  //(end involk serial mode DDS3)
  //(flush and command DDS3)D7,WCLK up,WCLK down,(repeat39more),FQUD up,FQUD down
  //present data to buffer,latch buffer,disable buffer,present data+clk to buffer,latch buffer,disable buffer
  USBwrbuf = "A12801";
  QString USBwrbuf3 = util.ToHex( filtbank );
  //QString temp
  for (int thisloop = 0; thisloop <= 39; thisloop++)
  {
     USBwrbuf = USBwrbuf + USBwrbuf3;
  }
  usb->usbMSADeviceWriteString(USBwrbuf, 43);
  usb->usbMSADeviceWriteString(USBwrbuf2, 5);
  //(end flush command DDS3)
  return; //to //(InitializeDDS 3)
}
void MainWindow::CommandDDS1()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CommandDDS1]'ver111-36b. ver113-4a
    'this will recalculate DDS1, using the values in the Command DDS 1 Box, and "with DDS Clock at" Box.
    'it will insert the new DDS 1 frequency into the command arrays for all steps, leaving others alone
    'it will initiate a re-command at thisstep (where the sweep was halted)
      'if Original Control Board is used, only the DDS 1 is re-commanded. ver113-4a
      'if SLIM Control Board is used, all 4 modules will be re-commanded. ver113-4a
    'using One Step or Continue will retain the new DDS1 frequency.
    'PLO1 will be non-functional until [Restart] button is clicked. PLL1 will break lock and "slam" to extreme.
    '[Restart] will reset arrays and begin sweeping at step 0. Special Tests Window will not be updated.
    'Signal Generator or Tracking Generator output will not be effected.
    'caution, do not enter a frequency that is higher than 1/2 the masterclock frequency (ddsclock)
    print #special.dds1out, "!contents? dds1out$";   'grab contents of Command DDS 1 Box
    ddsoutput = val(dds1out$) 'intended output frequency of DDS 1
    print #special.masclkf, "!contents? msclk$";   'grab contents of "with DDS Clock at" box
    msclk = val(msclk$) 'if "with DDS Clock at" box was not changed, this is the real MasterClock frequency
    ddsclock = msclk
    'caution: if ddsoutput >= to .5 ddsclock, the program will error out
    gosub [CreateBaseForDDSarray]'needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
    remember = thisstep 'remember where we were when entering this subroutine
    for thisstep = 0 to steps 'ver112-2a
    gosub [FillDDS1array]'need thisstep,sw0-sw39,w0-w4,base,ddsclock
    next thisstep 'ver112-2a
    thisstep = remember 'ver112-2a
    gosub [CreateCmdAllArray] 'ver112-2a
    if cb = 0 then gosub [CommandDDS1OrigCB]'will command DDS 1, only
'delver113-4a    if cb = 2 then gosub [CommandDDS1SlimCB]'will command DDS 1, only
    if cb = 2 then gosub [CommandAllSlims]'will command all 4 modules. ver113-4a
    if cb = 3 then gosub [CommandAllSlimsUSB]'will command all 4 modules. ver113-4a 'USB:01-08-2010
    wait

*/
}

void MainWindow::CommandDDS3()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CommandDDS3]'ver111-38a
    'this will recalculate DDS3, using the values in the Command DDS 3 Box, and "with DDS Clock at" Box.
    'it will insert the new DDS 3 frequency into the command arrays for all steps, leaving others alone
    'it will initiate a re-command at thisstep (where the sweep was halted)
      'only the DDS 3 is re-commanded
    'using One Step or Continue will retain the new DDS3 frequency.
    'PLO3 will be non-functional until [Restart] button is clicked. PLL3 will break lock and "slam" to extreme.
    '[Restart] will reset arrays and begin sweeping at step 0. Special Tests Window will not be updated.
    'Signal Generator or Tracking Generator output will be non functional.
    'Spectrum Analyzer function is not effected
    'caution, do not enter a frequency that is higher than 1/2 the masterclock frequency (ddsclock)
    print #special.dds3out, "!contents? dds3out$";   'grab contents of Command DDS 3 Box
    ddsoutput = val(dds3out$) 'intended output frequency of DDS 3
    print #special.masclkf, "!contents? msclk$";   'grab contents of "with DDS Clock at" box
    msclk = val(msclk$) 'if "with DDS Clock at" box was not changed, this is the real MasterClock frequency
    ddsclock = msclk
    'caution: if ddsoutput >= to .5 ddsclock, the program will error out
    gosub [CreateBaseForDDSarray]'needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
    remember = thisstep 'remember where we were when entering this subroutine
    for thisstep = 0 to steps
    gosub [FillDDS3array]'need thisstep,sw0-sw39,w0-w4,base,ddsclock
    next thisstep
    thisstep = remember
    gosub [CreateCmdAllArray]
    if cb = 0 then gosub [CommandDDS3OrigCB]'will command DDS 3, only
'delver113-4a    if cb = 2 then gosub [CommandDDS3SlimCB]'will command DDS 3, only
    if cb = 2 then gosub [CommandAllSlims]'will command all 4 modules. ver113-4a
    if cb = 3 then gosub [CommandAllSlimsUSB]'will command all 4 modules. ver113-4a 'USB:01-08-2010
    wait
*/
}

void MainWindow::DDS3Track()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[DDS3Track]'ver111-39d
    'This uses DDS3 as a Tracking Generator, but is limited to 0 to 32 MHz, when MasterClock is 64 MHz
    'DDS3 spare output is rich in harmonics and aliases.
    'Tracks the values in Working Window, Center Frequency and Sweep Width (already in the command arrays)
    'The Spectrum Analyzer function is not effected.
    'PLO3, Normal Tracking Generator, and Phase portion of VNA will be non-functional
    'Operation:
    'In Working Window, enter Center Frequency to be within 0 to 32 (MHz), or less than 1/2 the MasterClock
    'In Working Window, enter Sweep Width (in MHz). But, do not allow sweep to go below 0 or abov 1/2 MasterClock
    'Click [Restart], then halt.
    'In Special Tests Window, click [DDS 3 Track].  DDS 3 will, immediately, re-command to new frequency.
    'Click [Continue]. Sweep will resume, but with DDS 3 tracking the Spectrum Analalyzer
    '[One Step] and [Continue] and halting operates normally until [Restart] button is pressed.
    '[Restart] will reset arrays, and leave the DDS 3 Track Mode. ie, normal sweeping.
    ddsclock = masterclock
    remember = thisstep
    for thisstep = 0 to steps
    ddsoutput = datatable(thisstep,1)
    'caution: if ddsoutput >= to .5 ddsclock, the program will error out
    gosub [CreateBaseForDDSarray]'needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
    gosub [FillDDS3array]'need thisstep,sw0-sw39,w0-w4,base,ddsclock
    next thisstep
    thisstep = remember
    gosub [CreateCmdAllArray]
    if cb = 0 then gosub [CommandDDS3OrigCB]'will command DDS 3, only
    if cb = 2 then gosub [CommandAllSlims]'will command all 4 modules. ver113-4a
    if cb = 3 then gosub [CommandAllSlimsUSB]'will command all 4 modules. 'USB:01-08-2010
    wait
*/
}

void MainWindow::DDS1Sweep()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
/*
[DDS1Sweep]'ver112-2c
    'This forces the DDS 1 to the values in Working Window: Center Frequency and Sweep Width (already in the command arrays)
    'DDS1 spare output is rich in harmonics and aliases.
    'PLO1, and thus, the Spectrum Analyzer will be non-functional in this mode.
    'Signal Generator or Tracking Generator output will not be affected.
    'Operation:
    'In Working Window, enter Center Frequency to be within 0 to 32 (MHz), or less than 1/2 the MasterClock
    'In Working Window, enter Sweep Width (in MHz). But, do not allow sweep to go below 0 or abov 1/2 MasterClock
    'Click [Restart], then halt.
    'In Special Tests Window, click [DDS 1 Sweep].  DDS 1 will, immediately, re-command to new frequency.
    'Click [Continue]. Sweep will resume, but with DDS 1 sweeping.
    '[One Step] and [Continue] and halting operates normally until [Restart] button is pressed.
    '[Restart] will reset arrays, and will leave the DDS 1 Sweep Mode. ie, normal sweeping.
    ddsclock = masterclock
    remember = thisstep
    for thisstep = 0 to steps
    ddsoutput = datatable(thisstep,1)
    'caution: if ddsoutput >= to .5 ddsclock, the program will error out
    gosub [CreateBaseForDDSarray]'needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
    gosub [FillDDS1array]'need thisstep,sw0-sw39,w0-w4,base,ddsclock
    next thisstep
    thisstep = remember
    gosub [CreateCmdAllArray]
    if cb = 0 then gosub [CommandDDS1OrigCB]'will command DDS 1, only
    if cb = 2 then gosub [CommandAllSlims]'will command all 4 modules. ver113-4a
    if cb = 3 then gosub [CommandAllSlimsUSB]'will command all 4 modules.  'USB:01-08-2010 moved ver116-4f
    wait
*/
}

void MainWindow::ChangePDM()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ChangePDM]'ver112-2a
    'enters from Special Tests Window Button
    setpdm = setpdm + 1
    if setpdm > 2 then setpdm = 0
    if setpdm = 0 then print #special.pdminv, "PDM is Auto"
    if setpdm = 1 then print #special.pdminv, "PDM in Normal":gosub [PdmNorm]
    if setpdm = 2 then print #special.pdminv, "PDM in Inverted":gosub [PdmInv]
    wait
*/
}

void MainWindow::PdmNorm()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ChangePDM]'ver112-2a
    'enters from Special Tests Window Button
    setpdm = setpdm + 1
    if setpdm > 2 then setpdm = 0
    if setpdm = 0 then print #special.pdminv, "PDM is Auto"
    if setpdm = 1 then print #special.pdminv, "PDM in Normal":gosub [PdmNorm]
    if setpdm = 2 then print #special.pdminv, "PDM in Inverted":gosub [PdmInv]
    wait
[PdmNorm]'this commands the pdm to Normal, for all steps
    rememberthisstep = thisstep
    for thisstep = 0 to steps
    phaarray(thisstep,0) = 0
    next thisstep
    thisstep = rememberthisstep
    gosub [CommandPDMonly]
    return
*/
}

void MainWindow::PdmInv()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[PdmInv]'this commands the pdm to Invert, for all steps
    rememberthisstep = thisstep
    for thisstep = 0 to steps
    phaarray(thisstep,0) = 1
    next thisstep
    thisstep = rememberthisstep
    gosub [CommandPDMonly]
    return
    */
}

void MainWindow::SyncTestPDM()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[SyncTestPDM] 'ver112-2b
    'enters from Special Tests Window Button, only if in VNA Mode
    'this will set up defaults and begin sweeping to measure phase steps
    'when CF=0 and SW=0, the PDM will measure "rolling" phase of two different frequencies,
    'although the difference is less than 1 Hz.PDM is fixed at Norm(0)
    syncsweep = 1 'ver112-2b
    convdatapwr = 1 'ver112-2b
            'SEWgraph updated the following to deal with changes in text boxes
    call SetCenterSpanFreq 0,0  'SEWgraph zero width, zero center
    wate=3  'SEWgraph 3ms wait time ver114-4d
    if primaryAxisNum=1 then 'ver115-3b
        call SetY2Range 0,360 'Phase range 0 to 360 ver114-4d
        call SetY1Range -5, 5 'Mag range -5 to 5 ver114-4d
    else
        call SetY1Range 0,360 'Phase range 0 to 360 ver114-4d
        call SetY2Range -5, 5 'Mag range -5 to 5 ver114-4d
    end if

    call gSetXIsLinear 1    'SEWgraph Be sure sweep is linear
    call gCalcGraphParams   'SEWgraph Calculate scaling, etc. with new parameters
    setpdm = 1 : print #special.pdminv, "PDM in Normal":gosub [PdmNorm]
    haltsweep=0 'So Restart actually restarts 'ver114-6e
    goto [Restart]
    wait
*/
}
void MainWindow::SyncSweep()
{
  //comes here at end of sweep if syncsweep = 1 //ver112-2b
  //it will not continue sweeping until the phase data is between 80 and 90% of maxpdmout
  //hopefully, it will "trigger" a sweep at 81%
  while(1)
  {
    ReadPhase();
    //scan //this is a fail safe. Click any button to get out of this loop. ver113-6e
    QCoreApplication::processEvents();
    if (phadata > .8 * activeConfig.maxpdmout && phadata < .9 * activeConfig.maxpdmout)
      return;
  }
}

void MainWindow::ConvertDataToPower()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  [ConvertDataToPower] 'ver112-2b
      'this routine is a traffic director when the blue Magnitude trace is used for other data.
      'enters from [CalcMagpowerPixel] if convdatapwr = 1
          if syncsweep = 1 then goto [ConvertSync]
          goto [ConvertPDMlin]
*/
}

void MainWindow::ConvertSync()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ConvertSync] 'ver112-2b
    'this will take the phase difference of the previous step's and this step's phase
    'and convert it to power, for display
    'enters from [ConvertDataToPower] if syncsweep = 1
        if thisstep = 0 then return 'the last step in the sweep - step 0 is bogus data
    'grab raw phase bits from previous sweep and create deltabits
        deltabits = phaarray(thisstep-sweepDir,3) - phaarray(thisstep,3) 'ver114-4m
    'convert deltabits to delta phase
        deltaphase = 360 * deltabits/maxpdmout
        power = deltaphase
        return
*/
}

void MainWindow::PresetVNAlin()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[PresetVNAlin] 'ver112-2b
    print #special.prevnalin, "Test Transmission Linearity"  'change the button name
    if vnalintest = 1 then goto [VNAlinTest]
    vnalintest = 1
    for i=0 to steps : lineCalArray(i,0)=0 : lineCalArray(i,1)=0 : lineCalArray(i,2)=0 : next i 'ver116-1b
    call SignalNoCalInstalled   'ver116-4b
        'SEWgraph updated the following to deal with changes in text boxes
    call SetCenterSpanFreq 500,1000 'SEWgraph; 500 MHz center, 1000 MHz span
    wate=22  'ver114-4d; 22 ms wait time
    'afix the PDM to "Normal" (0).  It will remain fixed as long as the Special Tests Window is open.
    setpdm = 1 : print #special.pdminv, "PDM in Normal":gosub [PdmNorm]
    'set up magnitude limits, prolly +/- 5 degrees(db)
    if primaryAxisNum=1 then 'ver115-3b
        call SetY1Range -5, 5 'This is actually degrees, not power
        call SetY2Range -180, 180   'phase
    else
        call SetY2Range -5, 5 'This is actually degrees, not power
        call SetY1Range -180, 180   'phase
    end if

    call gSetXIsLinear 1    'SEWgraph be sure sweep is linear
    call gCalcGraphParams   'SEWgraph  Calculate scaling, etc. with restored parameters
    haltsweep=0 'So Restart actually restarts 'ver114-6e
    goto [Restart]
    'the sweep will begin, and display several sawtooths, the number depending on the length
        'of the cable between the TG output and the MSA input.
    'There will be extreme glitches near the center of each sawtooth, indicating the
        'zero degree crossovers.  This is due to the PDM being fixed at "Normal"
*/
}

void MainWindow::VNAlinTest()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[VNAlinTest]'for testing PDM Linearity 'ver112-2b
    'make sure pdm is fixed at norm or invert before entering. Accomplished by previously clicking [PresetVNAlin] Button.
    'before entering this routine, make sure the sweep is displaying one full segment that
      ' is greater than 360 degrees and less than 720 degrees.
    'before entering this routine, pick a step point to be the zero phase error refpoint. It has:
      'step #, frequency, and phase. It must be within the 0 degree crossover boundries.
      'we use it as a reference zero degrees error. Use the Mouse's left click to select the step.
      'then, click the Special Tests Window [Test VNA Linearity] Button.
    'a very long line, between TG out and MSA in, will reduce the error created by Mixer 4, AM to PM conversion
        convdatapwr = 1 'used in [CalcMagpowerPixel] to skip processing Mag Data
    'determine the "real" slope factor for this band segment, in degrees per MHz

    'retrieve raw phase data bits. Bits0, Bits400 'ver112-2g
        phabits0 = phaarray(0,3) 'ver112-2g
        phabits400 = phaarray(steps,3) 'ver112-2g
    'if phabits0 is less than or equal to phabits400, add 360 degrees(in bits) to phabits0 'ver112-2g
    'maxpdmout is the bit value of 360 degrees, for any AtoD Module
        if phabits0 <= phabits400 then phabits0 = phabits0 + maxpdmout 'ver112-2g
    'add maxpdmout (65535 bits) to phabits0 and take difference 'ver112-2g
    'delta bits = maxpdmout + phabits0 - phabits400 'ver112-2g
    'change to phase: 360 * (delta bits/maxpdmout) 'ver112-2g
        delpha = 360*((maxpdmout + phabits0 - phabits400)/maxpdmout) 'ver112-2g

    'freq at step 0 - freq at last step = delta freq (551.8 - 829 = -277.2)
        delfreq = datatable(0,1) - datatable(steps,1)
        'SEWgraph The following line avoids a crash with zero span
        if delfreq=0 then notice "Sweep must be preset to show 360-720 degrees." :wait 'SEWgraph
    'realslopefactor (in deg per MHz) = delta phase / delta freq = (393.7 / -277.2) = -1.42 d/m
        realslopefactor = delpha/delfreq '(in -degrees/MHz)
        refstep = leftstep
    'the processed phase for refstep is in datatable(refstep,3),always for previous sweep
        refstepphase = datatable(refstep,3)
    'the frequency of refstep is in datatable(refstep,1),always for previous sweep
        refstepfreq = datatable(refstep,1)
    'print the slope factor in the Message Box next time sweep is halted
    test=realslopefactor 'this will be cleared when leaving Spec Test Window. ver112-2g
    haltsweep=0 'So Restart actually restarts 'ver114-6e
    goto [Restart]
*/
}

void MainWindow::ConvertPDMlin()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ConvertPDMlin] 'ver112-2b
    'enters here from [CalcMagpowerPixel]. We are processing the previous step but it
    'is still called "thisstep"
    'the blue line will show how far off the real slope is from a theoritical slope, in degrees
    'the processed phase for thisstep is in datatable(thisstep,3)
    'the frequency of thisstep is in datatable(thisstep,1)
    'this step will have a theoritical phase equal to:
    ' theoryphase = thisstep freq - refstep freq times realslopefactor + refstep phase
        theoryphase = ((datatable(thisstep,1)- refstepfreq) * realslopefactor) + refstepphase
    'in wide band sweeps, theoryphase can get greater than 360 degrees 'ver112-2g
    'therefore, change theoryphase to decimal number compared to 360 degrees 'ver112-2g
        theoryphase = theoryphase/360   'example theoryphase = 1.324  or -1.226 'ver112-2g
    'cast out whole number and use decimal equivalent 'ver112-2g
        theoryphase = theoryphase - int(theoryphase)    'theoryphase = .324 or -.226 'ver112-2g
    'reconvert back to phase 'ver112-2g
        theoryphase = 360*theoryphase 'ver112-2g
        if theoryphase < -180 then theoryphase = theoryphase + 360
        if theoryphase > 180 then theoryphase = theoryphase - 360
      'at this line, I could make power = theoryphase and display blue over red error
    'the error will be equal to:  phaseerror = thisstep phase - theoryphase
        phaseerror = datatable(thisstep,3) - theoryphase
        if phaseerror < -180 then phaseerror = phaseerror + 360
        if phaseerror > 180 then phaseerror = phaseerror - 360
    'using the blue magpower line as phase error, the processed magpower becomes phaseerror
        power = phaseerror
        return
*/
}

void MainWindow::RequireRestart()
{
  //Disable OneStep and Continue so user can only proceed by Restart
  if (vars->doingInitialization) return;    //Buttons don't exist yet    ver114-3f
  graph.haltsweep=0;
  DisplayButtonsForHalted();
  ui->btnOneStep->setEnabled(false);
  ui->btnContinue->setEnabled(false);
}

void MainWindow::DisplayButtonsForRunning()
{
  //Display buttons for sweep in progress
  if (vars->doingInitialization) return;    //Buttons don't exist yet    ver114-3f
  ui->btnOneStep->setEnabled(true);
  ui->btnContinue->setEnabled(true);
  //print #handle.Restart, "Running"  //ver114-4c deleted print to #main.restart
              //OneStep becomes HaltAtEnd when scan is in progress
  //#handle.OneStep, "Halt At End"
  ui->btnOneStep->setText("Halt At End");
      //Continue becomes Halt when scan is in progress
  ui->btnContinue->setText("Halt");
  ui->btnRedraw->setVisible(false);

}

void MainWindow::DisplayButtonsForHalted()
{
  //Display buttons for sweep halted, to enable resuming or restarting
  if (vars->doingInitialization) return;    //Buttons don't exist yet    ver114-3f
  ui->btnOneStep->setEnabled(true);
  ui->btnContinue->setEnabled(true);
  // fix me print #handle.Restart, "Restart"  //ver114-4c deleted print to #main.restart
          //OneStep becomes HaltAtEnd when scan is in progress
  ui->btnOneStep->setText("One Step");
      //Continue becomes Halt when scan is in progress
  ui->btnContinue->setText("Continue");
  ui->btnRedraw->setVisible(true); //hide during scan
}


void MainWindow::GetDialogPlacement()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  sub GetDialogPlacement   'calculate top left for placement of dialog
      'Dialog height,width are in WindowHeight, WindowWidth. Placement is put into UpperLeftX and UpperLeftY
          'Dialog is placed relative to parent window client origin. Place so it does not cover graph area,
          'aligned with right graph window edge.
      call gGetGridCorner "LR", cornerX, cornerY
      UpperLeftX = currGraphBoxWidth+clientWidthOffset-WindowWidth-30     'puts right edge of dialog near right window edge
      UpperLeftY=cornerY+30   'puts top of dialog under graph
  end sub
      */
}

void MainWindow::InstallSelectedLineCal()
{

  //Apply full line cal or baseLine cal per applyCalLevel
  //We put the necessary data into lineCalArray, and set actualLineCalLevel to 0(None), 1 (BaseLine)
  //or 2 (Full LineCal) to indicate the level of line cal actually installed.
    //If desiredCalLevel=2 and LineCal is current, install it

  if (vars->calInProgress)    //ver115-1e
  {
        //Here we want no type of cal, so signal nothing is installed but don't clear the actual data
    vnaCal.SignalNoCalInstalled();   //ver116-4b
    return;
  }

  if (vnaCal.desiredCalLevel==2)
  {
    int isCurr=vnaCal.BandLineCalIsCurrent();
        //If we are already applying cal, and it is valid and the installed time stamp matches the band cal, we are done
    if (vnaCal.installedBandLineTimeStamp==vnaCal.bandLineTimeStamp && vnaCal.applyCalLevel==2 && isCurr) return; //ver115-2d
    if (isCurr) vnaCal.applyCalLevel=2; else vnaCal.applyCalLevel=1;
  }
  else
  {
    vnaCal.applyCalLevel=1; //base cal
  }

  if (vnaCal.applyCalLevel==2)
  {
    vnaCal.installedBaseLineNumSteps=-1;   //Indicate that base line cal is not installed  ver115-1b fixed typo
    for (int i=0; i <= vars->globalSteps; i++)    //retrieve data
    {
        vars->lineCalArray[i][0]=vars->bandLineCal[i][0]; //freq    //ver115-5c
        vars->lineCalArray[i][1]=vars->bandLineCal[i][1]; //mag
        vars->lineCalArray[i][2]=vars->bandLineCal[i][2]; //Phase
    }
    vnaCal.installedBandLineTimeStamp=vnaCal.bandLineTimeStamp;    //ver115-2d
    return;
  }

    //We get here if applyCalLevel<2 or we did not have a current LineCal, so use BaseLineCal
  if (vnaCal.desiredCalLevel>0)
  {
    if (vnaCal.BaseLineCalIsCurrent()) vnaCal.applyCalLevel=1; else vnaCal.applyCalLevel=0;  //ver115-1b
  }
  else
  {
    vnaCal.applyCalLevel=0;
  }

  if (vnaCal.applyCalLevel==1)
  {
    //We want and have BaseLine cal, so install it
    if (vnaCal.BaseLineCalIsInstalled()) return;   //Already installed with these sweep params

    int doPhase, doParams;
    //ver114-5p changed interpolation to use the new Interpolation Module
    if (vars->msaMode=="SA" || vars->msaMode=="ScalarTrans")
    {
      doPhase=0; doParams=1;   //do mag only
    }
    else
    {
        doPhase=1;doParams=3;   //Do both mag and phase
    }
    inter.intSetMaxNumPoints(1+qMax(vnaCal.baseLineNumSteps, vars->globalSteps));  //Be sure we have room ver115-9d
    inter.intClearSrc(); inter.intClearDest();
    for (int i=0; i <= vnaCal.baseLineNumSteps; i++) //copy cal table to intSrc
    {
        inter.intAddSrcEntry(vars->baseLineCal[i][0],vars->baseLineCal[i][1],graph.gNormalizePhase(vars->baseLineCal[i][2]));
    }
    for (int i=1; i <= vars->globalSteps+1; i++)
    {
      inter.intAddDestFreq(graph.gGetPointXVal(i));   //Install frequencies in intDest
    }

    inter.intSrcToDest(doPhase, 0, doParams);  //Do the actual interpolation into intDest()
    for (int i=0; i <= vars->globalSteps; i++)  //put the data where we want it
    {
      int f,m,p;
      inter.intGetDest(i+1,f, m, p);
      vars->lineCalArray[i][0]=f;
      //ver115-2d eliminated rounding
      vars->lineCalArray[i][1]=m;
      p=graph.gNormalizePhase(p);
      vars->lineCalArray[i][2]=p;
    }

    //Save the sweep params under which we installed base line cal
    vnaCal.installedBaseLineStartFreq=vars->startfreq;
    vnaCal.installedBaseLineEndFreq=vars->endfreq;
    vnaCal.installedBaseLineNumSteps=vars->globalSteps;
    vnaCal.installedBaseLineLinear=graph.gGetXIsLinear();
    vnaCal.installedBaseLineTimeStamp=vnaCal.baseLineTimeStamp;    //ver115-2d
    //ver115-1b xLL  deleted installedBaseLinePath$
    return;
  }  //end of applying baseLine cal

  //Here we want no type of line cal
  vnaCal.SignalNoCalInstalled();   //ver116-4b
}
int MainWindow::CreateOperatingCalFolder()
{
  if (!QDir().mkpath(DefaultDir + "/MSA_Info/OperatingCal"))
    return 1;

  return 0;
}

void MainWindow::SaveBaseLineCalFile()
{

 if (CreateOperatingCalFolder())
 {
   QMessageBox::warning(0, "Error", "Cannot save BaseLine Cal file.");
   return;
 }

 QStringList list;
 QFile baseLineOut(DefaultDir + "/MSA_Info/OperatingCal/BaseLineCal.txt");
 if (baseLineOut.open(QFile::WriteOnly))
 {
   QTextStream s(&baseLineOut);
   BaseLineCalContextToFile(list);
   QString sss = list.join("\r");
   s << sss;

   baseLineOut.close();
 }
}

QFile *MainWindow::OpenBaseLineCalFile()
{
  //Open baseline calibration file; return its handle
  //If file does not exist, return "".

  QFile *fFile = new QFile(DefaultDir + "/MSA_Info/OperatingCal/BaseLineCal.txt");

  if (!fFile->exists())
  {
    QString ggg = "file not found";
    delete fFile;
    fFile = NULL;
  }
  else if (!fFile->open(QFile::ReadOnly))
  {
    QString eee = fFile->errorString();
    qDebug() << eee;
    delete fFile;
    fFile = NULL;
  }

  return fFile;
}

int MainWindow::LoadBaseLineCalFile()
{
  int retVal;

  //Return 1 if error (file does not exist)
  QFile *fHndl = OpenBaseLineCalFile();
  if (fHndl==NULL) { retVal=1; vnaCal.baseLineNumSteps=-1; return retVal;}
  int nPoints=GetBaseLineCalContextFromFile(fHndl);
  if (nPoints<=0)
  {
   retVal=1; vnaCal.baseLineNumSteps=-1; //error
  }
  else
  {
   retVal=0;
  }
  fHndl->close();
  delete fHndl;

  return retVal;
}

void MainWindow::FunctChangeAndSaveSweepParams()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub FunctChangeAndSaveSweepParams saveSettings, bandToBase, newSteps, newStart, newEnd, newLinear    'Change sweep params for use in a Function; save old params
    'We also set sweep direction, planeadj and wate to default values
    'We also save the current band cal as a base cal. If saveSettings=1 we also save the current sweep settings.
    'if bandToBase=1, we transfer the existing band cal to the base cal (but not to the file) and use it.
    'Caller must set steps (but not globalSteps) before calling us because we can't change steps, but
    'we will handle informing the graph module of the new number of steps
    'Currently, this does not work for Reflection mode, because OSL is not saved.
    'Example of Use:
    '   steps=100   'Note globalSteps will change in FunctChangeAndSaveSweepParams
    '   call FunctChangeAndSaveSweepParams 1,1,1, 100, 2, 200, 1    'save settings and cal and change to Linear 2 to 200 MHz, 100 steps
    '   specialOneSweep=1   'So we return from [Restart]
    '   gosub [Restart]     'Do actual scan to acquire data
    '       ...Process data...
    '   gosub [FunctRestoreSweepParams]
    '   suppressHardwareInitOnRestart=1 'if there is no need to update hardware and we want to save time; this is set to 0 afer restarting
    '   gosub [PartialRestart]  'or call RequireRestart
    'If a function calls several times, the existing settings should be saved only on the first call; otherwise
    'the saved data will get overwritten by the previous changes

    if saveSettings then  'Save existing settings
        functSaveAlternate=alternateSweep : functSaveSweepDir=sweepDir : functSavePlaneAdj=planeadj
        functSaveSteps=globalSteps : functSaveStartFreq=startfreq : functSaveEndFreq=endfreq
        functSaveAutoY1=autoScaleY1 : functSaveAutoY2=autoScaleY2
        functSaveY1Mode=Y1DisplayMode : functSaveY1Mode=Y1DisplayMode
        functSaveY1DataType=Y1DataType : functSaveY2DataType=Y2DataType
        call gGetIsLinear functSaveXIsLinear, functSaveY1IsLinear, functSaveY2IsLinear
        call gGetNumDivisions functSaveNumHorDiv, functSaveNumVertDiv 'Note log sweep changes the hor divisions
        functSaveDesiredCalLevel=desiredCalLevel
    end if
    'Set up the sweep params we want
    call FunctSetVideoAndAutoWait saveSettings  'save old settings and set new
    alternateSweep=0 : sweepDir=1 : planeadj=0
    globalSteps=newSteps        'Caller must change steps
    call gSetXIsLinear newLinear
    call gSetNumDynamicSteps globalSteps    'Tell graph module
    call SetStartStopFreq newStart, newEnd
    if bandToBase and (msaMode$="VectorTrans" or msaMode$="ScalarTrans") then
        call TransferBandToBaseLineCal  'Make current band cal into a base cal so it can be interpolated
        desiredCalLevel=1   'Use the base cal we just created
        call SignalNoCalInstalled   'ver116-4b
    end if
end sub
*/
}

void MainWindow::FunctSetVideoAndAutoWait()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub FunctSetVideoAndAutoWait saveSettings    'Set video filter to narrow and wait to autoWait modver116-4k
    if saveSettings then _
        functSaveWate=wate : functSaveAutoWait=useAutoWait: functSaveAutoWaitPrecision$=autoWaitPrecision$ : functSaveVideoFilter$=videoFilter$ 'ver116-1b
    if videoFilter$="XNarrow" then desiredVideoFilter$="XNarrow" else desiredVideoFilter$="Narrow"
    if canUseAutoWait then
        if useAutoWait=0 or (autoWaitPrecision$<>"Precise") then autoWaitPrecision$="Normal"    'choose Normal unless user is already Precise
        useAutoWait=1
    else
        wate=500    'It should always be possible to use auto wait, but just in case...
    end if
    if videoFilter$<>desiredVideoFilter$ and switchHasVideo=0 then Notice "Physically select "; desiredVideoFilter$; " video filter."
    videoFilter$=desiredVideoFilter$
    call SelectVideoFilter  'also recalculates auto wait info
end sub
*/
}

void MainWindow::FunctRestoreVideoAndAutoWait()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub FunctRestoreVideoAndAutoWait    'restore auto wait settings and, if automated video switch, video filter setting
    wate=functSaveWate : autoWaitPrecision$=functSaveAutoWaitPrecision$ : useAutoWait=functSaveAutoWait
    if switchHasVideo then
        'if no automated video filter switch, we leave the video filter as it was set for the function
        videoFilter$=functSaveVideoFilter$
        call SelectVideoFilter  'Note this also calls autoWaitPrecalculate ver116-4b
    else
        call autoWaitPrecalculate
    end if
end sub
*/
}

void MainWindow::FunctRestoreSweepParams()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[FunctRestoreSweepParams]  'Restore params and base cal to what they were when function was entered
    'Not a true subroutine so it can access steps.
    call FunctRestoreVideoAndAutoWait
    sweepDir=functSaveSweepDir : alternateSweep=functSaveAlternate : planeadj=functSavePlaneAdj
    steps=functSaveSteps : startfreq=functSaveStartFreq : endfreq=functSaveEndFreq
    globalSteps=steps
    autoScaleY1=functSaveAutoY1 : autoScaleY2=functSaveAutoY2
    Y1DisplayMode=functSaveY1Mode : Y1DisplayMode=functSaveY1Mode
    Y1DataType=functSaveY1DataType : Y2DataType=functSaveY2DataType
    call gSetIsLinear functSaveXIsLinear, functSaveY1IsLinear, functSaveY2IsLinear
    call gSetNumDynamicSteps steps    'Tell graph module
    call SetStartStopFreq functSaveStartFreq, functSaveEndFreq
    call gSetNumDivisions functSaveNumHorDiv, functSaveNumVertDiv

    if msaMode$="VectorTrans" or msaMode$="ScalarTrans" then
        dum=LoadBaseLineCalFile()   'Reload base cal file to restore prior state
    end if
    desiredCalLevel=functSaveDesiredCalLevel    'ver115-5e
    call SignalNoCalInstalled   'ver116-4b'So new cal will get installed on Restart
return
*/
}

void MainWindow::menuRunConfig()
{
//Graph Window Menu,Setup,Configuration Manager was selected
  //if (graph.haltsweep==1) then gosub [FinishSweeping]     //Finish last point of sweep that was in progress. ver116-4j
  //savePath$=path$ //ver114-4c
  int cancelled=winConfigMan->configRunManager(0);   //0 signals we are not running on startup so cancellation is allowed
  if (cancelled)   //Cancelled; restore filter setting; Halt or wait
  {
    //vars->path=vars->savePath; //ver114-4c
    SelectFilter(filtbank);  //ver116-4j
    return;
  }
  finished();     //Must restart if config was changed

}

void MainWindow::menuRunCal()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[menuRunCal]'Menu item for config manager was selected 'SEW6 rewrote routine.ver113-7c
    if haltsweep=1 then gosub [FinishSweeping]     'Finish last point of sweep that was in progress. ver116-4j
    if calManWindHndl$<>"" then wait    'Do nothing if cal mangager already running ver116-1b
    savePath$=path$ 'ver114-4c
    'calRunManager will return with filter path 1 installed.
    call RequireRestart     'SEW8 Let the user proceed only by Restarting
    gosub [calManRunManager]
    path$=savePath$ 'ver114-4c  'restores prior filter
    call SelectFilter filtbank  'ver116-4j
    wait
*/
}
void MainWindow::CreateGraphWindow()
{
//We do this only once, at startup. After that, we work with the existing window, adjust its menus and redraw.
  //BackgroundColor="buttonface"; ForegroundColor="black";
  resize(800,600);
  //WindowWidth=800 : WindowHeight=600

  ConformMenusToMode(); //Hide whatever menu items we don't need for current msaMode$
/*
    print #handle.g, "when rightButtonDown [RightButDown]"
    print #handle.g, "when leftButtonDouble [LeftButDouble]"
    print #handle, "trapclose [finished]"   //goto [finished] if xit is clicked
        //Note that full initialization of graph parameters is done at the commencement of a scan series.
    #handle.g, "autoresize"
    #handle, "resizehandler [ResizeGraphHandler]"
    #handle.Cover, "!hide"  //Used only during resizing
    #handle.OneStep, "!font Arial 9 bold" : #handle.Continue, "!font Arial 9 bold"
    #handle.Redraw, "!font Arial 9 bold" : #handle.Restart, "!font Arial 9 bold"
    #handle.Redraw, "!hide"     //We start out running so hide this
*/        //Tell graph module what size we are, and calculate scaling ver114-6f
    graph.gUpdateGraphObject(graph.currGraphBoxWidth, graph.currGraphBoxHeight,  //ver115-1c
                                        graph.graphMarLeft, graph.graphMarRight, graph.graphMarTop, graph.graphMarBot);
    ui->graphicsView->fitInView(graph.getScene()->sceneRect());
    graph.gCalcGraphParams();   //Calculate new scaling. May change min or max.
    float xMin, xMax;
    graph.gGetXAxisRange(xMin, xMax);
    if (vars->startfreq!=xMin || vars->endfreq!=xMax) graph.SetStartStopFreq(xMin, xMax);
    return; //From [CreateGraphWindow]
}

void MainWindow::menuQuitMultiscan()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub menuQuitMultiscan   'Quit multiscan
    'This command is made in the main window, so the multiscan windows will already be hidden, but haven't actually been closed.
    multiscanIsOpen=0 : multiscanInProgress=0
    call multiscanCloseAll
end sub
*/
}
void MainWindow::ConformMenusToMode()
{

 //Make menus and window caption match mode.
  //msaMode$ is the current mode. menuMode$ is the mode to which the menus are currently conformed.

  QString modeTitle;
  if (vars->msaMode=="SA")
  {
    if (vars->gentrk==0) modeTitle="Spectrum Analyzer Mode"; else modeTitle="Spectrum Analyzer with TG Mode";
  }
  if (vars->msaMode=="ScalarTrans") modeTitle="Tracking Generator Mode";
  if (vars->msaMode=="VectorTrans") modeTitle="VNA Transmission Mode";
  if (vars->msaMode=="Reflection") modeTitle="VNA Reflection Mode";
  QString ver="Ver " + QCoreApplication::applicationVersion();
  setWindowTitle("MSA-Qt Graph Window for "+modeTitle+ "; "+ ver);

  //Note we continue even if there is no mode change, mainly to get multiscan window right
  //if msaMode$=menuMode$ then exit sub //Nothing to do

  int wasTransMode = (vars->menuMode=="ScalarTrans" || vars->menuMode=="VectorTrans");  //whether prior mode was transmission
  int isTransMode= (vars->msaMode=="ScalarTrans" || vars->msaMode=="VectorTrans"); //whether current mode is transmission
  if (wasTransMode && isTransMode) {vars->menuMode=vars->msaMode; return; }   //Nothing needs changing
/*
    //Hide every menu item that is ever to be hidden, then show what we want.
  menuOK=uHideCommandItem(hDataMenu, menuDataS21ID)
  menuOK=uHideCommandItem(hDataMenu, menuDataLineCalID)
  menuOK=uHideCommandItem(hDataMenu, menuDataS11ID)
  menuOK=uHideCommandItem(hDataMenu, menuDataS11DerivedID)
  menuOK=uHideCommandItem(hDataMenu, menuDataLineCalRefID)
  menuOK=uHideCommandItem(hDataMenu, menuDataLineCalOSLID)

  if menuOptionsSmithID>0 then menuOK=uHideCommandItem(hOptionsMenu, menuOptionsSmithID)

  menuOK=uHideCommandItem(hFunctionsMenu, menuFunctionsFilterID)
  menuOK=uHideCommandItem(hFunctionsMenu, menuFunctionsCrystalID)
  menuOK=uHideCommandItem(hFunctionsMenu, menuFunctionsGroupDelayID)  //ver115-8b
  menuOK=uHideCommandItem(hFunctionsMenu, menuFunctionsMeterID)
  menuOK=uHideCommandItem(hFunctionsMenu, menuFunctionsRLCID)
  menuOK=uHideCommandItem(hFunctionsMenu, menuFunctionsCoaxID)
  menuOK=uHideCommandItem(hFunctionsMenu, menuFunctionsGenerateS21ID)

  //Full menu would look like this:
  //File  Edit  Setup  Options  Data  Functions  OperatingCal  Mode  Multiscan  Two-Port
  //It is possible that Operating Cal, Mode and Two-Port menus were not created, due to limited hardware. In
  //that case their position is zero. It is possible for Operating Cal, Multiscan or Two-Port to be hidden,
  //in which case their flag menuXXXShowing equals zero.

  //Hide Two-Port, Multiscan and Operating Cal, but start at right side
  //so hiding one doesn//t change the position of another.
  */
  //if (vars->menuTwoPortShowing) menuOK=uHideSubMenu(hGraphMenuBar, menuTwoPortPosition);   //ver116-1b
  vars->menuTwoPortShowing=0;
  //if (vars->menuMultiscanShowing) menuOK=uHideSubMenu(hGraphMenuBar, menuMultiscanPosition);   //ver116-1b
  vars->menuMultiscanShowing=0;
    //If we have operating cal menu, hide it
  if (vars->menuOperatingCalShowing)
  {
   // menuOK=uHideSubMenu(hGraphMenuBar, menuOperatingCalPosition)
    vars->menuOperatingCalShowing=0;
  }
    //Multiscan will only be shown for SA without TG, so Operating cal will not be shown.
    //This will move Multiscan left one position.
  if (vars->menuModePosition==0) vars->menuMultiscanPosition=6; else vars->menuMultiscanPosition=7;

    //Two-Port will only be shown for full VNA modes, so Operating cal and Mode will be shown and
    //Multiscan will not. This puts Two-Port after Mode.
  vars->menuTwoPortPosition=vars->menuModePosition+1;

    //We now have bare minimum of menus.
    //Make adjustments from here
  //if (vars->msaMode!="Reflection") menuOK=uShowMenuItem(hFunctionsMenu, menuFunctionsFilterID,0, "Filter Analysis", 0);
  //frontEndID=uMenuItemID(hFileMenu,5);    // "Load Front End" menu is sixth in File menu, which is position 5
  if (vars->msaMode!="SA")
  {
        //menuOK=uGrayMenu(hFileMenu, frontEndID) //Disable Load Front End   //ver115-9d
        //menuOK=uShowMenuItem(hGraphMenuBar, -1, hOperatingCalMenu, "Operating Cal", menuOperatingCalPosition)      //Operating Cal menu
        vars->menuOperatingCalShowing=1;
        //menuOK=uShowMenuItem(hGraphMenuBar, -1, hTwoPortMenu, "Two-Port", menuTwoPortPosition)
        vars->menuTwoPortShowing=1;
    if (vars->msaMode=="Reflection")
    {
      /*
        //Reflection Mode
        menuOK=uShowMenuItem(hDataMenu, menuDataS11ID, 0,"S11 Parameters", 2)  //S11 is third in Data menu
        menuOK=uShowMenuItem(hDataMenu, menuDataS11DerivedID, 0,"S11 Derived Data", 3)
        menuOK=uShowMenuItem(hDataMenu, menuDataLineCalRefID, 0, "Cal Reference", 4)
        menuOK=uShowMenuItem(hDataMenu, menuDataLineCalOSLID, 0, "OSL Info",5)
        menuOK=uShowMenuItem(hOptionsMenu, menuOptionsSmithID, 0, "Smith Chart", 5)    //Smith chart is 6th in Options list
        menuOK=uShowMenuItem(hFunctionsMenu, menuFunctionsMeterID, 0,"Component Meter",0)    //Component meter is first on Functions list
        menuOK=uShowMenuItem(hFunctionsMenu, menuFunctionsRLCID, 0,"RLC Analysis",1)
        menuOK=uShowMenuItem(hFunctionsMenu, menuFunctionsCoaxID, 0, "Coax Parameters", 2)   //Coax is 3rd on Functions list
        menuOK=uShowMenuItem(hFunctionsMenu, menuFunctionsGenerateS21ID, 0, "Generate S21", 3)
            */
    }
    else
    {
      /*
        //Transmission Mode--Scalar or Vector
        menuOK=uShowMenuItem(hFunctionsMenu, menuFunctionsMeterID, 0,"Component Meter",1)    //Component meter is second on Functions list
        menuOK=uShowMenuItem(hFunctionsMenu, menuFunctionsRLCID, 0,"RLC Analysis",2)
        menuOK=uShowMenuItem(hFunctionsMenu, menuFunctionsCrystalID, 0, "Crystal Analysis", 3) //Crystal is 4th on Functions list
        if msaMode$="VectorTrans" then menuOK=uShowMenuItem(hFunctionsMenu, menuFunctionsGroupDelayID, 0, "Group Delay", 4) //ver115-8b
        menuOK=uShowMenuItem(hDataMenu, menuDataS21ID, 0,"S21 Parameters", 2)    //S21 is third in Data menu
        menuOK=uShowMenuItem(hDataMenu, menuDataLineCalID, 0, "Installed Line Cal", 3)
            */
    }
  }
  if (vars->msaMode=="SA")
  {
    //menuOK=uEnableMenu(hFileMenu, frontEndID) //Enable Load Front End   //ver115-9d
  }
  if (vars->msaMode=="SA" && vars->gentrk==0)   //Display Multiscan window only when in SA mode without TG
  {
    //menuOK=uShowMenuItem(hGraphMenuBar, 0, hMultiscanMenu, "Multiscan", menuMultiscanPosition)
    vars->menuMultiscanShowing=1;
    //showID=uMenuItemID(hMultiscanMenu,1)    // "Show Multiscan" menu
    //quitID=uMenuItemID(hMultiscanMenu,2)    // "Quit Multiscan" menu
        //If multiscan is open, allow user to Show or Quit
    if (vars->multiscanIsOpen)
    {
     //   menuOK=uEnableMenu(hMultiscanMenu, showID) //Enable Show
     //   menuOK=uEnableMenu(hMultiscanMenu, quitID) //Enable Quit
    }
    else
    {
     //   menuOK=uGrayMenu(hMultiscanMenu, showID) //Gray Show
     //   menuOK=uGrayMenu(hMultiscanMenu, quitID) //Gray Quit
    }
  }

  //call uDrawMenu hGraphWindow
  vars->menuMode=vars->msaMode;  //We are now conformed to the current mode.
}

void MainWindow::mMarkToCenter()
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

void MainWindow::menuExpandSweep()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[menuExpandSweep]
    call Expand
    if haltsweep then
        continueCode=3 : goto [PostScan]
    else
        goto [Restart]
    end if
*/
}

void MainWindow::Expand()
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

void MainWindow::btnRedraw()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub btnRedraw btn$  'Redraw button was pushed
    'haltsweep=1 if scan is in progress, so we set flag to halt sweeping on return from the "scan"
    'command that enabled this button to be handled.
    if haltsweep=1 then continueCode=1 : exit sub   'Signal to halt after "scan" command
    call mDeleteMarker "Halt"    'Delete Halt marker ver114-4c
    if smithGraphHndl$()<>"" then call smithDrawChart    'To recreate bitmap of background, just in case it is messed up ver115-2c
    refreshGridDirty=1 : call RefreshGraph 0    'ver114-7d
end sub
*/
}
void MainWindow::RedrawGraph(int restoreErase)
{
  //Redraw all layers of the graph
  //Redraw entire graph from scratch, using the point values previously accumulated.

  graph.refreshRedrawFromScratch=1;
  graph.RefreshGraph(restoreErase);
}
void MainWindow::RememberState()
{

  //Record state before loading a context, to allow detection of certain changes
  //We want to record enough to determine whether we have to restart, do a full redraw or just
  //refresh and redraw setup info.
  //Some of these changes can be made during a scan without an immediate redraw, but when loading
  //a context we will force a halt.

  //Change to msaMode$ may reqire a new graph window
  vars->prevMSAMode=vars->msaMode;

    //Changes to these will require a complete Restart
  vars->prevFreqMode=vars->freqBand;       //ver115-1c
  vars->prevPath=vars->path; //ver116-4j
  graph.gGetXAxisRange(vars->prevStartF, vars->prevEndF); vars->prevBaseF=vars->baseFrequency;
  graph.gGetIsLinear(vars->prevXIsLinear, vars->prevY1IsLinear, vars->prevY2IsLinear);
  vars->prevSteps=vars->globalSteps;
  vars->prevSweepDir=graph.gGetSweepDir(); vars->prevAlternate=vars->alternateSweep;
  vars->prevGenTrk=vars->gentrk ; vars->prevSpurCheck=vars->spurcheck; //ver114-6k
  vars->prevTGOff=vars->offset ; vars->prevSGFreq=vars->sgout; //ver115-1a
  vars->prevPath=vars->path; //ver115-1a

    //Changes to these will require calling gCalcGraphParams and then a full Redraw
  graph.gGetYAxisRange(1, vars->prevStartY1, vars->prevEndY1);
  graph.gGetYAxisRange(2, vars->prevStartY2, vars->prevEndY2);
  graph.gGetNumDivisions(vars->prevHorDiv, vars->prevVertDiv);
  vars->prevSwitchFR=vars->switchFR;   //forward/reverse switch affects axis labeling ver116-1b

    //Changes to graph data type will require a replotting to get the proper transform,
    //and redrawing from scratch to get axis labeling correct.
  vars->prevY1DataType=vars->Y1DataType;  //ver115-1b deleted source constants
  vars->prevY2DataType=vars->Y2DataType;

  //If in a mode where these are relevant, changes to the following are treated
  //the same as a change to the data source.   //ver114-7e added these
  vars->prevS21JigAttach=vnaCal.S21JigAttach;
  vars->prevS21JigR0=vnaCal.S21JigR0;   //ver115-1e
  vars->prevS21JigShuntDelay=vnaCal.S21JigShuntDelay;   //ver115-1f
  vars->prevS11BridgeR0=vnaCal.S11BridgeR0;
  vars->prevS11GraphR0=vnaCal.S11GraphR0;
  vars->prevS11JigType=vnaCal.S11JigType; //ver115-1b

    //Changes to these require a full Redraw from existing Y values
    //Note this refers to magtable data, not graph data
  vars->prevDataChanged=0;   //If changed, this has to be set elsewhere

     //Changes to these may require redraw from scratch or just normal refresh
  vars->prevY1Disp=vars->Y1DisplayMode;
  vars->prevY2Disp=vars->Y2DisplayMode;

            //If auto scale gets turned on, we will signal to do autoscale at end of sweep,
            //or do it immediately if we are halted.
  vars->prevAutoScaleY1=graph.autoScaleY1;
  vars->prevAutoScaleY2=graph.autoScaleY2;
    //Changing planeadj could require immediate recalc of existing data
  vars->prevPlaneAdj=vars->planeadj;   //ver114-7f
    //If none of the above are changed, we will do a RefreshGraph with refreshGridDirty=1
  //Items that reach here include;
  //Marker Options, Graph Appearance, trace width and trace color
  //RBW, video filter, SigGen freq, TG offset, spur test, wait time and plane extension

}
void MainWindow::DetectFullChanges()
{
  //Take appropriate redraw/restart action in response to certain changes
  //This is a gosub routine so it can access [ChangeMode]. This needs to be called only if msaMode$ may have changed;
  //otherwise call DetectChanges directly.
  int doRestart=0;
  if (vars->prevMSAMode!=vars->msaMode)
  {
    ChangeMode();
    doRestart=1;
  }
  DetectChanges(doRestart);
}


void MainWindow::ToggleTransmissionReflection()
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

  setCursor(Qt::WaitCursor);

  int restoreSettingsAfterChange=0;
  if (vars->menuMode=="VectorTrans")  //menuMode$ has prior mode ver116-1b
  {
    //If changing from vector trans mode and sweep frequencies are the same, we preserve some settings
    if (vars->refLastSteps!=0 && vars->refLastSteps==vars->steps && vars->refLastStartFreq==vars->startfreq
        && vars->refLastEndFreq==vars->endfreq && vars->refLastIsLinear==graph.gGetXIsLinear()) restoreSettingsAfterChange=1;
  }
  else    //changing from reflection to transmission
  {
    if (vars->transLastSteps!=0 && vars->transLastSteps==vars->steps && vars->transLastStartFreq==vars->startfreq
        && vars->transLastEndFreq==vars->endfreq && vars->transLastIsLinear==graph.gGetXIsLinear()) restoreSettingsAfterChange=1;
  }

  if (vars->msaMode=="Reflection") vars->msaMode="VectorTrans"; else vars->msaMode="Reflection"; //switch mode
  ChangeMode();  //Conform to new mode
  if (restoreSettingsAfterChange==0) { setCursor(Qt::ArrowCursor); ; return;}

  if (vars->msaMode=="Reflection")    //These changes will be fully implemented at PartialRestart
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
  CopyModeDataToVNAData(1);  //Copy data to be restored to VNAData
  vars->VNARestoreDoR0AndPlaneExt=1; RestoreVNAData();   //load and graph the data ver116-4j

  vars->VNAData.mresize(2,3);  //save space
  vars->VNADataNumSteps=1;
  setCursor(Qt::ArrowCursor);
  util.beep();

}
void MainWindow::CopyModeDataToVNAData(int doIntermed)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Copy current data to VNAData, VNATitle$, etc. 'ver116-4a
            'We save all steps, even if they have not gathered any info yet
            'If doIntermed=1, we copy the intermediate (pre-R0 and plane ext) data for S21 or S11.
    redim VNAData(globalSteps, 2)   'Allow for freq, db and ang
    for i=0 to globalSteps
        select case msaMode$
            case "SA"
                f=ActualSignalFrequency(datatable(i,1), datatable(i,4)) 'true freq in MHz, not what hardware thinks it tuned to ver116-4m 'ver116-4s
                db=datatable(i,2)   'dBm
                ang=0
            case "ScalarTrans"
                f=S21DataArray(i,0) 'true freq
                db=S21DataArray(i,1)   'dB
                ang=0
            case "VectorTrans"
                f=S21DataArray(i,0) 'true freq
                db=S21DataArray(i,1)   'dB
                if doIntermed then ang=S21DataArray(i,2) : ang=S21DataArray(i,2)    'deg, intermediate or final ver116-4j
            case else   'Reflection
                f=ReflectArray(i,0) 'true freq
                if doIntermed then  'ver116-4j
                    db=ReflectArray(i,constIntermedS11DB) : ang=ReflectArray(i,constIntermedS11Ang)   'intermed db and deg
                else
                    db=ReflectArray(i,constGraphS11DB) : ang=ReflectArray(i,constGraphS11Ang)    'final db and deg
                end if
        end select
        VNAData(i,0)=f : VNAData(i,1)=db : VNAData(i,2)=ang
    next i
    for i=1 to 4: VNADataTitle$(i)=gGetTitleLine$(i) : next i
    VNADataNumSteps=globalSteps : VNADataLinear=gGetXIsLinear()
        'In SA mode we will end up with a bogus value for VNADataZ0, but it is meaningless anyway.
    if msaMode$="Reflection" then VNADataZ0=S11GraphR0 else VNADataZ0=S21JigR0
    */
}
void MainWindow::ChangeMode()
{

  //Change mode to msaMode$
  //menuMode$ indicates the mode to which the menus are currently conformed, which is the mode we are changing from.
  //The very first time, we create the window at a standard size. After that, we work with the existing
  //window and redo the menus, leaving the size and location as is.
  if (vars->multiscanIsOpen) multiscanCloseAll();  //Quit multiscan--it is for SA only
  smithFinished("");   //Close smith chart if it is open ver115-1b
  if (activeConfig.hasVNA==0 && (vars->msaMode=="VectorTrans" || vars->msaMode=="Reflection")) { vars->msaMode="SA"; SetDefaultGraphData();} //ver115-4f
  if (activeConfig.TGtop==0) { vars->msaMode="SA"; SetDefaultGraphData();} //ver115-4e

  if (vars->msaMode == "ScalarTrans" || vars->msaMode == "VectorTrans")
    GoTransmissionMode();
  else if (vars->msaMode == "Reflection")
    GoReflectionMode();
  else
    GoSAmode();

  autoWaitPrecalculate(); //auto wait calculations depend on mode ver116-1b
  graph.haltsweep=0; //So Restart will actually restart
}
void MainWindow::RestartSATGmode()
{
  //menu for SA plus TG mode selected added ver115-4f
  vars->gentrk=1;    //Tracking gen on
  RestartSAmodes();
}
void MainWindow::RestartPlainSAmode()
{
  //menu for SA mode selected added ver115-4f
  vars->gentrk=0;    //Tracking gen off
  RestartSAmodes();
}

void MainWindow::RestartSAmodes()
{
  //Restart in MSA mode with or without TG. Renamed ver115-4f
  //called by the menu handlers for SA and SA/TG modes
  //For a menu-initiated change, we initialize certain variables
  //if the Special Tests Window is open, close it.

  if (special==1) { CloseSpecial(1); } //ver112-2f
  vars->msaMode="SA";  //ver115-2a
  SetDefaultGraphData();    //clears autoscale, sets Y1 and Y2 data types and range, and sets Y2DisplayMode and Y1DisplayMode ver115-3b
  vars->sgout=10;    //Set to 10 MHz no matter what band we are using. Will be irrelevant if in TG mode ver116-4p
  vars->offset=0;    //TG offset; will be irrelevant if in TG mode. ver115-4f
  vars->spurcheck = 0; //this assures Spur Test is OFF. ver114-2e
  graph.referenceLineSpec=""; graph.referenceLineType=0;
  ChangeMode();
  Restart();
}
void MainWindow::GoSAmode()
{
  //Switch to MSA mode and return; Get here only from [ChangeMode]
  //We don't initialize variables here because they may have been set by loading Preferences ver115-2a
  /*if (graph.graphBox=="")  //See if window is created yet ver115-5d
  {
    CreateGraphWindow();   //Note msaMode$ is new mode; menuMode$ is old mode
  }
  else*/
  {
    ConformMenusToMode(); //ver115-9a
    if (vars->menuMode!= vars->msaMode) //menuMode$ has prior mode, to which menus are now conformed ver115-5d
    {
      mClearMarkers();
      graph.gClearAllReferences();
      graph.gSetNumPoints(0);
    }
  }
}
void MainWindow::RestartTransmissionMode()
{
  //Restart in Transmission mode. Handles menu item for mode change
  //if the Special Tests Window is open, close it.
  if (special==1)
  {
    //returnflag=1;
    CloseSpecial(1);
    //returnflag=0;
  } //ver112-2f
  //For a menu-initiated change, we initialize certain variables ver115-2a
  vars->gentrk = 1;
  vars->normrev = 0;
  vars->offset = 0;  //turn on tracking generator, normal, zero offset //ver111-17
  graph.referenceLineSpec="";
  graph.referenceLineType=0;

  //Switching between VectorTrans and Reflection modes is specially handled, to preserve the data and Y-axis
  //settings that were last in effect for the new mode. This special treatment is done for menu-driven change
  //or by certain internally generated changes that call [ToggleTransmissionReflection], but not for changes
  //resulting from loading of preference files.   ver116-1b
  if (vars->menuMode=="Reflection" && vars->transLastSteps!=0)
  {
    ToggleTransmissionReflection();
    return;
    //wait
  }
  if (activeConfig.hasVNA)
  {
    vars->msaMode="VectorTrans";
  }
  else
  {
    vars->msaMode="ScalarTrans";
  }
  SetDefaultGraphData();    //clears autoscale, sets Y1 and Y2 data types and range, and sets Y2DisplayMode and Y1DisplayMode ver115-3b
  vnaCal.S21JigAttach="Series";  //Start using series jig ver115-5a
  ChangeMode();
  Restart();
}
void MainWindow::GoTransmissionMode()
{
  //Switch to Transmission mode and return; Get here only from [ChangeMode]
  //We don't initialize variables here because they may have been set by loading Preferences ver115-2a
  vars->spurcheck = 0; //this assures Spur Test is OFF. ver116-1b
  vars->switchTR=0;
  SelectLatchedSwitches(vars->freqBand); //Set transmission/reflection switch to transmission //ver116-1b ver116-4s
  /*if (graph.graphBox=="") //See if window is created yet ver115-5d
  {
    CreateGraphWindow();   //Note msaMode$ is new mode; menuMode$ is old mode
  }
  else*/
  {
    int smoothModeChange=0;
    if (vars->menuMode=="Reflection")   //menuMode$ has prior mode ver116-1b
    {
      //If changing from reflection mode and sweep frequencies are the same, we preserve some settings
      if (vars->transLastSteps!=0
          && vars->transLastSteps==vars->steps
          && vars->transLastStartFreq==vars->startfreq
          && vars->transLastEndFreq==vars->endfreq
          && vars->transLastIsLinear==graph.gGetXIsLinear())
      {
        smoothModeChange=1;
      }
    }
    if (smoothModeChange==0)
    {
      //We don't clear markers when changing between VectorTrans and Reflection if sweep freq is the same
      mClearMarkers();
    }
    ConformMenusToMode();
    graph.gSetNumPoints(0);
    graph.gClearAllReferences();    //Old ones may not make sense
    vnaCal.SignalNoCalInstalled();   //ver116-4b
  }
}

void MainWindow::RestartReflectionMode()
{
  //Restart in Reflection mode. Handles menu item for mode change
  //if the Special Tests Window is open, close it.
  if (special==1)
  {
    //fix me returnflag=1;
    CloseSpecial();
    // fix me returnflag=0; //ver112-2f
  }
  //For a menu-initiated change, we initialize certain variables ver115-2a
  vars->gentrk = 1;
  vars->normrev = 0;
  vars->offset = 0;  //turn on tracking generator, normal, zero offset //ver111-17

  graph.referenceLineSpec=""; graph.referenceLineType=0;

  //Switching between VectorTrans and Reflection modes is specially handled, to preserve the data and Y-axis
  //settings that were last in effect for the new mode. This special treatment is done for menu-driven change
  //or by certain internally generated changes that call [ToggleTransmissionReflection], but not for changes
  //resulting from loading of preference files.   ver116-1b
  //if graphBox$<>"" and menuMode$="VectorTrans" and refLastSteps<>0 then
  if (vars->menuMode=="VectorTrans" && vars->refLastSteps!=0)
  {
    ToggleTransmissionReflection();
    return; // : wait
  }
  vars->msaMode="Reflection"; //ver115-2a
  SetDefaultGraphData();    //clears autoscale, sets Y1 and Y2 data types and range, and sets Y2DisplayMode and Y1DisplayMode ver115-3b
  vnaCal.S11JigType="Reflect";   //Start using bridge ver115-5a
  ChangeMode();
  Restart();
}
void MainWindow::GoReflectionMode()
{

  //Switch to Reflection mode and return; Get here only from [ChangeMode]
  //We don't initialize variables here because they may have been set by loading Preferences ver115-2a
  vars->spurcheck = 0; //this assures Spur Test is OFF. ver116-1b
  vars->switchTR=1;
  SelectLatchedSwitches(vars->freqBand); //Set transmission/reflection switch to reflection //ver116-1b ver116-4s
  //if graphBox$="" then //See if window is created yet ver115-5d
  //        gosub [CreateGraphWindow]   //Note msaMode$ is new mode; menuMode$ is old mode
  //    else
  int smoothModeChange=0;
  if (vars->menuMode=="VectorTrans")  //menuMode$ has prior mode ver116-1b
  {
    //If changing from vector transmission mode and sweep frequencies are the same, we preserve some settings
    if (vars->refLastSteps!=0 && vars->refLastSteps==vars->steps
        && vars->refLastStartFreq==vars->startfreq
        && vars->refLastEndFreq==vars->endfreq
        && vars->refLastIsLinear==graph.gGetXIsLinear())
    {
      smoothModeChange=1;
    }
  }
  if (smoothModeChange==0)
  {
    //We don't clear markers when changing between VectorTrans and Reflection if sweep freq is the same
    mClearMarkers();
  }
  ConformMenusToMode();
  graph.gSetNumPoints(0);
  graph.gClearAllReferences();    //Old ones may not make sense
  vnaCal.SignalNoCalInstalled();   //ver116-4b
  //    end if
  //S21JigR0 is sometimes referenced if we explicitly use the series or shunt jig. But it causes problems
  //in reflection mode if it can have a different value from S11BridgeR0.
  vnaCal.S21JigR0=vnaCal.S11BridgeR0;    //ver116-4j
  vars->suppressPhase=0; //Reflection always needs phase, even if not displayed ver116-1b
  // fix me smithOpenChart(); //Create smith chart
}

void MainWindow::SetYAxes(int data1, int top1, int bot1, int auto1, int data2, int top2, int bot2, int auto2)
{
  //Set datatypes, axis top and bottom, and autoscale //ver116-1b
  //Note gCalcGraphParams must be called to fully update graph module
  graph.autoScaleY1=auto1;  graph.autoScaleY2=auto2;
  vars->Y1DataType=data1;
  vars->Y2DataType=data2;
  graph.SetY1Range(bot1, top1);
  graph.SetY2Range(bot2, top2);
  vars->Y1DisplayMode=1; vars->Y2DisplayMode=1; //ver115-4e
  graph.gSetDoAxis((vars->Y1DataType!=vars->constNoGraph), (vars->Y2DataType!=vars->constNoGraph));  //Tell graph module whether we have graphs ver115-2c
  ImplementDisplayModes();

}
void MainWindow::SetDefaultGraphData()
{
//  Set data type and range for default data, and Y2DisplayMode and Y1DisplayMode based on msaMode$
//  mod by 116-4h
  graph.autoScaleY1=0; graph.autoScaleY2=0;
  int min1, max1, min2, max2;
  GetDefaultGraphData(1, vars->Y1DataType, min1, max1);
  GetDefaultGraphData(2, vars->Y2DataType, min2, max2);
  graph.SetY1Range(min1, max1); graph.SetY2Range(min2, max2);
  vars->Y1DisplayMode=1; vars->Y2DisplayMode=1; //ver115-4e
  ImplementDisplayModes();

}
void MainWindow::GetDefaultGraphData(int axisNum, int &axisType, int &axisMin, int &axisMax)
{
  //get axis default data type and range for this mode ver116-4h
  if (vars->msaMode== "Reflection")
  {
    if (vars->primaryAxisNum==axisNum)
    {
      axisType=vars->constGraphS11DB ; axisMin=-100 ; axisMax=0;
    }
    else
    {
      axisType=vars->constGraphS11Ang ; axisMin=-180 ; axisMax=180;
    }
  }
  else if (vars->msaMode== "ScalarTrans")
  {
    if (vars->primaryAxisNum==axisNum)
    {
      axisType=vars->constMagDB ; axisMin=-100 ; axisMax=0;
    }
    else
    {
      axisType=vars->constNoGraph ; axisMin=-180 ; axisMax=180;
    }
  }
  else if (vars->msaMode== "VectorTrans")
  {
    if (vars->primaryAxisNum==axisNum)
    {
      axisType=vars->constMagDB ; axisMin=-100 ; axisMax=0;
    }
    else
    {
      axisType=vars->constAngle ; axisMin=-180 ; axisMax=180;
    }
  }
  else   // "SA"
  {
    if (vars->primaryAxisNum==axisNum)
    {
      axisType=vars->constMagDBM ; axisMin=-100 ; axisMax=0;
    }
    else
    {
      axisType=vars->constNoGraph ; axisMin=-1 ; axisMax=0;
    }
  }
}
void MainWindow::ResizeArrays(int nPoints)
{
  //Resize the arrays to hold at least nPoints points
  int maxPoints=qMax(nPoints+10, 802);
  if (graph.gMaxNumPoints()<maxPoints) //Redimension only if nPoints goes up ver114-5p
  {
    graph.gSetMaxPoints(maxPoints);     //Resize arrays in graph module

    vars->datatable.mresize(maxPoints,5);    //added element for band ver116-4s
    vars->magarray.mresize(maxPoints,4);
    vars->phaarray.mresize(maxPoints,5);
    vars->lineCalArray.mresize(maxPoints,3);
    graph.referenceSource.mresize(maxPoints, 3); //ver114-7f
    graph.referenceTransform.mresize(maxPoints, 3); //ver114-7f
    vars->PLL1array.mresize(maxPoints,49);
    vars->PLL3array.mresize(maxPoints,49);
    vars->DDS1array.mresize(maxPoints,47);
    vars->DDS3array.mresize(maxPoints,47);
    vars->cmdallarray.mresize(maxPoints,40);

    usb->resizeMemory(maxPoints);

    vars->freqCorrection.resize(maxPoints);
    vars->frontEndCorrection.resize(maxPoints); //ver115-9c
    vars->ReflectArray.mresize(maxPoints,17);   //ver115-2d
    vars->S21DataArray.mresize(maxPoints, 4);  //ver116-1b
    vars->bandLineCal.mresize(maxPoints, 3);  //ver114-5f
    vars->OSLa.mresize(maxPoints, 2);  //ver115-1b
    vars->OSLb.mresize(maxPoints, 2);  //ver115-1b
    vars->OSLc.mresize(maxPoints, 2);  //ver115-1b
    vars->OSLstdOpen.mresize(maxPoints,2);  //ver115-1b
    vars->OSLstdLoad.mresize(maxPoints,2);  //ver115-1b
    vars->OSLcalOpen.mresize(maxPoints,2);  //ver115-1b
    vars->OSLcalLoad.mresize(maxPoints,2);  //ver115-1b
    vars->OSLcalShort.mresize(maxPoints,2);  //ver115-1b
    vars->OSLBandA.mresize(maxPoints,2); vars->OSLBandB.mresize(800,2); vars->OSLBandC.mresize(800,2);       //ver115-4a
    vars->OSLBandRef.mresize(maxPoints,3);        //ver115-4a
    vars->auxGraphData.mresize(maxPoints, 6);    //ver115-4a

    //Note we do not resize arrays for base Line or base OSL cal, because resizing will invalidate the data, and
    //because base line cal is saved to/retrieved from a file so its max size needs to be known before retrieval.
    //Plus the nature of base cal does not require a massive number of points.
    inter.intSetMaxNumPoints(maxPoints);  //ver114-5q

    //ver115-1d put the loading of the cal file inside the if.. block
    //Load BaseLine Cal file if it exists ver114-5m
    if (activeConfig.TGtop>0)
    {
      CreateOperatingCalFolder();  //Create OperatingCal folder if it does not exist
      LoadBaseLineCalFile();       //Load BaseLine file if it exists; if not we don't care
      vnaCal.bandLineNumSteps=-1; //Indicate no data; we just erased it
      InstallSelectedLineCal();     //To restore line cal data
    }
  }
  //Note we don't have to resize configarray or configLineCalPoints$(); a flexible number of points
  //is handled by simply having the strings in configLineCalPoints$ be different lengths.
  //We don't resize VNAData or uWorkArray, because they are resized when used.
}

void MainWindow::LeftButDouble()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[LeftButDouble] 'Left button was double-clicked at MouseX, MouseY
   'If the double click was on the grid line values for a Y axis, then open
   'a preference window for that axis. If on the title, open a window to edit the title.
   'If in the grid, place a marker. If on the X-axis window or the sweep parameters text,
   'open x-axis preference window.
    if gPixIsInGrid(MouseX, MouseY)=1 then goto [MarkerClick]  'ver114-6d ver115-1a

    call gGetGridCorner "LL", axis1Right, axis1Bot   'Lower left corner of grid
    call gGetGridCorner "LR", axis2Left, axis2Bot    'Lower right corner of grid
    axis1Left=graphMarLeft-50 : axisTop=graphMarTop-5
    axis2Right=axis2Left+45
    if MouseX>axis1Right-5 and MouseX<axis2Left+5 and MouseY>axis1Bot and MouseY<axis1Bot+40 then
        'Double-click was on grid values for frequency axis
        goto [FreqAxisPreference]   'will end up waiting, continuing or restarting  ver114-6d
    end if
    'ver114-6d Double-click on sweep params text opens the Freq Axis preference
    if MouseX>axis2Right+5 and MouseY>axis2Top and MouseY<min(axis2Bot-10, axis2Top+300) then
        'Double-click was on sweep parameter text
        goto [FreqAxisPreference]   'will end up waiting, continuing or restarting  ver114-6d
    end if
    yAxisChange=0   'ver114-5c
    if MouseX>=axis1Left and _
                        MouseX<=axis1Right and MouseY<=axis1Bot+5 and MouseY>=axisTop-5 then    'ver115-3b
        'Double-click was on grid values for Y1
        if haltsweep=1 then gosub [FinishSweeping]  'ver115-8d
        call DisplayAxisYPreference 1, 0 : yAxisChange=1
    end if
    if MouseX>=axis2Left and MouseX<=axis2Right and MouseY<=axis2Bot+5 and MouseY>=axisTop-5 then
        'Double-click was on grid values for Y2
        if haltsweep=1 then gosub [FinishSweeping]  'ver115-8d
        call DisplayAxisYPreference 2, 0 : yAxisChange=1
    end if
    'ver114-6d revised the redrawing procedure when the Y axis changes
    if yAxisChange then 'ver114-5c Moved this here from DisplayYAxisPreference
        useExpeditedDraw=gCanUseExpeditedDraw()
        if continueCode=3 then suppressHardwareInitOnRestart=1 : gosub [PartialRestart] 'ver116-4d    no need for hardware init
        if multiscanIsOpen then call multiscanSaveContexts 0 'zero means main graph  ver115-8d
    end if
    if MouseY<axisTop-5 and MouseX>axis1Right and MouseX<axis2Left then
        'Double-click was in title area
        call DisplayTitleWindow 0   'open dialog to edit graph title
    end if
    'If in midsweep, continue scan. Note that a subroutine called above may have set
    'continueCode to cause a halt or restart.
    if haltsweep=1 then goto [PostScan]
    wait
*/
}

void MainWindow::preupdatevar()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[preupdatevar] 'ver111-36h
    remember = thisstep
    thisstep = leftstep 'ver115-1a
    gosub [updatevar]
    thisstep = remember
    return
*/
}

void MainWindow::RightButDown()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[RightButDown] 'when right mouse button is pressed, do the following
'Currently does nothing ver115-1a
    if haltsweep=1 then goto [PostScan]     'SEWgraph0
    wait
*/
}

void MainWindow::MarkerClick()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[MarkerClick]   'Add marker at clicked point
    call gFindClickedPoint MouseX, MouseY, clickedPointNum, clickedTraceNum  'Point numbers run 1...
    if clickedPointNum>0 then 'Click was not left or right of graphed points
        if selMarkerID$="" then newMarkID$="L" else newMarkID$=selMarkerID$
        'if gGetDoHist() or interpolateMarkerClicks=0 then clickedPointNum=int(clickedPointNum+0.5)   'Round to integral point ver115-1a
        roundedClickPointNum=int(clickedPointNum+0.5)   'ver116-4k
        if msaMode$="SA" then clickedPointNum=roundedClickPointNum   'Round to integral point for SA mode ver115-2d
        'If we are within one-half pixel of a integral point, we round off because it is nice to have integral points,
        'and there is no purpose to trying to get extreme resolution.
        if abs(clickedPointNum-roundedClickPointNum)*gPixelsPerStep()<=0.5 then clickedPointNum=roundedClickPointNum    'ver116-4k
        call mAddMarkerAndRedraw newMarkID$,clickedPointNum, clickedTraceNum
        if twoPortWinHndl$="" and varwindow = 1 then leftstep=roundedClickPointNum-1 : gosub [preupdatevar] 'will update variables at marker point ver116-4j
    end if
    'If a sweep is in progress return to the instruction after the scan command.
    if haltsweep=1 then goto [PostScan]     'SEWgraph0
    wait
*/
}
void MainWindow::StartingLimits()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub StartingLimits graph.componConst, oldData, byref axisMin, byref axisMax 'Determine starting min and max for axes
    'If the axis limits are unreasonable, we find better limits
    'graph.componConst specifies what type of value we are dealing with. E.g. constTheta.
    'oldData is the data type before the change.
    'axisMin and axisMax are the original limits; we change if necessary.
    'This is applied only when changing graph types.
    'In general we just pick standard limits. But if the old data type is related to the new one, we may take
    'into account the old limits.
    'If the user specifies goofy values without changing the data type, or after doing so, we go with his specs.
    absMin=abs(axisMin) : absMax=abs(axisMax)
    limitsDone=0
    select graph.componConst
        case constGraphS11DB,constMagDBM, constMagDB   'db and dbm values
            axisMax=0 : axisMin=-100

        case constReturnLoss,constInsertionLoss  'Positive dB values
            axisMin=0 : axisMax=60

        case constGraphS11Ang, constTheta    'Two names for the same angles
            axisMin=-180 : axisMax=180  'Expand to full range  'ver114-8c
            if oldData<>constGraphS11Ang and oldData<>constTheta then axisMin=-180 : axisMax=180
            'If we changed from a similar data type, keep the existing limits if they are reasonable
            if axisMin>=axisMax then axisMin=-180 : axisMax=180

        case constAngle, constRawAngle, constImpedAng, constAdmitAng    'angles 'ver115-1i added raw angle
            axisMin=-180 : axisMax=180

        case constAdmitMag  'ver115-4a
            axisMin=0 : axisMax=1

        case constConductance, constSusceptance   'ver115-4a
            axisMin=-1 : axisMax=1

        case constSerR, constParR, constImpedMag    'resistances and non-negative impedance
            if oldData<>constSerR and oldData<>constParR and oldData<>constImpedMag then axisMin=0 : axisMax=200
            'If we changed from a similar data type, keep the existing limits if they are reasonable
            if axisMin<0 or axisMax<0 or axisMin>=axisMax then axisMin=0 : axisMax=200

        case constSerReact, constParReact    'reactances
            if oldData<>constSerReact and oldData<>constParReact then axisMin=-200 : axisMax=200
            'If we changed from a similar data type, keep the existing limits if they are reasonable
            if axisMin>=axisMax then axisMin=-200 : axisMax=200

        case constMagV, constMagRatio, constMagV, constRho 'Fractional values that won't exceed 1 or be negative
            axisMin=0 : axisMax=1

         case constMagWatts 'Fractional value that won't be negative and likely won't exceed 0.001
            axisMin=0 : axisMax=0.001

        case constSerC, constParC     'Capacitance
            if oldData<>constSerC and oldData<>constParC then axisMin=0 : axisMax=0.000001
            'If we changed from a similar data type, keep the existing limits if they are reasonable
            if axisMin>=axisMax then axisMin=0 : axisMax=0.000001

        case constSerL, constParL     'Inductance
            if oldData<>constSerL and oldData<>constParL then axisMin=0 : axisMax=0.000001
            'If we changed from a similar data type, keep the existing limits if they are reasonable
            if axisMin>=axisMax then axisMin=0 : axisMax=0.000001

        case constSWR   'VSWR >=1 and we don't care about huge values ver114-8d
            axisMin=1 : axisMax=11

        case constComponentQ   '>=0 and we don't care about huge values ver115-2d
            axisMin=0: axisMax=100

        case constReflectPower   'a percentage ver115-2d
            axisMin=0 : axisMax=100

        case constAux0, constAux1, constAux2, constAux3, constAux4, constAux5 'ver115-4a
            auxNum=graph.componConst-constAux0    'e.g. constAux4 produces 4
            axisMin=auxGraphDataInfo(auxNum,1)  : axisMax=auxGraphDataInfo(auxNum,2)

        case constNoGraph   'ver115-2c
            'Do nothing
        case else       'constGD and anything we missed
            'don't mess with these
    end select
end sub
*/
}



void MainWindow::FillRegularGraphData(int axisNum)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function FillRegularGraphData(axisNum) 'Fill axisGraphData$ and axisDataType for regular scan graphs; return number of graphs
    'Added by ver116-1b
    for i=0 to 40 : axisGraphData$(i)="" : next i
    if msaMode$="SA" then
        axisGraphData$(0)="Magnitude (dBm)" : axisDataType(0)=constMagDBM
        axisGraphData$(1)="Magnitude (Watts)" : axisDataType(1)=constMagWatts
        axisGraphData$(2)="Magnitude (Volts)" : axisDataType(2)=constMagV
        if (axisNum=1 and Y2DataType<>constNoGraph) or (axisNum=2 and Y1DataType<>constNoGraph) then _
                        axisGraphData$(3)="None" : axisDataType(3)=constNoGraph   'ver115-3b
        numGraphs=4   'ver115-4a
    end if
    if msaMode$="ScalarTrans" then
        axisGraphData$(0)="Transmission (dB)": axisDataType(0)=constMagDB   'ver115-4f
        axisGraphData$(1)="Transmission (Ratio)" : axisDataType(1)=constMagRatio    'ver115-4f
        axisGraphData$(2)="Insertion Loss (db)" : axisDataType(2)=constInsertionLoss
        numGraphs=3   'ver116-1b
        if (axisNum=1 and Y2DataType<>constNoGraph) or (axisNum=2 and Y1DataType<>constNoGraph) then _
                        axisGraphData$(3)="None" : axisDataType(3)=constNoGraph: numGraphs=numGraphs+1 'ver116-1b
    end if

    if msaMode$="VectorTrans" then
        axisGraphData$(0)="S21 Magnitude (dB)" :axisDataType(0)=constMagDB
        axisGraphData$(1)="S21 Phase Angle" : axisDataType(1)=constAngle
        axisGraphData$(2)="Raw Power (dBm)" : axisDataType(2)=constMagDBM
        axisGraphData$(3)="Raw Phase Angle" : axisDataType(3)=constRawAngle
        axisGraphData$(4)="Insertion Loss (db)" : axisDataType(4)=constInsertionLoss
        axisGraphData$(5)="S21 Group Delay" : axisDataType(5)=constGD
        numGraphs=6   'ver116-1ba
            'Display "None" only if the other axis is not set to None.
        if (axisNum=1 and Y2DataType<>constNoGraph) or (axisNum=2 and Y1DataType<>constNoGraph) then _
                        axisGraphData$(6)="None" : axisDataType(6)=constNoGraph: numGraphs=numGraphs+1 'ver116-1b
    end if
        'ver115-2d changed some reflection graph names
    if msaMode$="Reflection" then   'ver115-1f added impedance and renumbered
        axisGraphData$(0)="S11 Magnitude (dB)" : axisDataType(0)=constGraphS11DB
        axisGraphData$(1)="S11 Phase Angle (Deg)" : axisDataType(1)=constGraphS11Ang
        axisGraphData$(2)="Reflect Coef. Mag (Rho)" : axisDataType(2)=constRho
        axisGraphData$(3)="Reflect Coef. Angle (Theta)" : axisDataType(3)=constTheta
        axisGraphData$(4)="Impedance Mag (Z Mag)" : axisDataType(4)=constImpedMag
        axisGraphData$(5)="Impedance Angle (Z Ang)" : axisDataType(5)=constImpedAng
        axisGraphData$(6)="Series Resistance (Rs)" : axisDataType(6)=constSerR
        axisGraphData$(7)="Series Reactance (Xs)" : axisDataType(7)=constSerReact
        axisGraphData$(8)="Series Capacitance (Cs)" : axisDataType(8)=constSerC
        axisGraphData$(9)="Series Inductance (Ls)" : axisDataType(9)=constSerL
        axisGraphData$(10)="Parallel Resistance (Rp)" : axisDataType(10)=constParR
        axisGraphData$(11)="Parallel Reactance (Xp)" : axisDataType(11)=constParReact
        axisGraphData$(12)="Parallel Capacitance (Cp)" : axisDataType(12)=constParC
        axisGraphData$(13)="Parallel Inductance (Lp)"  : axisDataType(13)=constParL
        axisGraphData$(14)="Return Loss (db)" : axisDataType(14)=constReturnLoss
        axisGraphData$(15)="Reflected Power (%)" : axisDataType(15)=constReflectPower 'ver115-2d
        axisGraphData$(16)="Component Q" : axisDataType(16)=constComponentQ 'ver115-2d
        axisGraphData$(17)="VSWR" : axisDataType(17)=constSWR
        numGraphs=18   'ver116-1b
            'Display "None" only if the other axis is not set to None.
        if (axisNum=1 and Y2DataType<>constNoGraph) or (axisNum=2 and Y1DataType<>constNoGraph) then _
                axisGraphData$(18)="None" : axisDataType(18)=constNoGraph : numGraphs=numGraphs+1 'ver116-1b

        'ver115-2c deleted informational text about R0
    end if

    for i=0 to 5    'ver115-4a
            'Add graphs for each auxiliary item that does not have a blank name
        auxName$=auxGraphDataFormatInfo$(i,0)
        if auxName$<>"" then
            axisGraphData$(numGraphs)=auxName$
            axisDataType(numGraphs)=constAux0+i    'aux constants are sequential starting with constAux0
            numGraphs=numGraphs+1
        end if
    next i
    FillRegularGraphData=numGraphs
end function
*/
}
void MainWindow::FillAppearancesArray()
{
  for (int i=0; i <= 10; i++)
  {
    vars->Appearances[i]="";
  } //Clear
  vars->Appearances[0]="DARK";
  vars->Appearances[1]="LIGHT";
  int k=1;     //max currently used appearance index ver115-2a
  for (int i=1; i < 5; i++)    //add custom names  ver115-2a
  {
    QString customName=vars->customPresetNames[i];
    if (customName!="Empty")
    {
      k=k+1;
      vars->Appearances[k]=customName;
    }
  }
}

void MainWindow::SetCenterSpanFreq(float cent, float span)
{

  //Use Center/Span to determine centfreq, sweepwidth, startfreq, endfreq
  //A centralized routine is used so all these related variables can be kept in sync.
  //Note gCalcGraphParams must be called to fully update graph module
  vars->centfreq=cent;
  vars->sweepwidth=span;
  if (vars->sweepwidth<0) vars->sweepwidth=0-vars->sweepwidth;
  vars->startfreq=(util.usingF("####.######",vars->centfreq-vars->sweepwidth/2)).toFloat(); //ver114-5n
  vars->endfreq=(util.usingF("####.######",vars->centfreq+vars->sweepwidth/2)).toFloat(); //ver114-5n
  //ver115-1b deleted calc of stepfreq
  graph.gSetXAxisRange(vars->startfreq, vars->endfreq);  //ver114-6d

}

void MainWindow::RecalcPlaneExtendAndR0AndRedraw()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub RecalcPlaneExtendAndR0AndRedraw  'Recalculate ReflectArray or S21DataArray for new planeadj, and Reflect array for new R0 and redraw graph
    call gGetMinMaxPointNum pStart, pEnd    'start and stop of points actually in current graph
    for currStep=pStart-1 to pEnd-1     'step numbers are one less than point numbers
        if msaMode$="Reflection" then
            'For reflection, ReflectArray contains intermediate values of db and angle before plane
            'extension was done, and before any R0 transform. We just start with those and recalculate.
            currFreq=ReflectArray(currStep,0)
            f=ReflectArray(currStep,0)
            db=ReflectArray(currStep,constIntermedS11DB)   'intermediate db--saved just for this purpose
            ang=ReflectArray(currStep,constIntermedS11Ang) 'intermed angle
            call ApplyExtensionAndTransformR0 f, db,ang
            ReflectArray(currStep,1)=db      'Enter new db
            ReflectArray(currStep,2)=ang      'Enter new phase
            call CalcReflectDerivedData currStep    'Calculate impedance and other derived quantities.
        else    'transmission
            currFreq=S21DataArray(currStep,0)
            thisPhase=S21DataArray(currStep, 3)    'original phase before plane ext ver116-1b
            call uExtendCalPlane currFreq, thisPhase, planeadj,0    'do adjustment
            S21DataArray(currStep,2)=thisPhase  'Enter new phase
                'Note transform to graph R0 is for Reflection mode only
        end if
    next currStep   'Process next frequency

    call RecalcYValues  'Recalculate graph module Y values from new data
    if autoScaleY1=1 or autoScaleY2=1 then call PerformAutoScale  'autoscale with the new data
    call CreateReferenceTransform
    refreshTracesDirty=1    'Indicate to RefreshGraph that current traces are no good
    refreshGridDirty=1  'To redraw setup info with plane extension value ver115-4j
    call RefreshGraph 0
end sub
*/
}

void MainWindow::DisplayTitleWindow(int doTwoPort)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub DisplayTitleWindow doTwoPort 'Display dialog to change title of graph, or of two port params.
        'If doTwoPort=1 then we are getting the title for the two port params, which we save
        'but don't print.
    WindowWidth = 280 : WindowHeight = 220
    call GetDialogPlacement 'set UpperLeftX and UpperLeftY ver115-1c
    BackgroundColor$="buttonface"   'ver116-1b changed colors
    ForegroundColor$="black"
    TextboxColor$="white"
    ComboboxColor$="white"

    statictext #title.Instruct, "Enter up to 3 lines of title information.", 40,20,200, 15
    if doTwoPort=0 then statictext #title.Instruct, "(Line 3 gets overridden with date/time at Restart.)", 20,35,260, 15
    textbox #title.t1, 10,50,250, 20
    textbox #title.t2, 10,75,250, 20
    textbox #title.t3, 10,100,250, 20

    button #title.OK, "OK", [titleFinished],UL, 50, 150,50,25
    button #title.Cancel, "Cancel", [titleCancel],UL, 160, 150,50,25

    open "Title" for dialog_modal as #title 'Open title dialog
    #title, "trapclose [titleFinished]"
        'Display existing title info and wait for user to change it
    print #title.t1, gGetTitleLine$(1)
    print #title.t2, gGetTitleLine$(2)
    print #title.t3, gGetTitleLine$(3)

    wait
*/
}

void MainWindow::titleCancel()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[titleCancel]   'embedded in DisplayTitleWindow
    close #title
    exit sub
*/
}

void MainWindow::titleFinished()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[titleFinished] 'embedded in DisplayTitleWindow
    #title.t1, "!contents? t1$"
    #title.t2, "!contents? t2$"
    #title.t3, "!contents? t3$"

    if doTwoPort then 'ver116-1b
        call TwoPortSetTitleLine 1, t1$ : call TwoPortSetTitleLine 2, t2$ : call TwoPortSetTitleLine 3, t3$
        call gPrintTitle 1    'Clear old title and print new
        #TwoPortGraphBox$, "flush"
    else
        call gSetTitleLine 1, t1$ : call gSetTitleLine 2, t2$ : call gSetTitleLine 3, t3$
        if msaMode$="Reflection" then 'ver116-1b
            refLastTitle$(1)=t1$ : refLastTitle$(2)=t2$ : refLastTitle$(3)=t3$
        else
            if msaMode$="VectorTrans" then transLastTitle$(1)=t1$ : transLastTitle$(2)=t2$ : transLastTitle$(3)=t3$
        end if
            'Refresh the graph,but in stick mode this will eliminate
        'the stuck traces, so just reprint the title ver114-7d revised this procedure
        refreshGridDirty=1  'So next refresh will reprint the title
        if isStickMode=0 then
            call RefreshGraph 0 'Refresh in case traces got covered
        else
            call gPrintTitle 1  '1 signals to clear old info first
            'Don't want to flush in stick mode
        end if
    end if
    close #title
end sub
*/
}
int MainWindow::mMarkerNum(QString markID)
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
void MainWindow::mDeleteMarker(QString markID)
{
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
void MainWindow::mAddMarker(QString markID, int pointNum, QString trace)
{

  //Add specified marker at specified point
  QString markTrace=trace;
  QString markStyle="LabeledWedge";
  int markNum=mMarkerNum(markID);
  if (markNum<0)
  {
    QMessageBox::warning(0,"Error", "Invalid Marker Number"); return;
  }
  if (pointNum<0) { mDeleteMarker(markID); return; }    //Adding with point num <0 is deleting
  if (markID == "L")
  {
    graph.hasMarkL=1;
  }
  else if (markID == "R")
  {
    graph.hasMarkR=1;
  }
  else if (markID == "P+")
  {
    graph.hasMarkPeakPos=1;
    markTrace=QString::number(vars->primaryAxisNum);   //Always do peak markers on primary trace
  }
  else if (markID == "P-")
  {
    graph.hasMarkPeakNeg=1;
    markTrace=QString::number(vars->primaryAxisNum);   //Always do peak markers on primary trace
    markStyle="LabeledInvertedWedge";
  }
  else if (markID == "Halt")   //ver114-4c
  {
        markTrace="Xaxis";  //ver114-6d
        markStyle="HaltPointer";    //ver114-5m
  }
  else if (markID == "1" || markID == "2" || markID == "3" || markID == "4" || markID == "5" || markID == "6")
  {
        //valid markers but nothing special to do
  }
  else
  {
    return;    //not valid marker
  }

  graph.hasAnyMark=1;    //Indicate that we have at least one marker
  gSetMarkerNum(markNum, pointNum, markID, markTrace, markStyle);
  //Note we do not change the selected marker here
}

void MainWindow::mAddMarkerAndRedraw(QString markID, int ptNum, int traceNum)
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

void MainWindow::mAddMarkerFromKeyboard()
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
void MainWindow::mClearMarkers()
{
  graph.hasMarkL=0; graph.hasMarkR=0; graph.hasMarkPeakPos=0; graph.hasMarkPeakNeg=0; graph.hasAnyMark=0;
  graph.gClearMarkers();
  graph.gDrawMarkerInfo();    //to clear info area ver114-7n
  mMarkSelect("");  //ver114-5L
}


void MainWindow::mUpdateMarkerEditButtons()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub mUpdateMarkerEditButtons     'Enable/disable buttons based on selected marker
    'Disable if marker does not exist or for peak markers, which cannot be manually located
    if selMarkerID$="" then   'ver114-4a added this if... block
        noMarker=1
    else
        pointNum=gMarkerPointNum(mMarkerNum(selMarkerID$))
        if pointNum<0 then noMarker=1 else noMarker=0
    end if
    if selMarkerID$="P+" or selMarkerID$="P-" then notManual=1 else notManual=0  'ver114-4a
    if twoPortWinHndl$="" then    'main graph window
        if noMarker or notManual then   'ver114-4a
            #handle.markInc, "!disable"
            #handle.markDec, "!disable"
            #handle.markFreq, "!disable"
            #handle.markEnterFreq, "!disable"
            if noMarker then #handle.markDelete, "!disable" else #handle.markDelete, "!enable"
        else
            #handle.markInc, "!enable"
            #handle.markDec, "!enable"
            #handle.markFreq, "!enable"
            #handle.markEnterFreq, "!enable"
            #handle.markDelete, "!enable"
        end if
    else
         if noMarker or notManual then   'ver114-4a
            #twoPortWin.markInc, "!disable"
            #twoPortWin.markDec, "!disable"
            #twoPortWin.markFreq, "!disable"
            #twoPortWin.markEnterFreq, "!disable"
            if noMarker then #twoPortWin.markDelete, "!disable" else #twoPortWin.markDelete, "!enable"
        else
            #twoPortWin.markInc, "!enable"
            #twoPortWin.markDec, "!enable"
            #twoPortWin.markFreq, "!enable"
            #twoPortWin.markEnterFreq, "!enable"
            #twoPortWin.markDelete, "!enable"
        end if
    end if
end sub
*/
}
void MainWindow::mUserMarkSelect(QString btn)
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
          //Update Smith chart. But if sweep is in progress don//t, because we may have
          //been called by program action. If sweeping, it will be updated at refresh time.
      if haltsweep=0 and smithGraphHndl$()<>"" then call smithRefreshMarkerInfo   //ver115-2c
  end sub
*/
}
void MainWindow::mMarkSelect(QString markID)
{
  QString s;
  //Program selection specified marker in combo box
  if (markID=="") s="None"; else s=markID;
  if (vars->twoPortWinHndl=="")
  {
   // #handle.selMark, "select ";s$
    if (vars->multiscanInProgress==0)   //don't do if window is hidden due to multiscan ver115-8d
    {
//        #handle.selMark, "setfocus" //ver116-2a
//        #handle.Restart, "!setfocus"    //take focus off combobox
    }
  }
  else
  {
    //#twoPortWin.selMark, "select ";s$ //ver116-2a
    if (vars->multiscanInProgress==0)   //don't do if window is hidden due to multiscan ver115-8d
    {
//        #twoPortWin.selMark, "setfocus" //ver116-2a
//        #twoPortWin.markInc, "+"    //take focus off combobox
    }
  }
  mUserMarkSelect("");  //Take same action as though user selected the marker
}

void MainWindow::mBtnMarkClear(QString markID)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub mBtnMarkClear btn$   'Button to clear all markers was clicked
    call mClearMarkers
    'call HideMarkerEdit 'delver114-4a
    if twoPortWinHndl$="" then  'ver116-4a
        if doGraphMarkers=1 then call RefreshGraph 0    'ver115-1b
    else
        call TwoPortDrawGraph 0   'ver116-4a
    end if
end sub
*/
}

void MainWindow::mBtnMarkEdit(QString markID)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub mBtnMarkEdit btn$    'Button to edit selected marker was clicked
    'call ShowMarkerEdit  'delver114-4a
    call mUpdateMarkerEditButtons
end sub
*/
}

void MainWindow::mBtnMarkDelete(QString markID)
{/*
sub mBtnMarkDelete btn$      'Button to delete selected marker was clicked
    call mDeleteMarker selMarkerID$
    'call HideMarkerEdit  'delver114-4a
    if twoPortWinHndl$="" then  'ver116-2a
        if doGraphMarkers=1 then call RefreshGraph 0 else call mDrawMarkerInfo 'ver114-7d
    else
        call TwoPortDrawGraph 0   'ver116-2a
    end if
end sub
*/
}

void MainWindow::btnIncPoint()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[btnIncPoint]   'added by ver115-1a
    if haltsweep=1 then gosub [FinishSweeping]
    call IncDecPoint "markInc"
    if varwindow = 1 then gosub [preupdatevar] 'will update variables window.
    wait
*/
}

void MainWindow::btnDecPoint()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[btnDecPoint]   'added by ver115-1a
    if haltsweep=1 then gosub [FinishSweeping]
    call IncDecPoint "markDec"
    if varwindow = 1 then gosub [preupdatevar] 'will update variables window.
    wait
*/
}

void MainWindow::IncDecPoint(QString btn)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub IncDecPoint btn$   'Button to increment or decrement frequency was clicked
    'The button handlers call us with btn$="markInc" or "markDec"
    'Change frequency, and redraw
    'to update pointer on screen. But if markers are hidden, just refresh the marker info.
    if selMarkerID$="" then exit sub    'No marker selected, so can't modify   ver114-4a
    'We go back to the marker point number rather than trying to derive it from the current frequency,
    'because of issues with zero-width scans or very narrow scans.
    markPoint=gMarkerPointNum(mMarkerNum(selMarkerID$))
    'Note: the idea of equating a frequency to a point number (possibly with fractional part) does not work perfectly
    'because of rounding of frequencies to the nearest Hz. If we set a marker to an integral point number, that
    'might in theory be a frequency with fractional Hz, so when the freq is rounded and converted to a point number,
    'it may be point 5.997, even though we specified it as point 5.
    if btn$="markDec" then
        'Decrease markPoint to an integral value. If its fractional part is less than 0.1, it will become
        'the integer next lower than its integral part; otherwise it becomes its integral part.
        markPoint=int(markPoint-0.1)  'ver116-4f
    else    'Increment
        'Increase markPoint to an integral value. If its fractional part is more than 0.9, it will become
        'the integer two larger than its integral part; otherwise it becomes one greater than its integral part.
        markPoint=int(markPoint+1.1)  'ver116-4f
    end if
    if markPoint<1 then markPoint=1 else if markPoint>gPointCount() then markPoint=gPointCount()
    markFreq=gGetPointXVal(markPoint)
    if twoPortWinHndl$="" then print #handle.markFreq, using("####.######", markFreq) _
                    else #twoPortWin.markFreq, using("####.######", markFreq) ' Enter new frequency into boxver116-2a
    call gUpdateMarkerPointNum mMarkerNum(selMarkerID$), markPoint
    if twoPortWinHndl$="" then  'ver116-2a
        if doGraphMarkers=1 then call RefreshGraph 0 else call mDrawMarkerInfo 'ver114-7d
    else
        call TwoPortDrawGraph 0   'ver116-2a
    end if

    leftstep=markPoint-1     'Make leftstep a step number, not point number, for [preupdatevar] ver115-1a
end sub
*/
}

void MainWindow::mEnterMarker(QString btn)
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

void MainWindow::mDisplaySelectedMarker()
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
void MainWindow::autoWaitPrecalculate()
{
  //Activate use of auto wait times with specified precision level ver116-1b
  //Video filter should be properly set first.
  float autoWaitMaxErrorDB;
  float autoWaitMaxErrorDegrees;
  if (vars->autoWaitPrecision == "Fast")
  {
      autoWaitMaxErrorDB=1; autoWaitMaxErrorDegrees=1.5;
  }
  else if (vars->autoWaitPrecision == "Normal")
  {
      autoWaitMaxErrorDegrees=0.1;
      if (vars->msaMode=="SA") autoWaitMaxErrorDB=0.25; else autoWaitMaxErrorDB=0.1;
  }
  else   // "Precise"
  {
      //Note that in precise mode we do an extra rep after determinining it is OK to quit,
      //so the actual error should be much less the specified.
      autoWaitMaxErrorDB=0.03; autoWaitMaxErrorDegrees=0.03;
  }

  //Calculate allowed errors. These are local variables
  int maxErrorPhaseADC=(int)(autoWaitMaxErrorDegrees*activeConfig.maxpdmout/360)+1;  //convert degree error to ADC error
  //Max allowed mag error in ADC units is max db error times slope in ADC per dB.
  float maxErrorLowEndADC=autoWaitMaxErrorDB*vars->calLowEndSlope;
  float maxErrorCenterADC=autoWaitMaxErrorDB*vars->calCenterSlope;
  float maxErrorHighEndADC=autoWaitMaxErrorDB*vars->calHighEndSlope;
  if (vars->msaMode=="SA" || vars->msaMode=="ScalarTrans") vars->autoWaitTC=vars->videoMagTC; else vars->autoWaitTC=qMax(vars->videoMagTC, vars->videoPhaseTC);    //ver116-4j


  //We do repeated reads
  //until we get stable readings. The delay between reads is nominally autoWaitTC.
  //So we pre-calculate here how much the ADC should be allowed to change during that time.
  //The fraction of the total change that remains unsettled after N time constants will
  //be e^(-N), and the proportion that did settle is 1-e^(-N). We don't want the remaining change to
  //exceed the allowed error.
  //The remaining settlement is a proportion R=e^(-N)/(1-e^(-N)) of the measured settlement.
  //The max allowed change is where R*measuredChange=max allowed error, so
  //max allowed measured change=maxAllowedError/R
  //
  //In fact the time delay will generally be more than we specify, so we could allow a bigger change
  //if we knew the actual time, but the approach here is conservative.
  //For mag, N=1 so R=0.58
  //ver116-4j changed to use autoWaitTC rather than videoMagTC as the basic wait time
  int N=vars->autoWaitTC/vars->videoMagTC;   //number of mag time constants that we wait
  float unsettledFract=exp(0-N);
  double R=qMax(0.3, (double)(unsettledFract/(1-unsettledFract)));    //Limit R to be conservative
  vars->autoWaitMaxChangeLowEndADC=int(maxErrorLowEndADC/R)+1;
  vars->autoWaitMaxChangeCenterADC=int(maxErrorCenterADC/R)+1;
  vars->autoWaitMaxChangeHighEndADC=int(maxErrorHighEndADC/R)+1;

  N=vars->autoWaitTC/vars->videoPhaseTC;   //number of phase time constants that we wait
  unsettledFract=exp(0-N);
  R=qMax(0.3, (double)(unsettledFract/(1-unsettledFract)));    //Limit R to be conservative
  vars->autoWaitMaxChangePhaseADC=int(maxErrorPhaseADC/R)+1;

}
void MainWindow::SelectVideoFilter()
{

  //Select XNarrow, Narrow, Mid or Wide video filter, based on videoFilter$ ver116-1b
  //We also record the capacitance values
  //For speed reasons, port is not global so here we have to use globalPort to identify the parallel port
  //Likewise, control is not global, so we have to recreate it here
  //int control=vars->globalPort+2;
  int videoFilterNum=0;
  for (int i=1; i <= 4; i++)    //match the name to a filter
  {
    if (activeConfig.videoFilterNames[i]==vars->videoFilter) { videoFilterNum=i; break; }
  }
  if (videoFilterNum==0)
  {
    // fix me QMessageBox::about(0, "Notice", "Invalid video filter selection.");
    videoFilterNum=1; vars->videoMagCap=0.001; vars->videoPhaseCap=0.001; //default to Wide ver116-4f
  }
  else
  {
    vars->videoMagCap=qMax((float)0.002, activeConfig.videoFilterCaps[videoFilterNum][0]);    //ver116-4f
    vars->videoPhaseCap=qMax((float)0.011, activeConfig.videoFilterCaps[videoFilterNum][1]); //capacitor values--but not zero //ver116-4f
  }
  vars->videoFilterAddress=videoFilterNum-1; //So address runs from 0 to 3.
  vars->videoPhaseTC=10*vars->videoPhaseCap;     //Time constant in ms, based on 10k resistor and cap in uF
  vars->videoMagTC=2.7*vars->videoMagCap;     //Time constant in ms, based on 2.7k resistor and cap in uF
  int switchData=switchLatchBits(vars->freqBand);    //All bits for latch #4, with latch pulse set high ver116-4b
  if (activeConfig.switchHasVideo && vars->suppressHardware==0)   //ver116-4b
  {
    switch(activeConfig.cb)
    {
      case 1:  //Original
            //Can//t do this switch on original control board
        break;
      case 2:  //SLIM
/*            out globalPort, switchData //presents switch data to control buffer. Note we don't toggle latch pulse ver116-4b
            out control, globalSTRB          //enters data to control board latch
            out control, globalContClear     //freezes latch data
            out globalPort, 0
            */
        break;
      case 3:  //USB ver116-4h
            QString USBwrbuf = "A20100"+util.ToHex(switchData); //write this string but don't do latch pulse
            usb->usbMSADeviceWriteString(USBwrbuf, 4);
        break;
    }
  }
  autoWaitPrecalculate();

}
void MainWindow::SelectLatchedSwitches(int desiredFreqBand)
{

  //Sets the switches which (may) require a latch pulse. These are the band switch, forward/reverse and transmit/reflect
  //videoFilterAddress, freqBand, switchFR and switchTR must be properly set before entering
  //For speed reasons, port is not global so here we have to use globalPort to identify the parallel port
  //Likewise, control is not global, so we have to recreate it here
  if (desiredFreqBand==0) desiredFreqBand=vars->lastSetBand;   //don't change if auto mode and actual band not yet determined  ver116-4s
  vars->lastSetBand=desiredFreqBand;
  if (vars->suppressHardware) return;
  if (activeConfig.switchHasBand==0 && activeConfig.switchHasTR==0 && activeConfig.switchHasFR==0) return;    //No physical latched switches
  //int control=vars->globalPort+2;
  int switchData=switchLatchBits(desiredFreqBand);    //All bits, with latch pulse set high ver116-4s
    //We output the required bits with the latch pulse high, then briefly bring the latch
    //pulse low. Hopefully the pulse lasts somewhere between 2 and 200 us.
  switch(activeConfig.cb)
  {
    case 1:  //Original
        //Can't do these switches on original control board
        break;
    case 2:  //SLIM

        //We output the required bits with the latch pulse high, then briefly bring the latch
        //pulse low. Hopefully the pulse lasts somewhere between 2 and 200 us.
    /*    out globalPort, switchData       //presents switch data to control buffer with latch pulse high
        out control, globalSTRB          //enters data to control board latch #4
        out control, globalContClear     //freezes latch data
        out globalPort, switchData-128   //presents switch data to control buffer with latch pulse low
        for i=1 to 5   //repeat for time delay to lengthen pulse
            out control, globalSTRB          //enters data to control board latch #4; basically just activates latch pulse
        next i
        out control, globalContClear     //freezes latch data
        out globalPort, switchData       //presents switch data to control buffer with latch pulse high
        out control, globalSTRB          //enters data to control board latch #4; basically just de-activates latch pulse
        out control, globalContClear     //freezes latch data
        out globalPort, 0   //zeroes parallel port data bits
            */
            break;
      case 3:  //USB ver116-4h
        QString USBwrbuf = "A20100"+util.ToHex(switchData);
        QString USBwrbuf2 = "A20100"+util.ToHex(switchData-128);

        usb->usbMSADeviceWriteString(USBwrbuf, 4 );
        usb->usbMSADeviceWriteString(USBwrbuf2, 4 );
        usb->usbMSADeviceWriteString(USBwrbuf, 4 );
        break;

    //There is substantial delay in the DLL calls so the pulse will likely be at least 100 us.
  }
  //Wait a bit for capacitors to recover a little, to be sure we don't do a lot of latching in a short
  //time and drain the switch capacitors. This should be OK if the capacitors discharge less than 10% during latching
  //and the recharge time constant is one second or less. The switches can probably take several latchings
  //separated by 750 ms during startup, and then one latching every sweep after that.
  //Caller during startup will have to add a little extra delay.
  uSleep(250);

}
int MainWindow::switchLatchBits(int desiredFreqBand)
{
  //Returns value for setting Latch U4 on SLIM control board //ver116-1b    //ver116-4s
  //bit 0    VS0   Video Filter Address, low order bit
  //bit 1    VS1   Video Filter Address, high order bit
  //bit 2    BS0   Band Selection, low order bit
  //bit 3    BS1   Band Selection, high order bit
  //bit 4    FR    DUT Direction, Forward (0) or Reverse
  //bit 5    TR    VNA Selection, Transmission (0) or Reflection
  //bit 6   Spare
  //bit 7    PS    Pulse Start (a/k/a Latch Pulse), common to all latching   relays.
  //Normally high; pulsed low for roughly 2-200 us to trigger relay latching.
  //Note we make PS high, its normal state
  return vars->videoFilterAddress + 4*desiredFreqBand + 16*vars->switchFR + 32*vars->switchTR + 128;   //ver116-4s
}
void MainWindow::SelectFilter(int &fbank)
{
  //Select filter indicated by path$, which is in form "Path N" modver116-4j
  //Note that hardware data may have to be recalculated if finalfreq changed, by doing [PartialRestart]
  //filtbank is passed here as fbank so we can change the non-global filtbank
  int filtIndex = (util.Word(vars->path,2)).toInt();    //ver114-4c path number is second word filtIndex will be 1-4
  // fix meif (filtIndex<0 || filtIndex>activeConfig.MSANumFilters) {filtIndex=0; QMessageBox::warning(0,"Error" ,"Invalid filter selection."); }
  calMan.calInstallFile(filtIndex);   //Loads file and sets finalfreq and finalbw
  if (filtIndex<=2) vars->FiltA1=0; else vars->FiltA1=1;             //Set filter address
  if (filtIndex==1 || filtIndex==3) vars->FiltA0=0; else vars->FiltA0=1;
  CommandFilter(fbank); //Calculate fbank and Physically select filter ver116-4j
}
void MainWindow::CommandFilter(int &fbank)
{
  //ver116-4j made this a subroutine
  //filtbank is passed here as fbank so we can change the non-global filtbank
  if (activeConfig.cb == 0)
  {
    fbank = vars->FiltA1*8 + vars->FiltA0*4
        ; lpt.CommandFilterOrigCB(fbank);
  }
  if (activeConfig.cb == 2)
  {
    fbank = vars->FiltA1*64 + vars->FiltA0*32;
    lpt.CommandFilterSlimCB(fbank);
  }
  if (activeConfig.cb == 3)
  {
    fbank = vars->FiltA1*64 + vars->FiltA0*32;
    CommandFilterSlimCBUSB(fbank);
  } //USB:01-08-2010
}
void MainWindow::CommandFilterSlimCBUSB(int &fbank)//  //USB:01-08-2010 ver116-4j made this a subroutine
{
  //fbank should be the non-global filtbank
  QString USBwrbuf = "A10300"+util.ToHex(fbank)+util.ToHex(fbank+128)+util.ToHex(fbank);
  usb->usbMSADeviceWriteString(USBwrbuf, 6);
}

void MainWindow::OneStep()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
/*[OneStep]
 The OneStep button shows HaltAtEnd during a scan
    if haltsweep = 1 then goto [HaltAtEnd]
    //If the graph has been redrawn without erasure, we need to erase that part, because
    //the normal one-point draw only erases a single segment, several points ahead.
    onestep = 1 //ver111-26
    goto [FocusKeyBox]
*/
}

void MainWindow::Continue()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[Continue]
 The Continue button shows Halt during a scan
    if haltsweep = 1 then goto [Halted]     //ver111-26
    onestep = 0 //ver111-26
*/
}

void MainWindow::FocusKeyBox()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[FocusKeyBox]
    call DisplayButtonsForRunning   //SEW8 consolidated various button commands
    if isStickMode=0 then call gRestoreErasure    //ver116-4s  No erasure in stick modes
    message$="" : call PrintMessage   //ver114-4f
    //Indicate whether we are resuming a scan that was stopped in the middle.
    //If PartialRestart was done, we stopped prior to the first step, and are not "resuming"
    if haltedAfterPartialRestart then scanResumed=0 else scanResumed=1  //ver119-1b
    goto [StartSweep]
*/
}
void MainWindow::PartialRestart()
{
  //Restart but return before taking first data
  //This is used to implement user changes without taking data.
  //User should set suppressHardwareInitOnRestart=1 if desired to save time by suppressing initialization ver116-4d
  //This flag is automatically turned off after Restart, so it is a one time thing.
  vars->returnBeforeFirstStep=1; //So we stop before actually scanning, at which point this flag is reset
  Restart();
}

void MainWindow::Restart()
{
  graph.haltsweep=0;
  //When a window is closed, all graphics drawing operations are deleted from memory
  //Auxiliary graph data is computed, and does not survive the generation of new data.
  //So if aux data is currently graphed, we turn it off.
  if (vars->Y1DataType>=vars->constAux0 && vars->Y1DataType<=vars->constAux5) vars->Y1DataType=vars->constNoGraph;
  if (vars->Y2DataType>=vars->constAux0 && vars->Y2DataType<=vars->constAux5) vars->Y2DataType=vars->constNoGraph;
  if (vars->Y1DataType==vars->constNoGraph && vars->Y2DataType==vars->constNoGraph) SetDefaultGraphData();    //So we have something to graph
  ClearAuxData(); //Indicate aux data not valid by clearing graph names
  onestep = 0;
  DisplayButtonsForRunning();   //SEW8 replaced print #main.restart, "Running"
  //1 Start new sweep series.
  //Ver114-2b  Reinitialize hardware every time
  if (vars->suppressHardwareInitOnRestart) //ver115-8c
  {
    vars->suppressHardwareInitOnRestart=0; //Clear flag; we only skip initialization for one restart after flag is set.
    SkipHardwareInitialization();
    return;
  }
  else
  {
    InitializeHardware();
    return;
  }
}
void MainWindow::ClearAuxData()
{
  //Clear the auxiliary graph data by blanking the graph names
  for (int i=0; i < 5; i++)
  {
    vars->auxGraphDataFormatInfo[i][0]="";
  }
}
void MainWindow::Showvar()
{
  //modified by ver114-4f to avoid halting sweep and to operate from menu
    /*if varwindow=1 then  close #varwin:varwindow = 0  //close existing window //ver114-4f
    WindowWidth = 200
    WindowHeight = 450 //ver111-26
    UpperLeftX = DisplayWidth-WindowWidth-10    //ver114-4f
    UpperLeftY = 10    //ver114-4f

    BackgroundColor$ = "darkblue"
    ForegroundColor$ = "white"
  */
  QString var = "";
  var = var + "dds1output = "+QString::number(vars->DDS1array[vars->thisstep][46])+" MHz" + "\n";
  var = var + "LO 1 = "+QString::number(vars->PLL1array[vars->thisstep][43])+" MHz" + "\n";
  var = var + "pdf1 = "+QString::number(vars->PLL1array[vars->thisstep][40])+" MHz" + "\n";
  var = var + "ncounter1 = "+QString::number(vars->PLL1array[vars->thisstep][45]) + "\n";
  var = var + "Bcounter1 = "+QString::number(vars->PLL1array[vars->thisstep][48]) + "\n";
  var = var + "Acounter1 = "+QString::number(vars->PLL1array[vars->thisstep][47]) + "\n";
  var = var + "fcounter1 = "+QString::number(vars->PLL1array[vars->thisstep][46]) + "\n";
  var = var + "rcounter1 = "+QString::number(rcounter1) + "\n";
  var = var + "LO2 = "+QString::number(vars->LO2)+" MHz" + "\n";
  var = var + "pdf2 = "+QString::number(pdf2)+" MHz" + "\n";
  var = var + "ncounter2 = "+QString::number(ncounter2) + "\n";
  var = var + "Bcounter2 = "+QString::number(Bcounter2) + "\n";
  var = var + "Acounter2 = "+QString::number(Acounter2) + "\n";
  var = var + "rcounter2 = "+QString::number(rcounter2) + "\n";
  var = var + "LO3 = "+QString::number(vars->PLL3array[vars->thisstep][43])+" MHz" + "\n";
  var = var + "pdf3 = "+QString::number(vars->PLL3array[vars->thisstep][40])+" MHz" + "\n";
  var = var + "ncounter3 = "+QString::number(vars->PLL3array[vars->thisstep][45]) + "\n";
  var = var + "Bcounter3 = "+QString::number(vars->PLL3array[vars->thisstep][48]) + "\n";
  var = var + "Acounter3 = "+QString::number(vars->PLL3array[vars->thisstep][47]) + "\n";
  var = var + "fcounter3 = "+QString::number(vars->PLL3array[vars->thisstep][46]) + "\n";
  var = var + "rcounter3 = "+QString::number(rcounter3) + "\n";
  var = var + "dds3output = "+QString::number(vars->DDS3array[vars->thisstep][46]) + "\n";
  var = var + "Magdata="+QString::number(vars->magarray[vars->thisstep][3])+"  magpower="
      +util.usingF("####.###",vars->datatable[vars->thisstep][2]) + "\n";
  var = var + "Phadata = "+QString::number(vars->phaarray[vars->thisstep][3])+"     PDM = "+QString::number(vars->phaarray[vars->thisstep][4])+ "\n";
  //var = var + "Real Final I.F. = "+QString::number(vars->LO2
//              - [QString::number(vars->PLL1array[QString::number(vars->thisstep][45]
//              *QString::number(vars->DDS1array[QString::number(vars->thisstep][46]/QString::number(rcounter1]
  //        + QString::number(vars->datatable[QString::number(vars->thisstep][1]+QString::number(vars->baseFrequency + "\n";
  //real final IF = LO2-[LO1-thisfreq]
  var = var + "glitchtime = "+QString::number(vars->glitchtime) + "\n";
  //open "Variables Window" for dialog as #varwin:varwindow = 1
  //print #varwin, "trapclose Closevarwin" //goto Closevarwin if xit is clicked  ver115-1b changed to subroutine
  //if haltsweep=1 then goto [PostScan]
  //wait
  QMessageBox::about(0,"Variables Window",var);
}

void MainWindow::updatevar()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[updatevar]
    print #varwin.variable1, "this step = ";thisstep 'ver111-35
    print #varwin.variable2, "dds1output = ";DDS1array(thisstep,46);" MHz"
    print #varwin.variable3, "LO 1 = ";PLL1array(thisstep,43);" MHz"
    print #varwin.variable4, "pdf1 = ";PLL1array(thisstep,40);" MHz"
    print #varwin.variable5, "ncounter1 = ";PLL1array(thisstep,45)
    print #varwin.variable6, "Bcounter1 = ";PLL1array(thisstep,48)
    print #varwin.variable7, "Acounter1 = ";PLL1array(thisstep,47)
    print #varwin.variable8, "fcounter1 = ";PLL1array(thisstep,46)
    print #varwin.variable9, "rcounter1 = ";rcounter1
    print #varwin.variable10, "LO2 = ";LO2;" MHz"
    print #varwin.variable11, "pdf2 = ";pdf2;" MHz"
    print #varwin.variable12, "ncounter2 = ";ncounter2
    print #varwin.variable13, "Bcounter2 = ";Bcounter2
    print #varwin.variable14, "Acounter2 = ";Acounter2
    print #varwin.variable15, "rcounter2 = ";rcounter2
    print #varwin.variable16, "LO3 = ";PLL3array(thisstep,43);" MHz"
    print #varwin.variable17, "pdf3 = ";PLL3array(thisstep,40);" MHz"
    print #varwin.variable18, "ncounter3 = ";PLL3array(thisstep,45)
    print #varwin.variable19, "Bcounter3 = ";PLL3array(thisstep,48)
    print #varwin.variable20, "Acounter3 = ";PLL3array(thisstep,47)
    print #varwin.variable21, "fcounter3 = ";PLL3array(thisstep,46)
    print #varwin.variable22, "rcounter3 = ";rcounter3
    print #varwin.variable23, "dds3output = ";DDS3array(thisstep,46)
    print #varwin.variable24, "Magdata= ";magarray(thisstep,3);" magpower=";using("####.###",datatable(thisstep,2))' ver115-5b raw magdata bits, MSA input power(massaged) 'ver111-39b
    print #varwin.variable25, "Phadata = ";phaarray(thisstep,3);"     PDM = ";phaarray(thisstep,4) 'ver111-39d
    print #varwin.variable26, "Real Final I.F. = ";LO2 - (PLL1array(thisstep,45)*DDS1array(thisstep,46)/rcounter1) + datatable(thisstep,1) 'ver112-2b
    print #varwin.variable27, "glitchtime = ";glitchtime 'ver114-7b

    return
*/
}

void MainWindow::Closevarwin()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub Closevarwin hndl$ 'ver115-1b changed to subroutine
    close #varwin:varwindow = 0     'close out variables window
end sub*/
}
float MainWindow::Equiv1GFreq(float f, int aBand)
{
  float retVal;
  //Return equivalent 1G frequency for f, based on aBand (0,1,2 or 3)    //ver116-4s
  if (aBand==0)  //ver116-4s
  {
    aBand=1;
    if (f > vars->bandEnd2G)
      aBand=3;
    else if (f > vars->bandEnd1G)
      aBand=2;
  }
  switch(aBand)    //ver116-4s
  {
    case 1:
        retVal=f;   //1G mode; no conversion necessary
        break;
    case 2:      //2G mode
        retVal = f - vars->LO2;
        break;
    default:   //3G mode
        float IF1 = vars->LO2 - activeConfig.finalfreq;
        retVal=f-2*IF1;
  }
  return retVal;
}

float MainWindow::ActualSignalFrequency(float f, int aBand)
{
  //Return actual signal frequency for equiv 1G freq f, based on aBand (1,2 or 3) ver116-4s
  switch(aBand)    //ver116-4s
  {
    case 1:
        return(f);   //1G mode; no conversion necessary
    case 2:      //2G mode
        return (f+vars->LO2);
    default:   //3G mode
        float IF1 = vars->LO2 - activeConfig.finalfreq;
        return(f+2*IF1);
  }
}
void MainWindow::CalculateAllStepsForLO1Synth()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    haltstep = thisstep //remember where we were in the sweep when halted
    for thisstep = 0 to steps
            //ver116-4s moved saving of frequency in other arrays to the place where hardware frequency is calculated.
             added baseFrequency, which gets added when commanding but does not affect the stored frequencies.
        thisfreq=datatable(thisstep, 1) //ver116-4s
        float LO1 = baseFrequency + thisfreq + LO2 - finalfreq    //calculates the actual LO1 frequency:thisfreq,LO2,finalfreq are actuals.

        //[CalculateThisStepPLL1]
        appxVCO=LO1 : reference=appxdds1 : rcounter=rcounter1
        if PLL1mode = 0 then gosub [CreateIntegerNcounter]//needed:appxVCO,reference,rcounter ; creates:ncount,ncounter,fcounter(0)
            //returns with ncount,ncounter,fcounter(0),pdf
        if PLL1mode = 1 then gosub [CreateFractionalNcounter]//needed:appxVCO,reference,rcounter ; creates:ncount,ncounter,fcounter,pdf
            //returns with ncount,ncounter,fcounter,pdf
        dds1output = pdf * rcounter    //actual output of DDS1(input Ref to PLL1)
        if PLL1mode = 1 then gosub [AutoSpur]//needed:LO2,finalfreq,dds1output,rcounter1,finalbw,appxdds1,fcounter,ncounter ver111-8
            //[AutoSpur] is a continuation of [CreateFractionalNcounter], used only in MSA when PLL 1 is Fractional
            //returns with possibly new: ncounter,fcounter,pdf,dds1output
        if PLL1mode = 1 then gosub [ManSpur]//ver111-10
        //[ManSpur] is a continuation of [CreateFractionalNcounter], used only in MSA when PLL 1 is Fractional
        //if Spur Test Button On, will return with new ncounter,fcounter,pdf,dds1output
        gosub [CreatePLL1N]//needs:ncounter,fcounter,PLL1mode,PLL1 ; creates PLL NBuffer N0-Nx
        gosub [FillPLL1array]//need:N0-Nx,pdf,dds1output,LO1,ncount,ncounter,Fcounter,Acounter,Bcounter;creates samePLL1
        //[endCalculateThisStepPLL1]
        //[CalculateThisStepDDS1]//need:dds1output,masterclock,appxdds1,dds1filbw
          ddsoutput = dds1output : ddsclock = masterclock
        if dds1output-appxdds1>dds1filbw/2 then  //ver114-4e
            beep:error$="DDS1output too high for filter"
            message$=error$ : call PrintMessage  //ver114-4e
            call RequireRestart   //ver115-1c
            wait
        end if
        if appxdds1-dds1output>dds1filbw/2 then  //ver114-4e
            beep:error$="DDS1output too low for filter"
            message$=error$ : call PrintMessage //ver114-4e
            call RequireRestart   //ver115-1c
            wait
        end if
            gosub [CreateBaseForDDSarray]//needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
            gosub [FillDDS1array]//need thisstep,sw0-sw39,w0-w4,base,ddsclock
        //[endCalculateThisStepDDS1]
    next thisstep
    thisstep = haltstep //return to the step in the sweep, where we halted, if needed
    return

*/

}

void MainWindow::CalculateAllStepsForLO3Synth()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
//for hybrid, and orig (fixed freq) TG
    //if TGtop = 0 then skip all this (return), actually we should not have even entered this subroutine.
    haltstep = thisstep //remember where we were in the sweep when halted
    for thisstep = 0 to steps  //ver111-17
             Frequencies have been pre-calculated in the graphing module via gGenerateXValues
        thisfreq=datatable(thisstep,1) : thisBand=datatable(thisstep,4) //ver116-4s
        TrueFreq=ActualSignalFrequency(thisfreq, thisBand)+baseFrequency    //ver116-4s
             baseFrequency gets added when commanding but does not affect the stored frequencies.
        //if FreqMode=1 then thisfreq=TrueFreq else thisfreq=Equiv1GFreq(TrueFreq)  //ver115-1c get equivalent 1G frequency ver115-1d delver116-4s

        if TGtop = 1 then LO3 = LO2 - finalfreq - offset //for orig, fixed freq TG  ver111-15a
                      //or LO3 = LO1 - thisfreq - offset
            //ver115-1c rearranged the following if... block and added the FreqMode=3 test
        if TGtop = 2 and gentrk = 1 then
            if normrev = 0 then
                if thisBand=3 then  //ver116-4s
                    LO3 = TrueFreq + offset - LO2    //Mode 3G sets LO3 differently
                else
                    LO3 = LO2 + thisfreq + offset   //for new TG, Trk Gen mode, normal
                end if
            end if
            if normrev = 1 then
                 Frequencies have been pre-calculated in the graphing module via gGenerateXValues
                //We can just retrieve them in reverse order.
                TrueFreq=gGetPointXVal(steps-thisstep+1)+baseFrequency    //Point number is 1 greater than step number ver116-4L
                if thisBand=1 then revfreq=TrueFreq else revfreq=Equiv1GFreq(TrueFreq, thisBand)  //ver115-1d get equiv 1G freq ver116-4s
                if thisBand=3 then  //ver115-1d added this if... block  ver116-4s
                    LO3 = TrueFreq + offset - LO2  //Mode 3G sets LO3 differently
                else
                    LO3 = LO2 + revfreq + offset //for new TG, Trk Gen mode, normal
                end if
            end if
        end if

        if TGtop = 2 and gentrk = 0 then
            //for new TG, Sig Gen mode ver116-4p
            //We will try to produce sgout, either by LO3-LO2(for 0 to LO2), LO3 (LO2 to 2*LO2) or LO3+LO2 (above 2*LO2)
            select case   //ver116-4p
                case sgout<=LO2 : LO3=sgout+LO2 //ver116-4r
                case sgout>2*LO2 : LO3=sgout-LO2 //ver116-4r
                case else : LO3=sgout //ver116-4r
            end select
        end if
        //[CalculateThisStepPLL3]
            appxVCO=LO3 : reference=appxdds3 : rcounter=rcounter3
            if appxdds3 = 0 then reference=masterclock //for orig, fixed freq TG with no DDS3 steering. ver111-17
            if PLL3mode = 0 then gosub [CreateIntegerNcounter]//needed:appxVCO,reference,rcounter ; creates:ncount,ncounter,fcounter(0)
                //returns with ncount,ncounter,fcounter(0),pdf
            if PLL3mode = 1 then gosub [CreateFractionalNcounter]//needed:appxVCO,reference,rcounter ; creates:ncount,ncounter,fcounter,pdf
                //returns with ncount,ncounter,fcounter,pdf
                dds3output = pdf * rcounter    //actual output of DDS3(input Ref to PLL3)
                gosub [CreatePLL3N]//needs:ncounter,fcounter,PLL3mode,PLL3 ; creates PLL NBuffer N0-Nx
                gosub [FillPLL3array]//need thisstep,N0thruN23,pdf3(40),dds3output(41),samePLL3(42)see dim PLL3array for slot info //ver111-14
        //[endCalculateThisStepPLL3]
        //[CalculateThisStepDDS3]//need:dds3output,masterclock,appxdds3,dds3filbw
          if appxdds3 = 0 then goto [endCalculateThisStepDDS3] //there is no DDS, skip this section ver111-17
            ddsoutput = dds3output : ddsclock = masterclock
            if dds3output-appxdds3>dds3filbw/2 then    //ver114-4e
                beep:error$="DDS3 output too high for filter"
                message$=error$ : call PrintMessage
                call RequireRestart   //ver115-1c
                wait
            end if

            if appxdds3-dds3output>dds3filbw/2 then  //ver114-4e
                beep:error$="DDS3output too low for filter"
                message$=error$ : call PrintMessage
                call RequireRestart   //ver115-1c
                wait
            end if
            gosub [CreateBaseForDDSarray]//needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
            gosub [FillDDS3array]//need thisstep,sw0-sw39,w0-w4,base,ddsclock  ver111-15
        [endCalculateThisStepDDS3]
        //del.ver112-2b phaarray(thisstep,0) = 0 //this will set all pdmstates, to 0 //ver112-1a
        phaarray(thisstep,0) = 0 //this will set all pdmstates, to 0 //undeleted, ver113-7e
    next thisstep
    thisstep = haltstep //return to the step in the sweep, where we halted, if needed
    lastpdmstate = 2 //this will guarantee that the PDM will get commanded //ver112-1a
    return
    */
}

void MainWindow::FillPLL1array()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  Int64N temp;
  if (activeConfig.cb == 3) //USB:11-08-2010
    usb->usbMSADevicePopulateDDSArrayBitReverse(ptrSPLL1Array, temp, vars->thisstep, 40, result);*/
      //if USBdevice <> 0 then CALLDLL #USB, "UsbMSADevicePopulateDDSArrayBitReverse", USBdevice as long, ptrSPLL1Array as ulong, Int64N as ptr, thisstep as short, 40 as short, result as boolean 'USB:11-08-2010

//need thisstep,N0thruN23,pdf1(40),dds1output(41),samePLL1(42)see dim PLL1array for slot info 'ver111-1
  /*
    if cb = 3 then 'USB:11-08-2010
        if USBdevice <> 0 then CALLDLL #USB, "UsbMSADevicePopulateDDSArrayBitReverse", USBdevice as long, ptrSPLL1Array as ulong, Int64N as ptr, thisstep as short, 40 as short, result as boolean 'USB:11-08-2010
    else 'USB:05/12/2010

        'reversed sequence for N23 to be first. ver111-31a
        PLL1array(thisstep,23) = N0:PLL1array(thisstep,22) = N1
        PLL1array(thisstep,21) = N2:PLL1array(thisstep,20) = N3
        PLL1array(thisstep,19) = N4:PLL1array(thisstep,18) = N5
        PLL1array(thisstep,17) = N6:PLL1array(thisstep,16) = N7
        PLL1array(thisstep,15) = N8:PLL1array(thisstep,14) = N9
        PLL1array(thisstep,13) = N10:PLL1array(thisstep,12) = N11
        PLL1array(thisstep,11) = N12:PLL1array(thisstep,10) = N13
        PLL1array(thisstep,9) = N14:PLL1array(thisstep,8) = N15
        PLL1array(thisstep,7) = N16:PLL1array(thisstep,6) = N17
        PLL1array(thisstep,5) = N18:PLL1array(thisstep,4) = N19
        PLL1array(thisstep,3) = N20:PLL1array(thisstep,2) = N21
        PLL1array(thisstep,1) = N22:PLL1array(thisstep,0) = N23
    end if 'USB:05/12/2010
    PLL1array(thisstep,40) = pdf
    PLL1array(thisstep,43) = LO1
    PLL1array(thisstep,45) = ncounter
    PLL1array(thisstep,46) = fcounter
    PLL1array(thisstep,47) = Acounter
    PLL1array(thisstep,48) = Bcounter
    return
*/
}

void MainWindow::FillPLL3array()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[FillPLL3array]'need thisstep,N0thruN23,pdf3(40),dds3output(41),samePLL3(42)see dim PLL3array for slot info 'ver111-14
    if cb = 3 then'USB:11-08-2010
        if USBdevice <> 0 then CALLDLL #USB, "UsbMSADevicePopulateDDSArrayBitReverse", USBdevice as long, ptrSPLL3Array as ulong, Int64N as ptr, thisstep as short, 40 as short, result as boolean 'USB:11-08-2010
    else 'USB:05/12/2010
        'reversed sequence for N23 to be first. ver111-31a
        PLL3array(thisstep,23) = N0:PLL3array(thisstep,22) = N1
        PLL3array(thisstep,21) = N2:PLL3array(thisstep,20) = N3
        PLL3array(thisstep,19) = N4:PLL3array(thisstep,18) = N5
        PLL3array(thisstep,17) = N6:PLL3array(thisstep,16) = N7
        PLL3array(thisstep,15) = N8:PLL3array(thisstep,14) = N9
        PLL3array(thisstep,13) = N10:PLL3array(thisstep,12) = N11
        PLL3array(thisstep,11) = N12:PLL3array(thisstep,10) = N13
        PLL3array(thisstep,9) = N14:PLL3array(thisstep,8) = N15
        PLL3array(thisstep,7) = N16:PLL3array(thisstep,6) = N17
        PLL3array(thisstep,5) = N18:PLL3array(thisstep,4) = N19
        PLL3array(thisstep,3) = N20:PLL3array(thisstep,2) = N21
        PLL3array(thisstep,1) = N22:PLL3array(thisstep,0) = N23
    end if 'USB:05/12/2010
    PLL3array(thisstep,40) = pdf
    PLL3array(thisstep,43) = LO3
    PLL3array(thisstep,45) = ncounter
    PLL3array(thisstep,46) = fcounter
    PLL3array(thisstep,47) = Acounter
    PLL3array(thisstep,48) = Bcounter
    return
*/
}

void MainWindow::FillDDS1array()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[FillDDS1array]'need thisstep,sw0-sw39,w0-w4,base,ddsclock 'ver111-12
    if cb = 3 then'USB:11-08-2010
        if USBdevice <> 0 then CALLDLL #USB, "UsbMSADevicePopulateDDSArray", USBdevice as long, ptrSDDS1Array as ulong, Int64SW as ptr, thisstep as short, result as boolean 'USB:11-08-2010
    else 'USB:05/12/2010
        DDS1array(thisstep,0) = sw0:DDS1array(thisstep,1) = sw1
        DDS1array(thisstep,2) = sw2:DDS1array(thisstep,3) = sw3
        DDS1array(thisstep,4) = sw4:DDS1array(thisstep,5) = sw5
        DDS1array(thisstep,6) = sw6:DDS1array(thisstep,7) = sw7
        DDS1array(thisstep,8) = sw8:DDS1array(thisstep,9) = sw9
        DDS1array(thisstep,10) = sw10:DDS1array(thisstep,11) = sw11
        DDS1array(thisstep,12) = sw12:DDS1array(thisstep,13) = sw13
        DDS1array(thisstep,14) = sw14:DDS1array(thisstep,15) = sw15
        DDS1array(thisstep,16) = sw16:DDS1array(thisstep,17) = sw17
        DDS1array(thisstep,18) = sw18:DDS1array(thisstep,19) = sw19
        DDS1array(thisstep,20) = sw20:DDS1array(thisstep,21) = sw21
        DDS1array(thisstep,22) = sw22:DDS1array(thisstep,23) = sw23
        DDS1array(thisstep,24) = sw24:DDS1array(thisstep,25) = sw25
        DDS1array(thisstep,26) = sw26:DDS1array(thisstep,27) = sw27
        DDS1array(thisstep,28) = sw28:DDS1array(thisstep,29) = sw29
        DDS1array(thisstep,30) = sw30:DDS1array(thisstep,31) = sw31
        DDS1array(thisstep,32) = sw32:DDS1array(thisstep,33) = sw33
        DDS1array(thisstep,34) = sw34:DDS1array(thisstep,35) = sw35
        DDS1array(thisstep,36) = sw36:DDS1array(thisstep,37) = sw37
        DDS1array(thisstep,38) = sw38:DDS1array(thisstep,39) = sw39
    end if 'USB:05/12/2010
    DDS1array(thisstep,40) = w0
    DDS1array(thisstep,41) = w1
    DDS1array(thisstep,42) = w2
    DDS1array(thisstep,43) = w3
    DDS1array(thisstep,44) = w4
    DDS1array(thisstep,45) = base 'base is decimal command
    DDS1array(thisstep,46) = base*ddsclock/2^32 'actual dds 1 output freq
    return
*/
}

void MainWindow::FillDDS3array()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[FillDDS3array]'need thisstep,sw0-sw39,w0-w4,base,ddsclock 'ver111-15
    if cb = 3 then'USB:11-08-2010
        if USBdevice <> 0 then CALLDLL #USB, "UsbMSADevicePopulateDDSArray", USBdevice as long, ptrSDDS3Array as ulong, Int64SW as ptr, thisstep as short, result as boolean 'USB:11-08-2010
    else 'USB:05/12/2010
        DDS3array(thisstep,0) = sw0:DDS3array(thisstep,1) = sw1
        DDS3array(thisstep,2) = sw2:DDS3array(thisstep,3) = sw3
        DDS3array(thisstep,4) = sw4:DDS3array(thisstep,5) = sw5
        DDS3array(thisstep,6) = sw6:DDS3array(thisstep,7) = sw7
        DDS3array(thisstep,8) = sw8:DDS3array(thisstep,9) = sw9
        DDS3array(thisstep,10) = sw10:DDS3array(thisstep,11) = sw11
        DDS3array(thisstep,12) = sw12:DDS3array(thisstep,13) = sw13
        DDS3array(thisstep,14) = sw14:DDS3array(thisstep,15) = sw15
        DDS3array(thisstep,16) = sw16:DDS3array(thisstep,17) = sw17
        DDS3array(thisstep,18) = sw18:DDS3array(thisstep,19) = sw19
        DDS3array(thisstep,20) = sw20:DDS3array(thisstep,21) = sw21
        DDS3array(thisstep,22) = sw22:DDS3array(thisstep,23) = sw23
        DDS3array(thisstep,24) = sw24:DDS3array(thisstep,25) = sw25
        DDS3array(thisstep,26) = sw26:DDS3array(thisstep,27) = sw27
        DDS3array(thisstep,28) = sw28:DDS3array(thisstep,29) = sw29
        DDS3array(thisstep,30) = sw30:DDS3array(thisstep,31) = sw31
        DDS3array(thisstep,32) = sw32 'x4 multiplier
        DDS3array(thisstep,33) = sw33 'control bit
        DDS3array(thisstep,34) = sw34 'power down bit
        DDS3array(thisstep,35) = sw35 '35-39 are Phase
        DDS3array(thisstep,36) = sw36:DDS3array(thisstep,37) = sw37
        DDS3array(thisstep,38) = sw38:DDS3array(thisstep,39) = sw39
    end if 'USB:05/12/2010
    DDS3array(thisstep,40) = w0 'word 0, 8 bits, mult, control and phase
    DDS3array(thisstep,41) = w1 'word 1, 8 bits
    DDS3array(thisstep,42) = w2 'word 2, 8 bits
    DDS3array(thisstep,43) = w3 'word 3, 8 bits
    DDS3array(thisstep,44) = w4 'word 4, 8 bits
    DDS3array(thisstep,45) = base 'base is decimal command
    DDS3array(thisstep,46) = base*ddsclock/2^32 'actual dds 3 output freq
    return
*/
}
void MainWindow::CreateCmdAllArray()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
 //for SLIM CB only //ver-31b
    //a DDS serial command, will begin with LSB (W0), thru MSB (W31), ending with Phase bit 4 (W39)
    //a PLL serial command, will begin with MSB (N23), thru LSB (N0, the address bit)
    rememberthisstep = thisstep //remember where we were when entering this subroutine
    if cb <> 3 then //USB:05/12/2010
        for thisstep = 0 to steps
            for clmn = 0 to 15
                cmdallarray(thisstep,clmn) = DDS1array(thisstep,clmn)*4 + DDS3array(thisstep,clmn)*16
            next clmn
            for clmn = 16 to 39
                cmdallarray(thisstep,clmn) = PLL1array(thisstep,clmn-16)*2 + DDS1array(thisstep,clmn)*4 + PLL3array(thisstep,clmn-16)*8 + DDS3array(thisstep,clmn)*16
            next clmn
        next thisstep
    else //USB:05/12/2010
        if USBdevice <> 0 then CALLDLL #USB, "UsbMSADevicePopulateAllArray", USBdevice as long, steps as short, 40 as short, _
                            0 as long, ptrSPLL1Array as long, ptrSDDS1Array as long, ptrSPLL3Array as long, _
                            ptrSDDS3Array as long, 0 as long, 0 as long, 0 as long, _
                            result as boolean //USB:11-08-2010
    end if //USB:05/12/2010
    thisstep = rememberthisstep
    return
    */
}
void MainWindow::CommandPLL()
{
  // comes here during PLL R Initializations and PLL 2 N command ver111-28
  if (activeConfig.cb == 0) lpt.CommandPLLorig(); //ver111-28
  if (activeConfig.cb == 2) lpt.CommandPLLslim(); //ver111-28
  if (activeConfig.cb == 3) CommandPLLslimUSB();  //USB:01-08-2010
  return; //to [InitializePLL2]or[CommandXPllRbuffer]

}


void MainWindow::CommandPLLslimUSB()
{
  Int64N fred;
  //USB:01-08-2010
  if (!usb->getUSBDevice()) return; //USB:05/12/2010
  //CALLDLL #USB, "UsbMSADeviceWriteInt64MsbFirst", USBdevice as long,
  //161 as short, Int64N as ptr, 24 as short, 1 as short, filtbank as short, datavalue as short, result as boolean  //USB:11-08-2010
  unsigned long result;
  usb->usbMSADeviceWriteInt64MsbFirst((short)161, (unsigned long *)(&fred), (short)24, (short)1, filtbank, datavalue, &result);
  int pdmcommand = vars->phaarray[vars->thisstep][0]*64; //do not disturb PDM state, this may be used during Spur Test

  QString USBwrbuf = "A30200"+util.ToHex(pdmcommand + levalue)+util.ToHex(pdmcommand);
  usb->usbMSADeviceWriteString(USBwrbuf,5);
  return;
}


void MainWindow::DetermineModule()
{
  //All "glitchXX's" are "0" when entering this subroutine. Either from "fresh RUN" or [WaitStatement]
  //if a module is not present, or if it doesn't need commanding, return with it's "glitchXX = 0"

  vars->dds1output = vars->DDS1array[vars->thisstep][46];
  if (vars->dds1output != vars->lastdds1output)
  {
    //dds 1 is same, don//t waste time commanding

    glitchd1 = 1;
    vars->lastdds1output = vars->dds1output;
  }
  ncounter1=vars->PLL1array[vars->thisstep][45];
  fcounter1=vars->PLL1array[vars->thisstep][46];
  if (!(ncounter1==lastncounter1 && fcounter1==lastfcounter1))
  {
    //dont waste time commanding
    glitchp1 = 1; //add 1 msec delay.
    lastncounter1=ncounter1;
    lastfcounter1=fcounter1;
  }

  if  (activeConfig.TGtop == 0)
    return; //there is no PLL 3, no DDS 3,and no PDM for VNA
  ncounter3=vars->PLL3array[vars->thisstep][45];
  fcounter3=vars->PLL3array[vars->thisstep][46];
  if (!(ncounter3==lastncounter3 && fcounter3==lastfcounter3))
  {
    //don't waste time commanding
    glitchp3 = 1; //add 1 msec delay
    lastncounter3=ncounter3;
    lastfcounter3=fcounter3;
  }

  if (activeConfig.appxdds3 != 0)
    //if 0, there is no DDS3, but, there can be VNA
    vars->dds3output = vars->DDS3array[vars->thisstep][46];
  if (vars->dds3output != vars->lastdds3output)
  {
    //dds 3 is same, don//t waste time commanding
    glitchd3 = 1;
    vars->lastdds3output = vars->dds3output;
  }

  if (vars->suppressPhase || vars->msaMode=="SA" || vars->msaMode=="ScalarTrans")
    return; // not in VNA mode, skip the PDM
  pdmcmd = vars->phaarray[vars->thisstep][0];
  if (pdmcmd == vars->lastpdmstate)
    return; //don't waste time commanding
  VideoGlitchPDM();
}

void MainWindow::CommandPDMonly()
{
  //ver111-28
  if (activeConfig.cb == 0) lpt.CommandPDMOrigCB(); //ver111-28
  if (activeConfig.cb == 2) lpt.CommandPDMSlimCB(); //ver111-28
  if (activeConfig.cb == 3) CommandPDMSlimUSB();  //USB:01-08-2010
  return; //to InvertPDmodule()
}
void MainWindow::CommandPDMSlimUSB()
{
  //USB:01-08-2010
  int i = vars->phaarray[vars->thisstep][0]*64;
  QString USBwrbuf = "A30300"+util.ToHex(i)+util.ToHex(i+32)+util.ToHex(i);
  usb->usbMSADeviceWriteString(USBwrbuf,6);
  vars->lastpdmstate=vars->phaarray[vars->thisstep][0];   //ver114-6c
}
/*
' ****************
'USB:05/12/2010
' following code changed from previous USB code
' USB: 15/08/10
' all three of the following work. SLowest at the top, fastest at the bottom
*/

void MainWindow::CommandAllSlimsUSB()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*// USB: 15/08/10
    //(send data and clocks without changing Filter Bank)
    if USBdevice = 0 then return //USB:05/12/2010
    if thisstep = 0 then //USB:05/12/2010
        UsbAllSlimsAndLoadData.filtbank.struct = filtbank //USB:05/12/2010
        UsbAllSlimsAndLoadData.latches.struct = le1 + fqud1 + le3 + fqud3 //USB:05/12/2010
        UsbAllSlimsAndLoadData.pdmcmdmult.struct = 64 //USB:05/12/2010
        UsbAllSlimsAndLoadData.pdmcmdadd.struct = 32 //USB:05/12/2010
    end if //USB:05/12/2010
    UsbAllSlimsAndLoadData.pdmcommand.struct = phaarray(thisstep,0) //USB:05/12/2010
    UsbAllSlimsAndLoadData.thisstep.struct = thisstep //USB:05/12/2010
    CALLDLL #USB, "UsbMSADeviceAllSlimsAndLoadStruct", USBdevice as long, UsbAllSlimsAndLoadData as struct, result as boolean // USB: 15/08/10
    lastpdmstate=phaarray(thisstep,0)   //ver114-6c
    return
*/
}

void MainWindow::finished()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  //this is the end of the software, close any open window
  if (special == 1) then close #special    //close out Special Tests window. ver113-5a
  if (varwindow == 1 then close #varwin   //close out variable window
  if (datawindow == 1 then close #datawin //close out data window. ver113-5a
  if lptwindow == 1 then close #LPTwindow //lpt  ver116-4b
  if calManWindHndl$<>"" then close #calManWindHndl$   //close window for calibration manager, SEWcal3 ver113-7g
  if configWindHndl$<>"" then close #configWindHndl$    //close window for configuration manager, SEWcal3 ver113-7g
  if axisPrefHandle$<>"" then close #axisPrefHandle    Close axis preference window; it//s a modal dialog so this should not happen
  if crystalListHndl$<> "" then close #crystalListHndl$
  if crystalWindHndl$<> "" then close #crystalWindHndl$   //ver115-5f
  if componentWindHndl$<>"" then close #componentWindHndl$
  if twoPortWinHndl$<>"" then close #twoPortWinHndl$ //ver116-1b
  call smithFinished ""      //ver115-1b
  for i=1 to multiscanMaxNum
      thisWindowHndl$= multiscanWindowHandlesLB$(i)
      if thisWindowHndl$<>"" then close #thisWindowHndl$    //Graph windows for multiscans ver115-8c
  next i
  CallDLL #DLL.OLE, "CoUninitialize", ret as void  //To avoid tooltips crash in file dialog ver116-4q
  Close #DLL.OLE //ver116-4q
  */
  // close USB interface if it was active
  usb->usbCloseInterface(); //USB:01-08-2010
  //close #handle   //close out graph window
  //ret = GlobalFree(hSAllArray) //USB:01-08-2010
  QCoreApplication::exit(0);
}

void MainWindow::SpecialTests()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[SpecialTests]'ver111-36b
    if haltsweep=1 then gosub [CleanupAfterSweep]    'ver114-4f
    if special=1 then goto [CloseSpecial]
    WindowWidth = 200
    WindowHeight = 300
    UpperLeftX = DisplayWidth-WindowWidth-20    'ver114-4f
    UpperLeftY = 5    'ver114-4f
    BackgroundColor$ = "buttonface" 'color changed by ver116-4k
    ForegroundColor$ = "black"
    TextboxColor$ = "white"
    button #special.DDS1, "Command DDS 1", [CommandDDS1], UL, 5, 5, 100, 20
    textbox #special.dds1out, 105, 5, 75, 20   'create DDS 1 frequency output box
    statictext #special.dds1txt, "with DDS Clock at ", 5, 30, 100, 15
    textbox #special.masclkf, 105, 30, 75, 20   'create master clock frequency box
    button #special.DDS3, "Command DDS 3", [CommandDDS3], UL, 5, 55, 100, 20 'ver111-38a
    textbox #special.dds3out, 105, 55, 75, 20   'create DDS 1 frequency output box
    button #special.dds3track, "DDS 3 Track", [DDS3Track], UL, 5, 105, 75, 20 'ver111-39d
    statictext #special.dds3trktxt, "0-32", 80, 107, 25, 15 'ver112-2c
    button #special.dds1track, "DDS 1 Sweep", [DDS1Sweep], UL, 115, 105, 75, 20 'ver112-2c
    if msaMode$<>"SA" and msaMode$<>"ScalarTrans" then
        button #special.pdminv, "Change PDM", [ChangePDM], UL, 5, 130, 90, 20 'ver115-4d
        button #special.insert, "Sync Test PDM", [SyncTestPDM], UL, 5, 155, 90, 20 'ver112-2f
        button #special.prevnalin, "Preset Phase Linearity", [PresetVNAlin], UL, 5, 180, 150, 20 'ver112-2f ver114-5n ver114-8c
    end if
    button #special.lpttest, "LPT Port Test", [LPTportTest], UL, 5, 220, 100, 20 'lpt ver116-4b
    button #special.cftest, "Cavity Filt Test", [CavityFilterTest], UL, 5, 250, 100, 20 'cav ver116-4c

    open "Special Tests Window" for dialog as #special:special = 1
    #special, "font ms_sans_serif 9" 'ver116-4m
    print #special.dds1out, "";DDS1array(thisstep,46) 'insert DDS1 output frequency at thisstep 'ver112-2d
    print #special.masclkf, "";masterclock 'insert masterclock frequency
    print #special.dds3out, "";DDS3array(thisstep,46) 'insert DDS3 output frequency at thisstep 'ver112-2d
    print #special, "trapclose [CloseSpecial]" 'goto [CloseSpecial] if xit is clicked
    wait
*/
}

void MainWindow::CloseSpecial()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CloseSpecial]'ver111-36b
    syncsweep = 0 'ver112-2b
    setpdm = 0 'makes sure the PDM returns to automatic operation ver112-2a
    convdatapwr = 0 'ver112-2b
    vnalintest = 0 'ver112-2f
    test = 0    'ver112-2g
    if cftest = 1 then gosub [CloseCavityFilterTest] 'cav ver116-4c
    close #special:special = 0     'close out Special Tests window
    call RequireRestart 'ver116-4c
    if returnflag = 1 then return   'ver112-2f
    wait
*/
}

void MainWindow::CavityFilterTest()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'comes here when Cavity Filter Test button is clicked
    if cftest = 1 then wait
    cftest = 1
    enterPLL2phasefreq = PLL2phasefreq
    PLL2phasefreq = .1
goto [Restart]
*/
}
void MainWindow::CloseCavityFilterTest()
{
  //will come here when Special Tests Window is closed
  cftest = 0;
  activeConfig.PLL2phasefreq = enterPLL2phasefreq;
}

void MainWindow::CommandLO2forCavTest()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CommandLO2forCavTest]
    appxVCO = finalfreq + PLL1array(thisstep,43)
    reference = masterclock
    rcounter = rcounter2
    gosub [CreateIntegerNcounter]'needs:appxVCO,reference,rcounter ; creates:ncounter,fcounter(0)
    ncounter2 = ncounter:fcounter2 = fcounter
    gosub [CreatePLL2N]'needs:ncounter,fcounter,PLL2 ; returns with Bcounter,Acounter, and N Bits N0-N23
    Bcounter2=Bcounter: Acounter2=Acounter
    LO2=((Bcounter*preselector)+Acounter+(fcounter/16))*pdf2 'actual LO2 frequency  'ver115-1c LO2 is now global
    'CommandPLL2N
    Jcontrol = SELT : LEPLL = 8
    datavalue = 16: levalue = 16 'PLL2 data and le bit values ver111-28
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111-5
return
*/
}

void MainWindow::OpenDataWindow()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[OpenDataWindow]'ver113-5a
if haltsweep = 1 then gosub [FinishSweeping] 'ver114-6f
    'if the "Array Data Window" is already open, close it.
        if datawindow = 1 then close #datawin:datawindow = 0
    'create window called, Data Window, to display all data for each step

    WindowWidth = 425   'ver115-4h
    WindowHeight = 300
    UpperLeftX = DisplayWidth-WindowWidth-20    'ver114-6f
    UpperLeftY = 20    'ver114-6f
    BackgroundColor$ = "white"
    ForegroundColor$ = "black"
    open "Data Window" for text as #datawin
    datawindow = 1
    #datawin, "!font Courier_New 9"  'ver115-2d
    return
*/
}

void MainWindow::CloseDataWindow(QString hndl)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub CloseDataWindow hndl$    'ver115-1b changed to sub. Note this is never used anyway
    close #datawin:datawindow = 0
end sub
*/
}

void MainWindow::MSAinputData()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[MSAinputData]  'renamed ver115-5d
    if msaMode$<>"SA" and msaMode$<>"ScalarTrans" then goto [MagnitudePhaseMSAinput]    'Do phase if we have it
    gosub [OpenDataWindow]
    print #datawin," Step           Calc Mag  Mag AtoD Freq Cal"
    print #datawin," Num   Freq(MHz)  Input   Bit Val   Factor"
    validSteps=gPointCount()-1  'Number of completed steps  'ver116-1b
    for i = 0 to validSteps  'ver116-1b
        freq$=using("####.######",gGetPointXVal(i+1))   'freq in MHz
        data1$=using("####.###", datatable(i,2))    'calculated mag input
        data2$=using("######", magarray(i,3))       'Raw ADC bits
        data3$=using("####.###", freqCorrection(i)) 'Freq correction
        print #datawin, uAlignDecimalInString$(str$(i),4,4); _
                    uAlignDecimalInString$(freq$,12,5); _
                    uAlignDecimalInString$(data1$,9,5); _
                    uAlignDecimalInString$(data2$,8,7); _
                    uAlignDecimalInString$(data3$,11,5)
    next i
    #datawin, "!origin 1 1"
    wait
*/
}

void MainWindow::MagnitudePhaseMSAinput()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[MagnitudePhaseMSAinput]'ver115-4d
    gosub [OpenDataWindow]
    print #datawin," Step           Calc Mag  Mag A/D  Freq Cal Pha A/D Processed"
    print #datawin," Num   Freq(MHz)  Input   Bit Val   Factor  Bit Val    Phase"
    validSteps=gPointCount()-1  'Number of completed steps  'ver116-1b
    for i = 0 to validSteps  'ver116-1b
        freq$=using("####.######",gGetPointXVal(i+1))   'freq in MHz
        data1$=using("####.###", datatable(i,2))    'calculated mag input
        data2$=using("######", magarray(i,3))       'Raw ADC bits
        data3$=using("####.###", freqCorrection(i)) 'Freq correction
        data4$=using("######",phaarray(i,3)) 'Phase A/D. Bits ver115-5d
        data5$=using("####.##",datatable(i,3)) 'Phase Processed ver115-5d
        print #datawin, uAlignDecimalInString$(str$(i),4,4); _
                    uAlignDecimalInString$(freq$,12,5); _
                    uAlignDecimalInString$(data1$,9,5); _
                    uAlignDecimalInString$(data2$,8,7); _
                    uAlignDecimalInString$(data3$,11,5); _
                    uAlignDecimalInString$(data4$,8,7); _
                    uAlignDecimalInString$(data5$,10,5)
    next i
    #datawin, "!origin 1 1"
    wait
*/
}
void MainWindow::MagPhaS21()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*'ver113-5b
    gosub void MainWindow::OpenDataWindow()
    xclm$="!"
    print #datawin, gGetTitleLine$(1)   'ver115-6a put title in header
    print #datawin,"!select 1 1"
    print #datawin,"!insert xclm$"
    print #datawin, gGetTitleLine$(2)
    print #datawin,"!select 1 2"
    print #datawin,"!insert xclm$"
    print #datawin, gGetTitleLine$(3)
    print #datawin,"!select 1 3"
    print #datawin,"!insert xclm$"
    print #datawin, "# MHZ S DB R ";S21JigR0
    print #datawin, "  MHz       S21_Mag   S21_Ang"
    print #datawin,"!select 1 5"
    print #datawin,"!insert xclm$"
    validSteps=gPointCount()-1  'Number of completed steps  'ver116-1b
    for i = 0 to validSteps  'ver116-1b
        freq$=using("####.######",S21DataArray(i,0))
        data1$=using("####.#####",S21DataArray(i,1)) 'ver115-4d
        data2$=using("####.##",S21DataArray(i,2))
        print #datawin, uAlignDecimalInString$(freq$,11,4); _
                    uAlignDecimalInString$(data1$,9,5); _
                    uAlignDecimalInString$(data2$,8,5)
    next i
    #datawin, "!origin 1 1"
    wait
*/
}
void MainWindow::MagPhaS11()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*' ver115-2d
    gosub void MainWindow::OpenDataWindow()
    xclm$="!"
    print #datawin, gGetTitleLine$(1)   'ver115-6a put title in header
    print #datawin,"!select 1 1"
    print #datawin,"!insert xclm$"
    print #datawin, gGetTitleLine$(2)
    print #datawin,"!select 1 2"
    print #datawin,"!insert xclm$"
    print #datawin, gGetTitleLine$(3)
    print #datawin,"!select 1 3"
    print #datawin,"!insert xclm$"
    print #datawin, "# MHZ S DB R ";S11GraphR0
    print #datawin, " MHz       S11_Mag   S11_Ang"
    print #datawin,"!select 1 5"
    print #datawin,"!insert xclm$ +"
    validSteps=gPointCount()-1  'Number of completed steps  'ver116-1b
    for i = 0 to validSteps  'ver116-1b
        freq$=using("####.######",ReflectArray(i,0))
        data1$=using("####.#####",ReflectArray(i,1))  'ver115-4d
        data2$=using("####.##",ReflectArray(i,2))
        print #datawin, uAlignDecimalInString$(freq$,11,4); _
                    uAlignDecimalInString$(data1$,9,5); _
                    uAlignDecimalInString$(data2$,8,5)
    next i
    #datawin, "!origin 1 1"
    wait
*/
}
void MainWindow::DataWin_GraphData()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*'Display data for current graph(s) ver115-4h
    gosub void MainWindow::OpenDataWindow()
    xclm$="!"
    print #datawin, gGetTitleLine$(1)   'ver115-6a put title in header
    print #datawin,"!select 1 1"
    print #datawin,"!insert xclm$"
    print #datawin, gGetTitleLine$(2)
    print #datawin,"!select 1 2"
    print #datawin,"!insert xclm$"
    print #datawin, gGetTitleLine$(3)
    print #datawin,"!select 1 3"
    print #datawin,"!insert xclm$"
    #datawin,"Graph Data"
    print #datawin,"!select 1 4"
    print #datawin,"!insert xclm$"
    s$="Freq(MHZ)"
    if Y1DataType<>constNoGraph then
        call DetermineGraphDataFormat Y1DataType, y1AxisLabel$, dum1$,dum2,dum3$
        s$=s$;"      ";y1AxisLabel$
    end if
    if Y2DataType<>constNoGraph then
        call DetermineGraphDataFormat Y2DataType, y2AxisLabel$, dum$,dum2,dum3$
        s$=s$;"        ";y2AxisLabel$
    end if
    print #datawin, s$      'Data heading
    print #datawin,"!select 1 5"
    print #datawin,"!insert xclm$"
    validSteps=gPointCount()-1  'Number of completed steps  'ver116-1b
    for i = 0 to validSteps  'ver116-1b
        freq$=using("####.######",gGetPointXVal(i+1))
        call CalcGraphData i, y1Val, y2Val, 0 'Get Y1 and Y2 values
        if Y1DataType=constNoGraph then
            data1$=""
        else
            aVal=abs(y1Val)
            select case
                case aVal>=1000000
                    data1$=uScientificNotation$(y1Val, 6, 1) '6 decimals with zero padding
                case aVal>=1000
                    data1$=using("########.###",y1Val)
                case aVal>=0.000001
                    data1$=using("#####.######",y1Val)
                case else   'small values
                    data1$=uScientificNotation$(y1Val, 6, 1) '6 decimals with zero padding
            end select
        end if
        if Y2DataType=constNoGraph then
            data2$=""
        else
            aVal=abs(y2Val)
            select case
                case aVal>=1000000
                    data2$=uScientificNotation$(y2Val, 6, 1) '6 decimals with zero padding
                case aVal>=1000
                    data2$=using("########.###",y2Val)
                case aVal>=0.000001
                    data2$=using("#####.######",y2Val)
                case else   'small values
                    data2$=uScientificNotation$(y2Val, 6, 1) '6 decimals with zero padding
            end select
        end if
        if Y1DataType=constNoGraph then 'skip data1 if nonexistent ver115-9d
            print #datawin, uAlignDecimalInString$(freq$,11,4); _
                        uAlignDecimalInString$(data2$,22,8)
        else
            if Y2DataType=constNoGraph then
                print #datawin, uAlignDecimalInString$(freq$,11,4); _
                            uAlignDecimalInString$(data1$,22,8)
            else
                print #datawin, uAlignDecimalInString$(freq$,11,4); _
                            uAlignDecimalInString$(data1$,22,8); _
                            uAlignDecimalInString$(data2$,22,8)
            end if
        end if
    next i
    #datawin, "!origin 1 1"
    wait
*/
}
void MainWindow::LineCalArray()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*'ver115-2d
    gosub void MainWindow::OpenDataWindow()
    xclm$="!"
    if msaMode$="Reflection" then print #datawin,"Cal Reference" else print #datawin,"Line Calibration" 'ver115-4d
    print #datawin,"!select 1 1"
    print #datawin,"!insert xclm$ +"
    print #datawin, " Freq(MHz)   Cal_Mag Cal_Ang"
    print #datawin,"!select 1 2"
    print #datawin,"!insert xclm$ +"
    for i = 0 to steps
        freq$=using("####.######",gGetPointXVal(i+1))
        data1$=using("####.###",lineCalArray(i,1))
        data2$=using("####.##",lineCalArray(i,2))
        print #datawin, uAlignDecimalInString$(freq$,11,4); _
                    uAlignDecimalInString$(data1$,9,5); _
                    uAlignDecimalInString$(data2$,10,5)
    next i
    #datawin, "!origin 1 1"
    wait
*/
}

void MainWindow::DataWin_OSL()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[DataWin_OSL]'ver115-4a  'OSL calibration standards and coefficients
    gosub [OpenDataWindow]
    print #datawin," Freq(MHz)  Open_Real  Open_Imag    Short_Real    Short_Imag   Load_Real    Load_Imag   OSL_A_Real   OSL_A_Imag   OSL_B_Real   OSL_B_Imag   OSL_C_Real   OSL_C_Imag"
    for i = 0 to steps
        freq$=using("####.######",gGetPointXVal(i+1)) : OSLdataMin=1e-7
        if abs(OSLstdOpen(i,0))<OSLdataMin then Or$="0" else Or$=uScientificNotation$(OSLstdOpen(i,0), 5,0) 'ver116-4n
        if abs(OSLstdOpen(i,1))<OSLdataMin then Oi$="0" else Oi$=uScientificNotation$(OSLstdOpen(i,1), 5,0)
        if abs(OSLstdShort(i,0))<OSLdataMin then Sr$="0" else Sr$=uScientificNotation$(OSLstdShort(i,0), 5,0)
        if abs(OSLstdShort(i,1))<OSLdataMin then Si$="0" else Si$=uScientificNotation$(OSLstdShort(i,1), 5,0)
        if abs(OSLstdLoad(i,0))<OSLdataMin then Lr$="0" else Lr$=uScientificNotation$(OSLstdLoad(i,0), 5,0)
        if abs(OSLstdLoad(i,1))<OSLdataMin then Li$="0" else Li$=uScientificNotation$(OSLstdLoad(i,1), 5,0)
        if abs(OSLa(i,0))<OSLdataMin then Ar$="0" else Ar$=uScientificNotation$(OSLa(i,0), 5,0)
        if abs(OSLa(i,1))<OSLdataMin then Ai$="0" else Ai$=uScientificNotation$(OSLa(i,1), 5,0)
        if abs(OSLb(i,0))<OSLdataMin then Br$="0" else Br$=uScientificNotation$(OSLb(i,0), 5,0)
        if abs(OSLb(i,1))<OSLdataMin then Bi$="0" else Bi$=uScientificNotation$(OSLb(i,1), 5,0)
        if abs(OSLc(i,0))<OSLdataMin then Cr$="0" else Cr$=uScientificNotation$(OSLc(i,0), 5,0)
        if abs(OSLc(i,1))<OSLdataMin then Ci$="0" else Ci$=uScientificNotation$(OSLc(i,1), 5,0)
        print #datawin, uAlignDecimalInString$(freq$,11,4);" "; _
                    uAlignDecimalInString$(Or$,13,3); _
                    uAlignDecimalInString$(Oi$,13,3); _
                    uAlignDecimalInString$(Sr$,13,3); _
                    uAlignDecimalInString$(Si$,13,3); _
                    uAlignDecimalInString$(Lr$,13,3); _
                    uAlignDecimalInString$(Li$,13,3); _
                    uAlignDecimalInString$(Ar$,13,3); _
                    uAlignDecimalInString$(Ai$,13,3); _
                    uAlignDecimalInString$(Br$,13,3); _
                    uAlignDecimalInString$(Bi$,13,3); _
                    uAlignDecimalInString$(Cr$,13,3); _
                    uAlignDecimalInString$(Ci$,13,3)
    next i
    #datawin, "!origin 1 1"
    wait
*/
}

void MainWindow::ReflectDerivedData()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ReflectDerivedData]
    gosub [OpenDataWindow]
    print #datawin,"   Freq      S11_DB   S11_Ang  Rho    Z_Mag   Z_Ang     Rs       Xs       Cs       Ls      Rp       Xp      Lp      Cp     VSWR     RL    %RefPwr    Q"
    validSteps=gPointCount()-1  'Number of completed steps  'ver116-1b
    for i = 0 to validSteps  'ver116-1b
         print #datawin, AlignedReflectData$(i)
    next i
    #datawin, "!origin 1 1"
    wait
*/
}

QString MainWindow::AlignedReflectData(int currStep)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function AlignedReflectData$(currStep)  'Return string of formatted ReflectArray data
        aForm$="3,3,4//UseMultiplier//DoCompact"
        resForm$="3,3,4//UseMultiplier//SuppressMilli//DoCompact" 'ver115-4e
        freq$=using("####.######",ReflectArray(currStep,0))
        db$=using("####.#####",ReflectArray(currStep,constGraphS11DB))
        ang$=using("####.##",ReflectArray(currStep,constGraphS11Ang))
        rho$=using("#.###",ReflectArray(currStep,constRho))
        ZMag$=uFormatted$(ReflectArray(currStep,constImpedMag), resForm$)  'ver115-4e
        ZAng$=using("####.##",ReflectArray(currStep,constImpedAng))
        serR$=uFormatted$(ReflectArray(currStep,constSerR), resForm$)  'ver115-4e
        serReact$=uFormatted$(ReflectArray(currStep,constSerReact), resForm$)  'ver115-4e
        serC$=uFormatted$(ReflectArray(currStep,constSerC), aForm$)
        serL$=uFormatted$(ReflectArray(currStep,constSerL), aForm$)
        parR$=uFormatted$(ReflectArray(currStep,constParR), resForm$)  'ver115-4e
        parReact$=uFormatted$(ReflectArray(currStep,constParReact), resForm$)  'ver115-4e
        parC$=uFormatted$(ReflectArray(currStep,constParC), aForm$)
        parL$=uFormatted$(ReflectArray(currStep,constParL), aForm$)
        swr$=uFormatted$(min(9999,ReflectArray(currStep,constSWR)),"4,2,4") 'ver115-5d
        RL$=using("###.###",0-ReflectArray(currStep,constGraphS11DB))
        RefPow$=using("###.###",100*ReflectArray(currStep,constRho)^2)
        X=ReflectArray(currStep,constSerReact) : R=ReflectArray(currStep,constSerR)
        if R=0 then Q$="9999" else Q$=using("####.#",abs(X)/R) 'Q=X/R works for single L or C only

        AlignedReflectData$=uAlignDecimalInString$(freq$,11,4); _
                    uAlignDecimalInString$(db$,11,5); _
                    uAlignDecimalInString$(ang$,8,5); _
                    uAlignDecimalInString$(rho$,6,2); _
                    uAlignDecimalInString$(ZMag$,9,5); _
                    uAlignDecimalInString$(ZAng$,8,5); _
                    uAlignDecimalInString$(serR$,9,5); _
                    uAlignDecimalInString$(serReact$,9,5); _
                    uAlignDecimalInString$(serC$,8,5); _
                    uAlignDecimalInString$(serL$,8,5); _
                    uAlignDecimalInString$(parR$,9,5); _
                    uAlignDecimalInString$(parReact$,9,5); _
                    uAlignDecimalInString$(parC$,8,5); _
                    uAlignDecimalInString$(parL$,8,5); _
                    uAlignDecimalInString$(swr$,8,5); _
                    uAlignDecimalInString$(RL$,8,4); _
                    uAlignDecimalInString$(RefPow$,8,4); _
                    uAlignDecimalInString$(Q$,7,5)
end function


*/
  return "fix me";
}

//--SEW Added the following routines to save/copy the graph image
//These routines are invoked through the File and Edit menus
//Scotty: I created additional variables here, all of which begin with captial X
//to avoid any conflicts with your variables.
//=====================Start Routines to Save/Copy Image===========================

void MainWindow::SaveImage()
{
  //Save graph image to file
  if (graph.haltsweep==1)
  {
    QMessageBox::information(this, "", "Sweep will halt at end. Then re-click Save Image.");
    graph.haltAtEnd=1;     //Set flag to cause halt at end of this sweep.
    PostScan();
    return;
  }
  if (vars->isStickMode==0) {graph.refreshGridDirty=1; graph.RefreshGraph(0); }  //Redraw without erasure mark; but not if we are in stick mode ver114-7d

  QString filter="Bitmap files(*.bmp);;All files(*.*)"; //ver115-6b
  QString defaultExt="bmp";
  QString initialDir=vars->imageSaveLastFolder+"/";
  QString initialFile="";
  QString graphFileName=uSaveFileDialog(filter, defaultExt, initialDir, initialFile, "Save Image To File");
  if (graphFileName!="")   //blank means cancelled
  {
    QImage image(ui->graphicsView->scene()->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
    image.fill(Qt::transparent);                                              // Start all pixels transparent

    QPainter painter(&image);
    ui->graphicsView->scene()->render(&painter);
    image.save(graphFileName);

    QString dum;
    uParsePath(graphFileName, vars->imageSaveLastFolder, dum); //Save folder in which file was saved ver115-2a
  }
}

QString MainWindow::FullGraphBmp()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function FullGraphBmp$() 'return name of bitmap for entire graph--grid and surrounding info
            'Get Image size by finding center positions and doubling them
    call HideButtonsOnGraph  'ver114-4f
    #graphBox$, "home"
    #graphBox$, "posxy CenterX CenterY"
    clientWidth = CenterX * 2-1
    clientHeight = CenterY * 2-1
    print #graphBox$, "getbmp graphbmp 0 0 ";clientWidth;" ";clientHeight
    call ShowButtonsOnGraph  'ver114-4f
    FullGraphBmp$="graphbmp"
end function*/
  return "fix me";
}

void MainWindow::CopyImage()
{
  //Copy graph image to clipboard
  if (graph.haltsweep==1)
  {
    QMessageBox::warning(0,"", "Sweep will halt at end. Then re-click Copy Image.");
    graph.haltAtEnd=1;     //Set flag to cause halt at end of this sweep.
    PostScan();
    return;
  }
  if (vars->isStickMode==0) { graph.refreshGridDirty=1; graph.RefreshGraph(0);}  //Redraw without erasure mark; but not if we are in stick mode ver114-7d

  QImage image("fn");
  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing);
  graph.getScene()->render(&painter);

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->clear();
  clipboard->setPixmap(QPixmap::grabWidget(ui->graphicsView));
}
//==================End of Save/Copy Image================*/

void MainWindow::HideButtonsOnGraph()
{
  //Hide buttons on the graph box so image can be copied
  //ver114-7b deleted the scale arrow buttons so there is nothing at the moment to hide
}
void MainWindow::ShowButtonsOnGraph()
{
  //Show buttons that lie on the graph box
  //ver114-7b deleted the scale arrow buttons so there is nothing at the moment to hide
}
void MainWindow::menuRLCAnalysis()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    if haltsweep=1 then gosub void MainWindow::FinishSweeping()  //Halt
    if msaMode$="Reflection" then call ReflectionRLC else call TranRLCAnalysis
    call RequireRestart     //Scan can continue only by Restart
    wait
*/
}
//ver115-5d added void MainWindow::menuS11ToS21()
void MainWindow::menuS11ToS21()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    if haltsweep then gosub void MainWindow::FinishSweeping()    //stop sweeping cleanly
    call S11ToS21   //Do conversion
    call RequireRestart     //Scan can continue only by Restart
    wait
*/
}

void MainWindow::ImpedToRLC() // connect$,centerStep, nLeft,nRight, resonStep, byref R, byref L, byref C
{
 qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Determine RLC values from ReflectArray data
    //If connect$="Series" then we have series RLC; otherwise parallel.
    //We analyse the slope of reactance or susceptance to determine L and C.
    //Slope is determined by best fit line of nPoints points centered at centerStep (0...)
    //and including nLeft points to the left and nRight points to the right.
    //if resonStep>0 then it is the step (perhaps not integral) of the resonant frequency
    //SERIES:
    //If the impedance is R+jX, then series R=R, and the
    //L and C combine to produce a reactance of X. We determine L and C by looking at the change in X
    //with respect to frequency : dX/dw, where w=2*pi*freq.
    //X=wL-1/wC
    //D=dX/dw=L+1/(C*w^2)
    //Once we know X and D, we determine L and C as follows:
    //If we know the resonant frequency (W),
    //  C=1/(DW^2) + 1/(Dw^2)  If w (freq where we know slope) equals W, then C=2/(DW^2)
    //  L=1/(CW^2)
    //If we don't know the resonant frequency,
    //  C=2/(w(wD-X))
    //  L=(X+1/(wC))/w

    //PARALLEL: We look at susceptance S=-X/(R^2+X^2) instead of reactance. D is then the slope of
    //susceptance, and the formulas for C and L are:
    //L=2/(w(wD-S))
    //C=(Y+1/(wL))/w
    //If we know the resonant frequency (W),
    //  L=1/(DW^2) + 1/(Dw^2)  If w (freq where we know slope) equals W, then C=2/(DW^2)
    //  C=1/(LW^2)
    //If we don't know the resonant frequency,
    //  L=2/(w(wD-S))
    //  C=(S+1/(wL))/w

    twoPiMillion=2*uPi()*1000000
    sumX=0 : sumY=0 : sumXY=0 : sumXSquared=0 : sumR=0
    if nLeft<0 then nLeft=0
    if nRight<0 then nRight=0
    if nLeft=0 and nRight=0 then nRight=1
    startStep=centerStep-nLeft : endStep=centerStep+nRight
    if startStep<0 or endStep>gPointCount()+1 then _
        notice "Invalid point specifications for RLC analysis" : R=0 : L=0 : C=0 : exit sub
    nPoints=nLeft+nRight+1
    for i=startStep to endStep
        x=ReflectArray(i, 0)
        y=ReflectArray(i,constSerReact)  //Reactance
        if connect$<>"Series" then
            R=ReflectArray(i,constSerR)  //Series resistance
            call cxInvert R, y, G, S //invert impedance to get admittance
            y=S
        end if
        sumX=sumX+x : sumY=sumY+y
        sumXY=sumXY+x*y : sumXSquared=sumXSquared+x*x
        if i=centerStep then centerXorS=y   //save middle impedance or susceptance
    next i
    if connect$<>"Series" then R=ReflectArray(centerStep,constSerR) else R=ReflectArray(centerStep,constParR)
    m=(nPoints*sumXY-sumX*sumY)/(nPoints*sumXSquared-sumX*sumX) //Slope of reactance or susceptance, ohms or siemens per million Hz
    w=twoPiMillion*ReflectArray(centerStep,0)   //frequency, radians/sec
    if w<=0 then notice "Frequency error in ImpedToRLC" : R=constMaxValue :L=constMaxValue : C=constMaxValue : exit sub
    D=m/twoPiMillion   //To get slope in ohms/(rad/sec)

    if resonStep>0 then
        //we know the resonant frequency
        Wres=twoPiMillion*gGetPointXVal(resonStep+1)
        if connect$="Series" then
            C=1/(D*Wres^2) + 1/(D*w^2)
            L=1/(C*Wres^2)
        else
            L=1/(D*Wres^2) + 1/(D*w^2)
            C=1/(L*Wres^2)
        end if
    else    //don't know resonance. Use info about centerXorS instead.
        denom=w*(w*D-centerXorS)
        if connect$="Series" then
            if denom=0 then C=1 else C=2/denom
            L=(centerXorS+1/(w*C))/w
        else
            if denom=0 then L=1 else L=2/denom
            C=(centerXorS+1/(w*L))/w
        end if
    end if
    if C<0 then C=0 //min of zero, max of 1
    if L<0 then L=0
    if C>1 then C=1
    if L>1 then L=1
end sub
*/
}
void MainWindow::DetermineLCEquiv() // connect$,startStep, endStep, resonStep
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Determine equivalent LC values from uWorkArray(,) data
    //If connect$="Series" then we have series RLC; otherwise parallel.
    //uBestFitLines must already have been called to set up uWorkArray with the necessary values
    //and slope for reactance (if series) or susceptance (if parallel).
    //We analyse the slope of reactance or susceptance to determine L and C.
    //We put L into auxGraphData(,0) and C into auxGraphData(,1), and unloaded Q into auxGraphData(,2)
    //if resonStep>0 then it is the step (perhaps not integral) of the resonant frequency
    //SERIES:
    //If the impedance is R+jX, then series R=R, and the
    //L and C combine to produce a reactance of X. We determine L and C by looking at the change in X
    //with respect to frequency : dX/dw, where w=2*pi*freq.
    //X=wL-1/wC
    //D=dX/dw=L+1/(C*w^2)
    //Once we know X and D, we determine L and C as follows:
    //If we know the resonant frequency (W),
    //  C=1/(DW^2) + 1/(Dw^2)  If w (freq where we know slope) equals W, then C=2/(DW^2)
    //  L=1/(CW^2)
    //If we don't know the resonant frequency,
    //  C=2/(w(wD-X))
    //  L=(X+1/(wC))/w

    //PARALLEL: We look at susceptance S=-X/(R^2+X^2) instead of reactance. D is then the slope of
    //susceptance, and the formulas for C and L are:
    //S=wC-1/(wL)
    //D=dS/dw=C+1/(L*w^2)
    //If we know the resonant frequency (W),
    //  L=1/(DW^2) + 1/(Dw^2)  If w (freq where we know slope) equals W, then L=2/(DW^2)
    //  C=1/(LW^2)
    //If we don't know the resonant frequency,
    //  L=2/(w(wD-S))
    //  C=(S+1/(wL))/w

        //Make sure all frequencies are positive
    if ReflectArray(startStep,0)<=0 then
        notice "Frequencies must be positive for RLC analysis"
            //Set R,L and C to large values
        for currStep=startStep to endStep
            auxGraphData(currStep,0)=constMaxValue : auxGraphData(currStep,1)=constMaxValue : auxGraphData(currStep,2)=constMaxValue
        next currStep
        exit sub
    end if

    twoPiMillion=2*uPi()*1000000
    maxL=1e-9 : maxC=1e-12 : maxQ=1  //For determining graph axis range. These may change
    call gGetMinMaxPointNum pMin, pMax
    minStep=pMin-1 : maxStep=pMax-1
    if resonStep<minStep or resonStep>maxStep then resonStep=-1 //invalid so we don't know resonance
    if resonStep<0 then Wres=-1 else Wres=twoPiMillion*gGetPointXVal(resonStep+1) //resonant freq in radians/sec ver115-4e
    if startStep<minStep then startStep=minStep
    if endStep>maxStep then endStep=maxStep
    for currStep=startStep to endStep
        m=uWorkArray(currStep,3)  //Slope of reactance or susceptance, ohms or siemens per million Hz
        w=twoPiMillion*ReflectArray(currStep,0)   //frequency, radians/sec
        D=m/twoPiMillion   //To get slope in ohms/(rad/sec)
        XorS=uWorkArray(currStep, 1)    //Reactance or susceptance
        if resonStep>0 then
            //we know the resonant frequency. This implicitly assumes the L and C values are the same
            //at this point and at resonance, which may not be a good idea. User can avoid this by
            //not including resonant frequency in the scan.
            if connect$="Series" then
                C=1/(D*Wres^2) + 1/(D*w^2)
                L=1/(C*Wres^2)
            else
                L=1/(D*Wres^2) + 1/(D*w^2)
                C=1/(L*Wres^2)
            end if
        else    //don't know resonance. Use info about reactance or susceptance instead.
            denom=w*(w*D-XorS)
            if connect$="Series" then
                if denom=0 then C=1 else C=2/denom
                L=(XorS+1/(w*C))/w
            else
                if denom=0 then L=1 else L=2/denom
                C=(XorS+1/(w*L))/w
            end if
        end if
        num=abs(XorS)+w*D  //ver115-4b
        if connect$="Series" then
            R=uWorkArray(currStep,2)    //resistance
            if R<0.00001 then Q=9999 else Q=num/(2*R) //ver115-4b
        else
            G=uWorkArray(currStep,2)    //conductance
            if G<0.00001 then Q=9999 else Q=num/(2*G) //ver115-4b
        end if
        if C<0 then C=0 //min of zero, max of 1
        if L<0 then L=0
        if C>1 then C=1
        if L>1 then L=1
        if Q<0 then Q=-1
        if Q>1e6 then Q=1e6
        if L>maxL and L<1 then maxL=L
        if C>maxC and C<1 then maxC=C
        if Q>maxQ and Q<1000 then maxQ=Q
        auxGraphData(currStep,0)=L  //inductance
        auxGraphData(currStep,1)=C  //capacitance
        auxGraphData(currStep,2)=Q  //Qu
    next currStep

    //We must now describe the data in auxGraphData for the graphing routines
    LCForm$="3,3,4//UseMultiplier"
    if connect$="Series" then
        s1$="Series" : s2$="Ser"
    else
        s1$="Parallel" : s2$="Par"
    end if

    auxGraphDataFormatInfo$(0,0)="LC-";s1$;" L" : auxGraphDataFormatInfo$(0,1)=LCForm$  //Inductance graph
    auxGraphDataFormatInfo$(0,2)="LC-";s1$;" L": auxGraphDataFormatInfo$(0,3)="LC-";s2$;" L"
    auxGraphDataInfo(0,0)=0 //not an angle
    auxGraphDataInfo(0,1)=0 //axis min
    auxGraphDataInfo(0,2)=util.uRoundUpToPower(maxL,10)  //axis max is power of 10

    auxGraphDataFormatInfo$(1,0)="LC-";s1$;" C" : auxGraphDataFormatInfo$(1,1)=LCForm$  //Capacitance graph
    auxGraphDataFormatInfo$(1,2)="LC-";s1$;" C": auxGraphDataFormatInfo$(1,3)="LC-";s2$;" C"
    auxGraphDataInfo(1,0)=0 //not an angle
    auxGraphDataInfo(1,1)=0 //axis min
    auxGraphDataInfo(1,2)=util.uRoundUpToPower(maxC,10)  //axis max is power of 10

    QForm$="3,2,3//UseMultiplier//SuppressMilli"    //ver115-4e
    auxGraphDataFormatInfo$(2,0)=s1$;" Q" : auxGraphDataFormatInfo$(2,1)=QForm$     //Q graph
    auxGraphDataFormatInfo$(2,2)=s1$;" Q" : auxGraphDataFormatInfo$(2,3)=s2$;" Q"
    auxGraphDataInfo(2,0)=0 //not an angle
    auxGraphDataInfo(2,1)=0 //axis min
    if maxQ<=9 then maxQ=10 else if maxQ<=25 then maxQ=30 else maxQ=100
    auxGraphDataInfo(2,2)=maxQ

end sub
*/
}
void MainWindow::menuQ()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Menu for Q factors was selected
    if haltsweep=1 then gosub void MainWindow::FinishSweeping()
    call AnalyzeQ
    call ChangeGraphsToAuxData constAux0, constAux1  //graph series Q and parallel Q
    wait
*/
}
void MainWindow::ChangeGraphsToAuxData()// aux1, aux2
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Change to specified graph types, which may be constNoGraph
    //This is used primarily to graph one or two auxiliary graphs created by a Function. The parameters for
    //auxiliary graphs are in auxGraphDataInfo
    //aux1 and aux2 are constants for aux data (constAux0...) or constNoGraph or other graph data constant
    //ver115-8b changed this to allow non-auxiliary data
    if aux1>=constAux0 and aux1<=constAux5 then
        auxIndex1=aux1-constAux0    //produces 0 for constAux0, 1 for constAux1, etc.
        axis1Min=auxGraphDataInfo(auxIndex1,1) : axis1Max=auxGraphDataInfo(auxIndex1,2) //function determined these
    else    //Get default axis limits for non-auxiliary data
        call StartingLimits aux1, constNoGraph, axis1Min, axis1Max
    end if
    if aux2>=constAux0 and aux2<=constAux5 then
        auxIndex2=aux2-constAux0
        axis2Min=auxGraphDataInfo(auxIndex2,1) : axis2Max=auxGraphDataInfo(auxIndex2,2)
    else    //Get default axis limits for non-auxiliary data
        call StartingLimits aux2, constNoGraph, axis2Min, axis2Max
    end if
    if aux1=constNoGraph and aux2=constNoGraph then exit sub    //don't do anything if both are none--error condition
    Y1DataType=aux1    //set Y1 graph data
    Y2DataType=aux2    //set Y2 graph data
    if aux1<>constNoGraph then call SetY1Range axis1Min, axis1Max //Y1 min and max
    if aux2<>constNoGraph then call SetY2Range axis2Min, axis2Max //Y2 min and max
    call UpdateGraphDataFormat 0   //To get new data format
    call gCalcGraphParams   //Calculate new scaling.
    call RecalcYValues
    call RedrawGraph 0  //Draw the new graphs
end sub
*/
}
void MainWindow::QFactors(int nPoints)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Calculate Q factors for each frequency. Put results in auxGraphData(,)
    //Slopes will be calculated with best fit to nPoints
    //We put results into auxGraphData(0...,): seriesQ(,0) and parallelQ(,1)
    call gGetMinMaxPointNum pMin, pMax
    startStep=pMin-1 : endStep=pMax-1

        //Calculate reactance slopes
    call uBestFitLines constSerReact, constSerR,nPoints, startStep, endStep
    for i=startStep to endStep : print i;" ";uWorkArray(i,3);" ";uWorkArray(i,4) : next i   //DEBUG
    //Calculate series Q and parallel Q put results into auxGraphData(,0) and auxGraphData(,1)
    //uWorkArray(,) contains frequency (,0), reactance (,1), resistance (,2),
    //reactance slope (,3) and reactance intercept (,4)

    twoPiMillion=2000000*uPi()
    maxSerQ=1 : maxParQ=1
    for i=startStep to endStep  //For each step calculate apparentQ and seriesQ.
        w=twoPiMillion*uWorkArray(i,0)
        absX=abs(uWorkArray(i,1))
        Rs=uWorkArray(i,2)
        dX=uWorkArray(i,3)/twoPiMillion     //current slope is based on million Hz; we want rads/sec
        if Rs=0 then Q=99999 else Q=(absX+w*dX)/(2*Rs)
        auxGraphData(i,0)=Q     //series Q
        if Q>maxSerQ then maxSerQ=Q
    next i
    //Calculate susceptance slopes
    call uBestFitLines constSusceptance, constConductance, nPoints, startStep, endStep
    //Calculate parallel Q
    //uWorkArray(,) contains frequency (,0), susceptance (,1), conductance (,2),
    //susceptance slope (,3) and susceptance intercept (,4)
    for i=startStep to endStep  //For each step calculate parallel Q
        w=twoPiMillion*uWorkArray(i,0)
        absS=abs(uWorkArray(i,1))
        G=uWorkArray(i,2)
        dS=uWorkArray(i,3)/twoPiMillion     //current slope is based on million Hz; we want rads/sec
        if G=0 then Q=99999 else Q=(absS+w*dS)/(2*G)
        auxGraphData(i,1)=Q     //parallel Q
        if Q>maxParQ then maxParQ=Q
    next i

    //We must now describe the data in auxGraphData for the graphing routines
    QForm$="3,2,3//UseMultiplier//SuppressMilli"    //ver115-4e
    auxGraphDataFormatInfo$(0,0)="Series Q" : auxGraphDataFormatInfo$(0,1)=QForm$
    auxGraphDataFormatInfo$(0,2)="Series Q" : auxGraphDataFormatInfo$(0,3)="Ser Q"
    auxGraphDataInfo(0,0)=0 //not an angle
    auxGraphDataInfo(0,1)=0 //axis min
    if maxSerQ<=9 then maxSerQ=10 else if maxSerQ<=25 then maxSerQ=30 else maxSerQ=100
    auxGraphDataInfo(0,2)=maxSerQ

    auxGraphDataFormatInfo$(1,0)="Parallel Q" : auxGraphDataFormatInfo$(1,1)=QForm$
    auxGraphDataFormatInfo$(1,2)="Parallel Q" : auxGraphDataFormatInfo$(1,3)="Par Q"
    auxGraphDataInfo(1,0)=0
    auxGraphDataInfo(1,1)=0
    if maxParQ<=9 then maxParQ=10 else if maxParQ<=25 then maxParQ=30 else maxParQ=100
    auxGraphDataInfo(1,2)=maxParQ
end sub
*/
}
void MainWindow::menuSaveDataFile()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//ver115-5f
    if haltsweep=1 then gosub void MainWindow::FinishSweeping()

    filter$="All files" + chr$(0) + "*.*" + chr$(0) + _
                "Parameter files" + chr$(0) + "*.s1p" + chr$(0) + _
                "Text files" + chr$(0) + "*.txt"   //ver115-6b
    defaultExt$="s1p"
    initialDir$=touchLastFolder$+"\"
    initialFile$=""
    dataFileName$=uSaveFileDialog$(filter$, defaultExt$, initialDir$, initialFile$, "Save Data To File")
    if dataFileName$="" then wait //blank means cancelled
    call SaveDataFile dataFileName$, 0     //Save data without preferences
    wait
*/
}
void MainWindow::SaveDataFile() //dataFileName$, doContexts
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Save current data in file dataFileName$
    //if doContexts=1, also save the contexts flagged in contextTypes
    //RestoreVNAData will be used to re-load data. It assumes VNA data is ready to go directly to
    //ReflectArray or S21DataArray, without certain adjustments such as plane extension and OSL calibration.
    //(of course, the calibration data may not even be available when the file is loaded.) Therefore,
    //in VNA modes we save the data from ReflectArray or S21DataArray, not from datatable.

    call uParsePath dataFileName$, touchLastFolder$, dum$ //Save folder we are using
    if doContexts=0 then    //flags indicate what to save
        for i=0 to 30 : contextTypes(i)=0 : next i
    end if
//    contextTypes(constGrid)=1   //DEBUG--caller should set context flags if desired
//    contextTypes(constTrace)=1
//    contextTypes(constSweep)=1
    contextTypes(constModeData)=1  //Indicate to save data along with whatever else is to be saved
    errMsg$=SaveContextFile$(dataFileName$)
    if errMsg$<>"" then notice errMsg$
end sub
*/
}
void MainWindow::RestoreVNAData()
{
  //Load data for current msaMode$ from VNAData(), which holds steps+1 points //added by ver116-1b
  //Note that VNAData may actually hold data from SA mode
  //We generate only those context changes necessary to accomodate the data
  //Note caller must restore title, preferably before calling us
  //Note caller must change msaMode$ via void MainWindow::ChangeMode() if necessary to suit the data
  //Caller should redim VNAData to a small size when done with it.
  //Caller must deal with whether plane extension must be forced to zero, and how to set the various
  //jig and graph R0 values, based on whether we know how the data was gathered. If we don't, plane
  //extension should be zeroed and S21JigR0 and S11BridgeR0 should be set to the VNADataZ0. ver116-4j
  //If user saved the pre-R0-conversion-and-plane-extension, then VNARestoreDoR0AndPlaneExt should be set to 1
  //so we apply R0 conversion and plane extension here. ver116-4j
    //See if sweep settings need to be changed. We use the same touchxxx variables used when reading touchstone files.

  float touchStartFreq = vars->VNAData[0][0];
  float touchEndFreq = vars->VNAData[vars->VNADataNumSteps][0];    //Start and Stop freq of data
  int touchIsLinear=vars->VNADataLinear;    //ver116-4a

    //If settings need to be changed, then do so
    //touchIsLinear, touchStartFreq and touchEndFreq are local
  if (vars->startfreq != touchStartFreq
      || vars->endfreq !=touchEndFreq
      || graph.gGetXIsLinear() != touchIsLinear
      || vars->globalSteps != uWork.uWorkNumPoints-1)
  {
    //If freq range or sweep type changed, we need to adjust to it.
    vars->steps=vars->VNADataNumSteps;
    vars->globalSteps=vars->steps;
    graph.gSetNumDynamicSteps(vars->globalSteps);
    graph.SetStartStopFreq(touchStartFreq, touchEndFreq);
    graph.gSetXIsLinear(touchIsLinear);
    //We will do PartialRestart below which will further implement these changes
  }

  //The data is now in VNAData(0,x) to VNAData(steps, x)
  //We need to put it where it belongs, based on the mode we are in
  //Note we do not verify that the file data matches our mode.
  for (int stepNum=0; stepNum <= vars->steps; stepNum++)
  {
    float thisfreq=vars->VNAData[stepNum][0]; //freq
    vars->datatable[stepNum][1]=thisfreq;
    vars->datatable[stepNum][2]=vars->VNAData[stepNum][1]; //dB or dBm
    if (vars->msaMode=="SA" || vars->msaMode=="ScalarTrans")
    {
        vars->datatable[stepNum][3]=0;
    }
    else
    {
      float ang=vars->VNAData[stepNum][2]; //angle
        while (ang>180)
        {
          ang=ang-360;
        }  //In case data range of angle was unusual
        while (ang<=-180)
        {
          ang=ang+360;
        }
        vars->datatable[stepNum][3]=ang; //angle
    }
  }
    //Data is now in datatable, but we have to generate the graph, which also puts the data
    //into the graph module and into S21DataArray or ReflectArray if necessary.

  for (int i=1; i <= 4; i++)    //Set title; only 3 lines print
  {
    graph.gSetTitleLine(i, vars->VNADataTitle[i]);
  }

  vnaCal.SignalNoCalInstalled();   //To get valid cal installed, if applicable ver116-4b
    //Do Restart with a flag to return before taking any data
  vars->suppressHardwareInitOnRestart=1; //no need for hardware init ver116-4d
  PartialRestart();

  //We must now recreate the graph. We proceed as though we just gathered the data
  //point by point
  for (vars->thisstep=vars->sweepStartStep; vars->thisstep != vars->sweepEndStep; vars->thisstep = vars->thisstep + vars->sweepDir)
  {
    //Plot this point the same as though we just gathered it.
    if (vars->msaMode!="SA")
      TransferToDataArrays();   //Enter data into S21DataArray or ReflectArray
    if (vars->VNARestoreDoR0AndPlaneExt)  //If user so specified, perform R0 conversion and plane extension
    {
        if (vars->msaMode=="VectorTrans" || vars->msaMode=="ScalarTrans")
        {
            if (vars->planeadj!=0)
            {
                float phaseToExtend=vars->S21DataArray[vars->thisstep][2];
                float phaseToAdj;
                util.uExtendCalPlane(vars->thisfreq, phaseToAdj, vars->planeadj,0);  //Do plane adjustment ver116-1b //ver116-4s
                vars->S21DataArray[vars->thisstep][2]=phaseToExtend;
            }
        }
        else
        {
            //ISSUE--If OSL was not used, we don't have the raw data from the series or shunt fixture,
            //so we need to force the fixture to Bridge. Or do we. Does any routine other than ConvertRawData... access raw data?
            if (vars->msaMode=="Reflection")
            {
                if (vars->planeadj!=0 || vnaCal.S11BridgeR0!=vnaCal.S11GraphR0)
                {
                    float f=vars->VNAData[vars->thisstep][0];
                    float db=vars->VNAData[vars->thisstep][1];
                    float ang=vars->VNAData[vars->thisstep][2];
                    ApplyExtensionAndTransformR0(f, db, ang);
                    vars->ReflectArray[vars->thisstep][vars->constGraphS11DB]=db;   //Save final S11 in db, angle format (in Graph R0, after plane ext)
                    while (ang>180)
                    {
                      ang=ang-360;
                    }
                    while (ang<=-180)
                    {
                      ang=ang+360;
                    }
                    vars->ReflectArray[vars->thisstep][vars->constGraphS11Ang]=ang;
                }
            }
        }
    }
    if (vars->msaMode=="Reflection")
      graph.CalcReflectDerivedData(vars->thisstep);
    PlotDataToScreen();
  }

    //void MainWindow::PlotDataToScreen() will have refreshed the graph at the final data point, unless
    //refreshEachScan is turned off. We want to be sure a refresh gets done.
  if (graph.refreshEachScan==0)
    graph.RefreshGraph(0);

    //Restart puts new time stamp in title, so put ours back in
  QString t3="      "+vars->VNADataTitle[3]+"      ";
  if (t3.length()<35)
    t3="          "+t3+"          ";
  graph.gSetTitleLine(3, t3);   //Extra blanks for clearing in case size changed

  //Reprint title without clearing; new data will cover old data
  //The refresh routine will have used saved info for faster drawing, and so will
  //have the time stamp from PartialRestart done in void MainWindow::LoadDataFromWorkArray(), which we don't want.

  graph.gPrintTitle(0);
  //#graphBox$, "flush"
  graph.gSetTitleLine(3,vars->VNADataTitle[3]);  //Get rid of extra blanks
  return;

}
void MainWindow::regraphDatatable()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[regraphDatatable]  'Regraph datatable as we did when we collected it, applying OSL, plane adj and graphR0 transforms
    'This is similar to the last part of [RestoreVNAData], but there we did not want OSL to be applied, because
    'the data was assumed already to be calibration-adjusted. [regraphDatatable] makes sense only if we know all
    'original calibration data is still intact, as we do when returning from the Two Port module
    call SignalNoCalInstalled   'To get valid cal installed, if applicable ver116-4j
    call uBringWindowToFront hwnd(#handle)  'bring graph to front
    saveTimeStamp$=gGetTitleLine$(3)
    suppressHardwareInitOnRestart=1 'no need for hardware init ver116-4d
    gosub [PartialRestart]

    'We must now recreate the graph. We proceed as though we just gathered the data
    'point by point
    for thisstep=sweepStartStep to sweepEndStep step sweepDir
        'Plot this point the same as though we just gathered it.
        'Apply planeadj and graph R0 transformed and put info into S21DataArray or ReflectArray
        if msaMode$<>"SA" then gosub [ProcessDataArrays]
        gosub [PlotDataToScreen]
    next thisstep

        '[PlotDataToScreen will have refreshed the graph at the final data point, unless
        'refreshEachScan is turned off. We want to be sure a refresh gets done.
    if refreshEachScan=0 then call RefreshGraph 0

        'Restart puts new time stamp in title, so put ours back in
    t3$="      ";saveTimeStamp$;"      " : if len(t3$)<35 then t3$="          ";t3$;"          "
    call gSetTitleLine 3, t3$   'Extra blanks for clearing in case size changed

    'Reprint title without clearing; new data will cover old data
    'The refresh routine will have used saved info for faster drawing, and so will
    'have the time stamp from PartialRestart done in [LoadDataFromWorkArray], which we don't want.
    call gPrintTitle 0
    #graphBox$, "flush"
    call gSetTitleLine 3, saveTimeStamp$  'Get rid of extra blanks
return
*/
}
void MainWindow::menuLoadDataFile()
{
   raise();
//ver115-5f
  if (graph.haltsweep==1) FinishSweeping();
  QString filter="Parameter files (*.s1p);;"
              "Text files (*.txt);;"
              "CSV files (*.csv);;"
              "All files (*.*)"; //ver115-6b
  QString defaultExt="s1p";
  QString initialDir= touch.touchLastFolder;//+"/";
  QString initialFile="";
  QString dataFileName=uOpenFileDialog(filter, defaultExt, initialDir, initialFile, "Open Data File");
  if (dataFileName=="")
    return;   //user cancelled

  QFile *dataHandle=touch.touchOpenInputFile(dataFileName);
  if (dataHandle==NULL)
  {
    vars->restoreErr="File failed to open: "+dataFileName;
    return;
  }
  QString dum;
  uParsePath(dataFileName, touch.touchLastFolder, dum); //Save folder from which file was loaded
  vars->restoreFileName=dataFileName;
  vars->restoreFileHndl=dataHandle;
  LoadDataFileWithContext(dataFileName);
  RequireRestart();
}
void MainWindow::LoadDataFileWithContext(QString dataFileName)
{
  //Load data from restoreFileName$, which is already open as restoreFileHndl$. We close it when done
  //Set restoreErr$ to error message or blank.
  //The data may be preceded by preference info, so we read and react to that first ver115-8c
  setCursor(Qt::WaitCursor);
  //Want baseFrequency=0 unless explicitly changed by loading a context.
  vars->baseFrequency=0;
  RememberState();  //So we can see what changed //ver115-8c
  LoadBasicContextsFromFile();   //Load preferences from restoreFileHndl$
  if (vars->restoreErr!="")
  {
    QMessageBox::warning(this, "Error", "Error loading file: "+vars->restoreErr);
    setCursor(Qt::ArrowCursor);
    return;
  }
  int contextLoaded=0; //will be set to 1 if any context was loaded above
  for (int i=0; i <= 30; i++)
  {
    if (vars->contextTypes[i]==1)
    {
      contextLoaded=1;
      break;
    }
  }
  if (contextLoaded)
  {
    DetectFullChanges(); //ver115-8c
    if (graph.continueCode==3)
      PartialRestart(); //implement changes ver115-3c
  }

  //Now we proceed to the actual data. Note that the above will already have read the first line
  //of the data section. If we created the data file, we would have made the first line StartContext Datatable.
  //A file from an external source will likely start with a comment, or might start with actual data. In the latter case
  //we need to start over at the beginning of the file, so as not to miss anything. If there is any context info
  //at the start of the file, we know we created the file, so no need to back up.
  if (contextLoaded==0)
  {
    //No contexts were read; close and reopen the file
    vars->restoreFileHndl->close();
    delete vars->restoreFileHndl;
    vars->restoreFileHndl=touch.touchOpenInputFile(vars->restoreFileName);
    if (vars->restoreFileHndl==NULL)
    {
      vars->restoreErr="File failed to open: "+dataFileName;
      setCursor(Qt::ArrowCursor);
      return;
    }
  }

  touch.touchReadParams(vars->restoreFileHndl,1);  //Read data from file into uWorkArray
  vars->restoreFileHndl->close();
  delete vars->restoreFileHndl;

  if (touch.touchBadLine>0)
  {
    QMessageBox::warning(this, "Error", "File Error in Line "+QString::number(touch.touchBadLine));
    setCursor(Qt::ArrowCursor);
    return;
  }   //touchReadParams sets touchBadLine if error
  if (uWork.uWorkNumPoints<2)
  {
    QMessageBox::warning(this, "Error", "File must contain two or more points");
    setCursor(Qt::ArrowCursor);
    return;
  }
    //If the file had more points than our arrays can handle, then resize them.
    //Note this does not resize uWorkArray, which would erase its data.
  if (uWork.uWorkNumPoints>=(unsigned int)graph.gMaxNumPoints())
    ResizeArrays(uWork.uWorkNumPoints+10); //ver115-6b

    //Note that we don//t make the plane extension or R0 adjustments that [ProcessDataArrays] makes. If we
    //have loaded the context with the data, we presume the data already is adjusted per the context. If we
    //did not load the context, we will force plane ext and R0 to avoid need for adjustments.
  if (contextLoaded==0)
  {
    vars->planeadj=0; vnaCal.S21JigR0=touch.touchRef;  //ver116-4j
    if (vars->msaMode=="Reflection")
      vnaCal.S11BridgeR0=touch.touchRef; //ver116-4j
  }

  //The data is now in uWorkArray(1,x) to uWorkArray(uWorkNumPoints, x)
  for (int i=1; i <= 4; i++)    //Get title from comments; line 4 is non-printing
  {
    if (i<=touch.touchCommentCount)
      graph.gSetTitleLine(i, touch.touchComments[i]);
    else
      graph.gSetTitleLine(i, "");
  }
  //The routine to restore data uses VNAData(), whereas we have loaded data to uWorkArry. Transfer it.
  vars->VNADataNumSteps=uWork.uWorkNumPoints-1;
  vars->VNAData.mresize(vars->VNADataNumSteps+1,3);    //Make room for all steps
  for (int i=0; i <= vars->VNADataNumSteps; i++)
  {
    vars->VNAData[i][0]=uWork.uWorkArray[i+1][0]/1000000;    //freq (we store in MHz)
    vars->VNAData[i][1]=uWork.uWorkArray[i+1][1];
    vars->VNAData[i][2]=uWork.uWorkArray[i+1][2];  //freq, dB and angle
  }
  vars->VNADataZ0=touch.touchRef;  //Determined from file   //ver116-4a
  vars->VNADataLinear=1; //ver116-4a
  if (vars->VNADataNumSteps>2 && vars->VNAData[0][0])   //See if we have log spacing ver116-4a
  {
    float interFirst=vars->VNAData[1][0]-vars->VNAData[0][0];  //first frequency interval
    float interLast=vars->VNAData[vars->VNADataNumSteps][0]-vars->VNAData[vars->VNADataNumSteps-1][0];
    if (interFirst>0)
    {
        //If the first and last interval differ by a significant percent of the first,
        //we treat scan as log. Of course, for linear they really should be virtually identical.
        //Note this could be confused by an imported log scan of very small range. We do not
        //allow log scans with tiny ranges.
        if (abs(interFirst-interLast)/interFirst>0.001)
          vars->VNADataLinear=0;
    }
  }

        //set work array to minimum size. touchReadParams may have made it large
  uWork.uSetMaxWorkPoints(0,3); //ver116-1b

  vars->VNARestoreDoR0AndPlaneExt=0; RestoreVNAData();   //load and graph the data ver116-4j

    //Restart puts new time stamp in title, so put ours back in
  if (touch.touchCommentCount>2)
  {
    vars->restartTimeStamp=touch.touchComments[3];
    graph.gSetTitleLine(3, "    "+vars->restartTimeStamp+"    ");   //Extra blanks for clearing in case size changed
  }
  else
  {
    graph.gSetTitleLine(3, "");
  }

  //Reprint title without clearing; new data will cover old data
  //The refresh routine will have used saved info for faster drawing, and so will
  //have the time stamp from PartialRestart done in [LoadDataFromWorkArray], which we don//t want.
  graph.gPrintTitle(0);
  //#graphBox$, "flush"
  vars->VNAData.mresize(2,3);  //to save space
  vars->VNADataNumSteps=1;
  setCursor(Qt::ArrowCursor);
  util.beep();
}

//===================START CONTEXTS MODULE=======================

//-------------Explanation of Contexts-------------  ver114-2d
//A context file or string contains a series of lines (separated by chr$(13) in strings) containing
//information on one or more types of contexts. Data for a given type begins with the line
// "StartContext Name", where Name is the name of the context type. It ends with the end of the
//file or string, or with the line "EndContext". Routines that restore individual types do not necessarily expect
//the file or string to begin with the StartContext xxx line, which should have been processed by the caller.
//

//Some variables are stored both inside the graphing module and, under a different name, outside it.
//In those cases, only one version needs to be saved, but both versions need to be restored.
//-------------------------------------------------

//ver114-2c added SweepContext$

QString MainWindow::SweepContext()
{

  //Return sweep context as string
  //successive lines are separated by chr(13)
  QString newLine="\r";
  QString aSpace=" ";
  //First include variables used outside the graph module
  QString s1= "Version=B";    //This item was added in ver114-7n and changed to B in ver115-1b
  s1= s1+newLine+"msaMode="+vars->msaMode;    //ver114-6f ver115-1b
  s1= s1+newLine+"FreqMode="+QString::number(vars->freqBand);    //ver115-1c  ver116-4s
  s1= s1+newLine+"BaseFreq="+QString::number(vars->baseFrequency);
  s1= s1+newLine+"SpecialGraph="+QString::number(vars->doSpecialGraph);
  s1=s1+newLine+ "RLCSpec="+vars->doSpecialRLCSpec+";;"+vars->doSpecialCoaxName; //ver115-4b
  if (vars->useAutoWait) s1=s1+newLine+"Wait="+vars->autoWaitPrecision; else s1= s1+newLine+"Wait="+QString::number(vars->wate);
  s1= s1+newLine+"PlaneAdj="+QString::number(vars->planeadj);
  s1= s1+newLine+"Path="+util.Word(vars->path,2);    //path is in form "Path N"
  s1= s1+newLine+"SGPreset="+QString::number(vars->sgout);     //ver114-4h
  s1= s1+newLine+"Offset="+QString::number(vars->offset);
  s1= s1+newLine+"IsTG="+QString::number(vars->gentrk);     //ver114-4i
  s1= s1+newLine+"NormRev="+QString::number(vars->normrev); //ver114-4k
  s1= s1+newLine+"VideoFilter="+vars->videoFilter; //ver114-5p
  s1= s1+newLine+"DUTDirection="+QString::number(vars->switchFR); //ver116-4d
  s1= s1+newLine+"GraphData="+QString::number(vars->Y1DataType)+aSpace+ QString::number(vars->Y2DataType); //ver115-1b deleted source constants
  s1= s1+newLine+"Autoscale="+QString::number(graph.autoScaleY1)+ aSpace+ QString::number(graph.autoScaleY2);  //ver114-7e
  s1= s1+newLine+"S21Jig="+vnaCal.S21JigAttach+aSpace+ QString::number(vnaCal.S21JigR0)+aSpace+QString::number(vnaCal.S21JigShuntDelay);  //modver115-1e
  s1= s1+newLine+"S11Bridge="+QString::number(vnaCal.S11BridgeR0)+ aSpace+ QString::number(vnaCal.S11GraphR0)+ aSpace+ vnaCal.S11JigType;

  return s1+newLine+graph.gSweepContext();   //Add items from graph module

}
void MainWindow::RestoreSweepContext()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
      'public routine to restore sweep context
    'There are a couple of things we can't do within a true subroutine, so we use this routine as a wrapper
    'Because this gosub routine cannot accept arguments, the following values (non-global) must be preset:
    '   restoreContext$ is the string containing the context
    '   restoreIsValidation=1 to do just validation run (i.e. don't change data); 0 otherwise
    'If there is an error, and error message is placed in restoreErr$; otherwise it is made blank
    'Note the StartContext line must already have been skipped.
    'Caller must call RememberState before coming here and [DetectFullChanges] on return
            'Get next line and increment startPos to start of the following line
    startPos=1
    tLine$=uGetLine$(restoreContext$, startPos)
    oldStartPos=startPos
    contextVersion$="A"
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
        v1=val(tLine$)  'Most of our data is numeric values
        select case tag$
        'Each tag represents one or several data items. These are retrieved one at a time,
        'and as each is retrieved it is deleted from tLine$. Numeric items are delimited
        'by spaces, tabs or commas. Text items are delimited by the double character
        'contained in sep$, because they may contain spaces or commas. If this is just
        'a validation run, we do not enter any of the retrieved data into our variables.
                'ver114-2d cleaned up and added VNA
            case "VERSION"  'added by ver114-7n and ver115-1b
                contextVersion$=Trim$(tLine$)
            case "MSAMODE"
                'if restoreIsValidation=0 and tLine$<>msaMode$ then changeModeTo$=tLine$ : gosub [ChangeMode]  'ver114-6f
                if restoreIsValidation=0 then msaMode$=tLine$
            case "BASEFREQ" 'ver116-4k
                if restoreIsValidation=0 then baseFrequency=v1
            case "FREQMODE" 'ver115-1c
                if restoreIsValidation=0 then freqBand=v1   'ver116-4s
            case "SPECIALGRAPH"
                if restoreIsValidation=0 then doSpecialGraph=v1
            case "RLCSPEC" 'ver115-4b
                if restoreIsValidation=0 then
                    doSpecialRLCSpec$=uExtractTextItem$(tLine$,";;")
                    doSpecialCoaxName$=tLine$
                end if
            case "WAIT"
                if restoreIsValidation=0 then   'ver116-1b
                    select case tLine$
                        case "Fast", "Normal", "Precise"
                            useAutoWait=1 : autoWaitPrecision$=tLine$
                         case else
                            useAutoWait=0 : wate=v1
                    end select
                end if
            case "PLANEADJ"
                if restoreIsValidation=0 then planeadj=v1
            case "PATH"
                isErr=(v1<=0 or v1>MSANumFilters)
                if isErr=0 and restoreIsValidation=0 then path$="Path ";v1
            case "SGPRESET"
                if restoreIsValidation=0 then sgout=v1 'ver114-4h
            case "OFFSET"
                if restoreIsValidation=0 then offset=v1
            case "ISTG" 'ver114-4i
                if restoreIsValidation=0 then
                    if TGtop>0 then gentrk=v1 else gentrk=0 'Set gentrk only if we have the TG ver115-4f
                end if
            case "NORMREV"  'ver114-4i
                if restoreIsValidation=0 then normrev=v1
            case "VIDEOFILTER"  'ver114-5p
                if restoreIsValidation=0 then videoFilter$=tLine$
            case "DUTDIRECTION" 'ver116-4d
                if restoreIsValidation=0 then switchFR=v1
            case "GRAPHDATA"
                'In Version A there was a different format for graph data, so we will
                'just use default values.
                if contextVersion$="A" then
                    v1=0 : v2=0
                    if msaMode$="SA" then v1=constNoGraph : v2=constMagDBM  'ver115-3b
                    if msaMode$="ScalarTrans" then v1=constNoGraph : v2=constMagDB
                    if msaMode$="VectorTrans" then v1=constAngle : v2=constMagDB
                    if msaMode$="Reflection" then v1=constGraphS11Ang : v2=constGraphS11DB
                else
                    isErr=uExtractNumericItems(2, tLine$, " ", v1, v2, v3)  'isErr if not two items
                end if
                    'Be sure data types are valid ver115-1b. An old preference file
                    'may use different data codes.
                call FilterDataType v1,1  'ver115-3b
                call FilterDataType v2,2  'ver115-3b
                if isErr=0 and restoreIsValidation=0 then Y1DataType=v1 : Y2DataType=v2
            case "AUTOSCALE"   'ver114-7e
                if restoreIsValidation=0 then
                    isErr=uExtractNumericItems(2, tLine$, " ", v1, v2, v3)      'ver115-1f
                    if (v1<>0 and v1<>1) or (v2<>0 and v2<>1) then isErr=1
                    if isErr=0 then autoScaleY1=v1 : autoScaleY2=v2
                end if
            case "S21JIG"  'ver114-7e
                if restoreIsValidation=0 then
                    w$=uExtractTextItem$(tLine$," ")   'ver115-1f
                    if w$<>"Series" and w$<>"Shunt" then isErr=1
                    if isErr=0 then S21JigAttach$=w$
                    isErr=uExtractNumericItems(2, tLine$, " ", v1, v2, v3)   'ver115-1f
                    if v1<=0 then isErr=1   'modver115-1e
                    if isErr=0 then S21JigR0=v1 : S21JigShuntDelay=v2   'modver115-1e
                end if
            case "S11BRIDGE"  'ver114-7e modver115-1b
                if restoreIsValidation=0 then
                    isErr=uExtractNumericItems(2,tLine$, " ", v1, v2, v3)  'Extract three items and leave the third, a text item
                    if v1<=0 or v2<=0 then isErr=1
                    tLine$=Trim$(tLine$) : if tLine$<>"Reflect" and tLine$<>"Trans" then tLine$="Trans" 'For backward compatibility
                    if isErr=0 then S11BridgeR0=v1 : S11GraphR0=v2 : S11JigType$=tLine$    'ver115-1e
                end if
            case else
            'Unrecognized tag. Must belong to the graph module, so we end here
            startPos=oldStartPos    'reset to beginning of this line
            exit while
        end select
        if isErr then restoreErr$="Sweep Context Error in: "; origLine$: return 'ver115-1b
        'Get next line and increment startPos to start of the following line
        oldStartPos=startPos
        tLine$=uGetLine$(restoreContext$, startPos)
    wend
    'If no error so far, process the data for the graph module
    restoreErr$=gRestoreSweepContext$(restoreContext$, startPos, isValidation)
    if restoreErr$<>"" or isValidation=1 then return 'done if error or if this is just validation run

    'Now set variables that are calculated from the data just retrieved.
    call gGetYAxisRange 2, Y2Bot, Y2Top       'mag range   ver114-4d reversed mag and phase
    call gGetYAxisRange 1, Y1Bot, Y1Top   'phase range
    globalSteps=gNumDynamicSteps()              'number of steps
    call gGetXAxisRange xMin, xMax
    call SetStartStopFreq xMin, xMax 'sets centfreq, sweepwidth
    'stepfreq=(xMax-xMin)/globalSteps   delver115-1a
    steps=globalSteps
    sweepDir=gGetSweepDir() 'ver114-4n
    call SelectFilter filtbank  'ver116-4j
return
    */
}
QString MainWindow::GridContext()
{
  //Get grid context as string
  QString s="CustomNames=";
  for (int i=1; i <= 5; i++)
  {
    s=s+vars->customPresetNames[i]+";:";   //use a goofy separator so user won't have used it
  }
  return s+"\r"+gGridContext();    //ver116-1b
}
void MainWindow::FilterDataType()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub FilterDataType byref t, axisNum   'Make sure data types are valid for current msaMode$
    'Change to default values if invalid
    'ver115-3b changed to do only a single variable, so we get called once per axis.
    select msaMode$
        case "SA"
            if t<>constMagDBM and t<>constMagWatts and t<>constMagV and t<>constNoGraph then
                if axisNum<>gGetPrimaryAxis() then t=constNoGraph else t=constMagDBM 'ver115-7a
            end if
        case "ScalarTrans"
            if t<>constMagDB and t<>constMagRatio and t<>constInsertionLoss and t<>constNoGraph then
               if axisNum<>gGetPrimaryAxis() then t=constNoGraph else t=constMagDB 'ver115-7a
            end if
        case "VectorTrans"  'ver115-1i added raw mag and phase to transmission mode
            if t<>constMagDB and t<>constMagDBM and t<>constMagRatio and t<>constAngle _
                    and t<>constRawAngle and t<>constGD and t<>constInsertionLoss _
                            and t<>constNoGraph then  'ver115-2c
               if axisNum<>gGetPrimaryAxis() then t=constGraphS11Ang else t=constMagDB 'ver115-7a
            end if
        case "Reflection"
            if (t<constGraphS11DB or t>constGraphS11SWR) and _
                    t<>constImpedMag and t<>constImpedAng and t>constReflectPower and t>constComponentQ _
                    and t<>constReturnLoss and t<>constNoGraph _
                    and t<>constAdmitMag and t<>constAdmitAng _
                    and t<>constConductance and t<>constSusceptance then  'ver115-4a
               if axisNum<>gGetPrimaryAxis() then t=constGraphS11Ang else t=constGraphS11DB 'ver115-7a
            end if
        case else
            t=constNoGraph
    end select
end sub
*/
}
QString MainWindow::TraceContext()
{
  //Return trace context as string
  //successive lines are separated by chr$(13)
  //First include variables used outside the graph module
  QString s1= "RefreshEachScan="+QString::number(graph.refreshEachScan);
  s1= s1+"\rTraceModes="+QString::number(vars->Y1DisplayMode)+","+QString::number(vars->Y2DisplayMode);
  if (vars->doCycleTraceColors)
  {
    vars->cycleNumber=1;
    gridappearance.gSetTraceColors(gridappearance.cycleColorsAxis1[1], gridappearance.cycleColorsAxis2[1]);
  } //ver116-4s to save basic trace color
  return s1+"\r"+gTraceContext();   //Add items from graph module
}
QString MainWindow::RestoreTraceContext(QString s, int &startPos, int isValidation)
{
  //Restore sweep context
  //Returns error message if error; otherwise 0. Ignores data prior to startPos.
  //startPos is updated to one past the last line we process (normally EndContext or end of string)
  //if isValidation=1, we merely check for errors
  //sep$ is a possibly multi-character separator to delimit text items on one line
  //successive lines are separated by chr$(13)
  QString newLine="\r";
  QString aSpace=" ";
  QString sep$=";;"; //Used to separate text items
  QString nonTextDelims=" ,\t";    //space, comma and tab are delimiters
    //Get next line and increment startPos to start of the following line
  int oldStartPos=startPos;
  QString tLine=util.uGetLine(s, startPos);
  QString errMsg;
  oldStartPos=startPos;
  while (tLine!="")
  {
    QString tag;
    QString origLine=tLine; //ver115-1b
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
      if (isValidation==0) graph.refreshEachScan=tLine.toInt();
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
  errMsg=gRestoreTraceContext(s, startPos, isValidation);
  gridappearance.SetCycleColors();     //Set trace colors for color cycling ver116-4s
  return errMsg;
}
QString MainWindow::RestoreGridContext(QString s, int &startPos, int isValidation)
{
  //Restore grid context
  //Returns error message if error; otherwise 0. Ignores data prior to startPos.
  //startPos is updated to one past the last line we process (normally EndContext or end of string)
  //if isValidation=1, we merely check for errors

  //Return error message if error
  //startPos=0;
  QString tLine=util.uGetLine(s, startPos);
  int oldStartPos=startPos;
  QString contextVersion="A";
  while (tLine!="")
  {
    QString tag;
    QString origLine=tLine; //ver115-1b
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
          vars->customPresetNames[i]=util.uExtractTextItem(tLine, ";:");   //5 items,separated by ";:"
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
  graph.gGetGraphicsSize(oldWidth, oldHeight);   //save so we can restore
  QString errMsg=gRestoreGridContext(s, startPos, isValidation);

  if (errMsg!="" || isValidation==1) return errMsg;
  vars->primaryAxisNum=graph.gGetPrimaryAxis(); //ver115-3c
  graph.gGetMargins(graph.graphMarLeft, graph.graphMarRight, graph.graphMarTop, graph.graphMarBot);  //Get graph margins
  //We may need to resize the graph window to match the dimensions just retrieved
  //For now, we restore the previous size with the possibly new margins
  graph.gUpdateGraphObject(oldWidth, oldHeight, graph.graphMarLeft, graph.graphMarRight, graph.graphMarTop, graph.graphMarBot);
  ui->graphicsView->fitInView(graph.getScene()->sceneRect());
  QString referenceColor1, referenceColor2, dum1, dum2;
  gridappearance.gGetSupplementalTraceColors(referenceColor1, referenceColor2, dum1, dum2); //ver116-4b
  if (vars->primaryAxisNum==1) graph.referenceColorSmith=referenceColor1; else graph.referenceColorSmith=referenceColor2;  //ver116-4b

  return errMsg;
}
int MainWindow::BandLineCalContextAsTextArray()
{
  //Put line cal points into uTextPointArray$, with header info
  //return number of lines placed into uTextPointArray$
  //First line begins with ! and is line 1 of the title: !Log Sweep Path N. "Log" may instead by "Linear"; N=path number
  //First 3 lines are title, each preceded by !
  //Next line is sweep info
  //Next is Touchstone options line
  //Next is comment data headings
  //Then comes each point as its own string
  QString sweep;
  if (vnaCal.bandLineLinear) sweep="!Linear Sweep; "; else sweep="!Log Sweep ";
  uWork.uTextPointArray[1]="! BandSweep Line Calibration Data";
  uWork.uTextPointArray[2]="!";
  uWork.uTextPointArray[3]="!"+vnaCal.bandLineTimeStamp;
    //Save sweep info: log/linear ; path info (in form Path N); Jig attachment and R0
  uWork.uTextPointArray[4]=sweep+vnaCal.bandLinePath+"; S21Jig="+vnaCal.bandLineS21JigAttach+ "; S21JigR0="+ vnaCal.bandLineS21JigR0; //ver115-1b
  uWork.uTextPointArray[5]="# MHz S DB R 50" ;  //Freq in MHz, data in DB/angle format
  uWork.uTextPointArray[6]="! MHz  S21_DB  S21_Degrees";
  QString aSpace=" ";
  for (int i=0; i < vars->globalSteps; i++)
  {
    //save freq, mag and phase
    uWork.uTextPointArray[i+7] = QString::number(vars->bandLineCal[i][0])
        + aSpace + QString::number(vars->bandLineCal[i][1])
        + aSpace + QString::number(vars->bandLineCal[i][2]); //ver114-5f
  }
  return vars->globalSteps+7; //Number of lines
}
QString MainWindow::BandLineCalContext()
{
  //Return data points as string, with title in first 3 lines
  //We do not include StartContext or EndContext lines
  int nLines=BandLineCalContextAsTextArray();   //Assemble strings into uTextPointArray$
  return uWork.uTextArrayToString(1,nLines); //Assemble array of strings into one string
}
void MainWindow::BandLineCalContextToFile(QStringList &fHndl)
{
  //save band line cal points to file
  //We do not include StartContext or EndContext lines
  //fHndl$ is the handle of an already open file. We output our data
  //but do not close the file.
  int nLines=BandLineCalContextAsTextArray();   //Assemble strings into uTextPointArray$
  for (int i=1; i < nLines; i++)
  {
    fHndl.append(uWork.uTextPointArray[i]);
  }
}
int MainWindow::BaseLineCalContextAsTextArray()
{
  //Put line cal points into uTextPointArray$, with header info
  //return number of lines placed into uTextPointArray$
  //First line begins with ! and is line 1 of the title: !Log Sweep Path N. "Log" may instead by "Linear"; N=path number
  //First 3 lines are title, each preceded by !
  //Next line is sweep info
  //Next is Touchstone options line
  //Next is comment data headings
  //Then comes each point as its own string
  QString sweep;
  if (vnaCal.baseLineLinear) sweep="!Linear Sweep; "; else sweep="!Log Sweep ";
  uWork.uTextPointArray[1]="! BaseLine Calibration Data";
  uWork.uTextPointArray[2]="!";
  uWork.uTextPointArray[3]="!"+vnaCal.baseLineTimeStamp;
  uWork.uTextPointArray[4]=sweep+vnaCal.baseLinePath+"; S21Jig="+vnaCal.baseLineS21JigAttach + "; S21JigR0=" + vnaCal.baseLineS21JigR0; //ver115-1b
  uWork.uTextPointArray[5]="# MHz S DB R 50";   //Freq in MHz, data in DB/angle format
  uWork.uTextPointArray[6]="! MHz  S21_DB  S21_Degrees";
  QString aSpace=" ";
  for (int i=0; i < vars->globalSteps; i++)
  {
    //save freq, mag and phase
    uWork.uTextPointArray[i+7]=QString::number(vars->baseLineCal[i][0])
        +aSpace+QString::number(vars->baseLineCal[i][1])
        +aSpace+QString::number(vars->baseLineCal[i][2]);
  }
  return vars->globalSteps+7; //Number of lines
}
QString MainWindow::BaseLineCalContext()
{
  //Return data points as string, with title in first 3 lines
  //We do not include StartContext or EndContext lines
  int nLines=BaseLineCalContextAsTextArray();   //Assemble strings into uTextPointArray$
  return uWork.uTextArrayToString(1, nLines); //Assemble array of strings into one string
}
void MainWindow::BaseLineCalContextToFile(QStringList &fHndl)
{
  //save line cal points to file
  //We do not include StartContext or EndContext lines
  //fHndl$ is the handle of an already open file. We output our data
  //but do not close the file.
  int nLines=BaseLineCalContextAsTextArray();   //Assemble strings into uTextPointArray$
  for (int i=0; i < nLines; i++)
  {
    fHndl.append(uWork.uTextPointArray[i]);
  }
}
void MainWindow::RestoreBandLineCalContext(QString &s, int &startpos)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;

  /*
function RestoreBandLineCalContext(byref s$, byref startPos)   'Restore line cal points. Return number of points
    'We return number of points read, or -1 for file error
    'We ignore data prior to startPos. We update startPos to the start of the next line after the line
    'with "!EndContext"; or if no such line then one past end of string

    isErr=uArrayFromString(s$, 1, startPos, 3)   'Get data into uWorkArray; 3 data per line (freq+mag+phase)
    if isErr then RestoreBandLineCalContext=0 : errCode=1 : exit function
        'Now transfer retrieved data from uWorkArray() to lineCalArray().
    errCode=0
    for i=1 to uWorkNumPoints
        bandLineCal(i-1,0)=uWorkArray(i, 0) 'freq
        bandLineCal(i-1,1)=uWorkArray(i, 1) : bandLineCal(i-1,2)=uWorkArray(i, 2) 'Mag and phase
    next i
    RestoreBandLineCalContext=uWorkNumPoints
    'Now derive the sweep parameters from the array and the title ver114-5h
    bandLineNumSteps=uWorkNumPoints-1
    bandLineStartFreq=bandLineCal(0,0) : bandLineEndFreq=bandLineCal(bandLineNumSteps,0)
    bandLineTimeStamp$=uWorkTitle$(3)   'uArrayFromString put date/time stamp here
    sweep$=uWorkTitle$(4)   'uArrayFromString put sweep info here
    if instr(sweep$,"Linear")>0 then bandLineLinear=1 else bandLineLinear=0
            'ver115-1b added the retrieval of the following sweep info
        'Get remaining sweep info from sweep$. Each item has a keyword and ends with a semi-colon or end of line
    bandPathNum$=uGetParamText$(sweep$, "Path ", ";")  'Gets text after "Path " to semicolon or end
    if bandPathNum$="" then bandLinePath$="Path 1" else bandLinePath$="Path ";bandPathNum$
    bandLineS21JigAttach$=uGetParamText$(sweep$, "S21Jig=", ";")  'Gets text after S21Jig= to semicolon or end
    if bandLineS21JigAttach$="" then bandLineS21JigAttach$="Series"
    bandLineS21JigR0=val(uGetParamText$(sweep$, "S21JigR0=", ";"))  'Gets text after S21Jig= to semicolon or end
    if bandLineS21JigR0=0 then bandLineS21JigR0=50
end function
*/
}
int MainWindow::GetBandLineCalContextFromFile(QString fHndl)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
 'get points from file; return number of points
    'We return number of points read, or -1 for file error
    'fHndl$ is the handle of an already open file. We read our data
    'but do not close the file. The last line we read will be "!EndContext"
    isErr=uArrayFromFile(fHndl$,3) 'Get data, 3 per line
    if isErr then GetBandLineCalContextFromFile=-1 : baseLineNumSteps=-1 : exit function
        'Move the data from uWorkArray to gGraphVal
    for i=1 to uWorkNumPoints
        bandLineCal(i-1,0)=uWorkArray(i, 0) 'freq
        bandLineCal(i-1,1)=uWorkArray(i, 1) : bandLineCal(i-1,2)=uWorkArray(i, 2) 'Mag and phase
    next i
    GetBandLineCalContextFromFile=uWorkNumPoints
        'Now derive the sweep parameters from the array and the title ver114-5h
    bandLineNumSteps=uWorkNumPoints-1
    bandLineStartFreq=bandLineCal(0,0) : bandLineEndFreq=bandLineCal(bandLineNumSteps,0)
    bandLineTimeStamp$=uWorkTitle$(3)   'uArrayFromFile put date/time stamp here
    sweep$=uWorkTitle$(4)   'uArrayFromFile put sweep info here
    if instr(sweep$,"Linear")>0 then bandLineLinear=1 else bandLineLinear=0
            'ver115-1b added the retrieval of the following sweep info
        'Get remaining sweep info from sweep$. Each item has a keyword and ends with a semi-colon or end of line
    bandPathNum$=uGetParamText$(sweep$, "Path ", ";")  'Gets text after "Path " to semicolon or end
    if bandPathNum$="" then bandLinePath$="Path 1" else bandLinePath$="Path ";bandPathNum$
    bandLineS21JigAttach$=uGetParamText$(sweep$, "S21Jig=", ";")  'Gets text after S21Jig= to semicolon or end
    if bandLineS21JigAttach$="" then bandLineS21JigAttach$="Series"
    bandLineS21JigR0=val(uGetParamText$(sweep$, "S21JigR0=", ";"))  'Gets text after S21Jig= to semicolon or end
    if bandLineS21JigR0=0 then bandLineS21JigR0=50
    */
  return 0;
}
int MainWindow::RestoreBaseLineCalContext(QString &s, int &startPos)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  'Restore line cal points. Return number of points
     'We return number of points read, or -1 for file error
     'We ignore data prior to startPos. We update startPos to the start of the next line after the line
     'with "!EndContext"; or if no such line then one past end of string

     isErr=uArrayFromString(s$, 1, startPos, 3)   'Get data into uWorkArray; 3 data per line (freq+mag+phase)
     if isErr then RestoreBaseLineCalContext=-1 : exit function
         'Now transfer retrieved data from uWorkArray() to lineCalArray().
         'Make sure baseLineCal is big enough. Note that it can be bigger than data arrays,
         'since it is used only by interpolating into a possibly smaller array.
     if uWorkNumPoints>gMaxNumPoints then redim baseLineCal(uWorkNumPoints+20, 2) 'ver114-5m

     for i=1 to uWorkNumPoints
         baseLineCal(i-1,0)=uWorkArray(i, 0)    'Freq
         baseLineCal(i-1,1)=uWorkArray(i, 1) : baseLineCal(i-1,2)=uWorkArray(i, 2) 'Mag and phase
     next i
     RestoreBaseLineCalContext=uWorkNumPoints
              'Now derive the sweep parameters from the array and the title ver114-5h
     baseLineNumSteps=uWorkNumPoints-1
     baseLineStartFreq=baseLineCal(0,0) : baseLineEndFreq=baseLineCal(baseLineNumSteps,0)
     baseLineTimeStamp$=uWorkTitle$(3)   'uArrayFromString put date/time stamp here
     sweep$=uWorkTitle$(4)   'uArrayFromString put sweep info here
     if instr(sweep$,"Linear")>0 then baseLineLinear=1 else baseLineLinear=0
         'ver115-1b added the retrieval of the following sweep info
         'Get remaining sweep info from sweep$. Each item has a keyword and ends with a semi-colon or end of line
     basePathNum$=uGetParamText$(sweep$, "Path ", ";")  'Gets text after "Path " to semicolon or end
     if basePathNum$="" then baseLinePath$="Path 1" else baseLinePath$="Path ";basePathNum$
     baseLineS21JigAttach$=uGetParamText$(sweep$, "S21Jig=", ";")  'Gets text after S21Jig= to semicolon or end
     if baseLineS21JigAttach$="" then baseLineS21JigAttach$="Series"
     baseLineS21JigR0=val(uGetParamText$(sweep$, "S21JigR0=", ";"))  'Gets text after S21Jig= to semicolon or end
     if baseLineS21JigR0=0 then baseLineS21JigR0=50
     */
  return 0;
}
int MainWindow::GetBaseLineCalContextFromFile(QFile *fHndl)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
 'get points from file; return number of points
    'We return number of points read, or -1 for file error
    'fHndl$ is the handle of an already open file. We read our data
    'but do not close the file. The last line we read will be "!EndContext"
    isErr=uArrayFromFile(fHndl$,3) 'Get data, 3 per line
    if isErr then GetBaseLineCalContextFromFile=-1 : baseLineNumSteps=-1 : exit function
        'Move the data from uWorkArray to gGraphVal
    for i=1 to uWorkNumPoints
        baseLineCal(i-1,0)=uWorkArray(i, 0)    'Freq
        baseLineCal(i-1,1)=uWorkArray(i, 1) : baseLineCal(i-1,2)=uWorkArray(i, 2) 'Mag and phase
    next i
    GetBaseLineCalContextFromFile=uWorkNumPoints
         'Now derive the sweep parameters from the array and the title ver114-5h
    baseLineNumSteps=uWorkNumPoints-1
    baseLineStartFreq=baseLineCal(0,0) : baseLineEndFreq=baseLineCal(baseLineNumSteps,0)
    baseLineTimeStamp$=uWorkTitle$(3)   'uArrayFromFile put date/time stamp here
    sweep$=uWorkTitle$(4)   'uArrayFromFile put sweep info here
    if instr(sweep$,"Linear")>0 then baseLineLinear=1 else baseLineLinear=0
        'ver115-1b added the retrieval of the following sweep info
        'Get remaining sweep info from sweep$. Each item has a keyword and ends with a semi-colon or end of line
    basePathNum$=uGetParamText$(sweep$, "Path ", ";")  'Gets text after "Path " to semicolon or end
    if basePathNum$="" then baseLinePath$="Path 1" else baseLinePath$="Path ";basePathNum$
    baseLineS21JigAttach$=uGetParamText$(sweep$, "S21Jig=", ";")  'Gets text after S21Jig= to semicolon or end
    if baseLineS21JigAttach$="" then baseLineS21JigAttach$="Series"
    baseLineS21JigR0=val(uGetParamText$(sweep$, "S21JigR0=", ";"))  'Gets text after S21Jig= to semicolon or end
    if baseLineS21JigR0=0 then baseLineS21JigR0=50
    */
  return 0;
}
int MainWindow::OSLCalContextAsTextArray(int isBand)
{
qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  //Put  cal points into uTextPointArray$, with header info
  //isBand=1 means we do band cal; =0 means base cal
  //return number of lines placed into uTextPointArray$
  //First line begins with ! and is line 1 of the title: !Log Sweep Path N. "Log" may instead by "Linear"; N=path number
  //First 3 lines are title, each preceded by !
  //Next line is sweep info
  //Next is Touchstone options line
  //Next is comment data headings
  //Then comes each point as its own string

  uWork.uTextPointArray[1]="! OSL Calibration Data";
  uWork.uTextPointArray[2]="!";
  /*
  if (vars->isBand)
  {
    if OSLBandLinear then sweep$="!Linear Sweep; " else sweep$="!Log Sweep "
    uTextPointArray$(3)="!";OSLBandTimeStamp$
    //Save sweep info: log/linear ; path info (in form Path N); Jig attachment and R0
    uTextPointArray$(4)=sweep$;OSLBandPath$;"; S11Jig=";OSLBandS11JigType$; "; S11BridgeR0=";S11BridgeR0; _ //ver115-9d
                "; S21Jig=";OSLBandS21JigAttach$; "; S21JigR0="; OSLBandS21JigR0 //ver115-1b, ver115-1g
  }
  else
  {
    if OSLBaseLinear then sweep$="!Linear Sweep; " else sweep$="!Log Sweep "
     uTextPointArray$(3)="!";OSLBaseTimeStamp$
    //Save sweep info: log/linear ; path info (in form Path N); Jig attachment and R0
    uTextPointArray$(4)=sweep$;OSLBasePath$;"; S11Jig=";OSLBaseS11JigType$; "; S11BridgeR0=";S11BridgeR0; _ //ver115-9d
                "; S21Jig=";OSLBaseS21JigAttach$; "; S21JigR0="; OSLBaseS21JigR0  //ver115-1b, ver115-1g
  }
  uTextPointArray$(5)="!"
  aSpace$=" "
  uTextPointArray$(6)="!  MHz    A_real   A_imag   B_real   B_imag     C_Real     C_Imag     RefDB     RefDeg"
  for i=0 to globalSteps
  {
    //save freq, mag and phase
    if isBand then  //Band cal
    {
        s$=OSLBandA(i,0);aSpace$;OSLBandA(i,1);aSpace$;OSLBandB(i,0);_
                                aSpace$;OSLBandB(i,1);aSpace$; OSLBandC(i,0);aSpace$;OSLBandC(i,1);aSpace$
        uTextPointArray$(i+7)=OSLBandRef(i,0);aSpace$;s$;OSLBandRef(i,1);aSpace$;OSLBandRef(i,2)
    }
    else    //Base cal
    {
        s$=OSLBaseA(i,0);aSpace$;OSLBaseA(i,1);aSpace$;OSLBaseB(i,0);_
                                aSpace$;OSLBaseB(i,1);aSpace$; OSLBaseC(i,0);aSpace$;OSLBaseC(i,1);aSpace$
        uTextPointArray$(i+7)=OSLBaseRef(i,0);aSpace$;s$;OSLBaseRef(i,1);aSpace$;OSLBaseRef(i,2)
    }
  }
  return vars->globalSteps+7; //Number of lines

*/
  return 0;
}
void MainWindow::OSLCalContextToFile(QFile *fHndl, int isBand)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub OSLCalContextToFile fHndl$, isBand   'save OSL cal points to file
    'isBand=1 means we do band cal; =0 means base cal
    'We do not include StartContext or EndContext lines
         'fHndl$ is the handle of an already open file. We output our data
        'but do not close the file.
    nLines=OSLCalContextAsTextArray(isBand)   'Assemble strings into uTextPointArray$
    for i=1 to nLines
        print #fHndl$, uTextPointArray$(i)
    next i
end sub
*/
}
void MainWindow::OSLGetCalContextFromFile(QFile *fHndl, int isBand)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function OSLGetCalContextFromFile(fHndl$, isBand)   'get points from file; return number of points
    'isBand=1 means we do band cal; =0 means base cal
    'We return number of points read, or -1 for file error
    'fHndl$ is the handle of an already open file. We read our data
    'but do not close the file. The last line we read will be "!EndContext"
    isErr=uArrayFromFile(fHndl$,100) 'Get data, 100 means number per line to be determined
    if isErr then
        OSLGetCalContextFromFile=-1
        if isBand then OSLBandNumSteps=-1 else OSLBaseNumSteps=-1 'Indicates no data
        exit function
    end if
        'Move the data from uWorkArray to gGraphVal
    if isBand then  'Band Cal
        for i=1 to uWorkNumPoints   'ver116-4n
            OSLBandRef(i-1,0)=uWorkArray(i, 0) 'freq
            OSLBandA(i-1,0)=uWorkArray(i, 1) : OSLBandA(i-1,1)=uWorkArray(i, 2) 'Coeff A real and imag
            OSLBandB(i-1,0)=uWorkArray(i, 3) : OSLBandB(i-1,1)=uWorkArray(i, 4) 'Coeff B real and imag
            OSLBandC(i-1,0)=uWorkArray(i, 5) : OSLBandC(i-1,1)=uWorkArray(i, 6) 'Coeff C real and imag
            OSLBandRef(i-1,1)=uWorkArray(i, 7) : OSLBandRef(i-1,2)=uWorkArray(i, 8) 'Mag and phase for ref
        next i
        OSLGetCalContextFromFile=uWorkNumPoints
            'Now derive the sweep parameters from the array and the title ver114-5h
        if uWorkNumPoints<1 then exit function
        OSLBandNumSteps=uWorkNumPoints-1
        OSLBandStartFreq=OSLBandRef(0,0) : OSLBandEndFreq=OSLBandRef(OSLBandNumSteps,0)
        OSLBandTimeStamp$=uWorkTitle$(3)   'uArrayFromFile put date/time stamp here
        sweep$=uWorkTitle$(4)   'uArrayFromFile put sweep info here
        if instr(sweep$,"Linear")>0 then OSLBandLinear=1 else OSLBandLinear=0
                'ver115-1b added the retrieval of the following sweep info
            'Get remaining sweep info from sweep$. Each item has a keyword and ends with a semi-colon or end of line
        calPathNum$=uGetParamText$(sweep$, "Path ", ";")  'Gets text after "Path " to semicolon or end
        if calPathNum$="" then OSLBandPath$="Path 1" else OSLBandPath$="Path ";calPathNum$
        OSLBandS11JigType$=uGetParamText$(sweep$, "S11Jig=", ";")  'Gets text after S11Jig= to semicolon or end ver115-1g
        if OSLBandS11JigType$="" then OSLBandS11JigType$="Reflect"  'ver115-1g
        if OSLBandS11JigType$="Reflect" then    'ver115-9d
            'If using bridge, get bridge R0
            OSLBandS11BridgeR0=val(uGetParamText$(sweep$, "S11BridgeR0=", ";"))  'Gets text after S11BridgeR0= to semicolon or end
            if OSLBandS11BridgeR0<=0 then OSLBandS11BridgeR0=50
        else
            'If using S21 fixture for reflection measurements, get attachment and R0
            OSLBandS21JigAttach$=uGetParamText$(sweep$, "S21Jig=", ";")  'Gets text after S21Jig= to semicolon or end
            if OSLBandS21JigAttach$="" then OSLBandS21JigAttach$="Series"
            OSLBandS21JigR0=val(uGetParamText$(sweep$, "S21JigR0=", ";"))  'Gets text after S21Jig= to semicolon or end
            if OSLBandS21JigR0<=0 then OSLBandS21JigR0=50
        end if
    else    'Base
        for i=1 to uWorkNumPoints   'ver116-4n
            OSLBaseRef(i-1,0)=uWorkArray(i, 0) 'freq
            OSLBaseA(i-1,0)=uWorkArray(i, 1) : OSLBaseA(i-1,1)=uWorkArray(i, 2) 'Coeff A real and imag
            OSLBaseB(i-1,0)=uWorkArray(i, 3) : OSLBaseB(i-1,1)=uWorkArray(i, 4) 'Coeff B real and imag
            OSLBaseC(i-1,0)=uWorkArray(i, 5) : OSLBaseC(i-1,1)=uWorkArray(i, 6) 'Coeff C real and imag
            OSLBaseRef(i-1,1)=uWorkArray(i, 7) : OSLBaseRef(i-1,2)=uWorkArray(i, 8) 'Mag and phase for ref
        next i
        OSLGetCalContextFromFile=uWorkNumPoints
            'Now derive the sweep parameters from the array and the title ver114-5h
        if uWorkNumPoints<1 then exit function
        OSLBaseNumSteps=uWorkNumPoints-1
        OSLBaseStartFreq=OSLBaseRef(0,0) : OSLBaseEndFreq=OSLBaseRef(OSLBaseNumSteps,0)
        OSLBaseTimeStamp$=uWorkTitle$(3)   'uArrayFromFile put date/time stamp here
        sweep$=uWorkTitle$(4)   'uArrayFromFile put sweep info here
        if instr(sweep$,"Linear")>0 then OSLBaseLinear=1 else OSLBaseLinear=0
                'ver115-1b added the retrieval of the following sweep info
            'Get remaining sweep info from sweep$. Each item has a keyword and ends with a semi-colon or end of line
        calPathNum$=uGetParamText$(sweep$, "Path ", ";")  'Gets text after "Path " to semicolon or end
        OSLBaseS11JigType$=uGetParamText$(sweep$, "S11Jig=", ";")  'Gets text after S11Jig= to semicolon or end ver115-1g
        if OSLBaseS11JigType$="" then OSLBaseS11JigType$="Reflect"  'ver115-1g
        if calPathNum$="" then OSLBasePath$="Path 1" else OSLBasePath$="Path ";calPathNum$

        if OSLBandS11JigType$="Reflect" then    'ver115-9d
            'If using bridge, get bridge R0
            OSLBaseS11BridgeR0=val(uGetParamText$(sweep$, "S11BridgeR0=", ";"))  'Gets text after S11BridgeR0= to semicolon or end
            if OSLBaseS11BridgeR0<=0 then OSLBaseS11BridgeR0=50
        else
            'If using S21 fixture for reflection measurements, get attachment and R0
            OSLBaseS21JigAttach$=uGetParamText$(sweep$, "S21Jig=", ";")  'Gets text after S21Jig= to semicolon or end
            if OSLBaseS21JigAttach$="" then OSLBaseS21JigAttach$="Series"
            OSLBaseS21JigR0=val(uGetParamText$(sweep$, "S21JigR0=", ";"))  'Gets text after S21Jig= to semicolon or end
            if OSLBaseS21JigR0<=0 then OSLBaseS21JigR0=50
        end if
    end if
end function
*/
}
int MainWindow::GraphDataContextAsTextArray()
{
  //Put graph data points into uTextPointArray, with header info
   //return number of lines placed into uTextPointArray
   //First 3 lines are title, each preceded by !
   //Next line is sweep info
   //Next is Touchstone options line
   //Next is comment data headings
   //Then comes each point as its own string
   uWork.uTextPointArray[1]="!"+graph.gGetTitleLine(1);
   uWork.uTextPointArray[2]="!"+graph.gGetTitleLine(2);
   uWork.uTextPointArray[3]="!"+graph.gGetTitleLine(3);
   uWork.uTextPointArray[4]="!"+graph.gGetTitleLine(4);  //This line has sweep info set by user+ not printed as part of title ver114-5m
   uWork.uTextPointArray[5]="# MHz S DB R 50";   //Freq in MHz, data in DB/angle format
   uWork.uTextPointArray[6]="! MHz  S21_DB  S21_Degrees";
   //We save frequency plus two data per point even if we only have one trace.
   QString aSpace=" ";
   int pMin, pMax;
   graph.gGetMinMaxPointNum(pMin, pMax);    //ver114-6d
   int minStep=pMin-1 ; int maxStep=pMax-1;
   for (int i=minStep; i < maxStep;i++)    //ver114-6d
   {
     int f, y1, y2;
     graph.gGetPointVal(i+1, f, y1, y2);    //point num is one more than step num.
     uWork.uTextPointArray[i+7]=QString::number(f)+aSpace+QString::number(y2)+aSpace+QString::number(y1);  //Save freq, Y2, Y1
   }
   return maxStep+7;
}
QString MainWindow::GraphDataContext()
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
void MainWindow::GraphDataContextToFile(QStringList &fHndl)
{
  //save data points to file
  //We do not include StartContext or EndContext lines
  //fHndl$ is the handle of an already open file. We output our data
  //but do not close the file.
  int nLines=GraphDataContextAsTextArray();   //Assemble strings into uTextPointArray$
  for (int i=0; i< nLines;i++)
  {
    fHndl.append(uWork.uTextPointArray[i]);
  }
}
void MainWindow::RestoreGraphDataContext(QString &s, int &startPos, int doTitle)
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
void MainWindow::GetGraphDataContextFromFile(QFile *fHndl, int doTitle)
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
void MainWindow::DataTableContextAsTextArray()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function DataTableContextAsTextArray()    'Put data points into uTextPointArray$, with header info
    'We save the full datatable() even if data for some steps has not been collected yet.
    'return number of lines placed into uTextPointArray$
    'First 3 lines are title, each preceded by !
    'Next line is sweep info
    'Next is Touchstone options line
    'Next is comment data headings
    'Then comes each point as its own string
    uTextPointArray$(1)="!";gGetTitleLine$(1)
    uTextPointArray$(2)="!";gGetTitleLine$(2)
    uTextPointArray$(3)="!";gGetTitleLine$(3)
    uTextPointArray$(4)="!";gGetTitleLine$(4)  'This line has sweep info set by user; not printed as part of title ver114-5m
    uTextPointArray$(5)="# MHz S DB R 50"   'Freq in MHz, data in DB/angle format
    uTextPointArray$(6)="! MHz  S21_DB  S21_Degrees"
    'We save frequency plus two data per point even if we only have one trace.
    aSpace$=" "
    for i=0 to globalSteps
        y1=datatable(i,3)
        y2=datatable(i,2)
        uTextPointArray$(i+7)=datatable(i,1);aSpace$;y2;aSpace$;y1  'Save freq, Y2, Y1
    next i
    DataTableContextAsTextArray=maxStep+7
end function
*/
}
QString MainWindow::DataTableContext()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function DataTableContext$()    'Return data points as string, with title in first 3 lines
    'We do not include !StartContext or !EndContext lines
    nLines=DataTableContextAsTextArray()   'Assemble strings into uTextPointArray$
    DataTableContext$=uTextArrayToString$(1,nLines) 'Assemble array of strings into one string
end function
*/
  return "fix me";
}
void MainWindow::DataTableContextToFile(QFile *fHndl)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub DataTableContextToFile fHndl$    'save data points to file
    'We save the full datatable() even if data for some steps has not been collected yet.
    'We do not include StartContext or EndContext lines
         'fHndl$ is the handle of an already open file. We output our data
        'but do not close the file.
    nLines=DataTableContextAsTextArray()   'Assemble strings into uTextPointArray$
    for i=1 to nLines
        print #fHndl$, uTextPointArray$(i)
    next i
end sub
*/
}
void MainWindow::RestoreDataTableContext(QString &s, int &startPos, int doTitle)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function RestoreDataTableContext(byref s$, byref startPos, doTitle)   'Restore data points from s$
    'Return number of points; -1 if error
    'Restore data; restore title only if doTitle=1
    'We ignore data prior to startPos. We update startPos to the start of the next line after
    'the point data, per uArrayFromString

    isErr=uArrayFromString(s$, 1, startPos, 3)   'Get data into uWorkArray; 3 data per line (freq+mag+phase)
    if isErr then RestoreDataTableContext=-1 : exit function

        'Now transfer retrieved data from uWorkArray() to gGraphVal().
        'TO DO--This resizing does not help other arrays, and to resize them we would need to erase them.
        'If we don't have room, we should notify the user that he needs to do a sweep with more points.
    if uWorkNumPoints>gMaxNumPoints() then call ResizeArrays uWorkNumPoints
    for i=1 to uWorkNumPoints
        datatable(i-1,1)=uWorkArray(i, 0)  'Frequency
        datatable(i-1,2)=uWorkArray(i, 1) : datatable(i-1,1)=uWorkArray(i, 2) 'Mag (Y2) first, then phase
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
    RestoreDataTableContext=uWorkNumPoints
    prevDataChanged=1
end function
*/
}
void MainWindow::GetDataTableContextFromFile(QFile *fHndl, int doTitle)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function GetDataTableContextFromFile(fHndl$, doTitle)   'get points from file; return number of points or -1 if error
    'Restore data; restore title only if doTitle=1
    'fHndl$ is the handle of an already open file. We read our data
    'but do not close the file. The last line we read will be "!EndContext"
    isErr=uArrayFromFile(fHndl$,3) 'Get data, 3 per line
    if isErr then GetDataTableContextFromFile=-1 : exit function
        'Move the data from uWorkArray to gGraphVal
    if uWorkNumPoints>gMaxNumPoints() then call ResizeArrays uWorkNumPoints   'Make sure we have enough room
    for i=1 to uWorkNumPoints
        datatable(i-1,1)=uWorkArray(i, 0)  'Frequency
        datatable(i-1,2)=uWorkArray(i, 1) : datatable(i-1,1)=uWorkArray(i, 2) 'Mag (Y2) first, then phase
    next i
    if doTitle then
        for i=1 to 4
            call gSetTitleLine i, uWorkTitle$(i)  'Save title, which uArrayFromString retrieved ver114-5i
        next i
    end if
    'Note title line 4 has info on linear/log. We don't change to match, so on a Restart we will
    'not conform. But that should be taken care of in restoring the sweep context. Likewise with path info.
    'Also note that we do not change anything in the graph module. Neither its data nor number of steps will
    'necessarily match ours. It must be dealt with separately.
    'We also don't set steps or globalSteps to match our number of points
    GetDataTableContextFromFile=uWorkNumPoints
    prevDataChanged=1
end function
*/
}
QString MainWindow::mMarkerContext()
{

  //Return marker context as string
  //successive lines are separated by chr$(13)
  QString newLine="\r";
  QString s=",";  //comma to separate items
  QString j="";
  QString s1="";
  for (int i=0; i < 9; i++)
  {
    QString id=graph.markerIDs[i]; int markNum=mMarkerNum(id);
    int pointNum;
    QString trace, style;
    graph.gGetMarkerByNum(markNum, pointNum, id, trace, style);
    if (pointNum>0)
    {
        s1=s1+j+"Marker=" + id+s+ QString::number(pointNum)+ s+ trace;     //Marker id, point number and trace
        j=newLine;
    }
  }
  s1= s1+j+"Selected="+graph.selMarkerID;
  j=newLine;
  s1= s1+newLine+"DoGraph="+QString::number(graph.doGraphMarkers);
  s1= s1+newLine+"LROptions="+QString::number(graph.doPeaksBounded)+s+graph.doLRRelativeTo
      +s+QString::number(graph.doLRRelativeAmount)+s+QString::number(graph.doLRAbsolute); //ver115-3f
  s1= s1+newLine+"FiltAnalysis="+QString::number(vars->doFilterAnalysis)+s+QString::number(vars->x1DBDown)+s+QString::number(vars->x2DBDown);
  return s1;

}
QString MainWindow::mRestoreMarkerContext(QString s, int &startPos, int isValidation)
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
void MainWindow::LoadBasicContextsFromFile()
{
  //Load contexts from file restoreFileHndl$; set restoreErr$ to error message or ""
  //User must call RememberState before coming here and then DetectFullChanges after returning, to deal with changes.
  //The file must already be open. We do not close it.

  //restoreFileHndl$ contains LB file handle, e.g. "#prefs"
  //restoreLastLineNum will be set to the last line we read, in case the caller wants to back up one line.
  //The file may contain several contexts, each starting with StartContext Name or !StartContext Name
  //and ending with EndContext. Grid context should be before Sweep context.
  //There may already have been other data read from the file. We start wherever Line Input reads.
  //The entries of contextTypes will be set to 1 if the corresponding context was read:
  //See Select statement for contextTypes values

  for (int i=0; i< 30;i++) {vars->contextTypes[i]=0;}  //Clear contextTypes we actually only need four
  vars->restoreErr="";  //assume no error
  vars->restoreLastLineNum=0;
  int foundData=0;
  QString errMsg;
  while(!vars->restoreFileHndl->atEnd())
  {
    //Find next "StartContext" or "!". The exclamation point means a data section has been
    //encountered that is not marked with "StartContext", which is OK but is the end of preferences
    QString contextName="";
    while(!vars->restoreFileHndl->atEnd())
    {
      QString tLine = util.readLine(vars->restoreFileHndl, '\r');
        //Line Input #restoreFileHndl$, tLine$
        vars->restoreLastLineNum=vars->restoreLastLineNum+1;
        tLine=tLine.trimmed().toUpper();
        if (tLine.left(1)=="!")
        {
          foundData=1;
          return;
        }  //Exclamation means data starts
        int tagPos=tLine.indexOf("STARTCONTEXT");
        if (tagPos>=0)
        {
          contextName=(tLine.mid(tagPos+12)).trimmed(); //text after tag is context name
          break;
        }
    }
    if (foundData) break;
    if (contextName == "HARDWARE")
    {
      errMsg="Can't restore Hardware Context";
    }
    else if (contextName == "GRID" || contextName == "TRACE" || contextName == "SWEEP" || contextName == "MARKER")   //These are read into a string and then processed ver115-8c
    {
      vars->restoreContext="";
      //Assemble context into string, without StartContext and EndContext lines
      QString joint="";
      while (!vars->restoreFileHndl->atEnd())
      {
        QString tLine = util.readLine(vars->restoreFileHndl, '\r');

        vars->restoreLastLineNum=vars->restoreLastLineNum+1;
        if (tLine.trimmed().toUpper()=="ENDCONTEXT") break;
        vars->restoreContext=vars->restoreContext+joint+tLine;
        joint="\r";
      }
        //Process with proper routine
      if (contextName == "GRID")
      {
        vars->contextTypes[vars->constGrid]=1;
        int i = 0;
        errMsg=RestoreGridContext(vars->restoreContext,i,0); //Actual restoration
      }
      else if (contextName == "TRACE")
      {
        int i = 0;
        errMsg=RestoreTraceContext(vars->restoreContext,i,0);
        vars->contextTypes[vars->constTrace]=1;
      }
      else if (contextName == "SWEEP")
      {
        vars->contextTypes[vars->constSweep]=1;
        vars->restoreIsValidation=0;
        RestoreSweepContext();     //Actual restoration
        errMsg=vars->restoreErr;
      }
      else if (contextName == "MARKER")
      {
        vars->contextTypes[vars->constMarker]=1;
        vars->restoreIsValidation=0;
        // fix me RestoreMarkerContext();     //Actual restoration
        errMsg=vars->restoreErr;
      }
      if (errMsg!="")
      {
        vars->restoreErr=errMsg;
        break;
      }
    }
    else if (contextName == "DATATABLE")
    {
      break; //Datatable context can mark the end, but is not error
    }
    else
    {
      vars->restoreErr="UNKNOWN CONTEXT: "+contextName; break;  //Unknown context name; may not actually be error
    }
  }
  //Reach here when entire file has been processed up to an unknown context or a "!", or error occurred
}
QFile * MainWindow::OpenContextFile(QString fName, QString inout)
{
  QFile *fFile = new QFile(fName);
  if (!fFile->exists())
  {
    QString ggg = "file not found";
  }
  if (inout.trimmed().toUpper() == "IN")
  {
    if (!fFile->open(QFile::ReadOnly))
    {
      delete fFile;
      fFile = NULL;
    }
  }
  else
  {
    if (!fFile->open(QFile::WriteOnly | QFile::Text))
    {
      delete fFile;
      fFile = NULL;
    }
  }
/*
  //Open specified file. Return handle or  ""
  //fName$ contains complete path and file name
  On Error goto [noFile]
  inout$=Upper$(Trim$(inout$))
  if inout$="IN" then open fName$ for input as #contextFile else open fName$ for output as #contextFile
  OpenContextFile$="#contextFile"
  exit function
[noFile]
      OpenContextFile$=""
      */
  return fFile;
}
QString MainWindow::SaveContextFile(QString fName)
{
  //Save specified Contexts to file+ return error message or ""
  //fName$ contains complete path and file name
  //The entries of contextTypes will be set to 1 if the corresponding context is to be saved;

  //fHndl$=OpenContextFile$(fName$,"Out")
  //if fHndl$="" then SaveContextFile$="Context file failed to open; "+fName$ ; exit function
  //newLine$=chr$(13)
  QStringList list;
  int contextCount=0;
  if (vars->contextTypes[vars->constHardware]==1)
  {
    contextCount=contextCount+1;
    list.append("StartContext Hardware\r"+winConfigMan->configHardwareContext(activeConfig)+"\rEndContext");
  }
  if (vars->contextTypes[vars->constGrid]==1)
  {
    contextCount=contextCount+1;
    list.append("StartContext Grid\r"+GridContext()+"\rEndContext");
  }
  if (vars->contextTypes[vars->constTrace]==1)
  {
    contextCount=contextCount+1;
    list.append("StartContext Trace\r"+TraceContext()+"\rEndContext");
  }
  if (vars->contextTypes[vars->constSweep]==1)
  {
    contextCount=contextCount+1;
    list.append("StartContext Sweep\r"+SweepContext()+"\rEndContext");
  }
  if (vars->contextTypes[vars->constMarker]==1)
  {
    contextCount=contextCount+1;
    list.append("StartContext Marker\r"+mMarkerContext()+"\rEndContext");
  }
  if (vars->contextTypes[vars->constBand]==1)
  {
     contextCount=contextCount+1;
     list.append("!StartContext BandLineCal");
     BandLineCalContextToFile( list);
     list.append("!EndContext");
  }
  if (vars->contextTypes[vars->constBase]==1)
  {
     contextCount=contextCount+1;
     list.append("!StartContext BaseLineCal");
     BaseLineCalContextToFile(list);
     list.append("!EndContext");
  }
  if (vars->contextTypes[vars->constGraphData]==1)
  {
     contextCount=contextCount+1;
     list.append("!StartContext GraphData");    //ver115-1d
     GraphDataContextToFile(list);   //ver115-1d
     list.append("!EndContext");
  }
  if (vars->contextTypes[vars->constModeData]==1)   //ver115-1d added type 8
  {
     contextCount=contextCount+1;
     //Print the StartContext line only if preceded by some other context
     if (contextCount>1) list.append("!StartContext DataTable");
     CopyModeDataToVNAData(0); //ver116-4j
     touch.touchWriteOnePortParameters(list, vars->msaMode);  //Write data to file
     if (contextCount>1) list.append("!EndContext");
  }
  //close #fHndl$
  //SaveContextFile="";

  QString settings = list.join("\r");
  QFile fOut(fName);
  if (fOut.open(QFile::WriteOnly | QFile::Text))
  {
    QTextStream s(&fOut);
    s << settings;

    fOut.close();
  }
  return "";
}
void MainWindow::SavePreferenceFile(QString fName)
{
  //Save current preferences file to fName
  //Find out whether we have the MSA_Prefs folder; if not, create it

  if (!QDir().mkpath(DefaultDir+"/MSA_Info/MSA_Prefs"))
  {
    QMessageBox::critical(0,"Error","Cannot create preference file.");
  }

  for (int i=0; i < 30; i++)
  {
    vars->contextTypes[i]=0;
  }
  vars->contextTypes[vars->constGrid]=1;   //Grid
  vars->contextTypes[vars->constTrace]=1;   //Trace
  vars->contextTypes[vars->constSweep]=1;   //Sweep
  QString errMsg=SaveContextFile(fName);
  if (errMsg!="")
    QMessageBox::critical(0,"Error", "Unable to save preferences.");
}
void MainWindow::LoadPreferenceFile(QString fileName)
{
  //Load preference
  if (graph.haltsweep==1)
    FinishSweeping(); //Finish current sweep cleanly

  vars->restoreFileHndl=OpenContextFile(fileName,"In");

  // handle missing file
  if (vars->restoreFileHndl == NULL)
  {
    QFile *fileHndl = OpenContextFile(fileName,"IN");
    if (fileHndl != NULL)
    {
      //A file exists and it is bad
      fileHndl->close();
      QMessageBox::warning(0, "Error in Preference File; ", vars->restoreErr);  //ver114-7n
    }
    else
    {
      vars->restoreErr="";  //Clear error if problem was file does not exist. ver115-3c
    }
    if (vars->restoreErr == "")
    {
      SavePreferenceFile(fileName); //Save Preference file in current format ver115-4a
    }
    vars->restoreFileHndl=OpenContextFile(fileName,"In");
  }



  RememberState();  //So we can see what changed //ver115-8c
  LoadBasicContextsFromFile();   //Load preferences from restoreFileHndl$
  vars->restoreFileHndl->close();
  if (vars->restoreErr!="" && vars->doingInitialization==0)
    QMessageBox::critical(0,"Error", "Error loading preference file: "+vars->restoreErr);
  DetectFullChanges(); //ver115-8c
  if (graph.continueCode==3 && vars->doingInitialization==0)
    PartialRestart(); //implement changes ver115-3c
  return;
}

//================================START DEBUG MODULE=======================================
//This module has routines to save/reload info to allow one person to partially recreate
//the state of another person//s MSA to help in debugging. Saved context files can be reloaded, and
//the copied MSA_Info can be examined or used as needed.

void MainWindow::DebugSaveData()
{
  QString folder=DefaultDir+"/xxDebug";
  if (!QDir().mkpath(folder))
  {
    QMessageBox::warning(this, "Error", "Cannot create Debug folder.");
    return;
  }
  DebugSaveArrays(folder);    //Save various debug arrays as individual files
  /*
      //Save preferences file
  for i=0 to 30 : contextTypes(i)=0: next i
  contextTypes(constGrid)=1   //Grid
  contextTypes(constTrace)=1   //Trace
  contextTypes(constSweep)=1   //Sweep
  errMsg$=SaveContextFile$(folder$;"\Prefs.txt")
  if errMsg$<>"" then notice "Unable to save preferences."
      //Save configuration file
  open folder$;"\config.txt" for output as #configOut
  print #configOut, configHardwareContext$()      //ver114-3h
  close #configOut

  //call DebugCopyFile DefaultDir$;"\MSA_Info\config.txt", DefaultDir$;"\xxDebug\config.txt"
  call DebugCopyDirectory DefaultDir$, "MSA_Info", DefaultDir$;"\xxDebug"
end sub
*/
}
void MainWindow::DebugCopyFile(QString source, QString dest)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*    open source$ for input as #copySource
    open dest$ for output as #copyDest
    print #copyDest, input$(#copySource, lof(#copySource));
    close #copySource
    close #copyDest
end sub
*/
}
void MainWindow::DebugCopyDirectory(QString sourcePath, QString dirName, QString destPath)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*   //Copy the directory in path sourcePath$ whose name is dirName$, into the path destPath$
    //These path names do not end with slashes.
    dest$=destPath$;"\";dirName$
    isErr=mkdir(dest$)
    source$=sourcePath$;"\";dirName$
    files source$, "*", fileInfo$()  //get list of all files and directories
        //copy the files
    numFiles=val(fileInfo$(0,0))    //0,0 has the number of files, which are listed first, from 1,x to numFiles,x
    numFolders=val(fileInfo$(0,1))  //0,1 has the number of folders, which are listd next, from numFiles+1,x to numFiles+numFolders,x

    for i=1 to numFiles
        fileName$=fileInfo$(i,0)  //file name (no path info) is in (i,0)
        call DebugCopyFile source$;"\";fileName$, dest$;"\";fileName$   //copy file from source to dest
    next  i

        //We want to recursively copy all folders in this folder. However, because LB makes arrays global,
        //each recursive call would use the same copy of fileInfo$(). So we save the names of the folders in local variables.
        //This means there will be a max number of allowed folders, but that works for us
        //folder name (no path info) is in (i,1)
    if numFolders>0 then folder1$=fileInfo$(numFiles+1,1)
    if numFolders>1 then folder2$=fileInfo$(numFiles+2,1)
    if numFolders>2 then folder3$=fileInfo$(numFiles+3,1)
    if numFolders>3 then folder4$=fileInfo$(numFiles+4,1)
    if numFolders>4 then folder5$=fileInfo$(numFiles+5,1)
    if numFolders>5 then folder6$=fileInfo$(numFiles+6,1)
    if numFolders>6 then folder7$=fileInfo$(numFiles+7,1)
    if numFolders>7 then folder8$=fileInfo$(numFiles+8,1)
    if numFolders>8 then folder9$=fileInfo$(numFiles+9,1)

    for i=1 to numFolders
        select i
            case 1
                folderName$=folder1$
            case 2
                folderName$=folder2$
            case 3
                folderName$=folder3$
            case 4
                folderName$=folder4$
            case 5
                folderName$=folder5$
            case 6
                folderName$=folder6$
            case 7
                folderName$=folder7$
            case 8
                folderName$=folder8$
            case 9
                folderName$=folder9$
            case else
                notice "Too many folders to copy." : exit sub
        end select
        call DebugCopyDirectory source$, folderName$, dest$  //Copy the contents of the source directory
    next  i
end sub
*/
}
void MainWindow::DebugLoadData()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Menu item to load debug data
    if haltsweep=1 then gosub void MainWindow::FinishSweeping()
    filter$="Text files" + chr$(0) + "*.txt" + chr$(0) + _
                "All files" + chr$(0) + "*.*"   //ver115-6b
    defaultExt$="txt"
    initialDir$=DefaultDir$;"\xxDebug\"
    initialFile$="Prefs.txt"
    fName$=uOpenFileDialog$(filter$, defaultExt$, initialDir$, initialFile$, _
                        "Load Debug Files--Select Any File in the Debug Folder")
    if fName$="" then wait  //Blank means cancelled
    for i=len(fName$) to 1 step -1
        //Delete the actual file name so we just have the directory info
        thisChar$=Right$(fName$,1)
        fName$=Left$(fName$, i-1)   //Drop last character
        if thisChar$="\" or thisChar$="/" then exit for  //Done when we find the separator character
    next i
    //fName$ now has the directory name
    call mClearMarkers
        //Load the preference file
    restoreFileName$=fName$;"\Prefs.txt"
        //Load preference file in restoreFileName$; set restoreErr$ with any error message
    gosub void MainWindow::LoadPreferenceFile()  //ver115-8c

    //Data is loaded from a folder called "xxDebug" in the default directory
    //We load the array info. The configuration info
    //is not used unless it is moved to the proper folder before startup.
    //This takes a long time because of the huge number of data items in the hardware arrays.
    cursor hourglass
    isErr=DebugLoadArrays(fName$)
    cursor normal
    if isErr then notice "Loading Debug Info was Unsuccessful." : wait

    //We must now recreate the graph from datatable(). We proceed as though we just gathered the data
    //point by point
    for thisstep=0 to globalSteps
        if msaMode$<>"SA" then gosub void MainWindow::ProcessDataArrays()    //fill S21DataArray and/or ReflectArray  ver115-8b
        gosub void MainWindow::PlotDataToScreen()    //Plot this point the same as though we just gathered it.
    next thisstep
    #graphBox$, "flush"
    beep
wait
*/
}
void MainWindow::DebugSaveArrays(QString folder)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
/*
  //Save Debug arrays as separate files in folder whose complete path is in folder$
  DebugArrayToFile(vars->magarray, folder+"magarray");
  DebugArrayToFile(vars->phaarray, folder+"phaarray");
  DebugArrayToFile(vars->PLL1array, folder+"PLL1array");
  DebugArrayToFile(vars->PLL3array, folder+"PLL3array");
  DebugArrayToFile(vars->DDS1array, folder+"DDS1array");
  DebugArrayToFile(vars->DDS3array, folder+"DDS3array");
  DebugArrayToFile(vars->cmdallarray, folder+"cmdallarray");
  //DebugArrayToFile(vars->, folder+"freqcoeff");
  //DebugArrayToFile(, folder+"magcoeff");
  DebugArrayToFile(vars->lineCalArray, folder+"linecal");
  DebugArrayToFile(vars->datatable, folder+"datatable");
  */
}
void MainWindow::DebugLoadArrays(QString folder)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Save Debug arrays as separate files in folder whose complete path is in folder$
    //return 1 if error; otherwise 0
    message$="Loading..."
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("magarray", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("phaarray", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("PLL1array", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("PLL3array", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("DDS1array", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("DDS3array", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("cmdallarray", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("freqcoeff", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("magcoeff", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("linecal", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    call PrintMessage : message$=message$;".."
    isErr=DebugGetArrayFromFile("datatable", folder$)
    if isErr then LoadDebugArrays=1 : exit function
    message$="" : call PrintMessage
end function
*/
}
void MainWindow:: DebugArrayAsTextArray(QString arrayID)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Put data points into uTextPointArray$, with header info
    //We save the full datatable() even if data for some steps has not been collected yet.
    //return number of lines placed into uTextPointArray$
    //First line is "!Array=Name", where Name is arrayID$
    //Then comes each array entry as its own string
    uTextPointArray$(1)="!Name=";arrayID$
    if arrayID$="magcoeff" then limit=100 else limit=globalSteps    //Do globalSteps steps, but only 100 for magcoeff
    for i=0 to limit
        thisLine$=""
        aSpace$=""  //starts out blank for each line
        select arrayID$
            case "magarray"
                for item=0 to 3
                    thisLine$=thisLine$;aSpace$;magarray(i, item) : aSpace$=" "
                next item
            case "phaarray"
                for item=0 to 4
                    thisLine$=thisLine$;aSpace$;phaarray(i, item) : aSpace$=" "
                next item
            case "PLL1array"
                for item=0 to 48
                    thisLine$=thisLine$;aSpace$;PLL1array(i, item) : aSpace$=" "
                next item
            case "PLL3array"
                for item=0 to 48
                    thisLine$=thisLine$;aSpace$;PLL3array(i, item) : aSpace$=" "
                next item
            case "DDS1array"
                for item=0 to 46
                    thisLine$=thisLine$;aSpace$;DDS1array(i, item) : aSpace$=" "
                next item
            case "DDS3array"
                for item=0 to 46
                    thisLine$=thisLine$;aSpace$;DDS3array(i, item) : aSpace$=" "
                next item
            case "cmdallarray"
                for item=0 to 39
                    thisLine$=thisLine$;aSpace$;cmdallarray(i, item) : aSpace$=" "
                next item
            case "freqcoeff"
                for item=0 to 3
                    thisLine$=thisLine$;aSpace$;calFreqCoeffTable(i,item) : aSpace$=" "
                next item
            case "magcoeff"
                for item=0 to 7
                    thisLine$=thisLine$;aSpace$;calMagCoeffTable(i, item) : aSpace$=" "
                next item
            case "linecal"
                for item=0 to 2
                    thisLine$=thisLine$;aSpace$;lineCalArray(i, item) : aSpace$=" "
                next item
            case "datatable"
                for item=0 to 3
                    thisLine$=thisLine$;aSpace$;datatable(i, item) : aSpace$=" "
                next item
            case else
                notice "Invalid Debug Array Name" : exit for
        end select
        uTextPointArray$(i+2)=thisLine$
    next i
    DebugArrayAsTextArray=limit+2   //Number of lines
end function
*/
}
void MainWindow:: DebugArrayToFile(QString arrayID, QString fullName)
{
  QFile debugOut(fullName);
  QFileInfo info(fullName);
  QString name = info.baseName();
  if (debugOut.open(QFile::WriteOnly))
  {
    QTextStream s(&debugOut);
    s << "!Name=" << name << "\n";
    debugOut.close();
  }
  /*
  //save array to a file in folder$ whose complete path is in folder$
  //We name the file per arrayID$ and override any existing file
  int nLines=DebugArrayAsTextArray(arrayID$)   //Assemble strings into uTextPointArray$
    fHndl$=DebugOpenOutputFile$(arrayID$, folder$)
    if isErr then notice "Could not save Debug file ";arrayID$ : exit sub
    for i=1 to nLines
        thisLine$=uTextPointArray$(i)
        print #fHndl$, thisLine$  //output each line
    next i
    close #fHndl$
    exit sub
end sub
*/
}
void MainWindow::DebugOpenInputFile$(QString arrayID, QString folder)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Open file; return handle or blank if not successful
    fName$=folder$;"\";arrayID$;".txt"
    On Error goto void MainWindow::fileError()
    open fName$ for input as #DebugArrayFile
    DebugOpenInputFile$="#DebugArrayFile"
    exit function
void MainWindow::fileError()
    notice "Could not open file ";arrayID$
    DebugOpenInputFile$=""  //error
end function
*/
}
void MainWindow::DebugOpenOutputFile$(QString arrayID, QString folder)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Open file; return handle or blank if not successful
    fName$=folder$;"\";arrayID$;".txt"
    On Error goto void MainWindow::fileError()
    open fName$ for output as #DebugArrayFile
    DebugOpenOutputFile$="#DebugArrayFile"
    exit function
*/
}
void MainWindow::MainWindow::fileError()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    notice "Could not open file ";arrayID$
    DebugOpenOutputFile$=""  //error
end function
*/
}
void MainWindow::DebugGetArrayFromFile(QString arrayID, QString folder)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//get points from file; return 1 if error; otherwise 0
    //Restore specified array
    //We will open file, read and close. File name is the same as arrayID$; file is in folder$
    fHndl$=DebugOpenInputFile$(arrayID$, folder$)
    if isErr then DebugGetArrayFromFile=1 : exit function
    isErr=uArrayFromFile(fHndl$,100) //Get data, determine number of items from first line
    if isErr then DebugGetArrayFromFile=1 : close #fHndl$ : exit function
        //Move the data from uWorkArray to gGraphVal
    if uWorkNumPoints>gMaxNumPoints() then call ResizeArrays uWorkNumPoints   //Make sure we have enough room
    for i=1 to uWorkNumPoints
        select arrayID$
            case "magarray"
                for item=0 to 3
                    magarray(i-1,item)=uWorkArray(i, item)
                next item
            case "phaarray"
                for item=0 to 4
                    phaarray(i-1,item)=uWorkArray(i, item)
                next item
            case "PLL1array"
                for item=0 to 48
                    PLL1array(i-1,item)=uWorkArray(i, item)
                next item
            case "PLL3array"
                for item=0 to 48
                    PLL3array(i-1,item)=uWorkArray(i, item)
                next item
            case "DDS1array"
                for item=0 to 46
                    DDS1array(i-1,item)=uWorkArray(i, item)
                next item
            case "DDS3array"
                for item=0 to 46
                    DDS3array(i-1,item)=uWorkArray(i, item)
                next item
            case "cmdallarray"
                for item=0 to 39
                    cmdallarray(i-1,item)=uWorkArray(i, item)
                next item
            case "freqcoeff"
                for item=0 to 3
                    calFreqCoeffTable(i-1,item)=uWorkArray(i, item)
                next item
            case "magcoeff"
                for item=0 to 7
                    calMagCoeffTable(i-1,item)=uWorkArray(i, item)
                next item
            case "linecal"
                for item=0 to 2
                    lineCalArray(i-1,item)=uWorkArray(i, item)
                next item
            case "datatable"
                for item=0 to 3
                    datatable(i-1,item)=uWorkArray(i, item)
                next item
            case else
                notice "Invalid Debug Array Name" : exit for
        end select
    next i
    DebugGetArrayFromFile=0  //no error
    close #fHndl$
    exit function
end function
*/
}

//==================================END DEBUG MODULE=======================================

//================End Configuration Manager Module=================
//==================================================================



//=================End Mag/Freq  Calibration Module===================
//=================End Calibration Manager Module===================
//--SEW Added the following module to provide utility routines used by other modules
//
//==========================UTILITIES MODULE===========================================



int  MainWindow::uArrayFromFile(QFile *fHndl, int nPerLine)   //get points from file into uWorkArray()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //Return 1 if error; otherwise zero
    //fHndl$ is the handle of an already open file. We read our data but do not close the file.
    //The file likely contains a title, some other preliminary info (such as Touchstone info) and then
    //many lines of numeric info, possibly ending with "EndContext". If the first lines begin with !, then
    //up to 3 lines are considered title info, and are put into uWorkTitle(1-3).
    //Once we encounter a line beginning with numeric data, we read it and all succeeding lines into uWorkArray()
    //beginning with entry 1. We stop at the end of file or when we read the line "!EndContext"
    //The required number of data items per line is nPerLine, but if nPerLine>=100 we get the required number
    //from the first point we process.
    //We put the number of points read into uWorkNumPoints and the number of items per line into uWorkNumPerPoint.
    //If all lines do not have the same number of data items, we end with an error.
    //Data on a line is delimited by spaces, tabs or commas.
    nLines=0
    //Break the file into separate lines, listed in uTextPointArray$()
    while EOF(#fHndl$)=0
        Line Input #fHndl$, tLine$
        tLine$=Trim$(tLine$)
        nLines=nLines+1
        if nLines>gMaxNumPoints()+maxPointExtraLines then notice "Too Many Lines" : uArrayFromFile=1 : exit void MainWindow::
        if Left$(tLine$,1)="!EndContext" then nLines=nLines-1 : exit while    //End if we find !EndContext
        uTextPointArray$(nLines)=tLine$ //Add this line to uTextPointArray$()
    wend
    uArrayFromFile=uWorkArrayFromTextArray(nLines, nPerLine) //Transfer data from strings to uWorkArray
*/
  return 0;
}

void MainWindow::uArrayFromString(QString s, int startN, int &startPos, int nPerLine)   //get points from string into uWorkArray()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //Return 1 for error; otherwise 0.
    //s$ is a string with lines delimited by carriage returns.
    //The lines likely contains a title, some other preliminary info (such as Touchstone info) and then
    //many lines of numeric info, possibly ending with "EndContext".
    //We start at position startPos. We update startPos tothe beginning of the first line after the last one
    //we process, or one past the end of the string.
    //If the first lines begin with !, then
    //up to 3 lines are considered title info, and are put into uWorkTitle(1-3).
    //Once we encounter a line beginning with numeric data, we read it and all succeeding lines into uWorkArray()
    //beginning with entry startN. We stop at the end of string or when we read the line "!EndContext"
    //The required number of data items per line is nPerLine, but if nPerLine>=100 we get the required number
    //from the first point we process.
    //We put the number of points read into uWorkNumPoints and the number of items per line into uWorkNumPerPoint.
    //If all lines do not have the same number of data items, we end with an error.
    //Data on a line is delimited by spaces, tabs or commas.
    nLines=0
    //Break the string into separate lines, listed in uTextPointArray$()
    sLen=len(s$)
    //ver114-5o added lines to skip over !StartContext if it is the first line
    saveStartPos=startPos
    firstLine$=uGetLine$(s$, startPos) //get data line and increment startPos to next line
    //If first line is !StartContext, skip it and start at the next line. startPos was already incremented
    if Upper$(Left$(firstLine$,12))<>"!STARTCONTEXT" then startPos=saveStartPos     //backup to start

    while startPos<=sLen
        sepPos=instr(s$, chr$(13), startPos)
        oldStartPos=startPos
        if sepPos=0 then
            tLine$=Trim$(Mid$(s$, startPos)) : startPos=sLen+1 //Entire remainder is final line
        else
            tLine$=Trim$(Mid$(s$, startPos, sepPos-startPos)) : startPos=sepPos+1  //This line goes to next CR  //ver114-2a
        end if
        if tLine$="!EndContext" then uArrayFromString=startPos : exit while  //ver114-2a
        nLines=nLines+1
        uTextPointArray$(nLines)=tLine$ //Add this line to uTextPointArray$()
    wend
    uArrayFromString=uWorkArrayFromTextArray(nLines, nPerLine) //Transfer data from strings to uWorkArray
*/
}

void MainWindow::uHighlightText(int handle)  //handle$ is handle variable for the target text box to highlight
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     handle = hWnd(#handle$)
    #handle$, "!Contents? txt$"
    param1 = Len(txt$)
    CallDLL #user32, "SendMessageA", _
    handle as Ulong, _
    _EM_SETSEL as Long, _
    param1 as Long, _
    0 as Long, _
    result as Long
*/
}
/*
void MainWindow::uSleep tms //wait for tms milliseconds
{    tms=int(tms+0.5)    //avoids crash if fractional
    if tms>0 then CALLDLL #kernel32, "Sleep", tms as ulong, Sleep as void   //ver116-1b

}*/
void MainWindow::uSleep(int ms)
{
  QMutex dummy;
  dummy.lock();
  QWaitCondition waitCondition;
  waitCondition.wait(&dummy, ms);
}


void MainWindow::uTickCount()   //Return windows tick count ver116-1b
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //ms since system started, max about 49 days.
    CALLDLL #kernel32, "GetTickCount", tick as ulong
    uTickCount=tick
*/
}

void MainWindow::uParsePath(QString fullPath, QString &folder, QString &file) //Parse full path name of file into the file name and folder name (no slash at end)
{
  //check to make sure we don't have a blank string as absolutePath is undfined if blank
  if (fullPath.trimmed() == "")
  {
    folder = "";
    file = "";
  }
  QFileInfo info1(fullPath);

  folder = info1.absolutePath();
  file = info1.fileName();
}

int MainWindow::uConfirmSave(QString fullPath) //if file fullPath$ exists, then ask user to confirm saving file. Return 1 if save, 0 if cancel.
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     call uParsePath fullPath$, folder$, file$   //separate into folder and file name
    if file$="" then uConfirmSave=0 : exit void MainWindow:://No valid file name
    files folder$, file$, fileInfo$()   //Look for matching files in this folder
    if val(fileInfo$(0, 0)) > 0 then   //This is number of matching files
        Confirm "File "+file$+" already exists. Do you want to replace it?"; response$
        if response$="yes" then uConfirmSave=1 else uConfirmSave=0
    else
        uConfirmSave=1
    end if
*/
  return 0;
}
QString MainWindow::uAddExtension(QString name, QString exten)    //Add extension to path or file name if there is no extension
{
  if (name.indexOf(".") > -1)   //There already is an extension
    return name;

  return name + "." + exten;
  /*
  //exten$ should not have a dot in it. e.g. bitmap exten$ is bmp, not .bmp.
  int L=name.length();
  dotPos=0;
  for (int i=L; i > 1; i--)  //scan character by character from end
  {
    QString thisChar=name.mid(i, 1);
    if (thisChar==".") {dotPos=i; break; } //looking for dot
    if (thisChar=="/") break;  //looking for backslash--indicates there is no extension
  }
  if (dotPos>0) { uAddExtension$=name$ : exit void MainWindow::  //There already is an extension
  uAddExtension$=name$;".";exten$  //append dot and exten$
        */
}

void MainWindow::uDeleteFile(QString fullName)   //Kill file if it exists; no error if it doesn//t
{
  QFile::remove(fullName);
}

int MainWindow::uFileOrFolderExists(QString Path) // checks for the existence of the given file or folder
{
  int retVal = 0;
  QFileInfo info(Path);
  if (info.exists())
    retVal = 1;

  return retVal;
}

QString  MainWindow::uSaveFileDialog(QString filter, QString defaultExt, QString initialDir, QString initialFile, QString windTitle)
{
  QString retVal;
  retVal = QFileDialog::getSaveFileName(this, windTitle, initialDir, filter);
  return retVal;
}

QString MainWindow::uOpenFileDialog(QString filter, QString defaultExt, QString initialDir, QString initialFile, QString windTitle)
{/*
  QStringList filt;
  filt = filter.split(";;");
  int filterIndex = filt.indexOf(defaultExt);
*/
  //QFileInfo ttt(initDir);
  //initalDir = ttt.absoluteFilePath();
  QString retVal;
  retVal = QFileDialog::getOpenFileName(this, windTitle, initialDir, filter);
  //retVal = "C:\\Qt\\projects\\MSA\\MSA\\debug\\test_save_data.s1p";
  return retVal;
}

bool MainWindow::uVerifyDLL(QString dllName)
{

  //Return 1 if specified DLL exists; otherwise 0
  //dllName$ can include or exclude the .dll extension
  //First turn off Windows error message for missing dll

  QLibrary *Lib = new QLibrary(dllName);
  if (!Lib->load())
  {
    qDebug() << dllName << " " << Lib->errorString();
    delete Lib;
    return false;
  }
  Lib->unload();
  delete Lib;
  return true;
}


QString MainWindow::uExtractFontColor(QString font)  //Return the color from a font specification
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  int pos=font.indexOf("color");
  if (pos=-1) return "";
  int semiPos=font.indexOf(";", pos);  //Find semicolon after "color"
  if (semiPos=-1) semiPos=font.length()+1;  //Pretend it is one past end if we didn//t find it
  //QString colorSpec=font.mid(pos, semiPos-pos);
  return  font.mid(6).trimmed(); //Everything except "color" is the actual color
  */
  return "fix me";
}

void MainWindow::FindPeakSteps(int dataType, int startStep, int endStep, int &minNum, int &maxNum, int &minY, int &maxY)    //find positive and negative peak
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //Search includes steps from s1 to s2, inclusive. dataType indicates the graph data to search (e.g. constRho),
    //so we don//t necessarily search the current graph.
    //minNum and maxNum will be set to the steps where the peaks occur; minY and maxY will be the peak values
    //Peak includes max and min
    //The result may be a non-integer; if two or more successive points have the same peak value, then the peak
    //step will be in the middle.
    minPeakEnded=1 : maxPeakEnded=1
    saveY1Type=Y1DataType : Y1DataType=dataType //Values will be calculated for Y1
    saveY2Type=Y2DataType : Y2DataType=constNoGraph

    for i=startStep to endStep
        call CalcGraphData i,y, dum,0   //Calculate Y1 data type from data arrays
        if i=p1 then
            minNumStart=p1: maxNumStart=p1
            minNumEnd=p1: maxNumEnd=p1
            minY=y : maxY=y //Start with values at first point
        else
            //See if peak is found. Once found, so long as we remain at that level, continue
            //to record maxPeakEnded
            if y>maxY then maxY=y : maxNumStart=i : maxPeakEnded=0 //new max
            if maxPeakEnded=0 and y>=maxY then maxNumEnd=i else maxPeakEnded=1
            if y<minY then minY=y : minNumStart=i : minNumEnd=i : minPeakEnded=0    //new min
            if minPeakEnded=0 and y<=minY then minNumEnd=i else minPeakEnded=1
        end if
    next i
    //Here the min or max start and end numbers indicate where the peak started and ended; we consider
    //the actual peak to be in the middle.
    maxNum=(maxNumEnd+maxNumStart)/2
    minNum=(minNumEnd+minNumStart)/2
    Y1DataType=saveY1Type
    Y2DataType=saveY2Type
*/
}

float MainWindow::StepWithValue(int dataType, int startStep, int dir, int targVal)   //Return first step number (maybe fractional) with the specified value
{
//start with step startStep and move up (dir=1) or down (dir=-1). Return result
  //We look at the specified dataType (constAngle, etc.)
  //If value is not found, return -1
  //Linear interpolation is done if necessary to find the exact point (possibly fractional) with the specified value.
  //startStep=int(startStep)
  int pMin, pMax;
  graph.gGetMinMaxPointNum(pMin, pMax);
  int stepMin=pMin-1; int stepMax=pMax-1;
  int endStep;
  if (dir==1) endStep=stepMax; else endStep=stepMin;
  if (startStep<stepMin) startStep=stepMin;
  if (startStep>stepMax) startStep=stepMax;

  //  fix me need to finish converting the rest of the function
  int saveY1Type=vars->Y1DataType; vars->Y1DataType=dataType; //Values will be calculated for Y1
  int saveY2Type=vars->Y2DataType; vars->Y2DataType=vars->constNoGraph;

  int dum;
  int stepVal;
  graph.CalcGraphData(startStep, stepVal, dum, 0);  //Calculate Y1 data type from data arrays
  if (stepVal==targVal)
  {
    vars->Y1DataType=saveY1Type;
    vars->Y2DataType=saveY2Type;

    return startStep;

  }   //Hit it exactly
  if (startStep==endStep)
  {
    vars->Y1DataType=saveY1Type;
    vars->Y2DataType=saveY2Type;
    return -1;
  }
  int prevVal=stepVal; int prevNum=startStep;    //ver115-1e

  int foundCrossing=0;     //Sets to 1 when value reaches or crosses target

  if (dir != 0)
  {
    QMessageBox::warning(this, "Coding Error", "read the source");
    // need to fix the for loop, as dir was only ever 0 in the Liberty code
  }
  for (int i=startStep+dir; i <= endStep; i++)  // step dir      //Start at second step
  {
    graph.CalcGraphData(i,stepVal, dum, 0);  //Calculate Y1 data type from data arrays
    if (stepVal==targVal)
    {
      return i; //Hit it exactly
    }
    if ((prevVal<targVal && stepVal>targVal) || (prevVal>targVal && stepVal<targVal))
    {
      foundCrossing=1;
      break;  // : exit for
    }
    prevVal=stepVal; prevNum=i;
  }

  float retVal;
  if (foundCrossing)
  {
        //prevVal is on one side of targVal; stepVal is on the other.
    int dif1=targVal-prevVal;
    int dif2=stepVal-prevVal;
    if (vars->DataTypeIsAngle(dataType))
    {
        //If angle difference has magnitude over 180 degrees, we assume wraparound
        //occurred and reduct the difference
        if (dif1>180) dif1=dif1-360;
        if (dif1<-180) dif1=dif1+360;
        if (dif2>180) dif2=dif2-360;
        if (dif2<-180) dif2=dif2+360;
    }
    float fract=dif1/dif2;
    retVal = prevNum+fract; //Return fractional point number with exact targVal
  }
  vars->Y1DataType=saveY1Type;
  vars->Y2DataType=saveY2Type;
  return retVal;
}

void MainWindow::uSeriesRLCFromPoints(float Z1r, float Z1i, float f1, float Z2r, float Z2i, float f2, float &R, float &L, float &C)   //From Z at two points, calculate series RLC components
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //The Z's are real and imaginary impedance at points 1 and 2; f1 and f2 are frequency in MHz
    //We return R (ohms), L (Henries) and C (Farads).
    //In theory Z1r=Z2r, but we use the one with the lower reactance value
    //because it gets measured most accurately.
    if abs(Z1i)<=abs(Z2i) then R=Z1r else R=Z2r
    if R<0 then R=0
    freqScale=1000000*2*uPi() //converts MHz to radians/sec
    w1=f1*freqScale : w2=f2*freqScale

    den=w1^2*w2*Z2i-w2^2*w1*Z1i
    if den=0 then C=constMaxValue else C=(w2^2-w1^2)/den
    //We determine L from C at one of the points. The best accuracy would be from the point with
    //the higher frequency, where the L is more dominant.
    if w1>w2 then
        w=w1 : X=X1
    else
        w=w2 : X=X2
    end if
    den=w^2*C
    if den=0 then L=constMaxValue else L=(w*X*C+1)/den
    if C<0 then C=0
    if L<0 then L=0
    if C<1e-14 then C=0
    if L<1e-11 then L=0
    if C>10 then C=10
    if L>10 then L=10
*/
}

void MainWindow::uParallelRLCFromPoints(float Z1r, float Z1i, float f1, float Z2r, float Z2i, float f2, float &R, float &L, float &C)   //From Z at two points, calculate parallel RLC components
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //The Z's are real and imaginary impedance at points 1 and 2; f1 and f2 are frequency in MHz
    //We return R (ohms), L (Henries) and C (Farads).
    //Convert the impedances to equivalent parallel resistance||reactance
    if Z1i=0 then X1=constMaxValue else X1=(Z1r^2+Z1i^2)/Z1i
    if Z2i=0 then X2=constMaxValue else X2=(Z2r^2+Z2i^2)/Z2i

    if Z1r=0 then R1=constMaxValue else R1=(Z1r^2+Z1i^2)/Z1r
    if Z2r=0 then R2=constMaxValue else R2=(Z2r^2+Z2i^2)/Z2r

    freqScale=1000000*2*uPi() //converts MHz to radians/sec
    w1=f1*freqScale : w2=f2*freqScale

    //In theory R1=R2, but we pick the one with the best measurement accuracy, meaning the one
    //with the highest parallel X.
    if abs(X1)>abs(X2) then R=R1 else R=R2
    if R<0 then R=0

        //Calculate C from X
    den=X1*X2*(w1^2-w2^2)
    if den=0 then C=constMaxValue else C=(w2*X1-w1*X2)/den

    //We determine L from C at one of the points. The best accuracy would be from the point with
    //the higher frequency, where the L is more dominant.
    if w1>w2 then
        w=w1 : X=X1
    else
        w=w2 : X=X2
    end if
    den=w+X*w^2*C
    if den=0 then L=constMaxValue else L=X/den
    if C<0 then C=0
    if L<0 then L=0
    if C<1e-14 then C=0
    if L<1e-11 then L=0
    if C>10 then C=10
    if L>10 then L=10
*/
}

//=================End Utilities Module=======================
/*
'=====================Start Complex Functions Module====================

sub cxRectToPolarRad R,I, byref m, byref ang  'Convert rectangular coordinates to polar(rad)
    'R+jI to mag m at angle ang (radians, -pi to +pi)
   ang=uATan2(R, I)*0.0174532925199433
   m=sqr(R^2+I^2)   'mag
end sub

sub cxPolarRadToRect  m,ang, byref R, byref I   'Convert polar(rad) coordinates to rectangular
    'mag m at angle ang (radians) to R+jI
    if m<0 then m=0-m : ang=ang+uPi()   'Negative mag; point in opposite direction
    R=m*cos(ang)   'Trig is done in radians
    I=m*sin(ang)
end sub

sub cxInvert R, I, byref Rres, byref Ires     'Invert complex number R + jI; put into Rres, Ires
    '1/(R+jI)=(R-jI)/(R^2+I^2)
    D=R^2+I^2
    if D=0 then Rres=constMaxValue : Ires=0 : exit sub
    Rres=R/D : Ires=0-I/D
end sub

sub cxDivide Rnum, Inum, Rden, Iden, byref Rres, byref Ires     'Divide (Rnum + jInum)/(Rden + jIden); put into Rres, Ires
    if Rnum=0 and Inum=0 then Rres=0: Ires=0 : exit sub   '0 numerator means zero result; we do this even if denominator=0
    on error goto [MathErr] 'ver115-4d
    'First invert the denominator
    D=Rden^2+Iden^2
    if D=0 then Rres=constMaxValue : Ires=0: exit sub
    Rinv=Rden/D : Iinv=0-Iden/D
    'Now multiply Rnum+jInum times Rinv+jIinv
    Rres=Rnum*Rinv-Inum*Iinv
    Ires=Rnum*Iinv+Inum*Rinv
    exit sub
[MathErr]
    notice "Division Error"
    Rres=constMaxValue : Ires=0
end sub

'ver115-4e renamed cxMultiply
sub cxMultiply R1, I1, R2, I2, byref Rres, byref Ires     'Multiply (R1 + jI1)*(R2 + jI2); put into Rres, Ires
    on error goto [MathErr]  'ver115-4d
    Rres=R1*R2-I1*I2
    Ires=R1*I2+I1*R2
    exit sub
[MathErr]
    notice "Multiplication Error"
    Rres=constMaxValue : Ires=0
end sub

sub cxSqrt R,I, byref resR, byref resI 'Square root of R+jI is resR+jresI (non-neg real part)
    'Square root of X; branch cut on negative X-axis
    magR = abs(R):magI = abs(I)
    if magR=0 and magI=0 then resR=0 : resI=0 : exit sub  'square root of zero equals zero
    If (magR >= magI) Then
        t = magI / magR
        w = sqr(magR) * sqr(0.5 * (1 + sqr(1 + t * t)))
    Else
        t = magR / magI
        w = sqr(magI) * sqr(0.5 * (t + sqr(1 + t * t)))
    End If

    if w=0 then resR=0 : resI=0 : exit sub

    if (R >= 0) then
        resR=w
        resI=I / (w + w)
    else
        'Note that resR will always be non-negative, since w is non-negative
        resR = magI/ (w + w)
        if I >= 0 then
            resI = w
        else
            resI = 0-w
        end if
    end if
end sub

sub cxNatLog R,I, byref resR, byref resI 'Nat log of R+jI is resR+jresI
    call cxRectToPolarRad R,I, m, ang  'Convert to mag, radian format
        'Take the log of the magnitude and carry over the same angle (radians) as imaginary part
    resR=log(m)
    resI=ang
end sub

sub cxEPower R,I, byref resR, byref resI 'e to powere of R+jI is resR+jresI
    if R=0 and I=0 then resR=1 : resI=0 : exit sub  'e^0 is 1
    ex=exp(R)
    if I=0 then resR=ex : resI=0 : exit sub    'For real X, e^X is simple
    resR=ex*cos(I) : resI=ex*sin(I)      'e^x=e^R*(cos(I)+j*sin(I))
end sub

sub cxCos R,I, byref resR, byref resI 'Cosine of X=R+jI is resR+jresI
    '[e^(X*j)+ e^(-X*j)]/2
    call cxEPower 0-I,R,R1,I1    'e^(X*j)
    call cxEPower I,0-R,R2,I2    'e^(-X*j)
    resR=(R1+R2)/2
    resI=(I1+I2)/2
end sub

sub cxSin R,I, byref resR, byref resI 'Sine of X=R+jI is resR+jresI
    '[e^(X*j)- e^(-X*j)]/(2*j)  Note 1/(2*j)=-j/2
    call cxEPower 0-I,R,R1,I1    'e^(X*j)
    call cxEPower I,0-R,R2,I2    'e^(-X*j)
    resR=(I1-I2)/2      '(a+jb)/(2j)=-j(a+jb)/2=(b-ja)/2
    resI=(R2-R1)/2
end sub

sub cxTan R,I, byref resR, byref resI 'Tan of X=R+jI is resR+jresI
    'sin(X)/cos(X)
    if R=0 and I=0 then resR=0 : resI=0 :  exit sub 'tan(0) is 0
    call cxSin R,I,sR,sI      'sin(X)
    call cxCos R,I,cR,cI      'cos(X)
    call cxDivide sR,sI,cR,cI,resR,resI  'sin(X)/cos(X)
end sub

sub cxCot R,I, byref resR, byref resI 'Cotan of X=R+jI is resR+jresI
    'cot(X)=tan(pi/2-X)
    halfPi=uPi()/2
    call cxTan halfPi-R, 0-I, resR, resI  'Tan(pi/2-X)
end sub

sub cxASin R,I, byref resR, byref resI 'Arcsine of X=R+jI is resR+jresI
    'arcsin(x)=-j*log(jX+sqrt(1-X^2))
    call cxMultiply R,I,0-R,0-I,xsqR, xsqI    '-X^2
    xsqR=xsqR+1         '1-X^2
    call cxSqrt xsqR,xsqI, sqrtR, sqrtI           'sqrt(1-X^2)
    call cxNatLog sqrtR-I, sqrtI+R, logR, logI    'log[jX+sqrt(1-X^2)]
    resR=logI            '-j*log[jX+sqrt(1-X^2)]
    resI=0-logR
 end sub

sub cxACos R,I, byref resR, byref resI 'Arccos of X=R+jI is resR+jresI
    'arccos(x)=-j*log(X+sqrt(X^2-1))=pi/2 - arcsin(X)
    call cxASin R,I,aR,aI                    'Arcsin(X)
    halfPi=uPi()/2
    resR=halfPi-aR           'pi/2 - arcsin(X)
    resI=0-aI
end sub

sub cxCosh R,I, byref resR, byref resI 'Cosh of X=R+jI is resR+jresI
        'hyperbolic cosine of X
        ' Cosh(z) = (e^z + e^(-z))/2 = cos(j*z)
    call cxCos 0-I, R,resR, resI          'cos(j*X)
end sub

sub cxSinh R,I, byref resR, byref resI 'Sinh of X=R+jI is resR+jresI
        'hyperbolic sine of X
        ' Sinh(z) = (e^z - e^(-z))/2 = -j*sin(j*z)
    call cxSin 0-I, R, sR, sI            'sin(j*X)
    resR=sI   '-j*sin(j*x)
    resI=0-sR
end sub

sub cxTanh R,I, byref resR, byref resI 'Tanh of X=R+jI is resR+jresI
 'hyperbolic tangent of X
       ' Tanh(z) = (sinh(z)/cosh(z))
    call cxSinh R,I,sR,sI        'sinh(X)
    call cxCosh R,I,cR,cI        'cosh(X)
    call cxDivide sR, sI, cR, cI, resR, resI    'sinh(X)/cosh(X)
end sub

'=====================End Complex Functions Module====================
*/

//==========================Two-Port Module  ver116-1b============================

//Individual scans can be assembled into two-port S-Params by adding them to the appropriate
//entries in TwoPortArray, whose first index specifies the step number, and second index indicates
//Frequency(0), S11(1=db, 2=angle), S21(3=db, 4=angle), S12(5=db, 6=angle) and S22(7=db, 8=angle)
//Frequency is the true frequency in MHz, not the equivalent 1G frequency.
//This array is resized when necessary, not in ResizeArrays

void MainWindow::TwoPortInitVariables()
{
  //Initialize variables including Y axis default ranges for each category
  TwoPortRefDBTop=0 ; TwoPortRefDBBot=-75;
  TwoPortTransDBTop=0 ; TwoPortTransDBBot=-100;
  TwoPortPhaseTop=180 ; TwoPortPhaseBot=-180;
  TwoPortStabilityTop=5 ; TwoPortStabilityBot=0;  //ver116-2a
  TwoPortZMagTop=250 ; TwoPortZMagBot=0;
  TwoPortReactTop=250 ; TwoPortReactBot=-250;  //ver116-4e
  TwoPortRTop=250 ; TwoPortRBot=-250;  //ver116-4e
  TwoPortCTop=1e-8 ;  TwoPortCBot=0;   //ver116-4e
  TwoPortLTop=1e-6 ;  TwoPortLBot=0;  //ver116-4e
  TwoPortReturnLossTop=75 ; TwoPortReturnLossBot=0;   //ver116-4e
  TwoPortFileDataForm="DB" ; TwoPortFileFreqForm="MHz";  //ver116-4m

  TwoPortMatchParamValid=0;
  TwoPortS11S22Form="S";
  TwoPortSourceIsCap=1 ; TwoPortLoadIsCap=1;
  TwoPortSourceComponValue=0 ; TwoPortLoadComponValue=0;
  TwoPortSourceIsShunt=1 ; TwoPortLoadIsShunt=1;
  TwoPortSourceZR=50 ; TwoPortLoadZR=50;
}

void MainWindow::TwoPortResize(int maxSteps)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub TwoPortResize maxSteps 'Resize TwoPortArray to hold maxSteps steps and clear sweep description
    'To save time, we never make the array smaller, unless it is huge
    if maxSteps>TwoPortMaxSteps or (maxSteps<TwoPortMaxSteps and TwoPortMaxSteps>1000) then
        redim TwoPortArray(maxSteps,8)
        redim TwoPortABCD(maxSteps,8)
        redim TwoPortMatchedSParam(maxSteps,8)
        TwoPortMaxSteps=maxSteps    'Caller must deal with TwoPortNumSteps
    end if
    'Enter default values for virtually no reflection or transmission. These will persist until
    'actual data is entered. This allows a valid two-port to be created with only actual S11 data.
    for i=0 to TwoPortMaxSteps 'Set to default values ver116-1b
        TwoPortArray(i,0)=0
        TwoPortArray(i,1)=-199 : TwoPortArray(i,2)=0
        TwoPortArray(i,3)=-199 : TwoPortArray(i,4)=0
        TwoPortArray(i,5)=-199 : TwoPortArray(i,6)=0
        TwoPortArray(i,7)=-199 : TwoPortArray(i,8)=0
    next i
    for i=0 to 4 : TwoPortValidEntries(i)=0 : next i  'We have no valid data
    for i=0 to 8 : TwoPortParamShowing(i)=0 : next i
    TwoPortMatchParamValid=0
    call mClearMarkers  'we assume the existing markers will make no sense
end sub
*/

}

int MainWindow::TwoPortGetY1Type()
{
  return TwoPortY1Type;
}

int MainWindow::TwoPortGetY2Type()
{
  return TwoPortY2Type;
}

void MainWindow::TwoPortDetermineGraphDataFormat(int componConst, QString &yAxisLabel, QString yLabel, int &yIsPhase, QString &yForm)
{
  //Return format info
  //graph.componConst indicates the data type. We return
  //yAxisLabel$  The label to use at the top of the Y axis
  //yLabel$   A typically shorter label for the marker info table
  //yIsPhase$ =1 if the value represents phase. This indicates whether we have wraparound issues.
  //yForm$    A formatting string to send to uFormatted$() to format the data
  //
  QString SRef;
  yIsPhase=0;  //Default, since most are not phase
  if (graph.componConst == vars->constTwoPortS11DB
      || graph.componConst == vars->constTwoPortS22DB
      || graph.componConst == vars->constTwoPortMatchedS11DB
      || graph.componConst == vars->constTwoPortMatchedS22DB) //ver116-4b
  {
    if (graph.componConst==vars->constTwoPortS11DB) SRef="S11";
    if (graph.componConst==vars->constTwoPortS22DB) SRef="S22";
    if (graph.componConst==vars->constTwoPortMatchedS11DB) SRef="*S11";   //Add * for matched param
    if (graph.componConst==vars->constTwoPortMatchedS22DB) SRef="*S22";
    if (TwoPortS11S22Form=="S")
    {
      yAxisLabel=SRef+" dB"; yLabel=SRef+"dB";
      yForm="####.###";
    }
    else //will graph reflection in another form, using the same data type constants as the regular graph
    {
      DetermineGraphDataFormat(TwoPortReflectTypeDB, yAxisLabel, yLabel,yIsPhase, yForm);     //ver116-4b
      yAxisLabel=SRef+" "+yAxisLabel; yLabel=SRef+" "+yLabel;   //ver116-4b
    }
  }
  else if (graph.componConst == vars->constTwoPortS21DB
           || graph.componConst == vars->constTwoPortS12DB
           || graph.componConst == vars->constTwoPortMatchedS21DB
           || graph.componConst == vars->constTwoPortMatchedS12DB) //ver116-4b
  {
    if (graph.componConst==vars->constTwoPortS21DB) SRef="S21";
    if (graph.componConst==vars->constTwoPortS12DB) SRef="S12";
    if (graph.componConst==vars->constTwoPortMatchedS21DB) SRef="*S21";
    if (graph.componConst==vars->constTwoPortMatchedS12DB) SRef="*S12";
    yAxisLabel=SRef+" dB"; yLabel=SRef+"dB";
    yForm="####.###";
  }
  else if (graph.componConst == vars->constTwoPortS11Ang
           || graph.componConst == vars->constTwoPortS22Ang
           || graph.componConst == vars->constTwoPortMatchedS11Ang
           || graph.componConst == vars->constTwoPortMatchedS22Ang)
  {
    if (graph.componConst==vars->constTwoPortS11Ang) SRef="S11";
    if (graph.componConst==vars->constTwoPortS22Ang) SRef="S22";
    if (graph.componConst==vars->constTwoPortMatchedS11Ang) SRef="*S11";
    if (graph.componConst==vars->constTwoPortMatchedS22Ang) SRef="*S22";
    if (TwoPortS11S22Form=="S")
    {
      yAxisLabel=SRef+" Deg"; yLabel=SRef+"Deg";
      yForm="#####.##";
      yIsPhase=1;
    }
    else //will graph reflection in another form, using the same data type constants as the regular graph
    {
      DetermineGraphDataFormat(TwoPortReflectTypeAng, yAxisLabel, yLabel,yIsPhase, yForm);     //ver116-4b            end if
      yAxisLabel=SRef+" "+yAxisLabel; yLabel=SRef+" "+yLabel;   //ver116-4b
    }
  }
  else if (graph.componConst == vars->constTwoPortS12Ang || graph.componConst == vars->constTwoPortS21Ang)
  {
    if (graph.componConst==vars->constTwoPortS12Ang) SRef="S12"; else SRef="S21";
    yAxisLabel=SRef+" Deg"; yLabel=SRef+"Deg";
    yIsPhase=1;
    yForm="#####.##";
  }
  else if (graph.componConst == vars->constTwoPortMatchedS21Ang || graph.componConst == vars->constTwoPortMatchedS12Ang) //ver116-2a
  {
    if (graph.componConst==vars->constTwoPortMatchedS21Ang) SRef="*S21"; else SRef="*S12";
    yAxisLabel=SRef+" Deg"; yLabel=SRef+"Deg";
    yIsPhase=1;
    yForm="#####.##";
  }
  else if (graph.componConst == vars->constTwoPortKStability) //ver116-2a
  {
    yAxisLabel="K Stability"; yLabel="K";
    yIsPhase=0;
    yForm="###.###";
  }
  else if (graph.componConst == vars->constTwoPortMuStability) //ver116-2a
  {
    yAxisLabel="Mu Stability"; yLabel="Mu";
    yIsPhase=0;
    yForm="###.###";
  }
  else if (graph.componConst == vars->constNoGraph)
  {
    yAxisLabel="None"; yLabel="None";
    yForm="####.##";    //Something valid, in case it gets mistakenly used

  }
  else
  {
    yForm="###.##";
    yAxisLabel="Invalid";  yLabel="Invalid";
  }
}



//==========================End Two-Port Module=================================

//===========================Real-Time Graphing Module=====================================
//Version 1.01
//Including module must also include the Utilities module
//Must call gInitFirstUse before any other routines
//
//   This module graphs lines on a background grid. It can do a normal "static" graph in which all
//   data exists in an array and is graphed all at once, and then graphing is complete. But it can
//   also graph one or two lines "dynamically", where the graph process continues repeatedly with
//   constantly changing data. In a dynamic graph, as a changed data point becomes available, the
//   old interval leading to that point is erased and the new one is drawn.
//
//   An array gGraphVal() is maintained, which can hold x-values and up to 2 y values for each x value.
//   It can be cleared by ClearPoints and points can be added by AddPoints. Points can be changed with
//   ChangePoints. The array of points can be graphed with gDrawGraphs.
//
//   Alternatively, the user can maintain point values and draw the graphs point-by-point. For static
//   graphs this is done with Draw routines, and any number of graphs can be drawn. For dynamic graphs,
//   a combination of Erase and Draw routines is used, and two graphs can be drawn/erased point-by-point
//   simultaneously.
//
    //The graph is contained in a grid with outer rectangular boundary and interior grid lines, which
    //is printed by calling PrintGrid. Graphs may be normal traces or a histogram.
    //The X axis is on the bottom, the Y1 axis on the left and the Y2 axis on the right.
    //X and Y numeric values have to be converted to pixel coordinates for drawing. Note that in
    //pixel terms, (0,0) is the top left, and increasing pixel Y values move downward, whereas
    //we want increasing numeric Y values to move upward. Conversion to pixel coordinates is done
    //by maintaining offset and scale values for each coordinate. Values on each coordinate may be
    //graphed linearly or on a logarithmic scale.
    //The grid consists of a number of horizontal and vertical divisions as specified by the user. The
    //span and scale of each axis are also specified. Alternatively, axis span and scale amd divisions
    //can be determined automatically.
        //Conversion from values to pixel coordinates is done as follows:
        //Linear:        xPixel= gXScale*(X-gXAxisMin)+gMarginLeft
        //Log Sweep:     xPixel= gXScale*(log10(X/gXAxisMin))+gMarginLeft
        //where X is the numeric values of X. Y//s are done similarly; y scales are made negative
        //xPixel, yPixel are the pixel location values used in the graphics command


//       -------Drawing: Dynamic graphing------
//   These routines are called by the user for the dynamic draw/erase procedure.
//   (0)Call gDrawGrid to clear screen and to draw the grid and text which are the background for the graphs.
//   (1)Call gInitDynamicDraw to initialize the process
//   (2)Call gDynamicDrawPoint to save and draw all points
//   (3)Call gStartNextDynamicScan at the start of each successive scan
//   (4)call gDynamicDrawPoint to erase/redraw the points of successive scan.
//   (5)flush graphics when graphing is halted
//   (6)If desired, call gDrawGrid and gDrawGraphs to redraw the final data without erasure residue
//   (7)If the scan is redrawn with gDrawGraphs, call gPauseDynamicScan before and gResumeDynamicScan after.
//   If the x-values are changed, the grid must be redrawn and the process repeated.
//sub gInitDynamicDraw nEraseLead1, nEraseLead2  //Initialize to commence dynamic draw process
//sub gStartNextDynamicScan   //Prepare for next pass of dynamic draw
//sub gResumeDynamicScan startPointNum //Resume scanning after screen has been fully redrawn
//sub gDynamicDrawPoint x, y1, y2 //Do segment erase and redraw for dynamic drawing
//sub gInitErase  //Initialize Erase.

void MainWindow::gInitFirstUse(QString hnd, int winWidth, int winHt, int marLeft, int marRight, int marTop, int marBot)
{
  graph.gNumMarkers=20;  //ver114-4d Some may not be in use at any given time
  graph.gSetMaxPoints(800);
  graph.gUpdateGraphObject(winWidth, winHt, marLeft, marRight, marTop, marBot);
  ui->graphicsView->fitInView(graph.getScene()->sceneRect());
  graph.gInitDefaults();
  graph.gClearMarkers();
  graph.gIsFirstDynamicScan=0;
  graph.gIsDynamic=0;
  graph.gGridString="";
  graph.gGridBmp="";
  graph.gInitCustomColors(); //Initialize custom color sets ver116-4b
}
QString MainWindow::gGetLastPresetColors()
{
  //Get last selected color preset
  return gridappearance.gGraphColorPreset;
}

void MainWindow::gChangeTextSpecs(QString btn, QString &newTxt)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  'Open text dialog to change a specific text spec
     'btn$ specifies the color to be changed. It may be in the form of a
     'handle, so we drop everything before the period, if there is one.
     'We return the new text in newTxt$, or "" if dialog is cancelled
     pos=instr(btn$,".")
     if pos>0 then btn$=Mid$(btn$,pos+1)
     btn$=Trim$(Upper$(btn$))
     select case btn$
         case "XFONT"    'x axis labels
             txt$=gXAxisFont$
         case "Y1FONT"   'y1 axis labels
             txt$=gY1AxisFont$
         case "Y2FONT"    'y2 axis labels
             txt$=gY2AxisFont$
         case "GRIDFONT"   'grid interior text labels
             txt$=gGridFont$
         case else
     end select

     newTxt$=""
     FontDialog txt$, newTxt$
     if newTxt$="" then exit sub     'Cancelled by user

     select case btn$
         case "XFONT"
             gXAxisFont$=newTxt$
         case "Y1FONT"
             gY1AxisFont$=newTxt$
         case "Y2FONT"
             gY2AxisFont$=newTxt$
         case "GRIDFONT"
             gGridFont$=newTxt$
         case else
     end select
 end sub*/
}

void MainWindow::gInitDynamicDraw()
{
  //Initialize to commence dynamic draw process
  //nEraseLead specifies the number of points by which the erasure leads the drawing.
  //doEraseN indicates whether trace N should be erased as it is redrawn.
  //A dynamic draw involves erasing prior lines while drawing new ones.
  //As each trace in a dynamic graph is drawn, it is necessary to "discard" occasionally to
  //clear accumulated graphics info. To avoid affecting user drawn graphics somehow, we start
  //with a flush which avoids discarding that prior graphics.
  //#gGraphHandle$, "flush"
  graph.gIsDynamic=1;    //Signal to accumulate drawing commands
  graph.gIsFirstDynamicScan=1;
  graph.gNumPoints=0;        //Clear existing points

  graph.gSetErasure(1,1,1);  //Default values. also calls gInitDraw   ver114-6e
  graph.gPrevPointNum=0; //ver114-6d
  //gTrace1$=gDraw1$
  //gTrace2$=gDraw2$
  //Start over accumulating draw commands. We use "down" as the empty command
  for (int i=1; i <= graph.gDynamicSteps+1; i++)
  {
    graph.gTrace1[i]=QPoint(0,0);
    graph.gTrace2[i]=QPoint(0,0);
  }
}

void MainWindow::smithFinished(QString)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub smithFinished btn$  //Window is closing
    if smithHndl$<>"" then close #smith : smithHndl$=""    //Note smithHndl$ is the graphic box, not the window
    if smithHasChartBmp then unloadBmp "smithChartBmp" : smithHasChartBmp=0
end sub
*/
}

void MainWindow::updateView()
{
  ui->graphicsView->setScene(graph.getScene());
  timerStart->stop();
  graph.gDrawGrid();
  graph.DrawSetupInfo();     //Draw info describing the sweep setup
  timerStart->stop();
}

void MainWindow::multiscanCloseAll()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub multiscanCloseAll    //Close all multiscan windows and clear all data. This quits multiscan
    for i=0 to multiscanMaxNum : call multiscanClose i : next i
    multiscanIsOpen=0
    multiscanInProgress=0
    refreshEachScan=multiscanSaveRefreshEachScan //Restore original setting
end sub
*/
}

void MainWindow::DetectChanges()
{
  //Take appropriate redraw/restart action in response to certain changes
  //This is a gosub routine so it can access [ChangeMode]. This needs to be called only if msaMode$ may have changed;
  //otherwise call DetectChanges directly.
  int doRestart=0;
  if (vars->prevMSAMode != vars->msaMode) { ChangeMode(); doRestart=1; }
  DetectChanges(doRestart);
}

void MainWindow::DetectChanges(int doRestart)
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

  graph.continueCode=0;  //Assume we can continue when we are done here
  int doCalcAndRedraw=0;
  int doTransform=0;
  int dataTypeChanged=0;   //ver115-1b

  //For certain switch settings, we don't bother to determine whether a change was made; we just
  //implement the current setting.
  SelectVideoFilter();  //Also makes auto wait calculations if necessary ver116-1b
  SelectLatchedSwitches(vars->lastSetBand);  //transmit/reflect, forward/reverse; leaves band as it is (handled by restart)  //ver116-1b ver116-4s
  if (vars->calCanUseAutoWait==0 && vars->useAutoWait) //ver116-1b
  {
      //If the path calibration had insufficient or bad data, auto wait was disabled when loading
      //the file. If preferences are loaded specifying to use it, we need to override that.
      vars->useAutoWait=0 ; vars->wate=100;
  }

      //Changes to graph data type will require recalculating grid labels with new format,
      //recalculating the transform and then
      //redrawing from scratch to get axis labeling correct.
  if (vars->prevY1DataType!=vars->Y1DataType || vars->prevY2DataType!=vars->Y2DataType) //ver115-4g
  {
      graph.gSetDoAxis ((vars->Y1DataType!=vars->constNoGraph), (vars->Y2DataType!=vars->constNoGraph));  //Tell graph module whether we have graphs ver115-2c
      ImplementDisplayModes();    //ver114-6e
      doCalcAndRedraw=1 ; dataTypeChanged=1 ;doTransform=1;  //ver115-1b
  }

      //Changes to these will require a complete Restart
  float currStartF, currEndF;
  graph.gGetXAxisRange(currStartF, currEndF);
  int currXIsLinear, currY1IsLinear, currY2IsLinear;
  graph.gGetIsLinear(currXIsLinear, currY1IsLinear, currY2IsLinear);
  if (vars->freqBand!=vars->prevFreqMode || currStartF!=vars->prevStartF
            || currEndF!=vars->prevEndF || vars->prevSteps!=vars->globalSteps || vars->baseFrequency!=vars->prevBaseF) doRestart=1;
  if (currXIsLinear!=vars->prevXIsLinear || currY1IsLinear!=vars->prevY1IsLinear
                                  || currY2IsLinear!=vars->prevY2IsLinear) doRestart=1;
  if (vars->prevGenTrk!=vars->gentrk || vars->prevSpurCheck!=vars->spurcheck) doRestart=1; //ver114-6k
  if (vars->prevTGOff!=vars->offset || vars->prevSGFreq!=vars->sgout) doRestart=1;  //ver115-1a
  int filtbank = 0;
  if (vars->prevPath!=vars->path) { SelectFilter(filtbank); doRestart=1;} //ver116-4j
  if (doRestart)
  {
      //If we are using fixed data as the reference source and any of the above changed,
      //we can no longer use it.
    if (graph.referenceLineType==1) { graph.referenceLineType=0 ; graph.referenceTrace=0 ; graph.gClearAllReferences(); }   //ver114-7f
  }

  if (vars->prevSweepDir!=graph.gGetSweepDir() || vars->prevAlternate!=vars->alternateSweep) doRestart=1;
  if (vars->prevPath!=vars->path) doRestart=1;    //ver115-1a
          //If in a mode where these are relevant, we need to require Restart,
      //but do not do anything to the existing data. Note that the calibration
      //will be invalidated elsewhere in these cases, so there is no meaningful way
      //to transform the data.
  if (vars->msaMode=="Reflection")    //ver115-1f
  {
      if (vars->prevS21JigAttach!=vnaCal.S21JigAttach) doRestart=1;
      if (vars->prevS21JigR0!=vnaCal.S21JigR0) doRestart=1;
      if (vars->prevS21JigShuntDelay!=vnaCal.S21JigShuntDelay) doRestart=1;
      if (vars->prevS11JigType!=vnaCal.S11JigType || vars->prevS11BridgeR0!=vnaCal.S11BridgeR0) doRestart=1;
  }

  if (doRestart)
  {
      //If the data changed, we should redraw it before we exit. We assume the frequency
      //points of the data are consistent with any changes to the graph module frequency points.
      //If the data did not change, we may not be able to redraw it with the new sweep parameters so
      //we don't try.
      graph.continueCode=3;
      if (vars->prevDataChanged==0) return;
      graph.gGenerateXValues(graph.gPointCount());  //Make sure x values correspond to sweep parameters
      //For RLC or fixed value reference, recalculate reference data
      if (graph.referenceLineType>1)
      {
        graph.CreateReferenceSource();
        graph.CreateReferenceTransform();
      }  //ver114-7f
      doCalcAndRedraw=1;
      doTransform=1;
  }

      //Changes to these will require calling gCalcGraphParams and then a full Redraw from existing Y values
  int rescaleReferences=0;
  int currStartY1, currEndY1;
  int currStartY2, currEndY2;
  int currHorDiv, currVertDiv;
  graph.gGetYAxisRange(1, currStartY1, currEndY1);
  graph.gGetYAxisRange(2, currStartY2, currEndY2);
  graph.gGetNumDivisions(currHorDiv, currVertDiv);
  if (vars->prevSwitchFR!=vars->switchFR) doCalcAndRedraw=1;    //Forward/Reverse switch affects axis labels ver116-1b
  if (currStartY1!=vars->prevStartY1 || currEndY1!=vars->prevEndY1) {doCalcAndRedraw=1 ; rescaleReferences=1;}
  if (currStartY2!=vars->prevStartY2 || currEndY2!=vars->prevEndY2) {doCalcAndRedraw=1 ; rescaleReferences=1;}
  if (currHorDiv!=vars->prevHorDiv || currVertDiv!=vars->prevVertDiv) doCalcAndRedraw=1;
  if (rescaleReferences) graph.CreateReferenceTransform();

  if (doCalcAndRedraw) //ver115-1b
  {
      if (dataTypeChanged || vars->prevSwitchFR!=vars->switchFR) UpdateGraphDataFormat(0);  //To get new data format ver116-1b
      graph.gCalcGraphParams();   //Calculate new scaling. May change min or max.
      float xMin, xMax;
      graph.gGetXAxisRange(xMin, xMax);
      if (vars->startfreq!=xMin || vars->endfreq!=xMax) {graph.SetStartStopFreq(xMin, xMax); graph.continueCode=3;}
  }
  int doRedraw=doCalcAndRedraw;

  int autoScaleTurnedOn;
  if ((vars->prevAutoScaleY1==0 || graph.autoScaleY1==1) || (vars->prevAutoScaleY2==0 || graph.autoScaleY2==1))
    autoScaleTurnedOn=1;
      else
    autoScaleTurnedOn=0;    //ver114-7a
  int autoScaleIsOn;
  if (graph.autoScaleY1==1 || graph.autoScaleY2==1) autoScaleIsOn=1; else autoScaleIsOn=0;  //ver114-7a

      //If data changes, recalc x pixel values and require a restart, though proceeding will destroy the new data
  if (vars->msaMode=="Reflection" && vars->prevS11GraphR0!=vnaCal.S11GraphR0) {graph.continueCode=3 ; doTransform=1;} //ver115-1f
  if (vars->prevDataChanged) {graph.continueCode=3 ; doTransform=1;}

  if (doTransform)
  {
      UpdateGraphDataFormat(0);  //Update data formatting; may also affect data transform
      graph.RecalcYValues();
      if (autoScaleIsOn)
      {
        graph.PerformAutoScale();  //autoscale with the new data   //ver114-7a
          autoScaleTurnedOn=0;     //Since we just did autoscale ver114-7e
      }
      doRedraw=1;  //Signal to do complete redraw from scratch
  }

           //If auto scale has been turned on, we signal to do the autoscaling at the end of the sweep or on redraw
  if (autoScaleTurnedOn) graph.refreshAutoScale=1;   //ver114-7a

  if (doRedraw) {RedrawGraph(0); return;}

   //Changing trace style requires recreating the traces and refreshing if it has just
  //been turned on (since we have no accumulated trace draw commands) or if it
  //was and remains on but the style changed (between histo/normal or erase/stick).
  int doRefreshTraces;
  if (vars->prevY1Disp!=vars->Y1DisplayMode && (vars->Y1DisplayMode!=0 || vars->prevY1Disp==0)) doRefreshTraces=1;    //ver114-7d
  if (vars->prevY2Disp!=vars->Y2DisplayMode && (vars->Y2DisplayMode!=0 || vars->prevY2Disp==0)) doRefreshTraces=1;    //ver114-7d
  if (doRefreshTraces) {graph.refreshTracesDirty=1 ; graph.refreshGridDirty=1 ; graph.RefreshGraph(0); return;}    //ver114-7d

      //Everything else we redraw everything except the traces from scratch; traces
      //will be drawn from gTrace1$() and gTrace2$()
      //Marker Options, Graph Appearance, trace width and trace color will be in this category
      //Also includes primaryAxisNum, which has no immediate effects except the order of marker info
  graph.refreshGridDirty=1;
      //We don't redraw if in stick mode, because the "stuck" traces will be erased.
  int doErasure;
  if (graph.haltsweep==1) doErasure=1; else doErasure=0;    //Do erasure gap if still sweeping
  if (vars->isStickMode==0) graph.RefreshGraph(doErasure);
}

QString MainWindow::smithGraphHndl()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  //Return handle to graphics
  // fix me
  //smithGraphHndl=vars->smithHndl;
  return "fix me 3";
}

void MainWindow::frontEndInterpolateToScan()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
 //Interpolate from frontEndCalData to frontEndCorrection based on current scan points ver115-9d

    //We copy data to the interpolation arrays, interpolate, and copy the results where we want them
    call intSetMaxNumPoints max(frontEndCalNumSteps, globalSteps)+1  //Be sure we have room

    call intClearSrc : call intClearDest
    for i=1 to frontEndCalNumPoints //copy cal table to intSrc
        call intAddSrcEntry frontEndCalData(i,0),frontEndCalData(i,1),0
    next i
    for i=1 to globalSteps+1
        call intAddDestFreq gGetPointXVal(i)   //Install frequencies in intDest
    next i

    favorFlat=1 : isAngle=0
        //1 means do mag; first 0 means don't do phase; final 0 means not phase correction ver116-1b
    call intCreateCubicCoeffTable 1,0,isAngle, favorFlat,0    //Get coefficients for cubic interp of front end cal //ver116-1b

    //0 means data is not polar , 1 means do cubic interp, 1 means do mag only
    call intSrcToDest 0, 1, 1

    for i=0 to globalSteps  //put the data where we want it
        call intGetDest i+1,f, m, p
        frontEndCorrection(i)=m //mag from interp results
    next i
    */
}

void MainWindow::CleanupAfterSweep()
{
  //Do cleanup after a sweep to be sure flags are set/reset properly
  //Called by [FinishSweeping]. Can also be called by other routines to immediately
  //terminate a sweep when they will be Restarting so they don't care about finishing the plotting.
  DisplayButtonsForHalted();    //ver114-4f replaced call to [UpdateBoxes]
  if (vars->thisstep == vars->sweepEndStep) haltWasAtEnd=1; else haltWasAtEnd=0;  //ver114-5c
  graph.haltAtEnd=0;      //In case we got here from auto halt at end of sweep
  vars->calInProgress=0;  //ver114-5h
  graph.haltsweep = 0; //this says the sweep has been halted, so don't print the first command of the next sweep step //ver111-20

}

void MainWindow::on_actionShow_Variables_triggered()
{
  Showvar();
}

QString MainWindow::gRestoreTraceContext(QString &s, int &startPos, int isValidation)
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
      if (isValidation==0) graph.gDoHist=v1;
    }
    else if (tag == "ACTIVEGRAPHS")
    {
      isErr=util.uExtractNumericItems(2, tLine,nonTextDelims, v1, v2, v3);
      int graphY1=v1; int graphY2=v2;
      if (isValidation==0) graph.gGraphY1=graphY1; graph.gGraphY2=graphY2;  //ver115-1a
    }
    else if (tag == "TRACEWIDTHS")
    {
      isErr=util.uExtractNumericItems(2, tLine,nonTextDelims, v1, v2, v3);
      if (isValidation==0) graph.gTrace1Width=v1; graph.gTrace2Width=v2;
    }
    else if (tag == "TRACECOLORS")
    {
      QString t1=util.uExtractTextItem(tLine,sep);
      QString t2=util.uExtractTextItem(tLine,sep);
      if (t1=="" || t2=="") isErr=1;
      //It was originally intended that trace colors and their grid labels be independent in the graph module,
      //but that is not how we use it, and the independence makes it awkward if the chosen Appearance has
      //one trace/label color and the preference file has a different trace color. So we tie them together.
      if (isValidation==0)
      {
        gridappearance.gTrace1Color=t1;
        gridappearance.gTrace2Color=t2;
        gridappearance.gY1TextColor=t1;
        gridappearance.gY2TextColor=t2; } //ver116-4L
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

QString MainWindow::gGridContext()
{
  //Return string with context info on grid size and appearance
  //successive lines are separated by chr(13)
  QString newLine="\r";
  QString aSpace=" ";
  QString sep=";;";   //delimits text items on one line

  QString s1= "WindowSize="+ QString::number(graph.gWindowWidth)+ aSpace+ QString::number(graph.gWindowHeight);
  s1= s1+ newLine+ "Margins="+ QString::number(graph.gMarginLeft)+ aSpace+ QString::number(graph.gMarginRight)+
                        aSpace+ QString::number(graph.gMarginTop)+ aSpace+ QString::number(graph.gMarginBot);
  s1= s1+ newLine+ "GridDivisions="+ QString::number(graph.gHorDiv)+ aSpace+ QString::number(graph.gVertDiv);
  s1= s1+ newLine+ "PrimaryAxis="+ QString::number(graph.gPrimaryAxis);
  s1= s1+ newLine+ "AxisLabels="+ graph.gXAxisLabel+ sep+ graph.gY1AxisLabel+ sep+ graph.gY2AxisLabel;

  QString s2= "AxisTextColors="+ gridappearance.gXTextColor+ sep+ gridappearance.gY1TextColor+ sep+ gridappearance.gY2TextColor;
  s2= s2+ newLine+ "AxisFonts="+ gridappearance.gXAxisFont+ sep+ gridappearance.gY1AxisFont+ sep+ gridappearance.gY1AxisFont;
  s2= s2+ newLine+ "BackColor="+ gridappearance.gBackColor;
  s2= s2+ newLine+ "GridTextColor="+ gridappearance.gGridTextColor;
  s2= s2+ newLine+ "GridFont="+ gridappearance.gGridFont;
  s2= s2+ newLine+ "GridLineColor="+ gridappearance.gGridLineColor;
  s2= s2+ newLine+ "GridBoundsColor="+ gridappearance.gGridBoundsColor;
  s2= s2+ newLine+ "GridStyles="+ graph.gXGridStyle+ sep+ graph.gY1GridStyle+ sep+ graph.gY2GridStyle;
  s2= s2+ newLine+ "AxisFormats="+ graph.gXAxisForm+ sep+ graph.gY1AxisForm+ sep+ graph.gY2AxisForm;

  for (int i=1; i<= 5; i++)    //custom colors. One line per color set. Start with set number, then list of colors ver115-2a
  {
    QString s3="CustomColors="+QString::number(i)+",";

    QString grid, bounds, back, gridText, XText, Y1Text, Y2Text, trace1, trace2, trace1A, trace2A, trace1B, trace2B;
    graph.gGetCustomPresetColors( i, grid,bounds,back,gridText,XText,Y1Text,Y2Text, trace1, trace2,
            trace1A, trace2A, trace1B, trace2B);

    s3=s3+grid+sep+bounds+sep+back+sep+ gridText+sep+XText+sep+Y1Text+sep+
                Y2Text+sep+trace1+sep+trace2+sep+trace1A+sep+trace2A+sep+trace1B+sep+trace2B+sep;

    s2=s2+newLine+s3; //Append this line to s1
  }
  return s1+ newLine+ s2+ newLine+ "LastPresets=" + gridappearance.gGraphColorPreset+ sep+ gridappearance.gGraphTextPreset;
}

QString MainWindow::gTraceContext()
{
  //Return string with context info on trace graphing
  //successive lines are separated by chr$(13)
  //We don't save/restore gDoErase1, gDoErase2, gEraseLead because user will reset them in gInitDynamicDraw
  QString newLine="\r";
  QString aSpace=" ";
  QString sep=";;";   //delimits text items on one line
  QString s1= newLine+ "ActiveGraphs="+ QString::number(graph.gGraphY1)+ aSpace+ QString::number(graph.gGraphY2);
  s1="DoHist="+ QString::number(graph.gDoHist);
  s1= s1+ newLine+ "TraceWidths="+ QString::number(graph.gTrace1Width)+ aSpace+ QString::number(graph.gTrace2Width); //ver114-4d eliminated typo
  s1= s1+ newLine+ "TraceColors="+ gridappearance.gTrace1Color+ sep+ gridappearance.gTrace2Color;
  return s1;
}

void MainWindow::gSetMarkerNum(int markNum, int pointNum, QString ID, QString trace, QString style)
{
  //set marker by position in the list
  //Enter marker data and update gNumMarkers to have the max entryNum of any entered marker
  if (markNum<1 || markNum>20) { QMessageBox::about(0,"", "Invalid marker number"); return; } //for debugging
  graph.gMarkerPoints[markNum][0]=pointNum;
  graph.gMarkerPoints[markNum][1]=graph.gGetPointXVal(pointNum); //Copy current x value (freq)
  graph.gMarkers[markNum][0]=ID;
  graph.gMarkers[markNum][1]=trace; graph.gMarkers[markNum][2]=style;
  if (markNum>graph.gNumMarkers) graph.gNumMarkers=markNum;
}

QString MainWindow::gRestoreGridContext(QString &s, int &startPos, int isValidation)
{
  //Restore info on grid size and appearance from string s$
  //If isValidation=1, then we just run through the data for error checking, without changing anything.
  //Return error message if error; otherwise "". We ignore data prior to startPos.
  //startPos is updated to one past the last line we process (normally EndContext or end of string)
  //successive lines are separated by chr$(13)
  //CalcGraphParams MUST BE CALLED at some point after this routine to adjust to the margins
  //and hor/vert divisions. This can be done by a subsequent call of gRestoreSweepContext or
  //by a direct call.
  QString newLine="\r";
  QString aSpace=" ";
  QString sep=";;"; //Used to separate text items
  QString nonTextDelims=" ,\t";    //space, comma and tab are delimiters
  //Get next line and increment startPos to start of the following line
  QString tLine=util.uGetLine(s, startPos);
  float v1, v2, v3;
  QString t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13;
  while (tLine!="")
  {
    QString origLine=tLine;  //ver115-1b
    if (tLine.left(10).toUpper()=="ENDCONTEXT") break;
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
        graph.gWindowWidth=v1;
        graph.gWindowHeight=v2;
      }
    }
    else if (tag == "MARGINS")
    {
      isErr=util.uExtractNumericItems(2, tLine,nonTextDelims, v1, v2, v3);
      if (isValidation==0)
      {
        graph.gMarginLeft=v1;
        graph.gMarginRight=v2;
      }
      isErr=util.uExtractNumericItems(2, tLine,nonTextDelims, v1, v2, v3);
      if (isValidation==0)
      {
        graph.gMarginTop=v1;
        graph.gMarginBot=v2;
      }
    }
    else if (tag == "PRIMARYAXIS")  //ver115-3b
    {
      isErr=util.uExtractNumericItems(1, tLine,nonTextDelims, v1, v2, v3);
      if (v1!=1 && v1!=2) isErr=1;
      if (isValidation==0 && isErr==0)
      {
        graph.gSetPrimaryAxis(v1);
      }
    }
    else if (tag == "AXISTEXTCOLORS")
    {
      t1=util.uExtractTextItem(tLine,sep);
      t2=util.uExtractTextItem(tLine,sep);
      t3=util.uExtractTextItem(tLine,sep);
      if (t1=="" || t2=="" || t3=="") isErr=1;
      if (isValidation==0)
      {
        gridappearance.gXTextColor=t1;
        gridappearance.gY1TextColor=t2;
        gridappearance.gY2TextColor=t3;
      }
    }
    else if (tag == "AXISFONTS")
    {
      t1=util.uExtractTextItem(tLine,sep);
      t2=util.uExtractTextItem(tLine,sep);
      t3=util.uExtractTextItem(tLine,sep);
      if (t1=="" || t2=="" || t3=="") isErr=1;
      if (isValidation==0)
      {
        gridappearance.gXAxisFont=t1;
        gridappearance.gY1AxisFont=t2;
        gridappearance.gY2AxisFont=t3;
      }
    }
    else if (tag == "AXISLABELS")
    {
      t1=util.uExtractTextItem(tLine,sep);
      t2=util.uExtractTextItem(tLine,sep);
      t3=util.uExtractTextItem(tLine,sep);
      if (isValidation==0)
      {
        graph.gXAxisLabel=t1;
        graph.gY1AxisLabel=t2;
        graph.gY2AxisLabel=t3;
      }
    }
    else if (tag == "BACKCOLOR")
    {
      t1=util.uExtractTextItem(tLine,sep);
      if (t1=="") isErr=1;
      if (isValidation==0)
      {
        gridappearance.gBackColor=t1;
      }
    }
    else if (tag == "GRIDTEXTCOLOR")
    {
      t1=util.uExtractTextItem(tLine,sep);
      if (t1=="") isErr=1;
      if (isValidation==0)
      {
        gridappearance.gGridTextColor=t1;
      }
    }
    else if (tag == "GRIDFONT")
    {
      t1=util.uExtractTextItem(tLine,sep);
      if (t1=="") isErr=1;
      if (isValidation==0)
      {
        gridappearance.gGridFont=t1;
      }
    }
    else if (tag == "GRIDLINECOLOR")
    {
      t1=util.uExtractTextItem(tLine,sep);
      if (t1=="") isErr=1;
      if (isValidation==0)
      {
        gridappearance.gGridLineColor=t1;
      }
    }
    else if (tag == "GRIDBOUNDSCOLOR")
    {
      t1=util.uExtractTextItem(tLine,sep);
      if (t1=="") isErr=1;
      if (isValidation==0)
      {
        gridappearance.gGridBoundsColor=t1;
      }
    }
    else if (tag == "GRIDDIVISIONS")
    {
      isErr=util.uExtractNumericItems(2, tLine,nonTextDelims, v1, v2, v3);
      if (isValidation==0)
      {
        graph.gHorDiv=v1;
        graph.gVertDiv=v2;
      }
    }
    else if (tag == "GRIDSTYLES")
    {
      t1=util.uExtractTextItem(tLine,sep);
      t2=util.uExtractTextItem(tLine,sep);
      t3=util.uExtractTextItem(tLine,sep);
      if (t1=="" || t2=="" || t3=="") isErr=1;
      if (isValidation==0)
      {
        graph.gXGridStyle=t1;
        graph.gY1GridStyle=t2;
        graph.gY2GridStyle=t3;
      }
    }
    else if (tag == "AXISFORMATS")
    {
      t1=util.uExtractTextItem(tLine,sep);
      t2=util.uExtractTextItem(tLine,sep);
      t3=util.uExtractTextItem(tLine,sep);
      if (t1=="" || t2=="" || t3=="") isErr=1;
      if (isValidation==0)
      {
        graph.gXAxisForm=t1;
        graph.gY1AxisForm=t2;
        graph.gY2AxisForm=t3;
      }
    }
    else if (tag == "CUSTOMCOLORS") //ver115-2a
    {
      isErr=util.uExtractNumericItems(1, tLine,nonTextDelims, v1, v2, v3);    //Get preset number
      if (v1<1 || v1>5) isErr=1;
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
      //ver116-4b added colors for traces 1A, 1B, 2A and 2B. If not present, duplicate the 1 and 2 colors
      if (tLine=="")
      {
        t10=t8 ; t11=t9 ; t12=t8 ; t13=t9;
      }
      else
      {
        t10=util.uExtractTextItem(tLine,sep);
        t11=util.uExtractTextItem(tLine,sep);
        t12=util.uExtractTextItem(tLine,sep);
        t13=util.uExtractTextItem(tLine,sep);
      }
      if (isErr==0 && isValidation==0)
        graph.gSetCustomPresetColors(v1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13); //ver116-4b
    }
    else if (tag == "LASTPRESETS")
    {
      t1=util.uExtractTextItem(tLine,sep) ;
      t2=util.uExtractTextItem(tLine,sep);
      if (t1=="" || t2=="") isErr=1;
      if (isValidation==0)
      {
        graph.gUsePresetColors(t1);
        graph.gUsePresetText(t2);
        gridappearance.SetCycleColors(); //ver116-4s
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
  graph.gUpdateGraphObject(graph.gWindowWidth, graph.gWindowHeight, graph.gMarginLeft
                           , graph.gMarginRight, graph.gMarginTop, graph.gMarginBot);
  ui->graphicsView->fitInView(graph.getScene()->sceneRect());
  return "";
}

void MainWindow::CloseSpecial(int returnflag)
{
  syncsweep = 0;
  setpdm = 0; //makes sure the PDM returns to automatic operation ver112-2a
  convdatapwr = 0;
  vnalintest = 0;
  test = 0;
  if (cftest == 1) CloseCavityFilterTest(); //cav ver116-4c
  //close #special:
  special = 0;     //close out Special Tests window
  RequireRestart();
  if (returnflag == 1)
    return;
}

void MainWindow::on_actionReference_Lines_triggered()
{
  referenceDialog ref(this);
  referDialog settings;
  settings.referenceColor1 = graph.referenceColor1;
  settings.referenceWidth1 = graph.referenceWidth1;

  settings.referenceColor2 = graph.referenceColor2;
  settings.referenceWidth2 = graph.referenceWidth2;

  settings.referenceColorSmith = graph.referenceColorSmith;
  settings.referenceWidthSmith = graph.referenceWidthSmith;

  settings.referenceLineType = graph.referenceLineType;
  settings.referenceTrace = graph.referenceTrace;
  settings.msaMode = vars->msaMode;
  settings.referenceDoMath = graph.referenceDoMath;
  settings.referenceOpA = graph.referenceOpA;
  settings.referenceOpB = graph.referenceOpB;
  settings.referenceLineSpec = graph.referenceLineSpec;
  QString dum1, dum2, aFormat;
  int dum3;
  DetermineGraphDataFormat(vars->Y1DataType, dum1, dum2, dum3, aFormat);
  settings.Y1DataFormat = aFormat;
  DetermineGraphDataFormat(vars->Y2DataType, dum1, dum2, dum3, aFormat);
  settings.Y2DataFormat = aFormat;
  settings.Y1DataType = vars->Y1DataType;
  settings.Y2DataType = vars->Y2DataType;

  ref.ReferenceDialog(&settings);

  if (!ref.cancelled)
  {
    graph.referenceColor1 = settings.referenceColor1;
    graph.referenceColor2 = settings.referenceColor2;
    graph.referenceWidth1 = settings.referenceWidth1;
    graph.referenceWidth2 = settings.referenceWidth2;
    graph.referenceColorSmith = settings.referenceColorSmith;
    graph.referenceWidthSmith = settings.referenceWidthSmith;

    graph.referenceLineType = settings.referenceLineType;
    graph.referenceTrace = settings.referenceTrace;
    vars->msaMode = settings.msaMode;

    graph.referenceDoMath = settings.referenceDoMath;
    graph.referenceOpA = settings.referenceOpA;
    graph.referenceOpB = settings.referenceOpB;
    graph.referenceLineSpec = settings.referenceLineSpec;

    if (ref.clearReferences)
    {
      graph.gClearAllReferences();
    }
    if (ref.createReferences)
    {
      graph.CreateReferenceSource();
    }
    graph.refreshGridDirty=1;
    graph.RefreshGraph(0);
  }
}

void MainWindow::Command2325R()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed:rcounter,preselector,control,Jcontrol,port,LEPLL,contclear ; commands LMX2325 rcounter and registers
    if rcounter <3 then beep:errora$ = "2325 Rcounter is < 3":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0 = 1           'address bit, 0 sets the N Buffer, 1 is for R Buffer
    rc1 = int(rcounter/2):N1 = rcounter - 2*rc1 'binary conversion from decimal
    rc2 = int(rc1/2):N2 = rc1 - 2*rc2
    rc3 = int(rc2/2):N3 = rc2 - 2*rc3
    rc4 = int(rc3/2):N4 = rc3 - 2*rc4
    rc5 = int(rc4/2):N5 = rc4 - 2*rc5
    rc6 = int(rc5/2):N6 = rc5 - 2*rc6
    rc7 = int(rc6/2):N7 = rc6 - 2*rc7
    rc8 = int(rc7/2):N8 = rc7 - 2*rc8
    rc9 = int(rc8/2):N9 = rc8 - 2*rc9
    rc10 = int(rc9/2):N10 = rc9 - 2*rc10
    rc11 = int(rc10/2):N11 = rc10 - 2*rc11
    rc12 = int(rc11/2):N12 = rc11 - 2*rc12
    rc13 = int(rc12/2):N13 = rc12 - 2*rc13
    rc14 = int(rc13/2):N14 = rc13 - 2*rc14
    N15 = 1: if preselector = 64 then N15 = 0   'sets preselector divide ratio, 1=32, 0=64
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
    return
    */
}

void MainWindow::Command2326R()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
/*
//needed:rcounter,phasepolarity,control,Jcontrol,port,LEPLL,contclear ; commands LMX2326 rcounter and registers
    //[Create2326InitBuffer]//need phasepolarity
    //ver116-4o deleted "if" block, per Lrev1
    N20=0;     //Test, use 0
    N19=0;     //1=Power Down Mode, use 0
    N18=0;     //Test, use 0
    N17=0;     //Test, use 0
    N16=0;     //Test, use 0
    N15=0;     //Fastlock Time out value, use 0
    N14=0;     //Fastlock Time out value, use 0
    N13=0;     //Fastlock Time out value, use 0
    N12=0;     //Fastlock Time out value, use 0
    N11=0;     //1=Time out enable, use 0
    N10=0;     //Fastlock control, use 0
    N9=0;    //1=Fastlock enable, use 0
    N8=0;    //1=Tristate the phase det output, use 0
    N7 = phasepolarity;     //Phase det polarity, 1=pos  0=neg
    N6=0;        //FoLD control(pin14 output), 0= tristate, 1= R Divider out
    N5=0;        //2= N Divider out, 3= Serial Data Output, 4= Digital Lock Detect
    N4=0;        //5= Open drain lock detect, 6= High output, 7= Low output
    N3=0;        //1= Power Down, use 0
    N2=0;        //1= Counter Reset Enable, allows reset of R,N counters,use 0
    N1=1;        //F1 address bit 1, must be 1
    N0=1;        //F1 address bit 0, must be 1
    if (activeConfig.cb == 3)
    {
      Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 //ver116-4o per Lrev1
    }
    if (activeConfig.cb == 3)
    {
      Int64N.msLong.struct = 0; ////ver116-4o per Lrev1
    }
  //[Command2326InitBuffer]//need Jcontrol,LEPLL,contclear
    CommandPLL();//needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
  //[Create2326Rbuffer]//need rcounter
    if (rcounter <3) then beep:errora$="2326 R counter <3":return //with errora$ ver111-37c
    if (rcounter >16383) then beep:errora$="2326 R counter >16383":return //with errora$ ver111-37c
    //ver116-4o deleted "if" block, per Lrev1
    N0 = 0                   //R address bit 0, must be 0
    N1 = 0                   //R address vit 1, must be 0
    ra0 = int(rcounter/2):N2 = rcounter- 2*ra0    //LSB
    ra1 = int(ra0/2):N3 = ra0- 2*ra1
    ra2 = int(ra1/2):N4 = ra1- 2*ra2
    ra3 = int(ra2/2):N5 = ra2- 2*ra3
    ra4 = int(ra3/2):N6 = ra3- 2*ra4
    ra5 = int(ra4/2):N7 = ra4- 2*ra5
    ra6 = int(ra5/2):N8 = ra5- 2*ra6
    ra7 = int(ra6/2):N9 = ra6- 2*ra7
    ra8 = int(ra7/2):N10 = ra7- 2*ra8
    ra9 = int(ra8/2):N11 = ra8- 2*ra9
    ra10 = int(ra9/2):N12 = ra9- 2*ra10
    ra11 = int(ra10/2):N13 = ra10- 2*ra11
    ra12 = int(ra11/2):N14 = ra11- 2*ra12
    ra13 = int(ra12/2):N15 = ra12- 2*ra13  //MSB
    N16 = 0     //Test Bit
    N17 = 0     //Test Bit
    N18 = 0     //Test Bit
    N19 = 0     //Test Bit
    N20 = 0     //Lock Detector Mode, 0=3 refcycles, 1=5 cycles
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 //ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 //ver116-4o per Lrev1
  //[Command2326Rbuffer]//need Jcontrol,LEPLL,contclear
    gosub [CommandPLL]//needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
    return
    */
}

void MainWindow::Command2350R()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed: rcounter,phasepolarity,control,Jcontrol,port,LEPLL,contclear,fractional ; commands LMX2350 rcounter
    '[CreateIFRbuffer2350]'needed:nothing,since IF section is turned off
    'ver116-4o deleted "if" block, per Lrev1
    N23=0     'osc. 0=separate
    N22=1     'Modulo F, 1=16 0=15
    N21=1     'ifr21-ifr19 is FO/LD, 3 Bits (0-7), MSB, 0=IF/RF alogLockDet(open drain)
    N20=1     '1=IF digLockDet, 2=RF digLockDet, 3=IF/RF digLockDet
    N19=1     '4=IF Rcntr, 5=IF Ncntr, 6=RF Rcntr, 7=RF Ncntr, LSB
    N18=0     'IF charge pump, 0=100ua  1=800ua
    N17=1     'IF polarity 1=positive phase action
    N16=0     'IFR counter IF section 15 Bits, MSB 14
    N15=0     'IFRcounter Bit 13
    N14=0     'IFRcounter Bit 12
    N13=0     'IFRcounter Bit 11
    N12=1     'IFRcounter Bit 10
    N11=1     'IFRcounter Bit 9
    N10=1     'IFRcounter Bit 8
    N9=1      'IFRcounter Bit 7
    N8=0      'IFRcounter Bit 6
    N7=1      'IFRcounter Bit 5
    N6=1      'IFRcounter Bit 4
    N5=0      'IFRcounter Bit 3
    N4=0      'IFRcounter Bit 2
    N3=0      'IFRcounter Bit 1
    N2=0      'IFR counter, IF section 15 Bits, LSB 0
    N1=0      '2350 IF_R register, 2 bits, must be 0
    N0=0      '2350 IF_R register, 2 bits, must be 0
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
  '[CommandIFRbuffer2350]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
  '[CreateIFNbuffer2350]'needed:nothing,since IF section is turned off(N22=1)
    'ver116-4o deleted "if" block, per Lrev1
    N23=0     'IF counter reset, 0=normal operation
    N22=1     'Power down mode for IF section, 1=powered down, 0=powered up
    N21=0     'PWN Mode,  0=async  1=syncro
    N20=0     'Fastlock, 0=CMOS outputs enabled 1= fastlock mode
    N19=0     'test bit, leave at 0
    N18=1     'OUT 0,  1
    N17=0     'OUT 1,  0
    N16=0     'IF N Bcounter 12 Bits MSB bit 11
    N15=0     'IF N Bcounter, bit 10, '512 = 0010 0000 0000
    N14=1     'IF N Bcounter, bit 9
    N13=0     'IF N Bcounter, bit 8
    N12=0     'IF N Bcounter, bit 7
    N11=0     'IF N Bcounter, bit 6
    N10=0     'IF N Bcounter, bit 5
    N9=0      'IF N Bcounter, bit 4
    N8=0      'IF N Bcounter, bit 3
    N7=0      'IF N Bcounter, bit 2
    N6=0      'IF N Bcounter, bit 1
    N5=0      'IF N Bcounter, 12 Bits, LSB bit 0
    N4=0      'bit 2, IF N Acounter 3 Bits MSB
    N3=0      'bit 1, 0 = 000 thru 7 = 111
    N2=0      'bit 0, IF N Acounter 3 Bits LSB
    N1=0      '2350 IF_N register, 2 bits, must be 0
    N0=1      '2350 IF_N register, 2 bits, must be 1
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
  '[CommandIFNbuffer2350]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
  '[CreateRFRbuffer2350]needed:rcounter,phasepolarity,fractional
    if rcounter < 3 then beep:errora$="2350 Rcounter <3":return 'with errora$ ver111-37c
    if rcounter > 32767 then beep:errora$="2350 Rcounter >32767":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0=0      '2350 RF_R register, 2 bits, must be 0
    N1=1      '2350 RF_R register, 2 bits, must be 1
    rfra2 = int(rcounter/2):N2 = rcounter- 2*rfra2
    rfra3 = int(rfra2/2):N3 = rfra2- 2*rfra3
    rfra4 = int(rfra3/2):N4 = rfra3- 2*rfra4
    rfra5 = int(rfra4/2):N5 = rfra4- 2*rfra5
    rfra6 = int(rfra5/2):N6 = rfra5- 2*rfra6
    rfra7 = int(rfra6/2):N7 = rfra6- 2*rfra7
    rfra8 = int(rfra7/2):N8 = rfra7- 2*rfra8
    rfra9 = int(rfra8/2):N9 = rfra8- 2*rfra9
    rfra10 = int(rfra9/2):N10 = rfra9- 2*rfra10
    rfra11 = int(rfra10/2):N11 = rfra10- 2*rfra11
    rfra12 = int(rfra11/2):N12 = rfra11- 2*rfra12
    rfra13 = int(rfra12/2):N13 = rfra12- 2*rfra13
    rfra14 = int(rfra13/2):N14 = rfra13- 2*rfra14
    rfra15 = int(rfra14/2):N15 = rfra14- 2*rfra15
    rfra16 = int(rfra15/2):N16 = rfra15- 2*rfra16
    N17 = phasepolarity     'RF phase polarity,  1=positive action, 0=inverted action
    N18=1     'LSB of RF charge pump sel, 4 Bits, 16 levels, 100ua/level
    N19=1     'total current = (100ua * bit value)+100ua
    N20=1     '100ua to 1600ua: ie, 800ua = 0111, 1600ua = 1111
    N21=1     'MSB of RF charge pump sel, 4 Bits 100ua/bit
    N22=0     'V2 enable voltage doubler =1   0=norm Vcc
    N23 = fractional   'DLL mode, delay line cal, 0=slow  1=fast,fractional mode
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 ''ver116-4o per Lrev1
    '[CommandRFRbuffer2350]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
    return
    */
}

void MainWindow::Command2353R()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed: rcounter,phasepolarity,control,Jcontrol,port,LEPLL,contclear,fractional ; commands LMX2353 rcounter
  '[Create2353F1Buffer]'globals reqd, none
    'ver116-4o deleted "if" block, per Lrev1
    N23=0
    N22=1     'divider, 1=16 0=15
    N21=0     'FO/LD output selection, 3 Bits 0-7 MSB
    N20=0     '0=alog lock det, 2=dig lock det
    N19=0     '6=Ndivider output, 7=Rdivider output
    N18=0:N17=0:N16=0:N15=0:N14=0
    N13=0:N12=0:N11=0:N10=0:N9=0
    N8=0:N7=0:N6=0:N5=0:N4=0
    N3=0:N2=0
    N1=0        'F1 address bit 1
    N0=0        'F1 address bit 0
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev11
  '[Command2353F1Buffer]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
  '[Create2353F2Buffer]'globals reqd: none
    'ver116-4o deleted "if" block, per Lrev1
    N23=0:N22=0
    N21=0     'Power Down Mode,  0=async  1=syncro
    N20=0     'Fastlock, 0=CMOS outputs enabled 1= fastlock mode
    N19=0     'test bit, leave at 0
    N18=0     'OUT 1,  0
    N17=0     'OUT 0,  0
    N16=0:N15=0:N14=0:N13=0
    N12=0:N11=0:N10=0:N9=0
    N8=0:N7=0:N6=0:N5=0
    N4=0:N3=0:N2=0
    N1=0        'F2 address bit 1
    N0=1        'F2 address bit 0
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
  '[Command2353F2Buffer]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
  '[Create2353Rbuffer]'needed:rcounter,phasepolarity,fractional
    if rcounter <3 then beep:errora$ = "2353 Rcounter is < 3":return 'with errora$ ver111-37c
    if rcounter >32767 then beep:errora$ = "2353 Rcounter is > 32767":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0 = 0                   'R address bit 0
    N1 = 1                   'R address bit 1
    ra0 = int(rcounter/2):N2 = rcounter- 2*ra0    'LSB R buffer
    ra1 = int(ra0/2):N3 = ra0- 2*ra1:ra2 = int(ra1/2):N4 = ra1- 2*ra2
    ra3 = int(ra2/2):N5 = ra2- 2*ra3:ra4 = int(ra3/2):N6 = ra3- 2*ra4
    ra5 = int(ra4/2):N7 = ra4- 2*ra5:ra6 = int(ra5/2):N8 = ra5- 2*ra6
    ra7 = int(ra6/2):N9 = ra6- 2*ra7:ra8 = int(ra7/2):N10 = ra7- 2*ra8
    ra9 = int(ra8/2):N11 = ra8- 2*ra9:ra10 = int(ra9/2):N12 = ra9- 2*ra10
    ra11 = int(ra10/2):N13 = ra10- 2*ra11:ra12 = int(ra11/2):N14 = ra11- 2*ra12
    ra13 = int(ra12/2):N15 = ra12- 2*ra13:ra14 = int(ra13/2):N16 = ra13- 2*ra14    'MSB R buffer
    N17 = phasepolarity     'phase detector polarity 1=normal,0=reverse for opamp
    N18 = 1   'LSB of Charge pump control, 100ua x1 +100ua
    N19 = 1          'Charge pump control, 100ua x2 +100ua
    N20 = 1          'Charge pump control, 100ua x4 +100ua
    N21 = 1   'MSB of Charge pump control, 100ua x8 +100ua
    N22 = 0   'Charge Pump Voltage Doubler Enabled when 1
    N23 = fractional 'Delay Line Loop Cal mode, set to 1 for fractional N
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
  '[Cmd2353Rbuffer]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
    return
    */
}

void MainWindow::Command4112R()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed: rcounter,preselector,phasepolarity,control,Jcontrol,port,LEPLL,contclear ; commands AD4112 rcounter
  '[Create4112InitBuffer]'needed:preselector,phasepolarity
    'ver116-4o deleted "if" block, per Lrev1
    N23=1     'N23,22 prescaler: 0=8, 1=16, 2=32, 3=64
    N22=0     'preselector defaulted to 32
    if preselector =8 then N23=0:N22=0
    if preselector =16 then N23=0:N22=1
    if preselector =64 then N23=1:N22=1
    N21=0     'Power Down Mode, 0=async, 1=sync  use 0
    N20=0     'N20,19,18 Phase Current for Set 2            '12-3-10
    N19=0     'current= min current + min current*bit value '12-3-10
    N18=1     'use bit value of 1 and 4.7 Kohm for 1.25 ma  '12-3-10
    N17=0     'N17,16,15 Phase Current for Set 1            '12-3-10
    N16=0     'current= min current + min current*bit value '12-3-10
    N15=1     'use bit value of 1 and 4.7 Kohm for 1.25 ma  '12-3-10
    N14=0     'N15,14,13,12 Fastlock Timer cycles
    N13=0     '4 Bits, Cycles= 3 cycles + 4*bit value
    N12=0     'Fastlock Time out value, use 0
    N11=0     'use 4 bit value = 0
    N10=0     '0=Fastlock Mode 1 (command), 1=Mode 2 (automatic)
    N9=0     '1=Fastlock enabled, 0 =Fastlock Disabled
    N8=0      '1=Tristate the phase det output, use 0
    N7 = phasepolarity     'Phase det polarity, 1=pos  0=neg
    N6=0      'FoLD control(pin14 output), 0= tristate, 1= Digital Lock Detect
    N5=0      '2= N Divider out, 3= High output, 4= R Divider output
    N4=0      '5= Open drain lock detect, 6= Serial Data output, 7= Low output
    N3=0      'PD1, Power Down, 0=normal operation, 1=select power down mode
    N2=0      '1= Counter Reset Enable, allows reset of R,N counters,use 0
    N1=1      'F1 address bit 1, must be 1
    N0=1      'F1 address bit 0, must be 1
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
  '[Command4112InitBuffer]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
  '[Create4112Rbuffer]'needs:rcounter
    if rcounter >16383 then beep:errora$="4112 R counter >16383":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0 = 0                   'R address bit 0, must be 0
    N1 = 0                   'R address vit 1, must be 0
    ra0 = int(rcounter/2):N2 = rcounter- 2*ra0    'LSB R0
    ra1 = int(ra0/2):N3 = ra0- 2*ra1
    ra2 = int(ra1/2):N4 = ra1- 2*ra2
    ra3 = int(ra2/2):N5 = ra2- 2*ra3
    ra4 = int(ra3/2):N6 = ra3- 2*ra4
    ra5 = int(ra4/2):N7 = ra4- 2*ra5
    ra6 = int(ra5/2):N8 = ra5- 2*ra6
    ra7 = int(ra6/2):N9 = ra6- 2*ra7
    ra8 = int(ra7/2):N10 = ra7- 2*ra8
    ra9 = int(ra8/2):N11 = ra8- 2*ra9
    ra10 = int(ra9/2):N12 = ra9- 2*ra10
    ra11 = int(ra10/2):N13 = ra10- 2*ra11
    ra12 = int(ra11/2):N14 = ra11- 2*ra12
    ra13 = int(ra12/2):N15 = ra12- 2*ra13  'MSB
    N16 = 0     'N17,16  Antibacklash width
    N17 = 0     '0=3ns, 1=1.5ns, 2=6ns, 3=3ns
    N18 = 0     'Test Bit, use 0
    N19 = 0     'Test Bit, use 0
    N20 = 0     'Lock Detector Mode, 0=3 refcycles, 1=5 cycles
    N21 = 0     'resyncronization enable 0=normal, 1=resync prescaler
    N22 = 1     '0=resync with nondelayed rf input, 1=resync with delayed rf
    N23 = 0   'reserved, use 0
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
  '[Command4112Rbuffer]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
    return
    */
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
  //ui->graphicsView->fitInView(0, 0, graph.getScene()->width(), graph.getScene()->height());
  ui->graphicsView->fitInView(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
  if (!vars->doingInitialization)
    ResizeGraphHandler();
}

void MainWindow::showEvent(QShowEvent *event)
{
  ui->graphicsView->fitInView(0, 0, graph.getScene()->width(), graph.getScene()->height());
  QWidget::showEvent(event);
}

void MainWindow::on_actionMultiscan_Help_triggered()
{
  QString Text = "Multiscan opens four sweep windows that will be swept in order. Originally they match the main graph window,"
  " but the settings can be changed either through the Settings menu, or by returning to the main graph with"
  " the Windows menu, changing settings there, returning to multiscan with the Show Multiscan menu, and selecting"
  " Transfer from Main in the Control menu. Likewise the settings and data for any multiscan window can be"
  " transfered to the main graph with Transfer To Main. A maximum of 2000 steps is allowed for multiscanning.\n\n"


  "The Control menu allows control of the sweep. The Windows menu allows navigating to the desired window, and also"
  " allows the windows to be Tiled or Stacked. Windows can be moved and resized; resizing does not affect the"
  " graph size (which matches that of the main graph when the windows were created), but affects the area of the"
  " graph that is shown. If you don't need all four windows, you can close the unwanted ones. You can also Skip"
  " a window that you don't need or don't want updated. Closed windows are gone; skipped windows can be unskipped.\n\n"

  "Once multiscan is run, the unclosed windows remain even after returning to the main graph, and can be shown"
  " using the Multiscan window. The main window and multiscan windows never show at the same time."
  " The multiscan windows are deleted when you Quit Multiscan from any Control menu or from the Multiscan"
  " menu of the main graph.";


  QMessageBox::about(0, "Multiscan Help", Text );
}

void MainWindow::on_actionVNA_Reflection_triggered()
{
  RestartReflectionMode();
}

void MainWindow::on_actionHardware_Config_Manager_triggered()
{
  menuRunConfig();
}

void MainWindow::on_actionVNA_TRansmission_triggered()
{
  RestartTransmissionMode();
}

void MainWindow::on_actionSpectrum_Analyzer_triggered()
{
  RestartPlainSAmode();
}

void MainWindow::on_actionSpectrum_Analyzer_with_TG_triggered()
{
  RestartSATGmode();
}

void MainWindow::on_actionCopy_Image_triggered()
{
  CopyImage();
}

void MainWindow::on_actionLoad_Data_triggered()
{
  menuLoadDataFile();
}

void MainWindow::on_actionInitial_Cal_Manager_triggered()
{
  //Menu item for config manager was selected //SEW6 rewrote routine.ver113-7c
  if (graph.haltsweep==1)
    FinishSweeping();     //Finish last point of sweep that was in progress. ver116-4j
  //if calManWindHndl$<>"" then wait    //Do nothing if cal mangager already running ver116-1b
  QString savePath=vars->path; //ver114-4c
  //calRunManager will return with filter path 1 installed.
  RequireRestart();     //SEW8 Let the user proceed only by Restarting
  calMan.calManRunManager(activeConfig.hasVNA);
  vars->path=savePath; //ver114-4c  //restores prior filter
  SelectFilter(filtbank);  //ver116-4j
}

void MainWindow::on_actionApperances_triggered()
{
  gridappearance.AppearanceShow();
  RedrawGraph(0);
}

void MainWindow::on_actionSave_Image_triggered()
{
  SaveImage();
}

void MainWindow::on_actionSave_Debug_Info_triggered()
{
  //Menu item to save debug data
  //Data is stored in a folder called "xxDebug" in the default directory
  if (graph.haltsweep==1) FinishSweeping();
  DebugSaveData();
}

void MainWindow::on_btnContinue_clicked()
{
  QPainterPath path;
  QPen bluePen(Qt::blue);

  path.moveTo(70, graph.clientHeightOffset);
  for (int i = 0; i < 400; i++)
  {
    path.lineTo(i + 70, rand() % graph.gGridHeight + graph.clientHeightOffset );
  }

  QGraphicsPathItem *item = graph.getScene()->addPath(path);
  item->setPen(bluePen);
  QCoreApplication::processEvents();
  graph.getScene()->removeItem(item);
  item = graph.getScene()->addPath(path);
  item->setPen(QPen(Qt::red));
}

void MainWindow::on_actionComponent_Meter_triggered()
{
  //comp.menuComponentMeasure();
}

void MainWindow::on_actionCrystal_Analysis_triggered()
{
  //xtal.menuCrystalAnalysis();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
  QMessageBox::aboutQt(this);
}

void MainWindow::on_actionAbout_triggered()
{
  QString copyWrite;
  copyWrite = "Modular Spectrum Analyzer , in Qt.\n\n";
  copyWrite += "Version " + QApplication::applicationVersion() + "\n";
  copyWrite += "\n"
      " Copyright (c) 2013 Bill Lindbergs\n"
      "\n"
      " The majority of this code is from spectrumanalyzer.bas, written by\n"
      " Scotty Sprowls and modified by Sam Wetterlin and Dave Robertsn\n"
      "\n"
      " This file may be distributed and/or modified under the terms of the\n"
      " GNU General Public License version 2 as published by the Free Software\n"
      " Foundation. (See COPYING.GPL for details.)\n"
      "\n"
      " This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE\n"
      " WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.\n";
  QMessageBox::about(this,"MSA-Qt",copyWrite);
}

void MainWindow::on_btnTestSetup_clicked()
{

}

void MainWindow::on_btnRestart_clicked()
{
  if (graph.haltsweep == 1)
    Halted();

  Restart();
}

void MainWindow::on_btnRedraw_clicked()
{
  //Redraw button was pushed
  //haltsweep=1 if scan is in progress, so we set flag to halt sweeping on return from the "scan"
  //command that enabled this button to be handled.
  if (graph.haltsweep==1)
  {
    graph.continueCode=1;
    return;   //Signal to halt after "scan" command
  }
  mDeleteMarker("Halt");    //Delete Halt marker ver114-4c
  if (smithGraphHndl()!="")
  {
    //smithDrawChart();    //To recreate bitmap of background, just in case it is messed up ver115-2c
  }
  graph.refreshGridDirty=1;
  graph.RefreshGraph(0);
}

void MainWindow::on_btnExpandLR_clicked()
{

}

void MainWindow::on_btnmMarkToCenter_clicked()
{

}

void MainWindow::on_btnMarkEnterFreq_clicked()
{

}

void MainWindow::on_btnMarkClear_clicked()
{

}

void MainWindow::on_btnOneStep_clicked()
{

}

void MainWindow::on_actionSave_Prefs_triggered()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;

  //Save current preferences file to user-specified location.
  QString filter="Text files (*.txt);;"
      "All files (*.*)";
  QString defaultExt="txt";
  QString initialDir= DefaultDir+"/MSA_Info/MSA_Prefs/";
  QString initialFile="Prefs.txt";

  QString fileName = uSaveFileDialog(filter, defaultExt, initialDir, initialFile, "Save Preference File");
  if (fileName!="")
  {
    SavePreferenceFile(fileName); //blank means cancelled
  }
}

void MainWindow::on_actionLoad_Prefs_triggered()
{
  //Load user-specified preferences file.
  if (graph.haltsweep==1)
    FinishSweeping();   //ver115-1e


  QString filter="Text files (*.txt);;"
      "All files (*.*)";
  QString defaultExt="txt";
  QString initialDir= DefaultDir+"/MSA_Info/MSA_Prefs/";
  QString initialFile="Prefs.txt";
  QString fileName=uOpenFileDialog(filter, defaultExt, initialDir, initialFile, "Load Preference File");

  if (fileName!="") //Blank means cancelled
    LoadPreferenceFile(fileName);
}

void MainWindow::on_actionSave_Data_triggered()
{

}
