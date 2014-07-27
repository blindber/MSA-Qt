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
#include "lptfunctions.h"
#include <QMessageBox>
#include "constants.h"
#include <QDebug>

lptFunctions::lptFunctions()
{
  libraryType = none;
}

int lptFunctions::init(int libraryType, int port)
{
  this->port = port;
  status = port + 1;
  control = port + 2;
#ifndef __WIN32__
  return false;
#endif
  this->libraryType = libraryType;
  if (libraryType == inpoutLib)
  {
    lptLib = new QLibrary("inpout32.dll");

    if (!lptLib->load())
    {
      QMessageBox::about(0, "DLL Error", lptLib->errorString() );
      return false;
    }

    lpoutport = (lpOut32) lptLib->resolve("Out32");
    if (lpoutport)
    {
      lpinport = (lpInp32) lptLib->resolve("Inp32");

      isDriverOpen = (lpIsInpOutDriverOpen) lptLib->resolve("IsInpOutDriverOpen");
      is64Bit = (lpIsXP64Bit) lptLib->resolve("IsXP64Bit");
    }
  }
  else if (libraryType == ntPortLin)
  {
    lptLib = new QLibrary("ntport.dll");

    if (!lptLib->load())
    {
      QMessageBox::about(0, "DLL Error", lptLib->errorString() );
      return false;
    }

    ntoutport = (Outp) lptLib->resolve("Outp");
    if (ntoutport)
    {
      ntinport = (Inp) lptLib->resolve("Inp");
    }

  }
  else if (libraryType == userPortLib)
  {

  }
  else if (libraryType == none)
  {
    qDebug() << "No LPT library type selected";
  }
  else
  {
    qDebug() << "No invalid library type selected";
  }

  return 0;
}

void lptFunctions::setLatchLines(int le1, int le2, int le3, int fqud1, int fqud3)
{
  this->le1 = le1;
  this->le2 = le2;
  this->le3 = le3;
  this->fqud1 = fqud1;
  this->fqud3 = fqud3;
}

void lptFunctions::Read8Bitmag()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
/*
'needed: port,status ; creates: magdata
    'This will 8 bit parallel on Original or Slim Control Board. ver111-29
    ' if inp(status) < 128 then wait (status bit 7) line is high (D/A<magnitude voltage)
    ' if inp(status) > 127 then wait line is low (magnitude voltage<ladder)
    'the data will always be 1 bit less than crossover region
    magdata = 0
    if cb = 2 then out control, AUTO 'enable P3 on SLIM Control Board. ver111-29
    if cb = 3 then return       ' not supportd on USB 'USB:01-08-2010
    out port, magdata + 128    'set D/A MSB to "1", creates 2.9v out of D/A ladder, half max.
    if inp(status) < 128 then magdata = magdata + 128
    out port, magdata + 64
    if inp(status) < 128 then magdata = magdata + 64
    out port, magdata + 32
    if inp(status) < 128 then magdata = magdata + 32
    out port, magdata + 16
    if inp(status) < 128 then magdata = magdata + 16
    out port, magdata + 8
    if inp(status) < 128 then magdata = magdata + 8
    out port, magdata + 4
    if inp(status) < 128 then magdata = magdata + 4
    out port, magdata + 2
    if inp(status) < 128 then magdata = magdata + 2
    out port, magdata + 1
    if inp(status) < 128 then magdata = magdata + 1
    out port, 0    'return data to zero
    if cb = 2 then out control, contclear 'disable P3 on SLIM Control Board. ver111-29
    return  'return, with magdata
  */
}
void lptFunctions::Read8Bitpha()
{
qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  //needed: port,status ; creates: phadata
      //read 8 bit parallel on Original or Slim Control Board. ver111-29
      // if inp(status) > 63 then ack line is high (phase voltage>ladder)
      //the data will always be 1 bit less than crossover region
/*      phadata = 0;
      //if (activeConfig.cb = 2) then out control, AUTO //enable P3 on SLIM Control Board. ver111-29
      if (activeConfig.cb == 3) return;       // not supportd on USB //USB:01-08-2010

      out port, phadata + 128    //set D/A MSB to "1", creates 2.5v out of D/A ladder
      magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 128
      out port, phadata + 64
      magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 64
      out port, phadata + 32
      magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 32
      out port, phadata + 16
      magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 16
      out port, phadata + 8
      magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 8
      out port, phadata + 4
      magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 4
      out port, phadata + 2
      magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 2
      out port, phadata + 1
      magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 1
      out port, 0    //return data to zero
      if cb = 2 then out control, contclear //disable P3 on SLIM Control Board. ver111-29
      return  //return, with phadata
          */
}

void lptFunctions::Read12Bitmag()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed: port,status ; creates: magdata
    'This will read 12 bit parallel, WAIT line, on Original or Slim Control Board. ver111-29
    ' if inp(status) < 128 then wait line (status bit 7)is high (D/A<magnitude voltage)
    ' if inp(status) > 127 then wait line is low (magnitude voltage<ladder)
    'the data will always be 1 bit less than crossover region
    if cb = 2 then out control, AUTO 'enable P3 on SLIM Control Board. ver111-29
    if cb = 3 then return       ' not supported on USB 'USB:01-08-2010
    out port,0    'present zero to latches
    out port,0 +64 +32 +16  'latch 0 into all 3 latches
    magdata = 0
    word = 0
    out port,8 +32 +16  ' put 8 into MSB latch, D/A = 2048 creates 2.5v out of D/A ladder, ver105
    if inp(status) < 128 then magdata = magdata + 2048: word = word + 8
    out port,word +4 +32 +16  ' put 4 into MSB latch, D/A = 1024
    if inp(status) < 128 then magdata = magdata + 1024: word = word + 4
    out port,word +2 +32 +16  ' put 2 into MSB latch, D/A = 512
    if inp(status) < 128 then magdata = magdata + 512: word = word + 2
    out port,word +1 +32 +16  ' put 1 into MSB latch, D/A = 256
    if inp(status) < 128 then magdata = magdata + 256: word = word + 1
    out port,word +32 +16  ' put appxm. into MSB latch, D/A = appx
    out port,word +64 +32 +16  ' latch MSB with appx

    word = 0
    out port,8 +64 +16  ' put 8 into middle latch, D/A = 128
    if inp(status) < 128 then magdata = magdata + 128: word = word + 8
    out port,word +4 +64 +16  ' put 4 into middle latch, D/A = 64
    if inp(status) < 128 then magdata = magdata + 64: word = word + 4
    out port,word +2 +64 +16  ' put 2 into middle latch, D/A = 32
    if inp(status) < 128 then magdata = magdata + 32: word = word + 2
    out port,word +1 +64 +16  ' put 1 into middle latch, D/A = 16
    if inp(status) < 128 then magdata = magdata + 16: word = word + 1
    out port,word +64 +16  ' put appxm. into middle latch, D/A = appx
    out port,word +64 +32 +16  ' latch middle with appx

    word = 0
    out port,8 +32 +64  ' put 8 into LSB latch, D/A = 8
    if inp(status) < 128 then magdata = magdata + 8: word = word + 8
    out port,word +4 +32 +64  ' put 4 into LSB latch, D/A = 4
    if inp(status) < 128 then magdata = magdata + 4: word = word + 4
    out port,word +2 +32 +64  ' put 2 into LSB latch, D/A = 2
    if inp(status) < 128 then magdata = magdata + 2: word = word + 2
    out port,word +1 +32 +64  ' put 1 into LSB latch, D/A = 1
    if inp(status) < 128 then magdata = magdata + 1
    out port, 0    'return data to zero
    if cb = 2 then out control, contclear 'disable P3 on SLIM Control Board. ver111-29
    return  'return with magdata

*/
}

