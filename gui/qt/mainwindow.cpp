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
#include "constants.h"
#include "dialogchooseprimaryaxis.h"
#include "dialogFreqAxisPreference.h"
#include <qwaitcondition.h>
#include "smithdialog.h"

QStateMachine wew;

enum
{
  doWait,
  doHalt,
  doRestart,
  doNothing
};


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  winConfigMan = NULL;
  vars = NULL;
  showVars = NULL;
  ui->setupUi(this);
  qApp->installEventFilter(this);

  QDesktopWidget *desktop = QApplication::desktop();
  if ( 1==desktop->screenCount()  )
  {
      // single monitor - use built in
  //    showFullScreen();
  } else
  {
      QRect rect = desktop->screenGeometry(0);
      move(rect.topLeft());
     // setWindowState(Qt::WindowFullScreen);
  }

  DefaultDir = QApplication::applicationDirPath();

  vars = new globalVars;
  graph = new msagraph(this);
  hwdIf = new hwdInterface(this);
  gridappearance = new dialogGridappearance(this);

  //smithDialog *smith = new smithDialog(this);
  //smith->exec();
  //QApplication::exit(0);
  //return;

  vnaCal.setUwork(&uWork);
  vnaCal.setGlobalVars(vars);
  vnaCal.setFilePath(DefaultDir);

  graph->setGlobalVars(vars);
  graph->setActiveConfig(&activeConfig);
  graph->setUwork(&uWork);
  graph->setAppearance(gridappearance);
  graph->setVna(&vnaCal);
  graph->setHwdIf(hwdIf);


  graph->currGraphBoxHeight=600 - graph->clientHeightOffset - 44;
  graph->currGraphBoxWidth=800 - graph->clientWidthOffset;

  graph->gInitFirstUse(ui->graphicsView, graph->currGraphBoxWidth, graph->currGraphBoxHeight, graph->graphMarLeft
                      , graph->graphMarRight, graph->graphMarTop, graph->graphMarBot);  //Initialize graphing module

  hwdIf->setActiveConfig(&activeConfig);
  hwdIf->setGlobalVars(vars);
  hwdIf->setUwork(&uWork);
  hwdIf->setVna(&vnaCal);
  hwdIf->setFilePath(DefaultDir);
  hwdIf->filtbank = 0;
  hwdIf->scanResumed = 0;
  hwdIf->suppressSweepTime=0;


  QString bgColorName = palette().color(QPalette::Normal, QPalette::Window).name();
  QString strStyleSheet = QString("QLineEdit {background-color: ").append(bgColorName).append("}");
  //ui->lineEdit->setStyleSheet(strStyleSheet);

  ui->lineEdit_SweepTime->setMaximumWidth(70);
  ui->lineEdit_SweepTime->setStyleSheet( strStyleSheet );

  ui->statusBar->addPermanentWidget(ui->lineEdit_SweepTime);
  ui->mainToolBar->setVisible(false);

  special = 0;

  ui->graphicsView->setScene(&scenePleaseWait);
  scenePleaseWait.clear();
  scenePleaseWait.addSimpleText("Please wait, starting up.");
  scenePleaseWait.setSceneRect(scenePleaseWait.itemsBoundingRect());
  ui->graphicsView->fitInView(scenePleaseWait.sceneRect());

  FindClientOffsets();   //set clientWidthOffset and clientHeightOffset from test window ver115-1b


  CreateGraphWindow();

  //timerStart = new QTimer(this);
  //connect(timerStart, SIGNAL(timeout()), this, SLOT(updateView()));
  //timerStart->setSingleShot(true);

  // use a timer to trigger the initilization so that
  // we can start displaying feedback to the user
  //timerStart2 = new QTimer(this);
  //connect(timerStart2, SIGNAL(timeout()), this, SLOT(delayedStart()));
  //timerStart2->setSingleShot(true);
  //timerStart2->start(50);
  QTimer::singleShot(0, this, SLOT(delayedStart()));
}

MainWindow::~MainWindow()
{
  if (showVars)
  {
    delete showVars;
  }
  //delete timerStart;
  //delete timerStart2;
  delete hwdIf->usb;
  delete ui;
  if (winConfigMan)
  {
    delete winConfigMan;
  }
  delete vars;
}

void MainWindow::delayedStart()
{
  //timerStart2->stop();

  //Suppress parallel port if we don't have the DLLs
  if (util.uVerifyDLL("ntport"))
    vars->suppressHardware=0;
  else
    vars->suppressHardware=1; //may change when we have cb info

  bool bUsbAvailable;
  if (util.uVerifyDLL("msadll"))
    bUsbAvailable = true;
  else
    bUsbAvailable = false;

  if (bUsbAvailable)
  {
    if (hwdIf->usb->usbInterfaceOpen(QApplication::applicationDirPath() + "/msadll.dll"))
    {
      int rev = hwdIf->usb->usbMSAGetVersions();
      if (rev / 256 < 2)
      {
        hwdIf->usb->usbCloseInterface();
        QMessageBox::critical(0, "MSADLL", QObject::tr("The version number of msadll is too old for me to use"));
      }
      if ((rev & 255) < 36)
      {
        hwdIf->usb->usbCloseInterface();
        QMessageBox::critical(0, "MSADLL", QObject::tr("The USB interface is either not plugged in or is too old a version for me"));
      }
    }
  }
  winConfigMan = new dialogConfigMan(this);

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
      if (activeConfig.videoFilterNames[1] != "Wide")
      {
        activeConfig.videoFilterNames[1] = "Wide";
        activeConfig.videoFilterCaps[1][0]=0.002;
        activeConfig.videoFilterCaps[1][1]=0.011;
      }
      winConfigMan->configSaveFile(activeConfig); //Save config file in current format ver114-5i
    }
  }

  hwdIf->port = activeConfig.globalPort;
  hwdIf->initVars();

  vars->glitchtime = 0;
  hwdIf->status = hwdIf->port + 1;
  hwdIf->control = hwdIf->port + 2;

  vars->bandEnd1G=1000; vars->bandEnd2G=2000;    //good for now ver116-4s

  if (activeConfig.cb == hwdUsbV1)
  {
    //            3 means USB. suppressHardware relates only to parallel port ver116-4b
    //            An initial low on the PS bit that controls latched switches may be draining switch capacitors
    //            Set it high and allow some recharge time. At this point we don't care what the switches get set to,
    //            and if the capacitors are discharging the switches won//t change state at all.
    vars->suppressHardware = 0;
  }
  vars->freqBand = 1;
  hwdIf->SelectLatchedSwitches(1);
  util.uSleep(1000);               //Wait 1 second for capacitor recharge. There will be additional software delays before PS is used.
  hwdIf->calMan->calInitFirstUse(201, 1001, activeConfig.hasVNA);    //Initialize Mag/Freq Calibration Module--201 max mag cal points; 1001 max freq cal points ver114-4b
  //ResizeArrays needs TGtop, so we do it after loading config file

  ResizeArrays(2001);   //Make all arrays big enough for 2001 points; also loads BaseLineCal file

  //---------Load path and freq calibration info------
  hwdIf->calMan->calInstallFile(0);   //Loads frequency calibration file; creates one if necessary
  for (int i = activeConfig.MSANumFilters; i == 1; i--)
  {
    //For each filter, create the file if necessary and load it
    //Each one loaded replaces the data from the previous one. We are just
    //trying to be sure they exist and are OK.
    //We do this in reverse order to path 1 will be the last one and stays in place
    //This also sets finalfreq and finalbw
    hwdIf->calMan->calInstallFile(i);
  }
  vars->path="Path 1";
  //Note physical selection of filter 1 is done in step 5 below
  for (int i=1; i <= activeConfig.MSANumFilters; i++)
  {
    //For each filter, combine freq and bw into nicely aligned string. Used to load #main.FiltList
    activeConfig.MSAFiltStrings[i-1]="P" + QString("%1").arg(i) + "   "
        + winConfigMan->configFormatFilter(activeConfig.MSAFilters[i][0]
        , activeConfig.MSAFilters[i][1]);
  }


  //The below are not actually the desired states. See step 3 (initialization) for explanation.
  //The latched filter addresses will be asserted by SelectVideoFilter because the video filter
  //shares the same latch. But the PS bit will not be toggled, so this will not actually affect
  //latched switches that rely on PS, and won't drain capacitors. But this will help initialize
  //latched switches that generate the latching pulse from a change of address.
  //This is done after loading config file so capacitor info is available, and after
  //loading cal files so auto wait info is available.
  vars->videoFilter="Wide";
  vars->freqBand=2;
  vars->switchTR=1;
  vars->switchFR=1;
  hwdIf->SelectVideoFilter();

  //---------Create OperatingCal Folder-------------
  /*if (activeConfig.TGtop>0)
  {
    int isErr=vnaCal.CreateOperatingCalFolder();  //Create OperatingCal folder if it does not exist
    if (isErr)
      QMessageBox::about(0,"Error", "Unable to create OperatingCal folder.");
  }
*/

  vars->bUseUsb = 1;


  //3.Initialize for whatever mode we will start up in
  //Some of these initializations may be changed when the preferences file
  //is loaded in [LoadPreferenceFile]
  vars->doingInitialization=1;
  vars->suppressPhase=0;     //Turns phase on ver116-1b
  vars->suppressHardwareInitOnRestart=0;     //Normally we do hardware initialization on each restart.
  vars->multiscanIsOpen=0;
  vars->multiscanInProgress=0;
  vars->baseFrequency=0;
  vars->cftest=0;    //cavity filter sweep test off ver116-4b
  vars->msaMode= modeSA;
  vars->planeadj=0;
  vars->gentrk=0;
  vars->normrev=0;
  vars->sweepDir=1;
  vars->videoFilter="Wide";
  vars->useAutoWait=0;
  vars->autoWaitPrecision="Normal";


  hwdIf->SelectVideoFilter(); //Sets videoFilterAddress and outputs it ver116-1b
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
  vars->switchTR=0;
  vars->switchFR=0 ;
  hwdIf->SelectLatchedSwitches(vars->lastSetBand);
  util.uSleep(500); //wait again because latching will occur again when preferences are loaded.

  vars->returnBeforeFirstStep=0;
  vars->haltedAfterPartialRestart=0;
  vars->specialOneSweep=0;
  vars->crystalLastUsedID=0;
  vars->imageSaveLastFolder = DefaultDir;    //Folder in which image was last saved ver115-2a
  hwdIf->touch.touchLastFolder = DefaultDir;     //Folder from which param data was last loaded ver115-5f
  vars->doSpecialRLCSpec ="RLC[P, R1000,C1n,L1u]";   //default for doSpecialGraph of simulated RLC
  vars->RefRLCLastNumPoints=0;
  vars->RefRLCLastConnect ="";   //For continuity calling ReflectionRLC
  vars->analyzeQLastNumPoints=0;   //For continuity in AnalyzeQ
  vars->GDLastNumPoints=0;       //Number of points last used for group delay analysis
  vars->frontEndCalNumPoints=0;  //No front end adjustment
  vars->frontEndActiveFilePath="";
  vars->frontEndLastFolder=DefaultDir;

  //ver114-3f moved the call to gInitFirstUse here from [CreateGraphWindow]
  graph->InitGraphParams();   //Initialize parameters to set up the graphing module ver114-3f moved

  //ChangeMode(); //create Graph Window in mode of msaMode$
  vnaCal.desiredCalLevel=0;   //Desire no cal
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

  ChangeMode(); //create Graph Window in mode of msaMode$

  //call uSleep 500     //Loading Preferences may re-latch switches; allow some recharge time ver116-1b delver116-4d
  if (graph->gGetXIsLinear())
  {
    vars->userFreqPref=0;
  }
  else
  {
    vars->userFreqPref=1;
  }      //Start with Center/Span for linear, Start/Stop for log //ver115-1d
  graph->mClearMarkers();   //Clear all graph markers

  //4.measure computer speed and update global, glitchtime
  //Determine speed of computer //ver111-37c
  if (vars->glitchtime == 0)
  {
    vars->glitchtime = 1;
    // fix me need to test the glitch time once there is hardware
    hwdIf->AutoGlitchtime();//ver111-37c
  }
  //return with glitchtime, number approximates 1 millisecond of computer processing speed with Liberty Basic
  //this is a "coarse" calculation.

  //5.Command Filter Bank
  hwdIf->InitializeHardware();

  //timerStart->start(50);
  //connect(timerStart, SIGNAL(timeout()), this, SLOT(updateView()));
//  QTimer::singleShot(0, this, SLOT(updateView()));
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
  graph->clientHeightOffset = 46;
  graph->clientWidthOffset = 8;
}

void MainWindow::RequireRestart()
{
  //Disable OneStep and Continue so user can only proceed by Restart
  if (vars->doingInitialization)
    return;    //Buttons don't exist yet    ver114-3f
  graph->haltsweep=0;
  DisplayButtonsForHalted();
  ui->btnOneStep->setEnabled(false);
  ui->btnContinue->setEnabled(false);
}

void MainWindow::DisplayButtonsForRunning()
{
  //Display buttons for sweep in progress
  if (vars->doingInitialization)
  {
    return;    //Buttons don't exist yet
  }
  ui->btnOneStep->setEnabled(true);
  ui->btnContinue->setEnabled(true);
  ui->btnRestart->setText("Running");
  //OneStep becomes HaltAtEnd when scan is in progress
  ui->btnOneStep->setText("Halt At End");
  //Continue becomes Halt when scan is in progress
  ui->btnContinue->setText("Halt");
  ui->btnRedraw->setVisible(false);
  //QApplication::processEvents();
}

