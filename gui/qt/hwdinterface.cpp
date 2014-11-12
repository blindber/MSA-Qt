#include "hwdinterface.h"
#include <QtCore>
#include <QString>
#include "constants.h"
#include "complexmaths.h"

hwdInterface::hwdInterface(QWidget *parent)
{
#ifdef USELIBUSB
  usb = new libUsbFunctions;
#else
  usb = new usbFunctions;
#endif
  calMan = new dialogCalManager(parent);
  specTests = new dialogSpecialTests(this, parent);


  // connect up the signals so we can tell the main windows what to do
  connect(this, SIGNAL(DisplayButtonsForHalted()), parent, SLOT(DisplayButtonsForHalted()));
  connect(this, SIGNAL(DisplayButtonsForRunning()), parent, SLOT(DisplayButtonsForRunning()));
  connect(this, SIGNAL(RequireRestart()), parent, SLOT(RequireRestart()));
  connect(this, SIGNAL(CleanupAfterSweep()), parent, SLOT(CleanupAfterSweep()));
  connect(this, SIGNAL(ChangeMode()), parent, SLOT(ChangeMode()));
  connect(this, SIGNAL(SkipHardwareInitialization()), parent, SLOT(SkipHardwareInitialization()));
  connect(this, SIGNAL(ResizeArrays(int)), parent, SLOT(ResizeArrays(int)));
  connect(this, SIGNAL(ProcessAndPrint()), parent, SLOT(ProcessAndPrint()));
  connect(this, SIGNAL(PrintMessage(QString)), parent, SLOT(PrintMessage(QString)));
  connect(this, SIGNAL(Halted()), parent, SLOT(Halted()));


  port = 0;
  status = 0;
  control = 0;
  globalSTRB = 0;
  globalINIT = 0;
  globalSELT = 0;
  globalContClear = 0;
  swclk = 0;
  LEPLL = 0;
  sfqud = 0;
  pdmlowlim = 0;
  pdmhighlim = 0;
  bUseUsb = 0;
  glitch = 0;
  glitchp1 = 0;
  glitchd1 = 0;
  glitchp3 = 0;
  glitchd3 = 0;
  glitchpdm = 0;
  glitchhlt = 0;
  haltWasAtEnd = 0;
  syncsweep = 0;
  setpdm = 0;
  convdatapwr = 0;
  vnalintest = 0;
  test = 0;
  cftest = 0;
  magdata = 0;
  phadata = 0;
  onestep = 0;
  startTime = 0;
  scanResumed = 0;
  suppressSweepTime = 0;
  rcounter = 0;
  appxpdf = 0;
  pdf3 = 0;
  datavalue = 0;
  levalue = 0;
  pdf2 = 0;
  appxVCO = 0;
  ncounter2 = 0;
  fcounter1 = 0;
  lastfcounter1 = 0;
  fcounter2 = 0;
  fcounter3 = 0;
  lastfcounter3 = 0;
  lastncounter1 = 0;
  lastncounter3 = 0;
  ncounter1 = 0;
  ncounter3 = 0;
  rcounter3 = 0;
  rcounter2 = 0;
  rcounter1 = 0;
  phasepolarity = 0;
  fractional = 0;
  preselector = 0;
  Jcontrol = 0;
  filtbank = 0;
  pdmcmd = 0;
  Acounter2 = 0;
  LO2 = 0;
  Bcounter2 = 0;

  //private vars
  enterPLL2phasefreq = 0;
  difPhase = 0;
  ddsoutput = 0;
  ddsclock = 0;

  pdf = 0;
  LO1 = 0;
  ncounter = 0;
  fcounter = 0;
  Bcounter = 0;
  Acounter = 0;

  resizeArrays(initMemSize);

}

hwdInterface::~hwdInterface()
{
  delete specTests;
  delete calMan;
  delete usb;
}
void hwdInterface::setActiveConfig(msaConfig *newActiveConfig)
{
  activeConfig = newActiveConfig;
  calMan->setMsaConfig(newActiveConfig);
}

void hwdInterface::setGlobalVars(globalVars *newVar)
{
  vars = newVar;
  calMan->setGlobalVars(newVar);
}

void hwdInterface::setUwork(cWorkArray *newuWork)
{
  uWork = newuWork;
  touch.setUwork(newuWork);
  oslCal.setUwork(newuWork);
}

void hwdInterface::setFilePath(QString Dir)
{
  DefaultDir = Dir;

  mSAconfig::coax.setFilePath(Dir);

  calMan->setFilePath(Dir);
}

void hwdInterface::setVna(dialogVNACal *newVnaCal)
{
  vnaCal = newVnaCal;
}

void hwdInterface::resizeArrays(int newSize)
{
  pll_1.resize(newSize);
  pll_3.resize(newSize);
  usb->resizeMemory(newSize);
  lpt.cmdAllArray.resize(newSize);
  calMan->resizeArrays(newSize);

}

void hwdInterface::initVars()
{
  globalSTRB=STRB;
  globalINIT=INIT;
  globalSELT=SELT;
  globalContClear=contclear;

  switch (activeConfig->cb)
  {
    case hwdOld:
      le1=4;
      le2=8;
      le3=16;
      fqud1=STRB;
      fqud3=2;
      break;
    case hwdOldNewHarness:
      le1=1;
      le2=1;
      le3=4;
      fqud1=2;
      fqud3=8;
      break;
    case hwdSlim:
      le1=1;
      le2=16;
      le3=4;
      fqud1=2;
      fqud3=8;
      break;
    case hwdUsbV1:
      le1=1;
      le2=16;
      le3=4;
      fqud1=2;
      fqud3=8;
    break;
  }
  lpt.setLatchLines(le1, le2, le3, fqud1, fqud3);
  if (activeConfig->adconv == 8)
  {
    pdmlowlim = 51 ;
    pdmhighlim = 205; //establish boundries for 8 bit parallel A to D ver111-36f
  }
  if (activeConfig->adconv == 12)
  {
    pdmlowlim = 819 ;
    pdmhighlim = 3277; //establish boundries for 12 bit parallel A to D ver111-36f
  }
  if (activeConfig->adconv == 16)
  {
    pdmlowlim = 13107 ;
    pdmhighlim = 52429; //establish boundries for 16 bit serial A to D ver111-36f
  }
  if (activeConfig->adconv == 22)
  {
    pdmlowlim = 819 ;
    pdmhighlim = 3277; //establish boundries for 12 bit serial A to D ver111-37a
  }
  lpt.init(inpoutLib, activeConfig->globalPort);
  //lpt.init(ntPortLin);

}

void hwdInterface::CreateRcounter()
{
  rcounter = (int(round(vars->reference/appxpdf)));
  pdf = vars->reference/rcounter;
}
void hwdInterface::CommandPLL1R()
{
  //needed:rcounter1,PLL1mode,PLL1phasepolarity,SELT,PLL1
  rcounter = rcounter1;
  preselector = 32;
  if (activeConfig->PLL1mode == 1)
  {
    preselector = 16;
  }
  phasepolarity = activeConfig->PLL1phasepolarity;    //inverting op amp is 0, non-inverting loop is 1
  fractional = activeConfig->PLL1mode;       //0 for Integer-N; 1 for Fractional-N
  Jcontrol = SELT;   //for PLL 1, on Control Board J1, the value is "3"
  LEPLL = 4;         //for PLL 1, on Control Board J1, the value is "4"
  vars->PLL = activeConfig->PLL1;
  CommandRBuffer();//needs:rcounter,preselector,phasepolarity,fractional,Jcontrol,LEPLL,PLL
  if (errora.length()>0)
  {
    PrintMessage("PLL 1, " + errora);
    RequireRestart();
  }
  return;
}

void hwdInterface::CommandPLL2R()
{
  //needed:reference,appxpdf,PLL2phasepolarity,SELT,PLL2
  preselector = 32;
  phasepolarity = activeConfig->PLL2phasepolarity;  //1  //inverting op amp is 0, non-inverting loop is 1
  fractional = 0;    //0 for Integer-N; PLL 2 should not be fractional due to increased noise
  Jcontrol = SELT;   //for PLL 2, on Control Board J2, the value is "3"
  LEPLL = 8;          //for PLL 2, on Control Board J2, the value is "8"
  vars->PLL = activeConfig->PLL2;
  CommandRBuffer();//needs:rcounter,preselector,phasepolarity,fractional,Jcontrol,LEPLL,PLL

  if (errora.length()>0)
  {
    PrintMessage("PLL 2, " + errora);
    RequireRestart();
  }

  return; //to //CommandPLL2R and Init Buffers
}

void hwdInterface::CommandPLL3R()
{
  //needed:PLL3mode,PLL3phasepolarity,INIT,PLL3
  preselector = 32; if (activeConfig->PLL3mode == 1) preselector = 16;
  phasepolarity = activeConfig->PLL3phasepolarity;    //inverting op amp is 0, non-inverting loop is 1
  fractional = activeConfig->PLL3mode;       //0 for Integer-N; 1 for Fractional-N
  Jcontrol = INIT;   //for Tracking Gen PLL, on Control Board J3, the value is "15"
  LEPLL = 16;         //for Tracking Gen PLL, on Control Board J3, the value is "16"
  vars->PLL = activeConfig->PLL3;
  CommandRBuffer();//needs:rcounter,preselector,phasepolarity,fractional,Jcontrol,LEPLL,PLL

  if (errora.length()>0)
  {
    PrintMessage("PLL 3, " + errora);
    RequireRestart();
  }
  return; //to //CommandPLL3R and Init Buffers
}
void hwdInterface::CommandRBuffer()
{
  //needed:rcounter,preselector,phasepolarity,fractional,Jcontrol,LEPLL,PLL
  if (vars->PLL == 2325)
    Command2325R();//needs:rcounter,preselector,Jcontrol,port,LEPLL,contclear ; commands LMX2325 rcounter and registers
  else if (vars->PLL == 2326)
    Command2326R();//needs:rcounter,phasepolarity,Jcontrol,port,LEPLL,contclear ; commands LMX2326 rcounter and registers
  else if (vars->PLL == 2350)
    Command2350R();//needs:rcounter,phasepolarity,Jcontrol,port,LEPLL,contclear,fractional ; commands LMX2350 rcounter
  else if (vars->PLL == 2353)
    Command2353R();//needs:rcounter,phasepolarity,Jcontrol,port,LEPLL,contclear,fractional ; commands LMX2353 rcounter
  else if (vars->PLL == 4112)
    Command4112R();//needs:rcounter,preselector,phasepolarity,Jcontrol,port,LEPLL,contclear ; commands AD4112 rcounter
  return;
}
void hwdInterface::CreateIntegerNcounter()
{

  //needed:appxVCO,reference,rcounter ; creates:ncount,ncounter,fcounter(0),pdf
  double ncount = appxVCO/(vars->reference/rcounter);  //approximates the Ncounter for PLL
  ncounter = int(round(ncount));
  fcounter = 0;
  pdf = appxVCO/ncounter;        //actual phase freq of PLL
  return;  //to //CreatePLL2N,//[CalculateThisStepPLL1],or //[CalculateThisStepPLL3] with ncount, ncounter and fcounter(=0)

}

void hwdInterface::CreateFractionalNcounter()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CreateFractionalNcounter]'needed:appxVCO,reference,rcounter ; creates:ncount,ncounter,fcounter,pdf
    ncount = appxVCO/(reference/rcounter)  'approximates the Ncounter for PLL
    ncounter = int(ncount)    'actual value for PLL Ncounter
    fcount = ncount - ncounter
    fcounter = int(fcount*16) 'ver111
    if (fcount*16) - fcounter >= .5 then fcounter = fcounter + 1 'rounds off fcounter  ver111
    if fcounter = 16 then ncounter = ncounter + 1:fcounter = 0
    pdf = appxVCO/(ncounter + (fcounter/16)) 'actual phase freq for PLL 'ver111-10
    return  'with ncount,ncounter,fcounter,pdf

*/
}

void hwdInterface::AutoSpur()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[AutoSpur]'needed:LO1,LO2,finalfreq,appxdds1,dds1output,rcounter1,finalbw,fcounter,ncounter,spurcheck;changes pdf,dds1output
    '[AutoSpur] is a continuation of [CreateFractionalNcounter], used only in MSA when PLL 1 is Fractional
    spur = 0    'reset spur, and determine if there is potential for a spur
    firstif = LO2 - finalfreq
    fractionalfreq = dds1output/(rcounter1*16)
    harnonicb = int(firstif/fractionalfreq)
    if (firstif/fractionalfreq)-harnonicb >=.5 then harnonicb = harnonicb + 1  'rev108
    harnonica = harnonicb - 1
    harnonicc = harnonicb + 1
    firstiflow = LO2 - (finalfreq + finalbw/1000)
    firstifhigh = LO2 - (finalfreq - finalbw/1000)
    if harnonica*fractionalfreq > firstiflow and harnonica*fractionalfreq < firstifhigh then spur = 1
    if harnonicb*fractionalfreq > firstiflow and harnonicb*fractionalfreq < firstifhigh then spur = 1
    if harnonicc*fractionalfreq > firstiflow and harnonicc*fractionalfreq < firstifhigh then spur = 1
    if spur = 1 and (dds1output<appxdds1) then fcounter = fcounter - 1
    if spur = 1 and (dds1output>appxdds1) then fcounter = fcounter + 1
    if fcounter = 16 then ncounter = ncounter + 1:fcounter = 0  'rev108
    if fcounter <0 then ncounter = ncounter - 1:fcounter = 15  'rev108
    pdf = LO1/(ncounter + (fcounter/16))
    dds1output = pdf * rcounter1    'actual output of DDS1(input Ref to PLL1)
    return 'with possibly new ncounter,fcounter,pdf,dds1output
    */
}

void hwdInterface::ManSpur()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ManSpur]'needed:spurcheck,dds1output,appxdds1,fcounter,ncounter
    '[ManSpur] is a continuation of [CreateFractionalNcounter], used only in MSA when PLL 1 is Fractional
    if spurcheck = 1 and (dds1output<appxdds1) then fcounter = fcounter - 1 'causes +shift in pdf1
    if spurcheck = 1 and (dds1output>appxdds1) then fcounter = fcounter + 1 'causes -shift in pdf1
    if fcounter = 16 then ncounter = ncounter + 1:fcounter = 0  'rev108
    if fcounter < 0 then ncounter = ncounter - 1:fcounter = 15  'rev108
    pdf = LO1/(ncounter + (fcounter/16))
    dds1output = pdf * rcounter1    'actual output of DDS1(input Ref to PLL1)
    return 'with possibly new:ncounter,fcounter,pdf,dds1output
    */
}

void hwdInterface::CreatePLL1N()
{
  //needed:ncounter,fcounter,PLL1mode,PLL1
  preselector = 32;
  if (activeConfig->PLL1mode == 1)
  {
    preselector = 16;
  }
  PLL = activeConfig->PLL1;
  CreateNBuffer();  //needs:ncounter,fcounter,PLL,preselector;creates:Bcounter,Acounter, and N Bits N0-Nx
  if (errora.length()>0)
  {
    PrintMessage("PLL 1, " + errora);
    RequireRestart();
    return;
  }
  pll_1[vars->thisstep].Bcounter = Bcounter;
  pll_1[vars->thisstep].Acounter = Acounter;
  return;

}
void hwdInterface::CreatePLL2N()
{
  //needed:ncounter,fcounter,PLL2
  preselector = 32;
  vars->PLL = activeConfig->PLL2;
  CreateNBuffer();//needs:ncounter,fcounter,PLL,preselector;creates:Bcounter,Acounter, and N Bits N0-Nx
  if (errora.length()>0)
  {
    PrintMessage("PLL 2, " + errora);
    RequireRestart();
  }
  return;
}

void hwdInterface::CreatePLL3N()
{
  //needed:ncounter,fcounter,PLL3mode,PLL3  ver111-14
  preselector = 32;
  if (activeConfig->PLL3mode == 1)
  {
    preselector = 16;
  }
  PLL = activeConfig->PLL3;
  CreateNBuffer();//needs:ncounter,fcounter,PLL,preselector;creates:Bcounter,Acounter, and N Bits N0-Nx
  if (errora.length()>0)
  {
    PrintMessage("PLL 3, " + errora);
    RequireRestart();
    return;
  }
  pll_3[vars->thisstep].Bcounter = Bcounter;
  pll_3[vars->thisstep].Acounter = Acounter;
  return;
}


void hwdInterface::CreateNBuffer()
{
  //needed:PLL,ncounter,fcounter,preselector
  if (vars->PLL == 2325)
    Create2325N();//needs:ncounter,preselector; creates LMX2325 N Buffer ver111
  else if (vars->PLL == 2326)
    Create2326N();//needs:ncounter ; creates LMX2326 N Buffer ver111
  else if (vars->PLL == 2350)
    Create2350N();//needs:ncounter,preselector,fcounter; creates LMX2350 RFN Buffer ver111
  else if (vars->PLL == 2353)
    Create2353N();//needs: ncounter,preselector,fcounter; creates LMX2353 N Buffer ver111
  else if (vars->PLL == 4112)
    Create4112N();//needs:ncounter,preselector; creates AD4112 N Buffer ver111
  return; //with Bcounter,Acounter, and N Bits N0-N23
}

void hwdInterface::Create2325N()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed:ncounter,preselector; creates LMX2325 n buffer
    Bcounter = int(ncounter/preselector)
    Acounter = ncounter- (Bcounter * preselector)
    if Bcounter<3 then beep:errora$ = "2325 Bcounter < 3":return 'with errora$ ver111-37c
    if Bcounter>2047 then beep:errora$ = "2325 Bcounter > 2047":return 'with errora$ ver111-37c
    if Bcounter<Acounter then beep:errora$ = "2325 Bcounter<Acounter":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0 = 0    'address bit, 0 sets the N Buffer, 1 is for R Buffer
    na1 = int(Acounter/2):N1 = Acounter - 2*na1 'binary conversion from decimal
    na2 = int(na1/2):N2 = na1 - 2*na2
    na3 = int(na2/2):N3 = na2 - 2*na3
    na4 = int(na3/2):N4 = na3 - 2*na4
    na5 = int(na4/2):N5 = na4 - 2*na5
    na6 = int(na5/2):N6 = na5 - 2*na6
    na7 = int(na6/2):N7 = na6 - 2*na7
    nb8 = int(Bcounter/2):N8 = Bcounter - 2*nb8
    nb9 = int(nb8/2):N9 = nb8 - 2*nb9
    nb10 = int(nb9/2):N10 = nb9 - 2*nb10
    nb11 = int(nb10/2):N11 = nb10 - 2*nb11
    nb12 = int(nb11/2):N12 = nb11 - 2*nb12
    nb13 = int(nb12/2):N13 = nb12 - 2*nb13
    nb14 = int(nb13/2):N14 = nb13 - 2*nb14
    nb15 = int(nb14/2):N15 = nb14 - 2*nb15
    nb16 = int(nb15/2):N16 = nb15 - 2*nb16
    nb17 = int(nb16/2):N17 = nb16 - 2*nb17
    nb18 = int(nb17/2):N18 = nb17 - 2*nb18
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
    return
    */
}

void hwdInterface::Create2326N()
{
  //needed:ncounter ; creates LMX2326 n buffer  ver111
  Bcounter = int(ncounter/32);
  Acounter = int(ncounter-(Bcounter*32));
  if (Bcounter < 3)
  {
    util.beep();
    errora="2326 Bcounter <3";
    return; //with errora
  }
  if (Bcounter > 8191)
  {
    util.beep();
    errora="2326 Bcounter >8191";
    return; //with errora
  }
  if (Bcounter < Acounter)
  {
    util.beep();
    errora="2326 Bcounter<Acounter";
    return; //with errora
  }
  if (activeConfig->cb == hwdUsbV1)
  {
    usb->int64N[1] = (1 << 20) + (Bcounter << 7) + (Acounter << 2) + 1;
    usb->int64N[0] = 0;
  }
}

void hwdInterface::Create2350N()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed: ncounter,preselector,fcounter; creates LMX2350 RFN Buffer
    Bcounter = int(ncounter/preselector)
    Acounter = int(ncounter-(Bcounter*preselector))
    if Bcounter < 3 then beep:errora$="2350 Bcounter <3":return 'with errora$ ver111-37c
    if Bcounter > 1023 then beep:errora$="2350 Bcounter >1023":return 'with errora$ ver111-37c
    if Bcounter < Acounter + 2 then beep:errora$="2350 Bcounter<Acounter+2":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0=1      '2350 RF_N register, must be 1
    N1=1      '2350 RF_N register, must be 1 'was N=1 ver113-7a
    f0 = int(fcounter/2):N2 = fcounter - 2*f0      'fcounter bit 0
    f1 = int(f0/2):N3 = f0 - 2*f1       'fcounter bit 1
    f2 = int(f1/2):N4 = f1 - 2*f2       'fcounter bit 2
    f3 = int(f2/2):N5 = f2 - 2*f3       'fcounter bit 3 (0 to 15)
    rfna6 = int(Acounter/2):N6 = Acounter- 2*rfna6
    rfna7 = int(rfna6/2):N7 = rfna6 - 2*rfna7
    rfna8 = int(rfna7/2):N8 = rfna7 - 2*rfna8
    rfna9 = int(rfna8/2):N9 = rfna8 - 2*rfna9
    rfna10 = int(rfna9/2):N10 = rfna9 - 2*rfna10
    rfnb11 = int(Bcounter/2):N11 = Bcounter- 2*rfnb11
    rfnb12 = int(rfnb11/2):N12 = rfnb11 - 2*rfnb12
    rfnb13 = int(rfnb12/2):N13 = rfnb12 - 2*rfnb13
    rfnb14 = int(rfnb13/2):N14 = rfnb13 - 2*rfnb14
    rfnb15 = int(rfnb14/2):N15 = rfnb14 - 2*rfnb15
    rfnb16 = int(rfnb15/2):N16 = rfnb15 - 2*rfnb16
    rfnb17 = int(rfnb16/2):N17 = rfnb16 - 2*rfnb17  'was rgb17 ver113-7a
    rfnb18 = int(rfnb17/2):N18 = rfnb17 - 2*rfnb18
    rfnb19 = int(rfnb18/2):N19 = rfnb18 - 2*rfnb19
    rfnb20 = int(rfnb19/2):N20 = rfnb19 - 2*rfnb20
    N21=0 :if preselector = 32 then N21 = 1  '0=16/17  1=32/33
    N22=0     'Pwr down RF,    0=normal  1=pwr down
    N23=0     'RF cntr reset,  0=normal  1=reset
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
    return
    */
}

void hwdInterface::Create2353N()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  'needed: ncounter,preselector,fcounter; creates LMX2353 N Buffer
      Bcounter = int(ncounter/preselector)
      Acounter = int(ncounter-(Bcounter*preselector))
      if Bcounter < 3 then beep:errora$ = "2353 Bcounter is < 3":return 'with errora$ ver111-37c
      if Bcounter > 1023 then beep:errora$ = "2353 Bcounter is > 1023":return 'with errora$ ver111-37c
      if Bcounter < Acounter + 2 then beep:errora$ = "2353 Bcounter < Acounter+2":return 'with errora$ ver111-37c
      'ver116-4o deleted "if" block, per Lrev1
      N0 = 1       'n address bit 0
      N1 = 1       'n address bit 1
      f0 = int(fcounter/2):N2 = fcounter - 2*f0       'fcounter bit 0
      f1 = int(f0/2):N3 = f0 - 2*f1       'fcounter bit 1
      f2 = int(f1/2):N4 = f1 - 2*f2       'fcounter bit 2
      f3 = int(f2/2):N5 = f2 - 2*f3       'fcounter bit 3 (0 to 15)
      na0 = int(Acounter/2):N6 = Acounter- 2*na0      'Acounter bit 0 LSB
      na1 = int(na0/2):N7 = na0 - 2*na1
      na2 = int(na1/2):N8 = na1 - 2*na2
      na3 = int(na2/2):N9 = na2 - 2*na3
      na4 = int(na3/2):N10 = na3 - 2*na4      'Acounter bit 4 MSB
      nb0 = int(Bcounter/2):N11 = Bcounter- 2*nb0      'Bcounter bit 0 LSB
      nb1 = int(nb0/2):N12 = nb0 - 2*nb1
      nb2 = int(nb1/2):N13 = nb1 - 2*nb2
      nb3 = int(nb2/2):N14 = nb2 - 2*nb3
      nb4 = int(nb3/2):N15 = nb3 - 2*nb4
      nb5 = int(nb4/2):N16 = nb4 - 2*nb5
      nb6 = int(nb5/2):N17 = nb5 - 2*nb6
      nb7 = int(nb6/2):N18 = nb6 - 2*nb7
      nb8 = int(nb7/2):N19 = nb7 - 2*nb8
      nb9 = int(nb8/2):N20 = nb8 - 2*nb9      'Bcounter bit 9 MSB
      N21 = 0 :if preselector = 32 then N21 = 1  '0=16/17  1=32/33
      N22 = 0          'power down if 1
      N23 = 0          'counter reset if 1
      if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
              2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
              2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
      if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
      return
          */
}

