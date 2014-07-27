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
#ifndef LPTFUNCTIONS_H
#define LPTFUNCTIONS_H
#include <QtCore>
#include "constants.h"
#ifdef __WIN32__
#include <windows.h>
#endif
//cmdallarray;//(2020,40) //(0-15)DDS1+DDS3, (16-39)PLL1+DDS1+PLL3+DDS3
class ccmdAllArray
{
public:
  int array[40];
};

enum lptLibType
{
  none = -1,
  ntPortLin = 0,
  userPortLib = 1,
  inpoutLib = 2
};
struct nValues
{
  int N0;
  int N1;
  int N2;
  int N3;
  int N4;
  int N5;
  int N6;
  int N7;
  int N8;
  int N9;
  int N10;
  int N11;
  int N12;
  int N13;
  int N14;
  int N15;
  int N16;
  int N17;
  int N18;
  int N19;
  int N20;
  int N21;
  int N22;
  int N23;
};

struct statarrayforreadad
{
  int stat15;
  int stat14;
  int stat13;
  int stat12;
  int stat11;
  int stat10;
  int stat9;
  int stat8;
  int stat7;
  int stat6;
  int stat5;
  int stat4;
  int stat3;
  int stat2;
  int stat1;
  int stat0;
};

class lptFunctions
{
public:
  lptFunctions();
  int init(int libraryType, int port);
  void setLatchLines(int le1, int le2, int le3, int fqud1, int fqud3);

  QVector<ccmdAllArray> cmdAllArray;
  statarrayforreadad stats;

  void Read8Bitmag();
  void Read8Bitpha();
  void Read12Bitmag();
  void Read12Bitpha();
  void Process16MagPha(int &magdata, int &phadata);
  void Process16Mag(int &magdata);
  void Process22MagPha();
  void Process22Mag();
  void CommandPLLorig();
  void CommandPLLslim(int filtbank, int datavalue, int pdmcommand, nValues *n, int levalue);
  void CommandOrigCB();
  void CommandDDS1OrigCB();
  void CommandDDS1OrigCBserial();
  void CommandPLL1OrigCB();
  void CommandDDS3OrigCB();
  void CommandPLL3OrigCB();
  void CommandPDMOrigCB();
  void CommandPDMSlimCB();
  void CommandAllSlims(int step, int filtbank, int pdmcmd);
  void ReadAD16Status();
  void ReadAD22Status();
  void ResetDDS1par();
  void ResetDDS1ser();
  void ResetDDS3ser();
  void ResetDDS1serSLIM(int pdmcmd, int filtbank);
  void ResetDDS3serSLIM(int pdmcmd, int filtbank);
  void LPTportTest();
  void SetLPTControl();
  void CommandFilterOrigCB(int &fbank);
  void CommandFilterSlimCB(int fbank);
  void Read16wSlimCB();

  int input(short port);
  void output(short port, short value);
private:
  QLibrary *lptLib;

//--------------inpout-------------------------
  typedef void	(*lpOut32)(short, short);
  typedef short	(*lpInp32)(short);
  typedef bool	(*lpIsInpOutDriverOpen)(void);
  typedef bool	(*lpIsXP64Bit)(void);

  lpOut32 lpoutport;
  lpInp32 lpinport;
  lpIsInpOutDriverOpen isDriverOpen;
  lpIsXP64Bit is64Bit;
//---------------------------------------------

#ifdef __WIN32__
  //unsigned short  Inp(unsigned short PortNum);
  typedef void (APIENTRY *Outp)(unsigned short, unsigned short);
  //typedef void (APIENTRY *Outpw)(unsigned short PortNum, unsigned short Data);
  //typedef void (APIENTRY *Outpd)(unsigned short PortNum, unsigned long Data);
  //typedef void (APIENTRY *Outport)(unsigned short PortNum, unsigned short Data);
  //typedef void (APIENTRY *OutportW)(unsigned short PortNum, unsigned short Data);
  //typedef void (APIENTRY *OutportD)(unsigned short PortNum, unsigned long Data);
  //typedef void (APIENTRY *LicenseInfo)(LPSTR sUserName, unsigned long lKey);
  //typedef void (APIENTRY *EnablePorts)(unsigned short PortStart, unsigned short PortStop);
  //typedef void (APIENTRY *DisablePorts)(unsigned short PortStart, unsigned short PortStop);
  //typedef bool (APIENTRY *GetFastMode)();
  //typedef int (APIENTRY *GetLastState)(char *sStatus);
  //typedef unsigned short (APIENTRY *GetLPTPortAddress)(unsigned short portID);
  //typedef unsigned short (APIENTRY *GetNTPortVersion)();
  //typedef bool (APIENTRY *IsWin64)();
  //typedef bool (APIENTRY *IsWinNT)();
  typedef unsigned short (APIENTRY *Inp)(unsigned short PortNum);
  //typedef unsigned short (APIENTRY *Inpw)(unsigned short PortNum);
  //typedef unsigned long (APIENTRY *Inpd)(unsigned short PortNum);
  //typedef unsigned short (APIENTRY *Inport)(unsigned short PortNum);
  //typedef unsigned short (APIENTRY *InportW)(unsigned short PortNum);
  //typedef unsigned long (APIENTRY *InportD)(unsigned short PortNum);
#else
  typedef void ( *Outp)(unsigned short, unsigned short);
  typedef unsigned short ( *Inp)(unsigned short PortNum);
#endif



  //typedef unsigned long (APIENTRY *GetFileVersionInfoSizeProtoType)(LPCTSTR, LPunsigned long);
  Outp ntoutport;
  Inp ntinport;

  int libraryType;
  int port;
  int status;
  int control;
  int le1, le2, le3, fqud1, fqud3;
};

#endif // LPTFUNCTIONS_H
