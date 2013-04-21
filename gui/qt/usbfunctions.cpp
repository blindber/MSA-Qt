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
#include "usbfunctions.h"
#include <QtCore>
#include <QMessageBox>

usbFunctions::usbFunctions()
{
  usbLib = NULL;
  USBDevice = NULL;

  /*
  UsbAdcControl.Adcs = 3; //ver116-4r
  UsbAdcControl.Clocking = 1; //ver116-4r
  UsbAdcControl.Delay = 4; //ver116-4r
  UsbAdcControl.Bits = 16; //ver116-4r
  UsbAdcControl.Average = 1; //ver116-4r
  */
}

usbFunctions::~usbFunctions()
{
  /*
  if (usbLib->isLoaded())
    usbLib->unload();
    */
}

int usbFunctions::usbMSAGetVersions()
{
  return UsbMSAGetVersions(USBDevice);
}

bool usbFunctions::usbInterfaceOpen(QString fileName)
{
  usbLib = new QLibrary(fileName);
  qDebug() << fileName;
  if (!usbLib->load())
  {
    QMessageBox::about(0, "MSADLL Error", usbLib->errorString() );
    return false;
  }

  UsbMSAInitialise = (_UsbMSAInitialise) usbLib->resolve("UsbMSAInitialise");
  if (UsbMSAInitialise)
  {
    USBDevice = UsbMSAInitialise();
  }

  UsbMSAGetVersions = (_UsbMSAGetVersions) usbLib->resolve("UsbMSAGetVersions");
  UsbMSARelease = (_UsbMSARelease) usbLib->resolve("UsbMSARelease");


  UsbMSADeviceSetAllArrayPtr = (_UsbMSADeviceSetAllArrayPtr) usbLib->resolve("UsbMSADeviceSetAllArrayPtr");
  UsbMSADeviceWriteString = (_UsbMSADeviceWriteString) usbLib->resolve("UsbMSADeviceWriteString");
  UsbMSADeviceReadAdcs = (_UsbMSADeviceReadAdcs) usbLib->resolve("UsbMSADeviceReadAdcs");
  UsbMSADeviceReadAdcsStruct = (_UsbMSADeviceReadAdcsStruct) usbLib->resolve("UsbMSADeviceReadAdcsStruct");
  UsbMSADevicePopulateDDSArrayBitReverse =(_UsbMSADevicePopulateDDSArrayBitReverse) usbLib->resolve("UsbMSADevicePopulateDDSArrayBitReverse");
  UsbMSADevicePopulateDDSArray = (_UsbMSADevicePopulateDDSArray) usbLib->resolve("UsbMSADevicePopulateDDSArray");
  UsbMSADevicePopulateAllArray = (_UsbMSADevicePopulateAllArray) usbLib->resolve("UsbMSADevicePopulateAllArray");
  UsbMSADeviceWriteInt64MsbFirst = (_UsbMSADeviceWriteInt64MsbFirst) usbLib->resolve("UsbMSADeviceWriteInt64MsbFirst");
  UsbMSADeviceAllSlims = (_UsbMSADeviceAllSlims) usbLib->resolve("UsbMSADeviceAllSlims");
  UsbMSADeviceAllSlimsAndLoad = (_UsbMSADeviceAllSlimsAndLoad) usbLib->resolve("UsbMSADeviceAllSlimsAndLoad");
  UsbMSADeviceAllSlimsAndLoadStruct = (_UsbMSADeviceAllSlimsAndLoadStruct) usbLib->resolve("UsbMSADeviceAllSlimsAndLoadStruct");

#ifdef __WIN32__
  HGLOBAL hDevMode = 0;
  hDevMode = GlobalAlloc(10, 2002 * 40);
  //needs to be more complex - this flag uses USB if it is available
  //do not set it if bUsbAvailable is not set
  //create a memory block for holding dds data (akin to cmdallarray[][] but accessible within dll)
  AllArrayBlockSize = 2002*40; //USB:01-08-2010
  DeviceArrayBlockSize = 2002 * 8; //USB:06-08-2010
  hSAllArray = GlobalAlloc( AllArrayBlockSize, GMEM_MOVEABLE ); //USB:01-08-2010
  ptrSAllArray = GlobalLock( hSAllArray ); //USB:01-08-2010
  hSDDS1Array = GlobalAlloc( DeviceArrayBlockSize, GMEM_MOVEABLE  ); //USB:06-08-2010
  ptrSDDS1Array = GlobalLock( hSDDS1Array ); //USB:06-08-2010
  hSDDS3Array = GlobalAlloc( DeviceArrayBlockSize, GMEM_MOVEABLE  ); //USB:06-08-2010
  ptrSDDS3Array = GlobalLock( hSDDS3Array ); //USB:06-08-2010
  hSPLL1Array = GlobalAlloc( DeviceArrayBlockSize, GMEM_MOVEABLE  ); //USB:06-08-2010
  ptrSPLL1Array = GlobalLock( hSPLL1Array ); //USB:06-08-2010
  hSPLL3Array = GlobalAlloc( DeviceArrayBlockSize, GMEM_MOVEABLE  ); //USB:06-08-2010
  ptrSPLL3Array = GlobalLock( hSPLL3Array ); //USB:06-08-2010
#endif
  unsigned long result = 0;
  usbMSADeviceSetAllArrayPtr(ptrSAllArray,2002,40, &result); //USB:01-08-2010


  return usbLib->isLoaded();
}