void hwdInterface::Create4112N()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed: ncounter,preselector; creates AD4112 N Buffer
    Bcounter = int(ncounter/preselector)
    Acounter = int(ncounter-(Bcounter*preselector))
    if Bcounter < 3 then beep:errora$="4112 N counter <3":return 'with errora$ ver111-37c
    if Bcounter > 8191 then beep:errora$="4112 N counter >8191":return 'with errora$ ver111-37c
    if Bcounter < Acounter then beep:errora$="4112 B counter<Acounter":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0 = 1       'N address bit 0, must be 1
    N1 = 0       'N address bit 1, must be 0
    na0 = int(Acounter/2):N2 = Acounter- 2*na0      'Acounter bit 0 LSB
    na1 = int(na0/2):N3 = na0 - 2*na1
    na2 = int(na1/2):N4 = na1 - 2*na2
    na3 = int(na2/2):N5 = na2 - 2*na3
    na4 = int(na3/2):N6 = na3 - 2*na4
    na5 = int(na4/2):N7 = na4 - 2*na5      'Acounter bit 5 MSB
    nb0 = int(Bcounter/2):N8 = Bcounter- 2*nb0      'Bcounter bit 0 LSB
    nb1 = int(nb0/2):N9 = nb0 - 2*nb1
    nb2 = int(nb1/2):N10 = nb1 - 2*nb2
    nb3 = int(nb2/2):N11 = nb2 - 2*nb3
    nb4 = int(nb3/2):N12 = nb3 - 2*nb4
    nb5 = int(nb4/2):N13 = nb4 - 2*nb5
    nb6 = int(nb5/2):N14 = nb5 - 2*nb6
    nb7 = int(nb6/2):N15 = nb6 - 2*nb7
    nb8 = int(nb7/2):N16 = nb7 - 2*nb8
    nb9 = int(nb8/2):N17 = nb8 - 2*nb9
    nb10 = int(nb9/2):N18 = nb9 - 2*nb10
    nb11 = int(nb10/2):N19 = nb10 - 2*nb11
    nb12 = int(nb11/2):N20 = nb11 - 2*nb12      'Bcounter bit 12 MSB
    N21 = 0    '0=ChargePump setting 1, 1=setting 2
    N22 = 0     'reserved
    N23 = 0     'reserved
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
    return
    */
}

void hwdInterface::CreateBaseForDDSarray()
{
  //needed:ddsoutput,ddsclock ; creates: base,sw0thrusw30,w0thruw4
  //the formula for the frequency output of the DDS(AD9850, 9851, or any 32 bit DDS) is:
  //ddsoutput = base*ddsclock/2^32, where "base" is the decimal equivalent of command words
  //to find "base": first, use: fullbase = (ddsoutput*2^32/ddsclock)
  double fullbase=(ddsoutput*pow(2.0,32.0)/ddsclock); //decimal number, including fraction
  //then, round it off to the nearest whole bit
  //(the following has a problem) 11-03-08
  //if ddsoutput is greater than ddsclock/2, the program will error out. I don//t know why but
  //halt and create an error message
  if (ddsoutput >= ddsclock/2)
  {
    util.beep();
    PrintMessage("Error, ddsoutput > .5 ddsclock");
    Halted();
    return;
  }
  base = int(fullbase); //rounded down to whole number
  if (fullbase - base >= .5)
  {
    base = base + 1; //rounded to nearest whole number
  }
  //now, the actual ddsoutput can be determined by: ddsoutput = base*ddsclock/2^32
  //Create Parallel Words //needed:base
  w0= 0; //a "1" here will activate the x4 internal multiplier, but not recommended
  w1= int(base/pow(2.0,24));  //w1 thru w4 converts decimal base code to 4 words, each are 8 bit binary
  w2= int((base-(w1*pow(2.0,24)))/pow(2.0,16));
  w3= int((base-(w1*pow(2.0,24))-(w2*pow(2.0,16)))/pow(2.0,8));
  w4= int(base-(w1*pow(2.0,24))-(w2*pow(2.0,16))-(w3*pow(2.0,8)));

  if (activeConfig->cb == hwdUsbV1)
  {
       usb->int64SW[0] = 0;
       usb->int64SW[1] = int( base );
  }
  else
  {
    //Create Serial Bits//needed:base ; creates serial word bits; sw0 thru sw39
    b0 = int(base/2);
    sw0 = base - 2*b0;  //LSB, Freq-b0.  sw is serial word bit
    b1 = int(b0/2);
    sw1 = b0 - 2*b1;
    b2 = int(b1/2);
    sw2 = b1 - 2*b2;
    b3 = int(b2/2);
    sw3 = b2 - 2*b3;
    b4 = int(b3/2);
    sw4 = b3 - 2*b4;
    b5 = int(b4/2);
    sw5 = b4 - 2*b5;
    b6 = int(b5/2);
    sw6 = b5 - 2*b6;
    b7 = int(b6/2);
    sw7 = b6 - 2*b7;
    b8 = int(b7/2);
    sw8 = b7 - 2*b8;
    b9 = int(b8/2);
    sw9 = b8 - 2*b9;
    b10 = int(b9/2);
    sw10 = b9 - 2*b10;
    b11 = int(b10/2);
    sw11 = b10 - 2*b11;
    b12 = int(b11/2);
    sw12 = b11 - 2*b12;
    b13 = int(b12/2);
    sw13 = b12 - 2*b13;
    b14 = int(b13/2);
    sw14 = b13 - 2*b14;
    b15 = int(b14/2);
    sw15 = b14 - 2*b15;
    b16 = int(b15/2);
    sw16 = b15 - 2*b16;
    b17 = int(b16/2);
    sw17 = b16 - 2*b17;
    b18 = int(b17/2);
    sw18 = b17 - 2*b18;
    b19 = int(b18/2);
    sw19 = b18 - 2*b19;
    b20 = int(b19/2);
    sw20 = b19 - 2*b20;
    b21 = int(b20/2);
    sw21 = b20 - 2*b21;
    b22 = int(b21/2);
    sw22 = b21 - 2*b22;
    b23 = int(b22/2);
    sw23 = b22 - 2*b23;
    b24 = int(b23/2);
    sw24 = b23 - 2*b24;
    b25 = int(b24/2);
    sw25 = b24 - 2*b25;
    b26 = int(b25/2);
    sw26 = b25 - 2*b26;
    b27 = int(b26/2);
    sw27 = b26 - 2*b27;
    b28 = int(b27/2);
    sw28 = b27 - 2*b28;
    b29 = int(b28/2);
    sw29 = b28 - 2*b29;
    b30 = int(b29/2);
    sw30 = b29 - 2*b30;
    b31 = int(b30/2);
    sw31 = b30 - 2*b31;  //MSB, Freq-b31
    sw32 = 0; //x4 multiplier, 1=enable, but not recommended
    sw33 = 0; //control bit
    sw34 = 0; //power down bit
    sw35 = 0; //phase data
    sw36 = 0; //phase data
    sw37 = 0;//phase data
    sw38 = 0; //phase data
    sw39 = 0; //phase data
  }
  return;

}
void hwdInterface::ResetDDS1serUSB()
{
  int pdmcmd = vars->phaarray[vars->thisstep][0];

  //(reset DDS3 to parallel)WCLK up,WCLK up and FQUD up,WCLK up and FQUD down,WCLK down
  QString USBwrbuf = "A10100" + util.ToHex(filtbank + 1);
  QString USBwrbuf2;
  bool result = usb->usbMSADeviceWriteString(USBwrbuf,4);
  if (!result)
  {
    usb->usbMSAInit();
    usb->usbMSADeviceWriteString(USBwrbuf,4);
  }
  USBwrbuf2 = "A30200"+util.ToHex(pdmcmd*64 + 2)+util.ToHex(pdmcmd*64);
  usb->usbMSADeviceWriteString(USBwrbuf2,5);
  USBwrbuf = "A10300"+util.ToHex(filtbank)+util.ToHex(filtbank + 1)+util.ToHex(filtbank);
  usb->usbMSADeviceWriteString(USBwrbuf,6);
  usb->usbMSADeviceWriteString(USBwrbuf2,5);
  //(end involk serial mode DDS3)
  //(flush and command DDS3)D7,WCLK up,WCLK down,(repeat39more),FQUD up,FQUD down
  //present data to buffer,latch buffer,disable buffer,present data+clk to buffer,latch buffer,disable buffer
  USBwrbuf = "A12801";
  QString USBwrbuf3 = util.ToHex( filtbank );
  for (int thisLoop = 0; thisLoop <= 39; thisLoop++)
  {
    USBwrbuf = USBwrbuf + USBwrbuf3;
  }
  usb->usbMSADeviceWriteString(USBwrbuf,43);
  usb->usbMSADeviceWriteString(USBwrbuf2,5);
  //(end flush command DDS3)

  return;
}

void hwdInterface::ResetDDS3serUSB()
{
  //reset serial DDS3 without disturbing Filter Bank or PDM. usb v1.0
  //must have DDS (AD9850/9851) hard wired. pin2=D2=0, pin3=D1=1,pin4=D0=1, D3-D7 are don't care.
  //this will reset DDS into parallel, involk serial mode, then command to 0 Hz.
  int pdmcmd = vars->phaarray[vars->thisstep][0];

  //(reset DDS3 to parallel)WCLK up,WCLK up and FQUD up,WCLK up and FQUD down,WCLK down
  QString USBwrbuf = "A10100"+util.ToHex(filtbank + 1);
  usb->usbMSADeviceWriteString(USBwrbuf, 4);
  QString USBwrbuf2 = "A30200"+util.ToHex(pdmcmd*64 + 8)+util.ToHex(pdmcmd*64);
  usb->usbMSADeviceWriteString(USBwrbuf2, 5);
  USBwrbuf = "A10300"+util.ToHex(filtbank)+util.ToHex(filtbank + 1)+util.ToHex(filtbank);
  usb->usbMSADeviceWriteString(USBwrbuf, 6);
  usb->usbMSADeviceWriteString(USBwrbuf2, 5);
  //(end involk serial mode DDS3)
  //(flush and command DDS3)D7,WCLK up,WCLK down,(repeat39more),FQUD up,FQUD down
  //present data to buffer,latch buffer,disable buffer,present data+clk to buffer,latch buffer,disable buffer
  USBwrbuf = "A12801";
  QString USBwrbuf3 = util.ToHex( filtbank );
  //QString temp
  for (int thisloop = 0; thisloop <= 39; thisloop++)
  {
     USBwrbuf = USBwrbuf + USBwrbuf3;
  }
  usb->usbMSADeviceWriteString(USBwrbuf, 43);
  usb->usbMSADeviceWriteString(USBwrbuf2, 5);
  //(end flush command DDS3)
  return; //to //(InitializeDDS 3)
}
void hwdInterface::CommandDDS1()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CommandDDS1]//ver111-36b. ver113-4a
    //this will recalculate DDS1, using the values in the Command DDS 1 Box, and "with DDS Clock at" Box.
    //it will insert the new DDS 1 frequency into the command arrays for all steps, leaving others alone
    //it will initiate a re-command at thisstep (where the sweep was halted)
      //if Original Control Board is used, only the DDS 1 is re-commanded. ver113-4a
      //if SLIM Control Board is used, all 4 modules will be re-commanded. ver113-4a
    //using One Step or Continue will retain the new DDS1 frequency.
    //PLO1 will be non-functional until [Restart] button is clicked. PLL1 will break lock and "slam" to extreme.
    //[Restart] will reset arrays and begin sweeping at step 0. Special Tests Window will not be updated.
    //Signal Generator or Tracking Generator output will not be effected.
    //caution, do not enter a frequency that is higher than 1/2 the masterclock frequency (ddsclock)
    print #special.dds1out, "!contents? dds1out$";   //grab contents of Command DDS 1 Box
    ddsoutput = val(dds1out$) //intended output frequency of DDS 1
    print #special.masclkf, "!contents? msclk$";   //grab contents of "with DDS Clock at" box
    msclk = val(msclk$) //if "with DDS Clock at" box was not changed, this is the real MasterClock frequency
    ddsclock = msclk
    //caution: if ddsoutput >= to .5 ddsclock, the program will error out
    gosub [CreateBaseForDDSarray]//needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
    remember = thisstep //remember where we were when entering this subroutine
    for thisstep = 0 to steps //ver112-2a
    gosub [FillDDS1array]//need thisstep,sw0-sw39,w0-w4,base,ddsclock
    next thisstep //ver112-2a
    thisstep = remember //ver112-2a
    gosub [CreateCmdAllArray] //ver112-2a
    if cb = 0 then gosub [CommandDDS1OrigCB]//will command DDS 1, only
//delver113-4a    if cb = 2 then gosub [CommandDDS1SlimCB]//will command DDS 1, only
    if cb = 2
{
  CommandAllSlims(vars->thisstep, filtbank, vars->phaarray[vars->thisstep][0]*64)]);  //will command all 4 modules. ver113-4a
  lastpdmstate=phaarray(thisstep,0);
  }
    if cb = 3 then gosub [CommandAllSlimsUSB]//will command all 4 modules.
    wait

*/
}

void hwdInterface::CommandDDS3()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
 /*
//[CommandDDS3]//ver111-38a
    //this will recalculate DDS3, using the values in the Command DDS 3 Box, and "with DDS Clock at" Box.
    //it will insert the new DDS 3 frequency into the command arrays for all steps, leaving others alone
    //it will initiate a re-command at thisstep (where the sweep was halted)
      //only the DDS 3 is re-commanded
    //using One Step or Continue will retain the new DDS3 frequency.
    //PLO3 will be non-functional until [Restart] button is clicked. PLL3 will break lock and "slam" to extreme.
    //[Restart] will reset arrays and begin sweeping at step 0. Special Tests Window will not be updated.
    //Signal Generator or Tracking Generator output will be non functional.
    //Spectrum Analyzer function is not effected
    //caution, do not enter a frequency that is higher than 1/2 the masterclock frequency (ddsclock)
    print #special.dds3out, "!contents? dds3out$";   //grab contents of Command DDS 3 Box
    ddsoutput = val(dds3out$) //intended output frequency of DDS 3
    print #special.masclkf, "!contents? msclk$";   //grab contents of "with DDS Clock at" box
    msclk = val(msclk$) //if "with DDS Clock at" box was not changed, this is the real MasterClock frequency
    ddsclock = msclk
    //caution: if ddsoutput >= to .5 ddsclock, the program will error out
    gosub [CreateBaseForDDSarray]//needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
    remember = thisstep //remember where we were when entering this subroutine
    for thisstep = 0 to steps
    gosub [FillDDS3array]//need thisstep,sw0-sw39,w0-w4,base,ddsclock
    next thisstep
    thisstep = remember
    gosub [CreateCmdAllArray]
    if cb = 0 then gosub [CommandDDS3OrigCB]//will command DDS 3, only
//delver113-4a    if cb = 2 then gosub [CommandDDS3SlimCB]//will command DDS 3, only
    if cb = 2
{CommandAllSlims(vars->thisstep, filtbank, vars->phaarray[vars->thisstep][0]*64)]); //will command all 4 modules. ver113-4a
lastpdmstate=phaarray(thisstep,0);
}
    if cb = 3 then gosub [CommandAllSlimsUSB]//will command all 4 modules.
    wait
*/
}

void hwdInterface::DDS3Track()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[DDS3Track]'ver111-39d
    'This uses DDS3 as a Tracking Generator, but is limited to 0 to 32 MHz, when MasterClock is 64 MHz
    'DDS3 spare output is rich in harmonics and aliases.
    'Tracks the values in Working Window, Center Frequency and Sweep Width (already in the command arrays)
    'The Spectrum Analyzer function is not effected.
    'PLO3, Normal Tracking Generator, and Phase portion of VNA will be non-functional
    'Operation:
    'In Working Window, enter Center Frequency to be within 0 to 32 (MHz), or less than 1/2 the MasterClock
    'In Working Window, enter Sweep Width (in MHz). But, do not allow sweep to go below 0 or abov 1/2 MasterClock
    'Click [Restart], then halt.
    'In Special Tests Window, click [DDS 3 Track].  DDS 3 will, immediately, re-command to new frequency.
    'Click [Continue]. Sweep will resume, but with DDS 3 tracking the Spectrum Analalyzer
    '[One Step] and [Continue] and halting operates normally until [Restart] button is pressed.
    '[Restart] will reset arrays, and leave the DDS 3 Track Mode. ie, normal sweeping.
    ddsclock = masterclock
    remember = thisstep
    for thisstep = 0 to steps
    ddsoutput = datatable(thisstep,1)
    'caution: if ddsoutput >= to .5 ddsclock, the program will error out
    gosub [CreateBaseForDDSarray]'needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
    gosub [FillDDS3array]'need thisstep,sw0-sw39,w0-w4,base,ddsclock
    next thisstep
    thisstep = remember
    gosub [CreateCmdAllArray]
    if cb = 0 then gosub [CommandDDS3OrigCB]'will command DDS 3, only
    if cb = 2
{
CommandAllSlims(vars->thisstep, filtbank, vars->phaarray[vars->thisstep][0]*64)]);  //will command all 4 modules. ver113-4a
lastpdmstate=phaarray(thisstep,0);
}
    if cb = 3 then gosub [CommandAllSlimsUSB]'will command all 4 modules. 'USB:01-08-2010
    wait
*/
}

void hwdInterface::DDS1Sweep()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
    //This forces the DDS 1 to the values in Working Window: Center Frequency and Sweep Width (already in the command arrays)
    //DDS1 spare output is rich in harmonics and aliases.
    //PLO1, and thus, the Spectrum Analyzer will be non-functional in this mode.
    //Signal Generator or Tracking Generator output will not be affected.
    //Operation:
    //In Working Window, enter Center Frequency to be within 0 to 32 (MHz), or less than 1/2 the MasterClock
    //In Working Window, enter Sweep Width (in MHz). But, do not allow sweep to go below 0 or abov 1/2 MasterClock
    //Click [Restart], then halt.
    //In Special Tests Window, click [DDS 1 Sweep].  DDS 1 will, immediately, re-command to new frequency.
    //Click [Continue]. Sweep will resume, but with DDS 1 sweeping.
    //[One Step] and [Continue] and halting operates normally until [Restart] button is pressed.
    //[Restart] will reset arrays, and will leave the DDS 1 Sweep Mode. ie, normal sweeping.
    ddsclock = activeConfig->masterclock;
    //int remember = thisstep
    for (int thisstep = 0; thisstep < vars->steps; thisstep++)
    {
      ddsoutput = vars->datatable[thisstep][1];
      //caution: if ddsoutput >= to .5 ddsclock, the program will error out
      CreateBaseForDDSarray();  //needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
      FillDDS1array(thisstep);//need thisstep,sw0-sw39,w0-w4,base,ddsclock
    }
    //thisstep = remember
    CreateCmdAllArray();
    if (activeConfig->cb == hwdOld)
    {
//      CommandDDS1OrigCB();  //will command DDS 1, only
    }
    else if (activeConfig->cb == hwdSlim)
    {
//      CommandAllSlims(vars->thisstep, filtbank, vars->phaarray[vars->thisstep][0]*64)]);  //will command all 4 modules. ver113-4a
//      lastpdmstate=phaarray(thisstep,0);
    }
    else if (activeConfig->cb == hwdUsbV1)
    {
      CommandAllSlimsUSB();//will command all 4 modules.
    }
    */
}

void hwdInterface::ChangePDM()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ChangePDM]'ver112-2a
    'enters from Special Tests Window Button
    setpdm = setpdm + 1
    if setpdm > 2 then setpdm = 0
    if setpdm = 0 then print #special.pdminv, "PDM is Auto"
    if setpdm = 1 then print #special.pdminv, "PDM in Normal":gosub [PdmNorm]
    if setpdm = 2 then print #special.pdminv, "PDM in Inverted":gosub [PdmInv]
    wait
*/
}

void hwdInterface::PdmNorm()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ChangePDM]'ver112-2a
    'enters from Special Tests Window Button
    setpdm = setpdm + 1
    if setpdm > 2 then setpdm = 0
    if setpdm = 0 then print #special.pdminv, "PDM is Auto"
    if setpdm = 1 then print #special.pdminv, "PDM in Normal":gosub [PdmNorm]
    if setpdm = 2 then print #special.pdminv, "PDM in Inverted":gosub [PdmInv]
    wait
[PdmNorm]'this commands the pdm to Normal, for all steps
    rememberthisstep = thisstep
    for thisstep = 0 to steps
    phaarray(thisstep,0) = 0
    next thisstep
    thisstep = rememberthisstep
    gosub [CommandPDMonly]
    return
*/
}

void hwdInterface::PdmInv()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[PdmInv]'this commands the pdm to Invert, for all steps
    rememberthisstep = thisstep
    for thisstep = 0 to steps
    phaarray(thisstep,0) = 1
    next thisstep
    thisstep = rememberthisstep
    gosub [CommandPDMonly]
    return
    */
}

void hwdInterface::SyncTestPDM()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[SyncTestPDM] 'ver112-2b
    'enters from Special Tests Window Button, only if in VNA Mode
    'this will set up defaults and begin sweeping to measure phase steps
    'when CF=0 and SW=0, the PDM will measure "rolling" phase of two different frequencies,
    'although the difference is less than 1 Hz.PDM is fixed at Norm(0)
    syncsweep = 1 'ver112-2b
    convdatapwr = 1 'ver112-2b
            'SEWgraph updated the following to deal with changes in text boxes
    call SetCenterSpanFreq 0,0  'SEWgraph zero width, zero center
    wate=3  'SEWgraph 3ms wait time ver114-4d
    if primaryAxisNum=1 then 'ver115-3b
        call SetY2Range 0,360 'Phase range 0 to 360 ver114-4d
        call SetY1Range -5, 5 'Mag range -5 to 5 ver114-4d
    else
        call SetY1Range 0,360 'Phase range 0 to 360 ver114-4d
        call SetY2Range -5, 5 'Mag range -5 to 5 ver114-4d
    end if

    call gSetXIsLinear 1    'SEWgraph Be sure sweep is linear
    call gCalcGraphParams   'SEWgraph Calculate scaling, etc. with new parameters
    setpdm = 1 : print #special.pdminv, "PDM in Normal":gosub [PdmNorm]
    haltsweep=0 'So Restart actually restarts 'ver114-6e
    goto [Restart]
    wait
*/
}
void hwdInterface::SyncSweep()
{
  //comes here at end of sweep if syncsweep = 1
  //it will not continue sweeping until the phase data is between 80 and 90% of maxpdmout
  //hopefully, it will "trigger" a sweep at 81%
  while(1)
  {
    ReadPhase();
    //scan //this is a fail safe. Click any button to get out of this loop. ver113-6e
    //QCoreApplication::processEvents();
    if (phadata > .8 * activeConfig->maxpdmout && phadata < .9 * activeConfig->maxpdmout)
      return;
  }
}