void MainWindow::DisplayButtonsForHalted()
{
  //Display buttons for sweep halted, to enable resuming or restarting
  if (vars->doingInitialization)
  {
    return;    //Buttons don't exist yet
  }
  ui->btnOneStep->setEnabled(true);
  ui->btnContinue->setEnabled(true);
  ui->btnRestart->setText("Restart");
          //OneStep becomes HaltAtEnd when scan is in progress
  ui->btnOneStep->setText("One Step");
      //Continue becomes Halt when scan is in progress
  ui->btnContinue->setText("Continue");
  ui->btnRedraw->setVisible(true); //hide during scan
  //QApplication::processEvents();

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

void MainWindow::menuRunConfig()
{
//Graph Window Menu,Setup,Configuration Manager was selected
  if (graph->haltsweep==1)
  {
    FinishSweeping();     //Finish last point of sweep that was in progress. ver116-4j
  }
  //savePath=path;
  int cancelled=winConfigMan->configRunManager(0);   //0 signals we are not running on startup so cancellation is allowed
  if (cancelled)   //Cancelled; restore filter setting; Halt or wait
  {
    //vars->path=vars->savePath; //ver114-4c
    hwdIf->SelectFilter(hwdIf->filtbank);  //ver116-4j
    return;
  }
  hwdIf->finished();     //Must restart if config was changed
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
  if (vars->msaMode==modeSA)
  {
    if (vars->gentrk==0) modeTitle="Spectrum Analyzer Mode"; else modeTitle="Spectrum Analyzer with TG Mode";
  }
  if (vars->msaMode==modeScalarTrans) modeTitle="Tracking Generator Mode";
  if (vars->msaMode==modeVectorTrans) modeTitle="VNA Transmission Mode";
  if (vars->msaMode==modeReflection) modeTitle="VNA Reflection Mode";
  QString ver="Ver " + QCoreApplication::applicationVersion();
  setWindowTitle("MSA-Qt Graph Window for "+modeTitle+ "; "+ ver);

  //Note we continue even if there is no mode change, mainly to get multiscan window right
  //if msaMode$=menuMode$ then exit sub //Nothing to do

  int wasTransMode = (vars->menuMode==modeScalarTrans || vars->menuMode==modeVectorTrans);  //whether prior mode was transmission
  int isTransMode= (vars->msaMode==modeScalarTrans || vars->msaMode==modeVectorTrans); //whether current mode is transmission
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
  //if (vars->msaMode!=modeRefelection) menuOK=uShowMenuItem(hFunctionsMenu, menuFunctionsFilterID,0, "Filter Analysis", 0);
  //frontEndID=uMenuItemID(hFileMenu,5);    // "Load Front End" menu is sixth in File menu, which is position 5
  if (vars->msaMode!=modeSA)
  {
        //menuOK=uGrayMenu(hFileMenu, frontEndID) //Disable Load Front End   //ver115-9d
        //menuOK=uShowMenuItem(hGraphMenuBar, -1, hOperatingCalMenu, "Operating Cal", menuOperatingCalPosition)      //Operating Cal menu
        vars->menuOperatingCalShowing=1;
        //menuOK=uShowMenuItem(hGraphMenuBar, -1, hTwoPortMenu, "Two-Port", menuTwoPortPosition)
        vars->menuTwoPortShowing=1;
    if (vars->msaMode==modeReflection)
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
        if msaMode$=modeVectorTrans then menuOK=uShowMenuItem(hFunctionsMenu, menuFunctionsGroupDelayID, 0, "Group Delay", 4) //ver115-8b
        menuOK=uShowMenuItem(hDataMenu, menuDataS21ID, 0,"S21 Parameters", 2)    //S21 is third in Data menu
        menuOK=uShowMenuItem(hDataMenu, menuDataLineCalID, 0, "Installed Line Cal", 3)
            */
    }
  }
  if (vars->msaMode==modeSA)
  {
    //menuOK=uEnableMenu(hFileMenu, frontEndID) //Enable Load Front End   //ver115-9d
  }
  if (vars->msaMode==modeSA && vars->gentrk==0)   //Display Multiscan window only when in SA mode without TG
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

  if (special==1)
  {
    CloseSpecial(1);
  }
  vars->msaMode=modeSA;
  graph->SetDefaultGraphData();    //clears autoscale, sets Y1 and Y2 data types and range, and sets Y2DisplayMode and Y1DisplayMode
  vars->sgout=10;    //Set to 10 MHz no matter what band we are using. Will be irrelevant if in TG mode
  vars->offset=0;    //TG offset; will be irrelevant if in TG mode.
  vars->spurcheck = 0; //this assures Spur Test is OFF.
  graph->referenceLineSpec=""; graph->referenceLineType=0;
  ChangeMode();
  QTimer::singleShot(0, this, SLOT(Restart()));
}

void MainWindow::GoSAmode()
{
  //Switch to MSA mode and return; Get here only from [ChangeMode]
  //We don't initialize variables here because they may have been set by loading Preferences ver115-2a
  /*if (graph->graphBox=="")  //See if window is created yet ver115-5d
  {
    CreateGraphWindow();   //Note msaMode$ is new mode; menuMode$ is old mode
  }
  else*/
  {
    ConformMenusToMode();
    if (vars->menuMode!= vars->msaMode) //menuMode$ has prior mode, to which menus are now conformed ver115-5d
    {
      graph->mClearMarkers();
      graph->gClearAllReferences();
      graph->gSetNumPoints(0);
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
  }
  //For a menu-initiated change, we initialize certain variables ver115-2a
  vars->gentrk = 1;
  vars->normrev = 0;
  vars->offset = 0;  //turn on tracking generator, normal, zero offset
  graph->referenceLineSpec="";
  graph->referenceLineType=0;

  //Switching between VectorTrans and Reflection modes is specially handled, to preserve the data and Y-axis
  //settings that were last in effect for the new mode. This special treatment is done for menu-driven change
  //or by certain internally generated changes that call [ToggleTransmissionReflection], but not for changes
  //resulting from loading of preference files.   ver116-1b
  if (vars->menuMode==modeReflection && vars->transLastSteps!=0)
  {
    setCursor(Qt::WaitCursor);
    graph->ToggleTransmissionReflection();
    setCursor(Qt::ArrowCursor);
    return;
  }
  if (activeConfig.hasVNA)
  {
    vars->msaMode=modeVectorTrans;
  }
  else
  {
    vars->msaMode=modeScalarTrans;
  }
  graph->SetDefaultGraphData();    //clears autoscale, sets Y1 and Y2 data types and range, and sets Y2DisplayMode and Y1DisplayMode ver115-3b
  vnaCal.S21JigAttach="Series";  //Start using series jig ver115-5a
  ChangeMode();
  QTimer::singleShot(0, this, SLOT(Restart()));
}
void MainWindow::GoTransmissionMode()
{
  //Switch to Transmission mode and return; Get here only from [ChangeMode]
  //We don't initialize variables here because they may have been set by loading Preferences ver115-2a
  vars->spurcheck = 0; //this assures Spur Test is OFF. ver116-1b
  vars->switchTR=0;
  hwdIf->SelectLatchedSwitches(vars->freqBand); //Set transmission/reflection switch to transmission //ver116-1b ver116-4s
  /*if (graph->graphBox=="") //See if window is created yet ver115-5d
  {
    CreateGraphWindow();   //Note msaMode$ is new mode; menuMode$ is old mode
  }
  else*/
  {
    int smoothModeChange=0;
    if (vars->menuMode==modeReflection)   //menuMode$ has prior mode ver116-1b
    {
      //If changing from reflection mode and sweep frequencies are the same, we preserve some settings
      if (vars->transLastSteps!=0
          && vars->transLastSteps==vars->steps
          && vars->transLastStartFreq==vars->startfreq
          && vars->transLastEndFreq==vars->endfreq
          && vars->transLastIsLinear==graph->gGetXIsLinear())
      {
        smoothModeChange=1;
      }
    }
    if (smoothModeChange==0)
    {
      //We don't clear markers when changing between VectorTrans and Reflection if sweep freq is the same
      graph->mClearMarkers();
    }
    ConformMenusToMode();
    graph->gSetNumPoints(0);
    graph->gClearAllReferences();    //Old ones may not make sense
    vnaCal.SignalNoCalInstalled();
  }
}

void MainWindow::RestartReflectionMode()
{
  //Restart in Reflection mode. Handles menu item for mode change
  //if the Special Tests Window is open, close it.
  if (special==1)
  {
    //fix me returnflag=1;
    hwdIf->CloseSpecial();
    // fix me returnflag=0; //ver112-2f
  }
  //For a menu-initiated change, we initialize certain variables ver115-2a
  vars->gentrk = 1;
  vars->normrev = 0;
  vars->offset = 0;  //turn on tracking generator, normal, zero offset //ver111-17

  graph->referenceLineSpec=""; graph->referenceLineType=0;

  //Switching between VectorTrans and Reflection modes is specially handled, to preserve the data and Y-axis
  //settings that were last in effect for the new mode. This special treatment is done for menu-driven change
  //or by certain internally generated changes that call [ToggleTransmissionReflection], but not for changes
  //resulting from loading of preference files.   ver116-1b
  //if graphBox$<>"" and menuMode$=modeVectorTrans and refLastSteps<>0 then
  if (vars->menuMode==modeVectorTrans && vars->refLastSteps!=0)
  {
    setCursor(Qt::WaitCursor);
    graph->ToggleTransmissionReflection();
    setCursor(Qt::ArrowCursor);

    return; // : wait
  }
  vars->msaMode=modeReflection; //ver115-2a
  graph->SetDefaultGraphData();    //clears autoscale, sets Y1 and Y2 data types and range, and sets Y2DisplayMode and Y1DisplayMode ver115-3b
  vnaCal.S11JigType="Reflect";   //Start using bridge ver115-5a
  ChangeMode();
  QTimer::singleShot(0, this, SLOT(Restart()));
}
void MainWindow::GoReflectionMode()
{

  //Switch to Reflection mode and return; Get here only from [ChangeMode]
  //We don't initialize variables here because they may have been set by loading Preferences ver115-2a
  vars->spurcheck = 0; //this assures Spur Test is OFF. ver116-1b
  vars->switchTR=1;
  hwdIf->SelectLatchedSwitches(vars->freqBand); //Set transmission/reflection switch to reflection //ver116-1b ver116-4s
  //if graphBox$="" then //See if window is created yet ver115-5d
  //        gosub [CreateGraphWindow]   //Note msaMode$ is new mode; menuMode$ is old mode
  //    else
  int smoothModeChange=0;
  if (vars->menuMode==modeVectorTrans)  //menuMode$ has prior mode ver116-1b
  {
    //If changing from vector transmission mode and sweep frequencies are the same, we preserve some settings
    if (vars->refLastSteps!=0 && vars->refLastSteps==vars->steps
        && vars->refLastStartFreq==vars->startfreq
        && vars->refLastEndFreq==vars->endfreq
        && vars->refLastIsLinear==graph->gGetXIsLinear())
    {
      smoothModeChange=1;
    }
  }
  if (smoothModeChange==0)
  {
    //We don't clear markers when changing between VectorTrans and Reflection if sweep freq is the same
    graph->mClearMarkers();
  }
  ConformMenusToMode();
  graph->gSetNumPoints(0);
  graph->gClearAllReferences();    //Old ones may not make sense
  vnaCal.SignalNoCalInstalled();
  //    end if
  //S21JigR0 is sometimes referenced if we explicitly use the series or shunt jig. But it causes problems
  //in reflection mode if it can have a different value from S11BridgeR0.
  vnaCal.S21JigR0=vnaCal.S11BridgeR0;    //ver116-4j
  vars->suppressPhase=0; //Reflection always needs phase, even if not displayed ver116-1b
  // fix me smithOpenChart(); //Create smith chart
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
        if msaMode$=modeSA then clickedPointNum=roundedClickPointNum   'Round to integral point for SA mode ver115-2d
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

//sub StartingLimits graph->componConst, oldData, byref axisMin, byref axisMax //Determine starting min and max for axes
    //If the axis limits are unreasonable, we find better limits
    //graph->componConst specifies what type of value we are dealing with. E.g. constTheta.
    //oldData is the data type before the change.
    //axisMin and axisMax are the original limits; we change if necessary.
    //This is applied only when changing graph types.
    //In general we just pick standard limits. But if the old data type is related to the new one, we may take
    //into account the old limits.
    //If the user specifies goofy values without changing the data type, or after doing so, we go with his specs.
    //absMin=abs(axisMin) : absMax=abs(axisMax)
    //limitsDone=0
/*    switch(componConst)
    {
        case constGraphS11DB:
        case constMagDBM:
        case constMagDB:   //db and dbm values
            axisMax=0;
            axisMin=-100;
            break;

        case constReturnLoss:
        case constInsertionLoss:  //Positive dB values
            axisMin=0;
            axisMax=60;
            break;
      }
    */
/*
        case constGraphS11Ang, constTheta    //Two names for the same angles
            axisMin=-180 : axisMax=180  //Expand to full range  //ver114-8c
            if oldData<>constGraphS11Ang and oldData<>constTheta then axisMin=-180 : axisMax=180
            //If we changed from a similar data type, keep the existing limits if they are reasonable
            if axisMin>=axisMax then axisMin=-180 : axisMax=180

        case constAngle, constRawAngle, constImpedAng, constAdmitAng    //angles //ver115-1i added raw angle
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
            auxNum=graph->componConst-constAux0    'e.g. constAux4 produces 4
            axisMin=auxGraphDataInfo(auxNum,1)  : axisMax=auxGraphDataInfo(auxNum,2)

        case constNoGraph   'ver115-2c
            'Do nothing
        case else       'constGD and anything we missed
            'don't mess with these
    end select
end sub
*/
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
  graph->mUserMarkSelect("");  //Take same action as though user selected the marker
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

void MainWindow::OneStep()
{
  // The OneStep button shows HaltAtEnd during a scan
  if (graph->haltsweep == 1)
  {
    HaltAtEnd();
    return;
  }
  //If the graph has been redrawn without erasure, we need to erase that part, because
  //the normal one-point draw only erases a single segment, several points ahead.
  hwdIf->onestep = 1;
  FocusKeyBox();
}

void MainWindow::HaltAtEnd()
{
  //Halt At End button pushed.
  //This is actually the OneStep button during a scan, so we get here from [OneStep]
  //The only way to get here is for the user to click Halt At End during a sweep, which will be
  //detected on the "scan" command in the sweep loop. We just set a flag and return to the point
  //after the scan command.
  graph->haltAtEnd=1;     //Set flag to cause halt at end of this sweep.
  PostScan();
}

void MainWindow::Continue()
{
  // The Continue button shows Halt during a scan
  if (graph->haltsweep == 1)
  {
    graph->continueCode=1;
    //Halted();
    return;
  }
  hwdIf->onestep = 0;
  FocusKeyBox();
}

void MainWindow::FocusKeyBox()
{
  DisplayButtonsForRunning();   //SEW8 consolidated various button commands
  if (vars->isStickMode==0)
  {
    graph->gRestoreErasure();    //No erasure in stick modes
  }
  graph->PrintMessage("");
  //Indicate whether we are resuming a scan that was stopped in the middle.
  //If PartialRestart was done, we stopped prior to the first step, and are not "resuming"
  if (vars->haltedAfterPartialRestart)
  {
    hwdIf->scanResumed=0;
  }
  else
  {
    hwdIf->scanResumed=1;
  }
  QTimer::singleShot(0, this, SLOT(StartSweep()));
}


//--SEW Added the following routines to save/copy the graph image
//These routines are invoked through the File and Edit menus
//Scotty: I created additional variables here, all of which begin with captial X
//to avoid any conflicts with your variables.
//=====================Start Routines to Save/Copy Image===========================

void MainWindow::SaveImage()
{
  //Save graph image to file
  if (graph->haltsweep==1)
  {
    QMessageBox::information(this, "", "Sweep will halt at end. Then re-click Save Image.");
    graph->haltAtEnd=1;     //Set flag to cause halt at end of this sweep.
    PostScan();
    return;
  }
  if (vars->isStickMode==0) {graph->refreshGridDirty=1; graph->RefreshGraph(0); }  //Redraw without erasure mark; but not if we are in stick mode ver114-7d

  QString filter="Bitmap files(*.bmp);;All files(*.*)"; //ver115-6b
  QString defaultExt="bmp";
  QString initialDir=vars->imageSaveLastFolder+"/";
  QString initialFile="";
  QString graphFileName = util.uSaveFileDialog(this, filter, defaultExt, initialDir, initialFile, "Save Image To File");
  if (graphFileName!="")   //blank means cancelled
  {
    QImage image(ui->graphicsView->scene()->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
    image.fill(Qt::transparent);                                              // Start all pixels transparent

    QPainter painter(&image);
    ui->graphicsView->scene()->render(&painter);
    image.save(graphFileName);

    QString dum;
    util.uParsePath(graphFileName, vars->imageSaveLastFolder, dum); //Save folder in which file was saved ver115-2a
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
  if (graph->haltsweep==1)
  {
    QMessageBox::warning(0,"", "Sweep will halt at end. Then re-click Copy Image.");
    graph->haltAtEnd=1;     //Set flag to cause halt at end of this sweep.
    PostScan();
    return;
  }
  if (vars->isStickMode==0) { graph->refreshGridDirty=1; graph->RefreshGraph(0);}  //Redraw without erasure mark; but not if we are in stick mode ver114-7d

  QImage image("fn");
  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing);
  graph->getScene()->render(&painter);

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
    if haltsweep=1 then gosub FinishSweeping()  //Halt
    if msaMode$=modeRefelection then call ReflectionRLC else call TranRLCAnalysis
    call RequireRestart     //Scan can continue only by Restart
    wait
*/
}
//ver115-5d added void MainWindow::menuS11ToS21()
void MainWindow::menuS11ToS21()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    if haltsweep then gosub FinishSweeping()    //stop sweeping cleanly
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

    QForm$="3,2,3//UseMultiplier//SuppressMilli"
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
    if haltsweep=1 then gosub FinishSweeping()
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
    QForm$="3,2,3//UseMultiplier//SuppressMilli"
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
    if haltsweep=1 then gosub FinishSweeping()

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
      || graph->gGetXIsLinear() != touchIsLinear
      || vars->globalSteps != uWork.uWorkNumPoints-1)
  {
    //If freq range or sweep type changed, we need to adjust to it.
    vars->steps=vars->VNADataNumSteps;
    vars->globalSteps=vars->steps;
    graph->gSetNumDynamicSteps(vars->globalSteps);
    graph->SetStartStopFreq(touchStartFreq, touchEndFreq);
    graph->gSetXIsLinear(touchIsLinear);
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
    if (vars->msaMode==modeSA || vars->msaMode==modeScalarTrans)
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
    graph->gSetTitleLine(i, vars->VNADataTitle[i]);
  }

  vnaCal.SignalNoCalInstalled();   //To get valid cal installed, if applicable ver116-4b
    //Do Restart with a flag to return before taking any data
  vars->suppressHardwareInitOnRestart=1; //no need for hardware init ver116-4d
  PartialRestart();

  //We must now recreate the graph-> We proceed as though we just gathered the data
  //point by point
  for (vars->thisstep=vars->sweepStartStep; vars->thisstep != vars->sweepEndStep; vars->thisstep = vars->thisstep + vars->sweepDir)
  {
    //Plot this point the same as though we just gathered it.
    if (vars->msaMode!=modeSA)
      hwdIf->TransferToDataArrays();   //Enter data into S21DataArray or ReflectArray
    if (vars->VNARestoreDoR0AndPlaneExt)  //If user so specified, perform R0 conversion and plane extension
    {
        if (vars->msaMode==modeVectorTrans || vars->msaMode==modeScalarTrans)
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
            if (vars->msaMode==modeReflection)
            {
                if (vars->planeadj!=0 || vnaCal.S11BridgeR0!=vnaCal.S11GraphR0)
                {
                    float f=vars->VNAData[vars->thisstep][0];
                    float db=vars->VNAData[vars->thisstep][1];
                    float ang=vars->VNAData[vars->thisstep][2];
                    hwdIf->ApplyExtensionAndTransformR0(f, db, ang);
                    vars->ReflectArray[vars->thisstep][constGraphS11DB]=db;   //Save final S11 in db, angle format (in Graph R0, after plane ext)
                    while (ang>180)
                    {
                      ang=ang-360;
                    }
                    while (ang<=-180)
                    {
                      ang=ang+360;
                    }
                    vars->ReflectArray[vars->thisstep][constGraphS11Ang]=ang;
                }
            }
        }
    }
    if (vars->msaMode==modeReflection)
      graph->CalcReflectDerivedData(vars->thisstep);
    graph->PlotDataToScreen();
  }

    //void MainWindow::PlotDataToScreen() will have refreshed the graph at the final data point, unless
    //refreshEachScan is turned off. We want to be sure a refresh gets done.
  if (graph->refreshEachScan==0)
    graph->RefreshGraph(0);

    //Restart puts new time stamp in title, so put ours back in
  QString t3="      "+vars->VNADataTitle[3]+"      ";
  if (t3.length()<35)
    t3="          "+t3+"          ";
  graph->gSetTitleLine(3, t3);   //Extra blanks for clearing in case size changed

  //Reprint title without clearing; new data will cover old data
  //The refresh routine will have used saved info for faster drawing, and so will
  //have the time stamp from PartialRestart done in void MainWindow::LoadDataFromWorkArray(), which we don't want.

  graph->gPrintTitle(0);
  //#graphBox$, "flush"
  graph->gSetTitleLine(3,vars->VNADataTitle[3]);  //Get rid of extra blanks
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

    'We must now recreate the graph-> We proceed as though we just gathered the data
    'point by point
    for thisstep=sweepStartStep to sweepEndStep step sweepDir
        'Plot this point the same as though we just gathered it.
        'Apply planeadj and graph R0 transformed and put info into S21DataArray or ReflectArray
        if msaMode$<>modeSA then gosub [ProcessDataArrays]
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
//ver115-5f
  if (graph->haltsweep==1)
    FinishSweeping();
  QString filter="Parameter files (*.s1p);;"
              "Text files (*.txt);;"
              "CSV files (*.csv);;"
              "All files (*.*)"; //ver115-6b
  QString defaultExt="s1p";
  QString initialDir= hwdIf->touch.touchLastFolder;//+"/";
  QString initialFile="";
  QString dataFileName = util.uOpenFileDialog(this, filter, defaultExt, initialDir, initialFile, "Open Data File");
  if (dataFileName=="")
    return;   //user cancelled

  QFile *dataHandle = hwdIf->touch.touchOpenInputFile(dataFileName);
  if (dataHandle==NULL)
  {
    vars->restoreErr="File failed to open: "+dataFileName;
    return;
  }
  QString dum;
  util.uParsePath(dataFileName, hwdIf->touch.touchLastFolder, dum); //Save folder from which file was loaded
  vars->restoreFileName=dataFileName;
  vars->restoreFileHndl=dataHandle;
  LoadDataFileWithContext(dataFileName);
  RequireRestart();
}