void lptFunctions::Read12Bitpha()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
//needed: port,status ; creates: phadata
    //This will read 12 bit parallel, ACK line, on Original or Slim Control Board. ver111-29
    // if inp(status) > 63 then ack line is high (phase voltage>ladder)
    // if inp(status) < 64 then ack line is low (phase voltage<ladder)
    if cb = 2 then out control, AUTO //enable P3 on SLIM Control Board. ver111-29
    if cb = 3 then return       // not supportd on USB //USB:01-08-2010
    out port,0    //preset to zero
    out port,0 +64 +32 +16  //latch 0 into all 3 latches
    phadata = 0
    word = 0
    out port,8 +16 +32  // put 8 into MSB latch, D/A = 2048 creates 2.5v out of D/A ladder, ver105
     magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 2048: word = word + 8
    out port,word +4 +16 +32  // put 4 into MSB latch, D/A = 1024
     magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 1024: word = word + 4
    out port,word +2 +16 +32  // put 2 into MSB latch, D/A = 512
     magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 512: word = word + 2
    out port,word +1 +16 +32  // put 1 into MSB latch, D/A = 256
     magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 256: word = word + 1
    out port,word +16 +32  // put appxm. into MSB latch, D/A = appx
    out port,word +16 +32 +64  // latch MSB with appx

    word = 0
    out port,8 +16 +64  // put 8 into middle latch, D/A = 128
     magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 128: word = word + 8
    out port,word +4 +16 +64  // put 4 into middle latch, D/A = 64
     magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 64: word = word + 4
    out port,word +2 +16 +64  // put 2 into middle latch, D/A = 32
     magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 32: word = word + 2
    out port,word +1 +16 +64  // put 1 into middle latch, D/A = 16
     magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 16: word = word + 1
    out port,word +16 +64  // put appxm. into middle latch, D/A = appx
    out port,word +16 +32 +64  // latch middle with appx

    word = 0
    out port,8 +32 +64  // put 8 into LSB latch, D/A = 8
     magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 8: word = word + 8
    out port,word +4 +32 +64  // put 4 into LSB latch, D/A = 4
     magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 4: word = word + 4
    out port,word +2 +32 +64  // put 2 into LSB latch, D/A = 2
     magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 2: word = word + 2
    out port,word +1 +32 +64  // put 1 into LSB latch, D/A = 1
     magpha = inp(status)
      if magpha > 127 then magpha = magpha - 128
      if magpha > 63 then phadata = phadata + 1
    out port, 0    //return data to zero
    if cb = 2 then out control, contclear //disable P3 on SLIM Control Board. ver111-29
    return  // return with phadata

*/
}
void lptFunctions::Process16MagPha(int &magdata, int &phadata)
{
  static int counter = -90;
  counter ++;
  //process the stat15-0 for both magnitude and phase.Determines magdata bit (D7) and phadata bit (D6) in each word
  //if cb = 3 then return //magdata and phadata is already processed. //ver116-4r
  magdata = 0;
  phadata = 0;
  if (stats.stat15>127) { stats.stat15=stats.stat15-128;magdata = magdata + 32768; }  //WAIT is low, MAG is high
  if (stats.stat15<64) { phadata = phadata + 32768; }  //ACK is low, PHASE is high
  if (stats.stat14>127) { stats.stat14=stats.stat14-128;magdata = magdata + 16384; }
  if (stats.stat14<64) { phadata = phadata + 16384; }
  if (stats.stat13>127) { stats.stat13=stats.stat13-128;magdata = magdata + 8192; }
  if (stats.stat13<64) { phadata = phadata + 8192; }
  if (stats.stat12>127) { stats.stat12=stats.stat12-128;magdata = magdata + 4096; }
  if (stats.stat12<64) { phadata = phadata + 4096; }
  if (stats.stat11>127) { stats.stat11=stats.stat11-128;magdata = magdata + 2048; }
  if (stats.stat11<64) { phadata = phadata + 2048; }
  if (stats.stat10>127) { stats.stat10=stats.stat10-128;magdata = magdata + 1024; }
  if (stats.stat10<64) { phadata = phadata + 1024; }
  if (stats.stat9>127) { stats.stat9=stats.stat9-128;magdata = magdata + 512; }
  if (stats.stat9<64) { phadata = phadata + 512; }
  if (stats.stat8>127) { stats.stat8=stats.stat8-128;magdata = magdata + 256; }
  if (stats.stat8<64) { phadata = phadata + 256; }
  if (stats.stat7>127) { stats.stat7=stats.stat7-128;magdata = magdata + 128; }
  if (stats.stat7<64) { phadata = phadata + 128; }
  if (stats.stat6>127) { stats.stat6=stats.stat6-128;magdata = magdata + 64; }
  if (stats.stat6<64) { phadata = phadata + 64; }
  if (stats.stat5>127) { stats.stat5=stats.stat5-128;magdata = magdata + 32; }
  if (stats.stat5<64) { phadata = phadata + 32; }
  if (stats.stat4>127) { stats.stat4=stats.stat4-128;magdata = magdata + 16; }
  if (stats.stat4<64) { phadata = phadata + 16; }
  if (stats.stat3>127) { stats.stat3=stats.stat3-128;magdata = magdata + 8; }
  if (stats.stat3<64) { phadata = phadata + 8; }
  if (stats.stat2>127) { stats.stat2=stats.stat2-128;magdata = magdata + 4; }
  if (stats.stat2<64) { phadata = phadata + 4; }
  if (stats.stat1>127) { stats.stat1=stats.stat1-128;magdata = magdata + 2; }
  if (stats.stat1<64) { phadata = phadata + 2; }
  if (stats.stat0>127) { stats.stat0=stats.stat0-128;magdata = magdata + 1; }
  if (stats.stat0<64) { phadata = phadata + 1; }
  return;  //to [ReadPhase] with magdata and phadata

  magdata = counter;
  phadata = counter;

}

void lptFunctions::Process16Mag(int &magdata)
{
    //process the (stats.stat15-0 for magnitude only. Determines magdata bit (D7) in each word
    //if cb = 3) return //magdata is already processed (along with phadata) //ver116-4r
    magdata = 0;
    if (stats.stat15>127) magdata = magdata + 32768;  //WAIT is low, MAG is high
    if (stats.stat14>127) magdata = magdata + 16384;  //WAIT is low, MAG is high
    if (stats.stat13>127) magdata = magdata + 8192;  //WAIT is low, MAG is high
    if (stats.stat12>127) magdata = magdata + 4096;  //WAIT is low, MAG is high
    if (stats.stat11>127) magdata = magdata + 2048;  //WAIT is low, MAG is high
    if (stats.stat10>127) magdata = magdata + 1024;  //WAIT is low, MAG is high
    if (stats.stat9>127) magdata = magdata + 512;  //WAIT is low, MAG is high
    if (stats.stat8>127) magdata = magdata + 256;  //WAIT is low, MAG is high
    if (stats.stat7>127) magdata = magdata + 128;  //WAIT is low, MAG is high
    if (stats.stat6>127) magdata = magdata + 64;  //WAIT is low, MAG is high
    if (stats.stat5>127) magdata = magdata + 32;  //WAIT is low, MAG is high
    if (stats.stat4>127) magdata = magdata + 16;  //WAIT is low, MAG is high
    if (stats.stat3>127) magdata = magdata + 8;  //WAIT is low, MAG is high
    if (stats.stat2>127) magdata = magdata + 4;  //WAIT is low, MAG is high
    if (stats.stat1>127) magdata = magdata + 2;  //WAIT is low, MAG is high
    if (stats.stat0>127) magdata = magdata + 1;  //WAIT is low, MAG is high
    return; //to [ReadMagnitude]with magdata


}
void lptFunctions::Process22MagPha()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
//ver111-37a
    //process the stat11-0 for both magnitude and phase.Determines magdata bit (D7) and phadata bit (D6) in each word
    if cb = 3 then return //magdata and phadata already processed. //ver116-4r
    magdata = 0
    phadata = 0
    if stat11>127 then stat11=stat11-128:magdata = magdata + 2048  //WAIT is low, MAG is high
    if stat11<64 then phadata = phadata + 2048  //ACK is low, PHASE is high
    if stat10>127 then stat10=stat10-128:magdata = magdata + 1024
    if stat10<64 then phadata = phadata + 1024
    if stat9>127 then stat9=stat9-128:magdata = magdata + 512
    if stat9<64 then phadata = phadata + 512
    if stat8>127 then stat8=stat8-128:magdata = magdata + 256
    if stat8<64 then phadata = phadata + 256
    if stat7>127 then stat7=stat7-128:magdata = magdata + 128
    if stat7<64 then phadata = phadata + 128
    if stat6>127 then stat6=stat6-128:magdata = magdata + 64
    if stat6<64 then phadata = phadata + 64
    if stat5>127 then stat5=stat5-128:magdata = magdata + 32
    if stat5<64 then phadata = phadata + 32
    if stat4>127 then stat4=stat4-128:magdata = magdata + 16
    if stat4<64 then phadata = phadata + 16
    if stat3>127 then stat3=stat3-128:magdata = magdata + 8
    if stat3<64 then phadata = phadata + 8
    if stat2>127 then stat2=stat2-128:magdata = magdata + 4
    if stat2<64 then phadata = phadata + 4
    if stat1>127 then stat1=stat1-128:magdata = magdata + 2
    if stat1<64 then phadata = phadata + 2
    if stat0>127 then stat0=stat0-128:magdata = magdata + 1
    if stat0<64 then phadata = phadata + 1
    return  //to [ReadPhase] with magdata and phadata
