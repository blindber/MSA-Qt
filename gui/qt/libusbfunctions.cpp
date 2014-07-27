#include "libusbfunctions.h"
#include <QtCore>
#include <QMessageBox>


libUsbFunctions::libUsbFunctions()
{
  device = NULL;
  int r;
  r = libusb_init(NULL);
  if (r < 0)
  {
    qDebug() << "libusb_init: failed";
    return;
  }
  else
  {
    qDebug() << "libusb_init: ok";
  }
  //create a memory block for holding dds data (akin to cmdallarray[][] but accessible within dll)
  AllArrayBlockSize = 2002*40;
  DeviceArrayBlockSize = 2002 * 8;

  ptrSAllArray = malloc(AllArrayBlockSize);
  ptrSDDS1Array = malloc(DeviceArrayBlockSize);
  ptrSDDS3Array = malloc(DeviceArrayBlockSize);
  ptrSPLL1Array = malloc(DeviceArrayBlockSize);
  ptrSPLL3Array = malloc(DeviceArrayBlockSize);

  unsigned long result = 0;
  usbMSADeviceSetAllArrayPtr(ptrSAllArray,2002,40, &result);
}

libUsbFunctions::~libUsbFunctions()
{
  libusb_exit(NULL);
}

int libUsbFunctions::usbMSAGetVersions()
{
  return (desc.bcdDevice & 0xff) + (DLL_VERSION<<8);
}

void libUsbFunctions::usbMSARelease()
{
 // nothing to do yet
}

int libUsbFunctions::usbMSAInit()
{
  // nothing to do yet
  return 1;
}

void libUsbFunctions::resizeMemory(int maxPoints)
{
  free (ptrSAllArray);
  free (ptrSDDS1Array);
  free (ptrSDDS3Array);
  free (ptrSPLL1Array);
  free (ptrSPLL3Array);
  AllArrayBlockSize = maxPoints * 40;
  ptrSAllArray = malloc(AllArrayBlockSize);
  DeviceArrayBlockSize = maxPoints * 8;
  ptrSDDS1Array = malloc(DeviceArrayBlockSize);
  ptrSDDS3Array = malloc(DeviceArrayBlockSize);
  ptrSPLL1Array = malloc(DeviceArrayBlockSize);
  ptrSPLL3Array = malloc(DeviceArrayBlockSize);
  unsigned long result = 0;
  usbMSADeviceSetAllArrayPtr(ptrSAllArray,maxPoints,40, &result);
}

int libUsbFunctions::usbMSADeviceSetAllArrayPtr(void *pAllArray, short nArrayRows, short nArrayCols, unsigned long *pResults)
{
  this->pAllArray = ( char *)pAllArray;
  this->nSteps = nArrayRows;
  this->nArrayCols = nArrayCols;
  return 1;
}

int libUsbFunctions::usbMSADeviceWriteString(QString data, int message_size)
{
  MSA_TXBUFFER writebuf;
  QByteArray byteArray = data.toUtf8();
  char* cString = (char*)byteArray.constData();

  for( int i=0; i< message_size && i < 254; i++ )
  {
    if( !isxdigit( *cString ) )
      return false;
    unsigned char c = toupper(*cString);
    cString++;
    c -= '0';
    if( c > 9 )
      c = c -'A' + '0' + 10;
    if( !isxdigit( *cString ) )
      return false;
    unsigned char c1 =  toupper(*cString);
    cString++;
    c1 -= '0';
    if( c1 > 9 )
      c1 = c1 -'A' + '0' + 10;
    c = (c<<4)+c1;
    if( i == 0 )
      writebuf.set.command_code = c;
    else if( i == 1 )
      writebuf.set.length = c;
    else
      writebuf.set.data[i-2] = c;
  }
  //MSADevice* MSA = (MSADevice* )pMSAData;
  return Write(&writebuf, message_size);
}

