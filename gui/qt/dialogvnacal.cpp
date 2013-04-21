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
#include "dialogvnacal.h"
#include "ui_dialogvnacal.h"

dialogVNACal::dialogVNACal(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogVNACal)
{
  ui->setupUi(this);
}

dialogVNACal::~dialogVNACal()
{
  delete ui;
}
void dialogVNACal::RunVNACal()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[RunVNACal]
    if haltsweep=1 then gosub [FinishSweeping]

    WindowWidth = 600 : WindowHeight = 400
    call GetDialogPlacement 'set UpperLeftX and UpperLeftY ver115-1c
    UpperLeftY=UpperLeftY-100
    BackgroundColor$="gray" 'ver115-5b changed color
    ForegroundColor$="black"
    TextboxColor$ = "white"
    ComboboxColor$="white"

    statictext #VNAcal.Path1, "The MSA is currently in Path X.",110,15,350, 18
    statictext #VNAcal.Path2, "MSA calibrations are not saved separately for different paths. If the current path is not the one",25,35,560, 18
    statictext #VNAcal.Path3, "for which the calibration will be used, close this window and change the path selection.",25,52,525, 18
    statictext #VNAcal.Video, "VIDEO FILTER should be set to NARROW bandwidth for maximum smoothing.",25,75,525, 18

    VNAbtnLeft=25 : VNAbtnTop=80
    s$="Band Sweep calibration is run at the same frequency points at which it will be used."
    statictext #VNAcal.BandInst, s$,VNAbtnLeft+25,VNAbtnTop+42,500, 35
    s$="You may save the current Band calibration as a Base calibration, to be used as a coarse"
    s$=s$;" reference when the Band calibration is not current."
    if steps>2000 then s$="(Saving Base Line calibration is disabled because it is limited to 2,000 steps.)"  'ver115-1b
    statictext #VNAcal.BaseInst, s$,VNAbtnLeft+25,VNAbtnTop+85,450, 40
    if msaMode$="VectorTrans" then    'ver115-4e
        statictext #VNAcal.connect, "TG output must have THROUGH connection to MSA input.",25,90,550, 18
        statictext #VNAcal.DelayLabel, "Delay of Calibration Through Connection (ns):",VNAbtnLeft+10,VNAbtnTop+160,270, 20
        textbox #VNAcal.Delay, VNAbtnLeft+282,VNAbtnTop+160,50, 20
    else    'Reflection
        statictext #VNAcal.connect, "Connect TG output and MSA input to test fixture and attach proper cal standards.",25,90,550, 18
    end if

    statictext #VNAcal.LastBandCal, "xx",VNAbtnLeft,VNAbtnTop+62,570, 18   'Description of last calibration
    statictext #VNAcal.LastBaseCal, "xx",VNAbtnLeft,VNAbtnTop+120,570, 18   'Description of last calibration

    button #VNAcal.Perform, "Perform Band Cal",[PerformCal], UL, VNAbtnLeft+10, VNAbtnTop+210, 120,25    'ver114-5f
    button #VNAcal.ClearBand, "Clear Band Cal",[ClearBandCal], UL, VNAbtnLeft+175, VNAbtnTop+210, 120,25    'ver114-5f
    button #VNAcal.SaveBase, "Save As Base",[SaveBaseCal], UL, VNAbtnLeft+10, VNAbtnTop+250, 120,25    'ver114-5f
    button #VNAcal.ClearBase, "Clear Base Cal",[ClearBaseCal], UL, VNAbtnLeft+175, VNAbtnTop+250, 120,25    'ver114-5f

    button #VNAcal.Done, "Done",[VNACalFinished], UL, VNAbtnLeft+375, VNAbtnTop+210, 75,25
    button #VNAcal.Explain, "Help",ExplainTransCal, UL, VNAbtnLeft+375, VNAbtnTop+250, 75,25

    open "Perform Calibration" for dialog_modal as #VNAcal  'ver114-3g
    print #VNAcal, "trapclose [VNACalFinished]"   'goto [finished] if xit is clicked
    print #VNAcal, "font ms_sans_serif 10"

    #VNAcal.Path1, "The MSA is currently in "; path$    'ver114-5p
    calInProgress=0
    BandSweepCalDone=0  'ver114-5L 'ver115-2d
    if msaMode$="VectorTrans" then 'ver115-4e
        #VNAcal.Delay, lineCalThroughDelay  'Delay in ns 'ver115-5a
    end if
        'Display info about last band and base cals
    if msaMode$="ScalarTrans" or msaMode$="VectorTrans" then 'ver115-1b
        #VNAcal.LastBandCal, "Band: ";CalInfo$(bandLinePath$, bandLineTimeStamp$, bandLineLinear, _
                                bandLineNumSteps, bandLineStartFreq, bandLineEndFreq)
        #VNAcal.LastBaseCal, "Base: ";CalInfo$(baseLinePath$, baseLineTimeStamp$, baseLineLinear, _
                                baseLineNumSteps, baseLineStartFreq, baseLineEndFreq)
    else
        #VNAcal.LastBandCal, "Band: ";CalInfo$(OSLBandPath$, OSLBandTimeStamp$, OSLBandLinear, _
                                OSLBandNumSteps, OSLBandStartFreq, OSLBandEndFreq)
        #VNAcal.LastBaseCal, "Base: ";CalInfo$(OSLBasePath$, OSLBaseTimeStamp$, OSLBaseLinear, _
                                OSLBaseNumSteps, OSLBaseStartFreq, OSLBaseEndFreq)
    end if
    wait