void usbFunctions::usbCloseInterface()
{
  if (!usbLib)
    return;
  if (usbLib->isLoaded())
  {
    //GlobalFree(hSAllArray);
    UsbMSARelease(USBDevice);
    USBDevice = NULL;
    usbLib->unload();
    usbLib = NULL;
  }
}
/*
void usbFunctions::setUSBwrbuf(QString str)
{
  USBwrbuf = str;
}

void usbFunctions::setUSBwrbuf2(QString str)
{
  USBwrbuf2 = str;
}
*/
void *usbFunctions::getUSBDevice()
{
  return USBDevice;
}

void usbFunctions::usbMSARelease()
{
  UsbMSARelease(USBDevice);
}

int usbFunctions::usbMSAInit()
{
  if (!USBDevice) return 0;
  return UsbMSAInit(USBDevice);
}

void usbFunctions::resizeMemory(int maxPoints)
{
  if (USBDevice)  //USB:01-08-2010
  {
#ifdef __WIN32__
    GlobalFree(hSAllArray); //USB:01-08-2010
    AllArrayBlockSize = maxPoints * 40; //USB:01-08-2010
    hSAllArray = GlobalAlloc(AllArrayBlockSize, GMEM_MOVEABLE ); //USB:01-08-2010
    ptrSAllArray = GlobalLock(hSAllArray); //USB:01-08-2010
    DeviceArrayBlockSize = maxPoints * 8; //USB:06-08-2010
    hSDDS1Array = GlobalAlloc(DeviceArrayBlockSize, GMEM_MOVEABLE ); //USB:06-08-2010
    ptrSDDS1Array = GlobalLock(hSDDS1Array); //USB:06-08-2010
    hSDDS3Array = GlobalAlloc(DeviceArrayBlockSize, GMEM_MOVEABLE ); //USB:06-08-2010
    ptrSDDS3Array = GlobalLock(hSDDS3Array); //USB:06-08-2010
    hSPLL1Array = GlobalAlloc(DeviceArrayBlockSize, GMEM_MOVEABLE ); //USB:06-08-2010
    ptrSPLL1Array = GlobalLock(hSPLL1Array); //USB:06-08-2010
    hSPLL3Array = GlobalAlloc(DeviceArrayBlockSize, GMEM_MOVEABLE ); //USB:06-08-2010
    ptrSPLL3Array = GlobalLock(hSPLL3Array); //USB:06-08-2010
#endif
    unsigned long result = 0;
    usbMSADeviceSetAllArrayPtr(ptrSAllArray,maxPoints,40, &result); //USB:01-08-2010
  }
}

