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

class lptFunctions
{
public:
  lptFunctions();


  void Read8Bitmag();
  void Read8Bitpha();
  void Read12Bitmag();
  void Read12Bitpha();
  void Process16MagPha();
  int Process16Mag();
  void Process22MagPha();
  void Process22Mag();
  void CommandPLLorig();
  void CommandPLLslim();
  void CommandOrigCB();
  void CommandDDS1OrigCB();
  void CommandDDS1OrigCBserial();
  void CommandPLL1OrigCB();
  void CommandDDS3OrigCB();
  void CommandPLL3OrigCB();
  void CommandPDMOrigCB();
  void CommandPDMSlimCB();
  void CommandAllSlims();
  void ReadAD16Status();
  void ReadAD22Status();
  void ResetDDS1par();
  void ResetDDS1ser();
  void ResetDDS3ser();
  void ResetDDS1serSLIM();
  void ResetDDS3serSLIM();
  void LPTportTest();
  void CommandFilterOrigCB(int &fbank);
  void CommandFilterSlimCB(int  &fbank);



};

#endif // LPTFUNCTIONS_H
