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
#define __int64 qint64
#endif

struct USBrBuf
{
  unsigned long numreads;
  unsigned long magnitude;
  unsigned long phase;
};

struct Int64N
{
  unsigned long msLong;
  unsigned long lsLong;
}; //, msLong as ulong, lsLong as ulong // USB:15/08/10

struct Int64SW
{
  unsigned long msLong;
  unsigned long lsLong;
};

struct UsbAllSlimsAndLoadData
{
  short thisstep;
  short filtbank;
  short latches;
  short pdmcommand;
  short pdmcmdmult;
  short pdmcmdadd;
};// USB:15/08/10

// The following struct is used to control ADC reading configuration.
struct UsbAdcControl
{
  short Adcs;
  short Clocking;
  short Delay;
  short Bits;
  short Average;
};// USB:15/08/10


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

  //void setUSBwrbuf(QString str);
  //void setUSBwrbuf2(QString str);

  void *getUSBDevice();

/*
  //------Start items for USB interface----------
   // USB interface dll //USB:01-08-2010
  // this is used as a form of handle used by the USB interface
  // it is necessary because of the limitations of liberty basic
  // and it is actually a memory pointer to a USB device class object
  // we set it to zero when the interface is not initialised
  global USBdevice //USB:01-08-2010
  // this is a boolean flag used to control when the USB interfcae dll is open (I hate basic)
  global UsbInterfaceOpen //USB:01-08-2010
  // set this flag to != 0 for usb interface active
  global bUseUsb //USB:01-08-2010
  global bUsbAvailable //USB:01-08-2010
  // these are string buffers used in USB I/O operations
  */
  //QString USBwrbuf; //USB:01-08-2010
  //QString USBwrbuf2;
  /*
  // used in ADC input functions; the number of ADC readings made and the results read
//ver116-4r deleted (since not used)    global UsbAdcCount //USB:01-08-2010
//ver116-4r deleted    global UsbAdcResult1 //USB:01-08-2010
//ver116-4r deleted    global UsbAdcResult2 //USB:01-08-2010
  USBdevice = 0 //USB:01-08-2010
  UsbInterfaceOpen = 0 //USB:01-08-2010
  bUseUsb = 0 //USB:01-08-2010
  struct USBrBuf, numreads as ulong, magnitude as ulong , phase as ulong //USB:01-08-2010
  // the next 3 are used to create a memory buffer for the dll to use to save scan time
  // it contains a char[][] version of cmdallarray
  global AllArrayBlockSize // current size of memory block allocated for AllArrays
  global hSAllArray // handle for memory block
  global ptrSAllArray // pointer to memory block

  global hSDDS1Array //USB:05/12/2010
  global ptrSDDS1Array //USB:05/12/2010
  global hSDDS3Array //USB:05/12/2010
  global ptrSDDS3Array //USB:05/12/2010
  global hSPLL1Array //USB:05/12/2010
  global ptrSPLL1Array //USB:05/12/2010
  global hSPLL3Array //USB:05/12/2010
  global ptrSPLL3Array //USB:05/12/2010
  */

  //struct Int64N, msLong as ulong, lsLong as ulong ' USB:15/08/10
  //struct Int64SW, msLong as ulong, lsLong as ulong // USB:15/08/10
  // This structure is used to minimize time spent forming commands to send to the USB DLL
  // the values correspond to the parameters of the same name (or similar description) in the parallel function
  // we aim to try not to set them each time to save processing time in basic code
  //struct UsbAllSlimsAndLoadData, thisstep as short, filtbank as short, latches as short, pdmcommand as short, pdmcmdmult as short, pdmcmdadd as short // USB:15/08/10
  // The following struct is used to control ADC reading configuration.
//  struct UsbAdcControl, Adcs as short, Clocking as short, Delay as short, Bits as short, Average as short // USB:15/08/10
  // Placed here to save liberty basic from having to fill it in each time it goes to read the ADC//s
  // The parameters map to the parameters in the ADC convert commands in fw-msa
  // Adcs takes values 1 or 3 normally ( 1 = magnitude ADC only, 3 = both ) but will also do 2 (phase only) //it will always be =3 //ver116-4r
  // Clocking is the clocking option - 0 for AD7685 and 1 for LT1860. Use same clocking option for either 12 or 16 bit serial ADC. =1 //ver116-4r
  // Delay is the ADC Convert clock high time delay - 2 for AD7684, 4 for LT1960. Use same delay for both. =4 (about 5.4 usec) //ver116-4r
  // Bits is the number of data bits - 16 for AD7685, 10 for LT1860. Will always use 16. When 12 bit ADC is used, we delete the last 4 serial bits. //ver116-4r
  // Average is normally 1 - set to higher number if you want the interface to do multiple readings and average. Always use 1. //ver116-4r
/*          UsbAdcControl.Adcs.struct = 3 //ver116-4r
          UsbAdcControl.Clocking.struct = 1 //ver116-4r
          UsbAdcControl.Delay.struct = 4 //ver116-4r
          UsbAdcControl.Bits.struct = 16 //ver116-4r
          UsbAdcControl.Average.struct = 1 //ver116-4r

  //---------End items for USB interface------------

*/

  int AllArrayBlockSize; // current size of memory block allocated for AllArrays
  int DeviceArrayBlockSize;
  void * hSAllArray; // handle for memory block
  void * ptrSAllArray; // pointer to memory block

  void * hSDDS1Array; //USB:05/12/2010
  void * ptrSDDS1Array; //USB:05/12/2010
  void * hSDDS3Array; //USB:05/12/2010
  void * ptrSDDS3Array; //USB:05/12/2010
  void * hSPLL1Array; //USB:05/12/2010
  void * ptrSPLL1Array; //USB:05/12/2010
  void * hSPLL3Array; //USB:05/12/2010
  void * ptrSPLL3Array; //USB:05/12/2010


private:
  QLibrary *usbLib;
  void *USBDevice;

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


};

#endif // USBFUNCTIONS_H