int usbFunctions::usbMSADeviceSetAllArrayPtr(void *pAllArray, short nArrayRows, short nArrayCols, unsigned long *pResults)
{
  return UsbMSADeviceSetAllArrayPtr(USBDevice, pAllArray, nArrayRows, nArrayCols, pResults);
}

int usbFunctions::usbMSADeviceWriteString(QString data, int message_size)
{
  QByteArray byteArray = data.toUtf8();
  char* cString = (char*)byteArray.constData();

  if (!USBDevice) return 0;

  return UsbMSADeviceWriteString(USBDevice, cString, message_size);
}

int usbFunctions::usbMSADeviceReadAdcs(char *data, int message_size, unsigned long *pResults)
{
  if (!USBDevice) return 0;
  return UsbMSADeviceReadAdcs(USBDevice, data, message_size, pResults);
}

int usbFunctions::usbMSADeviceReadAdcsStruct(unsigned short *pData, unsigned long *pResults)
{
  if (!USBDevice) return 0;
  return UsbMSADeviceReadAdcsStruct(USBDevice, pData, pResults);
}

int usbFunctions::usbMSADevicePopulateDDSArrayBitReverse(long long *pArray, unsigned long *pData, unsigned short step, unsigned short bits, unsigned long *pResults)
{
  if (!USBDevice) return 0;
  return UsbMSADevicePopulateDDSArrayBitReverse(USBDevice, pArray, pData, step, bits, pResults);
}

int usbFunctions::usbMSADevicePopulateDDSArray(long long *pArray, unsigned long *pData, unsigned short step, unsigned long *pResults)
{
  if (!USBDevice) return 0;
  return UsbMSADevicePopulateDDSArray(USBDevice, pArray, pData, step, pResults);
}

int usbFunctions::usbMSADevicePopulateAllArray(unsigned short Steps, unsigned short bits, long long *pBit0Array, long long *pBit1Array, long long *pBit2Array, long long *pBit3Array, long long *pBit4Array, long long *pBit5Array, long long *pBit6Array, long long *pBit7Array, unsigned long *pResults)
{
  if (!USBDevice) return 0;
  return UsbMSADevicePopulateAllArray(USBDevice, Steps, bits, pBit0Array, pBit1Array, pBit2Array, pBit3Array, pBit4Array, pBit5Array, pBit6Array, pBit7Array, pResults);
}

int usbFunctions::usbMSADeviceWriteInt64MsbFirst(short nUsbCommandByte, unsigned long *pData, short nBits, short clock, short fixeddata, short vardata, unsigned long *pResults)
{
  if (!USBDevice) return 0;
  return UsbMSADeviceWriteInt64MsbFirst(USBDevice, nUsbCommandByte, pData, nBits, clock, fixeddata, vardata, pResults);
}

int usbFunctions::usbMSADeviceAllSlims(unsigned short thisstep, unsigned short filtbank, unsigned long *pResults)
{
  if (!USBDevice) return 0;
  return UsbMSADeviceAllSlims(USBDevice, thisstep, filtbank, pResults);
}

int usbFunctions::usbMSADeviceAllSlimsAndLoad(unsigned short thisstep, unsigned short filtbank, unsigned short latch, unsigned short pdmcmd, unsigned short pdmlatch, unsigned long *pResults)
{
  if (!USBDevice) return 0;
  return UsbMSADeviceAllSlimsAndLoad(USBDevice, thisstep, filtbank, latch, pdmcmd, pdmlatch, pResults);
}

int usbFunctions::usbMSADeviceAllSlimsAndLoadStruct(unsigned short *pData, unsigned long *pResults)
{
  if (!USBDevice) return 0;
  return UsbMSADeviceAllSlimsAndLoadStruct(USBDevice, pData, pResults);
}
