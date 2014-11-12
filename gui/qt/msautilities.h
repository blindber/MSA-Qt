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
#include "vectortypes.h"
#include "complexmaths.h"

class msaUtilities
{
public:
  msaUtilities();
  QString uGetLine(QString inStr, int &startPos);
  QString Word(QString strExp, int n);
  QString uExtractTextItem(QString &data, QString delim);
  int uExtractNumericItems(int nItems, QString &data, QString delims, double &val1, double &val2, double &val3);
  QString uExtractDataItem(QString &data, QString delims);
  int uIsNumeric(QString data);
  QString ToHex(int value);
  void beep();
  QString time(QString t);
  QString usingF(QString form, double v);
  QString readLine(QFile *fHndl, char delm);
  QString Space(int N);

  double uSinh(double x);
  double uCosh(double x);       //sinh of real number
  double uTanh(double x);       //sinh of real number
  double uValWithMult(QString data);   //Return value of number which end with a multipler (e.g. K, M,...);
  QString uRepeat(QString s, int n);
  QString  uGetParamText(QString s,QString key, QString delim); //Return parameter text following the text in key$, to but not including the delim$ char
  QString uFormatByDig(double v, int maxWhole, int maxDec, int maxSig);
  QString uFormatted(double v, QString form);
  QString uCompact(QString str);
  void uScaleWithMultiplier(double  &v, QString &mult);
  double uMultiplierValue(QString mult);    //Return value of specified multiplier
  QString uPrompt(QString caption, QString msg, int doYesNo, int allowCancel);   //Post message and return user response

  double uRadsPerDegree();
  double uDegreesPerRad();
  double uPi();
  double uNatLog10();
  double uE();
  double uSafeLog10(double aVal);
  double uLog10(double aVal);
  int uRoundDown(double x);
  int uRoundUp(double x);
  int uRoundUpToMultiple(double X, int mult);
  int uRoundDownToMultiple(double X, int mult);
  double uRoundUpToPower(double X, double base);
  double uRoundDownToPower(double x, double base);
  double uTenPower(double pow);
  double uPower(double x, double pow);
  double uATan2(double r, double i);
  double uNormalizeDegrees(double deg);

  QString uAlignDecimalInString(QString v, int lenInDigits, int nLeft);
  QString uScientificNotation(double v, int nDec, int padZero);
  void uCrystalParameters(double Fs, double Fp, double PeakS21DB, double Fdb3A, double Fdb3B, double &Rm, double &Cm, double &Lm, double &Cp, double &Qu, double &QL);
  void uParallelRLCFromScalarS21(double Fp, double PeakS21DB, double Fdb3A, double Fdb3B, double &parR, double &L, double &C, double &Qu, double &QL, double &serR);
  void uRLCFromTwoImpedances(QString connectType, double F1, double R1, double X1, double F2, double R2, double X2, double &R, double &L, double &C);
  void uShuntJigImpedance(double R0, double S21DB, double S21Deg, double delay, double freq, double *Res, double &React);
  void uAdjustS21ForConnectorDelay(double freq, double &S21DB, double &S21Ang);
  void uSeriesJigImpedance(double R0, double S21DB, double S21Deg, double &Res, double &React);
  void uS11DBToImpedance(double R0, double S11DB, double S11Deg, double &Res, double &React);
  void uRefcoToImpedance(double R0, double rho, double theta, double &Res, double &React);
  void uImpedanceToRefco(double R0, double R, double I, double &rho, double &theta);
  void uImpedanceToRefcoRI(double R0, double R, double I, double &GR, double &GI);
  void uSeriesImpedanceToS21DB(double R0, double R, double I, double &db, double &deg);
  void uShuntImpedanceToS21DB(double R0, double R, double I, double delay, double freq, double &db, double &deg);
  void uRefcoToSeriesS21DB(double rho, double theta, double &db, double &deg);
  void uRefcoToShuntS21DB(double rho, double theta, double &db, double &deg);
  int uParseRLC(QString spec, QString &connect, double &R, double &L, double &C, double &QL, double &QC, double &D, QString &coaxSpecs);
  void uComboImpedance(QString connect, double R, double L, double C, double QL, double QC, double freq, double &Zr, double &Zi);
  int uRLCComboResponse(QString spec, double Z0, QString jig, int uWorkNumPoints, Q2DfloatVector &uWorkArray);
  void uPhaseShiftImpedance(double R0, double theta, double &Zr, double &Zi);
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
  void uEquivSeriesLC(double freq, double serR, double serReact,  double &serL, double &serC);
  void uEquivParallelImped(double sR, double sX, double &pR, double &pX);

  double NormalizePhase(double p);

  int uWorkArrayFromTextArray(int nLines, int nPerLine);
  int  uArrayFromFile(QFile *fHndl, int nPerLine);   //get points from file into uWorkArray();
  void uArrayFromString(QString s, int startN, int &startPos, int nPerLine);   //get points from string into uWorkArray();
  void uHighlightText(int handle);  //handle$ is handle variable for the target text box to highlight
  void uSleep(int ms);
  void uTickCount();   //Return windows tick count
  void uParsePath(QString fullPath, QString &folder, QString &file); //Parse full path name of file into the file name and folder name (no slash at end);
  int uConfirmSave(QString fullPath); //if file fullPath$ exists, then ask user to confirm saving file. Return 1 if save, 0 if cancel.
  QString uAddExtension(QString name, QString exten);    //Add extension to path or file name if there is no extension
  void uDeleteFile(QString fullName);   //Kill file if it exists; no error if it doesn//t
  int uFileOrFolderExists(QString Path); // checks for the existence of the given file or folder
  QString  uSaveFileDialog(QWidget *parent, QString filter, QString defaultExt, QString initialDir, QString initialFile, QString windTitle);
  QString uOpenFileDialog(QWidget *parent, QString filter, QString defaultExt, QString initialDir, QString initialFile, QString windTitle);
  bool uVerifyDLL(QString dllName);
  QString uExtractFontColor(QString font);  //Return the color from a font specification
  void uSeriesRLCFromPoints(double Z1r, double Z1i, double f1, double Z2r, double Z2i, double f2, double &R, double &L, double &C);   //From Z at two points, calculate series RLC components
  void uParallelRLCFromPoints(double Z1r, double Z1i, double f1, double Z2r, double Z2i, double f2, double &R, double &L, double &C);   //From Z at two points, calculate parallel RLC components


  QString fixColor(QString col);
  QColor fixColor1(QString col);

private:
  double uKRadsPerDegree;
  double uKDegreesPerRad;
  double uKPi;
  double uKE;
  double uKNatLog10;

  ComplexMaths complexMaths;


};

#endif // MSAUTILITIES_H