void hwdInterface::ConvertDataToPower()
{
  //this routine is a traffic director when the blue Magnitude trace is used for other data.
  //enters from [CalcMagpowerPixel] if convdatapwr = 1
  if (syncsweep == 1)
  {
    ConvertSync(vars->thisstep);
    return;
  }
  ConvertPDMlin();
}

void hwdInterface::ConvertSync(int step)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;

  //this will take the phase difference of the previous step//s and this step//s phase
  //and convert it to power, for display
  //enters from [ConvertDataToPower] if syncsweep = 1
  if (step == 0)
  {
    return; //the last step in the sweep - step 0 is bogus data
  }
  //grab raw phase bits from previous sweep and create deltabits
  float deltabits = vars->phaarray[step-vars->sweepDir][3] - vars->phaarray[step][3];
  //convert deltabits to delta phase
  float deltaphase = 360 * deltabits/activeConfig->maxpdmout;
  power = deltaphase;
  return;

}

void hwdInterface::PresetVNAlin()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[PresetVNAlin] 'ver112-2b
    print #special.prevnalin, "Test Transmission Linearity"  'change the button name
    if vnalintest = 1 then goto [VNAlinTest]
    vnalintest = 1
    for i=0 to steps : lineCalArray(i,0)=0 : lineCalArray(i,1)=0 : lineCalArray(i,2)=0 : next i 'ver116-1b
    call SignalNoCalInstalled   'ver116-4b
        'SEWgraph updated the following to deal with changes in text boxes
    call SetCenterSpanFreq 500,1000 'SEWgraph; 500 MHz center, 1000 MHz span
    wate=22  'ver114-4d; 22 ms wait time
    'afix the PDM to "Normal" (0).  It will remain fixed as long as the Special Tests Window is open.
    setpdm = 1 : print #special.pdminv, "PDM in Normal":gosub [PdmNorm]
    'set up magnitude limits, prolly +/- 5 degrees(db)
    if primaryAxisNum=1 then 'ver115-3b
        call SetY1Range -5, 5 'This is actually degrees, not power
        call SetY2Range -180, 180   'phase
    else
        call SetY2Range -5, 5 'This is actually degrees, not power
        call SetY1Range -180, 180   'phase
    end if

    call gSetXIsLinear 1    'SEWgraph be sure sweep is linear
    call gCalcGraphParams   'SEWgraph  Calculate scaling, etc. with restored parameters
    haltsweep=0 'So Restart actually restarts 'ver114-6e
    goto [Restart]
    'the sweep will begin, and display several sawtooths, the number depending on the length
        'of the cable between the TG output and the MSA input.
    'There will be extreme glitches near the center of each sawtooth, indicating the
        'zero degree crossovers.  This is due to the PDM being fixed at "Normal"
*/
}

void hwdInterface::VNAlinTest()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[VNAlinTest]'for testing PDM Linearity 'ver112-2b
    'make sure pdm is fixed at norm or invert before entering. Accomplished by previously clicking [PresetVNAlin] Button.
    'before entering this routine, make sure the sweep is displaying one full segment that
      ' is greater than 360 degrees and less than 720 degrees.
    'before entering this routine, pick a step point to be the zero phase error refpoint. It has:
      'step #, frequency, and phase. It must be within the 0 degree crossover boundries.
      'we use it as a reference zero degrees error. Use the Mouse's left click to select the step.
      'then, click the Special Tests Window [Test VNA Linearity] Button.
    'a very long line, between TG out and MSA in, will reduce the error created by Mixer 4, AM to PM conversion
        convdatapwr = 1 'used in [CalcMagpowerPixel] to skip processing Mag Data
    'determine the "real" slope factor for this band segment, in degrees per MHz

    'retrieve raw phase data bits. Bits0, Bits400 'ver112-2g
        phabits0 = phaarray(0,3) 'ver112-2g
        phabits400 = phaarray(steps,3) 'ver112-2g
    'if phabits0 is less than or equal to phabits400, add 360 degrees(in bits) to phabits0 'ver112-2g
    'maxpdmout is the bit value of 360 degrees, for any AtoD Module
        if phabits0 <= phabits400 then phabits0 = phabits0 + maxpdmout 'ver112-2g
    'add maxpdmout (65535 bits) to phabits0 and take difference 'ver112-2g
    'delta bits = maxpdmout + phabits0 - phabits400 'ver112-2g
    'change to phase: 360 * (delta bits/maxpdmout) 'ver112-2g
        delpha = 360*((maxpdmout + phabits0 - phabits400)/maxpdmout) 'ver112-2g

    'freq at step 0 - freq at last step = delta freq (551.8 - 829 = -277.2)
        delfreq = datatable(0,1) - datatable(steps,1)
        'SEWgraph The following line avoids a crash with zero span
        if delfreq=0 then notice "Sweep must be preset to show 360-720 degrees." :wait 'SEWgraph
    'realslopefactor (in deg per MHz) = delta phase / delta freq = (393.7 / -277.2) = -1.42 d/m
        realslopefactor = delpha/delfreq '(in -degrees/MHz)
        refstep = leftstep
    'the processed phase for refstep is in datatable(refstep,3),always for previous sweep
        refstepphase = datatable(refstep,3)
    'the frequency of refstep is in datatable(refstep,1),always for previous sweep
        refstepfreq = datatable(refstep,1)
    'print the slope factor in the Message Box next time sweep is halted
    test=realslopefactor 'this will be cleared when leaving Spec Test Window. ver112-2g
    haltsweep=0 'So Restart actually restarts 'ver114-6e
    goto [Restart]
*/
}

void hwdInterface::ConvertPDMlin()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ConvertPDMlin] 'ver112-2b
    'enters here from [CalcMagpowerPixel]. We are processing the previous step but it
    'is still called "thisstep"
    'the blue line will show how far off the real slope is from a theoritical slope, in degrees
    'the processed phase for thisstep is in datatable(thisstep,3)
    'the frequency of thisstep is in datatable(thisstep,1)
    'this step will have a theoritical phase equal to:
    ' theoryphase = thisstep freq - refstep freq times realslopefactor + refstep phase
        theoryphase = ((datatable(thisstep,1)- refstepfreq) * realslopefactor) + refstepphase
    'in wide band sweeps, theoryphase can get greater than 360 degrees 'ver112-2g
    'therefore, change theoryphase to decimal number compared to 360 degrees 'ver112-2g
        theoryphase = theoryphase/360   'example theoryphase = 1.324  or -1.226 'ver112-2g
    'cast out whole number and use decimal equivalent 'ver112-2g
        theoryphase = theoryphase - int(theoryphase)    'theoryphase = .324 or -.226 'ver112-2g
    'reconvert back to phase 'ver112-2g
        theoryphase = 360*theoryphase 'ver112-2g
        if theoryphase < -180 then theoryphase = theoryphase + 360
        if theoryphase > 180 then theoryphase = theoryphase - 360
      'at this line, I could make power = theoryphase and display blue over red error
    'the error will be equal to:  phaseerror = thisstep phase - theoryphase
        phaseerror = datatable(thisstep,3) - theoryphase
        if phaseerror < -180 then phaseerror = phaseerror + 360
        if phaseerror > 180 then phaseerror = phaseerror - 360
    'using the blue magpower line as phase error, the processed magpower becomes phaseerror
        power = phaseerror
        return
*/
}
void hwdInterface::Command2325R()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed:rcounter,preselector,control,Jcontrol,port,LEPLL,contclear ; commands LMX2325 rcounter and registers
    if rcounter <3 then beep:errora$ = "2325 Rcounter is < 3":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0 = 1           'address bit, 0 sets the N Buffer, 1 is for R Buffer
    rc1 = int(rcounter/2):N1 = rcounter - 2*rc1 'binary conversion from decimal
    rc2 = int(rc1/2):N2 = rc1 - 2*rc2
    rc3 = int(rc2/2):N3 = rc2 - 2*rc3
    rc4 = int(rc3/2):N4 = rc3 - 2*rc4
    rc5 = int(rc4/2):N5 = rc4 - 2*rc5
    rc6 = int(rc5/2):N6 = rc5 - 2*rc6
    rc7 = int(rc6/2):N7 = rc6 - 2*rc7
    rc8 = int(rc7/2):N8 = rc7 - 2*rc8
    rc9 = int(rc8/2):N9 = rc8 - 2*rc9
    rc10 = int(rc9/2):N10 = rc9 - 2*rc10
    rc11 = int(rc10/2):N11 = rc10 - 2*rc11
    rc12 = int(rc11/2):N12 = rc11 - 2*rc12
    rc13 = int(rc12/2):N13 = rc12 - 2*rc13
    rc14 = int(rc13/2):N14 = rc13 - 2*rc14
    N15 = 1: if preselector = 64 then N15 = 0   'sets preselector divide ratio, 1=32, 0=64
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
    return
    */
}

void hwdInterface::Command2326R()
{
  if (activeConfig->cb == hwdUsbV1)
  {
    usb->int64N[1] = 0x00000003 | (phasepolarity << 7);
    usb->int64N[0] = 0;
  }

/*
  //needed:rcounter,phasepolarity,control,Jcontrol,port,LEPLL,contclear ; commands LMX2326 rcounter and registers
  //[Create2326InitBuffer]//need phasepolarity
  //ver116-4o deleted "if" block, per Lrev1
  n.N20=0;     //Test, use 0
  n.N19=0;     //1=Power Down Mode, use 0
  n.N18=0;     //Test, use 0
  n.N17=0;     //Test, use 0
  n.N16=0;     //Test, use 0
  n.N15=0;     //Fastlock Time out value, use 0
  n.N14=0;     //Fastlock Time out value, use 0
  n.N13=0;     //Fastlock Time out value, use 0
  n.N12=0;     //Fastlock Time out value, use 0
  n.N11=0;     //1=Time out enable, use 0
  n.N10=0;     //Fastlock control, use 0
  n.N9=0;    //1=Fastlock enable, use 0
  n.N8=0;    //1=Tristate the phase det output, use 0
  n.N7 = phasepolarity;     //Phase det polarity, 1=pos  0=neg
  n.N6=0;        //FoLD control(pin14 output), 0= tristate, 1= R Divider out
  n.N5=0;        //2= N Divider out, 3= Serial Data Output, 4= Digital Lock Detect
  n.N4=0;        //5= Open drain lock detect, 6= High output, 7= Low output
  n.N3=0;        //1= Power Down, use 0
  n.N2=0;        //1= Counter Reset Enable, allows reset of R,N counters,use 0
  n.N1=1;        //F1 address bit 1, must be 1
  n.N0=1;        //F1 address bit 0, must be 1
  if (activeConfig->cb == hwdUsbV1)
  {
    //Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
    //            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
    //            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 //ver116-4o per Lrev1
  }
  if (activeConfig->cb == hwdUsbV1)
  {
    //    Int64N.msLong.struct = 0; //ver116-4o per Lrev1
  }
  */
  //[Command2326InitBuffer]//need Jcontrol,LEPLL,contclear
  CommandPLL(vars->thisstep);//needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
  //[Create2326Rbuffer]//need rcounter
  if (rcounter <3)
  {
    util.beep();
    errora ="2326 R counter <3";
    return;
  }
  if (rcounter >16383)
  {
    util.beep();
    errora="2326 R counter >16383";
    return;
  }
  //ver116-4o deleted "if" block, per Lrev1
  n.N0 = 0;                   //R address bit 0, must be 0
  n.N1 = 0;                   //R address vit 1, must be 0
  ra0 = (int)(rcounter/2);
  n.N2 = rcounter- 2*ra0;    //LSB
  ra1 = (int)(ra0/2);
  n.N3 = ra0- 2*ra1;
  ra2 = (int)(ra1/2);
  n.N4 = ra1- 2*ra2;
  ra3 = (int)(ra2/2);
  n.N5 = ra2- 2*ra3;
  ra4 = (int)(ra3/2);
  n.N6 = ra3- 2*ra4;
  ra5 = (int)(ra4/2);
  n.N7 = ra4- 2*ra5;
  ra6 = (int)(ra5/2);
  n.N8 = ra5- 2*ra6;
  ra7 = (int)(ra6/2);
  n.N9 = ra6- 2*ra7;
  ra8 = (int)(ra7/2);
  n.N10 = ra7- 2*ra8;
  ra9 = (int)(ra8/2);
  n.N11 = ra8- 2*ra9;
  ra10 = (int)(ra9/2);
  n.N12 = ra9- 2*ra10;
  ra11 = (int)(ra10/2);
  n.N13 = ra10- 2*ra11;
  ra12 = (int)(ra11/2);
  n.N14 = ra11- 2*ra12;
  ra13 = (int)(ra12/2);
  n.N15 = ra12- 2*ra13;  //MSB
  n.N16 = 0;     //Test Bit
  n.N17 = 0;     //Test Bit
  n.N18 = 0;     //Test Bit
  n.N19 = 0;     //Test Bit
  n.N20 = 0;     //Lock Detector Mode, 0=3 refcycles, 1=5 cycles

  if (activeConfig->cb == hwdUsbV1)
  {
    usb->int64N[1] = rcounter << 2;
    usb->int64N[0] = 0;
  }
  CommandPLL(vars->thisstep);//needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
}

void hwdInterface::Command2350R()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed: rcounter,phasepolarity,control,Jcontrol,port,LEPLL,contclear,fractional ; commands LMX2350 rcounter
    '[CreateIFRbuffer2350]'needed:nothing,since IF section is turned off
    'ver116-4o deleted "if" block, per Lrev1
    N23=0     'osc. 0=separate
    N22=1     'Modulo F, 1=16 0=15
    N21=1     'ifr21-ifr19 is FO/LD, 3 Bits (0-7), MSB, 0=IF/RF alogLockDet(open drain)
    N20=1     '1=IF digLockDet, 2=RF digLockDet, 3=IF/RF digLockDet
    N19=1     '4=IF Rcntr, 5=IF Ncntr, 6=RF Rcntr, 7=RF Ncntr, LSB
    N18=0     'IF charge pump, 0=100ua  1=800ua
    N17=1     'IF polarity 1=positive phase action
    N16=0     'IFR counter IF section 15 Bits, MSB 14
    N15=0     'IFRcounter Bit 13
    N14=0     'IFRcounter Bit 12
    N13=0     'IFRcounter Bit 11
    N12=1     'IFRcounter Bit 10
    N11=1     'IFRcounter Bit 9
    N10=1     'IFRcounter Bit 8
    N9=1      'IFRcounter Bit 7
    N8=0      'IFRcounter Bit 6
    N7=1      'IFRcounter Bit 5
    N6=1      'IFRcounter Bit 4
    N5=0      'IFRcounter Bit 3
    N4=0      'IFRcounter Bit 2
    N3=0      'IFRcounter Bit 1
    N2=0      'IFR counter, IF section 15 Bits, LSB 0
    N1=0      '2350 IF_R register, 2 bits, must be 0
    N0=0      '2350 IF_R register, 2 bits, must be 0
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
  '[CommandIFRbuffer2350]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
  '[CreateIFNbuffer2350]'needed:nothing,since IF section is turned off(N22=1)
    'ver116-4o deleted "if" block, per Lrev1
    N23=0     'IF counter reset, 0=normal operation
    N22=1     'Power down mode for IF section, 1=powered down, 0=powered up
    N21=0     'PWN Mode,  0=async  1=syncro
    N20=0     'Fastlock, 0=CMOS outputs enabled 1= fastlock mode
    N19=0     'test bit, leave at 0
    N18=1     'OUT 0,  1
    N17=0     'OUT 1,  0
    N16=0     'IF N Bcounter 12 Bits MSB bit 11
    N15=0     'IF N Bcounter, bit 10, '512 = 0010 0000 0000
    N14=1     'IF N Bcounter, bit 9
    N13=0     'IF N Bcounter, bit 8
    N12=0     'IF N Bcounter, bit 7
    N11=0     'IF N Bcounter, bit 6
    N10=0     'IF N Bcounter, bit 5
    N9=0      'IF N Bcounter, bit 4
    N8=0      'IF N Bcounter, bit 3
    N7=0      'IF N Bcounter, bit 2
    N6=0      'IF N Bcounter, bit 1
    N5=0      'IF N Bcounter, 12 Bits, LSB bit 0
    N4=0      'bit 2, IF N Acounter 3 Bits MSB
    N3=0      'bit 1, 0 = 000 thru 7 = 111
    N2=0      'bit 0, IF N Acounter 3 Bits LSB
    N1=0      '2350 IF_N register, 2 bits, must be 0
    N0=1      '2350 IF_N register, 2 bits, must be 1
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
  '[CommandIFNbuffer2350]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
  '[CreateRFRbuffer2350]needed:rcounter,phasepolarity,fractional
    if rcounter < 3 then beep:errora$="2350 Rcounter <3":return 'with errora$ ver111-37c
    if rcounter > 32767 then beep:errora$="2350 Rcounter >32767":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0=0      '2350 RF_R register, 2 bits, must be 0
    N1=1      '2350 RF_R register, 2 bits, must be 1
    rfra2 = int(rcounter/2):N2 = rcounter- 2*rfra2
    rfra3 = int(rfra2/2):N3 = rfra2- 2*rfra3
    rfra4 = int(rfra3/2):N4 = rfra3- 2*rfra4
    rfra5 = int(rfra4/2):N5 = rfra4- 2*rfra5
    rfra6 = int(rfra5/2):N6 = rfra5- 2*rfra6
    rfra7 = int(rfra6/2):N7 = rfra6- 2*rfra7
    rfra8 = int(rfra7/2):N8 = rfra7- 2*rfra8
    rfra9 = int(rfra8/2):N9 = rfra8- 2*rfra9
    rfra10 = int(rfra9/2):N10 = rfra9- 2*rfra10
    rfra11 = int(rfra10/2):N11 = rfra10- 2*rfra11
    rfra12 = int(rfra11/2):N12 = rfra11- 2*rfra12
    rfra13 = int(rfra12/2):N13 = rfra12- 2*rfra13
    rfra14 = int(rfra13/2):N14 = rfra13- 2*rfra14
    rfra15 = int(rfra14/2):N15 = rfra14- 2*rfra15
    rfra16 = int(rfra15/2):N16 = rfra15- 2*rfra16
    N17 = phasepolarity     'RF phase polarity,  1=positive action, 0=inverted action
    N18=1     'LSB of RF charge pump sel, 4 Bits, 16 levels, 100ua/level
    N19=1     'total current = (100ua * bit value)+100ua
    N20=1     '100ua to 1600ua: ie, 800ua = 0111, 1600ua = 1111
    N21=1     'MSB of RF charge pump sel, 4 Bits 100ua/bit
    N22=0     'V2 enable voltage doubler =1   0=norm Vcc
    N23 = fractional   'DLL mode, delay line cal, 0=slow  1=fast,fractional mode
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 ''ver116-4o per Lrev1
    '[CommandRFRbuffer2350]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
    return
    */
}

void hwdInterface::Command2353R()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed: rcounter,phasepolarity,control,Jcontrol,port,LEPLL,contclear,fractional ; commands LMX2353 rcounter
  '[Create2353F1Buffer]'globals reqd, none
    'ver116-4o deleted "if" block, per Lrev1
    N23=0
    N22=1     'divider, 1=16 0=15
    N21=0     'FO/LD output selection, 3 Bits 0-7 MSB
    N20=0     '0=alog lock det, 2=dig lock det
    N19=0     '6=Ndivider output, 7=Rdivider output
    N18=0:N17=0:N16=0:N15=0:N14=0
    N13=0:N12=0:N11=0:N10=0:N9=0
    N8=0:N7=0:N6=0:N5=0:N4=0
    N3=0:N2=0
    N1=0        'F1 address bit 1
    N0=0        'F1 address bit 0
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev11
  '[Command2353F1Buffer]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
  '[Create2353F2Buffer]'globals reqd: none
    'ver116-4o deleted "if" block, per Lrev1
    N23=0:N22=0
    N21=0     'Power Down Mode,  0=async  1=syncro
    N20=0     'Fastlock, 0=CMOS outputs enabled 1= fastlock mode
    N19=0     'test bit, leave at 0
    N18=0     'OUT 1,  0
    N17=0     'OUT 0,  0
    N16=0:N15=0:N14=0:N13=0
    N12=0:N11=0:N10=0:N9=0
    N8=0:N7=0:N6=0:N5=0
    N4=0:N3=0:N2=0
    N1=0        'F2 address bit 1
    N0=1        'F2 address bit 0
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
  '[Command2353F2Buffer]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
  '[Create2353Rbuffer]'needed:rcounter,phasepolarity,fractional
    if rcounter <3 then beep:errora$ = "2353 Rcounter is < 3":return 'with errora$ ver111-37c
    if rcounter >32767 then beep:errora$ = "2353 Rcounter is > 32767":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0 = 0                   'R address bit 0
    N1 = 1                   'R address bit 1
    ra0 = int(rcounter/2):N2 = rcounter- 2*ra0    'LSB R buffer
    ra1 = int(ra0/2):N3 = ra0- 2*ra1:ra2 = int(ra1/2):N4 = ra1- 2*ra2
    ra3 = int(ra2/2):N5 = ra2- 2*ra3:ra4 = int(ra3/2):N6 = ra3- 2*ra4
    ra5 = int(ra4/2):N7 = ra4- 2*ra5:ra6 = int(ra5/2):N8 = ra5- 2*ra6
    ra7 = int(ra6/2):N9 = ra6- 2*ra7:ra8 = int(ra7/2):N10 = ra7- 2*ra8
    ra9 = int(ra8/2):N11 = ra8- 2*ra9:ra10 = int(ra9/2):N12 = ra9- 2*ra10
    ra11 = int(ra10/2):N13 = ra10- 2*ra11:ra12 = int(ra11/2):N14 = ra11- 2*ra12
    ra13 = int(ra12/2):N15 = ra12- 2*ra13:ra14 = int(ra13/2):N16 = ra13- 2*ra14    'MSB R buffer
    N17 = phasepolarity     'phase detector polarity 1=normal,0=reverse for opamp
    N18 = 1   'LSB of Charge pump control, 100ua x1 +100ua
    N19 = 1          'Charge pump control, 100ua x2 +100ua
    N20 = 1          'Charge pump control, 100ua x4 +100ua
    N21 = 1   'MSB of Charge pump control, 100ua x8 +100ua
    N22 = 0   'Charge Pump Voltage Doubler Enabled when 1
    N23 = fractional 'Delay Line Loop Cal mode, set to 1 for fractional N
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
  '[Cmd2353Rbuffer]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
    return
    */
}