*/
}

void lptFunctions::Process22Mag()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'ver111-37a
    'process the stat11-0 for magnitude only. Determines magdata bit (D7) in each word
    if cb = 3 then return 'magdata already processed. 'ver116-4r
    magdata = 0
    if stat11>127 then magdata = magdata + 2048  'WAIT is low, MAG is high
    if stat10>127 then magdata = magdata + 1024  'WAIT is low, MAG is high
    if stat9>127 then magdata = magdata + 512  'WAIT is low, MAG is high
    if stat8>127 then magdata = magdata + 256  'WAIT is low, MAG is high
    if stat7>127 then magdata = magdata + 128  'WAIT is low, MAG is high
    if stat6>127 then magdata = magdata + 64  'WAIT is low, MAG is high
    if stat5>127 then magdata = magdata + 32  'WAIT is low, MAG is high
    if stat4>127 then magdata = magdata + 16  'WAIT is low, MAG is high
    if stat3>127 then magdata = magdata + 8  'WAIT is low, MAG is high
    if stat2>127 then magdata = magdata + 4  'WAIT is low, MAG is high
    if stat1>127 then magdata = magdata + 2  'WAIT is low, MAG is high
    if stat0>127 then magdata = magdata + 1  'WAIT is low, MAG is high
    return 'to [ReadMagnitude]with magdata
*/
}
void lptFunctions::CommandPLLorig()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111-28
    'used during initialization of PLL1, PLL2, and PLL3.  PDM will get set to "0".
    'when PLL1 or PLL2 then Jcontrol=SELT. when PLL3 the Jcontrol=INIT
    out control, Jcontrol   'enable Control Board J connector
    out port, N23:out port, N23 + 2
    out port, N22:out port, N22 + 2:out port, N21:out port, N21 + 2
    out port, N20:out port, N20 + 2:out port, N19:out port, N19 + 2
    out port, N18:out port, N18 + 2:out port, N17:out port, N17 + 2
    out port, N16:out port, N16 + 2:out port, N15:out port, N15 + 2
    out port, N14:out port, N14 + 2:out port, N13:out port, N13 + 2
    out port, N12:out port, N12 + 2:out port, N11:out port, N11 + 2
    out port, N10:out port, N10 + 2:out port, N9:out port, N9 + 2
    out port, N8:out port, N8 + 2:out port, N7:out port, N7 + 2
    out port, N6:out port, N6 + 2:out port, N5:out port, N5 + 2
    out port, N4:out port, N4 + 2:out port, N3:out port, N3 + 2
    out port, N2:out port, N2 + 2:out port, N1:out port, N1 + 2
    out port, N0:out port, N0 + 2:out port, LEPLL:out port, 0     'Latch buffer
    out control, contclear       'Disable the Control Board J connector
    return 'to [CommandPLL]
    */
}
void lptFunctions::CommandPLLslim(int filtbank, int datavalue, int pdmcommand, nValues *n, int levalue)
{
  //needs:datavalue,levalue,N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,SLIM ControlBoard ver111-28
  //used during initialization of PLL1, PLL2, and PLL3.  PDM will get set to "0" during Initializations
  //selt word = 1 common clock, 4 datas, plus 3 (filtbank). entering this sub, selt word should = filtbank only
  //init word = 5 latch lines plus 2 pdm commands. entering this sub, init word should = pdmcmd + pdmclk only.ver111-39d
  //two steps to do: command data and clock without disturbing Filter Bank, then send LE without disturbing PDM
  //step 1. Command the PLL without changing the filter bank.
  //For PLL1,datavalue=2, for PLL2,datavalue=16, for PLL3,datavalue=8
  //following code lines changed in ver113-3c
  int a;
  a=filtbank + n->N23*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N22*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N21*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N20*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N19*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N18*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N17*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N16*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N15*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N14*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N13*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N12*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N11*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N10*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N9*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N8*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N7*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N6*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N5*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N4*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N3*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N2*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N1*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  a=filtbank + n->N0*datavalue;output(port, a);output(control, SELT);output(control, contclear);output(port, a+1);output(control, SELT);output(control, contclear);
  output(port, filtbank);output(control, SELT);output(control, contclear); //leaving lines latched to filter bank
  output(port, 0);
  //step 2. Command the PLL without changing the PDM
  //pdmcommand = phaarray(thisstep,0)*64 //do not disturb PDM state, this may be used during Spur Test
  output(port, pdmcommand + levalue);  //levalues: PLL1=1, PLL2=16, PLL3=4
  output(control, INIT);
  output(port, pdmcommand);
  output(control, contclear);  //leaving lines latched, and unchanged, to PDM
  output(port, 0);
}
void lptFunctions::CommandOrigCB()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CommandOrigCB]' correct modules have been determined in [DetermineModule]
    'Command necessary modules, independently, from Original Control Board
    if glitchd1 > 0 then gosub [CommandDDS1OrigCB]
    if glitchp1 > 0 then gosub [CommandPLL1OrigCB]
    if glitchd3 > 0 and TGtop > 0 then gosub [CommandDDS3OrigCB]
    if glitchp3 > 0 and TGtop > 0 then gosub [CommandPLL3OrigCB]
    if glitchpdm > 0  and msaMode$<>"SA" and msaMode$<>"ScalarTrans" then gosub [CommandPDMOrigCB] 'ver114-5n
    return 'to [CommandThisStep]
*/
}

void lptFunctions::CommandDDS1OrigCB()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*'needed:DDS1array 'ver111-21
    if dds1parser = 1 then goto void MainWindow::CommandDDS1OrigCBserial() 'ver111-21
   '(CommandDDS1OrigCBparallel)'needed:DDS1array(w0-w4),port,control,AUTO,STRB,contclear ; commands DDS1 on J5, parallel. ver111-21
        'note, a DDS commanded parallel, will begin with Control Word (W0), then MSB Word (W1), ending with LSB Word (W4)
          'set word 0        'set 8 bit word, W0 (0), phase info
        out port,DDS1array(thisstep,40) ' a "1" here would activate the x4 internal multiplier, but not recommended
        out control, AUTO       'wclk line goes high
        out control, contclear      'wclk line goes low
          'set word 1
        out port,DDS1array(thisstep,41) 'set 8 bit word, W1, MSB freq
        out control,AUTO:out control, contclear
          'set word 2
        out port,DDS1array(thisstep,42) 'W2
        out control,AUTO:out control, contclear
          'set word 3
        out port,DDS1array(thisstep,43) 'W3
        out control,AUTO:out control, contclear
         'set word 4
        out port,DDS1array(thisstep,44) 'set 8 bit word, W4, LSB freq
        out control,AUTO:out control, contclear
        out port, 0            'return the output port data lines to 0
         'send fqud
        out control, STRB          'set fqud to 1, freq changes now
        out control, contclear         'set fqud to 0 and all others to 0
    return 'to void MainWindow::CommandOrigCB()
