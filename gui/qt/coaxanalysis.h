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
#ifndef COAXANALYSIS_H
#define COAXANALYSIS_H

#include <QtCore>
#include "msautilities.h"

class coaxAnalysis
{
public:
  coaxAnalysis();

  void CoaxLoadDataFile(QString path);
  bool CoaxCreateFile(QString path);
  void setFilePath(QString path);
  QStringList coaxNames;  //Coax names; 0 entry not used
  float coaxData[101][5]; //Coax data: R0(,1), VF(,2), K1(,3), K2(,4)
  int getTest() {return test;}
  void setTest(int t) { test = t; }
  void CoaxS21FromSpecs(double sysZ0real, double sysZ0imag, QString coaxSpecs, double fMHz, double &S21dB, double &S21ang);
  QString CoaxSpecs(float R0, float VF, float K1, float K2, float lenFeet);
  int CoaxParseSpecs(QString coaxSpecs, double &R0, double &VF, double &K1, double &K2, double &lenFeet);
  void CoaxGetData(QString coaxName, float &R0, float &VF, float &K1, float &K2);
  void CoaxTerminatedZFromSpecs( QString coaxSpecs, double fMHz, double ZtReal, double ZtImag, double &ZReal, double &ZImag);


private:
  msaUtilities util;

  int test;

  QFile *CoaxOpenDataFile(bool isInput);
  void CoaxSaveDataFile();




  QString RLCDialogCoaxTypes[111];  //For RLC specification dialog ver115-4a
  int maxCoaxEntries, numCoaxEntries;   //Maximum and actual number of entries in coax data arrays
  QString DefaultDir;
};



#endif // COAXANALYSIS_H