void hwdInterface::Command4112R()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'needed: rcounter,preselector,phasepolarity,control,Jcontrol,port,LEPLL,contclear ; commands AD4112 rcounter
  '[Create4112InitBuffer]'needed:preselector,phasepolarity
    'ver116-4o deleted "if" block, per Lrev1
    N23=1     'N23,22 prescaler: 0=8, 1=16, 2=32, 3=64
    N22=0     'preselector defaulted to 32
    if preselector =8 then N23=0:N22=0
    if preselector =16 then N23=0:N22=1
    if preselector =64 then N23=1:N22=1
    N21=0     'Power Down Mode, 0=async, 1=sync  use 0
    N20=0     'N20,19,18 Phase Current for Set 2            '12-3-10
    N19=0     'current= min current + min current*bit value '12-3-10
    N18=1     'use bit value of 1 and 4.7 Kohm for 1.25 ma  '12-3-10
    N17=0     'N17,16,15 Phase Current for Set 1            '12-3-10
    N16=0     'current= min current + min current*bit value '12-3-10
    N15=1     'use bit value of 1 and 4.7 Kohm for 1.25 ma  '12-3-10
    N14=0     'N15,14,13,12 Fastlock Timer cycles
    N13=0     '4 Bits, Cycles= 3 cycles + 4*bit value
    N12=0     'Fastlock Time out value, use 0
    N11=0     'use 4 bit value = 0
    N10=0     '0=Fastlock Mode 1 (command), 1=Mode 2 (automatic)
    N9=0     '1=Fastlock enabled, 0 =Fastlock Disabled
    N8=0      '1=Tristate the phase det output, use 0
    N7 = phasepolarity     'Phase det polarity, 1=pos  0=neg
    N6=0      'FoLD control(pin14 output), 0= tristate, 1= Digital Lock Detect
    N5=0      '2= N Divider out, 3= High output, 4= R Divider output
    N4=0      '5= Open drain lock detect, 6= Serial Data output, 7= Low output
    N3=0      'PD1, Power Down, 0=normal operation, 1=select power down mode
    N2=0      '1= Counter Reset Enable, allows reset of R,N counters,use 0
    N1=1      'F1 address bit 1, must be 1
    N0=1      'F1 address bit 0, must be 1
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
  '[Command4112InitBuffer]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
  '[Create4112Rbuffer]'needs:rcounter
    if rcounter >16383 then beep:errora$="4112 R counter >16383":return 'with errora$ ver111-37c
    'ver116-4o deleted "if" block, per Lrev1
    N0 = 0                   'R address bit 0, must be 0
    N1 = 0                   'R address vit 1, must be 0
    ra0 = int(rcounter/2):N2 = rcounter- 2*ra0    'LSB R0
    ra1 = int(ra0/2):N3 = ra0- 2*ra1
    ra2 = int(ra1/2):N4 = ra1- 2*ra2
    ra3 = int(ra2/2):N5 = ra2- 2*ra3
    ra4 = int(ra3/2):N6 = ra3- 2*ra4
    ra5 = int(ra4/2):N7 = ra4- 2*ra5
    ra6 = int(ra5/2):N8 = ra5- 2*ra6
    ra7 = int(ra6/2):N9 = ra6- 2*ra7
    ra8 = int(ra7/2):N10 = ra7- 2*ra8
    ra9 = int(ra8/2):N11 = ra8- 2*ra9
    ra10 = int(ra9/2):N12 = ra9- 2*ra10
    ra11 = int(ra10/2):N13 = ra10- 2*ra11
    ra12 = int(ra11/2):N14 = ra11- 2*ra12
    ra13 = int(ra12/2):N15 = ra12- 2*ra13  'MSB
    N16 = 0     'N17,16  Antibacklash width
    N17 = 0     '0=3ns, 1=1.5ns, 2=6ns, 3=3ns
    N18 = 0     'Test Bit, use 0
    N19 = 0     'Test Bit, use 0
    N20 = 0     'Lock Detector Mode, 0=3 refcycles, 1=5 cycles
    N21 = 0     'resyncronization enable 0=normal, 1=resync prescaler
    N22 = 1     '0=resync with nondelayed rf input, 1=resync with delayed rf
    N23 = 0   'reserved, use 0
    if cb = 3 then Int64N.lsLong.struct = 2^23*N23+ 2^22*N22+ 2^21*N21+ 2^20*N20+ 2^19*N19+ 2^18*N18+ 2^17*N17+ 2^16*N16+ 2^15*N15+_
            2^14*N14+ 2^13*N13+ 2^12*N12+ 2^11*N11+ 2^10*N10+ 2^9*N9+ 2^8*N8+_
            2^7*N7+ 2^6*N6+ 2^5*N5+ 2^4*N4+ 2^3*N3+ 2^2*N2+ 2^1*N1+ 2^0*N0 'ver116-4o per Lrev1
    if cb = 3 then Int64N.msLong.struct = 0 'ver116-4o per Lrev1
  '[Command4112Rbuffer]
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111
    return
    */

}


void hwdInterface::InitializeHardware()
{

  //These hardware initializations are performed on startup and usually repeated on Restart. The reason
  //they are repeated on Restart is to fix any hardware glitches that might occur. Whenever it is known
  //that a hardware change is made, such as filter selection changing, it is best to take action immediately,
  //and not rely on the Restart process. In some cases, Restart skips these initializations for speed.
  if (vars->suppressHardware==0 && activeConfig->cb != hwdUsbV1)
  {
    lpt.output(port, 0);    //begin with all data lines low
    if (activeConfig->cb == hwdSlim)
    {
      lpt.output(control, INITSELT); //latch "0" into SLIM Control Board Buffers 1 and 2
      lpt.output(control, AUTO); //latch "0" into SLIM Control Board Buffers 3
      //We don't clear SLIM Buffer 4, because it controls among other things the latched switches
      //It was initialized near the beginning to make the PS line high.
    }
    lpt.output(control, contclear);      //begin with all control lines low
  }
  if (activeConfig->cb == hwdUsbV1 && vars->bUseUsb != 0)
  {
    QString USBwrbuf = "A5010000"; // reset all lines low
    usb->usbMSADeviceWriteString(USBwrbuf,4);
  }
  //the following are meaningless values to guarantee first time commanding. Used in subroutine, [DetermineModule]
  vars->lastdds1output = activeConfig->appxdds1;
  vars->lastdds3output = activeConfig->appxdds3;
  vars->lastpdmstate = 2;
  lastncounter1 = 0;
  lastncounter3 = 0; //to guarantee Original MSA will command PLL//s after init. ver114-6c
  error="";
  errora="";

  //Initialize Final Filter path.
  int filtbank;
  CommandFilter(filtbank);   //Commands and sets filtbank. Does nothing if suppressHardware=1. ver115-6c

  SelectVideoFilter();  //reselect video filter in case a glitch got it //ver116-1b
  //Note we don't reset the latched switches on Restart (for startup, they are set when prefs are loaded),
  //because it can get obnoxius and requires a time delay.
  //Plus, we don't want to set them when the user makes a change, and immediately set again on Restart.
  //These switches are properly set whenever DetectChanges is called, which should take care of them.
  //ver116-4d deleted call to SelectLatchedSwitches

  //6.if configured, initialize DDS3 by reseting to serial mode. Frequency is commanded to zero
  if (!vars->suppressHardware)
  {

    if (activeConfig->TGtop != 0) //goto endInitializeTrkGen;// there is no Tracking Generator ver111-22
    {
      //Initialize DDS 3
      if (activeConfig->cb == hwdOld && activeConfig->TGtop == 2)
      {
        Jcontrol = INIT;
        swclk = 32;
        sfqud = 2;
        lpt.ResetDDS3ser();
      } //ver111-7
      //[ResetDDS3ser]needs:port,control,Jcontrol,swclk,sfqud,contclear ; resets DDS3 into Serial mode
      if (activeConfig->cb == hwdSlim)
      {
        lpt.ResetDDS3serSLIM(vars->phaarray[vars->thisstep][0], filtbank);
      }
      else if (activeConfig->cb == hwdUsbV1)
      {
        ResetDDS3serUSB();
      }
      //7.if configured, initialize PLO3. No frequency command yet.
      //Initialize PLL 3. //CreatePLL3R,CommandPLL3R
      appxpdf=activeConfig->PLL3phasefreq; //ver111-4
      if (activeConfig->TGtop == 1)
        vars->reference=activeConfig->masterclock; //ver111-4
      if (activeConfig->TGtop == 2)
        vars->reference=activeConfig->appxdds3; //ver111-4
      CreateRcounter();//needs:reference,appxpdf ; creates:rcounter
      rcounter3=rcounter;
      pdf3=pdf;
      //CommandPLL3R and Init Buffers
      datavalue = 8;
      levalue = 4; //PLL3 data and le bit values ver111-28
      CommandPLL3R();//needs:PLL3mode,PLL3phasepolarity,INIT,PLL3 ; Initializes and commands PLL3 R Buffer(s) //ver111-7
    }


    //[endInitializeTrkGen]   //skips to here if no TG

    //8.initialize and command PLO2 to proper frequency
    //CreatePLL2R
    appxpdf = activeConfig->PLL2phasefreq;    //4
    vars->reference = activeConfig->masterclock;    //64.0028
    CreateRcounter();//needed:reference,appxpdf ; creates:rcounter,pdf
    rcounter2 = rcounter;   //16
    pdf2 = pdf;    //actual phase detector frequency of PLL 2   //4.0000175
    //CommandPLL2R and Init Buffers
    datavalue = 16;
    levalue = 16; //PLL2 data and le bit values
    CommandPLL2R();//needs:PLL2phasepolarity,SELT,PLL2 ; Initializes and commands PLL2 R Buffer(s)
    //CreatePLL2N

    appxVCO = activeConfig->appxLO2;
    vars->reference = activeConfig->masterclock;
    CreateIntegerNcounter();//needs:appxVCO,reference,rcounter ; creates:ncounter,fcounter(0)
    ncounter2 = ncounter;
    fcounter2 = fcounter;
    CreatePLL2N();//needs:ncounter,fcounter,PLL2 ; returns with Bcounter,Acounter, and N Bits N0-N23
    Bcounter2=Bcounter; Acounter2=Acounter;
    LO2=((Bcounter*preselector)+Acounter+(fcounter/16))*pdf2; //actual LO2 frequency  //ver115-1c LO2 is now global
    //CommandPLL2N
    Jcontrol = SELT;
    LEPLL = 8;
    datavalue = 16;
    levalue = 16; //PLL2 data and le bit values ver111-28
    CommandPLL(vars->thisstep);//needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111-5

    //9.Initialize PLO 1. No frequency command yet.
    //[InitializePLL1]//set PLL1 to proper Rcount and initialize
    //        appxpdf=PLL1phasefreq //ver111-4
    //        reference=appxdds1 //ver111-4
    //        gosub [CreateRcounter]//needed:reference,appxpdf ; creates:rcounter,pdf
    //        rcounter1 = rcounter //ver111-4
    //Create rcounter1 ver114-2e
    rcounter1=(int)(activeConfig->appxdds1/activeConfig->PLL1phasefreq);
    if ((activeConfig->appxdds1/activeConfig->PLL1phasefreq) - rcounter1 >= 0.5)
      rcounter1 = rcounter1 + 1;   //rounds off rcounter
    if (vars->spurcheck==1 && activeConfig->PLL1mode==0)
      rcounter1 = rcounter1 +1; //only do this for IntegerN PLL

    //CommandPLL1R and Init Buffers
    datavalue = 2;
    levalue = 1; //PLL1 data and le bit values ver111-28
    CommandPLL1R();//needs:rcounter1,PLL1mode,PLL1phasepolarity,SELT,PLL1 ; Initializes and commands PLL1 R Buffer(s)

    //10.initialize DDS1 by resetting. Frequency is commanded to zero
    //It should power up in parallel mode, but could power up in a bogus condition.
    if (activeConfig->cb == hwdOld && activeConfig->dds1parser == 0)
      lpt.ResetDDS1par();//(Orig Control)//needs:control,STRBAUTO,contclear ; resets DDS1 on J5, parallel ver111-21

    if (activeConfig->cb == hwdOld && activeConfig->dds1parser == 1)
      lpt.ResetDDS1ser();//(Orig Control)//needed:control,AUTO,STRB,contclear  ; resets DDS1 on J5, into serial mode ver111-21

    if (activeConfig->cb == hwdSlim)
      lpt.ResetDDS1serSLIM(vars->phaarray[vars->thisstep][0], filtbank);//reset serial DDS1 without disturbing Filter Bank or PDM //ver111-29

    if (activeConfig->cb == hwdUsbV1)
      ResetDDS1serUSB();//reset serial DDS1 without disturbing Filter Bank or PDM
  }
  SkipHardwareInitialization();
}

void hwdInterface::CommandCurrentStep(int step)
{
  //a separate gosub from the old [CommandThisStep] so it can be called not only during regular scanning,
  //but on in combination with [ReadStep] to command and read a particular step, once all info is set up.
  //needs:thisstep ; commands PLL1,DDS1,PLL3,DDS3,PDM //ver111-7
  //a. first, check to see if any or all the 5 module commands are necessary [DetermineModule]
  //b. calculate how much delay is needed for each module[DetermineModule], but use only the largest one[WaitStatement].
  //c. send individual data, clocks, and latch commands that are necessary for[CommandOrigCB]
  //or for SLIM, use [CommandAllSlims] for commanding concurrently
  if (vars->suppressHardware==0)
  {
    int thisBand=vars->datatable[step][4];
    if (thisBand!=vars->lastSetBand)
    {
      SelectLatchedSwitches(thisBand);   //Set band switch
    }
    DetermineModule(step); //determine which, if any, module needs commanding.
    int cmdneeded = glitchp1 + glitchd1 + glitchp3 + glitchd3 + glitchpdm;
    if (cmdneeded > 0 && activeConfig->cb == hwdOld)
      lpt.CommandOrigCB();//old Control (150 usec, 0 SW) //ver111-28ver111-38a
    //if cb = 1 then gosub [CommandRevB]//old Control looking like SLIM  //not created yet
    if (cmdneeded > 0 && activeConfig->cb == hwdSlim)
    {
      lpt.CommandAllSlims(step, filtbank, vars->phaarray[step][0]*64);
      vars->lastpdmstate=vars->phaarray[step][0];
    }
    if (cmdneeded > 0 && activeConfig->cb == hwdUsbV1)
      CommandAllSlimsUSB();
    if (cftest==1)
      CommandLO2forCavTest();
  }
}

void hwdInterface::ReadStep()
{

  //and put raw data bits into arrays. //heavily modified ver116-1b
  int nonPhaseMode=((vars->msaMode==modeSA) || (vars->msaMode==modeScalarTrans));
  int doingPhase= ((vars->suppressPhase==0) && (nonPhaseMode==0));
  int phaseIsStable;
  int magIsStable=0;
  int changePhaseADC=0;
  int changeMagADC=0;
  int repeatOnceMore=0;
  if (vars->useAutoWait)
  {
    vars->wate=(int)(vars->autoWaitTC+0.5);  //wait this much between measurements
    if (doingPhase) phaseIsStable=0; else phaseIsStable=1;
  }
  int prevReadPhaseData = 0;
  int prevReadMagData = 0;
  for (int readStepCount=1; readStepCount <= 25; readStepCount++)
  {
    //If doing auto wait, repeat up to 25 times until readings become stable, as shown by comparing two
    //successive readings. If not doing auto wait, we bail out in the middle of the first pass.
    //if readStepCount=1 then readTime=uTickCount()  //DEBUG
    WaitStatement();//needs:wate,glitch variables,glitchtime ;slows program before reading data //ver111-20b
    prevReadMagData=magdata; //Note if we are starting a new step, but not first of sweep, this is last step final data
    magdata = 0; //reset this variable before reading data
    //Read phase even in non-phase modes unless suppressPhase=1; we just don't process it in non-phase modes
    if (vars->suppressPhase==0)
    {
      prevReadPhaseData=phadata;
      ReadPhase();
      //and return with phadata(in bits). Also installed into pharray(thisstep,3).
      // If serial AtoD, magdata is returned, but not installed in any array
      //if magdata is collected during [ReadPhase], skip Read Magnitude
    }
    else
    {
      phadata=0;   //zero phase info if we are suppressing phase
      vars->phaarray[vars->thisstep][3]=0; //phadata
      vars->phaarray[vars->thisstep][4]=0; //pdm Read state
    }

    //prevReadTime=readTime  //DEBUG
    //readTime=uTickCount()  //DEBUG
    if (magdata == 0)
    {
      ReadMagnitude();//and return with raw magdata bits
    }

    vars->magarray[vars->thisstep][3] = magdata; //put raw data into array
    //the phadata could be in dead zone, but magnitude is still valid.
    //if in VNA Mode and PDM is in automatic, check for phasedata (bits) for limits
    //If magnitude is so low that phase is not valid and will be forced to zero, don't do PDM inversion.
    int readStepDidInvert=0;
    if (doingPhase)
    {
      if (setpdm == 0 && vars->suppressPDMInversion==0
          && (phadata < pdmlowlim || phadata > pdmhighlim))
      {
        //Invert PDM and re-read after waiting. But if mag reading is too low for phase
        //to be valid, don't bother.
        if (magdata>=vars->validPhaseThreshold)
        {
          readStepDidInvert=1;
          InvertPDmodule(vars->thisstep);
        }
      }
    }
    //Note InvertPDmodule will impose some wait time, but not very much in auto wait mode.
    if (vars->useAutoWait==0)
    {
      break;
    }
    //The rest of the loop is just for determining whether to repeat when doing auto wait
    if (repeatOnceMore)
    {
      //            if thisstep>=45 and thisstep<=55 then    //For DEBUG
      //                print "Final Repeat: ms=";magIsStable;" ps=";phaseIsStable;" magChange=";magdata-prevReadMagData;" phaChange=";phadata-prevReadPhaseData
      //            end if
      break; //exit for //we just finished the extra repeat
    }
    //Decide whether we need to repeat
    //        if readStepCount=1 and thisstep>=45 and thisstep<=55 then    //For DEBUG
    //            print "-----------";thisstep;"--------------"
    //            print "First Read: wait=";wate;" ms=";magIsStable;" ps=";phaseIsStable;" mag=";magdata;" pha=";phadata; " Delay=";readTime-prevReadTime
    //        end if

    //We want to keep reading until two successive reads are close to each other, or until the direction reverses.
    //We initially waited one half time constant of the magnitude filter, took a reading, waited,
    //and took a second reading. We determine here whether the changes were less than a predetermined value
    //Once mag or phase is determined to be stable, we flag it as stable so we don't have
    //to re-evaluate after the next read.
    int directionReversal=0; //flag for reversal of sign of change
    int lowLevelADC=0;
    int evaluateThisRead = 0;
    if (vars->thisstep != vars->sweepStartStep)
    {
      //For the first read, we generate the change in readings from the readings left over
      //from the previous step, but only if this is not the first step.
      evaluateThisRead=1;
    }
    else
    {
      //Always evaluate if not first read
      if (readStepCount>1)
        evaluateThisRead=1;
      else
        evaluateThisRead=0;
    }

    if (evaluateThisRead)
    {
      if (magIsStable==0)   //Evaluate mag change if mag not already deemed stable
      {
        int prevMagADCChange=changeMagADC;  //save previous change to compare direction
        changeMagADC=magdata-prevReadMagData;

        int maxADCChange = 0;
        if (magdata<vars->calADCofLowFringe)
        {
          //For very low level signals, just repeat once more, but not if Wide filter
          //We can never expect these to be perfectly stable.
          lowLevelADC=1;
          if (vars->videoFilter != "Wide")
            repeatOnceMore=1;
          magIsStable=1;
          phaseIsStable=1; //pretend. Note phase will be no good at this level so we don't evaluate it.
        }
        else if (magdata < vars->calLowADCofCenterSlope)
        {
          maxADCChange = vars->autoWaitMaxChangeLowEndADC;
        }
        else if (magdata > vars->calHighADCofCenterSlope)
        {
          maxADCChange = vars->autoWaitMaxChangeHighEndADC;
        }
        else   //in center region
        {
          if (changeMagADC<0)
          {
            //If in center on first read but headed for low end, we may end up
            //in the low end, so may want to use the minimum allowed change for center and low end
            //The low end allowed change is likely much smaller than that for center.
            if (abs(changeMagADC)>(magdata - vars->calLowADCofCenterSlope))    //See whether another change like this gets us to low end
            {
              maxADCChange=qMin(vars->autoWaitMaxChangeCenterADC,vars->autoWaitMaxChangeLowEndADC);
            }
            else
            {
              maxADCChange=vars->autoWaitMaxChangeCenterADC;
            }
          }
          else
          {
            maxADCChange=vars->autoWaitMaxChangeCenterADC;
          }
        }

        if (abs(changeMagADC)<=maxADCChange)
          magIsStable=1;
        if (magIsStable==0 && lowLevelADC==0 && readStepCount>1)
        {
          //Even if we don't have two close readings, we can stop when
          //the direction of change reverses, which is a sign the change is being dominated by noise.
          if ((prevMagADCChange<0 && changeMagADC>0) || (prevMagADCChange>0 && changeMagADC<0))
          {
            magIsStable=1;
            directionReversal=1;
          }
        }
      }

      //Now do phase
      if (phaseIsStable==0) //evaluate phase change if phase not already deemed stable
      {
        //Note that if we just inverted phase, it is still valid to compare this phadata to the
        //previous one, because settling time will still be based on that change.
        if (magdata<vars->validPhaseThreshold)
        {
          //In this case we have no valid phase reading. If this is likely a final reading, then just deem
          //phase to be stable.
          if (magIsStable)
            phaseIsStable=1; //can//t actually evaluate low level phase
        }
        else
        {
          int prevPhaseADCChange=changePhaseADC;
          changePhaseADC=phadata-prevReadPhaseData;
          if (abs(changePhaseADC)<=vars->autoWaitMaxChangePhaseADC)
          {
            phaseIsStable=1;
          }
          else
          {
            //Even if we don't have two close readings, we can stop when
            //the direction of change reverses, which is a sign the change is being dominated by noise.
            //Can//t do this if we just inverted the PDM for this read, because that might cause
            //reversal of sign of the change
            if (readStepDidInvert==0 && readStepCount>1
                && ((prevPhaseADCChange<0 && changePhaseADC>0)  || (prevPhaseADCChange>0 && changePhaseADC<0)))
            {
              phaseIsStable=1;
              directionReversal=1;
            }
          }
        }
      }
      //            if thisstep>=45 and thisstep<=55 then    //For DEBUG
      //                print "Analysis "; readStepCount; ": ms=";magIsStable;" ps=";phaseIsStable;" mag=";magdata;" pha=";phadata;" magChange=";changeMagADC;" phaChange=";changePhaseADC;" Delay=";readTime-prevReadTime
      //            end if
      if (magIsStable && phaseIsStable)
      {
        //If Precise, repeat one more
        //time and stop after that without comparing readings.
        //low level is already set to repeat once more unless Wide filter
        if (vars->autoWaitPrecision=="Precise") repeatOnceMore=1;
        //For lowLevelADC, we already set repeatOnceMore (and phase is meaningless)
        if (lowLevelADC==0 && repeatOnceMore==0) break; // then exit for
        //Also repeat once more if ending because of direction reversal, other than
        //direction reversal on second read.
        if (directionReversal && readStepCount<3) repeatOnceMore=1; else break; //exit for
      }
    }
    //No point repeating if we aren//t actually reading data. But we went through the motions above
    //for possible debugging use.
    if (vars->suppressHardware)
    {
      break;
    }
  }
}
void hwdInterface::ProcessAndPrintLastStep()
{
  int rememberstep = vars->thisstep; //remember where we were when entering this routine
  //since we are processing and printing the previous step, use raw data in array(thisstep - sweepDir,data)

  if (vars->thisstep == vars->sweepStartStep)
  {
    vars->thisstep = vars->sweepEndStep;   //back up one and wrap around
  }
  else
  {
    vars->thisstep = vars->thisstep - vars->sweepDir;  //Back up one step; no wraparound to worry about
  }
  ProcessAndPrint();//get raw data, process, print to the computer monitor
  vars->thisstep = rememberstep;
}

void hwdInterface::specTestToggleVisible()
{
  specTests->setVisible(!specTests->isVisible());
}