*/
}
void lptFunctions::CommandDDS1OrigCBserial()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*'needed:DDS1array(sw0-sw39),control,AUTO,STRB,contclear ; commands DDS1 on J5, serially ver111-21
        'note: once the DDS1 has been reset into serial mode, the D0 thru D6 data lines are "don't care".
        'note, a DDS serial command, will begin with LSB (W0), thru MSB (W31), ending with Phase bit 4 (W39)
        for clmn = 0 to 39 'ver111-21
        out port, DDS1array(thisstep,clmn)*128 'apply data bit to DDS1pin25, D7 data line
        out control, AUTO:out control, contclear  'retain data bit while wclk up, then down
        next clmn 'next bit in 40 bit serial data transfer
        out port, 0
        out control, STRB:out control, contclear 'fqud up, fqud down
    return 'to void MainWindow::CommandOrigCB()
*/
}
//'void lptFunctions::endCommandDDS1OldRevA()

void lptFunctions::CommandPLL1OrigCB()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*'needed:PLL1array(N23-N0),SELT,lastncounter1,lastfcounter1 'ver111-21
    'ver111-28a makes the SELT buffer "see" the pdm state before commanding PLL1, to prevent orig PDM from changing states.
    Jcontrol = SELT : LEPLL = 4 'ver111-21
    'Command PLL1,oldControl using N23-N0,control,Jcontrol,port,contclear,LEPLL ver111-21
    'note, a PLL will serially command beginning with N23 and end with N0 (address bit)
    pdmcmd = phaarray(thisstep,0) 'ver111-39d
    out port, pdmcmd*128 'ver111-28a
    out control, Jcontrol   'enable Control Board J connector
    for clmn = 0 to 23  'reversed order 'ver111-31a
    out port, pdmcmd*128 + PLL1array(thisstep,clmn):out port, pdmcmd*128 + PLL1array(thisstep,clmn) + 2 'ver111-21 'ver111-28a
    next clmn 'ver111-21
    out port, pdmcmd*128 + LEPLL:out port, pdmcmd*128     'Latch buffer 'ver111-28a
    out control, contclear       'Disable the Control Board J connector
    out port, 0 'ver111-28a
    lastpdmstate=phaarray(thisstep,0)   'ver114-6c
    return 'to void MainWindow::CommandOrigCB()
*/
}
void lptFunctions::CommandDDS3OrigCB()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*'needed:DDS3array,lastdds3output,INIT 'ver111-18
    Jcontrol = INIT:swclk = 32:sfqud = 2 'for Orig Control Bd,J4,DDS3 ver111-16
    'Command DDS3,serially,oldControl using sw0-sw39,swclk,sfqud,control,Jcontrol,port,contclear,LEPLL ver111-21
    'note, a DDS commanded serially, will begin with LSB, continue to MSB, and end with Control Word MSB Phase Bit
    'present filter bank data while commanding DDS3, so as not to change filter bank ver111-29
    out port, filtbank 'ver111-29
    out control, Jcontrol  'enable Control Board J connector
    for clmn = 0 to 39 'ver111-21
    out port, filtbank + DDS3array(thisstep,clmn) 'apply data bit to DDS, and also filter lines 'ver111-29
    out port, filtbank + DDS3array(thisstep,clmn) + swclk  'apply data bit and wclk 'ver111-29
    next clmn
    out port, filtbank:out port, filtbank + sfqud:out port, filtbank 'last sw down and swclk down, sfqud up, sfqud down 'ver111-29
    out control, contclear  'disable J connector
    out port, 0 'ver111-29
    return 'to void MainWindow::CommandOrigCB()
*/
}
void lptFunctions::CommandPLL3OrigCB()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*'needed:PLL3array(N23-N0),INIT,lastncounter3,lastfcounter3 ver111-18
    Jcontrol = INIT : LEPLL = 16 'ver111-21
    'Command PLL3,Orig Control using N23-N0,control,Jcontrol,port,contclear,LEPLL ver111-21
    'note, a PLL will serially command beginning with N23 and end with N0 (address bit)
    'present filter bank data while commanding PLL3, so as not to change filter bank ver111-29
    out port, filtbank 'ver111-29
    out control, Jcontrol   'enable Control Board J connector
    for clmn = 0 to 23  'reversed order 'ver111-31a
    out port, filtbank + PLL3array(thisstep,clmn) 'ver111-29
    out port, filtbank + PLL3array(thisstep,clmn) + 2 'ver111-21 'ver111-29
    next clmn 'ver111-21
    out port, filtbank + LEPLL:out port, filtbank     'Latch buffer 'ver111-29
    out control, contclear       'Disable the Control Board J connector
    out port, 0 'ver111-29
    return 'to void MainWindow::CommandOrigCB()
*/
}
void lptFunctions::CommandPDMOrigCB()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*'Set original PDM phase for last known mode, since a PLL1 or PLL2 command will reset the PDM to Norm.
    out port, phaarray(thisstep,0)*128: out control, SELT: out control, contclear: out port, 0  'pdmcmd is determined in void MainWindow::InvertPDmodule() 'ver111-20
    lastpdmstate=phaarray(thisstep,0)   'ver114-6c
    return 'to void MainWindow::CommandOrigCB()orvoid MainWindow::CommandPDMonly()
*/
}
void lptFunctions::CommandPDMSlimCB()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*'also sending a "latch signal", used by orig PDM module
    out port, phaarray(thisstep,0)*64
    out control, INIT
    out port, phaarray(thisstep,0)*64 + 32
    out port, phaarray(thisstep,0)*64
    out control, contclear
    out port, 0
    lastpdmstate=phaarray(thisstep,0)   'ver114-6c
    return 'to void MainWindow::CommandPDMonly()
