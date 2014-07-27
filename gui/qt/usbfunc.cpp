#include "usbfunc.h"


usbFunc::usbFunc()
{

}

usbFunc::~usbFunc()
{

}

int usbFunc::usbMSAGetVersions()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

void usbFunc::usbMSARelease()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

int usbFunc::usbMSAInit()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

void usbFunc::resizeMemory(int maxPoints)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

int usbFunc::usbMSADeviceSetAllArrayPtr(void *pAllArray, short nArrayRows, short nArrayCols, unsigned long *pResults)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

int usbFunc::usbMSADeviceWriteString(QString data, int message_size)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

int usbFunc::usbMSADeviceReadAdcs(char *data, int message_size, unsigned long *pResults)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

int usbFunc::usbMSADeviceReadAdcsStruct(unsigned short *pData, unsigned long *pResults)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

int usbFunc::usbMSADevicePopulateDDSArrayBitReverse(long long *pArray, unsigned long *pData, unsigned short step, unsigned short bits, unsigned long *pResults)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

int usbFunc::usbMSADevicePopulateDDSArray(long long *pArray, unsigned long *pData, unsigned short step, unsigned long *pResults)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

int usbFunc::usbMSADevicePopulateAllArray(unsigned short Steps, unsigned short bits, long long *pBit0Array, long long *pBit1Array, long long *pBit2Array, long long *pBit3Array, long long *pBit4Array, long long *pBit5Array, long long *pBit6Array, long long *pBit7Array, unsigned long *pResults)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

int usbFunc::usbMSADeviceWriteInt64MsbFirst(short nUsbCommandByte, unsigned long *pData, short nBits, short clock, short fixeddata, short vardata, unsigned long *pResults)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

int usbFunc::usbMSADeviceAllSlims(unsigned short thisstep, unsigned short filtbank, unsigned long *pResults)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

int usbFunc::usbMSADeviceAllSlimsAndLoad(unsigned short thisstep, unsigned short filtbank, unsigned short latch, unsigned short pdmcmd, unsigned short pdmlatch, unsigned long *pResults)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

int usbFunc::usbMSADeviceAllSlimsAndLoadStruct(unsigned short *pData, unsigned long *pResults)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

bool usbFunc::usbInterfaceOpen(QString fileName)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}

void usbFunc::usbCloseInterface()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
}
