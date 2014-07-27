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
#ifndef LIBUSBFUNCTIONS_H
#define LIBUSBFUNCTIONS_H

#include "usbfunc.h"
#include "libusb.h"

#include <QtCore>

#define DLL_VERSION 0x10

// This is the structure used for messages returned to the caller
// from the USB chip.

typedef struct _MSA_RXBUFFER {
  unsigned char last_command;		// command type last received
  unsigned char return_status;	// see below for status flag defintions
  unsigned char ioa;				// FX2 PortA data
  unsigned char iob;				// FX2 PortB data
  unsigned char ioc;				// FX2 PortC data
  unsigned char iod;				// FX2 PortD data
  unsigned char ioe;				// FX2 PortE data
  unsigned char ADC_reads_done;	// Number of ADC reads performed
  unsigned char data[240];		// VARIABLE nuumber of ADC reads performed
} MSA_RXBUFFER;


// bit definitions in MSA_RXBUFFER.return_status
//
//#define bMsaStatusAdcTimeoutFlag	0x80	// If set, FX2 timed out ADC read operation
//#define bMsaStatusNoMSAPowerFlag	0x40	// If set, portB bit 7 is low so no MSA power
#define bMsnaStatusAdcDataReadyFlag	0x20	// If set, the data[] part conatins ADC data
//#define bMsaStatusAdcConvPendFlag	0x10	// Internal flag, if high FX2 is waiting to start ADC read operations


// There are two types of message that we may send to the MSA. This is managed as a union.
// The first one is a raw port write to FX2 ports A and/or B.

typedef struct _MSA_TXBUFFER_RAW {
  unsigned char command_code;		// set to 0x5A to signal raw write
  unsigned char flags;			// see below for definitions
  unsigned char portA;			// value to write to port A if flag set
  unsigned char portB;			// value to write to port B if flag set
  unsigned char portC;			// value to write to port B if flag set
  unsigned char portD;			// value to write to port B if flag set
  unsigned char portE;			// value to write to port B if flag set
} MSA_TXBUFFER_RAW;
//
// bit definitions for Flags for raw command
//
#define CmdMSARawDataFlagsWriteA 0x80              //if set, write specifiied value to port A
#define CmdMSARawDataFlagsWriteB 0x40              //if set, write specifiied value to port B
#define CmdMSARawDataFlagsWriteC 0x20              //if set, write specifiied value to port C
#define CmdMSARawDataFlagsWriteD 0x10              //if set, write specifiied value to port D
#define CmdMSARawDataFlagsWriteE 0x08              //if set, write specifiied value to port E

typedef struct _MSA_TXBUFFER_SET {
  unsigned char command_code;		// command code 0xa0-A4 for Set command
  unsigned char length;			// see belo for definitions
  unsigned char data[254];			// LO data to write to DDS
} MSA_TXBUFFER_SET;


typedef union _MSA_TXBUFFER {
  MSA_TXBUFFER_RAW raw;
  MSA_TXBUFFER_SET set;
} MSA_TXBUFFER;

class libUsbFunctions : public usbFunc
{
public:
  libUsbFunctions();
  ~libUsbFunctions();

  int usbMSAGetVersions();
  void usbMSARelease();
  int usbMSAInit();
  void resizeMemory(int maxPoints);

  int usbMSADeviceSetAllArrayPtr(void *pAllArray, short nArrayRows, short nArrayCols, unsigned long *pResults );

  int usbMSADeviceWriteString(QString data, int message_size );
  /*int usbMSADeviceReadAdcs(char *data, int message_size, unsigned long *pResults );
   * */
  int usbMSADeviceReadAdcsStruct(unsigned short *pData, unsigned long *pResults );

  int usbMSADevicePopulateDDSArrayBitReverse(qint64 *pArray, unsigned long *pData, unsigned short step, unsigned short bits, unsigned long *pResults );

  int usbMSADevicePopulateDDSArray(qint64 *pArray, unsigned long *pData, unsigned short step, unsigned long *pResults );

  int usbMSADevicePopulateAllArray(unsigned short Steps, unsigned short bits,
                                qint64 *pBit0Array,
                                qint64 *pBit1Array,
                                qint64 *pBit2Array,
                                qint64 *pBit3Array,
                                qint64 *pBit4Array,
                                qint64 *pBit5Array,
                                qint64 *pBit6Array,
                                qint64 *pBit7Array,
                                unsigned long *pResults );

  int usbMSADeviceWriteInt64MsbFirst(short nUsbCommandByte, unsigned long *pData,
                                  short nBits, short clock, short fixeddata, short vardata,
                                  unsigned long *pResults );

  int usbMSADeviceAllSlims(unsigned short thisstep, unsigned short filtbank, unsigned long *pResults );
  /*int usbMSADeviceAllSlimsAndLoad(unsigned short thisstep, unsigned short filtbank, unsigned short latch, unsigned short pdmcmd, unsigned short pdmlatch, unsigned long *pResults );
*/
  int usbMSADeviceAllSlimsAndLoadStruct(unsigned short *pData, unsigned long *pResults );


  bool usbInterfaceOpen(QString fileName);
  void usbCloseInterface();

private:
  libusb_device_descriptor desc;
  libusb_config_descriptor *config;
  libusb_device_handle *device = NULL;
  libusb_device *dev = NULL;

  bool Write(MSA_TXBUFFER * writebuf, int message_size);
  bool Read(MSA_RXBUFFER * readbuf);
  inline void ProcessBitArray( unsigned char *pVal, int bits, qint64 x, int bit );


  int nArrayCols;
  int nSteps;
  char *pAllArray;
};

#endif // USBFUNCTIONS_H
