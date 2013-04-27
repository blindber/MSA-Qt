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
#ifndef DIALOGOSLCAL_H
#define DIALOGOSLCAL_H

#include <QtCore>
#include <QDialog>
#include "uwork.h"

namespace Ui {
class dialogOSLCal;
}

class dialogOSLCal : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogOSLCal(QWidget *parent = 0);
  ~dialogOSLCal();
  void InstallSelectedOSLCal();
  void setUwork(cWorkArray *newuWork);
  
private:
  Ui::dialogOSLCal *ui;
  cWorkArray *uWork;

  //---------------Routines to Handle OSL Calibration------------ added by ver115-1b
    void BandOSLCalIsCurrent();
    void BaseOSLCalIsCurrent();
    void BaseOSLCalIsInstalled();
    void PerformOSLCal();
    void OSLEnterPresetValues();
    void OSLcalSetBridge();
    void OSLcalSetSeries();
    void OSLcalSetShunt();
    void OSLCheckFull();
    void OSLCheckRef();
    void SetOSLCalCheckboxStatus();
    void OSLCancel();
    void OSLFinished();
    void OSLDone();
    void OSLSelectSet();
    void OSLdoOpen();
    void OSLdoShort();
    void OSLdoLoad();
    void SetOSLCalButtonStatus();
    void OSLCalAborted();
    void OSLdoCal();
    void ProcessOSLBandCal();
    void ExplainOSL();
    void OSLExplainFinished();
    void ExplainJigType();
    void JigExplainFinished();
    void OSLOpenFile();
    void OSLGetFileData();
    void OSLSaveCalSetFile();
    void OSLGetCalSets();
    void ProcessOSLCal();
    void CalcOSLCoeff1();
    void CalcOSLCoeff();
    void CalcOSLStandards();
    void RunCalUpdate();
    void updateNil();
    void PerformCalUpdate();
    void PerformOSLCalUpdate();
    void PerformLineCalUpdate();
    void CalUpdateAborted();
    void CalUpdateFinished();

    int OSLCalContextAsTextArray(int isBand);
    void OSLCalContextToFile(QFile *fHndl, int isBand );
    void OSLGetCalContextFromFile(QFile *fHndl, int isBand);

  //---------------End Routines to Handle OSL Calibration---------------

};

#endif // DIALOGOSLCAL_H