*/
}
void lptFunctions::CommandAllSlims(int step, int filtbank, int pdmcmd)
{
  //for SLIM Control and SLIM modules. Old PDM and old Filt Bank can be used
  //(send data and clocks without changing Filter Bank)
  //0-15 is DDS1bit*4 + DDS3bit*16, data = 0 to PLL 1 and PLL 3. seevoid MainWindow::CreateCmdAllArray().
  //present new Data with no clock,latch high,latch low,present new data with clock,latch high,latch low.
  //repeat for each bit. (40 data bits and 40 clocks for each module, even if they don//t need that many)
  //this format guarantees that the common clock will not transition with a data transition, preventing crosstalk in LPT cable.
  for (int clmn = 0; clmn <= 39; clmn++)
  {
    int a= cmdAllArray[step].array[clmn]+ filtbank;
    output(port, a);
    output(control, SELT);
    output(control, contclear); //a is the data, with clock
    output(port, a+1);
    output(control, SELT);
    output(control, contclear); //a+1 is data, plus clock
  }
  output(port, filtbank); //remove data, leaving filtbank data to filter bank.
  output(control, SELT);
  output(control, contclear); //disable buffer. filtbank signals will be latched to filter bank assembly

  //send LE//s to PLL1, PLL3, FQUD//s to DDS1, DDS3, and command PDM
  //begin by setting up init word=LE//s and Fquds + PDM state for thisstep
  //pdmcmd = phaarray(thisstep,0)*64;
  output(port, le1 + fqud1 + le3 + fqud3 + pdmcmd); //present data to buffer input
  output(control, INIT);
  output(control, contclear);  //latch the buffer, moving the signals to the 5 modules
  output(port, pdmcmd + 32); //remove LEs and Fquds, leaving PDM data, but add a latch signal P2D5 for old PDM if used.
  output(control, INIT);
  output(control, contclear);  //sends latch signal to old PDM
  output(port, pdmcmd);  //remove the added latch signal to PDM, leaving just the PDM//s static data
  output(control, INIT);
  output(control, contclear);
  output(port, 0);    //bring all Data lines low. PDM data remains static
  //lastpdmstate=phaarray(thisstep,0);
  return;

}
void lptFunctions::ReadAD16Status()
{
  //For reading the 16 bit serial AtoD. Changed 4-27-10 ver115-9b
     //This routine modified to help reject noise glitching caused by newer, faster, cheaply made computers.
     //needed: port, control, status ; creates: 16 status port words (stat15-stat0)mag,(and, pha if two A/D//s installed) //ver111-33a
     //written for Analog Devices, AD7685, but other 16 bit serial AtoD//s will probably work with this code
     //reads 16 bit serial using Original or Slim Control Board. ver111-33c
     //good for 16 Bit Original AtoD Module or SLIM-ADC-16
     //MAG is WAIT, PHASE is ACK, SCLK is BD6, CVN is BD7.
      //if cb = 2 then goto [Read16wSlimCB]// if SLIM Contol Board, jump over [Read16wOrigCB]
     //Using the Original Control Board.  added by ver115-9b
      output(port, 128);  //take CVN high. Begins data capture inside AtoD
      output(port, 128);
      output(port, 128); //keep CVN high for 3 port commands to assure full AtoD conversion
      output(port, 64);  //CVN low and SCLK=1 //bit 15 is valid
      stats.stat15 = input(status); //read data, statX is an 8 bit word for the Status Port
      output(port, 0);output(port, 64); //Status bit 14 is now valid
      stats.stat14 = input(status);
      output(port, 0);output(port, 64); //Status bit 13 is now valid
      stats.stat13 = input(status);
      output(port, 0);output(port, 64); //Status bit 12 is now valid
      stats.stat12 = input(status);
      output(port, 0);output(port, 64); //Status bit 11 is now valid
      stats.stat11 = input(status);
      output(port, 0);output(port, 64); //Status bit 10 is now valid
      stats.stat10 = input(status);
      output(port, 0);output(port, 64); //Status bit 9 is now valid
      stats.stat9 = input(status);
      output(port, 0);output(port, 64); //Status bit 8 is now valid
      stats.stat8 = input(status);
      output(port, 0);output(port, 64); //Status bit 7 is now valid
      stats.stat7 = input(status);
      output(port, 0);output(port, 64); //Status bit 6 is now valid
      stats.stat6 = input(status);
      output(port, 0);output(port, 64); //Status bit 5 is now valid
      stats.stat5 = input(status);
      output(port, 0);output(port, 64); //Status bit 4 is now valid
      stats.stat4 = input(status);
      output(port, 0);output(port, 64); //Status bit 3 is now valid
      stats.stat3 = input(status);
      output(port, 0);output(port, 64); //Status bit 2 is now valid
      stats.stat2 = input(status);
      output(port, 0);output(port, 64); //Status bit 1 is now valid
      stats.stat1 = input(status);
      output(port, 0); //Status bit 0 is now valid
      stats.stat0 = input(status);
      //we now have raw a/d status words in stat15-stat0
      //to [ReadMagnitude]or[ReadPhase]with status words
}
void lptFunctions::ReadAD22Status()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  //For reading the 12 bit serial AtoD. Changed 4-27-10 ver115-9b
     //This routine modified to help reject noise glitching caused by newer, faster, cheaply made computers.
     //needed: port,status ; creates: 12 status port words (stat11-stat0)mag,(and, pha if two A/D//s installed) //ver111-37a
     //written for Linear Technology, LTC1860, but other 12 bit serial AtoD//s will probably work with this code
     //reads 12 bit serial using Original Control Board or Slim Control Board. ver111-33c
     //good for serial, 12 Bit Original AtoD Module or SLIM-ADC-12
      //serial data out is incremented on the falling SCLK edge.
     //MAG is WAIT, PHASE is ACK, SCLK is BD6, CVN is BD7.

      if cb = 2 then goto [Read22wSlimCB]// if SLIM Contol Board, jump over routine [Read22wOrigCB]
    [Read22wOrigCB] //Using the Original Control Board. added by ver115-9b
      out port, 128  //take CVN high. Begins data conversion inside AtoD,
      //and is completed within 3.2 usec.
      out port, 128:out port, 128 //keep CVN high for 3 port commands to assure full AtoD conversion
      out port, 128  //More delay; first bit will appear without being clocked ver116-1b
      out port, 0  //CVN low, SCLK=0  Status bit 11 is now valid.
      out port, 64  //CVN low, SCLK=1
      stat11 = inp(status) //read data, statX is 8 bit word

      out port, 0:out port, 64 //Status bit 10 is now valid
      stat10 = inp(status)
      out port, 0:out port, 64 //Status bit 9 is now valid
      stat9 = inp(status)
      out port, 0:out port, 64 //Status bit 8 is now valid
      stat8 = inp(status)
      out port, 0:out port, 64 //Status bit 7 is now valid
      stat7 = inp(status)
      out port, 0:out port, 64 //Status bit 6 is now valid
      stat6 = inp(status)
      out port, 0:out port, 64 //Status bit 5 is now valid
      stat5 = inp(status)
      out port, 0:out port, 64 //Status bit 4 is now valid
      stat4 = inp(status)
      out port, 0:out port, 64 //Status bit 3 is now valid
      stat3 = inp(status)
      out port, 0:out port, 64 //Status bit 2 is now valid
      stat2 = inp(status)
      out port, 0:out port, 64 //Status bit 1 is now valid
      stat1 = inp(status)
      out port, 0 //Status bit 0 is now valid
      stat0 = inp(status)
      return //to [ReadMagnitude]or[ReadPhase]with status words
      //a/d outputs would go high z on 13th SCLK trailing edge, but only 12 have been sent.
      stat0 = inp(status) //read data, statX is 8 bit word
      //we have raw a/d status words in stat11-stat0
      return //to [ReadMagnitude]or[ReadPhase]with status words
*/
}
void lptFunctions::ResetDDS1par()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed:control,STRBAUTO,contclear ; resets DDS1 on J5(OrigControlBd), into parallel mode
    out control, STRBAUTO        'wclk and fqud lines high, causing DDS "Reset" line to go high
    out control, contclear     'wclk and fqud lines low (all control lines low)
    return
    */
}

void lptFunctions::ResetDDS1ser()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'OrigControlBoard.needed:AUTO,STRB,STRBAUTO ; set DDS1(J5)to serial mode. ver113-2c
    'DDS (AD9850/9851) can be hard wired. pin2=D2=0, pin3=D1=1,pin4=D0=1, D3-D7 are don't care.
    'this will reset DDS into parallel, involk serial mode, then command zero output
    out port, 3 'data=0000 0011, if the DDS is not already hard wired
    '(reset DDS1 to parallel)Data,WCLK up,WCLK up and FQUD up,WCLK up and FQUD down,WCLK down
    out control, AUTO       'WCLK up, FQUD=0
    out control, STRBAUTO   'WCLK=1 and FQUD up
    out control, AUTO       'WCLK=1, FQUD down
    out control, contclear  'WCLK down, FQUD=0
    '(end reset DDS1 to parallel)
    '(involk serial mode DDS1)WCLK up, WCLK down, FQUD up, FQUD down
    out control, AUTO:out control, contclear 'WCLK up, WCLK down
    out control, STRB:out control, contclear 'FQUD up, FQUD down
    'even if the DDS1, D0-D2 is not hard wired, it will be in Serial Mode
    '(end involk serial mode DDS1)
    '(command DDS1 to flush registers)D7=0,WCLK up,WCLK down,(repeat39more),FQUD up,FQUD down
    out port, 0  'D7=0
    for thisloop = 0 to 39
    out control, AUTO:out control, contclear  'D7=0,WCLK up,WCLK down
    next thisloop
    out control, STRB:out control, contclear 'FQUD up, FQUD down
    '(end command DDS1 flush)DDS will output a DC signal
    return
    */
}
void lptFunctions::ResetDDS3ser()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  //OrigControlBoard.needed:AUTO,STRB,STRBAUTO ; set DDS3(J4)to serial mode. ver113-2c
  //DDS3 (AD9850/9851) must be hard wired. pin2=D2=0, pin3=D1=1,pin4=D0=1, D3-D7 are don't care.
  out control, Jcontrol  //enable Control Board J connector
  //(reset DDS3 to parallel)WCLK up and FQUD up,WCLK down and FQUD down
  out port, 34 //WCLK up and FQUD up.  DDS register pointer will reset
  //(end reset DDS1 to parallel)



  out port, sfqud:out port, 0 // DDSpin8, FQUD up, FQUD down.  DDS register pointer will reset
  out port, swclk:out port, 0 // DDSpin9, WCLK up, DDS WCLK down
  out port, sfqud:out port, 0 // DDSpin8, FQUD up, FQUD down.  DDS will go to 0 Hz.
  out control, contclear  //disable Control Board J connector
    */
}