void hwdInterface::WaitStatement()
{
  //needed;wate,glitch()(p1,d1,p3,d3,pdm,hlt),glitchtime ; this slows the program //ver111-27
  glitch = qMax(qMax(qMax(glitchp1, glitchd1),qMax(glitchp3, glitchd3)), qMax(glitchpdm, glitchhlt)); //ver111-27
    //glitchp1=PLL1;glitchd1=DDS1;glitchp3=PLL3;glitchd3=DDS3;glitchpdm=PDM(10);glitchhlt=halted(10)
  int waittime = vars->wate + glitch;   //number of ms we need to wait //ver115-1i
    //in my Toshiba, a waittime count of 80 gives a delay of approx, 1 millisecond
    //therefore, each increment of any "glitchXX" or "wate" (Wait Box) should add 1 ms of delay before a "read"
  if (waittime>0)   //ver115-1i added the use of the system sleep function
  {
    if (vars->doingInitialization || waittime<15)
    {
      //For short wait times we use our own timing loop
      //Also for initialization, when we are measuring glitchtime
      waittime=waittime*vars->glitchtime;
      int timecounter = 0;
      //[TimeLoop]
      while (timecounter < waittime)
      {
        timecounter = timecounter + 1;
      }//;goto [TimeLoop]
    }
    else
    {
        //It is preferable to use the system Sleep function, but it operates in increments of 10-15 ms.
        //So it is only suitable for long wait times
      util.uSleep(waittime);
    }
  }
  glitchp1=0;
  glitchd1=0;
  glitchp3=0;
  glitchd3=0;
  glitchpdm=0;
  glitchhlt=0; //reset glitch variables back to 0 //ver111-27
}
void hwdInterface::AutoGlitchtime()
{
  int whatiswate;
  QString a;
  QString b;
  vars->glitchtime = 10000;
  whatiswate = vars->wate;
  vars->wate = 1;
  a = util.time("ms"); //time of day, in milliseconds. This uses the computer//s internal clock
  int count = 100;
  while (count > 0)
  {
    WaitStatement();
    count--;
  }
  b = util.time("ms");
  int lasped = b.toInt()-a.toInt();
  if (lasped == 0)
    lasped = 1;
  vars->glitchtime = vars->glitchtime/lasped * 100; //glitchtime is the value required for a 1 ms wait time
  /*
  vars->wate = 10;
  a = util.time("ms");
  WaitStatement();
  b = util.time("ms");
  lasped = b.toInt()-a.toInt();
  qDebug() << "lasped" << lasped;
  */
  vars->wate = whatiswate; //change wate back to it's original global value
}
void hwdInterface::ReadMagnitude()
{
  //needed: port,status ; creates: magdata (and phadata for serial A/D//s)
  if (vars->suppressHardware)
  {
    magdata=0;
    return;
  }
  if (activeConfig->adconv ==  8)
  {
    lpt.Read8Bitmag(); //and return here with magdata
  }
  else if (activeConfig->adconv == 12)
  {
    lpt.Read12Bitmag(); //and return here with magdata
  }
  if (activeConfig->cb == hwdUsbV1)
  {
    ReadADCviaUSB(); // and return here with magdata and phadata
  }
  else if (activeConfig->cb != hwdUsbV1 && activeConfig->adconv == 16)
  {
    if (activeConfig->cb == hwdSlim)
    {
      lpt.Read16wSlimCB();
    }
    else
    {
      lpt.ReadAD16Status();
    }
    lpt.Process16Mag(magdata);
  }
    //and return here with just magdata //ver111-33b
  else if (activeConfig->cb != hwdUsbV1 && activeConfig->adconv == 22)
  {
    lpt.ReadAD22Status();
    lpt.Process22Mag();
  }
    //and return here with just magdata //ver111-37a
}
void hwdInterface::ReadPhase()
{
  //needed: port,status ; creates: phadata (and magdata for serial A/D//s)
  if (vars->suppressHardware)
  {
    phadata=0;
  }
  else
  {
    switch(activeConfig->adconv)
    {
    case 8:
      lpt.Read8Bitpha(); //and return here with phadata only
      break;
    case 12:
      lpt.Read12Bitpha(); //and return here with phadata only
      break;
    default:
      if (activeConfig->cb == hwdUsbV1)
      {
        ReadADCviaUSB();
      }
      else if (activeConfig->cb != hwdUsbV1 && activeConfig->adconv == 16)
      {
        lpt.ReadAD16Status();
        lpt.Process16MagPha(magdata, phadata);
      }
      else if (activeConfig->cb != hwdUsbV1 && activeConfig->adconv == 22)
      {
        lpt.ReadAD22Status();
        lpt.Process22MagPha();
      }
      // and return here with magdata and phadata
    }
  }
  if (vars->doSpecialGraph>0)
    phadata=activeConfig->maxpdmout/4 + vars->thisstep*30;    //Force to a value not requiring constant inversion
  //and return here with phadata (and magdata, if serial AtoD)
  //if calibrating the PDM inversion, don't put raw data into arrays, used only in [CalPDMinvdeg]
  if (vars->doingPDMCal == 1)
    return; //to [CalPDMinvdeg]
  vars->phaarray[vars->thisstep][3] = phadata; //put raw data into array
  vars->phaarray[vars->thisstep][4] = vars->phaarray[vars->thisstep][0]; //PDM state at which this data is taken. ver112-2a
  //it is only used in Variables Windows to show state of PDM when data was collected.
  return;

}
void hwdInterface::InvertPDmodule(int step)
{
  //qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;

  //this will change the state of the PDM for this step and future steps //ver111-28
  //entered from [ReadStep], where it was determined that phadata was in, or close to "dead zone"
  //this subroutine will re-command PDM, and return to [ReadStep] and read the phase again,
  //but not test for dead zone again, just assumes data to be viable
  //determine what the pdmstate was when entering, and "flip" it
  int newpdmstate;
    if (vars->suppressHardware)
      return;
    if (vars->phaarray[step][0] == 0)
      newpdmstate = 1;
    if (vars->phaarray[step][0] == 1)
      newpdmstate = 0;
    //change the pdm state for thisstep to the newpdmstate
    vars->phaarray[step][0] = newpdmstate;
    //now, go and command the PDM to the new state (Command PDM only!)
    CommandPDMonly(); //command just the PDM ver111-28
    //this has just created a large glitch in the PDM output, so while it is settling down,
    //change future PDM commands for all subsequent steps to end of sweep.
    for (int i = step; i <= vars->sweepEndStep; i = i + vars->sweepDir)
    {
        vars->phaarray[i][0] = newpdmstate; //inverts pdmcmd for thisstep and subsequent steps to end of sweep. ver111-28
    }
    //add appropriate wait time before reading the phase again
    VideoGlitchPDM();//calculates glitchpdm, depending on Video Selection and auto wait state
    WaitStatement(); //and use the new glitchpdm value
    //now, go and read Phase, again. Use its "return" to return to [ReadStep]
    ReadPhase(); //re-read the phase and return to [ReadStep]
    //when back in [ReadStep], it will not test for dead zone again. Assumes valid Phase.
}
void hwdInterface::VideoGlitchPDM()
{
//entered from [InvertPDmodule],[PDM] //changed by ver116-1b
  //We want to wait 10 ms plus 12 time constants when inverting the PDM, but max of 5 seconds.
  //If auto wait mode, we wait less time. For small time constants, we wait a certain minimum in auto wait because
  //we need the PDM to become stable before we can rely on steady settling per the time constant.
  if (vars->useAutoWait)
    glitchpdm=glitchpdm + (int)(20+vars->videoPhaseTC*5);
  else
    glitchpdm=glitchpdm + (int)(20+vars->videoPhaseTC*12);
  if (glitchpdm>5000) glitchpdm=5000;   //5 sec max
  if (vars->useAutoWait==0 && glitchpdm < vars->wate)
    glitchpdm=vars->wate; //no less than the normal wate time
  //    if videoFilter$ = "Wide" then glitchpdm = glitchpdm + 10 // or maybe just glitchpdm=10, etc
  //    if videoFilter$ = "Mid" then glitchpdm = glitchpdm + 100
  //    if videoFilter$ = "Narrow" then glitchpdm =  glitchpdm + 1000

}
void hwdInterface::CalPDMinvdeg()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CalPDMinvdeg] //to find the amount of phase shift when the PDM state is inverted
    //invdeg is a calibration value used in [ConvertPhadata], (phase of inverted PDM) - (invdeg) = real phase of PDM. ver113-7b
    //the VNA must be in "0" sweepwidth, freq close to the transition point.
    if suppressHardware then //ver115-6c
        message$=180;"  ";phase1 : call PrintMessage //ver114-4e
        return
    end if
    rememberpdmstate = phaarray(thisstep,0) //ver112-2e
    #handle.Restart, "Cal PDM"      //ver114-4c deleted print to #main.restart
    beep
    phaarray(thisstep,0) = 0 //command PDM to Normal //ver111-29
    gosub [CommandPDMonly] //ver111-29
    glitchpdm = 5000 //should equate to 5 seconds of delay //ver111-29
    gosub [WaitStatement] //ver111-29
    gosub [ReadPhase]//return here with phadata //ver111-29
    //the 16 bit serial has just been "hit" with a conv (D7)in Commanding the Orig PDM. OK to hit it again? Yes.
    //expect phadata to be either 80% or 20% of maxpdmout
    phase0 = 360*phadata/maxpdmout //convert to degrees
    phaarray(thisstep,0) = 1 //command PDM to Inverted //ver111-29
    gosub [CommandPDMonly] //ver111-29
    glitchpdm = 5000 //should equate to 5 seconds of delay //ver111-29
    gosub [WaitStatement] //ver111-29
    gosub [ReadPhase]//return here with phadata //ver111-29
    //expect phadata to be either 20% or 80% of maxpdmout
    phase1 = 360*phadata/maxpdmout //convert to degrees
    CalInvDeg = phase1 - phase0     //ver114-5L
    if CalInvDeg < 0 then CalInvDeg = CalInvDeg + 360
    //The inversion deviation from 180 degrees is really a fixed time period, so its value in degrees depends
    //on frequency. We store invdeg normalized to 10.7 MHz.
    CalInvDeg=180+(CalInvDeg-180)*10.7/finalfreq    //ver116-1b
    CalInvDeg = val(using("####.##",CalInvDeg)) //PDM//s phase shift when inverted, in .01 degree resolution
    call DisplayButtonsForHalted   //SEW8 replaced print #main.restart, "Restart"
    message$=phase0;"  ";phase1 : call PrintMessage //ver114-4e
    beep  //ver111-36d
    //put PDM into the state at which it entered this subroutine
    phaarray(thisstep,0) = rememberpdmstate //ver112-2e
    gosub [CommandPDMonly] //ver112-2e
    return      //ver114-5k changed wait to return

    */
}
void hwdInterface::ReadADCviaUSB()
{
 // the main code has already set up the structure that defines the ADC conversion so just go for it !
  //This is new common subroutine for reading the ADC//s with USB. //ver116-4r
  //This will read the ADC//s and return the values for magdata and phadata
  magdata = 0;
  phadata = 0;
  unsigned long result;

  UsbAdcControl ctrl;
  ctrl.Adcs = 3;
  ctrl.Clocking = 1;
  ctrl.Delay = 4;
  ctrl.Bits = 16;
  ctrl.Average = 1;
  USBrBuf buf;
  result = usb->usbMSADeviceReadAdcsStruct((unsigned short *)&ctrl,(unsigned long *)&buf);


  if( result )
  {
//    UsbAdcCount = USBrBuf.numreads.struct   //this is not used anywhere, but I will leave it here as a reference //ver116-4r
    magdata = buf.magnitude;
    phadata = buf.phase;
    //Since the 12 bit serial ADC is read as 16 bits, throw away the last 4 bits
    if (activeConfig->adconv == 22)
    {
      magdata = (int)(magdata/16);
    }
    if (activeConfig->adconv == 22)
    {
      phadata = (int)(phadata/16);
    }
  }
  else
  {
    magdata = qrand() / 2;
    phadata = qrand() / 2;
  }
  return;
}



void hwdInterface::ConvertPhadata()
{
  //needed: phadata,PDM polarity,difPhase ; creates "phaseofpdm" and "thispointphase", the pixel value
  //retrieve phadata from array
  //convert phadata to phase, round off to .01 deg
  //compensate phase using "invdeg" (if PDM was inverted during the phase reading)
  //compensate phase using "difPhase", Phase Error Correction Factor//
  //   as determined in Path Calibration, (variation of phase readings over signal level)
  //if normal sweep, process phase by removing line calibration phase
  //put final phase into datatable
  //Do not do Reference Plane Extension here
  //convert phase to +180 to -180 format and round off to .01 degree

  //grab raw phase data bits from array
  phadata = vars->phaarray[vars->thisstep][3];
  //convert phadata to absolute phase lead, referenced to 0 degrees. maxpdmout/4 = 90 degrees lead,maxpdmout/2 = 180 degrees lead
  float phase = 360*phadata/activeConfig->maxpdmout;  //converts phadata bits to absolute phase. ver15-2d moved rounding to later
  //the absolute phase will normally be between limits of +288 and +72 degrees,
  //however, it can be between 360 and 0 degrees if the PDM is "forced" into a set state (setpdm=1)
  //ver116-4h prevented getting here when doSpecialGraph>0, so we don't need to test it here
  //if PDM was inverted, subtract the inverted phase change
  //The deviation in the PDM inversion from theoretical 180 deg is actually a fixed time period
  //invdeg is maintained as the actual inversion that occurs at 10.7 MHz. With a different finalfreq,
  //the deviation from 180 degrees will change.
  /*float PDMinversionDeviation;
  if (vars->phaarray[vars->thisstep][0]==1)
  {
    PDMinversionDeviation=(activeConfig->invdeg-180)*activeConfig->finalfreq/10.7;
    phase = phase - (180+PDMinversionDeviation);
  }*/
  if (vars->phaarray[vars->thisstep][0] == 1)
  {
    phase = phase - activeConfig->invdeg;
  }
  phase=phase-difPhase;    //SEW3: subtract correction for change of phase over signal level.
  int thisBand=vars->datatable[vars->thisstep][4];
  if (thisBand==3)
  {
    phase=0-phase;    //For 3G mode, true phase is negative of measured phase, due to low side LO1
  }
  //calculate phase with calibration table factored in
  //ver114-5f Apply lineCalArray only if LineCal or BaseLineCal is active, and not when calibrating
  //Note if calInProgress=1, applyCalLevel will have been set to 0 by cal installation routine
  if (vnaCal->applyCalLevel>0)
  {
    phase = phase - vars->lineCalArray[vars->thisstep][2]; //subtract reference.
  }
  if (phase>=0)
  {
    phase=((int)(phase*100+0.5))/100;
  }
  else
  {
    phase=((int)(phase*100-0.5))/100; //round to two decimal places
  }
  //ver115-2b moved the application of planeadj. The data in datatable() never contains that adjustment.

  //The phase correction is set to 180 degrees when the phase reading during initial calibration
  //is suspect. In that case, we override all the foregoing and set the final phase to 0.
  //validPhaseThreshold indicates the lowest magnitude level at which phase is valid.
  if (magdata < vars->validPhaseThreshold)
  {
    phase=0;
  }

  //convert to standard +180 -180 format
  while (phase >180)
  {
    phase = phase - 360;
  }
  while (phase <=-180)
  {
    phase = phase + 360;
  }
  //write the processed phase into the memory array, +180 to -180 deg
  vars->datatable[vars->thisstep][3] = phase;    //put current phase measurement into the array, line value= thisstep
  //Note that phase may have to be adjusted during graphing to fit the bounds of the display
}
//--SEW Replaced [ConvertMagData] and [ConvertFreq] with following combined routine,
//to utilize the calibration module to interpolate the necessary correction factors
//to convert the raw ADC reading into dbm, and then to correct that number for variations
//over frequency. Phase correction is also calculated so the routine name was changed.
//That phase correction is difPhase and is subtracted from phase in ConvertPhadata
//The phase correction for signal level has to be calculated here, because it
//is a function of magnitude ADC reading and so can be interpolated at the same time
//as that ADC reading is converted to mag power. This messes up the terminology a bit
//because we now have ConvertPhadata and ConvertMagPhaseData. But it works.
void hwdInterface::ConvertMagPhaseData()
{
  //convert magnitude data bits to MSA input power(in dBm) and to pixels.
  //needed: magarray,calibration table
  //this converts magdata to MSA input power, using
  //a Magnitude Error Correction Factor, (determined in Frequency Calibration)
  //If in VNA mode, it also finds the phase correction for the power level indicated
  //by magdata, and put it into difPhase, to be subtracted from phase later.

  float magdata, doPhaseCor, power, difPhase;
  //int freqerror;
  if (vars->doSpecialGraph==0)
  {
    //Normal scan. Apply the calibration
    magdata = vars->magarray[vars->thisstep][3];
    //Apply mag calibration to get power and phase correction

    if (vars->msaMode != modeSA && vars->msaMode != modeScalarTrans)
    {
      doPhaseCor=1;
    }
    else
    {
      doPhaseCor=0;
    }
    calMan->calConvertMagPhase(magdata, doPhaseCor, power, difPhase);
    vars->thisfreq = vars->datatable[vars->thisstep][1];
    freqerror=vars->freqCorrection[vars->thisstep]; //find freq cal adjustment SEWgraph1
    //In SA mode, if there is an active front end file, we add the front end correction factor
    if (vars->msaMode==modeSA)
    {
      if (vars->frontEndActiveFilePath!="")
        freqerror=freqerror-vars->frontEndCorrection[vars->thisstep];
    }
  }
  else
  {
    if (vars->calInProgress && vars->msaMode!=modeReflection)
    {
      //If calibrating transmission, we want to use ideal results so when we display the actual doSpecialGraph
      //it will come out the way we want.
      power=0;
      int phase=0;
      vars->datatable[vars->thisstep][3] = phase;
    }
    else
    {
      DoSpecialGraph(power);
    }
  }

  power = power + freqerror;
  if (convdatapwr == 1)
  {
    ConvertDataToPower();
  }
  //round off MSA input power to .01 dBm, magpower, no matter which AtoD is used
  float magpower = power;
  //Note if calInProgress=1, applyCalLevel will have been set to 0 by cal installation routine
  if (vnaCal->applyCalLevel>0)
  {
    if (vars->msaMode!=modeSA)
      magpower = magpower - vars->lineCalArray[vars->thisstep][1];
  }  // subtract reference.

  //round to five decimal places
  if (magpower>=0)
    magpower=((int)(magpower*100000+0.5))/100000.0;
  else
    magpower=((int)(magpower*100000-0.5))/100000.0;

  vars->datatable[vars->thisstep][2] = magpower;    //put current power measurement into the array
  return;

}

void hwdInterface::DoSpecialGraph(float &power)
{

  difPhase=0;
  freqerror=0;
  float minADC, maxADC, calMag, calPhase, dum;
  float testSlope;
  float phase;
  float magData;
  float currXVal;
  double part1, part2;
  double specialWholeFreq;
  double specialFractFreq;
  double specialNoise;


  switch(vars->doSpecialGraph)
  {
  case 1:
    //Graph mag calibration table. Find the min and max ADC values
    //and make magdata run linearly from the minimum to the maximum.
    //Any non-linearities in the graph then reflect the calibration
    //We do nothing with phase
    calMan->calGetMagPoint(1, minADC, calMag, calPhase);    //ignore calMag and calPhase
    calMan->calGetMagPoint(calMan->calNumMagPoints(), maxADC, calMag, calPhase);
    testSlope=(maxADC-minADC)/vars->steps;
    magData=minADC+testSlope*vars->thisstep;
    //Apply mag calibration to get power, but forget phase correction
    calMan->calConvertMagPhase(magData, 0, power, dum);
    //skip freq cal
    break;

  case 2:
    //Force power to 0 dbm and then find the frequency compensation.
    //The resulting graph will show the shape of the frequency compensation
    //curve.
    power=0;
    vars->thisfreq = vars->datatable[vars->thisstep][1];
    freqerror=vars->freqCorrection[vars->thisstep]; //find freq cal adjustment SEWgraph1
    break;
  case 3:
    //Generate random values for magdata superimposed on a sine wave centered midway in ADC
    //Make Transmission values a little higher.
    calMan->calGetMagPoint(1,minADC, calMag, calPhase);    //ignore calMag and calPhase
    calMan->calGetMagPoint(calMan->calNumMagPoints(),maxADC, calMag, calPhase);
    //ran = rand() % 100 / 100.0;
    //noise = 3*(1+ rand() % 100 / 100.0 / 20.0) * (maxADC)/4;
    magData= (3*(1+ rand() % 100 / 100.0 / 20.0) * (maxADC)/4) +  3000*sin(10*vars->datatable[vars->thisstep][1]);
    if (vars->msaMode==modeVectorTrans || vars->msaMode==modeScalarTrans)
    {
      magData=1.1*magData;    //Increase transmission values
    }
    //Note: without being connected to MSA, which is when DoSpecialGraph is useful,
    //phase will bounce between 0 and 180 and thus will show some graph action.
    calMan->calConvertMagPhase(magData, 1, power, difPhase);
    vars->thisfreq = vars->datatable[vars->thisstep][1];
    freqerror=vars->freqCorrection[vars->thisstep]; //find freq cal adjustment SEWgraph1
    phase=180-10*vars->datatable[vars->thisstep][1];  //linear change over frequency
    vars->datatable[vars->thisstep][3] = phase;
    break;

  case 4:
    //Generate a peak near 1 MHz
    currXVal=gGetPointXVal(vars->gGraphVal, vars->gMaxPoints,  vars->thisstep+1) - 1;
    //Upside down parabola centered  near MHz
    power=qMax(-6-(3000+vars->doSpecialRandom*2000) * pow((currXVal+0.025-vars->doSpecialRandom/20),2.0),-100.0);
    phase=270-300*vars->datatable[vars->thisstep][1];
    vars->datatable[vars->thisstep][3] = phase;
    break;

  case 5:
    //For SA mode, do response of a 1 MHz square wave.
    //For VNA modes, Calc response of an RLC circuit with optional transmission line //ver114-7e
    if (vars->msaMode==modeSA)   //added the 1 MHz square wave for SA mode
    {
      currXVal=gGetPointXVal(vars->gGraphVal, vars->gMaxPoints,  vars->thisstep+1);
      specialWholeFreq=round(currXVal);
      //specialWholeFreq=int(currXVal + 0.5);
      specialFractFreq=currXVal-specialWholeFreq; //fract may be -0.5 to +0.5
      specialNoise=(1e-11)*activeConfig->finalbw*(1+4*(rand() % 100 / 100.0));
      if (specialWholeFreq == 2*int(specialWholeFreq/2))
      {
        power=10*util.uSafeLog10(specialNoise);
      }
      else
      {
        //1 MHz square wave at odd multiples of 1 MHz has power of 1/N mw, where N is the multiple.
        //It tapers off per a parabola, which is wider for higher RBW.
        part1 = 1/pow(specialWholeFreq,2.0);
        part2 = part1 * qMax(0.0, (1-pow((1400*specialFractFreq/activeConfig->finalbw),2) ));
        power=10 * util.uSafeLog10(specialNoise + part2);
      }
      phase=0;
      vars->datatable[vars->thisstep][3] = 0;
    }
    else    //VNA modes
    {
      uWork->uWorkNumPoints=1;
      uWork->uWorkArray[1][0]=gGetPointXVal(vars->gGraphVal, vars->gMaxPoints,  vars->thisstep+1);//set up for uRLCComboResponse
      uWork->uWorkArray[1][1]=0;
      uWork->uWorkArray[1][2]=0; //Default in case of error
      //Calc response in whatever S11 or S21 setup the user has chosen

      QString doSpecialJig;
      double doSpecialR0;
      if (vars->msaMode == modeReflection)
      {
        doSpecialR0=vars->S11BridgeR0;
        doSpecialJig="S11";
      }
      else
      {
        doSpecialR0=vnaCal->S21JigR0;
        if (vnaCal->S21JigAttach == "Series")
          doSpecialJig="S21Series";
        else
          doSpecialJig="S21Shunt";

      }

      //Note we only have one point in uWorkArray for uRLCComboResponse to process
      //Note calibration will not be applied for doSpecialGraph=5, so uRLCComboResponse
      //calculates the actual final response. e.g. S21JigShuntDelay is not taken into account, because
      //it would be removed by a perfect calibration.
      int isErr = util.uRLCComboResponse(vars->doSpecialRLCSpec, doSpecialR0, doSpecialJig, uWork->uWorkNumPoints, uWork->uWorkArray);
      power=uWork->uWorkArray[1][1];   //get results of uRLCComboResponse
      phase=uWork->uWorkArray[1][2];
      vars->datatable[vars->thisstep][3] = phase;

    }
    break;
  default:
    power=0;
    phase=0;
    vars->datatable[vars->thisstep][3] = phase;
    qDebug() << "Unhandled doSpecial";
  }
}