int libUsbFunctions::usbMSADeviceReadAdcsStruct(unsigned short *pData, unsigned long *pResults)
{
  MSA_RXBUFFER readbuf;
  MSA_TXBUFFER writebuf;

  if( pData[0] < 1 || pData[0] > 3 )
    return 0;
  writebuf.set.command_code = 0xB0+pData[0]-1;
  writebuf.set.length = (unsigned char)pData[1];
  writebuf.set.data[0] = (unsigned char)pData[2];
  writebuf.set.data[1] = (unsigned char)pData[3];
  writebuf.set.data[2] = (unsigned char)pData[4];
  if( !Write(&writebuf, 0x5))
    return 0;
  for( int i=0; i < 5; i++ )
  {
    if( !Read( &readbuf ) )
      return false;
    if( readbuf.ADC_reads_done != 0 )
    {
      pResults[0] = readbuf.ADC_reads_done;
      pResults[1] = *((unsigned long *)&readbuf.data[0]);
      if( readbuf.ADC_reads_done > 1 )
        pResults[2] = *((unsigned long *)&readbuf.data[4]);
      return 1;
    }
  }
  return 0;
}

int libUsbFunctions::usbMSADevicePopulateDDSArrayBitReverse(long long *pArray, unsigned long *pData, unsigned short step, unsigned short bits, unsigned long *pResults)
{
  qint64 y = 0;
  qint64 x = (((qint64)(pData[0])) <<32)+pData[1];
  for (int i = 0; i < bits; ++i)
  {
    y <<= 1;
    y |= (x & 1);
    x >>= 1;
  }

  pArray[step] = y;
  return 1;
}

int libUsbFunctions::usbMSADevicePopulateDDSArray(long long *pArray, unsigned long *pData, unsigned short step, unsigned long *pResults)
{
  qint64 llData = (((qint64)(pData[0])) <<32)+pData[1];
  pArray[step] = llData;
  return 1;
}

int libUsbFunctions::usbMSADevicePopulateAllArray(unsigned short Steps, unsigned short bits, long long *pBit0Array, long long *pBit1Array, long long *pBit2Array, long long *pBit3Array, long long *pBit4Array, long long *pBit5Array, long long *pBit6Array, long long *pBit7Array, unsigned long *pResults)
{
  unsigned char val[64];
  int i,j;

  if( pAllArray == NULL )
    return 0;
  if( nSteps < Steps )
    return 0;
  if( nArrayCols < bits )
    return 0;
  for( i=0; i<= Steps; i++)
  {
    for( j=0; j<bits; j++)
      val[j] = 0;
    if( pBit0Array != 0 )
      ProcessBitArray( val, bits, pBit0Array[i], 1 );
    if( pBit1Array != 0 )
      ProcessBitArray( val, bits, pBit1Array[i], 2 );
    if( pBit2Array != 0 )
      ProcessBitArray( val, bits, pBit2Array[i], 4 );
    if( pBit3Array != 0 )
      ProcessBitArray( val, bits, pBit3Array[i], 8 );
    if( pBit4Array != 0 )
      ProcessBitArray( val, bits, pBit4Array[i], 0x10 );
    if( pBit5Array != 0 )
      ProcessBitArray( val, bits, pBit5Array[i], 0x20 );
    if( pBit6Array != 0 )
      ProcessBitArray( val, bits, pBit6Array[i], 0x40 );
    if( pBit7Array != 0 )
      ProcessBitArray( val, bits, pBit7Array[i], 0x80 );
    for( j=0; j<bits; j++)
      *(pAllArray+bits*i+j) = val[j];
  }
  return 1;

}

int libUsbFunctions::usbMSADeviceWriteInt64MsbFirst(short nUsbCommandByte, unsigned long *pData, short nBits, short clock, short fixeddata, short vardata, unsigned long *pResults)
{
  if( nBits > 63 || nBits < 1 )
    return 0;

  MSA_TXBUFFER writebuf;
  unsigned char nByte;
  qint64 x = (((qint64)(pData[0])) <<32)+pData[1];
  qint64 nBitMask = 1<<(nBits-1);

  writebuf.set.command_code = nUsbCommandByte & 0xff;
  writebuf.set.length = nBits & 0xff;
  writebuf.set.data[0] = clock & 0xff;
  for( int i=1; i<=nBits; i++, nBitMask >>=1)
  {
    nByte = (fixeddata&0xff)+( (x&nBitMask) ? vardata & 0xff : 0 );
    writebuf.set.data[i] = nByte;
  }
  if( !Write(&writebuf, nBits+3))
    return 0;
  return 1;
}