*/
}

void dialogVNACal::setGlobalVars(globalVars *newVar)
{
  vars = newVar;
}
/*
void dialogVNACal::setGlobalGraph(msagraph *newGraph)
{
  graph = newGraph;
}*/

void dialogVNACal::VNACalFinished()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[VNACalFinished]
    if calInProgress=1 then goto [PostScan] 'Don't allow quit in middle of cal
    if msaMode$="ScalarTrans" or msaMode$="VectorTrans" then    'ver115-4e
        if msaMode$="VectorTrans" then   'ver115-4e
            #VNAcal.Delay, "!contents? s$"
            lineCalThroughDelay=val(uCompact$(s$))      'Store in ns; used to initialize if we open dialog again ver115-5a
        else
            lineCalThroughDelay=0
        end if
        'If we did a cal, make it the desired one and force restart to implement
        if BandSweepCalDone=1 then     'ver114-5L created if... 'ver115-2d
            if BandSweepCalDone then desiredCalLevel=2   'desire BandSweep; overrides BaseLine ver114-5L 'ver115-2d
            call SignalNoCalInstalled   'ver116-4b
            call RequireRestart 'To activate desired cal; graph is messed up anyway ver114-5L
        end if
    else
        'Reflection; handled by [PerformOSLCal]
    end if
    close #VNAcal  'ver114-5n moved this outside the if... block
    wait
*/
}
void dialogVNACal::ExplainTransCal()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub ExplainTransCal h$ 'Put up dialog explaining band/base and transmission cal
    WindowWidth = 635
    WindowHeight = 220
    UpperLeftX=0    'Doesn't seem to matter what is specified here
    UpperLeftY=150
    BackgroundColor$="gray"
    ForegroundColor$="black"
    statictext #Explain, "Band calibration is performed at the frequency points of immediate interest and is used only as",  10, 10, 630,  20
    statictext #Explain, "long as the sweep matches those points. Base calibration is performed over a broad frequency range,",  10, 30, 630,  20
    statictext #Explain, "to be interpolated to the current sweep frequencies when there is no current band calibration.",  10, 50, 630,  20
    statictext #Explain, "To create a Base calibration you perform a Band calibration and save it as a Base calibration.", 10, 70, 630,  20
    statictext #Explain, "It is intended as a convenient coarse reference, especially when phase precision is not required.", 10, 90, 630,  20
    statictext #Explain, "In Transmission Mode, Base calibrations are saved in a file for use in future sessions.",  10, 110, 630,  20
    statictext #Explain, "In Transmision Mode you also specify the time delay of the calibration Through connection,",  10, 130, 630,  20
    statictext #Explain, "which is ideally zero but may be greater if you need to use an adapter.",  10, 150, 630,  20
    open "Reflection Calibration Help" for dialog_modal as #Explain
    #Explain, "trapclose [ExplainTransCalFinished]"
    print #Explain, "font ms_sans_serif 10"
    wait
*/
}

void dialogVNACal::ExplainTransCalFinished()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ExplainTransCalFinished]
    close #Explain
