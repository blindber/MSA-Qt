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
#ifndef MSAUTILITIES_H
#define MSAUTILITIES_H
#include <QtCore>
#include <QApplication>
#include <math.h>

class msaUtilities
{
public:
  msaUtilities();
  QString uGetLine(QString inStr, int &startPos);
  QString Word(QString strExp, int n);
  QString uExtractTextItem(QString &data, QString delim);
  int uExtractNumericItems(int nItems, QString &data, QString delims, float &val1, float &val2, float &val3);
  QString uExtractDataItem(QString &data, QString delims);
  int uIsNumeric(QString data);
  QString ToHex(int value);
  void beep();
  QString time(QString t);
  QString usingF(QString form, float v);
  QString readLine(QFile *fHndl, char delm);
  QString Space(int N);

  float uSinh(float x);
  float uCosh(float x);       //sinh of real number
  float uTanh(float x);       //sinh of real number
  float uValWithMult(QString data);   //Return value of number which end with a multipler (e.g. K, M,...);
  QString uRepeat(QString s, int n);
  QString  uGetParamText(QString s,QString key, QString delim); //Return parameter text following the text in key$, to but not including the delim$ char
  QString uFormatByDig(float v, int maxWhole, int maxDec, int maxSig);
  QString uFormatted(double v, QString form);
  QString uCompact(QString str);
  void uScaleWithMultiplier(double  &v, QString &mult);
  double uMultiplierValue(QString mult);    //Return value of specified multiplier
  QString uPrompt(QString caption, QString msg, int doYesNo, int allowCancel);   //Post message and return user response

  float uRadsPerDegree();
  float uDegreesPerRad();
  float uPi();
  float uNatLog10();
  float uE();
  float uSafeLog10(float aVal);
  float uLog10(float aVal);
  int uRoundDown(float x);
  int uRoundUp(float x);
  int uRoundUpToMultiple(float X, int mult);
  int uRoundDownToMultiple(float X, int mult);
  float uRoundUpToPower(float X, float base);
  float uRoundDownToPower(float x, float base);
  float uTenPower(float pow);
  float uPower(float x, float pow);
  float uATan2(float r, float i);
  float uNormalizeDegrees(float deg);

