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
}

usbFunctions::~usbFunctions()
{
  usbCloseInterface();
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
 // HGLOBAL hDevMode = 0;
  //hDevMode = GlobalAlloc(10, 2002 * 40);
  //needs to be more complex - this flag uses USB if it is available
  //do not set it if bUsbAvailable is not set
  //create a memory block for holding dds data (akin to cmdallarray[][] but accessible within dll)
  AllArrayBlockSize = 2002*40;
  DeviceArrayBlockSize = 2002 * 8;
  hSAllArray = GlobalAlloc(GMEM_MOVEABLE, AllArrayBlockSize );
  ptrSAllArray = GlobalLock(hSAllArray );
  hSDDS1Array = GlobalAlloc(GMEM_MOVEABLE, DeviceArrayBlockSize);
  ptrSDDS1Array = GlobalLock(hSDDS1Array );
  hSDDS3Array = GlobalAlloc(GMEM_MOVEABLE, DeviceArrayBlockSize);
  ptrSDDS3Array = GlobalLock(hSDDS3Array );
  hSPLL1Array = GlobalAlloc(GMEM_MOVEABLE, DeviceArrayBlockSize);
  ptrSPLL1Array = GlobalLock(hSPLL1Array );
  hSPLL3Array = GlobalAlloc(GMEM_MOVEABLE, DeviceArrayBlockSize);
  ptrSPLL3Array = GlobalLock(hSPLL3Array );
#endif
  unsigned long result = 0;
  usbMSADeviceSetAllArrayPtr(ptrSAllArray,2002,40, &result);
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
  if (USBDevice)
  {
#ifdef __WIN32__
    GlobalFree(hSAllArray);
    AllArrayBlockSize = maxPoints * 40;
    hSAllArray = GlobalAlloc(GMEM_MOVEABLE, AllArrayBlockSize );
    ptrSAllArray = GlobalLock(hSAllArray);
    DeviceArrayBlockSize = maxPoints * 8;
    hSDDS1Array = GlobalAlloc(GMEM_MOVEABLE, DeviceArrayBlockSize);
    ptrSDDS1Array = GlobalLock(hSDDS1Array);
    hSDDS3Array = GlobalAlloc(GMEM_MOVEABLE, DeviceArrayBlockSize);
    ptrSDDS3Array = GlobalLock(hSDDS3Array);
    hSPLL1Array = GlobalAlloc(GMEM_MOVEABLE, DeviceArrayBlockSize);
    ptrSPLL1Array = GlobalLock(hSPLL1Array);
    hSPLL3Array = GlobalAlloc(GMEM_MOVEABLE, DeviceArrayBlockSize);
    ptrSPLL3Array = GlobalLock(hSPLL3Array);
#endif
    unsigned long result = 0;
    usbMSADeviceSetAllArrayPtr(ptrSAllArray,maxPoints,40, &result);
  }
}

int usbFunctions::usbMSADeviceSetAllArrayPtr(void *pAllArray, short nArrayRows, short nArrayCols, unsigned long *pResults)
{
  return UsbMSADeviceSetAllArrayPtr(USBDevice, pAllArray, nArrayRows, nArrayCols, pResults);
}

int usbFunctions::usbMSADeviceWriteString(QString data, int message_size)
{
  int retVal;
  QByteArray byteArray = data.toUtf8();
  char* cString = (char*)byteArray.constData();

  if (!USBDevice) return 0;

  retVal =  UsbMSADeviceWriteString(USBDevice, cString, message_size);
  return retVal;
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

int usbFunctions::usbMSADevicePopulateDDSArrayBitReverse(qint64 *pArray, unsigned long *pData, unsigned short step, unsigned short bits, unsigned long *pResults)
{
  int retVal;
  if (!USBDevice) return 0;
  retVal = UsbMSADevicePopulateDDSArrayBitReverse(USBDevice, pArray, pData, step, bits, pResults);
  return retVal;
}

int usbFunctions::usbMSADevicePopulateDDSArray(qint64 *pArray, unsigned long *pData, unsigned short step, unsigned long *pResults)
{
  if (!USBDevice) return 0;
  return UsbMSADevicePopulateDDSArray(USBDevice, pArray, pData, step, pResults);
}

int usbFunctions::usbMSADevicePopulateAllArray(unsigned short Steps, unsigned short bits, qint64 *pBit0Array, qint64 *pBit1Array, qint64 *pBit2Array, qint64 *pBit3Array, qint64 *pBit4Array, qint64 *pBit5Array, qint64 *pBit6Array, qint64 *pBit7Array, unsigned long *pResults)
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
