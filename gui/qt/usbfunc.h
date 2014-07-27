#ifndef USBFUNC_H
#define USBFUNC_H

#include <QtCore>

class usbFunc
{
public:
  usbFunc();
  virtual ~usbFunc();
  virtual int usbMSAGetVersions();
  virtual void usbMSARelease();
  virtual int usbMSAInit();
  virtual void resizeMemory(int maxPoints);

  virtual int usbMSADeviceSetAllArrayPtr(void *pAllArray, short nArrayRows, short nArrayCols, unsigned long *pResults );
  virtual int usbMSADeviceWriteString(QString data, int message_size );
  virtual int usbMSADeviceReadAdcs(char *data, int message_size, unsigned long *pResults );
  virtual int usbMSADeviceReadAdcsStruct(unsigned short *pData, unsigned long *pResults );
  virtual int usbMSADevicePopulateDDSArrayBitReverse(__int64 *pArray, unsigned long *pData, unsigned short step, unsigned short bits, unsigned long *pResults );
  virtual int usbMSADevicePopulateDDSArray(__int64 *pArray, unsigned long *pData, unsigned short step, unsigned long *pResults );
  virtual int usbMSADevicePopulateAllArray(unsigned short Steps, unsigned short bits,
                                __int64 *pBit0Array,
                                __int64 *pBit1Array,
                                __int64 *pBit2Array,
                                __int64 *pBit3Array,
                                __int64 *pBit4Array,
                                __int64 *pBit5Array,
                                __int64 *pBit6Array,
                                __int64 *pBit7Array,
                                unsigned long *pResults );
  virtual int usbMSADeviceWriteInt64MsbFirst(short nUsbCommandByte, unsigned long *pData,
                                  short nBits, short clock, short fixeddata, short vardata,
                                  unsigned long *pResults );
  virtual int usbMSADeviceAllSlims(unsigned short thisstep, unsigned short filtbank, unsigned long *pResults );
  virtual int usbMSADeviceAllSlimsAndLoad(unsigned short thisstep, unsigned short filtbank, unsigned short latch, unsigned short pdmcmd, unsigned short pdmlatch, unsigned long *pResults );
  virtual int usbMSADeviceAllSlimsAndLoadStruct(unsigned short *pData, unsigned long *pResults );


  virtual bool usbInterfaceOpen(QString fileName);
  virtual void usbCloseInterface();

  unsigned long int64SW[2];
  unsigned long int64N[2];

  void * ptrSDDS1Array;
  void * ptrSPLL1Array;
  void * ptrSPLL3Array;
  void * ptrSDDS3Array;

  int AllArrayBlockSize; // current size of memory block allocated for AllArrays
  int DeviceArrayBlockSize;
  void * hSAllArray; // handle for memory block
  void * ptrSAllArray; // pointer to memory block

  void * hSDDS1Array;
  void * hSDDS3Array;
  void * hSPLL1Array;
  void * hSPLL3Array;

};


#endif // USBFUNC_H