float hwdInterface::gGetPointXVal(Q2DfloatVector &GraphVal, int MaxPoints, float N)
{
  float x;
  //Return x for specified point (1...)
  //We don't verify that N is in bounds, because its value may have been created with gGenerateXValues
  //and the actual point data may not have been added yet.
  //N may have a fractional part, so we do linear interpolation
  if (N>0 && N<=MaxPoints)
  {
    int whole=int(N);
    float fract=N-whole;
    x=GraphVal[whole][0];
    if (fract>0)
    {
      x=x+fract*(GraphVal[whole+1][0]-x);
    }
  }
  else
  {
    x=-1;
  }
  return x;
}
void hwdInterface::FunctChangeAndSaveSweepParams()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub FunctChangeAndSaveSweepParams saveSettings, bandToBase, newSteps, newStart, newEnd, newLinear    'Change sweep params for use in a Function; save old params
    'We also set sweep direction, planeadj and wate to default values
    'We also save the current band cal as a base cal. If saveSettings=1 we also save the current sweep settings.
    'if bandToBase=1, we transfer the existing band cal to the base cal (but not to the file) and use it.
    'Caller must set steps (but not globalSteps) before calling us because we can't change steps, but
    'we will handle informing the graph module of the new number of steps
    'Currently, this does not work for Reflection mode, because OSL is not saved.
    'Example of Use:
    '   steps=100   'Note globalSteps will change in FunctChangeAndSaveSweepParams
    '   call FunctChangeAndSaveSweepParams 1,1,1, 100, 2, 200, 1    'save settings and cal and change to Linear 2 to 200 MHz, 100 steps
    '   specialOneSweep=1   'So we return from [Restart]
    '   gosub [Restart]     'Do actual scan to acquire data
    '       ...Process data...
    '   gosub [FunctRestoreSweepParams]
    '   suppressHardwareInitOnRestart=1 'if there is no need to update hardware and we want to save time; this is set to 0 afer restarting
    '   gosub [PartialRestart]  'or call RequireRestart
    'If a function calls several times, the existing settings should be saved only on the first call; otherwise
    'the saved data will get overwritten by the previous changes

    if saveSettings then  'Save existing settings
        functSaveAlternate=alternateSweep : functSaveSweepDir=sweepDir : functSavePlaneAdj=planeadj
        functSaveSteps=globalSteps : functSaveStartFreq=startfreq : functSaveEndFreq=endfreq
        functSaveAutoY1=autoScaleY1 : functSaveAutoY2=autoScaleY2
        functSaveY1Mode=Y1DisplayMode : functSaveY1Mode=Y1DisplayMode
        functSaveY1DataType=Y1DataType : functSaveY2DataType=Y2DataType
        call gGetIsLinear functSaveXIsLinear, functSaveY1IsLinear, functSaveY2IsLinear
        call gGetNumDivisions functSaveNumHorDiv, functSaveNumVertDiv 'Note log sweep changes the hor divisions
        functSaveDesiredCalLevel=desiredCalLevel
    end if
    'Set up the sweep params we want
    call FunctSetVideoAndAutoWait saveSettings  'save old settings and set new
    alternateSweep=0 : sweepDir=1 : planeadj=0
    globalSteps=newSteps        'Caller must change steps
    call gSetXIsLinear newLinear
    call gSetNumDynamicSteps globalSteps    'Tell graph module
    call SetStartStopFreq newStart, newEnd
    if bandToBase and (msaMode$=modeVectorTrans or msaMode$=modeScalarTrans) then
        call TransferBandToBaseLineCal  'Make current band cal into a base cal so it can be interpolated
        desiredCalLevel=1   'Use the base cal we just created
        call SignalNoCalInstalled   'ver116-4b
    end if
end sub
*/
}

void hwdInterface::FunctSetVideoAndAutoWait()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub FunctSetVideoAndAutoWait saveSettings    'Set video filter to narrow and wait to autoWait modver116-4k
    if saveSettings then _
        functSaveWate=wate : functSaveAutoWait=useAutoWait: functSaveAutoWaitPrecision$=autoWaitPrecision$ : functSaveVideoFilter$=videoFilter$ 'ver116-1b
    if videoFilter$="XNarrow" then desiredVideoFilter$="XNarrow" else desiredVideoFilter$="Narrow"
    if canUseAutoWait then
        if useAutoWait=0 or (autoWaitPrecision$<>"Precise") then autoWaitPrecision$="Normal"    'choose Normal unless user is already Precise
        useAutoWait=1
    else
        wate=500    'It should always be possible to use auto wait, but just in case...
    end if
    if videoFilter$<>desiredVideoFilter$ and switchHasVideo=0 then Notice "Physically select "; desiredVideoFilter$; " video filter."
    videoFilter$=desiredVideoFilter$
    call SelectVideoFilter  'also recalculates auto wait info
end sub
*/
}

void hwdInterface::FunctRestoreVideoAndAutoWait()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub FunctRestoreVideoAndAutoWait    'restore auto wait settings and, if automated video switch, video filter setting
    wate=functSaveWate : autoWaitPrecision$=functSaveAutoWaitPrecision$ : useAutoWait=functSaveAutoWait
    if switchHasVideo then
        'if no automated video filter switch, we leave the video filter as it was set for the function
        videoFilter$=functSaveVideoFilter$
        call SelectVideoFilter  'Note this also calls autoWaitPrecalculate ver116-4b
    else
        call autoWaitPrecalculate
    end if
end sub
*/
}

void hwdInterface::FunctRestoreSweepParams()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[FunctRestoreSweepParams]  'Restore params and base cal to what they were when function was entered
    'Not a true subroutine so it can access steps.
    call FunctRestoreVideoAndAutoWait
    sweepDir=functSaveSweepDir : alternateSweep=functSaveAlternate : planeadj=functSavePlaneAdj
    steps=functSaveSteps : startfreq=functSaveStartFreq : endfreq=functSaveEndFreq
    globalSteps=steps
    autoScaleY1=functSaveAutoY1 : autoScaleY2=functSaveAutoY2
    Y1DisplayMode=functSaveY1Mode : Y1DisplayMode=functSaveY1Mode
    Y1DataType=functSaveY1DataType : Y2DataType=functSaveY2DataType
    call gSetIsLinear functSaveXIsLinear, functSaveY1IsLinear, functSaveY2IsLinear
    call gSetNumDynamicSteps steps    'Tell graph module
    call SetStartStopFreq functSaveStartFreq, functSaveEndFreq
    call gSetNumDivisions functSaveNumHorDiv, functSaveNumVertDiv

    if msaMode$=modeVectorTrans or msaMode$=modeScalarTrans then
        dum=LoadBaseLineCalFile()   'Reload base cal file to restore prior state
    end if
    desiredCalLevel=functSaveDesiredCalLevel    'ver115-5e
    call SignalNoCalInstalled   'ver116-4b'So new cal will get installed on Restart
return
*/
}
void hwdInterface::FillRegularGraphData(int axisNum)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function FillRegularGraphData(axisNum) 'Fill axisGraphData$ and axisDataType for regular scan graphs; return number of graphs
    'Added by ver116-1b
    for i=0 to 40 : axisGraphData$(i)="" : next i
    if msaMode$=modeSA then
        axisGraphData$(0)="Magnitude (dBm)" : axisDataType(0)=constMagDBM
        axisGraphData$(1)="Magnitude (Watts)" : axisDataType(1)=constMagWatts
        axisGraphData$(2)="Magnitude (Volts)" : axisDataType(2)=constMagV
        if (axisNum=1 and Y2DataType<>constNoGraph) or (axisNum=2 and Y1DataType<>constNoGraph) then _
                        axisGraphData$(3)="None" : axisDataType(3)=constNoGraph   'ver115-3b
        numGraphs=4   'ver115-4a
    end if
    if msaMode$=modeScalarTrans then
        axisGraphData$(0)="Transmission (dB)": axisDataType(0)=constMagDB   'ver115-4f
        axisGraphData$(1)="Transmission (Ratio)" : axisDataType(1)=constMagRatio    'ver115-4f
        axisGraphData$(2)="Insertion Loss (db)" : axisDataType(2)=constInsertionLoss
        numGraphs=3   'ver116-1b
        if (axisNum=1 and Y2DataType<>constNoGraph) or (axisNum=2 and Y1DataType<>constNoGraph) then _
                        axisGraphData$(3)="None" : axisDataType(3)=constNoGraph: numGraphs=numGraphs+1 'ver116-1b
    end if

    if msaMode$=modeVectorTrans then
        axisGraphData$(0)="S21 Magnitude (dB)" :axisDataType(0)=constMagDB
        axisGraphData$(1)="S21 Phase Angle" : axisDataType(1)=constAngle
        axisGraphData$(2)="Raw Power (dBm)" : axisDataType(2)=constMagDBM
        axisGraphData$(3)="Raw Phase Angle" : axisDataType(3)=constRawAngle
        axisGraphData$(4)="Insertion Loss (db)" : axisDataType(4)=constInsertionLoss
        axisGraphData$(5)="S21 Group Delay" : axisDataType(5)=constGD
        numGraphs=6   'ver116-1ba
            'Display "None" only if the other axis is not set to None.
        if (axisNum=1 and Y2DataType<>constNoGraph) or (axisNum=2 and Y1DataType<>constNoGraph) then _
                        axisGraphData$(6)="None" : axisDataType(6)=constNoGraph: numGraphs=numGraphs+1 'ver116-1b
    end if
        'ver115-2d changed some reflection graph names
    if msaMode$=modeReflection then   'ver115-1f added impedance and renumbered
        axisGraphData$(0)="S11 Magnitude (dB)" : axisDataType(0)=constGraphS11DB
        axisGraphData$(1)="S11 Phase Angle (Deg)" : axisDataType(1)=constGraphS11Ang
        axisGraphData$(2)="Reflect Coef. Mag (Rho)" : axisDataType(2)=constRho
        axisGraphData$(3)="Reflect Coef. Angle (Theta)" : axisDataType(3)=constTheta
        axisGraphData$(4)="Impedance Mag (Z Mag)" : axisDataType(4)=constImpedMag
        axisGraphData$(5)="Impedance Angle (Z Ang)" : axisDataType(5)=constImpedAng
        axisGraphData$(6)="Series Resistance (Rs)" : axisDataType(6)=constSerR
        axisGraphData$(7)="Series Reactance (Xs)" : axisDataType(7)=constSerReact
        axisGraphData$(8)="Series Capacitance (Cs)" : axisDataType(8)=constSerC
        axisGraphData$(9)="Series Inductance (Ls)" : axisDataType(9)=constSerL
        axisGraphData$(10)="Parallel Resistance (Rp)" : axisDataType(10)=constParR
        axisGraphData$(11)="Parallel Reactance (Xp)" : axisDataType(11)=constParReact
        axisGraphData$(12)="Parallel Capacitance (Cp)" : axisDataType(12)=constParC
        axisGraphData$(13)="Parallel Inductance (Lp)"  : axisDataType(13)=constParL
        axisGraphData$(14)="Return Loss (db)" : axisDataType(14)=constReturnLoss
        axisGraphData$(15)="Reflected Power (%)" : axisDataType(15)=constReflectPower 'ver115-2d
        axisGraphData$(16)="Component Q" : axisDataType(16)=constComponentQ 'ver115-2d
        axisGraphData$(17)="VSWR" : axisDataType(17)=constSWR
        numGraphs=18   'ver116-1b
            'Display "None" only if the other axis is not set to None.
        if (axisNum=1 and Y2DataType<>constNoGraph) or (axisNum=2 and Y1DataType<>constNoGraph) then _
                axisGraphData$(18)="None" : axisDataType(18)=constNoGraph : numGraphs=numGraphs+1 'ver116-1b

        'ver115-2c deleted informational text about R0
    end if

    for i=0 to 5    'ver115-4a
            'Add graphs for each auxiliary item that does not have a blank name
        auxName$=auxGraphDataFormatInfo$(i,0)
        if auxName$<>"" then
            axisGraphData$(numGraphs)=auxName$
            axisDataType(numGraphs)=constAux0+i    'aux constants are sequential starting with constAux0
            numGraphs=numGraphs+1
        end if
    next i
    FillRegularGraphData=numGraphs
end function
*/
}


void hwdInterface::RecalcPlaneExtendAndR0AndRedraw()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub RecalcPlaneExtendAndR0AndRedraw  'Recalculate ReflectArray or S21DataArray for new planeadj, and Reflect array for new R0 and redraw graph
    call gGetMinMaxPointNum pStart, pEnd    'start and stop of points actually in current graph
    for currStep=pStart-1 to pEnd-1     'step numbers are one less than point numbers
        if msaMode$=modeReflection then
            'For reflection, ReflectArray contains intermediate values of db and angle before plane
            'extension was done, and before any R0 transform. We just start with those and recalculate.
            currFreq=ReflectArray(currStep,0)
            f=ReflectArray(currStep,0)
            db=ReflectArray(currStep,constIntermedS11DB)   'intermediate db--saved just for this purpose
            ang=ReflectArray(currStep,constIntermedS11Ang) 'intermed angle
            call ApplyExtensionAndTransformR0 f, db,ang
            ReflectArray(currStep,1)=db      'Enter new db
            ReflectArray(currStep,2)=ang      'Enter new phase
            call CalcReflectDerivedData currStep    'Calculate impedance and other derived quantities.
        else    'transmission
            currFreq=S21DataArray(currStep,0)
            thisPhase=S21DataArray(currStep, 3)    'original phase before plane ext ver116-1b
            call uExtendCalPlane currFreq, thisPhase, planeadj,0    'do adjustment
            S21DataArray(currStep,2)=thisPhase  'Enter new phase
                'Note transform to graph R0 is for Reflection mode only
        end if
    next currStep   'Process next frequency

    call RecalcYValues  'Recalculate graph module Y values from new data
    if autoScaleY1=1 or autoScaleY2=1 then call PerformAutoScale  'autoscale with the new data
    call CreateReferenceTransform
    refreshTracesDirty=1    'Indicate to RefreshGraph that current traces are no good
    refreshGridDirty=1  'To redraw setup info with plane extension value ver115-4j
    call RefreshGraph 0
end sub
*/
}
void hwdInterface::DisplayTitleWindow(int doTwoPort)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub DisplayTitleWindow doTwoPort 'Display dialog to change title of graph, or of two port params.
        'If doTwoPort=1 then we are getting the title for the two port params, which we save
        'but don't print.
    WindowWidth = 280 : WindowHeight = 220
    call GetDialogPlacement 'set UpperLeftX and UpperLeftY ver115-1c
    BackgroundColor$="buttonface"   'ver116-1b changed colors
    ForegroundColor$="black"
    TextboxColor$="white"
    ComboboxColor$="white"

    statictext #title.Instruct, "Enter up to 3 lines of title information.", 40,20,200, 15
    if doTwoPort=0 then statictext #title.Instruct, "(Line 3 gets overridden with date/time at Restart.)", 20,35,260, 15
    textbox #title.t1, 10,50,250, 20
    textbox #title.t2, 10,75,250, 20
    textbox #title.t3, 10,100,250, 20

    button #title.OK, "OK", [titleFinished],UL, 50, 150,50,25
    button #title.Cancel, "Cancel", [titleCancel],UL, 160, 150,50,25

    open "Title" for dialog_modal as #title 'Open title dialog
    #title, "trapclose [titleFinished]"
        'Display existing title info and wait for user to change it
    print #title.t1, gGetTitleLine$(1)
    print #title.t2, gGetTitleLine$(2)
    print #title.t3, gGetTitleLine$(3)

    wait
*/
}

void hwdInterface::titleCancel()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[titleCancel]   'embedded in DisplayTitleWindow
    close #title
    exit sub
*/
}

void hwdInterface::titleFinished()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[titleFinished] 'embedded in DisplayTitleWindow
    #title.t1, "!contents? t1$"
    #title.t2, "!contents? t2$"
    #title.t3, "!contents? t3$"

    if doTwoPort then 'ver116-1b
        call TwoPortSetTitleLine 1, t1$ : call TwoPortSetTitleLine 2, t2$ : call TwoPortSetTitleLine 3, t3$
        call gPrintTitle 1    'Clear old title and print new
        #TwoPortGraphBox$, "flush"
    else
        call gSetTitleLine 1, t1$ : call gSetTitleLine 2, t2$ : call gSetTitleLine 3, t3$
        if msaMode$=modeReflection then 'ver116-1b
            refLastTitle$(1)=t1$ : refLastTitle$(2)=t2$ : refLastTitle$(3)=t3$
        else
            if msaMode$=modeVectorTrans then transLastTitle$(1)=t1$ : transLastTitle$(2)=t2$ : transLastTitle$(3)=t3$
        end if
            'Refresh the graph,but in stick mode this will eliminate
        'the stuck traces, so just reprint the title ver114-7d revised this procedure
        refreshGridDirty=1  'So next refresh will reprint the title
        if isStickMode=0 then
            call RefreshGraph 0 'Refresh in case traces got covered
        else
            call gPrintTitle 1  '1 signals to clear old info first
            'Don't want to flush in stick mode
        end if
    end if
    close #title
end sub
*/
}
void hwdInterface::SelectVideoFilter()
{

  //Select XNarrow, Narrow, Mid or Wide video filter, based on videoFilter$ ver116-1b
  //We also record the capacitance values
  //For speed reasons, port is not global so here we have to use globalPort to identify the parallel port
  //Likewise, control is not global, so we have to recreate it here
  //int control=vars->globalPort+2;
  int videoFilterNum=0;
  for (int i=1; i <= 4; i++)    //match the name to a filter
  {
    if (activeConfig->videoFilterNames[i]==vars->videoFilter)
    {
      videoFilterNum=i;
      break;
    }
  }
  if (videoFilterNum==0)
  {
    QMessageBox::about(0, "Notice", "Invalid video filter selection.");
    videoFilterNum=1;
    vars->videoMagCap=0.001;
    vars->videoPhaseCap=0.001; //default to Wide
  }
  else
  {
    vars->videoMagCap=qMax((float)0.002, activeConfig->videoFilterCaps[videoFilterNum][0]);    //ver116-4f
    vars->videoPhaseCap=qMax((float)0.011, activeConfig->videoFilterCaps[videoFilterNum][1]); //capacitor values--but not zero //ver116-4f
  }
  vars->videoFilterAddress=videoFilterNum-1; //So address runs from 0 to 3.
  vars->videoPhaseTC=10*vars->videoPhaseCap;     //Time constant in ms, based on 10k resistor and cap in uF
  vars->videoMagTC=2.7*vars->videoMagCap;     //Time constant in ms, based on 2.7k resistor and cap in uF
  int switchData=switchLatchBits(vars->freqBand);    //All bits for latch #4, with latch pulse set high ver116-4b
  if (activeConfig->switchHasVideo && vars->suppressHardware==0)   
  {
    switch(activeConfig->cb)
    {
      case 1:  //Original
            //Can//t do this switch on original control board
        break;
      case 2:  //SLIM
/*            out globalPort, switchData //presents switch data to control buffer. Note we don't toggle latch pulse ver116-4b
            out control, globalSTRB          //enters data to control board latch
            out control, globalContClear     //freezes latch data
            out globalPort, 0
            */
        break;
      case 3:  //USB ver116-4h
            QString USBwrbuf = "A20100"+util.ToHex(switchData); //write this string but don't do latch pulse
            usb->usbMSADeviceWriteString(USBwrbuf, 4);
        break;
    }
  }
  autoWaitPrecalculate();

}
void hwdInterface::SelectLatchedSwitches(int desiredFreqBand)
{

  //Sets the switches which (may) require a latch pulse. These are the band switch, forward/reverse and transmit/reflect
  //videoFilterAddress, freqBand, switchFR and switchTR must be properly set before entering
  //For speed reasons, port is not global so here we have to use globalPort to identify the parallel port
  //Likewise, control is not global, so we have to recreate it here
  if (desiredFreqBand==0)
    desiredFreqBand=vars->lastSetBand;   //don't change if auto mode and actual band not yet determined
  vars->lastSetBand=desiredFreqBand;
  if (vars->suppressHardware) return;
  if (activeConfig->switchHasBand==0 && activeConfig->switchHasTR==0 && activeConfig->switchHasFR==0)
    return;    //No physical latched switches
  //int control=vars->globalPort+2;
  int switchData=switchLatchBits(desiredFreqBand);    //All bits, with latch pulse set high
    //We output the required bits with the latch pulse high, then briefly bring the latch
    //pulse low. Hopefully the pulse lasts somewhere between 2 and 200 us.
  switch(activeConfig->cb)
  {
    case hwdOld:  //Original
        //Can't do these switches on original control board
        break;
    case hwdSlim:  //SLIM

        //We output the required bits with the latch pulse high, then briefly bring the latch
        //pulse low. Hopefully the pulse lasts somewhere between 2 and 200 us.
    /*    out globalPort, switchData       //presents switch data to control buffer with latch pulse high
        out control, globalSTRB          //enters data to control board latch #4
        out control, globalContClear     //freezes latch data
        out globalPort, switchData-128   //presents switch data to control buffer with latch pulse low
        for i=1 to 5   //repeat for time delay to lengthen pulse
            out control, globalSTRB          //enters data to control board latch #4; basically just activates latch pulse
        next i
        out control, globalContClear     //freezes latch data
        out globalPort, switchData       //presents switch data to control buffer with latch pulse high
        out control, globalSTRB          //enters data to control board latch #4; basically just de-activates latch pulse
        out control, globalContClear     //freezes latch data
        out globalPort, 0   //zeroes parallel port data bits
            */
            break;
      case hwdUsbV1:
        setSwitches(switchData);
        break;

    //There is substantial delay in the DLL calls so the pulse will likely be at least 100 us.
  }
  //Wait a bit for capacitors to recover a little, to be sure we don't do a lot of latching in a short
  //time and drain the switch capacitors. This should be OK if the capacitors discharge less than 10% during latching
  //and the recharge time constant is one second or less. The switches can probably take several latchings
  //separated by 750 ms during startup, and then one latching every sweep after that.
  //Caller during startup will have to add a little extra delay.
  util.uSleep(250);

}
void hwdInterface::setSwitches(int switchData)
{
  QString USBwrbuf = "A20100"+util.ToHex(switchData);
  QString USBwrbuf2 = "A20100"+util.ToHex(switchData-128);

  usb->usbMSADeviceWriteString(USBwrbuf, 4 );
  usb->usbMSADeviceWriteString(USBwrbuf2, 4 );
  usb->usbMSADeviceWriteString(USBwrbuf, 4 );

}

