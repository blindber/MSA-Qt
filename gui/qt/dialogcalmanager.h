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
#ifndef DIALOGCALMANAGER_H
#define DIALOGCALMANAGER_H

#include <QtCore>

#if (QT_VERSION < 0x050000)
#include <QtGui>
#else
#include <QtWidgets>
#endif
#include "globalvars.h"
#include "msautilities.h"
#include "interpolation.h"
#include "dialogconfigman.h"

namespace Ui {
class dialogCalManager;
}

class dialogCalManager : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogCalManager(QWidget *parent = 0);
  ~dialogCalManager();
  void setMsaConfig(msaConfig *config);
  void setGlobalVars(globalVars *newVars);
  void setFilePath(QString path);
  //@==============Calibration Manager Module==========================


  int calEditorPathNum;  //0 for freq; 1-N for magnitude

  QString calManFiles;  //Indicates which files exist (marked by "1")
                         //First char is freq cal file, Nth (1...41) is mag cal file N-1
  QString calFileInfo[11][4];  //Used internally to request file info

  int calManEntryIsRef;  //Used to keep track of whether data entry is on first (reference) point
  float calManRefPhase, calManRefFreq, calManRefPower;     //Used during entry of data
  QString calManOldText;   //original text or text at time of last Save.
  int calManLastAutoPoint;  //Previous point number entered for freq cal from the user sweep //SEW8
  int calManEnterError;   //Set to 1 if error occurs in calManEnter; otherwise 0 ver114-3d

  void calManRunManager(int hasVNA);
  void calManForceOn();
  void calManForceOff();
  void calManSetPhase();
  void calManResetPhase();
  void calManBtnReturn();
  void calManFinished();
  void calCloseWindows();
  void calManEnterAll();
  void calManEnter(QString btn);
  void calFileExists();
  void calNoFile();
  void calManBtnNextFreqPoint();
  void calManBtnPrevFreqPoint();
  void calManGetFreqInput(int measStep);
  void calManMenuMeasure();
  void calManMeasure();
  void calManClean();
  void calManDisplayDefault(QString btn);
  void calManEnterAvailablePaths();
  void calManPrepareEntry();
  void calManEnterInstructions(QString btn);
  int calManWarnToSave(int allowCancel);
  void calInitFirstUse(int maxMagPoints, int maxFreqPoints, int doPhase);
  int calVersion();
  void calSetMaxPoints(int maxMagPoints, int maxFreqPoints);
  void calClearMagPoints();
  void calClearFreqPoints();
  int calDataHadPhase();
  void calSetDoPhase(int doPhase);
  int calGetDoPhase();
  void calClearComments();
  int calNumMagPoints();
  int calNumFreqPoints();
  void calGetMagPoint(int N, float &adc, float &db, float &phase);
  void calGetFreqPoint(int N, float &f, float &db);
  int calAddMagPoint(float adc, float db, float phase);
  int calAddFreqPoint(float f, float db);


  void calSortMag();
  void calSortFreq();
  void calCreateMagCubicCoeff();
  void calCreateFreqCubicCoeff();
  int calBinarySearch(int dataType, float searchVal);
  void calConvertMagPhase(float magdata, int wantPhase, float &magDB, float &phaseCorrect);
  float calConvertFreqError(float freq);
  void calCreateDefaults(int pathNum, QString editor, int doPoints);
  void calAvailableFiles(int pathNum, QString editor, int doPoints);
  void calInstallFile(int pathNum);
  void calSaveToFile(int pathNum);
  void calSaveToEditor(QPlainTextEdit *editor, int pathNum);
  void calOutputData(QStringList &lines, int pathNum, int &startLine);
  QString calOpenFile(int pathNum);
  QString calFileName(int pathNum);
  QString calFilePath();
  QString calLoadFromEditor(QPlainTextEdit *editor, int pathNum);
  QString calLoadFromFile(int pathNum);
  QString calReadFile(QFile *calFile, QPlainTextEdit *editor, int isFile, int pathNum, int &startLine);

  //---------------Start Global Variables for Mag/Freq Calibration Module---------
      //calMagTable contains calibration data for the current signal path
      //The zero entry for the first dimension is not used.
      //magCalTable(i, v) gives the ith entry; v=0 gives ADC reading; v=1 gives actual db value.
  int  calMaxMagPoints, calMaxFreqPoints;
  Q2DfloatVector calMagTable;  //Size can be changed dynamically
      //calMagTable contains calibration data for the current signal path
      //The zero entry for the first dimension is not used.
      //calMagTable(i, v) gives the ith entry; v=0 gives freq (in MHz); v=1 gives dbm magnitude;
      //v=2 gives phase correction to be subtracted from raw phase readings, used only for VNA.
  //Each entry of calMagCoeffTable() will have 8 numbers. 0-3 are the A,B,C,D
  //coefficients for interpolating the real part; 4-7 are for the imag part.
  //dim calMagCoeffTable(100,7)    //SEWcal Cubic coefficients for interpolating in calMagTable //ver115-1d moved to top of program

  int calMagPoints;  //Number of actual points in calMagTable
  int calMagFileHadPhase; //Set to 1 if file data had third data element (for VNA)
                          //Otherwise, 0.
  int calDoPhase;     //Set to 1 to treat mag cal data as including phase; otherwise phase ignored.

       //calFreqTable contains calibration data for response over frequency
      //The zero entry for the first dimension is not used.
      //calFreqTable(i, v) gives the ith entry; v=0 gives freq (in MHz);
      //v=1 gives magnitude correction.
  Q2DfloatVector calFreqTable;     //Size can be changed dynamically
   //calFreqCoeffTable has the A,B,C,D coefficients for interpolating in the calFreqTable()
  //dim calFreqCoeffTable(800,3)    //SEWcal Cubic interpolation coefficients for calFreqTable()  //ver115-1d moved to top of program
  int calFreqPoints;  //Number of actual points in calFreqTable

  QString calFileComments[3];    //Up to 2 lines of comments from files
  QString calFileCommentChar;     //Character marking comments in data files
  float calModuleVersion; //Version of this module; set in calInitFirstUse
  int calFileVersion;   //Version of last opened file
  int calfigModuleVersion;
  //Each entry of calMagCoeffTable() will have 8 numbers. 0-3 are the A,B,C,D
  //coefficients for interpolating the real part; 4-7 are for the imag part.
  Q2DintVector calMagCoeffTable;    //SEWcal Cubic coefficients for interpolating in calMagTable moved here by ver115-1d
   //calFreqCoeffTable has the A,B,C,D coefficients for interpolating in the calFreqTable()
  Q2DintVector calFreqCoeffTable;    //SEWcal Cubic interpolation coefficients for calFreqTable()  moved here by ver115-1d
  QString DefaultDir;


private:
  Ui::dialogCalManager *ui;
  msaUtilities util;
  interpolation inter;
  msaConfig *activeConfig;
  globalVars *vars;

  QStringList calManFileList;
  bool teTextChanged;
  int calManFileReload();

public slots:

private slots:
  void on_Reload_clicked();
  void on_Save_clicked();
  void on_Return_clicked();
  void on_StartEntry_clicked();
  void on_Clean_clicked();
  void on_DispDefault_clicked();
  void on_Measure_clicked();
  void on_Enter_clicked();
  void on_NextPoint_clicked();
  void on_EnterAll_clicked();

  void on_pathList_currentRowChanged(int currentRow);

  void on_te_textChanged();

signals:
  void RequireRestart();
  void PartialRestart();

};

#endif // DIALOGCALMANAGER_H
