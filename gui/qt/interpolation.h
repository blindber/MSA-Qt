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
#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "globalvars.h"

class interpolation
{
public:
  interpolation();
  void setGlobalVars(globalVars *newVars);

  //====================START INTERPOLATION ROUTINES==========================
  //Variables for interpolation routines
  Q2DfloatVector intSrc, intDest; //Data for InterpolateTableToTable (freq, real, imag); first index runs from 1
  Q2DfloatVector intSrcCoeff;   //Cubic coefficents (A,B,C,D) for interpolating real and imag parts from intSrc()
  int intSrcPoints, intDestPoints, intMaxPoints;

  //------------------Data access routines--------------------
  void intSetMaxNumPoints(int maxPoints);
  void intReset();
  void intClearSrc();
  void intClearDest();
  void intAddSrcEntry(int f, int r, int im);
  void intAddDestFreq(float f);
  void intGetSrc(int num, int &f, int &r, int &im);
  int intSrcFreq(int num);
  void intGetDest(int num, int &f, int &r, int &im);
  int intDestFreq(int num);
  int intMaxEntries();
  int intSrcEntries();
  int intDestEntries();
  //------------------End Data access routines--------------------
  float intLinearInterpolateDegrees(float fract, float v1, float v2, float angleMin, float angleMax);
  void intLinearInterpolation(int freq, int isPolar, int f1, int R1, int I1, int f2, int R2, int I2, int &p1, int &p2);
  void intCreateCubicCoeffTable(int doPart1, int doPart2, int isAngle, int favorFlat, int doingPhaseCorrection);
  void intCalcCubicCoeff(int pointNum, int partNum, int isAngle, int favorFlat, float &A, float &B, float &C, float &D);
  int intBinarySearch(int searchVal);
  void intSrcToDest(int isPolar, int interpMode, int params);

  int LinearInterpolateDataType(int dataType, float fract, float v1, float v2);
  //=====================END INTERPOLATION ROUTINES===========================

private:
  globalVars *vars;

};

#endif // INTERPOLATION_H