int hwdInterface::switchLatchBits(int desiredFreqBand)
{
  //Returns value for setting Latch U4 on SLIM control board
  //bit 0    VS0   Video Filter Address, low order bit
  //bit 1    VS1   Video Filter Address, high order bit
  //bit 2    BS0   Band Selection, low order bit
  //bit 3    BS1   Band Selection, high order bit
  //bit 4    FR    DUT Direction, Forward (0) or Reverse
  //bit 5    TR    VNA Selection, Transmission (0) or Reflection
  //bit 6   Spare
  //bit 7    PS    Pulse Start (a/k/a Latch Pulse), common to all latching   relays.
  //Normally high; pulsed low for roughly 2-200 us to trigger relay latching.
  //Note we make PS high, its normal state
  return vars->videoFilterAddress + 4*desiredFreqBand + 16*vars->switchFR + 32*vars->switchTR + 128;
}
void hwdInterface::SelectFilter(int &fbank)
{
  //Select filter indicated by path$, which is in form "Path N" modver116-4j
  //Note that hardware data may have to be recalculated if finalfreq changed, by doing [PartialRestart]
  //filtbank is passed here as fbank so we can change the non-global filtbank
  int filtIndex = (util.Word(vars->path,2)).toInt();    //ver114-4c path number is second word filtIndex will be 1-4
  // fix meif (filtIndex<0 || filtIndex>activeConfig->MSANumFilters) {filtIndex=0; QMessageBox::warning(0,"Error" ,"Invalid filter selection."); }
  calMan->calInstallFile(filtIndex);   //Loads file and sets finalfreq and finalbw
  if (filtIndex<=2)
    vars->FiltA1=0;
  else
    vars->FiltA1=1;             //Set filter address
  if (filtIndex==1 || filtIndex==3)
    vars->FiltA0=0;
  else
    vars->FiltA0=1;
  CommandFilter(fbank); //Calculate fbank and Physically select filter ver116-4j
}
void hwdInterface::CommandFilter(int &fbank)
{
  //filtbank is passed here as fbank so we can change the non-global filtbank
  if (activeConfig->cb == hwdOld)
  {
    fbank = vars->FiltA1*8 + vars->FiltA0*4;
    lpt.CommandFilterOrigCB(fbank);
  }
  else if (activeConfig->cb == hwdSlim)
  {
    fbank = vars->FiltA1*64 + vars->FiltA0*32;
    lpt.CommandFilterSlimCB(fbank);
  }
  else if (activeConfig->cb == hwdUsbV1)
  {
    fbank = vars->FiltA1*64 + vars->FiltA0*32;
    CommandFilterSlimCBUSB(fbank);
  }
}
void hwdInterface::CommandFilterSlimCBUSB(int fbank)
{
  //fbank should be the non-global filtbank
  QString USBwrbuf = "A10300"+util.ToHex(fbank)+util.ToHex(fbank+128)+util.ToHex(fbank);
  usb->usbMSADeviceWriteString(USBwrbuf, 6);
}

void hwdInterface::setDDS1(double freq)
{
  /*
  //this will recalculate DDS1, using the values in the Command DDS 1 Box, and "with DDS Clock at" Box.
  //it will insert the new DDS 1 frequency into the command arrays for all steps, leaving others alone
  //it will initiate a re-command at thisstep (where the sweep was halted)
    //if Original Control Board is used, only the DDS 1 is re-commanded. ver113-4a
    //if SLIM Control Board is used, all 4 modules will be re-commanded. ver113-4a
  //using One Step or Continue will retain the new DDS1 frequency.
  //PLO1 will be non-functional until [Restart] button is clicked. PLL1 will break lock and "slam" to extreme.
  //[Restart] will reset arrays and begin sweeping at step 0. Special Tests Window will not be updated.
  //Signal Generator or Tracking Generator output will not be effected.
  //caution, do not enter a frequency that is higher than 1/2 the masterclock frequency (ddsclock)
  print #special.dds1out, "!contents? dds1out$";   //grab contents of Command DDS 1 Box
  ddsoutput = val(dds1out$) //intended output frequency of DDS 1
  print #special.masclkf, "!contents? msclk$";   //grab contents of "with DDS Clock at" box
  msclk = val(msclk$) //if "with DDS Clock at" box was not changed, this is the real MasterClock frequency
  ddsclock = msclk
  //caution: if ddsoutput >= to .5 ddsclock, the program will error out
  gosub [CreateBaseForDDSarray]//needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
  remember = thisstep //remember where we were when entering this subroutine
  for thisstep = 0 to steps //ver112-2a
  gosub [FillDDS1array]//need thisstep,sw0-sw39,w0-w4,base,ddsclock
  next thisstep //ver112-2a
  thisstep = remember //ver112-2a
  gosub [CreateCmdAllArray] //ver112-2a
  if cb = 0 then gosub [CommandDDS1OrigCB]//will command DDS 1, only
//delver113-4a    if cb = 2 then gosub [CommandDDS1SlimCB]//will command DDS 1, only
  if cb = 2 then gosub [CommandAllSlims]//will command all 4 modules. ver113-4a
  if cb = 3 then gosub [CommandAllSlimsUSB]//will command all 4 modules. ver113-4a //USB:01-08-2010
    */
}
void hwdInterface::ClearAuxData()
{
  //Clear the auxiliary graph data by blanking the graph names
  for (int i=0; i <= 5; i++)
  {
    vars->auxGraphDataFormatInfo[i][0]="";
  }
}

float hwdInterface::Equiv1GFreq(float f, int aBand)
{
  float retVal;
  //Return equivalent 1G frequency for f, based on aBand (0,1,2 or 3)    //ver116-4s
  if (aBand==0)  //ver116-4s
  {
    aBand=1;
    if (f > vars->bandEnd2G)
      aBand=3;
    else if (f > vars->bandEnd1G)
      aBand=2;
  }
  switch(aBand)
  {
    case 1:
        retVal=f;   //1G mode; no conversion necessary
        break;
    case 2:      //2G mode
        retVal = f - LO2;
        break;
    default:   //3G mode
        float IF1 = LO2 - activeConfig->finalfreq;
        retVal=f-2*IF1;
  }
  return retVal;
}
float hwdInterface::ActualSignalFrequency(float f, int aBand)
{
  //Return actual signal frequency for equiv 1G freq f, based on aBand (1,2 or 3) ver116-4s
  switch(aBand)
  {
    case 1:
        return(f);   //1G mode; no conversion necessary
    case 2:      //2G mode
        return (f+LO2);
    default:   //3G mode
        float IF1 = LO2 - activeConfig->finalfreq;
        return(f+2*IF1);
  }
}
void hwdInterface::CalculateAllStepsForLO1Synth()
{
  int haltstep = vars->thisstep; //remember where we were in the sweep when halted
  for (vars->thisstep = 0; vars->thisstep <= vars->steps; vars->thisstep++)
  {
    //ver116-4s moved saving of frequency in other arrays to the place where hardware frequency is calculated.
    //added baseFrequency, which gets added when commanding but does not affect the stored frequencies.
    vars->thisfreq=vars->datatable[vars->thisstep][1];
    LO1 = vars->baseFrequency + vars->thisfreq + LO2 - activeConfig->finalfreq;    //calculates the actual LO1 frequency:thisfreq,LO2,finalfreq are actuals.

    //[CalculateThisStepPLL1]
    appxVCO=LO1;
    vars->reference=activeConfig->appxdds1;
    rcounter=rcounter1;
    if (activeConfig->PLL1mode == 0)
    {
      CreateIntegerNcounter();//needed:appxVCO,reference,rcounter ; creates:ncount,ncounter,fcounter(0)
    }
    //returns with ncount,ncounter,fcounter(0),pdf
    if (activeConfig->PLL1mode == 1)
    {
      CreateFractionalNcounter();//needed:appxVCO,reference,rcounter ; creates:ncount,ncounter,fcounter,pdf
    }
    //returns with ncount,ncounter,fcounter,pdf
    vars->dds1output = pdf * rcounter;    //actual output of DDS1(input Ref to PLL1)
    if (activeConfig->PLL1mode == 1)
    {
      AutoSpur();//needed:LO2,finalfreq,dds1output,rcounter1,finalbw,appxdds1,fcounter,ncounter ver111-8
    }
    //[AutoSpur] is a continuation of [CreateFractionalNcounter], used only in MSA when PLL 1 is Fractional
    //returns with possibly new: ncounter,fcounter,pdf,dds1output
    if (activeConfig->PLL1mode == 1)
    {
      ManSpur();
    }
    //[ManSpur] is a continuation of [CreateFractionalNcounter], used only in MSA when PLL 1 is Fractional
    //if Spur Test Button On, will return with new ncounter,fcounter,pdf,dds1output
    CreatePLL1N();//needs:ncounter,fcounter,PLL1mode,PLL1 ; creates PLL NBuffer N0-Nx
    FillPLL1array(vars->thisstep);//need:N0-Nx,pdf,dds1output,LO1,ncount,ncounter,Fcounter,Acounter,Bcounter;creates samePLL1
    //[endCalculateThisStepPLL1]
    //[CalculateThisStepDDS1]//need:dds1output,masterclock,appxdds1,dds1filbw
    ddsoutput = vars->dds1output;
    ddsclock = activeConfig->masterclock;
    if (vars->dds1output-activeConfig->appxdds1 > activeConfig->dds1filbw/2)
    {
      util.beep();
      PrintMessage("DDS1output too high for filter");
      RequireRestart();
      return;
    }
    if (activeConfig->appxdds1 - vars->dds1output > activeConfig->dds1filbw/2)
    {
      util.beep();
      PrintMessage("DDS1output too low for filter");
      RequireRestart();
      return;
    }
    CreateBaseForDDSarray();//needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
    FillDDS1array(vars->thisstep);//need thisstep,sw0-sw39,w0-w4,base,ddsclock
  }
  vars->thisstep = haltstep; //return to the step in the sweep, where we halted, if needed
}

void hwdInterface::CalculateAllStepsForLO3Synth()
{
  //for hybrid, and orig (fixed freq) TG
  //if TGtop = 0 then skip all this (return), actually we should not have even entered this subroutine.
  int haltstep = vars->thisstep; //remember where we were in the sweep when halted
  //float LO3;
  for (vars->thisstep = 0; vars->thisstep <= vars->steps; vars->thisstep++)
  {
    //Frequencies have been pre-calculated in the graphing module via gGenerateXValues
    vars->thisfreq = vars->datatable[vars->thisstep][1];
    int thisBand = vars->datatable[vars->thisstep][4];
    float TrueFreq = ActualSignalFrequency(vars->thisfreq, thisBand) + vars->baseFrequency;
    //baseFrequency gets added when commanding but does not affect the stored frequencies.
    //if FreqMode=1 then thisfreq=TrueFreq else thisfreq=Equiv1GFreq(TrueFreq)  //ver115-1c get equivalent 1G frequency ver115-1d delver116-4s

    if (activeConfig->TGtop == 1)
    {
      LO3 = LO2 - activeConfig->finalfreq - vars->offset; //for orig, fixed freq TG  ver111-15a
    }
    //or LO3 = LO1 - thisfreq - offset
    //ver115-1c rearranged the following if... block and added the FreqMode=3 test
    if (activeConfig->TGtop == 2 && vars->gentrk == 1)
    {
      if (vars->normrev == 0)
      {
        if (thisBand==3)
        {
          LO3 = TrueFreq + vars->offset - LO2;    //Mode 3G sets LO3 differently
        }
        else
        {
          LO3 = LO2 + vars->thisfreq + vars->offset;   //for new TG, Trk Gen mode, normal
        }
      }
      if (vars->normrev == 1)
      {
        float revfreq;
        //Frequencies have been pre-calculated in the graphing module via gGenerateXValues
        //We can just retrieve them in reverse order.
        qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
       // fix me  TrueFreq = gGetPointXVal(vars->steps-vars->thisstep+1) + vars->baseFrequency;    //Point number is 1 greater than step number ver116-4L
        if (thisBand==1)
        {
          revfreq=TrueFreq;
        }
        else
        {
          revfreq=Equiv1GFreq(TrueFreq, thisBand);  //get equiv 1G freq
        }
        if (thisBand==3)
        {
          LO3 = TrueFreq + vars->offset - LO2;  //Mode 3G sets LO3 differently
        }
        else
        {
          LO3 = LO2 + revfreq + vars->offset; //for new TG, Trk Gen mode, normal
        }
      }
    }

    if (activeConfig->TGtop == 2 && vars->gentrk == 0)
    {
      //for new TG, Sig Gen mode ver116-4p
      //We will try to produce sgout, either by LO3-LO2(for 0 to LO2), LO3 (LO2 to 2*LO2) or LO3+LO2 (above 2*LO2)
      if (vars->sgout <= LO2)
      {
        LO3=vars->sgout+LO2;
      }
      else if (vars->sgout>2*LO2)
      {
        LO3=vars->sgout-LO2;
      }
      else
      {
        LO3=vars->sgout;
      }
    }
    //[CalculateThisStepPLL3]
    appxVCO=LO3;
    vars->reference=activeConfig->appxdds3;
    rcounter=rcounter3;
    if (activeConfig->appxdds3 == 0)
    {
      vars->reference=activeConfig->masterclock; //for orig, fixed freq TG with no DDS3 steering. ver111-17
    }
    if (activeConfig->PLL3mode == 0)
    {
      CreateIntegerNcounter();//needed:appxVCO,reference,rcounter ; creates:ncount,ncounter,fcounter(0)
    }
    //returns with ncount,ncounter,fcounter(0),pdf
    if (activeConfig->PLL3mode == 1)
    {
      CreateFractionalNcounter();//needed:appxVCO,reference,rcounter ; creates:ncount,ncounter,fcounter,pdf
    }
    //returns with ncount,ncounter,fcounter,pdf
    vars->dds3output = pdf * rcounter;    //actual output of DDS3(input Ref to PLL3)
    CreatePLL3N();//needs:ncounter,fcounter,PLL3mode,PLL3 ; creates PLL NBuffer N0-Nx
    FillPLL3array(vars->thisstep);//need thisstep,N0thruN23,pdf3(40),dds3output(41),samePLL3(42)see dim PLL3array for slot info //ver111-14
    //[endCalculateThisStepPLL3]
    //[CalculateThisStepDDS3]//need:dds3output,masterclock,appxdds3,dds3filbw
    if (activeConfig->appxdds3 != 0)
    {
      ddsoutput = vars->dds3output;
      ddsclock = activeConfig->masterclock;
      if (vars->dds3output-activeConfig->appxdds3 > activeConfig->dds3filbw/2)
      {
        util.beep();
        PrintMessage("DDS3 output too high for filter");
        RequireRestart();
        return;
      }

      if (activeConfig->appxdds3-vars->dds3output > activeConfig->dds3filbw/2)
      {
        util.beep();
        PrintMessage("DDS3output too low for filter");
        RequireRestart();
        return;
      }
      CreateBaseForDDSarray();//needed:ddsoutput,ddsclock ; creates: base,sw0thrusw39,w0thruw4
      FillDDS3array(vars->thisstep);//need thisstep,sw0-sw39,w0-w4,base,ddsclock  ver111-15
    }
    //del.ver112-2b phaarray(thisstep,0) = 0 //this will set all pdmstates, to 0 //ver112-1a
    vars->phaarray[vars->thisstep][0] = 0; //this will set all pdmstates, to 0 //undeleted, ver113-7e
  }
  vars->thisstep = haltstep; //return to the step in the sweep, where we halted, if needed
  vars->lastpdmstate = 2; //this will guarantee that the PDM will get commanded //ver112-1a
}

void hwdInterface::FillPLL1array(int step)
{
  //need thisstep,N0thruN23,pdf1(40),dds1output(41),samePLL1(42)see dim PLL1array for slot info 'ver111-1
  if (activeConfig->cb == hwdUsbV1)
  {
    unsigned long result;
    //          if USBdevice <> 0 then CALLDLL #USB, "UsbMSADevicePopulateDDSArrayBitReverse", USBdevice as long, ptrSPLL1Array as ulong
    //                  , Int64N as ptr, thisstep as short, 40 as short, result as boolean 'USB:11-08-2010
    usb->usbMSADevicePopulateDDSArrayBitReverse((qint64 *)usb->ptrSPLL1Array, usb->int64N, vars->thisstep, 40, &result);
  }
  else
  {
    //reversed sequence for N23 to be first.
    pll_1[step].array[23] = n.N0;
    pll_1[step].array[22] = n.N1;
    pll_1[step].array[21] = n.N2;
    pll_1[step].array[20] = n.N3;
    pll_1[step].array[19] = n.N4;
    pll_1[step].array[18] = n.N5;
    pll_1[step].array[17] = n.N6;
    pll_1[step].array[16] = n.N7;
    pll_1[step].array[15] = n.N8;
    pll_1[step].array[14] = n.N9;
    pll_1[step].array[13] = n.N10;
    pll_1[step].array[12] = n.N11;
    pll_1[step].array[11] = n.N12;
    pll_1[step].array[10] = n.N13;
    pll_1[step].array[9] = n.N14;
    pll_1[step].array[8] = n.N15;
    pll_1[step].array[7] = n.N16;
    pll_1[step].array[6] = n.N17;
    pll_1[step].array[5] = n.N18;
    pll_1[step].array[4] = n.N19;
    pll_1[step].array[3] = n.N20;
    pll_1[step].array[2] = n.N21;
    pll_1[step].array[1] = n.N22;
    pll_1[step].array[0] = n.N23;
  }
  pll_1[step].pdf = pdf;
  pll_1[step].freq = LO1;
  pll_1[step].ncounter = 1040; //ncounter;
  pll_1[step].fcounter = fcounter;
  pll_1[step].Acounter = 16;//Acounter;
  pll_1[step].Bcounter = Bcounter;

}

void hwdInterface::FillPLL3array(int step)
{
//need thisstep,N0thruN23,pdf3(40),dds3output(41),samePLL3(42)see dim PLL3array for slot info 'ver111-14
  if (activeConfig->cb == hwdUsbV1)
  {
    unsigned long result;
  //   if USBdevice <> 0 then CALLDLL #USB, "UsbMSADevicePopulateDDSArrayBitReverse", USBdevice as long
    //, ptrSPLL3Array as ulong, Int64N as ptr, thisstep as short, 40 as short, result as boolean 'USB:11-08-2010
    usb->usbMSADevicePopulateDDSArrayBitReverse((qint64 *)usb->ptrSPLL3Array, usb->int64N, vars->thisstep, 40, &result);
  }
  else
  {
    //reversed sequence for N23 to be first. ver111-31a
    pll_3[step].array[23] = n.N0;
    pll_3[step].array[22] = n.N1;
    pll_3[step].array[21] = n.N2;
    pll_3[step].array[20] = n.N3;
    pll_3[step].array[19] = n.N4;
    pll_3[step].array[18] = n.N5;
    pll_3[step].array[17] = n.N6;
    pll_3[step].array[16] = n.N7;
    pll_3[step].array[15] = n.N8;
    pll_3[step].array[14] = n.N9;
    pll_3[step].array[13] = n.N10;
    pll_3[step].array[12] = n.N11;
    pll_3[step].array[11] = n.N12;
    pll_3[step].array[10] = n.N13;
    pll_3[step].array[9] = n.N14;
    pll_3[step].array[8] = n.N15;
    pll_3[step].array[7] = n.N16;
    pll_3[step].array[6] = n.N17;
    pll_3[step].array[5] = n.N18;
    pll_3[step].array[4] = n.N19;
    pll_3[step].array[3] = n.N20;
    pll_3[step].array[2] = n.N21;
    pll_3[step].array[1] = n.N22;
    pll_3[step].array[0] = n.N23;
  }
  pll_3[step].pdf = pdf;
  pll_3[step].freq = LO3;
  pll_3[step].ncounter = ncounter;
  pll_3[step].fcounter = fcounter;
  pll_3[step].Acounter = Acounter;
  pll_3[step].Bcounter = Bcounter;

}

void hwdInterface::FillDDS1array(int step)
{
  //need thisstep,sw0-sw39,w0-w4,base,ddsclock //ver111-12
  if (activeConfig->cb == hwdUsbV1)
  {
    unsigned long result;
    //  if USBdevice <> 0 then CALLDLL #USB, "UsbMSADevicePopulateDDSArray", USBdevice as long, ptrSDDS1Array as ulong, Int64SW as ptr, thisstep as short, result as boolean //USB:11-08-2010
    usb->usbMSADevicePopulateDDSArray((qint64 *)usb->ptrSDDS1Array, usb->int64SW, vars->thisstep, &result);

  }
  else
  {
    dds_1[step].array[0] = sw0;
    dds_1[step].array[1] = sw1;
    dds_1[step].array[2] = sw2;
    dds_1[step].array[3] = sw3;
    dds_1[step].array[4] = sw4;
    dds_1[step].array[5] = sw5;
    dds_1[step].array[6] = sw6;
    dds_1[step].array[7] = sw7;
    dds_1[step].array[8] = sw8;
    dds_1[step].array[9] = sw9;
    dds_1[step].array[10] = sw10;
    dds_1[step].array[11] = sw11;
    dds_1[step].array[12] = sw12;
    dds_1[step].array[13] = sw13;
    dds_1[step].array[14] = sw14;
    dds_1[step].array[15] = sw15;
    dds_1[step].array[16] = sw16;
    dds_1[step].array[17] = sw17;
    dds_1[step].array[18] = sw18;
    dds_1[step].array[19] = sw19;
    dds_1[step].array[20] = sw20;
    dds_1[step].array[21] = sw21;
    dds_1[step].array[22] = sw22;
    dds_1[step].array[23] = sw23;
    dds_1[step].array[24] = sw24;
    dds_1[step].array[25] = sw25;
    dds_1[step].array[26] = sw26;
    dds_1[step].array[27] = sw27;
    dds_1[step].array[28] = sw28;
    dds_1[step].array[29] = sw29;
    dds_1[step].array[30] = sw30;
    dds_1[step].array[31] = sw31;
    dds_1[step].array[32] = sw32;
    dds_1[step].array[33] = sw33;
    dds_1[step].array[34] = sw34;
    dds_1[step].array[35] = sw35;
    dds_1[step].array[36] = sw36;
    dds_1[step].array[37] = sw37;
    dds_1[step].array[38] = sw38;
    dds_1[step].array[39] = sw39;
  }
  dds_1[step].array[40] = w0;
  dds_1[step].array[41] = w1;
  dds_1[step].array[42] = w2;
  dds_1[step].array[43] = w3;
  dds_1[step].array[44] = w4;
  dds_1[step].base = base; //base is decimal command
  dds_1[step].freq = base*ddsclock / pow(2.0,32); //actual dds 1 output freq
}