void lptFunctions::ResetDDS1serSLIM(int pdmcmd, int filtbank)
{
  //reset serial DDS1 without disturbing Filter Bank or PDM. ver113-2c
  //must have DDS (AD9850/9851) hard wired. pin2=D2=0, pin3=D1=1,pin4=D0=1, D3-D7 are don't care.
  //this will reset DDS into parallel, involk serial mode, then command to 0 Hz.
  //pdmcmd = phaarray(thisstep,0) //ver111-39d

  //(reset DDS1 to parallel)WCLK up,WCLK up and FQUD up,WCLK up and FQUD down,WCLK down
  output(port, filtbank + 1);     //apply last known filter path and WCLK=D0=1 to buffer
  output(control, SELT);          //DDSpin9, WCLK up to DDS
  output(control, contclear);     //disable buffer,leaving filtbank, and WCLK=high to DDS
  output(port, pdmcmd*64 + 2);    //apply last known pdmcmd and FQUD=D3=1 to buffer
  output(control, INIT);          //DDSpin8, FQUD up,DDS resets to parallel,register pointer will reset
  output(port, pdmcmd*64);        //DDSpin8, FQUD down
  output(control, contclear);     //disable buffer, leaving last known PDM state latched
  output(port, filtbank);         //apply last known filter path and WCLK=D0=0 to buffer
  output(control, SELT);          //DDSpin9, WCLK down
  output(control, contclear);     //disable buffer,leaving filtbank
  //(end reset DDS1 to parallel)
  //(involk serial mode DDS1)WCLK up, WCLK down, FQUD up, FQUD down
  output(port, filtbank + 1);     //apply last known filter path and WCLK=D0=1 to buffer
  output(control, SELT);          //DDSpin9, WCLK up to DDS
  output(port, filtbank);         //apply last known filter path and WCLK=D0=0 to DDS
  output(control, contclear);     //disable buffer,leaving filtbank
  output(port, pdmcmd*64 + 2);    //apply last known pdmcmd and FQUD=D3=1 to buffer
  output(control, INIT);          //DDSpin8, FQUD up,DDS resets to parallel,register pointer will reset
  output(port, pdmcmd*64);        //DDSpin8, FQUD down
  output(control, contclear);     //disable buffer, leaving last known PDM state latched
  //(end involk serial mode DDS1)
  //(flush and command DDS1)D7,WCLK up,WCLK down,(repeat39more),FQUD up,FQUD down
  //present data to buffer,latch buffer,disable buffer,present data+clk to buffer,latch buffer,disable buffer
  int a=filtbank;
  for (int thisloop = 0; thisloop <= 39; thisloop++)
  {
    output(port, a);
    output(control, SELT);
    output(control, contclear);
    output(port, a+1);
    output(control, SELT);
    output(control, contclear);
  }
  output(port, a);
  output(control, SELT);
  output(control, contclear); //leaving filtbank latched
  output(port, pdmcmd*64 + 2);    //apply last known pdmcmd and FQUD=D3=1 to buffer
  output(control, INIT);          //DDSpin8, FQUD up,DDS resets to parallel,register pointer will reset
  output(port, pdmcmd*64);        //DDSpin8, FQUD down
  output(control, contclear);     //disable buffer, leaving last known PDM state latched
  //(end flush command DDS1)
  return; //to //[InitializeDDS1]
}
void lptFunctions::ResetDDS3serSLIM(int pdmcmd, int filtbank)
{
  //reset serial DDS3 without disturbing Filter Bank or PDM. ver113-2c
  //must have DDS (AD9850/9851) hard wired. pin2=D2=0, pin3=D1=1,pin4=D0=1, D3-D7 are don//t care.
  //this will reset DDS into parallel, involk serial mode, then command to 0 Hz.

  //(reset DDS3 to parallel)WCLK up,WCLK up and FQUD up,WCLK up and FQUD down,WCLK down
  output(port, filtbank + 1);     //apply last known filter path and WCLK=D0=1 to buffer
  output(control, SELT);          //DDSpin9, WCLK up to DDS
  output(control, contclear);     //disable buffer,leaving filtbank, and WCLK=high to DDS
  output(port, pdmcmd*64 + 8);    //apply last known pdmcmd and FQUD=D3=1 to buffer
  output(control, INIT);          //DDSpin8, FQUD up,DDS resets to parallel,register pointer will reset
  output(port, pdmcmd*64);        //DDSpin8, FQUD down
  output(control, contclear);     //disable buffer, leaving last known PDM state latched
  output(port, filtbank);         //apply last known filter path and WCLK=D0=0 to buffer
  output(control, SELT);          //DDSpin9, WCLK down
  output(control, contclear);     //disable buffer,leaving filtbank
  //(end reset DDS3 to parallel)
  //(involk serial mode DDS3)WCLK up, WCLK down, FQUD up, FQUD down
  output(port, filtbank + 1);     //apply last known filter path and WCLK=D0=1 to buffer
  output(control, SELT);          //DDSpin9, WCLK up to DDS
  output(port, filtbank);         //apply last known filter path and WCLK=D0=0 to DDS
  output(control, contclear);     //disable buffer,leaving filtbank
  output(port, pdmcmd*64 + 8);    //apply last known pdmcmd and FQUD=D3=1 to buffer
  output(control, INIT);          //DDSpin8, FQUD up,DDS resets to parallel,register pointer will reset
  output(port, pdmcmd*64);        //DDSpin8, FQUD down
  output(control, contclear);     //disable buffer, leaving last known PDM state latched
  //(end involk serial mode DDS3)
  //(flush and command DDS3)D7,WCLK up,WCLK down,(repeat39more),FQUD up,FQUD down
  //present data to buffer,latch buffer,disable buffer,present data+clk to buffer,latch buffer,disable buffer
  int a=filtbank;
  for (int thisloop = 0; thisloop <= 39; thisloop++)
  {
    output(port, a);
    output(control, SELT);
    output(control, contclear);
    output(port, a+1);
    output(control, SELT);
    output(control, contclear);
  }
  output(port, a);
  output(control, SELT);
  output(control, contclear); //leaving filtbank latched
  output(port, pdmcmd*64 + 8);    //apply last known pdmcmd and FQUD=D3=1 to buffer
  output(control, INIT);          //DDSpin8, FQUD up,DDS resets to parallel,register pointer will reset
  output(port, pdmcmd*64);        //DDSpin8, FQUD down
  output(control, contclear);     //disable buffer, leaving last known PDM state latched
  //(end flush command DDS3)
  //to //(InitializeDDS 3)
}
void lptFunctions::LPTportTest()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'lpt, added by Scotty ver116-4b
[LPTportTest]
    if suppressHardware then wait   'No hardware--can't test port
    enterport=port:enterstatus=status:entercontrol=control
      UpperLeftX = 5
      UpperLeftY = 5
      WindowWidth = 350
      WindowHeight = 350

    BackgroundColor$ = "darkblue"
    ForegroundColor$ = "white"
    TextboxColor$ = "blue"
    button #LPTwindow.button17down, "0", [Pin17down], UL, 5, 5, 20, 20
    button #LPTwindow.button16down, "0", [Pin16down], UL, 5, 30, 20, 20
    button #LPTwindow.button14down, "0", [Pin14down], UL, 5, 55, 20, 20
    button #LPTwindow.button1down, "0", [Pin1down], UL, 5, 80, 20, 20
    button #LPTwindow.button2down, "0", [Pin2down], UL, 5, 105, 20, 20
    button #LPTwindow.button3down, "0", [Pin3down], UL, 5, 130, 20, 20
    button #LPTwindow.button4down, "0", [Pin4down], UL, 5, 155, 20, 20
    button #LPTwindow.button5down, "0", [Pin5down], UL, 5, 180, 20, 20
    button #LPTwindow.button6down, "0", [Pin6down], UL, 5, 205, 20, 20
    button #LPTwindow.button7down, "0", [Pin7down], UL, 5, 230, 20, 20
    button #LPTwindow.button8down, "0", [Pin8down], UL, 5, 255, 20, 20
    button #LPTwindow.button9down, "0", [Pin9down], UL, 5, 280, 20, 20
    button #LPTwindow.button17up, "1", [Pin17up], UL, 30, 5, 20, 20
    button #LPTwindow.button16up, "1", [Pin16up], UL, 30, 30, 20, 20
    button #LPTwindow.button14up, "1", [Pin14up], UL, 30, 55, 20, 20
    button #LPTwindow.button1up, "1", [Pin1up], UL, 30, 80, 20, 20
    button #LPTwindow.button2up, "1", [Pin2up], UL, 30, 105, 20, 20
    button #LPTwindow.button3up, "1", [Pin3up], UL, 30, 130, 20, 20
    button #LPTwindow.button4up, "1", [Pin4up], UL, 30, 155, 20, 20
    button #LPTwindow.button5up, "1", [Pin5up], UL, 30, 180, 20, 20
    button #LPTwindow.button6up, "1", [Pin6up], UL, 30, 205, 20, 20
    button #LPTwindow.button7up, "1", [Pin7up], UL, 30, 230, 20, 20
    button #LPTwindow.button8up, "1", [Pin8up], UL, 30, 255, 20, 20
    button #LPTwindow.button9up, "1", [Pin9up], UL, 30, 280, 20, 20
    statictext #LPTwindow.button17, "Pin 17, Sel(L1) = 0", 55, 7, 110, 15    'ver116-4e added latch numbers
    statictext #LPTwindow.button16, "Pin 16, Init(L2) = 0", 55, 32, 110, 15
    statictext #LPTwindow.button14, "Pin 14, Auto(L3) = 0", 55, 57, 110, 15
    statictext #LPTwindow.button1, "Pin 1, Strobe(L4) = 0", 55, 82, 110, 15
    statictext #LPTwindow.button2, "Pin 2, D0 = 0", 55, 107, 75, 15
    statictext #LPTwindow.button3, "Pin 3, D1 = 0", 55, 132, 75, 15
    statictext #LPTwindow.button4, "Pin 4, D2 = 0", 55, 157, 75, 15
    statictext #LPTwindow.button5, "Pin 5, D3 = 0", 55, 182, 75, 15
    statictext #LPTwindow.button6, "Pin 6, D4 = 0", 55, 207, 75, 15
    statictext #LPTwindow.button7, "Pin 7, D5 = 0", 55, 232, 75, 15
    statictext #LPTwindow.button8, "Pin 8, D6 = 0", 55, 257, 75, 15
    statictext #LPTwindow.button9, "Pin 9, D7 = 0", 55, 282, 75, 15
    statictext #LPTwindow.explain, "Click 0 or 1 to change state.", 5, 305, 180, 20
    statictext #LPTwindow.message4, "Click to read Status Pins:", 175, 7, 150, 15
