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
#ifndef UWORK_H
#define UWORK_H

#include "globalvars.h"

class cWorkArray
{
public:
  cWorkArray();
  void uSetMaxWorkPoints(int maxPoints, int maxData);
  QString uTextArrayToString(int startN, int endN);
  int uWorkArrayFromTextArray(int nLines, int nPerLine);

  Q2DfloatVector uWorkArray; //Initially 800 points with up to 9 (0...8) data items per point

  unsigned int uWorkMaxPoints, uWorkNumPoints;   //max points and actual used points in uWorkArray

  unsigned int uWorkMaxPerPoint, uWorkNumPerPoint; //max and actual number of data items per entry of uWorkArray
  QVector<QString> uWorkFormats;    //Format strings for each data item, in form suitable for "using" function,
                          //or blank to cause str$() function to be used.
  QString uTextPointArray[4105];

  QString uWorkTitle[6];      //Title info extracted when processing uTextPointArray$; 0 not used   ver114-3e; ver114-5m increased to 6

};

#endif // UWORK_H