void hwdInterface::FillDDS3array(int step)
{
  //qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;

    if (activeConfig->cb == hwdUsbV1)
    {
      unsigned long result;
      usb->usbMSADevicePopulateDDSArray((qint64 *)usb->ptrSDDS3Array, usb->int64SW, vars->thisstep, &result);
    }
/*
    else 'USB:05/12/2010
        DDS3array(thisstep,0) = sw0:DDS3array(thisstep,1) = sw1
        DDS3array(thisstep,2) = sw2:DDS3array(thisstep,3) = sw3
        DDS3array(thisstep,4) = sw4:DDS3array(thisstep,5) = sw5
        DDS3array(thisstep,6) = sw6:DDS3array(thisstep,7) = sw7
        DDS3array(thisstep,8) = sw8:DDS3array(thisstep,9) = sw9
        DDS3array(thisstep,10) = sw10:DDS3array(thisstep,11) = sw11
        DDS3array(thisstep,12) = sw12:DDS3array(thisstep,13) = sw13
        DDS3array(thisstep,14) = sw14:DDS3array(thisstep,15) = sw15
        DDS3array(thisstep,16) = sw16:DDS3array(thisstep,17) = sw17
        DDS3array(thisstep,18) = sw18:DDS3array(thisstep,19) = sw19
        DDS3array(thisstep,20) = sw20:DDS3array(thisstep,21) = sw21
        DDS3array(thisstep,22) = sw22:DDS3array(thisstep,23) = sw23
        DDS3array(thisstep,24) = sw24:DDS3array(thisstep,25) = sw25
        DDS3array(thisstep,26) = sw26:DDS3array(thisstep,27) = sw27
        DDS3array(thisstep,28) = sw28:DDS3array(thisstep,29) = sw29
        DDS3array(thisstep,30) = sw30:DDS3array(thisstep,31) = sw31
        DDS3array(thisstep,32) = sw32 'x4 multiplier
        DDS3array(thisstep,33) = sw33 'control bit
        DDS3array(thisstep,34) = sw34 'power down bit
        DDS3array(thisstep,35) = sw35 '35-39 are Phase
        DDS3array(thisstep,36) = sw36:DDS3array(thisstep,37) = sw37
        DDS3array(thisstep,38) = sw38:DDS3array(thisstep,39) = sw39
    end if 'USB:05/12/2010
    DDS3array(thisstep,40) = w0 'word 0, 8 bits, mult, control and phase
    DDS3array(thisstep,41) = w1 'word 1, 8 bits
    DDS3array(thisstep,42) = w2 'word 2, 8 bits
    DDS3array(thisstep,43) = w3 'word 3, 8 bits
    DDS3array(thisstep,44) = w4 'word 4, 8 bits
    DDS3array(thisstep,45) = base 'base is decimal command
    DDS3array(thisstep,46) = base*ddsclock/2^32 'actual dds 3 output freq
    return
*/
    dds_3[step].array[40] = w0;
    dds_3[step].array[41] = w1;
    dds_3[step].array[42] = w2;
    dds_3[step].array[43] = w3;
    dds_3[step].array[44] = w4;
    dds_3[step].base = base; //base is decimal command
    dds_3[step].freq = base*ddsclock / pow(2.0,32); //actual dds 1 output freq
}
void hwdInterface::CreateCmdAllArray()
{
  //for SLIM CB only //ver-31b
  //a DDS serial command, will begin with LSB (W0), thru MSB (W31), ending with Phase bit 4 (W39)
  //a PLL serial command, will begin with MSB (N23), thru LSB (N0, the address bit)

  if (activeConfig->cb != hwdUsbV1)
  {
    for (int index = 0; index <= vars->steps; index++)
    {
      for (int clmn = 0; clmn <= 15; clmn++)
      {
        lpt.cmdAllArray[index].array[clmn] = dds_1[index].array[clmn]*4
            + dds_3[index].array[clmn]*16;
      }
      for (int clmn = 16; clmn <= 39; clmn++)
      {
        lpt.cmdAllArray[index].array[clmn] = pll_1[index].array[clmn-16]*2
            + dds_1[index].array[clmn]*4
            + pll_3[index].array[clmn-16]*8
            + dds_3[index].array[clmn]*16;
      }
    }
  }
  else
  {
    /*
        if USBdevice <> 0 then CALLDLL #USB, "UsbMSADevicePopulateAllArray",
USBdevice as long,
steps as short,
40 as short, _
0 as long,
ptrSPLL1Array as long,
ptrSDDS1Array as long,
ptrSPLL3Array as long, _
ptrSDDS3Array as long,
0 as long,
0 as long,
0 as long, _
result as boolean //USB:11-08-2010
            */
    unsigned long result;
    usb->usbMSADevicePopulateAllArray(vars->steps,
                                      40,
                                      0,
                                      (qint64 *)usb->ptrSPLL1Array,
                                      (qint64 *)usb->ptrSDDS1Array,
                                      (qint64 *)usb->ptrSPLL3Array,
                                      (qint64 *)usb->ptrSDDS3Array,
                                      0,
                                      0,
                                      0,
                                      &result);
  }
}
void hwdInterface::CommandPLL(int step)
{
  //comes here during PLL R Initializations and PLL 2 N command ver111-28
  if (activeConfig->cb == hwdOld)
  {
    lpt.CommandPLLorig();
  }
  else if (activeConfig->cb == hwdSlim)
  {
    lpt.CommandPLLslim(filtbank, datavalue, vars->phaarray[step][0]*64, &n, levalue);
  }
  else if (activeConfig->cb == hwdUsbV1)
  {
    CommandPLLslimUSB();
  }
}
void hwdInterface::CommandPLLslimUSB()
{
  unsigned long result;
  usb->usbMSADeviceWriteInt64MsbFirst((short)161, usb->int64N, (short)24, (short)1, filtbank, datavalue, &result);
  int pdmcommand = vars->phaarray[vars->thisstep][0]*64; //do not disturb PDM state, this may be used during Spur Test

  QString USBwrbuf = "A30200"+util.ToHex(pdmcommand + levalue)+util.ToHex(pdmcommand);
  usb->usbMSADeviceWriteString(USBwrbuf,5);
  return;
}

void hwdInterface::DetermineModule(int step)
{
  //All "glitchXX's" are "0" when entering this subroutine. Either from "fresh RUN" or [WaitStatement]
  //if a module is not present, or if it doesn't need commanding, return with it's "glitchXX = 0"

  vars->dds1output = dds_1[step].freq;
  if (vars->dds1output != vars->lastdds1output)
  {
    //dds 1 is same, don't waste time commanding

    glitchd1 = 1;
    vars->lastdds1output = vars->dds1output;
  }
  ncounter1= pll_1[step].ncounter;
  fcounter1=pll_1[step].fcounter;
  if (!(ncounter1==lastncounter1 && fcounter1==lastfcounter1))
  {
    //dont waste time commanding
    glitchp1 = 1; //add 1 msec delay.
    lastncounter1=ncounter1;
    lastfcounter1=fcounter1;
  }

  if  (activeConfig->TGtop == 0)
  {
    return; //there is no PLL 3, no DDS 3,and no PDM for VNA
  }

  ncounter3=pll_3[step].ncounter;
  fcounter3=pll_3[step].fcounter;
  if (!(ncounter3==lastncounter3 && fcounter3==lastfcounter3))
  {
    //don't waste time commanding
    glitchp3 = 1; //add 1 msec delay
    lastncounter3=ncounter3;
    lastfcounter3=fcounter3;
  }

  if (activeConfig->appxdds3 != 0)
    //if 0, there is no DDS3, but, there can be VNA
    vars->dds3output = dds_3[step].freq;
  if (vars->dds3output != vars->lastdds3output)
  {
    //dds 3 is same, don//t waste time commanding
    glitchd3 = 1;
    vars->lastdds3output = vars->dds3output;
  }

  if (vars->suppressPhase || vars->msaMode==modeSA || vars->msaMode==modeScalarTrans)
    return; // not in VNA mode, skip the PDM
  pdmcmd = vars->phaarray[step][0];
  if (pdmcmd == vars->lastpdmstate)
    return; //don't waste time commanding
  VideoGlitchPDM();
}

void hwdInterface::CommandPDMonly()
{
  //ver111-28
  if (activeConfig->cb == hwdOld)
    lpt.CommandPDMOrigCB();
  else if (activeConfig->cb == hwdSlim)
    lpt.CommandPDMSlimCB();
  else if (activeConfig->cb == hwdUsbV1)
    CommandPDMSlimUSB();
  return;
}
void hwdInterface::CommandPDMSlimUSB()
{
  //USB:01-08-2010
  int i = vars->phaarray[vars->thisstep][0]*64;
  QString USBwrbuf = "A30300"+util.ToHex(i)+util.ToHex(i+32)+util.ToHex(i);
  usb->usbMSADeviceWriteString(USBwrbuf,6);
  vars->lastpdmstate=vars->phaarray[vars->thisstep][0];   //ver114-6c
}

void hwdInterface::CommandAllSlimsUSB()
{
  unsigned long result;
  if (activeConfig->cb != hwdUsbV1)
  {
    return;
  }
  //(send data and clocks without changing Filter Bank)
  if (vars->thisstep == 0)
  {
    data.filtbank = filtbank;
    data.latches = le1 + fqud1 + le3 + fqud3;
    //data.latches = le1 + fqud1;
    data.pdmcmdmult = 64;
    data.pdmcmdadd = 32;
  }
  data.pdmcommand = vars->phaarray[vars->thisstep][0];
  data.thisstep = vars->thisstep;
  usb->usbMSADeviceAllSlimsAndLoadStruct((unsigned short *)&data, &result);
  vars->lastpdmstate=vars->phaarray[vars->thisstep][0];

}

void hwdInterface::finished()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  //this is the end of the software, close any open window
  if (special == 1) then close #special    //close out Special Tests window. ver113-5a
  if (varwindow == 1 then close #varwin   //close out variable window
  if (datawindow == 1 then close #datawin //close out data window. ver113-5a
  if lptwindow == 1 then close #LPTwindow //lpt  ver116-4b
  if calManWindHndl$<>"" then close #calManWindHndl$   //close window for calibration manager, SEWcal3 ver113-7g
  if configWindHndl$<>"" then close #configWindHndl$    //close window for configuration manager, SEWcal3 ver113-7g
  if axisPrefHandle$<>"" then close #axisPrefHandle    Close axis preference window; it//s a modal dialog so this should not happen
  if crystalListHndl$<> "" then close #crystalListHndl$
  if crystalWindHndl$<> "" then close #crystalWindHndl$   //ver115-5f
  if componentWindHndl$<>"" then close #componentWindHndl$
  if twoPortWinHndl$<>"" then close #twoPortWinHndl$ //ver116-1b
  call smithFinished ""      //ver115-1b
  for i=1 to multiscanMaxNum
      thisWindowHndl$= multiscanWindowHandlesLB$(i)
      if thisWindowHndl$<>"" then close #thisWindowHndl$    //Graph windows for multiscans ver115-8c
  next i
  CallDLL #DLL.OLE, "CoUninitialize", ret as void  //To avoid tooltips crash in file dialog ver116-4q
  Close #DLL.OLE //ver116-4q
  */
  // close USB interface if it was active
  usb->usbCloseInterface();
  //close #handle   //close out graph window
  //ret = GlobalFree(hSAllArray) //USB:01-08-2010
  //QCoreApplication::exit(0);
}

void hwdInterface::SpecialTests()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[SpecialTests]'ver111-36b
    if haltsweep=1 then gosub [CleanupAfterSweep]    'ver114-4f
    if special=1 then goto [CloseSpecial]
    WindowWidth = 200
    WindowHeight = 300
    UpperLeftX = DisplayWidth-WindowWidth-20    'ver114-4f
    UpperLeftY = 5    'ver114-4f
    BackgroundColor$ = "buttonface" 'color changed by ver116-4k
    ForegroundColor$ = "black"
    TextboxColor$ = "white"
    button #special.DDS1, "Command DDS 1", [CommandDDS1], UL, 5, 5, 100, 20
    textbox #special.dds1out, 105, 5, 75, 20   'create DDS 1 frequency output box
    statictext #special.dds1txt, "with DDS Clock at ", 5, 30, 100, 15
    textbox #special.masclkf, 105, 30, 75, 20   'create master clock frequency box
    button #special.DDS3, "Command DDS 3", [CommandDDS3], UL, 5, 55, 100, 20 'ver111-38a
    textbox #special.dds3out, 105, 55, 75, 20   'create DDS 1 frequency output box
    button #special.dds3track, "DDS 3 Track", [DDS3Track], UL, 5, 105, 75, 20 'ver111-39d
    statictext #special.dds3trktxt, "0-32", 80, 107, 25, 15 'ver112-2c
    button #special.dds1track, "DDS 1 Sweep", [DDS1Sweep], UL, 115, 105, 75, 20 'ver112-2c
    if msaMode$<>modeSA and msaMode$<>modeScalarTrans then
        button #special.pdminv, "Change PDM", [ChangePDM], UL, 5, 130, 90, 20 'ver115-4d
        button #special.insert, "Sync Test PDM", [SyncTestPDM], UL, 5, 155, 90, 20 'ver112-2f
        button #special.prevnalin, "Preset Phase Linearity", [PresetVNAlin], UL, 5, 180, 150, 20 'ver112-2f ver114-5n ver114-8c
    end if
    button #special.lpttest, "LPT Port Test", [LPTportTest], UL, 5, 220, 100, 20 'lpt ver116-4b
    button #special.cftest, "Cavity Filt Test", [CavityFilterTest], UL, 5, 250, 100, 20 'cav ver116-4c

    open "Special Tests Window" for dialog as #special:special = 1
    #special, "font ms_sans_serif 9" 'ver116-4m
    print #special.dds1out, "";DDS1array(thisstep,46) 'insert DDS1 output frequency at thisstep 'ver112-2d
    print #special.masclkf, "";masterclock 'insert masterclock frequency
    print #special.dds3out, "";DDS3array(thisstep,46) 'insert DDS3 output frequency at thisstep 'ver112-2d
    print #special, "trapclose [CloseSpecial]" 'goto [CloseSpecial] if xit is clicked
    wait
*/
}

void hwdInterface::CloseSpecial()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CloseSpecial]'ver111-36b
    SyncSweep = 0 'ver112-2b
    setpdm = 0 'makes sure the PDM returns to automatic operation ver112-2a
    convdatapwr = 0 'ver112-2b
    vnalintest = 0 'ver112-2f
    test = 0    'ver112-2g
    if cftest = 1 then gosub [CloseCavityFilterTest] 'cav ver116-4c
    close #special:special = 0     'close out Special Tests window
    call RequireRestart 'ver116-4c
    if returnflag = 1 then return   'ver112-2f
    wait
*/
}

void hwdInterface::CavityFilterTest()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'comes here when Cavity Filter Test button is clicked
    if cftest = 1 then wait
    cftest = 1
    enterPLL2phasefreq = PLL2phasefreq
    PLL2phasefreq = .1
goto [Restart]
*/
}
void hwdInterface::CloseCavityFilterTest()
{
  //will come here when Special Tests Window is closed
  cftest = 0;
  activeConfig->PLL2phasefreq = enterPLL2phasefreq;
}

void hwdInterface::CommandLO2forCavTest()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CommandLO2forCavTest]
    appxVCO = finalfreq + PLL1array(thisstep,43)
    reference = masterclock
    rcounter = rcounter2
    gosub [CreateIntegerNcounter]'needs:appxVCO,reference,rcounter ; creates:ncounter,fcounter(0)
    ncounter2 = ncounter:fcounter2 = fcounter
    gosub [CreatePLL2N]'needs:ncounter,fcounter,PLL2 ; returns with Bcounter,Acounter, and N Bits N0-N23
    Bcounter2=Bcounter: Acounter2=Acounter
    LO2=((Bcounter*preselector)+Acounter+(fcounter/16))*pdf2 'actual LO2 frequency  'ver115-1c LO2 is now global
    'CommandPLL2N
    Jcontrol = SELT : LEPLL = 8
    datavalue = 16: levalue = 16 'PLL2 data and le bit values ver111-28
    gosub [CommandPLL]'needs:N23-N0,control,Jcontrol,port,contclear,LEPLL ; commands N23-N0,old ControlBoard ver111-5
return
*/
}


//===================START CONTEXTS MODULE=======================

//-------------Explanation of Contexts-------------  ver114-2d
//A context file or string contains a series of lines (separated by chr$(13) in strings) containing
//information on one or more types of contexts. Data for a given type begins with the line
// "StartContext Name", where Name is the name of the context type. It ends with the end of the
//file or string, or with the line "EndContext". Routines that restore individual types do not necessarily expect
//the file or string to begin with the StartContext xxx line, which should have been processed by the caller.
//

//Some variables are stored both inside the graphing module and, under a different name, outside it.
//In those cases, only one version needs to be saved, but both versions need to be restored.
//-------------------------------------------------

//ver114-2c added SweepContext$



QFile * hwdInterface::OpenContextFile(QString fName, QString inout)
{
  QFile *fFile = new QFile(fName);
  if (!fFile->exists())
  {
    QString ggg = "file not found";
  }
  if (inout.trimmed().toUpper() == "IN")
  {
    if (!fFile->open(QFile::ReadOnly))
    {
      delete fFile;
      fFile = NULL;
    }
  }
  else
  {
    if (!fFile->open(QFile::WriteOnly | QFile::Text))
    {
      delete fFile;
      fFile = NULL;
    }
  }
/*
  //Open specified file. Return handle or  ""
  //fName$ contains complete path and file name
  On Error goto [noFile]
  inout$=Upper$(Trim$(inout$))
  if inout$="IN" then open fName$ for input as #contextFile else open fName$ for output as #contextFile
  OpenContextFile$="#contextFile"
  exit function
[noFile]
      OpenContextFile$=""
      */
  return fFile;
}

void hwdInterface::FindPeakSteps(int dataType, int startStep, int endStep, int &minNum, int &maxNum, int &minY, int &maxY)    //find positive and negative peak
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*     //Search includes steps from s1 to s2, inclusive. dataType indicates the graph data to search (e.g. constRho),
    //so we don//t necessarily search the current graph.
    //minNum and maxNum will be set to the steps where the peaks occur; minY and maxY will be the peak values
    //Peak includes max and min
    //The result may be a non-integer; if two or more successive points have the same peak value, then the peak
    //step will be in the middle.
    minPeakEnded=1 : maxPeakEnded=1
    saveY1Type=Y1DataType : Y1DataType=dataType //Values will be calculated for Y1
    saveY2Type=Y2DataType : Y2DataType=constNoGraph

    for i=startStep to endStep
        call CalcGraphData i,y, dum,0   //Calculate Y1 data type from data arrays
        if i=p1 then
            minNumStart=p1: maxNumStart=p1
            minNumEnd=p1: maxNumEnd=p1
            minY=y : maxY=y //Start with values at first point
        else
            //See if peak is found. Once found, so long as we remain at that level, continue
            //to record maxPeakEnded
            if y>maxY then maxY=y : maxNumStart=i : maxPeakEnded=0 //new max
            if maxPeakEnded=0 and y>=maxY then maxNumEnd=i else maxPeakEnded=1
            if y<minY then minY=y : minNumStart=i : minNumEnd=i : minPeakEnded=0    //new min
            if minPeakEnded=0 and y<=minY then minNumEnd=i else minPeakEnded=1
        end if
    next i
    //Here the min or max start and end numbers indicate where the peak started and ended; we consider
    //the actual peak to be in the middle.
    maxNum=(maxNumEnd+maxNumStart)/2
    minNum=(minNumEnd+minNumStart)/2
    Y1DataType=saveY1Type
    Y2DataType=saveY2Type
*/
}



void hwdInterface::frontEndInterpolateToScan()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
 //Interpolate from frontEndCalData to frontEndCorrection based on current scan points ver115-9d

    //We copy data to the interpolation arrays, interpolate, and copy the results where we want them
    call intSetMaxNumPoints max(frontEndCalNumSteps, globalSteps)+1  //Be sure we have room

    call intClearSrc : call intClearDest
    for i=1 to frontEndCalNumPoints //copy cal table to intSrc
        call intAddSrcEntry frontEndCalData(i,0),frontEndCalData(i,1),0
    next i
    for i=1 to globalSteps+1
        call intAddDestFreq gGetPointXVal(i)   //Install frequencies in intDest
    next i

    favorFlat=1 : isAngle=0
        //1 means do mag; first 0 means don't do phase; final 0 means not phase correction ver116-1b
    call intCreateCubicCoeffTable 1,0,isAngle, favorFlat,0    //Get coefficients for cubic interp of front end cal //ver116-1b

    //0 means data is not polar , 1 means do cubic interp, 1 means do mag only
    call intSrcToDest 0, 1, 1

    for i=0 to globalSteps  //put the data where we want it
        call intGetDest i+1,f, m, p
        frontEndCorrection(i)=m //mag from interp results
    next i
    */
}
void hwdInterface::CopyModeDataToVNAData(int doIntermed)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
'Copy current data to VNAData, VNATitle$, etc. 'ver116-4a
            'We save all steps, even if they have not gathered any info yet
            'If doIntermed=1, we copy the intermediate (pre-R0 and plane ext) data for S21 or S11.
    redim VNAData(globalSteps, 2)   'Allow for freq, db and ang
    for i=0 to globalSteps
        select case msaMode$
            case modeSA
                f=ActualSignalFrequency(datatable(i,1), datatable(i,4)) 'true freq in MHz, not what hardware thinks it tuned to ver116-4m 'ver116-4s
                db=datatable(i,2)   'dBm
                ang=0
            case modeScalarTrans
                f=S21DataArray(i,0) 'true freq
                db=S21DataArray(i,1)   'dB
                ang=0
            case modeVectorTrans
                f=S21DataArray(i,0) 'true freq
                db=S21DataArray(i,1)   'dB
                if doIntermed then ang=S21DataArray(i,2) : ang=S21DataArray(i,2)    'deg, intermediate or final ver116-4j
            case else   'Reflection
                f=ReflectArray(i,0) 'true freq
                if doIntermed then  'ver116-4j
                    db=ReflectArray(i,constIntermedS11DB) : ang=ReflectArray(i,constIntermedS11Ang)   'intermed db and deg
                else
                    db=ReflectArray(i,constGraphS11DB) : ang=ReflectArray(i,constGraphS11Ang)    'final db and deg
                end if
        end select
        VNAData(i,0)=f : VNAData(i,1)=db : VNAData(i,2)=ang
    next i
    for i=1 to 4: VNADataTitle$(i)=gGetTitleLine$(i) : next i
    VNADataNumSteps=globalSteps : VNADataLinear=gGetXIsLinear()
        'In SA mode we will end up with a bogus value for VNADataZ0, but it is meaningless anyway.
    if msaMode$=modeReflection then VNADataZ0=S11GraphR0 else VNADataZ0=S21JigR0
    */
}
void hwdInterface::autoWaitPrecalculate()
{
  //Activate use of auto wait times with specified precision level ver116-1b
  //Video filter should be properly set first.
  float autoWaitMaxErrorDB;
  float autoWaitMaxErrorDegrees;
  if (vars->autoWaitPrecision == "Fast")
  {
      autoWaitMaxErrorDB=1; autoWaitMaxErrorDegrees=1.5;
  }
  else if (vars->autoWaitPrecision == "Normal")
  {
      autoWaitMaxErrorDegrees=0.1;
      if (vars->msaMode==modeSA) autoWaitMaxErrorDB=0.25; else autoWaitMaxErrorDB=0.1;
  }
  else   // "Precise"
  {
      //Note that in precise mode we do an extra rep after determinining it is OK to quit,
      //so the actual error should be much less the specified.
      autoWaitMaxErrorDB=0.03; autoWaitMaxErrorDegrees=0.03;
  }

  //Calculate allowed errors. These are local variables
  int maxErrorPhaseADC=(int)(autoWaitMaxErrorDegrees*activeConfig->maxpdmout/360)+1;  //convert degree error to ADC error
  //Max allowed mag error in ADC units is max db error times slope in ADC per dB.
  float maxErrorLowEndADC=autoWaitMaxErrorDB*vars->calLowEndSlope;
  float maxErrorCenterADC=autoWaitMaxErrorDB*vars->calCenterSlope;
  float maxErrorHighEndADC=autoWaitMaxErrorDB*vars->calHighEndSlope;
  if (vars->msaMode==modeSA || vars->msaMode==modeScalarTrans) vars->autoWaitTC=vars->videoMagTC; else vars->autoWaitTC=qMax(vars->videoMagTC, vars->videoPhaseTC);    //ver116-4j


  //We do repeated reads
  //until we get stable readings. The delay between reads is nominally autoWaitTC.
  //So we pre-calculate here how much the ADC should be allowed to change during that time.
  //The fraction of the total change that remains unsettled after N time constants will
  //be e^(-N), and the proportion that did settle is 1-e^(-N). We don't want the remaining change to
  //exceed the allowed error.
  //The remaining settlement is a proportion R=e^(-N)/(1-e^(-N)) of the measured settlement.
  //The max allowed change is where R*measuredChange=max allowed error, so
  //max allowed measured change=maxAllowedError/R
  //
  //In fact the time delay will generally be more than we specify, so we could allow a bigger change
  //if we knew the actual time, but the approach here is conservative.
  //For mag, N=1 so R=0.58
  //ver116-4j changed to use autoWaitTC rather than videoMagTC as the basic wait time
  float N=vars->autoWaitTC/vars->videoMagTC;   //number of mag time constants that we wait
  float unsettledFract=exp(0-N);
  double R=qMax(0.3, (double)(unsettledFract/(1-unsettledFract)));    //Limit R to be conservative
  vars->autoWaitMaxChangeLowEndADC=int(maxErrorLowEndADC/R)+1;
  vars->autoWaitMaxChangeCenterADC=int(maxErrorCenterADC/R)+1;
  vars->autoWaitMaxChangeHighEndADC=int(maxErrorHighEndADC/R)+1;

  N=vars->autoWaitTC/vars->videoPhaseTC;   //number of phase time constants that we wait
  unsettledFract=exp(0-N);
  R=qMax(0.3, (double)(unsettledFract/(1-unsettledFract)));    //Limit R to be conservative
  vars->autoWaitMaxChangePhaseADC=int(maxErrorPhaseADC/R)+1;

}