if USBdevice <> 0 then statictext #LPTwindow.noteusb, "(Invalid with USB)", 175, 27, 170, 15 'ver116-4r
    'the positions of all below have been lowered by 20 pixels  'ver116-4r
    button #LPTwindow.status, "CAPTURE STATUS", [ReadLPTStatus], UL, 175, 50, 130, 20
    textbox #LPTwindow.waitbox, 175, 75, 20, 20
    textbox #LPTwindow.ackbox, 175, 100, 20, 20
    textbox #LPTwindow.pebox, 175, 125, 20, 20
    textbox #LPTwindow.selbox, 175, 150, 20, 20
    textbox #LPTwindow.errbox, 175, 175, 20, 20
    statictext #LPTwindow.message11, "WAIT, pin 11", 200, 77, 80, 15
    statictext #LPTwindow.message10, "ACK, pin 10", 200, 102, 80, 15
    statictext #LPTwindow.message12, "PE, pin 12", 200, 127, 80, 15
    statictext #LPTwindow.message13, "SELECT, pin 13", 200, 152, 80, 15
    statictext #LPTwindow.message15, "ERROR, pin 15", 200, 177, 80, 15
    statictext #LPTwindow.portaddres, "LPT Port Address (Hex)", 150, 227, 120, 15
    textbox #LPTwindow.addressbox, 270, 225, 35, 20
    statictext #LPTwindow.changeas, "Enter value as:", 150, 250, 75, 15
    button #LPTwindow.hexbutton, "Hex", [ChangeAsHex], UL, 235, 250, 30, 20
    button #LPTwindow.decbutton, "Dec", [ChangeAsDec], UL, 270, 250, 30, 20
  open "LPT Printer Port Test" for dialog as #LPTwindow:lptwindow=1 'put lptwindow in main sw.
    print #LPTwindow.status, "!setfocus"
    print #LPTwindow, "trapclose [closeLPTwindow]"
    lptpin1=1 : lptpin14=2 : lptpin16=0 : lptpin17=8 'start with control line values at zero
    if port = 0 then port = 888 'default address in decimal, when LPT Test is stand-alone
    print #LPTwindow.addressbox,""; dechex$(port)

    [ChangeAsHex]
    print #LPTwindow.addressbox, "!contents? newport$";
    port = hexdec(newport$):status=port+1:control=port+2
    print #LPTwindow.addressbox,""; dechex$(port)
    wait

    [ChangeAsDec]
    print #LPTwindow.addressbox, "!contents? newport$";
    port = val(newport$):status=port+1:control=port+2
    print #LPTwindow.addressbox,""; dechex$(port)
    wait

    [Pin2down]
    lptD0=0
    print #LPTwindow.button2, "Pin 2, D0 = 0"
    goto [SetLPTport]
    [Pin3down]
    lptD1=0
    print #LPTwindow.button3, "Pin 3, D1 = 0"
    goto [SetLPTport]
    [Pin4down]
    lptD2=0
    print #LPTwindow.button4, "Pin 4, D2 = 0"
    goto [SetLPTport]
    [Pin5down]
    lptD3=0    '116-4sLPT
    print #LPTwindow.button5, "Pin 5, D3 = 0"
    goto [SetLPTport]
    [Pin6down]
    lptD4=0    '116-4sLPT
    print #LPTwindow.button6, "Pin 6, D4 = 0"
    goto [SetLPTport]
    [Pin7down]
    lptD5=0    '116-4sLPT
    print #LPTwindow.button7, "Pin 7, D5 = 0"
    goto [SetLPTport]
    [Pin8down]
    lptD6=0    '116-4sLPT
    print #LPTwindow.button8, "Pin 8, D6 = 0"
    goto [SetLPTport]
    [Pin9down]
    lptD7=0    '116-4sLPT
    print #LPTwindow.button9, "Pin 9, D7 = 0"
    goto [SetLPTport]

    [Pin2up]
    lptD0=1
    print #LPTwindow.button2, "Pin 2, D0 = 1"
    goto [SetLPTport]
    [Pin3up]
    lptD1=2
    print #LPTwindow.button3, "Pin 3, D1 = 1"
    goto [SetLPTport]
    [Pin4up]
    lptD2=4
    print #LPTwindow.button4, "Pin 4, D2 = 1"
    goto [SetLPTport]
    [Pin5up]
    lptD3=8    '116-4sLPT
    print #LPTwindow.button5, "Pin 5, D3 = 1"
    goto [SetLPTport]
    [Pin6up]
    lptD4=16    '116-4sLPT
    print #LPTwindow.button6, "Pin 6, D4 = 1"
    goto [SetLPTport]
    [Pin7up]
    lptD5=32    '116-4sLPT
    print #LPTwindow.button7, "Pin 7, D5 = 1"
    goto [SetLPTport]
    [Pin8up]
    lptD6=64    '116-4sLPT
    print #LPTwindow.button8, "Pin 8, D6 = 1"
    goto [SetLPTport]
    [Pin9up]
    lptD7=128    '116-4sLPT
    print #LPTwindow.button9, "Pin 9, D7 = 1"
    goto [SetLPTport]

    [Pin1down] 'STROBE
    lptpin1 = 1
regPD3 = 0 'ver116-4r
    print #LPTwindow.button1, "Pin 1, Strobe(L4) = 0"
    goto [SetLPTControl]
    [Pin14down] 'AUTO
    lptpin14 = 2
regPD2 = 0 'ver116-4r
    print #LPTwindow.button14, "Pin 14, Auto(L3) = 0"
    goto [SetLPTControl]
    [Pin16down] 'INIT
    lptpin16 = 0
regPD1 = 0 'ver116-4r
    print #LPTwindow.button16, "Pin 16, Init(L2) = 0"
    goto [SetLPTControl]
    [Pin17down] 'SELT
    lptpin17 = 8
regPD0 = 0 'ver116-4r
    print #LPTwindow.button17, "Pin 17, Sel(L1) = 0"
    goto [SetLPTControl]

    [Pin1up] 'STROBE
    lptpin1 = 0
