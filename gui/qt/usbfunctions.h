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
#ifndef USBFUNCTIONS_H
#define USBFUNCTIONS_H

#include <QtCore>
#ifdef __WIN32__
#include <windows.h>
#endif

#ifndef __WIN32__
//#define __int64 qint64
#endif

struct USBrBuf
{
  unsigned long numreads;
  unsigned long magnitude;
  unsigned long phase;
};


struct UsbAllSlimsAndLoadData
{
  short thisstep;
  short filtbank;
  short latches;
  short pdmcommand;
  short pdmcmdmult;
  short pdmcmdadd;
};

// The following struct is used to control ADC reading configuration.
struct UsbAdcControl
{
  short Adcs;
  short Clocking;
  short Delay;
  short Bits;
  short Average;
};


class usbFunctions
{
public:
  usbFunctions();
  ~usbFunctions();
  int usbMSAGetVersions();
  void usbMSARelease();
  int usbMSAInit();
  void resizeMemory(int maxPoints);

  int usbMSADeviceSetAllArrayPtr(void *pAllArray, short nArrayRows, short nArrayCols, unsigned long *pResults );
  int usbMSADeviceWriteString(QString data, int message_size );
  int usbMSADeviceReadAdcs(char *data, int message_size, unsigned long *pResults );
  int usbMSADeviceReadAdcsStruct(unsigned short *pData, unsigned long *pResults );
  int usbMSADevicePopulateDDSArrayBitReverse(__int64 *pArray, unsigned long *pData, unsigned short step, unsigned short bits, unsigned long *pResults );
  int usbMSADevicePopulateDDSArray(__int64 *pArray, unsigned long *pData, unsigned short step, unsigned long *pResults );
  int usbMSADevicePopulateAllArray(unsigned short Steps, unsigned short bits,
                                __int64 *pBit0Array,
                                __int64 *pBit1Array,
                                __int64 *pBit2Array,
                                __int64 *pBit3Array,
                                __int64 *pBit4Array,
                                __int64 *pBit5Array,
                                __int64 *pBit6Array,
                                __int64 *pBit7Array,
                                unsigned long *pResults );
  int usbMSADeviceWriteInt64MsbFirst(short nUsbCommandByte, unsigned long *pData,
                                  short nBits, short clock, short fixeddata, short vardata,
                                  unsigned long *pResults );
  int usbMSADeviceAllSlims(unsigned short thisstep, unsigned short filtbank, unsigned long *pResults );
  int usbMSADeviceAllSlimsAndLoad(unsigned short thisstep, unsigned short filtbank, unsigned short latch, unsigned short pdmcmd, unsigned short pdmlatch, unsigned long *pResults );
  int usbMSADeviceAllSlimsAndLoadStruct(unsigned short *pData, unsigned long *pResults );


  bool usbInterfaceOpen(QString fileName);
  void usbCloseInterface();

  unsigned long int64SW[2];
  unsigned long int64N[2];
  void * ptrSDDS1Array;
  void * ptrSPLL1Array;
  void * ptrSPLL3Array;
  void * ptrSDDS3Array;

private:
  QLibrary *usbLib;
  void *USBDevice;
  void *getUSBDevice();

  typedef int (*_UsbMSAGetVersions)(void *MSAData);
  typedef void * (*_UsbMSAInitialise)();
  typedef void (*_UsbMSARelease)(void *MSAData);
  typedef int (*_UsbMSAInit)(void *MSAData);
  typedef int (*_UsbMSADeviceSetAllArrayPtr)( void *pMSAData, void *pAllArray, short nArrayRows, short nArrayCols, unsigned long *pResults );

  typedef int (*_UsbMSADeviceWriteString)( void *pMSAData, char *data, int message_size );
  typedef int (*_UsbMSADeviceReadAdcs)( void *pMSAData, char *data, int message_size, unsigned long *pResults );
  typedef int (*_UsbMSADeviceReadAdcsStruct)( void *pMSAData, unsigned short *pData, unsigned long *pResults );
  typedef int (*_UsbMSADevicePopulateDDSArrayBitReverse)( void *pMSAData, __int64 *pArray, unsigned long *pData, unsigned short step, unsigned short bits, unsigned long *pResults );
  typedef int (*_UsbMSADevicePopulateDDSArray)( void *pMSAData, __int64 *pArray, unsigned long *pData, unsigned short step, unsigned long *pResults );
  typedef int (*_UsbMSADevicePopulateAllArray)( void *pMSAData, unsigned short Steps, unsigned short bits,
                                __int64 *pBit0Array,
                                __int64 *pBit1Array,
                                __int64 *pBit2Array,
                                __int64 *pBit3Array,
                                __int64 *pBit4Array,
                                __int64 *pBit5Array,
                                __int64 *pBit6Array,
                                __int64 *pBit7Array,
                                unsigned long *pResults );
  typedef int (*_UsbMSADeviceWriteInt64MsbFirst)( void *pMSAData, short nUsbCommandByte, unsigned long *pData,
                                  short nBits, short clock, short fixeddata, short vardata,
                                  unsigned long *pResults );
  typedef int (*_UsbMSADeviceAllSlims)( void *pMSAData, unsigned short thisstep, unsigned short filtbank, unsigned long *pResults );
  typedef int (*_UsbMSADeviceAllSlimsAndLoad)( void *pMSAData, unsigned short thisstep, unsigned short filtbank, unsigned short latch, unsigned short pdmcmd, unsigned short pdmlatch, unsigned long *pResults );
  typedef int (*_UsbMSADeviceAllSlimsAndLoadStruct)( void *pMSAData, unsigned short *pData, unsigned long *pResults );


  _UsbMSAInitialise UsbMSAInitialise;
  _UsbMSAGetVersions UsbMSAGetVersions;
  _UsbMSARelease UsbMSARelease;
  _UsbMSAInit UsbMSAInit;

  _UsbMSADeviceSetAllArrayPtr UsbMSADeviceSetAllArrayPtr;
  _UsbMSADeviceWriteString UsbMSADeviceWriteString;
  _UsbMSADeviceReadAdcs UsbMSADeviceReadAdcs;
  _UsbMSADeviceReadAdcsStruct UsbMSADeviceReadAdcsStruct;
  _UsbMSADevicePopulateDDSArrayBitReverse UsbMSADevicePopulateDDSArrayBitReverse;
  _UsbMSADevicePopulateDDSArray UsbMSADevicePopulateDDSArray;
  _UsbMSADevicePopulateAllArray UsbMSADevicePopulateAllArray;
  _UsbMSADeviceWriteInt64MsbFirst UsbMSADeviceWriteInt64MsbFirst;
  _UsbMSADeviceAllSlims UsbMSADeviceAllSlims;
  _UsbMSADeviceAllSlimsAndLoad UsbMSADeviceAllSlimsAndLoad;
  _UsbMSADeviceAllSlimsAndLoadStruct UsbMSADeviceAllSlimsAndLoadStruct;

  int AllArrayBlockSize; // current size of memory block allocated for AllArrays
  int DeviceArrayBlockSize;
  void * hSAllArray; // handle for memory block
  void * ptrSAllArray; // pointer to memory block

  void * hSDDS1Array;
  void * hSDDS3Array;
  void * hSPLL1Array;
  void * hSPLL3Array;


};

#endif // USBFUNCTIONS_H