end sub
*/
}

void dialogVNACal::SetCalButtonStatus()
{/*
sub SetCalButtonStatus stat$ 'Enable or disable all calibration buttons
    'stat$ should be "!enable" or "!disable"
    #VNAcal.Perform, stat$
    #VNAcal.SaveBase, stat$
    #VNAcal.ClearBand, stat$
    #VNAcal.ClearBase, stat$
    #VNAcal.Done, stat$
end sub
*/
}

void dialogVNACal::CalAborted()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[CalAborted]    ' aborted line calibration; OSL has its own window and abort button
    gosub [FinishSweeping] 'Finish nicely 'ver114-7d
    #VNAcal.Perform, "Perform Band Cal"     'ver115-1e
    specialOneSweep=0
    calInProgress=0     'ver114-5L
    sweepDir=saveSweepDir : alternateSweep=saveAlternate
    planeadj=savePlaneAdj : wate=saveWate
    call SetCalButtonStatus "!enable" 'ver114-6k
return
*/
}

void dialogVNACal::PerformCal()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[PerformCal]
    if calInProgress then gosub [CalAborted] : wait 'Perform changes to Abort during calibration 'ver116-4b
    if msaMode$="Reflection" then  'ver115-1b
        gosub [PerformOSLCal]    'ver115-5b changed back to gosub so others can call [PerformOSLCal]
        #VNAcal.LastBandCal, "Band: ";CalInfo$(OSLBandPath$, OSLBandTimeStamp$, OSLBandLinear, _
                                OSLBandNumSteps, OSLBandStartFreq, OSLBandEndFreq)      'ver115-5b
        wait
    else    'ver115-1f moved some items here from [BandLineCal] to make it independent of this dialog
        call SetCalButtonStatus "!disable"
        #VNAcal.Perform, "!enable" : #VNAcal.Perform, "Abort Cal"
        if msaMode$="VectorTrans" then   'ver115-5a
            #VNAcal.Delay, "!contents? s$"
            lineCalThroughDelay=val(uCompact$(s$))      'Store in ns; used by [BandLineCal]
        else
            lineCalThroughDelay=0
        end if
        gosub [BandLineCal]
        #VNAcal.Perform, "Perform Band Cal"
        BandSweepCalDone=1  'ver114-5L
        #VNAcal.LastBandCal, "Band: ";CalInfo$(bandLinePath$, bandLineTimeStamp$, bandLineLinear, _
                                bandLineNumSteps, bandLineStartFreq, bandLineEndFreq)
        call SetCalButtonStatus "!enable"
        wait
    end if
        'Can't get here
*/
}

void dialogVNACal::ClearBandCal()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ClearBandCal]
    if msaMode$="Reflection" then
        OSLBandNumSteps=-1 'ver116-4n
    else
        bandLineNumSteps=-1
    end if
    #VNAcal.LastBandCal, "Band: Calibration is cleared."
    wait
*/
}

void dialogVNACal::ClearBaseCal()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[ClearBaseCal]
    if msaMode$="Reflection" then
        OSLBaseNumSteps=-1
    else
        'Clear baseline and delete the file
        baseLineNumSteps=-1
        Kill DefaultDir$;"\MSA_Info\OperatingCal\BaseLineCal.txt"
    end if
    #VNAcal.LastBaseCal, "Base: Calibration is cleared."
    wait
*/
}
void dialogVNACal::SaveBaseCal()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[SaveBaseCal]    'Save current cal as base cal
    if msaMode$="ScalarTrans" or msaMode$="VectorTrans" then
        call TransferBandToBaseLineCal  'ver115-5c
        call SaveBaseLineCalFile    'Save base line in a file
        #VNAcal.LastBaseCal, "Base: ";CalInfo$(baseLinePath$, baseLineTimeStamp$, baseLineLinear, _
                                baseLineNumSteps, baseLineStartFreq, baseLineEndFreq)
    else    'Reflection--saved in array but not to file
        call TransferBandToBaseOSLCal   'ver115-5c
        #VNAcal.LastBaseCal, "Base: ";CalInfo$(OSLBasePath$, OSLBaseTimeStamp$, OSLBaseLinear, _
                    OSLBaseNumSteps, OSLBaseStartFreq, OSLBaseEndFreq)
    end if
    wait