regPD3 = 8 'ver116-4r
    print #LPTwindow.button1, "Pin 1, Strobe(L4) = 1"
    goto [SetLPTControl]
    [Pin14up] 'AUTO
    lptpin14 = 0
regPD2 = 4 'ver116-4r
    print #LPTwindow.button14, "Pin 14, Auto(L3) = 1"
    goto [SetLPTControl]
    [Pin16up] 'INIT
    lptpin16 = 4
regPD1 = 2 'ver116-4r
    print #LPTwindow.button16, "Pin 16, Init(L2) = 1"
    goto [SetLPTControl]
    [Pin17up] 'SELT
    lptpin17 = 0
regPD0 = 1 'ver116-4r
    print #LPTwindow.button17, "Pin 17, Sel(L1) = 1"
    goto [SetLPTControl]

    [SetLPTport]
    out port, lptD0 +lptD1 +lptD2 +lptD3 +lptD4 +lptD5 +lptD6 +lptD7
'for USB, this is register PB, bits 0-7
QString USBwrbuf = "A60200"+ToHex(lptD0+lptD1+lptD2 +lptD3 +lptD4 +lptD5 +lptD6 +lptD7)+"000000";    //116-4sLPT
usb->usbMSADeviceWriteString(USBwrbuf,7);
    wait
    */
}
void lptFunctions::SetLPTControl()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    out control, lptpin1 +lptpin14 +lptpin16 +lptpin17
'for USB, this is register PD, bits 0-3
QString USBwrbuf = "A608000000"+ToHex(regPD0+regPD1+regPD2+regPD3)+"00"; //ver116-4r
usb->usbMSADeviceWriteString(USBwrbuf,7);
    wait

    [ReadLPTStatus]
    lptpin11=1:lptpin10=0:lptpin12=0:lptpin13=0:lptpin15=0
    read7 = inp(status)
    if read7 > 127 then lptpin11 = 0:read7 = read7 - 128
    print #LPTwindow.waitbox, lptpin11
    if read7 > 63 then lptpin10 = 1:read7 = read7 - 64
    print #LPTwindow.ackbox, lptpin10
    if read7 >31 then lptpin12 = 1:read7 = read7 - 32
    print #LPTwindow.pebox, lptpin12
    if read7 > 15 then lptpin13 = 1:read7 = read7 - 16
    print #LPTwindow.selbox, lptpin13
    if read7 > 7 then lptpin15 = 1
    print #LPTwindow.errbox, lptpin15
    wait

    [closeLPTwindow]
    out port, 0
    out control, contclear
    port=enterport:status=enterstatus:control=entercontrol
     'put MSA addresses back to what they were when entering this subroutine
    close #LPTwindow
    lptwindow=0
    wait
'End LPT port tester
*/
}
void lptFunctions::CommandFilterOrigCB(int &fbank)// //command 1 of 4 and latch it  ver116-4j made this a subroutine
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    //fbank should be the non-global filtbank
    if suppressHardware then exit sub //ver115-6c
    control=port+2
    out port, fbank       //presents filter address to control buffer
    out control, globalINIT        //presents filter address to Filter Bank
    out port, fbank + 1   //latches filter address into Filter Bank using latch signal
    out port, fbank       //leaves filter address on Filter Bank, removing latch signal
    out control, globalContClear   //removes filter address from Filter Bank
    out port, 0              //removes filter address from control buffer
        */
}

void lptFunctions::CommandFilterSlimCB(int  fbank)
{
  //fbank should be the non-global filtbank
  //if suppressHardware then exit sub
  control=port+2;
  output(port, fbank);         //presents filter address to control buffer
  output(control, SELT);       //presents filter address to Filter Bank
  output(port, fbank + 128);   //latches filter address into Filter Bank using latch signal
  output(port, fbank);         //leaves filter address on Filter Bank, removing latch signal
  output(control, contclear);  //removes filter address from Filter Bank
  output(port, 0);             //removes filter address from control buffer
}

int lptFunctions::input(short port)
{
  if (!lptLib->isLoaded())
  {
    qDebug() << "LPT low level driver not loaded";
    return 0;
  }
  if (libraryType == inpoutLib)
  {
    return lpinport(port);
  }
  else if (libraryType == ntPortLin)
  {
    return ntinport(port);
  }
  else if (libraryType == userPortLib)
  {

  }
  return 0;
}

void lptFunctions::output(short port, short value)
{
  if (!lptLib->isLoaded())
  {
    qDebug() << "LPT low level driver not loaded";
    return;
  }
  if (libraryType == inpoutLib)
  {
    lpoutport(port, value) ;
  }
  else if (libraryType == ntPortLin)
  {
    ntoutport((unsigned short)port, (unsigned short)value);
  }
  else if (libraryType == userPortLib)
  {

  }
}

void lptFunctions::Read16wSlimCB() //Using the SLIM Control Board. added by ver115-9b
{
  output(port, 128);output(control, AUTO);output(control, contclear);  //take CVN high. Begins data conversion inside AtoD,
  //and is completed within 2.2 usec.
  output(control, contclear);  //keep CVN high for 3 port commands to assure full AtoD conversion

  output(port, 64);output(control, AUTO);output(control, contclear);  //CVN low and SCLK=1 //Status bit 15
  //of the serial data is valid and can be read at any time.
  stats.stat15 = input(status); //read data, statX is an 8 bit word for the Status Port

  output(port, 0);output(control, AUTO);output(control, contclear);  //SCLK=0  //bit 14 is valid,
  //data (SDO) is incremented on the falling edge of SCLK
  output(port, 64);output(control, AUTO);output(control, contclear);  //SCLK=1
  stats.stat14 = input(status); //read data, statX is 8 bit word

  output(port, 0);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=0  //bit 13 is valid
  output(port, 64);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=1
  stats.stat13 = input(status); //read data, statX is 8 bit word

  output(port, 0);output(control, AUTO);output(control, contclear); //CVN low, SCLK=0  //bit 12 is valid
  output(port, 64);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=1
  stats.stat12 = input(status); //read data, statX is 8 bit word

  output(port, 0);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=0  //bit 11 is valid
  output(port, 64);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=1
  stats.stat11 = input(status); //read data, statX is 8 bit word

  output(port, 0);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=0  //bit 10 is valid
  output(port, 64);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=1
  stats.stat10 = input(status); //read data, statX is 8 bit word

  output(port, 0);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=0  //bit 9 is valid
  output(port, 64);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=1
  stats.stat9 = input(status); //read data, statX is 8 bit word

  output(port, 0);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=0  //bit 8 is valid
  output(port, 64);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=1
  stats.stat8 = input(status); //read data, statX is 8 bit word

  output(port, 0);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=0  //bit 7 is valid
  output(port, 64);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=1
  stats.stat7 = input(status); //read data, statX is 8 bit word

  output(port, 0);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=0  //bit 6 is valid
  output(port, 64);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=1
  stats.stat6 = input(status); //read data, statX is 8 bit word

  output(port, 0);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=0  //bit 5 is valid
  output(port, 64);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=1
  stats.stat5 = input(status); //read data, statX is 8 bit word

  output(port, 0);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=0  //bit 4 is valid
  output(port, 64);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=1
  stats.stat4 = input(status); //read data, statX is 8 bit word

  output(port, 0);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=0  //bit 3 is valid
  output(port, 64);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=1
  stats.stat3 = input(status); //read data, statX is 8 bit word

  output(port, 0);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=0  //bit 2 is valid
  output(port, 64);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=1
  stats.stat2 = input(status); //read data, statX is 8 bit word

  output(port, 0);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=0  //bit 1 is valid
  output(port, 64);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=1
  stats.stat1 = input(status); //read data, statX is 8 bit word

  output(port, 0);output(control, AUTO);output(control, contclear);  //CVN low, SCLK=0  //bit 0 is valid
  //a/d outputs would go high z on 16th SCLK trailing edge, but only 15 have been sent.
  stats.stat0 = input(status); //read data, statX is 8 bit word
  //U3 and P3 on SLIM Control Board is disabled.
  //we now have raw a/d status words in stat15-stat0
  return; //to [ReadMagnitude]or[ReadPhase]with status words
}