int libUsbFunctions::usbMSADeviceAllSlims(unsigned short thisstep, unsigned short filtbank, unsigned long *pResults)
{
  //MSADevice* MSA = (MSADevice* )pMSAData;
  char *pData = pAllArray + (thisstep * nArrayCols);

  MSA_TXBUFFER writebuf;

  if( pAllArray == NULL )
    return 0;
  if( thisstep > nSteps )
    return 0;
  writebuf.set.command_code = 0xA1;
  writebuf.set.length = 0x28;
  writebuf.set.data[0] = 1;
  for( int i=0; i<40; i++)
    writebuf.set.data[i+1] = (*pData++) +filtbank;
  if( !Write(&writebuf, 0x2B))
    return 0;
  writebuf.set.command_code = 0xA1;
  writebuf.set.length = 0x1;
  writebuf.set.data[0] = 0;
  writebuf.set.data[1] = (unsigned char)filtbank;
  if( !Write(&writebuf, 0x4))
    return 0;
  return 1;
}

int libUsbFunctions::usbMSADeviceAllSlimsAndLoadStruct(unsigned short *pData, unsigned long *pResults)
{
  MSA_TXBUFFER writebuf;
  //MSADevice* MSA = (MSADevice* )pMSAData;
  if( !usbMSADeviceAllSlims( pData[0] /*thisstep*/, pData[1] /*filtbank*/, pResults ) )
    return 0;
  writebuf.set.command_code = 0xA3;
  writebuf.set.length = 0x3;
  writebuf.set.data[0] = 0;
  unsigned char pdmcmd = (unsigned char)(pData[3]*pData[4]);
  writebuf.set.data[1] = (unsigned char)(pData[2]+pdmcmd); // (latch+pdmcmd);
  writebuf.set.data[2] = (unsigned char)(pdmcmd+pData[5]);
  writebuf.set.data[3] = (unsigned char)(pdmcmd);
  if( !Write(&writebuf, 0x6))
    return 0;
  return 1;
}

bool libUsbFunctions::usbInterfaceOpen(QString fileName)
{
  libusb_device **devs;

  int cnt = libusb_get_device_list(NULL, &devs);
  if (cnt < 0)
    return false;

  int i = 0;
  bool devFound = false;
  while ((dev = devs[i++]) != NULL)
  {
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0)
    {
      qDebug() << "failed to get device descriptor";
      return false;
    }

    if (desc.idVendor == 0x0547 && desc.idProduct == 0x1015)
    {
      qDebug() << "Found MSA interface";
      devFound = true;
      break;
    }
  }
  if (devFound)
  {
    int status = libusb_open(dev, &device);
    status = libusb_get_config_descriptor (dev, 0, &config);

    status = libusb_claim_interface(device, 0);
    if (status == 0)
      qDebug() << "USB device claimed";
  }
  else
  {
    device = NULL;
  }
  libusb_free_device_list (devs, 1);
  return devFound;
}

void libUsbFunctions::usbCloseInterface()
{
  //release the interface claimed earlier
  libusb_release_interface (device,  0);
  //all tasks done close the device handle
  libusb_close (device);
  device = NULL;
}

bool libUsbFunctions::Write(MSA_TXBUFFER *writebuf, int message_size)
{
  //LONG len = message_size;
  int sent;
  unsigned char *wbuf = (unsigned char *)writebuf;

  if (device == NULL)
    return false;

  int err = libusb_bulk_transfer(device, 2, wbuf, message_size, &sent, 100);
  if (err != 0)
  {
    qDebug() << __FILE__ << " " << __FUNCTION__ << ":" << err;
  }
  return sent;
}

bool libUsbFunctions::Read(MSA_RXBUFFER *readbuf)
{
  int received;
  int len = 255;
  unsigned char *rbuf = (unsigned char *)readbuf;

  if (device == NULL)
    return false;

  int err = libusb_bulk_transfer ( device, 0x86, rbuf, len, &received, 100);
  if (err != 0)
  {
    qDebug() << __FILE__ << " " << __FUNCTION__ << ":" << err;
  }
  return received;
}

void libUsbFunctions::ProcessBitArray(unsigned char *pVal, int bits, long long x, int bit)
{
  qint64 mask = 1;
  for( int j=0; j<bits; j++, mask <<=1)
  {
    if( x & mask )
    {
      pVal[j] |=  bit;
    }
  }
}