*/
}
void dialogVNACal::BandLineCal()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
[BandLineCal]   'Run Line Calibration and return. Can be called from anywhere; lineCalThroughDelay must be set
    calInProgress=1 : call SignalNoCalInstalled   'ver116-4n
    haltsweep=0     'So Restart will actually restart. ver114-4f
    specialOneSweep=1

    saveAlternate=alternateSweep : saveSweepDir=sweepDir : savePlaneAdj=planeadj : saveWate=wate
    alternateSweep=0 : sweepDir=1
    call FunctSetVideoAndAutoWait 1  'set video filter to narrow and autowait to Normal or Precise; 1 means save settings ver116-4b
    planeadj=0  'So phase will not be affected  'ver116-4b
        'Note with calInProgress=1, the cal installation routine will not install anything and sets applyCalLevel=0
    gosub [Restart] 'Perform one sweep and fill datatable(,)
    specialOneSweep=0
    sweepDir=saveSweepDir : alternateSweep=saveAlternate
    planeadj=savePlaneAdj
    call FunctRestoreVideoAndAutoWait
    'Cal data is now in datatable
    if msaMode$="VectorTrans" then  'ver115-5a
        degPerMillionHz=0.36*lineCalThroughDelay    'lineCalThroughDelay is delay in ns ver116-4m
    else
        degPerMillionHz=0
    end if
    for i=0 to steps    'retrieve data
            'ver114-8a added through delay calc
        freq=gGetPointXVal(i+1) 'freq in MHz   actual tuning freq, not equiv 1G freq ver115-5c
        phaseDelay=degPerMillionHz*freq
        bandLineCal(i,0)=freq 'frequency
        bandLineCal(i,1)=datatable(i,2) 'mag
        p=datatable(i,3)+phaseDelay 'Phase, with through delay removed  'ver115-4g
        p=p mod 360
        if p<=-180 then p=p+360 else if p>180 then p=p-360  'put in range -180 to 180
        bandLineCal(i,2)=p
    next i
            'Save the conditions under which the cal was done
    bandLineStartFreq=startfreq
    bandLineEndFreq=endfreq
    bandLineNumSteps=steps
    bandLineLinear=gGetXIsLinear()
    bandLineS21JigAttach$=S21JigAttach$ 'ver115-1b
    bandLineS21JigR0=S21JigR0 'ver115-1b
    bandLinePath$=path$
    bandLineTimeStamp$=date$("mm/dd/yy"); "; ";time$()
    calInProgress=0
    desiredCalLevel=2   'desire BandSweep since we just did it
    call RequireRestart 'So cal gets installed before user proceeds, but we don't install it here
    return
*/
}

void dialogVNACal::TransferBandToBaseLineCal()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub TransferBandToBaseLineCal   'Create base line cal from band line cal but don't save to file ver115-5c
    baseLineNumSteps=min(bandLineNumSteps,2000) 'Base cal can only have 2000 steps
    for i=0 to baseLineNumSteps
        baseLineCal(i,0)=bandLineCal(i,0) 'frequency
        baseLineCal(i,1)=bandLineCal(i,1) 'mag
        baseLineCal(i,2)=bandLineCal(i,2) 'phase
    next i
        'Save the conditions under which the cal was done
    baseLineStartFreq=bandLineStartFreq
    baseLineEndFreq=bandLineEndFreq
    baseLineLinear=bandLineLinear
    baseLineS21JigAttach$=bandLineS21JigAttach$
    baseLineS21JigR0=bandLineS21JigR0
    baseLinePath$=bandLinePath$      'ver115-1e
    baseLineTimeStamp$=bandLineTimeStamp$
end sub
*/
}