//================End Configuration Manager Module=================
//==================================================================



//=================End Mag/Freq  Calibration Module===================
//=================End Calibration Manager Module===================
//--SEW Added the following module to provide utility routines used by other modules
//

//=================End Utilities Module=======================

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

  graph->gDrawGrid();
  graph->DrawSetupInfo();     //Draw info describing the sweep setup

  ui->graphicsView->setScene(graph->getScene());
  //graph->getScene()->setSceneRect(graph->getScene()->itemsBoundingRect());
  //ui->graphicsView->fitInView(graph->getScene()->sceneRect());
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


QString MainWindow::smithGraphHndl()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  //Return handle to graphics
  // fix me
  //smithGraphHndl=vars->smithHndl;
  return "fix me 3";
}


void MainWindow::CleanupAfterSweep()
{
  //Do cleanup after a sweep to be sure flags are set/reset properly
  //Called by [FinishSweeping]. Can also be called by other routines to immediately
  //terminate a sweep when they will be Restarting so they don't care about finishing the plotting.
  DisplayButtonsForHalted();
  if (vars->thisstep == vars->sweepEndStep)
    hwdIf->haltWasAtEnd=1;
  else
    hwdIf->haltWasAtEnd=0;
  graph->haltAtEnd=0;      //In case we got here from auto halt at end of sweep
  vars->calInProgress=0;
  graph->haltsweep = 0; //this says the sweep has been halted, so don't print the first command of the next sweep step //ver111-20

}
void MainWindow::ChangeMode()
{
  //Change mode to msaMode$
  //menuMode$ indicates the mode to which the menus are currently conformed, which is the mode we are changing from.
  //The very first time, we create the window at a standard size. After that, we work with the existing
  //window and redo the menus, leaving the size and location as is.
  if (vars->multiscanIsOpen)
    multiscanCloseAll();  //Quit multiscan--it is for SA only
  smithFinished("");   //Close smith chart if it is open ver115-1b
  if (activeConfig.hasVNA==0 && (vars->msaMode==modeVectorTrans || vars->msaMode==modeReflection))
  {
    vars->msaMode=modeSA;
    graph->SetDefaultGraphData();
  }
  if (activeConfig.TGtop==0)
  {
    vars->msaMode=modeSA;
    graph->SetDefaultGraphData();
  }

  if (vars->msaMode == modeScalarTrans || vars->msaMode == modeVectorTrans)
    GoTransmissionMode();
  else if (vars->msaMode == modeReflection)
    GoReflectionMode();
  else
    GoSAmode();

  hwdIf->autoWaitPrecalculate(); //auto wait calculations depend on mode ver116-1b
  graph->haltsweep=0; //So Restart will actually restart

}

void MainWindow::on_actionShow_Variables_triggered()
{
  Showvar();
}
void MainWindow::on_actionReference_Lines_triggered()
{
  referenceDialog ref(this);
  referDialog settings;
  settings.referenceColor1 = graph->referenceColor1;
  settings.referenceWidth1 = graph->referenceWidth1;

  settings.referenceColor2 = graph->referenceColor2;
  settings.referenceWidth2 = graph->referenceWidth2;

  settings.referenceColorSmith = graph->referenceColorSmith;
  settings.referenceWidthSmith = graph->referenceWidthSmith;

  settings.referenceLineType = graph->referenceLineType;
  settings.referenceTrace = graph->referenceTrace;
  settings.msaMode = vars->msaMode;
  settings.referenceDoMath = graph->referenceDoMath;
  settings.referenceOpA = graph->referenceOpA;
  settings.referenceOpB = graph->referenceOpB;
  settings.referenceLineSpec = graph->referenceLineSpec;
  QString dum1, dum2, aFormat;
  int dum3;
  graph->DetermineGraphDataFormat(vars->Y1DataType, dum1, dum2, dum3, aFormat);
  settings.Y1DataFormat = aFormat;
  graph->DetermineGraphDataFormat(vars->Y2DataType, dum1, dum2, dum3, aFormat);
  settings.Y2DataFormat = aFormat;
  settings.Y1DataType = vars->Y1DataType;
  settings.Y2DataType = vars->Y2DataType;

  ref.ReferenceDialog(&settings);

  if (ref.cancelled)
  {
    return;
  }
  graph->referenceColor1 = settings.referenceColor1;
  graph->referenceColor2 = settings.referenceColor2;
  graph->referenceWidth1 = settings.referenceWidth1;
  graph->referenceWidth2 = settings.referenceWidth2;
  graph->referenceColorSmith = settings.referenceColorSmith;
  graph->referenceWidthSmith = settings.referenceWidthSmith;

  graph->referenceLineType = settings.referenceLineType;
  graph->referenceTrace = settings.referenceTrace;
  vars->msaMode = settings.msaMode;

  graph->referenceDoMath = settings.referenceDoMath;
  graph->referenceOpA = settings.referenceOpA;
  graph->referenceOpB = settings.referenceOpB;
  graph->referenceLineSpec = settings.referenceLineSpec;

  if (ref.clearReferences)
  {
    graph->gClearAllReferences();
  }
  if (ref.createReferences)
  {
    graph->CreateReferenceSource();
  }
  graph->refreshGridDirty=1;
  graph->RefreshGraph(0);
}

