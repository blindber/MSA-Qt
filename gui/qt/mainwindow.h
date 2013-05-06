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
//#include "usbfunctions.h"
#include "globalvars.h"
#include "referencedialog.h"
#include "msautilities.h"
#include "uwork.h"
#include "interpolation.h"
#include "dialoggridappearance.h"
//#include "dialogcomponentmeasure.h"
//#include "dialogcrystal.h"
#include "dialogvnacal.h"
#include "graph.h"
#include "hwdinterface.h"
#include "debugmodule.h"
#include "dialogshowvars.h"

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
  QGraphicsScene scenePleaseWait;
  dialogConfigMan *winConfigMan;
//  usbFunctions *usb;
  globalVars *vars;
  msaUtilities util;
  cWorkArray uWork;
  interpolation inter;
  dialogGridappearance *gridappearance;
  //dialogCrystal xtal;
  dialogVNACal vnaCal;
  msagraph *graph;
  hwdInterface *hwdIf;
  DebugModule debug;
  //QTimer *timerStart;
  //QTimer *timerStart2;
  dialogShowVars *showVars;


  void CalcFreqCorrection();

  void CommandThisStep();
  int PostScan();
  void FinishSweeping();
  void ReverseSweepDirection();

  QString SaveContextFile(QString fName);
  void SavePreferenceFile(QString fName);
  void LoadPreferenceFile(QString fileName);
  void DetectFullChanges();


  void FindClientOffsets();

  void CreateGraphWindow();







  void GetDialogPlacement();



  void menuRunConfig();
  void menuRunCal();
  void menuQuitMultiscan();
  void ConformMenusToMode();
  void menuExpandSweep();
  void RestartSATGmode();
  void RestartPlainSAmode();
  void RestartSAmodes();

  void GoSAmode();
  void RestartTransmissionMode();
  void GoTransmissionMode();
  void RestartReflectionMode();
  void GoReflectionMode();
  void LeftButDouble();
  void preupdatevar();
  void RightButDown();
  void MarkerClick();
  void StartingLimits();






  void mUpdateMarkerEditButtons();
  void mMarkSelect(QString markID);
  void mBtnMarkClear(QString markID);
  void mBtnMarkEdit(QString markID);
  void mBtnMarkDelete(QString markID);
  void btnIncPoint();
  void btnDecPoint();
  void IncDecPoint(QString btn);
  void OneStep();
  void HaltAtEnd();
  void Continue();
  void FocusKeyBox();

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
//===============Configuration Manager Module=======================

  msaConfig activeConfig;
//===========================Real-Time Graphing Module=====================================
  // see the global file for the variables

  void gChangeTextSpecs(QString btn, QString &newTxt);
  bool CoaxCreateFile();
  void smithFinished(QString );
  void multiscanCloseAll();
  QString smithGraphHndl();
  void CloseSpecial(int returnflag);
  QString DefaultDir;
  int special;
  QString SweepContext();
  void RestoreSweepContext();
  void FilterDataType(int &t, int axisNum);
  void DataTableContextAsTextArray();
  QString DataTableContext();
  void DataTableContextToFile(QFile *fHndl);
  void RestoreDataTableContext(QString &s, int &startPos, int doTitle);
  void GetDataTableContextFromFile(QFile *fHndl, int doTitle);


  void LoadBasicContextsFromFile();
  float StepWithValue(int dataType, int startStep, int dir, int targVal);   //Return first step number (maybe fractional); with the specified value




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

  void on_actionPrimary_Axis_triggered();

  void on_actionSweep_triggered();

public slots:
  void updateView();
  void DisplayButtonsForHalted();
  void DisplayButtonsForRunning();
  void RequireRestart();
  void CleanupAfterSweep();
  void ChangeMode();
  void SkipHardwareInitialization();
  void ResizeArrays(int);
  void delayedStart();
  void PartialRestart();
  void ProcessAndPrint();
  void PrintMessage();

  void Showvar();
  void updatevar(int);

  void Restart();
  void Halted();
  //void IncrementOneStep();
  void StartSweep();




};

#endif // MAINWINDOW_H