void dialogVNACal::TransferBandToBaseOSLCal()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub TransferBandToBaseOSLCal   'Create base OSL cal from band OSL cal but don't save to file ver115-5c
    OSLBaseNumSteps=min(OSLBandNumSteps,2000) 'Base cal can only have 2000 steps
    for i=0 to OSLBaseNumSteps     'ver115-2d
        OSLBaseRef(i,0)=OSLBandRef(i,0) 'Reference, frequency
        OSLBaseRef(i,1)=OSLBandRef(i,1) 'mag
        OSLBaseRef(i,2)=OSLBandRef(i,2) 'phase
        OSLBaseA(i,0)=OSLBandA(i,0) : OSLBaseA(i,1)=OSLBandA(i,1) 'Coeff A real, imag
        OSLBaseB(i,0)=OSLBandB(i,0) : OSLBaseB(i,1)=OSLBandB(i,1) 'Coeff B real, imag
        OSLBaseC(i,0)=OSLBandC(i,0) : OSLBaseC(i,1)=OSLBandC(i,1) 'Coeff C real, imag
    next i

    OSLBaseStartFreq=OSLBandStartFreq
    OSLBaseEndFreq=OSLBandEndFreq
    OSLBaseLinear=OSLBandLinear
    OSLBasePath$=OSLBandPath$
    OSLBaseS11JigType$=OSLBandS11JigType$
    OSLBaseS21JigAttach$=OSLBandS21JigAttach$
    OSLBaseS11BridgeR0=OSLBandS11BridgeR0
    OSLBaseS21JigR0=OSLBandS21JigR0
    OSLBaseRefType$=OSLBandRefType$ 'ver116-4n
    OSLBaseTimeStamp$=OSLBandTimeStamp$
end sub
*/
}


void dialogVNACal::CalInfo()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function CalInfo$(cPath$, dateStamp$, cLinear, nSteps, fStart, fEnd) 'Return line or OSL cal info in a string
    if nSteps<0 then CalInfo$="Calibration is cleared." : exit function
    if cLinear then lin$="linear" else lin$="log"
    CalInfo$="Performed ";dateStamp$;"; ";cPath$;"; ";nSteps;" ";lin$;" steps, ";fStart;" to ";fEnd;" MHz." 'ver115-1g
end function
*/
}
void dialogVNACal::SignalNoCalInstalled()
{

  //Clear time stamps and num of steps for installed cal to show none is installed  //ver116-4b
  //We don't have to clear the cal data itself.
  applyCalLevel=0;
  installedBandLineTimeStamp="";
  installedBaseLineTimeStamp="";
  installedOSLBandTimeStamp="";
  installedOSLBaseTimeStamp="";
  //Probably not necessary to set num steps to -1, but this is how we originally did it.
  installedBaseLineNumSteps=-1;
  installedOSLBaseNumSteps=-1;

}
int dialogVNACal::BandLineCalIsCurrent()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  int retVal;
  //Returns 1 if full line cal data was taken at current sweep settings
  //Current means the full line cal exists and matches current sweep params
  float xMin, xMax;
  graph->gGetXAxisRange(xMin, xMax);
  if (vars->globalSteps==bandLineNumSteps && graph->gGetXIsLinear()==bandLineLinear && xMin==bandLineStartFreq && xMax==bandLineEndFreq)
    retVal = 1;
  else
    retVal = 0; //ver115-1b
  return retVal;
  */
}

int dialogVNACal::BaseLineCalIsCurrent()
{
  int retVal;
  //Returns 1 if OSL band cal has valid data even if different sweep params
  //Current does not mean it is actually installed
  int isCurr;
  if (baseLineNumSteps>0 && vars->path==baseLinePath && baseLineS21JigAttach==S21JigAttach
                    && baseLineS21JigR0==S21JigR0) isCurr=1; else isCurr=0;
  if (isCurr)
  {
    retVal=1;
  }
  else
  {
    retVal=0; installedBaseLineNumSteps=-1; //Indicate not validly installed ver115-1b
  }
  return retVal;
}

int dialogVNACal::BaseLineCalIsInstalled()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  int retVal;
  //Returns 1 if base line cal is already installed at current sweep params
  //Current means the full line cal exists and was last installed with sweep params matching the current params
  if (BaseLineCalIsCurrent()==0) {retVal=0; return retVal;}   //don't have current base cal ver114-1L
    //If the installed Base line cal has different time stamp from current Base cal, then current is not installed
  if (installedBaseLineTimeStamp != baseLineTimeStamp) { retVal=0; return retVal; }    //ver115-2d
  float xMin, xMax;
  graph->gGetXAxisRange(xMin, xMax);
  if (vars->globalSteps==installedBaseLineNumSteps && graph->gGetXIsLinear()==installedBaseLineLinear
      && xMin==installedBaseLineStartFreq && xMax==installedBaseLineEndFreq)
                        retVal=1; else retVal=0;      //ver115-1b
  return retVal;
  */
}