void MainWindow::CloseSpecial(int returnflag)
{
  hwdIf->syncsweep = 0;
  hwdIf->setpdm = 0; //makes sure the PDM returns to automatic operation ver112-2a
  hwdIf->convdatapwr = 0;
  hwdIf->vnalintest = 0;
  hwdIf->test = 0;
  if (hwdIf->cftest == 1)
    hwdIf->CloseCavityFilterTest(); //cav ver116-4c
  //close #special:
  special = 0;     //close out Special Tests window
  RequireRestart();
  if (returnflag == 1)
    return;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
  static bool inHere = false;

  if (inHere)
  {
    qDebug() << "reentrant";
    return;
  }
  inHere = true;
  //ui->graphicsView->fitInView(0, 0, graph->getScene()->width(), graph->getScene()->height());
  ui->graphicsView->fitInView(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
  if (!vars->doingInitialization)
  {
    if (graph->haltsweep)
    {
      graph->continueCode=1;
      FinishSweeping();
    }
    graph->ResizeGraphHandler();
  }
  inHere = false;
}

void MainWindow::showEvent(QShowEvent *event)
{
  ui->graphicsView->fitInView(0, 0, graph->getScene()->width(), graph->getScene()->height());
  QWidget::showEvent(event);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
  static bool mouseDown = false;
  if (event->type() == QEvent::MouseMove)
  {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    if (mouseDown)
    {
     // graph->gDrawHaltPointerPix((mouseEvent->pos()).x()
//                                 , (mouseEvent->pos()).y());
      int xPix = mouseEvent->pos().x();
      int yPix = mouseEvent->pos().y();
      int pointNum;
      int traceNum;
      graph->gFindClickedPoint(xPix, yPix, pointNum, traceNum);
      statusBar()->showMessage(QString("%1,%2").arg(xPix).arg(yPix));
      gMouseQuery(xPix, yPix);
      //graph->gDrawHaltPointerPix(xPix, yPix);
    }
    else
    {
      QString global = QString("Mouse move (%1,%2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y());
      QString local = QString("%1,%2").arg(ui->graphicsView->mapFromGlobal(mouseEvent->pos()).x()).arg(ui->graphicsView->mapFromGlobal(mouseEvent->pos()).y());
      statusBar()->showMessage(global + " " + local);
    }
  }
  else if (event->type() == QEvent::MouseButtonPress)
  {
    mouseDown = true;
  }
  else if (event->type() == QEvent::MouseButtonRelease)
  {
    mouseDown = false;
  }
  return false;
}
void MainWindow::gMouseQuery(float x, float y) //Display info at mouse location
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  if (graph->haltsweep==1 || vars->isStickMode)
    return; //do nothing
  if (graph->gPixIsInGrid(x, y)==0)
  {
    return; //outside chart
  }
  int queryPointNum, gLastQueryTraceNum;

    int xx = x;
    int yy = y;
    graph->gFindClickedPoint( xx, yy, queryPointNum, gLastQueryTraceNum);  //Point numbers run 1...
    int roundedPointNum=int(queryPointNum+0.5);
    if (vars->msaMode==modeSA)
    {
      queryPointNum=roundedPointNum;  //integral points if SA mode
    }
    //If we are within one-half pixel of a integral point, we round off because it is nice to have integral points,
    //and there is no purpose to trying to get extreme resolution.
    if (abs(queryPointNum-roundedPointNum)*graph->gPixelsPerStep()<=0.5)
      queryPointNum=roundedPointNum;
    /*
    if (gLastQueryPointNum>0)
    {
        dum$=gDrawMarkerAtPointNum$(gLastQueryPointNum,"ALL","XOR","");    //Erase prior markers
        //if smithGraphHndl$()<>"" then call smithDrawSliderMarker gLastQueryPointNum   //erase in Smith chart too
    }
    dum$=gDrawMarkerAtPointNum$(queryPointNum,"ALL","XOR","");
    gLastQueryPointNum=queryPointNum; //draw marker
    message$=using("####.######", gGetPointXVal(queryPointNum)); " MHz"
    s1$="";
    s2$="";
    call gGetAxisLabels dum$, L1$, L2$ : call gGetAxisFormats dum$, form1$, form2$
    if Y1DataType<>constNoGraph then s1$=L1$;"=";uCompact$(uFormatted$(gGetPointYVal(queryPointNum,1), form1$))
    if Y2DataType<>constNoGraph then s2$=L2$;"=";uCompact$(uFormatted$(gGetPointYVal(queryPointNum,2), form2$))
    if primaryAxisNum=1 then s$= s1$;"    ";s2$ else s$= s2$;"    ";s1$
    message$=message$;"    ";s$ : call PrintMessage
                                  */
    /*
    if smithGraphHndl$()<>"" then
        call smithDrawSliderMarker queryPointNum   //draw in Smith chart too
        call smithClearReflectInfo  //Clear area where marker info will go
        if calInProgress=0 then call smithDisplayReflectInfo queryPointNum  //Data is garbage during cal
    end if */
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
  " menu of the main graph->";


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
  if (graph->haltsweep==1)
    FinishSweeping();     //Finish last point of sweep that was in progress. ver116-4j
  //if calManWindHndl$<>"" then wait    //Do nothing if cal mangager already running ver116-1b
  QString savePath=vars->path; //ver114-4c
  //calRunManager will return with filter path 1 installed.
  RequireRestart();     //SEW8 Let the user proceed only by Restarting
  hwdIf->calMan->calManRunManager(activeConfig.hasVNA);
  vars->path=savePath; //ver114-4c  //restores prior filter
  hwdIf->SelectFilter(hwdIf->filtbank);  //ver116-4j
}

void MainWindow::on_actionApperances_triggered()
{
  gridappearance->AppearanceShow(graph->gPrimaryAxis);
  graph->RedrawGraph(0);
}

void MainWindow::on_actionSave_Image_triggered()
{
  SaveImage();
}

void MainWindow::on_actionSave_Debug_Info_triggered()
{
  //Menu item to save debug data
  //Data is stored in a folder called "xxDebug" in the default directory
  if (graph->haltsweep==1)
  {
    FinishSweeping();
  }
  debug.DebugSaveData();
}

void MainWindow::on_btnContinue_clicked()
{
  Continue();
  /*
  QPainterPath path;
  QPen bluePen(Qt::blue);

  path.moveTo(70, graph->clientHeightOffset);
  for (int i = 0; i < 400; i++)
  {
    path.lineTo(i + 70, rand() % graph->gGridHeight + graph->clientHeightOffset );
  }

  QGraphicsPathItem *item = graph->getScene()->addPath(path);
  item->setPen(bluePen);
  QCoreApplication::processEvents();
  graph->getScene()->removeItem(item);
  item = graph->getScene()->addPath(path);
  item->setPen(QPen(Qt::red));
  */
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
  vars->thisstep = 0;
  QFile textFile(DefaultDir + "/data.txt");
  if (textFile.open(QFile::ReadOnly | QFile::Text))
  {
    //... (open the file for reading, etc.)
    QTextStream textStream(&textFile);
    while (true)
    {
      QString fullLine;
      QString line = textStream.readLine().trimmed();
      if (line.isNull())
          break;
      if (line.isEmpty())
      {
        continue;
      }
      else
      {
        QStringList list;
        list = line.split(QRegExp("\\s+"));
        if (list.count() != 5)
        {
          QMessageBox::warning(this, "Dud data", "line" + QString::number(vars->thisstep));
          textFile.close();
          return;
        }
        graph->gGraphVal[vars->thisstep][0] = list.at(1).toFloat();
        vars->datatable[vars->thisstep][2] = list.at(2).toFloat();
        vars->magarray[vars->thisstep][3] = list.at(3).toFloat();
        vars->freqCorrection[vars->thisstep] = list.at(4).toFloat();

        vars->thisstep++;
      }
    }
    textFile.close();
  }
}

void MainWindow::on_btnRestart_clicked()
{
  if (graph->haltsweep == 1)
  {
    //Halted();
    graph->continueCode=1;
    return;
  }
  QTimer::singleShot(0, this, SLOT(Restart()));
}

void MainWindow::on_btnRedraw_clicked()
{
  //Redraw button was pushed
  //haltsweep=1 if scan is in progress, so we set flag to halt sweeping on return from the "scan"
  //command that enabled this button to be handled.
  if (graph->haltsweep==1)
  {
    graph->continueCode=1;
    return;   //Signal to halt after "scan" command
  }
  graph->mDeleteMarker("Halt");    //Delete Halt marker ver114-4c
  if (smithGraphHndl()!="")
  {
    //smithDrawChart();    //To recreate bitmap of background, just in case it is messed up ver115-2c
  }
  graph->refreshGridDirty=1;
  graph->RefreshGraph(0);
}

void MainWindow::on_btnExpandLR_clicked()
{
  graph->gDrawMarkerPix("INVERTEDWEDGE", "A", 100, 100);
  graph->gDrawMarkerPix("LABELEDINVERTEDWEDGE", "B", 100, 150);
  graph->gDrawMarkerPix("LABELEDWEDGE", "C", 100, 200);
  /*graph->gDrawWedgePix(100,100);
  graph->gDrawInvertedWedgePix(100,150);
  graph->gDrawSmallInvertedWedgePix(100,200);
  graph->gDrawHaltPointerPix(100,250);*/
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
  OneStep();
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

  QString fileName = util.uSaveFileDialog(this, filter, defaultExt, initialDir, initialFile, "Save Preference File");
  if (fileName!="")
  {
    SavePreferenceFile(fileName); //blank means cancelled
  }
}

void MainWindow::on_actionLoad_Prefs_triggered()
{
  //Load user-specified preferences file.
  if (graph->haltsweep==1)
    FinishSweeping();   //ver115-1e


  QString filter="Text files (*.txt);;"
      "All files (*.*)";
  QString defaultExt="txt";
  QString initialDir= DefaultDir+"/MSA_Info/MSA_Prefs/";
  QString initialFile="Prefs.txt";
  QString fileName = util.uOpenFileDialog(this, filter, defaultExt, initialDir, initialFile, "Load Preference File");

  if (fileName!="") //Blank means cancelled
    LoadPreferenceFile(fileName);
}

void MainWindow::on_actionSave_Data_triggered()
{

}
void MainWindow::CreateGraphWindow()
{
//We do this only once, at startup. After that, we work with the existing window, adjust its menus and redraw.
  resize(800,600);

  ConformMenusToMode(); //Hide whatever menu items we don't need for current msaMode$
  //Tell graph module what size we are, and calculate scaling ver114-6f
  graph->gUpdateGraphObject(graph->currGraphBoxWidth, graph->currGraphBoxHeight,
                            graph->graphMarLeft, graph->graphMarRight, graph->graphMarTop, graph->graphMarBot);
  ui->graphicsView->fitInView(graph->getScene()->sceneRect());
  graph->gCalcGraphParams();   //Calculate new scaling. May change min or max.
  float xMin, xMax;
  graph->gGetXAxisRange(xMin, xMax);
  xMin = 1;
  xMax = 2;
  if (vars->startfreq != xMin || vars->endfreq != xMax)
  {
    graph->SetStartStopFreq(xMin, xMax);
  }
}

void MainWindow::StartSweep()
{
  //14.[StartSweep]//Begin sweeping from step 0
  //StartSweep begins the outer loop that repeats the entire scan process until halted.
  //The scan loop continues until a user action which aborts the scan, or in the case of
  //OneStep it continues only for a single point. If specialOneSweep=1 or HaltAtEnd=1, it
  //automatically stops at the end of a single sweep.
  //[StartSweep]//enters from above, or [IncrementOneStep]or[FocusKeyBox]([OneStep][Continue])

  //bool dostart = true;

  //while(true)
//  {
//    if (dostart)
    {
      if (vars->specialOneSweep)
      {
        graph->haltAtEnd=1;
      }
      else
      {
        graph->haltAtEnd=0;
      }
      if (vars->haltedAfterPartialRestart==0 && hwdIf->scanResumed==1)
      {
        //For a resumed scan, a halt occurred after the previous step and that step was fully processed.
        //haltsweep will equal 0. If alternateSweep=1 and the halt occurred at the end of a sweep, we need to
        //repeat the last point as the first point of the new sweep. But in the case where we are continuing
        //after a halt resulting from partial restart, we returned before the first step was taken and need to
        //start with that step.
        graph->mDeleteMarker("Halt");
        if (vars->thisstep == vars->sweepStartStep && hwdIf->syncsweep == 1)
        {
          hwdIf->SyncSweep();
        }
        if (vars->alternateSweep==0 || hwdIf->haltWasAtEnd==0)   //ver114-5c Go to next step unless we need to repeat this one
        {
          if (vars->sweepDir==1)
          {
            if (vars->thisstep<vars->sweepEndStep)
              vars->thisstep = vars->thisstep + 1;
            else
              vars->thisstep=vars->sweepStartStep;
          }
          else
          {
            if (vars->thisstep > vars->sweepEndStep)
              vars->thisstep = vars->thisstep - 1;
            else
              vars->thisstep=vars->sweepStartStep;
          }
        }
      }
      else
      {
        vars->thisstep=vars->sweepStartStep;
      }

      vars->haltedAfterPartialRestart=0; //Reset. Will stay zero until next partial restart. 116-1b
      hwdIf->scanResumed=0;   //Reset flag

      //dostart = false;
    }
    QTimer::singleShot(0, this, SLOT(CommandThisStep()));
}
void MainWindow::CommandThisStep()
{
  //15.[CommandThisStep]. command relevant Control Board and modules
  //SEW CommandThisStep begins the inner loop that moves from step to step to complete a single
  //SEW scan.This branch label is accessed only from the end of the loop.
  //[CommandThisStep]//needs:thisstep ; commands PLL1,DDS1,PLL3,DDS3,PDM
  //a. first, check to see if any or all the 5 module commands are necessary [DetermineModule]
  //b. calculate how much delay is needed for each module[DetermineModule], but use only the largest one[WaitStatement].
  //c. send individual data, clocks, and latch commands that are necessary for[CommandOrigCB]
  //or for SLIM, use [CommandAllSlims] for commanding concurrently
  hwdIf->CommandCurrentStep(vars->thisstep);
  if (vars->thisstep == 0)
  {
    // give the first step extra time to settle
    util.uSleep(150);
  }
  //16.Determine sequence of operations after commanding the modules
  if (hwdIf->onestep == 1)   //in the One Step mode
  {
    hwdIf->glitchhlt = 10; //add extra settling time
    hwdIf->ReadStep(); //read this step
    ProcessAndPrint(); //process and print this step
    DisplayButtonsForHalted();

    graph->mAddMarker("Halt", vars->thisstep+1, "1");
    //If marker is shown on graph, we need to redraw the whole graph
    //Otherwise just redraw the marker info
    if (graph->doGraphMarkers)
    {
      graph->RefreshGraph(0);
    }
    else
    {
      graph->mDrawMarkerInfo();  //No erasure gap in redraw ver114-5m
    }
    if (vars->thisstep == vars->sweepEndStep)
    {
      //Note reversal is after graph is redrawn
      if (vars->alternateSweep)
      {
        ReverseSweepDirection();
      }
      hwdIf->haltWasAtEnd=1;
    }
    else
    {
      hwdIf->haltWasAtEnd=0;
    }
    return;
  }

  if (graph->haltsweep == 0)  //in first step after a Halt
  {
    graph->haltsweep = 1; //change flag to say we are not in first step after a Halt, for future steps
    hwdIf->glitchhlt = 10;  //add extra settling time
    hwdIf->ReadStep(); //read this step
  }
  else  //if in middle of sweep. process and print the previous step, then read this step
  {
    hwdIf->ProcessAndPrintLastStep();
    hwdIf->ReadStep();//read this step
  }
  //print sweep time after any refresh action from the prior scan
  if (vars->thisstep == vars->sweepStartStep)
  {
    int currTime = util.time("ms").toInt();
    if (hwdIf->suppressSweepTime == 0)
    {
      if (graph->displaySweepTime)
      {
        graph->PrintMessage("Sweep Time="+util.usingF("####.##", (currTime-hwdIf->startTime)/1000.0)+" sec.");
      }
      ui->lineEdit_SweepTime->setText(util.usingF("####.##", (currTime-hwdIf->startTime)/1000.0)+" s");
    }
    hwdIf->suppressSweepTime=0; //Only suppress on first scan
    hwdIf->startTime=currTime;        //timer for testing
  }
  int action = PostScan();
  if (action == doHalt)
  {
    Halted();
    return;
  }
  else if (action == doWait)
  {
    return;
  }
  else if (action == doRestart)
  {
    QTimer::singleShot(0, this, SLOT(Restart()));
    return;
  }
  //}
  //void MainWindow::IncrementOneStep()
  //{
  //18.[IncrementOneStep]
  //SEW IncrementOneStep is the end of both the inner loop over points and the outer loop
  //SEW over scans. goto [CommandThisStep] continues the inner loop with the next point.
  //SEW goto[StartSweep] continues the outer loop with the next scan.
  //SEW [IncrementOneStep] is commented out to be clear it is not used for any goto.
  //[IncrementOneStep]

  if (vars->thisstep == vars->sweepEndStep && hwdIf->syncsweep == 1)
  {
    hwdIf->SyncSweep();
  }
  //ver114-5a modified the following
  if (vars->sweepDir==1)   //ver114-4k added this block to handle possible reverse sweeps
  {
    if (vars->thisstep < vars->sweepEndStep)
    {
      vars->thisstep = vars->thisstep + 1;
      QTimer::singleShot(0, this, SLOT(CommandThisStep()));
      return;
    }
  }
  else
  {
    if (vars->thisstep > vars->sweepEndStep)
    {
      vars->thisstep = vars->thisstep - 1;
      QTimer::singleShot(0, this, SLOT(CommandThisStep()));
      return;
    }
  }
  //If we are here, we have just read the final step of a sweep

  if (graph->haltAtEnd==0)
  {
    //Alternate sweep directions if required. When we switch direction, thisstep
    //was the final point of one sweep and becomes the first point of the next.
    //We process and print it  immediately as the last point of this sweep; then reverse
    //direction and start with the same point. To avoid re-processing it at the next step we
    //set haltsweep=0.
    if (vars->alternateSweep)
    {
      ProcessAndPrint();
      ReverseSweepDirection();
      graph->haltsweep=0;
    }
    QTimer::singleShot(0, this, SLOT(StartSweep()));
    return;
  }
  Halted();
}
int MainWindow::PostScan()
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
  QApplication::processEvents();
  if (graph->continueCode!=0)  // =0 means continue normally
  {
    if (graph->continueCode==1)
    {
      graph->continueCode=0;
      return doHalt;
    }   //=1 means halt immediately
    if (graph->continueCode==2)
    {
      graph->continueCode=0;
      graph->haltsweep=0;
      return doWait;
    }     //=2 means wait immediately
    graph->continueCode=0;
    graph->haltsweep=0;
    return doRestart;
  }
  return doNothing;
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
}