  QString uAlignDecimalInString(QString v, int lenInDigits, int nLeft);
  QString uScientificNotation(float v, int nDec, int padZero);
  void uCrystalParameters(float Fs, float Fp, float PeakS21DB, float Fdb3A, float Fdb3B, float &Rm, float &Cm, float &Lm, float &Cp, float &Qu, float &QL);
  void uParallelRLCFromScalarS21(float Fp, float PeakS21DB, float Fdb3A, float Fdb3B, float &parR, float &L, float &C, float &Qu, float &QL, float &serR);
  void uRLCFromTwoImpedances(QString connectType, float F1, float R1, float X1, float F2, float R2, float X2, float &R, float &L, float &C);
  void uShuntJigImpedance(float R0, float S21DB, float S21Deg, float delay, float freq, float *Res, float &React);
  void uAdjustS21ForConnectorDelay(float freq, float &S21DB, float &S21Ang);
  void uSeriesJigImpedance(float R0, float S21DB, float S21Deg, float &Res, float &React);
  void uS11DBToImpedance(float R0, float S11DB, float S11Deg, float &Res, float &React);
  void uRefcoToImpedance(float R0, float rho, float theta, float &Res, float &React);
  void uImpedanceToRefco(float R0, float R, float I, float &rho, float &theta);
  void uImpedanceToRefcoRI(float R0, float R, float I, float &GR, float &GI);
  void uSeriesImpedanceToS21DB(float R0, float R, float I, float &db, float &deg);
  void uShuntImpedanceToS21DB(float R0, float R, float I, float delay, float freq, float &db, float &deg);
  void uRefcoToSeriesS21DB(float rho, float theta, float &db, float &deg);
  void uRefcoToShuntS21DB(float rho, float theta, float &db, float &deg);
  int uParseRLC(QString spec, QString &connect, float &R, float &L, float &C, float &QL, float &QC, float &D, QString &coaxSpecs);
  void uComboImpedance(QString connect, float R, float L, float C, float QL, float QC, float freq, float &Zr, float &Zi);
  int uRLCComboResponse(QString spec, float Z0, QString jig);
  void uPhaseShiftImpedance(float R0, float theta, float &Zr, float &Zi);
  void uHideCommandItem(int hParent, int itemID);
  void uHideSubMenu(int hParent, int position);
  void uShowMenuItem(int hParent, int itemID, int hMenu, QString caption, int precedeNum);
  void uDrawMenu(int hWind  );
  void uSubMenuHandle(int hParent, int subPosition);
  void uMenuItemID(int hSub, int itemPosition);
  void uGrayMenu(int hSub, int hItemID);
  void uEnableMenu(int hSub, int hItemID);
  void uCheckMenu(int hSub, int hItemID);
  void uUncheckMenu(int hSub, int hItemID);
  void uChangeMenuState(int hSub, int hItemID, int newStateFlags);
  void uCheckMenuRadioItem(int hSubMenu, int posFirst, int posLast, int posCheck);
  void uSetWindowText(int hWin, QString newText);
  void uBringWindowToFront(int hWin);
  void uGetWindowBounds(int hWin, int &topLeftX, int &topLeftY, int &botRightX, int &botRightY);
  void uGetWindowWidthHeight(int hWindow, int &width, int &height);
  void uMoveWindow(int hWindow, int xOrg, int yOrg, int width, int height);
  void uBestFitLines(int dataType1, int dataType2, int nPoints, int startStep, int endStep);
  void uBestFitLine(int dataType, int startStep, int endStep, int &m, int &b);
  void uExtendCalPlane(float freq, float &phase, float extend, int isReflect);
  void uTransformShuntJigRefResist(int origRef, int S21DB, int S21Deg, int newRef, int &newS21DB, int &newS21Deg);
  void uTransformSeriesJigRefResist(int origRef, int S21DB, int S21Deg, int newRef, int &newS21DB, int &newS21Deg);
  void uEquivParallelRLC(int freq, int serR, int serReact, int &parR, int &parL, int &parC);
  void uEquivSeriesLC(int freq, int serR, int serReact,  int &serL, int &serC);
  void uEquivParallelImped(int sR, int sX, int &pR, int &pX);

  float NormalizePhase(float p);

  int uWorkArrayFromTextArray(int nLines, int nPerLine);
  int  uArrayFromFile(QFile *fHndl, int nPerLine);   //get points from file into uWorkArray();
  void uArrayFromString(QString s, int startN, int &startPos, int nPerLine);   //get points from string into uWorkArray();
  void uHighlightText(int handle);  //handle$ is handle variable for the target text box to highlight
  void uSleep(int ms);
  void uTickCount();   //Return windows tick count ver116-1b
  void uParsePath(QString fullPath, QString &folder, QString &file); //Parse full path name of file into the file name and folder name (no slash at end);
  int uConfirmSave(QString fullPath); //if file fullPath$ exists, then ask user to confirm saving file. Return 1 if save, 0 if cancel.
  QString uAddExtension(QString name, QString exten);    //Add extension to path or file name if there is no extension
  void uDeleteFile(QString fullName);   //Kill file if it exists; no error if it doesn//t
  int uFileOrFolderExists(QString Path); // checks for the existence of the given file or folder
  QString  uSaveFileDialog(QWidget *parent, QString filter, QString defaultExt, QString initialDir, QString initialFile, QString windTitle);
  QString uOpenFileDialog(QWidget *parent, QString filter, QString defaultExt, QString initialDir, QString initialFile, QString windTitle);
  bool uVerifyDLL(QString dllName);
  QString uExtractFontColor(QString font);  //Return the color from a font specification
  void uSeriesRLCFromPoints(float Z1r, float Z1i, float f1, float Z2r, float Z2i, float f2, float &R, float &L, float &C);   //From Z at two points, calculate series RLC components
  void uParallelRLCFromPoints(float Z1r, float Z1i, float f1, float Z2r, float Z2i, float f2, float &R, float &L, float &C);   //From Z at two points, calculate parallel RLC components


  QString fixColor(QString col);
  QColor fixColor1(QString col);

private:
  float uKRadsPerDegree;
  float uKDegreesPerRad;
  float uKPi;
  float uKE;
  float uKNatLog10;


};

#endif // MSAUTILITIES_H
