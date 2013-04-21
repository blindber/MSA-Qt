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
#ifndef TOUCHSTONE_H
#define TOUCHSTONE_H
#include <QtCore>
#if (QT_VERSION < 0x050000)
#include <QtGui>
#else
#include <QtWidgets>
#endif
#include "msautilities.h"
#include "uwork.h"

class touchStone
{
public:
  touchStone();
  void setUwork(cWorkArray *uw);
  msaUtilities util;


  int touchBadLine;     //Line number of Touchstone file error; zero if no error
  //---Variables relating to touchstone data files--- ver115-5f
  QString touchFreq, touchForm;
  int touchRef; //Touchstone format specs
  float touchFreqMult;     //Frequency multiplier corresponding to touchFreq$. E.g. 1e6 for MHZ.
  QString dataLoadLastFolder;  //Folder from which parameter data was last loaded

  int touchMaxData;     //Max data lines in our touchstone files when reading
  //touchMaxData=10000  //ver116-1b
  QString touchLastFolder;    //Last folder accessed for save or load of parameter files.
  QString touchSParamType;   //Type of params in sParam: "", "Reflection", Transmission" or "TwoPort"

  QString touchComments[10];   //Up to 10 lines of comments from parameter file. Zero entry not used
  int touchCommentCount;    //Number of comment lines in touchComments$
  //---End variables for data files----

  //==================================================================
  //         TOUCHSTONE MODULE ver115-5f
  //==================================================================


  QFile *touchOpenInputFile(QString fullPath); //Open file; return handle or blank if not successful
  QFile *touchOpenOutputFile(QString fullPath);  //Open file; return handle or blank if not successful
  void touchWriteOnePortParameters(QStringList &fHndl, QString dataMode);
  void touchReadParams(QString source, int sourceType);
  void touchReadParams(QFile *source, int sourceType);
  void touchProcessPostOptionComments(QString tLine);
  void touchProcessOptionsLine(int fileLine, QString tLine);
  void touchProcessDataLine(int fileLine, QString tLine);   //mod 116-1b
  QString touchStringInput(QString &s);
  void touchConvertSingleParam(QString oldForm, QString newForm, float &p1, float &p2);  //convert single param in place //ver116-4m
  void touchConvertParamForm(QString newForm);
  //===============End Touchstone Module==============

private:
  cWorkArray *uWork;
};

#endif // TOUCHSTONE_H
