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
#ifndef DIALOGVNACAL_H
#define DIALOGVNACAL_H

#include <QDialog>
#include "globalvars.h"
//#include "graph.h"

namespace Ui {
class dialogVNACal;
}

class dialogVNACal : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogVNACal(QWidget *parent = 0);
  ~dialogVNACal();
  void RunVNACal();
  void setGlobalVars(globalVars *newVar);
//  void setGlobalGraph(msagraph *newGraph);
  void SignalNoCalInstalled();
  int BandLineCalIsCurrent();
  int BaseLineCalIsCurrent();
  int BaseLineCalIsInstalled();
  



  QString installedBandLineTimeStamp;  //Time stamp of the band cal that was installed; not when it was installed ver115-2d
  //Following are sweep params at which Base Line Cal was last installed ver114-5f
  int installedBaseLineStartFreq, installedBaseLineEndFreq, installedBaseLineNumSteps;
  int installedBaseLineLinear;
  QString installedBaseLineTimeStamp; //Time stamp of the base cal that was installed; not when it was installed ver115-2d
  QString installedOSLBaseTimeStamp;   //Time stamp of the installed base cal; not when it was installed ver115-2d
  int bandLineStartFreq, bandLineEndFreq, bandLineNumSteps, bandLineLinear;
  QString bandLinePath, bandLineTimeStamp;
      //Following are Base Line cal sweep params ver114-5e
  int baseLineStartFreq, baseLineEndFreq, baseLineNumSteps, baseLineLinear;
  QString baseLinePath, baseLineTimeStamp;
  QString S21JigAttach;  //"Series" or "Shunt" to indicate the Transmission jig used ver114-6k
  int S21JigR0;   //Source and load impedances of Transmission jig
  int S21JigShuntDelay;     //One-way delay of connector from shunt fixture through line to the DUT, in ns. ver115-1e
  int S11BridgeR0, S11GraphR0;   //Bridge reference and graph reference for S11   //ver114-6k
  QString S11JigType;      //="Trans" if S21Jig is being used for reflection mode; "Reflect" if reflection bridge is used ver115-1b
  int installedOSLBaseStartFreq, installedOSLBaseEndFreq, installedOSLBaseNumSteps;
  QString baseLineS21JigAttach, bandLineS21JigAttach;     //S21JigAttach$ for applicable cal //ver115-1B
  int baseLineS21JigR0, bandLineS21JigR0;               //S21 jig R0 for applicable cal ver115-1b
  int desiredCalLevel, applyCalLevel;    //2=band Cal (if exists); 1=Base Cal(if exists); 0=None

  QString installedOSLBandTimeStamp;   //Time stamp of the installed band cal; not when it was installed ver115-2d
  QString calLevel;
          //We keep track of the parameters for the last cal. Number of steps is set to -1 to indicate no valid cal
      //Following are Full Line cal sweep params ver114-5e


private:
  Ui::dialogVNACal *ui;
  globalVars *vars;
  //msagraph *graph;

  void VNACalFinished();
  void ExplainTransCal();
  void ExplainTransCalFinished();
  void SetCalButtonStatus();
  void CalAborted();
  void PerformCal();
  void ClearBandCal();
  void ClearBaseCal();
  void SaveBaseCal();
  void BandLineCal();
  void TransferBandToBaseLineCal();
  void TransferBandToBaseOSLCal();
  void CalInfo();




};

#endif // DIALOGVNACAL_H