void MainWindow::FinishSweeping()
{
  //created FinishSweeping;
  //ver114-6e split the non-graphing cleanup into [CleanupAfterSweep]
  //0 Do cleanup to end sweeping but return for further actions
  //This is a modified version of the former [Halted], without the wait at the end
  ProcessAndPrint();//process, print to the computer monitor ver111-22
  if (graph->haltAtEnd==0)
    graph->mAddMarker("Halt", vars->thisstep+1, "1"); //Add Halt marker ver114-4d
  graph->haltsweep=0; //do now so RefreshGraph will "flush" ver115-1a
  if (vars->isStickMode==0)
  {
    if (graph->refreshOnHalt)
    {
      graph->refreshGridDirty=1;
      graph->RefreshGraph(1);  //redraw and show erasure gap; don't do if stick mode ver114-7d
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
    util.beep();
    graph->PrintMessage("Sweep Aborted");
  }
  else
  {
    if (vars->calInProgress)
    {
      util.beep();
      graph->PrintMessage("Calibration Complete");
    }
  }
  //test is used for troubleshooting. Coder can insert
  //test = (any variable) anywhere in the code, and it will get displayed in the Messages Box during Halt.
  if (vars->test!=0)
  {
    graph->PrintMessage(QString::number(vars->test));
  }
  //Alternate sweep directions if required; added by ver114-5a
  if (vars->thisstep==vars->sweepEndStep)
  {
    if (vars->alternateSweep)
    {
      ReverseSweepDirection();
    }
  }
  CleanupAfterSweep();
}
void MainWindow::ReverseSweepDirection()
{
  //[ReverseSweepDirection] //Reverse direction of sweep
  //This is called after sweepEndStep has been fully processed, but only if alternateSweep=1
  if (vars->sweepDir==1)
  {
    vars->sweepDir=-1;
    vars->sweepStartStep=vars->steps;
    vars->sweepEndStep=0;
  }
  else
  {
    vars->sweepDir=1;
    vars->sweepStartStep=0;
    vars->sweepEndStep=vars->steps;
  }
  graph->gSetSweepDir(vars->sweepDir); //Notify graph module of new direction
}

void MainWindow::ProcessAndPrint()
{
//process and print "thisstep"
//SEW3 changed the next few lines to have phase degrees adjusted for phase-change-over-signal-level
//The calculation of the phase adjustment, difPhase, is made in ConvertMagPhaseData (formerly ConvertMagData).
//That correction is then added to phase in ConvertPhadata.Note that ConvertPhadata must now be
//called after ConvertMagPhaseData so difPhase is valid when ConvertPhadata is executed.
  hwdIf->ConvertMagPhaseData(); //convert magdata (bits read) to magpower (dBm)
  if (vars->msaMode!=modeSA)
  {
    //convert phadata (bits read) to phase (degrees) if we have phase, but not for special graphs, which set phase directly
    if (vars->msaMode!=modeScalarTrans && vars->doSpecialGraph==0)
      hwdIf->ConvertPhadata();
    hwdIf->ProcessDataArrays();   //Enter data in S21DataArray or ReflectArray
  }
  graph->PlotDataToScreen();
  return; //from [ProcessAndPrint]

}

void MainWindow::PrintMessage(QString message)
{
  graph->PrintMessage(message);
}

void MainWindow::Showvar()
{
  QString var = "";
  if (!showVars)
  {
    showVars = new dialogShowVars(this);
  }
  showVars->move(x() + width() + 3, y());
  showVars->show();
  updatevar(vars->thisstep);

  if (graph->haltsweep==1)
  {
    PostScan();
  }
}

void MainWindow::updatevar(int step)
{
  if (!showVars)
  {
    return;
  }
  if (!showVars->isVisible())
  {
    return;
  }

  QStringList values;
  values << QString("this step = %1").arg(step);
  values << QString("dds1output = %1 MHz").arg(hwdIf->dds_1[step].freq, 5,'f',8,'0');
  values << QString("LO 1 = %1 MHz").arg(hwdIf->pll_1[step].freq, 5,'f',8,'0');  // PLL1array[step][43]);
  values << QString("pdf1 = %1 MHz").arg(hwdIf->pll_1[step].pdf, 5,'f',8,'0');  // vars->PLL1array[step][40]);
  values << QString("ncounter1 = %1").arg(hwdIf->pll_1[step].ncounter);  // vars->PLL1array[step][45]);
  values << QString("Bcounter1 = %1").arg(hwdIf->pll_1[step].Bcounter );  // vars->PLL1array[step][48]);
  values << QString("Acounter1 = %1").arg(hwdIf->pll_1[step].Acounter);  // vars->PLL1array[step][47]);
  values << QString("fcounter1 = %1").arg(hwdIf->pll_1[step].fcounter);  // vars->PLL1array[step][46]);
  values << QString("rcounter1 = %1").arg(hwdIf->rcounter1);
  values << QString("LO2 = %1 MHz").arg(hwdIf->LO2, 5,'f',8,'0');
  values << QString("pdf2 = %1 MHz").arg(hwdIf->pdf2, 5,'f',8,'0');
  values << QString("ncounter2 = %1").arg(hwdIf->ncounter2);
  values << QString("Bcounter2 = %1").arg(hwdIf->Bcounter2);
  values << QString("Acounter2 = %1").arg(hwdIf->Acounter2);
  values << QString("rcounter2 = %1").arg(hwdIf->rcounter2);
  values << QString("LO3 = %1 MHz").arg(hwdIf->pll_3[step].freq, 5,'f',8,'0');// vars->PLL3array[step][43]);
  values << QString("pdf3 = %1 MHz").arg(hwdIf->pll_3[step].pdf, 5,'f',8,'0');
  values << QString("ncounter3 = %1").arg(hwdIf->pll_3[step].ncounter);
  values << QString("Bcounter3 = %1").arg(hwdIf->pll_3[step].Bcounter);
  values << QString("Acounter3 = %1").arg(hwdIf->pll_3[step].Acounter);
  values << QString("fcounter3 = %1").arg(hwdIf->pll_3[step].fcounter);
  values << QString("rcounter3 = %1").arg(hwdIf->rcounter3);
  values << QString("dds3output = %1").arg(hwdIf->dds_3[step].freq, 5,'f',8,'0');
  values << QString("Magdata= %1").arg(vars->magarray[step][3]);
  values << QString("magpower=%1").arg(vars->datatable[step][2]);  //raw magdata bits, MSA input power(massaged)
  values << QString("Phadata = %1").arg(vars->phaarray[step][3]);
  values << QString("PDM = %1").arg(vars->phaarray[step][4]);
  values << QString("Real Final I.F. = %1")
            .arg(hwdIf->LO2 - (hwdIf->pll_1[step].fcounter * hwdIf->dds_1[step].freq / hwdIf->rcounter1) + vars->datatable[step][1], 5,'f',5,'0');
  values << QString("glitchtime = %1").arg(vars->glitchtime);


  showVars->update(values);
}



void MainWindow::Restart()
{
  graph->haltsweep=0;
  //When a window is closed, all graphics drawing operations are deleted from memory
  //Auxiliary graph data is computed, and does not survive the generation of new data.
  //So if aux data is currently graphed, we turn it off.
  if (vars->Y1DataType>=constAux0 && vars->Y1DataType<=constAux5)
  {
    vars->Y1DataType=constNoGraph;
  }
  if (vars->Y2DataType>=constAux0 && vars->Y2DataType<=constAux5)
  {
    vars->Y2DataType=constNoGraph;
  }
  if (vars->Y1DataType==constNoGraph && vars->Y2DataType==constNoGraph)
  {
    graph->SetDefaultGraphData();    //So we have something to graph
  }
  hwdIf->ClearAuxData(); //Indicate aux data not valid by clearing graph names
  hwdIf->onestep = 0;

  DisplayButtonsForRunning();   //SEW8 replaced print #main.restart, "Running"

  //1 Start new sweep series.
  //Reinitialize hardware every time
  if (vars->suppressHardwareInitOnRestart)
  {
    vars->suppressHardwareInitOnRestart=0; //Clear flag; we only skip initialization for one restart after flag is set.
    setCursor(Qt::WaitCursor);
    SkipHardwareInitialization();
    setCursor(Qt::ArrowCursor);

    return;
  }
  else
  {
    setCursor(Qt::WaitCursor);
    hwdIf->InitializeHardware();
    setCursor(Qt::ArrowCursor);
    return;
  }
}
void MainWindow::SkipHardwareInitialization()    //Skips to here if there is no hardware (suppressHardware=1)
{
  //11.[BeginScanSeries] get info from windows and update variables
  //[BeginScanSeries]   //Start a new series of scans, which requires some initialization

  //12.[InitializeGraphModule]
  vars->suppressPDMInversion=0;  //ver115-1a
  //ver115-8d moved test for specialOneSweep to step 14
  graph->UpdateGraphParams();  //Update graph module for any changes made by the user
  graph->firstScan=1;     //Signal that the next scan is the first after Restart
  //ver114-5f moved some items to UpdateGraphParams

  //Note x values must be calculated first (in [UpdateGraphParams]) ; modVer115-1c
  //If calInProgress=1, InstallSelectedxxx will just set applyCal=0 and installed base steps=-1
  if (vars->msaMode==modeReflection)
  {
    hwdIf->oslCal.InstallSelectedOSLCal();
  }
  else
  {
    if (vars->msaMode!=modeSA)
      vnaCal.InstallSelectedLineCal(graph->gGraphVal, graph->gNumPoints, graph->gGetXIsLinear());
  }
  vars->cycleNumber=1;
  gridappearance->gSetTraceColors(gridappearance->cycleColorsAxis1[0],gridappearance->cycleColorsAxis2[0]);
  QColor xText, dum1, dum2, gridText;

  gridappearance->gGetTextColors(xText, dum1, dum2, gridText);
  gridappearance->gSetTextColors(xText, gridappearance->cycleColorsAxis1[0],gridappearance->cycleColorsAxis2[0], gridText);    //match text to trace ver116-4s
  vars->doCycleTraceColors=0;    //start with cycling off. No preference file item for this.

  graph->gInitDynamicDraw();   //Set up for first scan of dynamic draw/erase/redraw...
  graph->ImplementDisplayModes();  //Done in [UpdateGraphParams] but gInitDynamicDraw overrode it

  //In multiscan, we don't want to update the time stamp on every redraw, which sometimes happens without scanning.
  if (vars->multiscanIsOpen==0 || vars->multiscanInProgress==1)
  {
    vars->restartTimeStamp=QDateTime::currentDateTime().toString("MM/dd/yy; hh:mm:ss"); //date$("mm/dd/yy"); "; ";time$()
    graph->gSetTitleLine(2, vars->restartTimeStamp);    //Put date and time in line 3 of title
    if (graph->gGetXIsLinear())
      graph->gSetTitleLine(3, "MSA Linear Sweep " + vars->path);
    else
      graph->gSetTitleLine(3, "MSA Log Sweep "+vars->path);      //Save linear/log and path info
  }

  //For multiscan, the redraw of the background is done prior to scanning via [PartialRestart], and on refresh
  if (vars->multiscanInProgress==0)
  {
    //Redraw background stuff on first scan of a series. ver115-8d
    //ver115-8d deleted calc of centerstep, which is no longer used
    graph->gDrawGrid();      // Clear graphics area and draw the background grid and labels. Wipes out all prior flushes.
    graph->DrawSetupInfo();    // Draw info describing the sweep setup

    /*
      //fix me no smith chart yet
      if (smithGraphHndl()!="")   //ver115-1b draw smith chart if we have one ver115-1e
      {
        //smithRedrawChart(); //Draw blank chart ver115-2c
      }
*/
    if (graph->referenceLineType!=0)    //Draw reference lines ver114-8a
    {
      if (graph->referenceLineType>1)
        graph->CreateReferenceSource();  //RLC or fixed value
      graph->CreateReferenceTransform();   //Generate actual reference graph data
      graph->gClearAllReferences();
      QColor refHeadingColor1;
      QColor refHeadingColor2;
      if (graph->referenceDoMath==0)    //don't draw ref if we are using ref for math
      {
        if (graph->referenceTrace & 2)
        {
          graph->gAddReference(1,graph->CreateReferenceTraces(graph->referenceColor2,graph->referenceWidth2,2,&graph->refLine[2]));  //Do Y2 reference
        }
        if (graph->referenceTrace & 1)
        {
          graph->gAddReference(2,graph->CreateReferenceTraces(graph->referenceColor1,graph->referenceWidth1,1,&graph->refLine[1])); //Do Y1 reference
        }
        graph->gDrawReferences();
        refHeadingColor1=graph->referenceColor1;
        refHeadingColor2=graph->referenceColor2;
      }
      else
      {
        gridappearance->gGetTraceColors(refHeadingColor1, refHeadingColor2); //Use trace colors for "REF" if math is used
      }
      graph->PrintReferenceHeading();  //Print above axis to indicate which line matches which axis //ver115-5d
    }
  }

  //fix me
  //useExpeditedDraw=gCanUseExpeditedDraw();   ; For normal SA use, [gDrawSingleTrace] will be used.
  //ver115-1a deleted printing of glitchtime
  vars->doingInitialization=0;   //We are done with initialization on startup

  if (vars->calInProgress==1)
  {
    graph->PrintMessage("Calibration in progress.");
  }
  else
  {
    graph->PrintMessage("");
  }
  if (vars->msaMode==modeSA && vars->gentrk==0 && vars->multiscanInProgress==0)
  {
    if ((vars->endfreq-vars->startfreq)/vars->steps >activeConfig.finalbw/1000)      //compare as MHz
    {
      graph->PrintMessage("Frequency step size exceeds RBW; signals may be missed.");
    }
  }

  //13.Calculate the command information for first step through last step of the sweep and put in arrays

  //ver116-4s changed this so datatable  and phaarray are set up here whether or not suppresshardware=1.
  for (int i=0; i < vars->steps;i++)
  {
    float thisfreq=graph->gGetPointXVal(i+1);    //Point number is 1 greater than step number SEWgraph
    if (vars->msaMode!=modeSA)   //Store actual signal freq in VNA arrays
    {
      if (vars->msaMode!=modeReflection)
        vars->ReflectArray[vars->thisstep][0]=thisfreq;
      else
        vars->S21DataArray[vars->thisstep][0]=thisfreq;
    }
    int thisBand;
    if (vars->freqBand==0)
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
    if (thisBand!=1)
    {
      thisfreq=hwdIf->Equiv1GFreq(thisfreq, thisBand);  //Convert from actual freq to equivalent 1G frequency ver116-4s
    }
    vars->datatable[i][0] = vars->thisstep;    //put current step number into the array, row value= thisstep //moved ver111-18
    vars->datatable[i][1] = thisfreq;
    vars->datatable[i][4] = thisBand;
    vars->phaarray[i][0] = 0;   //pdm state
  }
  if (vars->suppressHardware==0)    //Do these only if we are using the hardware
  {
    hwdIf->CalculateAllStepsForLO1Synth();
    if (activeConfig.TGtop > 0)
      hwdIf->CalculateAllStepsForLO3Synth();
    hwdIf->CreateCmdAllArray();
  }
  CalcFreqCorrection();     //Calculate power correction at each frequency SEWgraph1
  if (vars->msaMode==modeSA && vars->frontEndActiveFilePath!="")
    hwdIf->frontEndInterpolateToScan();  //Calculate corrections for front end ver115-9d
  graph->continueCode=0;     //Set to other values by subroutines to cause halt, wait or restart

  vars->haltedAfterPartialRestart=0; //May get set to 1 a few lines below. 116-1b
  //ver114-6e Normally, refresh will occur at end of scan only if halted or refreshEachScan=1,
  //and will be done by expedited methods. But if the user makes certain changes, the following
  //variables are used to force more extensive redrawing.
  graph->mDeleteMarker("Halt");    //ver114-4h moved the -4d version
  hwdIf->suppressSweepTime=1;     //to suppress it for the first scan ver114-4h
  //if we just want to go through the initialization procedure we set returnBeforeFirstStep
  //and invoke [Restart] with a gosub; here we return to the caller

  //Save some sweep settings for reflection and transmission for use when changing
  //back to a previously used mode, so we know the nature of the last gathered data
  if (vars->msaMode==modeReflection)    //ver116-1b
  {
    vars->refLastSteps=vars->steps;
    vars->refLastStartFreq=vars->startfreq;
    vars->refLastEndFreq=vars->endfreq;
    vars->refLastIsLinear=graph->gGetXIsLinear();
    vars->refLastGraphR0=vnaCal.S11GraphR0;
    vars->refLastY1Type=vars->Y1DataType;
    vars->refLastY1Top=vars->Y1Top;
    vars->refLastY1Bot=vars->Y1Bot;
    vars->refLastY1AutoScale=graph->autoScaleY1;
    vars->refLastY2Type=vars->Y2DataType;
    vars->refLastY2Top=vars->Y2Top;
    vars->refLastY2Bot=vars->Y2Bot;
    vars->refLastY2AutoScale=graph->autoScaleY2;
    for (int i=1; i <= 4; i++)
    {
      vars->refLastTitle[i]=graph->gGetTitleLine(i);
    }
  }
  else
  {
    if (vars->msaMode==modeVectorTrans)
    {
      vars->transLastSteps=vars->steps ;
      vars->transLastStartFreq=vars->startfreq ;
      vars->transLastEndFreq=vars->endfreq ;
      vars->transLastIsLinear=graph->gGetXIsLinear();
      vars->transLastGraphR0=vnaCal.S21JigR0;
      vars->transLastY1Type=vars->Y1DataType ;
      vars->transLastY1Top=vars->Y1Top ;
      vars->transLastY1Bot=vars->Y1Bot ;
      vars->transLastY1AutoScale=graph->autoScaleY1;
      vars->transLastY2Type=vars->Y2DataType ;
      vars->transLastY2Top=vars->Y2Top ;
      vars->transLastY2Bot=vars->Y2Bot ;
      vars->transLastY2AutoScale=graph->autoScaleY2;
      for (int i=1; i <= 4; i++)
      {
        vars->transLastTitle[i]=graph->gGetTitleLine(i);
      }
    }
  }

  if (vars->returnBeforeFirstStep)
  {
    vars->thisstep=vars->sweepStartStep;
    vars->returnBeforeFirstStep=0;
    vars->haltedAfterPartialRestart=1;
    CleanupAfterSweep();
    return;
  }
  updateView();
  QTimer::singleShot(0, this, SLOT(StartSweep()));
}

void MainWindow::LoadDataFileWithContext(QString dataFileName)
{
  //Load data from restoreFileName$, which is already open as restoreFileHndl$. We close it when done
  //Set restoreErr$ to error message or blank.
  //The data may be preceded by preference info, so we read and react to that first ver115-8c
  //Want baseFrequency=0 unless explicitly changed by loading a context.
  vars->baseFrequency=0;
  //RememberState();  //So we can see what changed
  LoadBasicContextsFromFile();   //Load preferences from restoreFileHndl$
  if (vars->restoreErr!="")
  {
    QMessageBox::warning(0, "Error", "Error loading file: "+vars->restoreErr);
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
    DetectFullChanges();
    if (graph->continueCode==3)
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
    vars->restoreFileHndl=hwdIf->touch.touchOpenInputFile(vars->restoreFileName);
    if (vars->restoreFileHndl==NULL)
    {
      vars->restoreErr="File failed to open: "+dataFileName;
      return;
    }
  }

  hwdIf->touch.touchReadParams(vars->restoreFileHndl,1);  //Read data from file into uWorkArray
  vars->restoreFileHndl->close();
  delete vars->restoreFileHndl;

  if (hwdIf->touch.touchBadLine>0)
  {
    QMessageBox::warning(0, "Error", "File Error in Line "+QString::number(hwdIf->touch.touchBadLine));
    return;
  }   //touchReadParams sets touchBadLine if error
  if (uWork.uWorkNumPoints<2)
  {
    QMessageBox::warning(0, "Error", "File must contain two or more points");
    return;
  }
    //If the file had more points than our arrays can handle, then resize them.
    //Note this does not resize uWorkArray, which would erase its data.
  if (uWork.uWorkNumPoints>=graph->gMaxNumPoints())
  {
    ResizeArrays(uWork.uWorkNumPoints+10);
  }
    //Note that we don//t make the plane extension or R0 adjustments that [ProcessDataArrays] makes. If we
    //have loaded the context with the data, we presume the data already is adjusted per the context. If we
    //did not load the context, we will force plane ext and R0 to avoid need for adjustments.
  if (contextLoaded==0)
  {
    vars->planeadj=0;
    vnaCal.S21JigR0=hwdIf->touch.touchRef;
    if (vars->msaMode==modeReflection)
      vnaCal.S11BridgeR0=hwdIf->touch.touchRef;
  }

  //The data is now in uWorkArray(1,x) to uWorkArray(uWorkNumPoints, x)
  for (int i=1; i <= 4; i++)    //Get title from comments; line 4 is non-printing
  {
    if (i<=hwdIf->touch.touchCommentCount)
      graph->gSetTitleLine(i, hwdIf->touch.touchComments[i]);
    else
      graph->gSetTitleLine(i, "");
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
  vars->VNADataZ0=hwdIf->touch.touchRef;  //Determined from file   //ver116-4a
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

  vars->VNARestoreDoR0AndPlaneExt=0;
  RestoreVNAData();   //load and graph the data ver116-4j

    //Restart puts new time stamp in title, so put ours back in
  if (hwdIf->touch.touchCommentCount>2)
  {
    vars->restartTimeStamp=hwdIf->touch.touchComments[3];
    graph->gSetTitleLine(3, "    "+vars->restartTimeStamp+"    ");   //Extra blanks for clearing in case size changed
  }
  else
  {
    graph->gSetTitleLine(3, "");
  }

  //Reprint title without clearing; new data will cover old data
  //The refresh routine will have used saved info for faster drawing, and so will
  //have the time stamp from PartialRestart done in [LoadDataFromWorkArray], which we don//t want.
  graph->gPrintTitle(0);

  //#graphBox$, "flush"
  vars->VNAData.mresize(2,3);  //to save space
  vars->VNADataNumSteps=1;
  util.beep();
}
void MainWindow::ResizeArrays(int nPoints)
{
  //Resize the arrays to hold at least nPoints points
  int maxPoints=qMax(nPoints+10, 802);
  if (graph->gMaxNumPoints()<maxPoints) //Redimension only if nPoints goes up ver114-5p
  {
    graph->gSetMaxPoints(maxPoints);     //Resize arrays in graph module

//    hwdIf->pll_1.resize(maxPoints);
//    hwdIf->pll_3.resize(maxPoints);
    vars->datatable.mresize(maxPoints,5);    //added element for band ver116-4s
    vars->magarray.mresize(maxPoints,4);
    vars->phaarray.mresize(maxPoints,5);
    vars->lineCalArray.mresize(maxPoints,3);
    graph->referenceSource.mresize(maxPoints, 3);
    graph->referenceTransform.mresize(maxPoints, 3);
    //vars->PLL1array.mresize(maxPoints,49);
    //vars->PLL3array.mresize(maxPoints,49);
    //vars->DDS1array.mresize(maxPoints,47);
    //vars->DDS3array.mresize(maxPoints,47);
    hwdIf->dds_1.resize(maxPoints);
    hwdIf->dds_3.resize(maxPoints);


    //vars->cmdallarray.mresize(maxPoints,40);
    //hwdIf->usb->resizeMemory(maxPoints);

    vars->resizeArrays(maxPoints);

    vars->freqCorrection.resize(maxPoints);
    vars->frontEndCorrection.resize(maxPoints);
    vars->ReflectArray.mresize(maxPoints,17);
    vars->S21DataArray.mresize(maxPoints, 4);
    vars->bandLineCal.mresize(maxPoints, 3);
    vars->OSLa.mresize(maxPoints, 2);
    vars->OSLb.mresize(maxPoints, 2);
    vars->OSLc.mresize(maxPoints, 2);
    vars->OSLstdOpen.mresize(maxPoints,2);
    vars->OSLstdLoad.mresize(maxPoints,2);
    vars->OSLcalOpen.mresize(maxPoints,2);
    vars->OSLcalLoad.mresize(maxPoints,2);
    vars->OSLcalShort.mresize(maxPoints,2);
    vars->OSLBandA.mresize(maxPoints,2);
    vars->OSLBandB.mresize(maxPoints,2);
    vars->OSLBandC.mresize(maxPoints,2);
    vars->OSLBandRef.mresize(maxPoints,3);
    vars->auxGraphData.mresize(maxPoints, 6);

    //Note we do not resize arrays for base Line or base OSL cal, because resizing will invalidate the data, and
    //because base line cal is saved to/retrieved from a file so its max size needs to be known before retrieval.
    //Plus the nature of base cal does not require a massive number of points.
    inter.intSetMaxNumPoints(maxPoints);

    //ver115-1d put the loading of the cal file inside the if.. block
    //Load BaseLine Cal file if it exists ver114-5m
    if (activeConfig.TGtop>0)
    {
      vnaCal.CreateOperatingCalFolder();  //Create OperatingCal folder if it does not exist
      vnaCal.LoadBaseLineCalFile();       //Load BaseLine file if it exists; if not we don't care
      vnaCal.bandLineNumSteps=-1; //Indicate no data; we just erased it
      vnaCal.InstallSelectedLineCal(graph->gGraphVal, graph->gNumPoints, graph->gGetXIsLinear());     //To restore line cal data
    }
  }
  //Note we don't have to resize configarray or configLineCalPoints$(); a flexible number of points
  //is handled by simply having the strings in configLineCalPoints$ be different lengths.
  //We don't resize VNAData or uWorkArray, because they are resized when used.
}
void MainWindow::PartialRestart()
{
  //Restart but return before taking first data
  //This is used to implement user changes without taking data.
  //User should set suppressHardwareInitOnRestart=1 if desired to save time by suppressing initialization ver116-4d
  //This flag is automatically turned off after Restart, so it is a one time thing.
  vars->returnBeforeFirstStep=1; //So we stop before actually scanning, at which point this flag is reset
  QTimer::singleShot(0, this, SLOT(Restart()));
}

void MainWindow::CalcFreqCorrection()
{
  //Calculate power correction factors for each frequency step.
  for (int s=0; s <= vars->globalSteps; s++)
  {
    float currFreq=vars->baseFrequency+graph->gGetPointXVal(s+1);    //Point number is one more than step num  ver116-4k added baseFrequency
    vars->freqCorrection[s]=hwdIf->calMan->calConvertFreqError(currFreq); //Put power correction into the array
  }
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
  if (vars->contextTypes[constHardware]==1)
  {
    contextCount=contextCount+1;
    list.append("StartContext Hardware\r"+winConfigMan->configHardwareContext(activeConfig)+"\rEndContext");
  }
  if (vars->contextTypes[constGrid]==1)
  {
    contextCount=contextCount+1;
    list.append("StartContext Grid\r"+graph->GridContext()+"\rEndContext");
  }
  if (vars->contextTypes[constTrace]==1)
  {
    contextCount=contextCount+1;
    list.append("StartContext Trace\r"+graph->TraceContext()+"\rEndContext");
  }
  if (vars->contextTypes[constSweep]==1)
  {
    contextCount=contextCount+1;
    list.append("StartContext Sweep\r"+SweepContext()+"\rEndContext");
  }
  if (vars->contextTypes[constMarker]==1)
  {
    contextCount=contextCount+1;
    list.append("StartContext Marker\r"+graph->mMarkerContext()+"\rEndContext");
  }
  if (vars->contextTypes[constBand]==1)
  {
     contextCount=contextCount+1;
     list.append("!StartContext BandLineCal");
     vnaCal.BandLineCalContextToFile( list);
     list.append("!EndContext");
  }
  if (vars->contextTypes[constBase]==1)
  {
     contextCount=contextCount+1;
     list.append("!StartContext BaseLineCal");
     vnaCal.BaseLineCalContextToFile(list);
     list.append("!EndContext");
  }
  if (vars->contextTypes[constGraphData]==1)
  {
     contextCount=contextCount+1;
     list.append("!StartContext GraphData");    //ver115-1d
     graph->GraphDataContextToFile(list);   //ver115-1d
     list.append("!EndContext");
  }
  if (vars->contextTypes[constModeData]==1)   //ver115-1d added type 8
  {
     contextCount=contextCount+1;
     //Print the StartContext line only if preceded by some other context
     if (contextCount>1) list.append("!StartContext DataTable");
     hwdIf->CopyModeDataToVNAData(0); //ver116-4j
     hwdIf->touch.touchWriteOnePortParameters(list, vars->msaMode);  //Write data to file
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
  vars->contextTypes[constGrid]=1;   //Grid
  vars->contextTypes[constTrace]=1;   //Trace
  vars->contextTypes[constSweep]=1;   //Sweep
  QString errMsg=SaveContextFile(fName);
  if (errMsg!="")
    QMessageBox::critical(0,"Error", "Unable to save preferences.");
}
void MainWindow::LoadPreferenceFile(QString fileName)
{
  //Load preference
  if (graph->haltsweep==1)
    FinishSweeping(); //Finish current sweep cleanly

  vars->restoreFileHndl=hwdIf->OpenContextFile(fileName,"In");

  // handle missing file
  if (vars->restoreFileHndl == NULL)
  {
    QFile *fileHndl = hwdIf->OpenContextFile(fileName,"IN");
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
    vars->restoreFileHndl=hwdIf->OpenContextFile(fileName,"In");
  }



  graph->RememberState();  //So we can see what changed
  LoadBasicContextsFromFile();   //Load preferences from restoreFileHndl$
  vars->restoreFileHndl->close();
  if (vars->restoreErr!="" && vars->doingInitialization==0)
    QMessageBox::critical(0,"Error", "Error loading preference file: "+vars->restoreErr);
  DetectFullChanges();
  if (graph->continueCode==3 && vars->doingInitialization==0)
    PartialRestart(); //implement changes ver115-3c
  return;
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
  graph->DetectChanges(doRestart);
}
QString MainWindow::SweepContext()
{
  //Return sweep context as string
  //successive lines are separated by chr(13)
  QString newLine="\r";
  QString aSpace=" ";
  //First include variables used outside the graph module
  QString s1= "Version=B";    //This item was added in ver114-7n and changed to B in ver115-1b
  s1= s1+newLine+"msaMode="+vars->msaMode;
  s1= s1+newLine+"FreqMode="+QString::number(vars->freqBand);
  s1= s1+newLine+"BaseFreq="+QString::number(vars->baseFrequency);
  s1= s1+newLine+"SpecialGraph="+QString::number(vars->doSpecialGraph);
  s1=s1+newLine+ "RLCSpec="+vars->doSpecialRLCSpec+";;"+vars->doSpecialCoaxName;
  if (vars->useAutoWait)
  {
    s1=s1+newLine+"Wait="+vars->autoWaitPrecision;
  }
  else
  {
    s1= s1+newLine+"Wait="+QString::number(vars->wate);
  }
  s1= s1+newLine+"PlaneAdj="+QString::number(vars->planeadj);
  s1= s1+newLine+"Path="+util.Word(vars->path,2);    //path is in form "Path N"
  s1= s1+newLine+"SGPreset="+QString::number(vars->sgout);     //ver114-4h
  s1= s1+newLine+"Offset="+QString::number(vars->offset);
  s1= s1+newLine+"IsTG="+QString::number(vars->gentrk);     //ver114-4i
  s1= s1+newLine+"NormRev="+QString::number(vars->normrev); //ver114-4k
  s1= s1+newLine+"VideoFilter="+vars->videoFilter; //ver114-5p
  s1= s1+newLine+"DUTDirection="+QString::number(vars->switchFR); //ver116-4d
  s1= s1+newLine+"GraphData="+QString::number(vars->Y1DataType)+aSpace+ QString::number(vars->Y2DataType); //ver115-1b deleted source constants
  s1= s1+newLine+"Autoscale="+QString::number(graph->autoScaleY1)+ aSpace+ QString::number(graph->autoScaleY2);  //ver114-7e
  s1= s1+newLine+"S21Jig="+vnaCal.S21JigAttach+aSpace+ QString::number(vnaCal.S21JigR0)+aSpace+QString::number(vnaCal.S21JigShuntDelay);  //modver115-1e
  s1= s1+newLine+"S11Bridge="+QString::number(vnaCal.S11BridgeR0)+ aSpace+ QString::number(vnaCal.S11GraphR0)+ aSpace+ vnaCal.S11JigType;

  return s1+newLine+graph->gSweepContext();   //Add items from graph module
}
void MainWindow::RestoreSweepContext()
{
  //public routine to restore sweep context
  //There are a couple of things we can//t do within a true subroutine, so we use this routine as a wrapper
  //Because this gosub routine cannot accept arguments, the following values (non-global) must be preset:
  //   restoreContext$ is the string containing the context
  //   restoreIsValidation=1 to do just validation run (i.e. don//t change data); 0 otherwise
  //If there is an error, and error message is placed in restoreErr$; otherwise it is made blank
  //Note the StartContext line must already have been skipped.
  //Caller must call RememberState before coming here and [DetectFullChanges] on return
  //Get next line and increment startPos to start of the following line
  int startPos=0;
  QString tLine = util.uGetLine(vars->restoreContext, startPos);
  int oldStartPos=startPos;
  QString contextVersion="A";
  QString tag;
  while (tLine!="")
  {
    QString origLine = tLine;
    if (tLine.left(10).toUpper()=="ENDCONTEXT")
    {
      break;  //then exit while
    }
    int isErr=0;
    int equalPos = tLine.indexOf("=");     //equal sign marks end of tag
    if (equalPos == -1)
    {
      isErr=1;
      break;  //exit while
    }
    else
    {
      tag=tLine.left(equalPos).toUpper();  //tag is stuff before equal sign
      tLine=tLine.mid(equalPos+1).trimmed();  //contents is stuff after equal sign
    }
    float v1 = tLine.toFloat();  //Most of our data is numeric values
        //Each tag represents one or several data items. These are retrieved one at a time,
        //and as each is retrieved it is deleted from tLine$. Numeric items are delimited
        //by spaces, tabs or commas. Text items are delimited by the double character
        //contained in sep$, because they may contain spaces or commas. If this is just
        //a validation run, we do not enter any of the retrieved data into our variables.
                //ver114-2d cleaned up and added VNA
    if (tag == "VERSION")
    {
      contextVersion=tLine.trimmed();
    }
    else if (tag == "MSAMODE")
    {
      if (vars->restoreIsValidation==0)
      {
        if (tLine == "SA")
        {
          vars->msaMode = modeSA;
        }
        else if (tLine == "ScalarTrans")
        {
          vars->msaMode = modeScalarTrans;
        }
        else if (tLine == "VectorTrans")
        {
          vars->msaMode = modeVectorTrans;
        }
        else if (tLine == "Reflection")
        {
          vars->msaMode = modeReflection;
        }
      }
    }
    else if (tag == "BASEFREQ")
    {
      if (vars->restoreIsValidation==0)
      {
        vars->baseFrequency=v1;
      }
    }
    else if (tag == "FREQMODE")
    {
      if (vars->restoreIsValidation==0)
      {
        vars->freqBand=v1;
      }
    }
    else if (tag == "SPECIALGRAPH")
    {
      if (vars->restoreIsValidation==0)
      {
        vars->doSpecialGraph=v1;
      }
    }
    else if (tag == "RLCSPEC")
    {
      if (vars->restoreIsValidation==0)
      {
        vars->doSpecialRLCSpec = util.uExtractTextItem(tLine,";;");
        vars->doSpecialCoaxName=tLine;
      }
    }
    else if (tag == "WAIT")
    {
      if (vars->restoreIsValidation==0)
      {
        if (tLine == "Fast" || tLine == "Normal" || tLine == "Precise")
        {
          vars->useAutoWait=1;
          vars->autoWaitPrecision=tLine;
        }
        else
        {
          vars->useAutoWait=0;
          vars->wate=v1;
        }
      }
    }
    else if (tag == "PLANEADJ")
    {
      if (vars->restoreIsValidation==0)
      {
        vars->planeadj=v1;
      }
    }
    else if (tag == "PATH")
    {
      isErr=(v1<=0 || v1 > activeConfig.MSANumFilters);
      if (isErr==0 && vars->restoreIsValidation==0)
      {
        vars->path="Path " + QString::number(v1);
      }
    }
    else if (tag == "SGPRESET")
    {
      if (vars->restoreIsValidation==0)
      {
        vars->sgout=v1;
      }
    }
    else if (tag == "OFFSET")
    {
      if (vars->restoreIsValidation==0)
      {
        vars->offset=v1;
      }
    }
    else if (tag == "ISTG")
    {
      if (vars->restoreIsValidation==0)
      {
        if (activeConfig.TGtop>0)
        {
          vars->gentrk=v1;
        }
        else
        {
          vars->gentrk=0; //Set gentrk only if we have the TG ver115-4f
        }
      }
    }
    else if (tag == "NORMREV")
    {
      if (vars->restoreIsValidation==0)
      {
        vars->normrev=v1;
      }
    }
    else if (tag == "VIDEOFILTER")
    {
      if (vars->restoreIsValidation==0)
      {
        vars->videoFilter=tLine;
      }
    }
    else if (tag == "DUTDIRECTION")
    {
      if (vars->restoreIsValidation==0)
      {
        vars->switchFR=v1;
      }
    }
    else if (tag == "GRAPHDATA")
    {
      //In Version A there was a different format for graph data, so we will
      //just use default values.
      float v1, v2, v3;
      if (contextVersion=="A")
      {
        float v1=0,  v2=0;
        if (vars->msaMode==modeSA)
        {
          v1=constNoGraph;
          v2=constMagDBM;
        }
        if (vars->msaMode==modeScalarTrans)
        {
          v1=constNoGraph;
          v2=constMagDB;
        }
        if (vars->msaMode==modeVectorTrans)
        {
          v1=constAngle;
          v2=constMagDB;
        }
        if (vars->msaMode==modeReflection)
        {
          v1=constGraphS11Ang;
          v2=constGraphS11DB;
        }
      }
      else
      {
        isErr=util.uExtractNumericItems(2, tLine, " ", v1, v2, v3);  //isErr if not two items
      }
      //Be sure data types are valid ver115-1b. An old preference file
      //may use different data codes.
      int tempV1 = (int)v1;
      int tempV2 = (int)v2;
      FilterDataType(tempV1,1);
      FilterDataType(tempV2,2);
      if (isErr==0 && vars->restoreIsValidation==0)
      {
        vars->Y1DataType=tempV1;
        vars->Y2DataType=tempV2;
      }
    }
    else if (tag == "AUTOSCALE")
    {
      float v1, v2, v3;
      if (vars->restoreIsValidation==0)
      {
        isErr=util.uExtractNumericItems(2, tLine, " ", v1, v2, v3);
        if ((v1!=0 && v1!=1) || (v2!=0 && v2!=1))
        {
          isErr=1;
        }
        if (isErr==0)
        {
          graph->autoScaleY1=v1;
          graph->autoScaleY2=v2;
        }
      }
    }
    else if (tag == "S21JIG")
    {
      float v1, v2, v3;
      if (vars->restoreIsValidation==0)
      {
        QString w = util.uExtractTextItem(tLine," ");
        if (w!="Series" && w!="Shunt")
        {
          isErr=1;
        }
        if (isErr==0)
        {
          vnaCal.S21JigAttach=w;
        }
        isErr=util.uExtractNumericItems(2, tLine, " ", v1, v2, v3);
        if (v1<=0)
        {
          isErr=1;
        }
        if (isErr==0)
        {
          vnaCal.S21JigR0=v1;
          vnaCal.S21JigShuntDelay=v2;
        }
      }
    }
    else if (tag == "S11BRIDGE")
    {
      float v1, v2, v3;
      if (vars->restoreIsValidation==0)
      {
        isErr=util.uExtractNumericItems(2,tLine, " ", v1, v2, v3);  //Extract three items and leave the third, a text item
        if (v1<=0 || v2<=0)
        {
          isErr=1;
        }
        tLine=tLine.trimmed();
        if (tLine!="Reflect" && tLine!="Trans")
        {
          tLine="Trans"; //For backward compatibility
        }
        if (isErr==0)
        {
          vnaCal.S11BridgeR0=v1;
          vnaCal.S11GraphR0=v2;
          vnaCal.S11JigType=tLine;
        }
      }
    }
    else
    {
      //Unrecognized tag. Must belong to the graph module, so we end here
      startPos=oldStartPos;    //reset to beginning of this line
      break; //exit while
    }
    if (isErr)
    {
      vars->restoreErr="Sweep Context Error in: " + origLine;
      return;
    }
    //Get next line and increment startPos to start of the following line
    oldStartPos=startPos;
    tLine=util.uGetLine(vars->restoreContext, startPos);
  }
  //If no error so far, process the data for the graph module
  // fix me isValidation does not come from anywhere
  int isValidation = 0;
  vars->restoreErr=graph->gRestoreSweepContext(vars->restoreContext, startPos, isValidation);
  if (vars->restoreErr!="" || isValidation==1)
  {
    return; //done if error or if this is just validation run
  }

  //Now set variables that are calculated from the data just retrieved.
  graph->gGetYAxisRange(2, vars->Y2Bot, vars->Y2Top);       //mag range   ver114-4d reversed mag and phase
  graph->gGetYAxisRange(1, vars->Y1Bot, vars->Y1Top);   //phase range
  vars->globalSteps=graph->gNumDynamicSteps();              //number of steps
  float xMin, xMax;
  graph->gGetXAxisRange(xMin, xMax);
  graph->SetStartStopFreq(xMin, xMax); //sets centfreq, sweepwidth
  //stepfreq=(xMax-xMin)/globalSteps
  vars->steps=vars->globalSteps;
  vars->sweepDir=graph->gGetSweepDir();
  hwdIf->SelectFilter(hwdIf->filtbank );
}
void MainWindow::FilterDataType(int &t, int axisNum)
{
  //Make sure data types are valid for current msaMode$
  //Change to default values if invalid
  //ver115-3b changed to do only a single variable, so we get called once per axis.
  if (vars->msaMode==modeSA)
  {
    if (t!=constMagDBM && t!=constMagWatts && t!= constMagV && t!=constNoGraph)
    {
      if (axisNum!=graph->gGetPrimaryAxis())
      {
        t=constNoGraph;
      }
      else
      {
        t=constMagDBM;
      }
    }
  }
  else if (vars->msaMode==modeScalarTrans)
  {
    if (t!=constMagDB && t!=constMagRatio && t!=constInsertionLoss && t!=constNoGraph)
    {
      if (axisNum!=graph->gGetPrimaryAxis())
      {
        t=constNoGraph;
      }
      else
      {
        t=constMagDB;
      }
    }
  }
  else if (vars->msaMode==modeVectorTrans)
  {
    if (t!=constMagDB && t!=constMagDBM && t!=constMagRatio && t!=constAngle
        && t!=constRawAngle && t!=constGD && t!=constInsertionLoss
        && t!=constNoGraph)
    {
      if (axisNum!=graph->gGetPrimaryAxis())
      {
        t=constGraphS11Ang;
      }
      else
      {
        t=constMagDB;
      }
    }
  }
  else if (vars->msaMode==modeReflection)
  {
    // fix me what is || t > constGraphS11SWR
    if ((t<constGraphS11DB )
        && t != constImpedMag && t != constImpedAng && t>constReflectPower
        && t > constComponentQ && t != constReturnLoss && t!=constNoGraph
        && t != constAdmitMag && t !=constAdmitAng && t != constConductance
        && t!= constSusceptance)
    {
      if (axisNum!=graph->gGetPrimaryAxis())
      {
        t=constGraphS11Ang;
      }
      else
      {
        t=constGraphS11DB;
      }
    }
  }
  else
  {
    t=constNoGraph;
  }
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
        vars->contextTypes[constGrid]=1;
        int i = 0;
        errMsg=graph->RestoreGridContext(vars->restoreContext,i,0); //Actual restoration
      }
      else if (contextName == "TRACE")
      {
        int i = 0;
        errMsg=graph->RestoreTraceContext(vars->restoreContext,i,0);
        vars->contextTypes[constTrace]=1;
      }
      else if (contextName == "SWEEP")
      {
        vars->contextTypes[constSweep]=1;
        vars->restoreIsValidation=0;
        RestoreSweepContext();     //Actual restoration
        errMsg=vars->restoreErr;
      }
      else if (contextName == "MARKER")
      {
        vars->contextTypes[constMarker]=1;
        vars->restoreIsValidation=0;
        //fix me graph->mRestoreMarkerContext();     //Actual restoration
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
float MainWindow::StepWithValue(int dataType, int startStep, int dir, int targVal)   //Return first step number (maybe fractional) with the specified value
{

//start with step startStep and move up (dir=1) or down (dir=-1). Return result
  //We look at the specified dataType (constAngle, etc.)
  //If value is not found, return -1
  //Linear interpolation is done if necessary to find the exact point (possibly fractional) with the specified value.
  //startStep=int(startStep)

  int pMin, pMax;
  graph->gGetMinMaxPointNum(pMin, pMax);
  int stepMin=pMin-1; int stepMax=pMax-1;
  int endStep;
  if (dir==1) endStep=stepMax; else endStep=stepMin;
  if (startStep<stepMin) startStep=stepMin;
  if (startStep>stepMax) startStep=stepMax;

  //  fix me need to finish converting the rest of the function
  int saveY1Type=vars->Y1DataType; vars->Y1DataType=dataType; //Values will be calculated for Y1
  int saveY2Type=vars->Y2DataType; vars->Y2DataType=constNoGraph;

  float dum;
  float stepVal;
  graph->CalcGraphData(startStep, stepVal, dum, 0);  //Calculate Y1 data type from data arrays
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
    QMessageBox::warning(0, "Coding Error", "read the source");
    // need to fix the for loop, as dir was only ever 0 in the Liberty code
  }
  for (int i=startStep+dir; i <= endStep; i++)  // step dir      //Start at second step
  {
    graph->CalcGraphData(i,stepVal, dum, 0);  //Calculate Y1 data type from data arrays
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

void MainWindow::on_actionPrimary_Axis_triggered()
{
  if (graph->haltsweep==0)
    FinishSweeping();

  dialogChoosePrimaryAxis choosePrimary(this);
  choosePrimary.setPrimaryAxis(vars->primaryAxisNum);
  choosePrimary.exec();
  if (choosePrimary.cancelled)
  {
    return;
  }

  vars->primaryAxisNum = choosePrimary.getPrimaryAxis();
  graph->gSetPrimaryAxis(vars->primaryAxisNum);
  QString lastCol = gridappearance->gGetLastPresetColors();
  if (lastCol=="DARK" || lastCol=="LIGHT")
  {
    gridappearance->gUsePresetColors(lastCol, graph->gPrimaryAxis);    //Reset colors; may be affected by primary axis change
  }
  QColor dum1, dum2;
  QColor referenceColor1, referenceColor2;
  gridappearance->gGetSupplementalTraceColors(referenceColor1, referenceColor2, dum1, dum2);
  if (vars->primaryAxisNum == 1)
  {
    graph->referenceColorSmith = referenceColor1;
  }
  else
  {
    graph->referenceColorSmith = referenceColor2;
  }
  gridappearance->SetCycleColors();



  gridappearance->gUsePresetColors(gridappearance->gGetLastPresetColors(), graph->gPrimaryAxis);  //Reselect same appearance in case primary axis change had effect.
  gridappearance->SetCycleColors();
  if (vars->msaMode == modeReflection)
  {
    RestartReflectionMode();
    return;
  }
  else if (vars->msaMode == modeScalarTrans || vars->msaMode == modeVectorTrans)
  {
    RestartTransmissionMode();
    return;
  }
  else   // modeSA
  {
    if (vars->gentrk==1)
    {
      RestartSATGmode();
      return;
    }
    else
    {
      RestartPlainSAmode();
      return;
    }
  }
}

void MainWindow::on_actionSweep_triggered()
{

  if (graph->haltsweep)
  {
    graph->continueCode=1;
    FinishSweeping();
  }

  dialogFreqAxisPreference sweepWindow(this);

  sweepStruct config;
  if (vars->msaMode == modeSA)
  {
    config.msaMode = modeSA;
  }
  else if (vars->msaMode == modeReflection)
  {
    config.msaMode = modeReflection;
  }
  else if (vars->msaMode == modeVectorTrans)
  {
    config.msaMode = modeVectorTrans;
  }
  else if (vars->msaMode == modeScalarTrans)
  {
    config.msaMode = modeScalarTrans;
  }
  else
  {
    QMessageBox::critical(this, "Bad msamode", "Who did a typo?");
  }

  config.gentrk = vars->gentrk;
  config.calCanUseAutoWait = vars->calCanUseAutoWait;
  config.useAutoWait = vars->useAutoWait;
  config.alternateSweep = vars->alternateSweep;
  config.centfreq = vars->centfreq;
  config.sweepwidth = vars->sweepwidth;
  config.startfreq = vars->startfreq;
  config.endfreq = vars->endfreq;
  config.baseFrequency = vars->baseFrequency;
  config.globalSteps = vars->globalSteps;
  config.wate = vars->wate;
  config.path = vars->path;
  config.doSpecialGraph = vars->doSpecialGraph;
  config.videoFilter = vars->videoFilter;
  config.spurcheck = vars->spurcheck;
  config.normrev = vars->normrev;
  config.sgout = vars->sgout;
  config.offset = vars->offset;
  config.switchFR = vars->switchFR;
  config.userFreqPref = vars->userFreqPref;
  config.planeadj = vars->planeadj;
  config.prevPlaneAdj = vars->prevPlaneAdj;
  config.freqBand = vars->freqBand;
  config.LO2 = hwdIf->LO2;
  config.autoWaitPrecision = vars->autoWaitPrecision;


  // need to hange the MSAFiltStrings away from an array
  config.MSANumFilters = activeConfig.MSANumFilters;
  for (int i = 0; i < activeConfig.MSANumFilters; i++)
  {
    config.MSAFiltStrings.append(activeConfig.MSAFiltStrings[i]);
  }
  for (int i = 1; i <= 4; i++)
  {
    if (activeConfig.videoFilterNames[i].trimmed() != "")
    {
      config.videoFilterNames.append(activeConfig.videoFilterNames[i]);
    }
  }
  config.TGtop = activeConfig.TGtop;
  config.appxLO2 = activeConfig.appxLO2;
  config.invdeg = activeConfig.invdeg;

  config.S11GraphR0 = vnaCal.S11GraphR0;
  config.prevS11GraphR0 = vars->prevS11GraphR0;
  config.S11JigType = vnaCal.S11JigType;
  config.S21JigAttach = vnaCal.S21JigAttach;

  gridappearance->FillAppearancesArray(config.appearances);
  config.lastPresetColor = gridappearance->gGetLastPresetColors();
  gridappearance->getcustomPresetNames(config.customPresetNames);

  config.sweepDir = graph->gSweepDir;
  config.XIsLinear = graph->gGetXIsLinear();
  int HorDiv;
  int VertDiv;
  graph->gGetNumDivisions(HorDiv, VertDiv);
  config.HorDiv = HorDiv;
  config.VertDiv = VertDiv;
  config.refreshEachScan = graph->refreshEachScan;
  config.displaySweepTime = graph->displaySweepTime;

  graph->RememberState();  //Remember various variables so we can see if they change
  int needRestart = sweepWindow.DisplayAxisXPreference(&config);

  if (!sweepWindow.cancelled )
  {
    //-------------------------------------------------------------
    // grab all the changes from the form

    //vars->gentrk = config.gentrk;
    vars->calCanUseAutoWait = config.calCanUseAutoWait;
    vars->useAutoWait = config.useAutoWait;
    vars->alternateSweep = config.alternateSweep;

    vars->centfreq = config.centfreq;
    vars->sweepwidth = config.sweepwidth;
    vars->startfreq = config.startfreq;
    vars->endfreq = config.endfreq;
    graph->gSetXAxisRange(config.startfreq, config.endfreq);

    vars->baseFrequency = config.baseFrequency;
    vars->globalSteps = config.globalSteps;
    vars->wate = config.wate;
    vars->path = config.path;
    vars->doSpecialGraph = config.doSpecialGraph;
    vars->videoFilter = config.videoFilter;
    vars->spurcheck = config.spurcheck;
    vars->normrev = config.normrev;
    vars->sgout = config.sgout;
    vars->offset = config.offset;
    vars->switchFR = config.switchFR;
    vars->userFreqPref = config.userFreqPref;
    vars->planeadj = config.planeadj;
    vars->prevPlaneAdj = config.prevPlaneAdj;
    vars->freqBand = config.freqBand;
    //vars->LO2 = config.LO2;
    vars->autoWaitPrecision = config.autoWaitPrecision;

    activeConfig.invdeg = activeConfig.invdeg;

    vnaCal.S11GraphR0 = config.S11GraphR0;
    vars->prevS11GraphR0 = config.prevS11GraphR0;
    vnaCal.S11JigType = config.S11JigType;
    vnaCal.S21JigAttach = config.S21JigAttach;

    graph->gSetSweepDir(config.sweepDir);
    graph->gSetXIsLinear(config.XIsLinear);

    graph->gSetNumDivisions(config.HorDiv,config.VertDiv);
    graph->refreshEachScan = config.refreshEachScan;
    graph->displaySweepTime = config.displaySweepTime;

    //-------------------------------------------------------------
    graph->DetectChanges(0);   //Do necessary redrawing and set continueCode ver114-6e
    if (graph->continueCode==3)
    {
      needRestart = 1;
    }
    else
    {
      graph->continueCode=0;
      needRestart = 0;
    }

    vars->steps=vars->globalSteps;   //transfer to non-global
    vars->sweepDir=graph->gGetSweepDir();  //transfer to non-global
    if (vars->calCanUseAutoWait==0 && vars->useAutoWait)
    {
      vars->useAutoWait=0;
      vars->wate=100;
    }
    if (needRestart==1)
    {
      PartialRestart();
    }
    graph->continueCode=0;  //signal to keep going ver115-8d
    /*if (vars->multiscanIsOpen)
    {
      multiscanSaveContexts(0); //zero means main graph  ver115-8d
    }*/
  }
}
//-----------------------------------------------------------------------------

