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
#include <QMessageBox>
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
void dialogVNACal::setFilePath(QString path)
{
  DefaultDir = path;
  if (!QDir().mkpath(DefaultDir + "/MSA_Info/OperatingCal"))
  {
    QMessageBox::about(0,"Error", "Unable to create OperatingCal folder.");
  }
}

void dialogVNACal::setUwork(cWorkArray *newuWork)
{
  uWork = newuWork;
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
int dialogVNACal::BandLineCalContextAsTextArray()
{
  //Put line cal points into uTextPointArray$, with header info
  //return number of lines placed into uTextPointArray$
  //First line begins with ! and is line 1 of the title: !Log Sweep Path N. "Log" may instead by "Linear"; N=path number
  //First 3 lines are title, each preceded by !
  //Next line is sweep info
  //Next is Touchstone options line
  //Next is comment data headings
  //Then comes each point as its own string
  QString sweep;
  if (bandLineLinear) sweep="!Linear Sweep; "; else sweep="!Log Sweep ";
  uWork->uTextPointArray[1]="! BandSweep Line Calibration Data";
  uWork->uTextPointArray[2]="!";
  uWork->uTextPointArray[3]="!"+bandLineTimeStamp;
    //Save sweep info: log/linear ; path info (in form Path N); Jig attachment and R0
  uWork->uTextPointArray[4]=sweep+bandLinePath+"; S21Jig="+bandLineS21JigAttach+ "; S21JigR0="+ bandLineS21JigR0; //ver115-1b
  uWork->uTextPointArray[5]="# MHz S DB R 50" ;  //Freq in MHz, data in DB/angle format
  uWork->uTextPointArray[6]="! MHz  S21_DB  S21_Degrees";
  QString aSpace=" ";
  for (int i=0; i < vars->globalSteps; i++)
  {
    //save freq, mag and phase
    uWork->uTextPointArray[i+7] = QString::number(vars->bandLineCal[i][0])
        + aSpace + QString::number(vars->bandLineCal[i][1])
        + aSpace + QString::number(vars->bandLineCal[i][2]); //ver114-5f
  }
  return vars->globalSteps+7; //Number of lines
}
QString dialogVNACal::BandLineCalContext()
{
  //Return data points as string, with title in first 3 lines
  //We do not include StartContext or EndContext lines
  int nLines=BandLineCalContextAsTextArray();   //Assemble strings into uTextPointArray$
  return uWork->uTextArrayToString(1,nLines); //Assemble array of strings into one string
}
void dialogVNACal::BandLineCalContextToFile(QStringList &fHndl)
{
  //save band line cal points to file
  //We do not include StartContext or EndContext lines
  //fHndl$ is the handle of an already open file. We output our data
  //but do not close the file.
  int nLines=BandLineCalContextAsTextArray();   //Assemble strings into uTextPointArray$
  for (int i=1; i < nLines; i++)
  {
    fHndl.append(uWork->uTextPointArray[i]);
  }
}
int dialogVNACal::GetBandLineCalContextFromFile(QString fHndl)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
 'get points from file; return number of points
    'We return number of points read, or -1 for file error
    'fHndl$ is the handle of an already open file. We read our data
    'but do not close the file. The last line we read will be "!EndContext"
    isErr=uArrayFromFile(fHndl$,3) 'Get data, 3 per line
    if isErr then GetBandLineCalContextFromFile=-1 : baseLineNumSteps=-1 : exit function
        'Move the data from uWorkArray to gGraphVal
    for i=1 to uWorkNumPoints
        bandLineCal(i-1,0)=uWorkArray(i, 0) 'freq
        bandLineCal(i-1,1)=uWorkArray(i, 1) : bandLineCal(i-1,2)=uWorkArray(i, 2) 'Mag and phase
    next i
    GetBandLineCalContextFromFile=uWorkNumPoints
        'Now derive the sweep parameters from the array and the title ver114-5h
    bandLineNumSteps=uWorkNumPoints-1
    bandLineStartFreq=bandLineCal(0,0) : bandLineEndFreq=bandLineCal(bandLineNumSteps,0)
    bandLineTimeStamp$=uWorkTitle$(3)   'uArrayFromFile put date/time stamp here
    sweep$=uWorkTitle$(4)   'uArrayFromFile put sweep info here
    if instr(sweep$,"Linear")>0 then bandLineLinear=1 else bandLineLinear=0
            'ver115-1b added the retrieval of the following sweep info
        'Get remaining sweep info from sweep$. Each item has a keyword and ends with a semi-colon or end of line
    bandPathNum$=uGetParamText$(sweep$, "Path ", ";")  'Gets text after "Path " to semicolon or end
    if bandPathNum$="" then bandLinePath$="Path 1" else bandLinePath$="Path ";bandPathNum$
    bandLineS21JigAttach$=uGetParamText$(sweep$, "S21Jig=", ";")  'Gets text after S21Jig= to semicolon or end
    if bandLineS21JigAttach$="" then bandLineS21JigAttach$="Series"
    bandLineS21JigR0=val(uGetParamText$(sweep$, "S21JigR0=", ";"))  'Gets text after S21Jig= to semicolon or end
    if bandLineS21JigR0=0 then bandLineS21JigR0=50
    */
  return 0;
}
void dialogVNACal::RestoreBandLineCalContext(QString &s, int &startpos)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;

  /*
function RestoreBandLineCalContext(byref s$, byref startPos)   'Restore line cal points. Return number of points
    'We return number of points read, or -1 for file error
    'We ignore data prior to startPos. We update startPos to the start of the next line after the line
    'with "!EndContext"; or if no such line then one past end of string

    isErr=uArrayFromString(s$, 1, startPos, 3)   'Get data into uWorkArray; 3 data per line (freq+mag+phase)
    if isErr then RestoreBandLineCalContext=0 : errCode=1 : exit function
        'Now transfer retrieved data from uWorkArray() to lineCalArray().
    errCode=0
    for i=1 to uWorkNumPoints
        bandLineCal(i-1,0)=uWorkArray(i, 0) 'freq
        bandLineCal(i-1,1)=uWorkArray(i, 1) : bandLineCal(i-1,2)=uWorkArray(i, 2) 'Mag and phase
    next i
    RestoreBandLineCalContext=uWorkNumPoints
    'Now derive the sweep parameters from the array and the title ver114-5h
    bandLineNumSteps=uWorkNumPoints-1
    bandLineStartFreq=bandLineCal(0,0) : bandLineEndFreq=bandLineCal(bandLineNumSteps,0)
    bandLineTimeStamp$=uWorkTitle$(3)   'uArrayFromString put date/time stamp here
    sweep$=uWorkTitle$(4)   'uArrayFromString put sweep info here
    if instr(sweep$,"Linear")>0 then bandLineLinear=1 else bandLineLinear=0
            'ver115-1b added the retrieval of the following sweep info
        'Get remaining sweep info from sweep$. Each item has a keyword and ends with a semi-colon or end of line
    bandPathNum$=uGetParamText$(sweep$, "Path ", ";")  'Gets text after "Path " to semicolon or end
    if bandPathNum$="" then bandLinePath$="Path 1" else bandLinePath$="Path ";bandPathNum$
    bandLineS21JigAttach$=uGetParamText$(sweep$, "S21Jig=", ";")  'Gets text after S21Jig= to semicolon or end
    if bandLineS21JigAttach$="" then bandLineS21JigAttach$="Series"
    bandLineS21JigR0=val(uGetParamText$(sweep$, "S21JigR0=", ";"))  'Gets text after S21Jig= to semicolon or end
    if bandLineS21JigR0=0 then bandLineS21JigR0=50
end function
*/
}
int dialogVNACal::BaseLineCalContextAsTextArray()
{
  //Put line cal points into uTextPointArray$, with header info
  //return number of lines placed into uTextPointArray$
  //First line begins with ! and is line 1 of the title: !Log Sweep Path N. "Log" may instead by "Linear"; N=path number
  //First 3 lines are title, each preceded by !
  //Next line is sweep info
  //Next is Touchstone options line
  //Next is comment data headings
  //Then comes each point as its own string
  QString sweep;
  if (baseLineLinear) sweep="!Linear Sweep; "; else sweep="!Log Sweep ";
  uWork->uTextPointArray[1]="! BaseLine Calibration Data";
  uWork->uTextPointArray[2]="!";
  uWork->uTextPointArray[3]="!"+baseLineTimeStamp;
  uWork->uTextPointArray[4]=sweep+baseLinePath+"; S21Jig="+baseLineS21JigAttach + "; S21JigR0=" + baseLineS21JigR0; //ver115-1b
  uWork->uTextPointArray[5]="# MHz S DB R 50";   //Freq in MHz, data in DB/angle format
  uWork->uTextPointArray[6]="! MHz  S21_DB  S21_Degrees";
  QString aSpace=" ";
  for (int i=0; i < vars->globalSteps; i++)
  {
    //save freq, mag and phase
    uWork->uTextPointArray[i+7]=QString::number(vars->baseLineCal[i][0])
        +aSpace+QString::number(vars->baseLineCal[i][1])
        +aSpace+QString::number(vars->baseLineCal[i][2]);
  }
  return vars->globalSteps+7; //Number of lines
}
QString dialogVNACal::BaseLineCalContext()
{
  //Return data points as string, with title in first 3 lines
  //We do not include StartContext or EndContext lines
  int nLines=BaseLineCalContextAsTextArray();   //Assemble strings into uTextPointArray$
  return uWork->uTextArrayToString(1, nLines); //Assemble array of strings into one string
}
void dialogVNACal::BaseLineCalContextToFile(QStringList &fHndl)
{
  //save line cal points to file
  //We do not include StartContext or EndContext lines
  //fHndl$ is the handle of an already open file. We output our data
  //but do not close the file.
  int nLines=BaseLineCalContextAsTextArray();   //Assemble strings into uTextPointArray$
  for (int i=0; i < nLines; i++)
  {
    fHndl.append(uWork->uTextPointArray[i]);
  }
}
int dialogVNACal::RestoreBaseLineCalContext(QString &s, int &startPos)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  'Restore line cal points. Return number of points
     'We return number of points read, or -1 for file error
     'We ignore data prior to startPos. We update startPos to the start of the next line after the line
     'with "!EndContext"; or if no such line then one past end of string

     isErr=uArrayFromString(s$, 1, startPos, 3)   'Get data into uWorkArray; 3 data per line (freq+mag+phase)
     if isErr then RestoreBaseLineCalContext=-1 : exit function
         'Now transfer retrieved data from uWorkArray() to lineCalArray().
         'Make sure baseLineCal is big enough. Note that it can be bigger than data arrays,
         'since it is used only by interpolating into a possibly smaller array.
     if uWorkNumPoints>gMaxNumPoints then redim baseLineCal(uWorkNumPoints+20, 2) 'ver114-5m

     for i=1 to uWorkNumPoints
         baseLineCal(i-1,0)=uWorkArray(i, 0)    'Freq
         baseLineCal(i-1,1)=uWorkArray(i, 1) : baseLineCal(i-1,2)=uWorkArray(i, 2) 'Mag and phase
     next i
     RestoreBaseLineCalContext=uWorkNumPoints
              'Now derive the sweep parameters from the array and the title ver114-5h
     baseLineNumSteps=uWorkNumPoints-1
     baseLineStartFreq=baseLineCal(0,0) : baseLineEndFreq=baseLineCal(baseLineNumSteps,0)
     baseLineTimeStamp$=uWorkTitle$(3)   'uArrayFromString put date/time stamp here
     sweep$=uWorkTitle$(4)   'uArrayFromString put sweep info here
     if instr(sweep$,"Linear")>0 then baseLineLinear=1 else baseLineLinear=0
         'ver115-1b added the retrieval of the following sweep info
         'Get remaining sweep info from sweep$. Each item has a keyword and ends with a semi-colon or end of line
     basePathNum$=uGetParamText$(sweep$, "Path ", ";")  'Gets text after "Path " to semicolon or end
     if basePathNum$="" then baseLinePath$="Path 1" else baseLinePath$="Path ";basePathNum$
     baseLineS21JigAttach$=uGetParamText$(sweep$, "S21Jig=", ";")  'Gets text after S21Jig= to semicolon or end
     if baseLineS21JigAttach$="" then baseLineS21JigAttach$="Series"
     baseLineS21JigR0=val(uGetParamText$(sweep$, "S21JigR0=", ";"))  'Gets text after S21Jig= to semicolon or end
     if baseLineS21JigR0=0 then baseLineS21JigR0=50
     */
  return 0;
}
int dialogVNACal::GetBaseLineCalContextFromFile(QFile *fHndl)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
 'get points from file; return number of points
    'We return number of points read, or -1 for file error
    'fHndl$ is the handle of an already open file. We read our data
    'but do not close the file. The last line we read will be "!EndContext"
    isErr=uArrayFromFile(fHndl$,3) 'Get data, 3 per line
    if isErr then GetBaseLineCalContextFromFile=-1 : baseLineNumSteps=-1 : exit function
        'Move the data from uWorkArray to gGraphVal
    for i=1 to uWorkNumPoints
        baseLineCal(i-1,0)=uWorkArray(i, 0)    'Freq
        baseLineCal(i-1,1)=uWorkArray(i, 1) : baseLineCal(i-1,2)=uWorkArray(i, 2) 'Mag and phase
    next i
    GetBaseLineCalContextFromFile=uWorkNumPoints
         'Now derive the sweep parameters from the array and the title ver114-5h
    baseLineNumSteps=uWorkNumPoints-1
    baseLineStartFreq=baseLineCal(0,0) : baseLineEndFreq=baseLineCal(baseLineNumSteps,0)
    baseLineTimeStamp$=uWorkTitle$(3)   'uArrayFromFile put date/time stamp here
    sweep$=uWorkTitle$(4)   'uArrayFromFile put sweep info here
    if instr(sweep$,"Linear")>0 then baseLineLinear=1 else baseLineLinear=0
        'ver115-1b added the retrieval of the following sweep info
        'Get remaining sweep info from sweep$. Each item has a keyword and ends with a semi-colon or end of line
    basePathNum$=uGetParamText$(sweep$, "Path ", ";")  'Gets text after "Path " to semicolon or end
    if basePathNum$="" then baseLinePath$="Path 1" else baseLinePath$="Path ";basePathNum$
    baseLineS21JigAttach$=uGetParamText$(sweep$, "S21Jig=", ";")  'Gets text after S21Jig= to semicolon or end
    if baseLineS21JigAttach$="" then baseLineS21JigAttach$="Series"
    baseLineS21JigR0=val(uGetParamText$(sweep$, "S21JigR0=", ";"))  'Gets text after S21Jig= to semicolon or end
    if baseLineS21JigR0=0 then baseLineS21JigR0=50
    */
  return 0;
}
void dialogVNACal::SaveBaseLineCalFile()
{

 if (CreateOperatingCalFolder())
 {
   QMessageBox::warning(0, "Error", "Cannot save BaseLine Cal file.");
   return;
 }

 QStringList list;
 QFile baseLineOut(DefaultDir + "/MSA_Info/OperatingCal/BaseLineCal.txt");
 if (baseLineOut.open(QFile::WriteOnly))
 {
   QTextStream s(&baseLineOut);
   BaseLineCalContextToFile(list);
   QString sss = list.join("\r");
   s << sss;

   baseLineOut.close();
 }
}

QFile *dialogVNACal::OpenBaseLineCalFile()
{
  //Open baseline calibration file; return its handle
  //If file does not exist, return "".

  QFile *fFile = new QFile(DefaultDir + "/MSA_Info/OperatingCal/BaseLineCal.txt");

  if (!fFile->exists())
  {
    QString ggg = "file not found";
    delete fFile;
    fFile = NULL;
  }
  else if (!fFile->open(QFile::ReadOnly))
  {
    QString eee = fFile->errorString();
    qDebug() << eee;
    delete fFile;
    fFile = NULL;
  }

  return fFile;
}

int dialogVNACal::LoadBaseLineCalFile()
{
  int retVal;

  //Return 1 if error (file does not exist)
  QFile *fHndl = OpenBaseLineCalFile();
  if (fHndl==NULL) { retVal=1; baseLineNumSteps=-1; return retVal;}
  int nPoints=GetBaseLineCalContextFromFile(fHndl);
  if (nPoints<=0)
  {
   retVal=1; baseLineNumSteps=-1; //error
  }
  else
  {
   retVal=0;
  }
  fHndl->close();
  delete fHndl;

  return retVal;
}
int dialogVNACal::CreateOperatingCalFolder()
{
  if (!QDir().mkpath(DefaultDir + "/MSA_Info/OperatingCal"))
    return 1;

  return 0;
}

void dialogVNACal::InstallSelectedLineCal(Q2DfloatVector &GraphVal, int MaxPoints, int xIsLinear)
{

  //Apply full line cal or baseLine cal per applyCalLevel
  //We put the necessary data into lineCalArray, and set actualLineCalLevel to 0(None), 1 (BaseLine)
  //or 2 (Full LineCal) to indicate the level of line cal actually installed.
    //If desiredCalLevel=2 and LineCal is current, install it

  if (vars->calInProgress)    //ver115-1e
  {
        //Here we want no type of cal, so signal nothing is installed but don't clear the actual data
    SignalNoCalInstalled();   //ver116-4b
    return;
  }

  if (desiredCalLevel==2)
  {
    int isCurr=BandLineCalIsCurrent();
        //If we are already applying cal, and it is valid and the installed time stamp matches the band cal, we are done
    if (installedBandLineTimeStamp==bandLineTimeStamp && applyCalLevel==2 && isCurr) return; //ver115-2d
    if (isCurr) applyCalLevel=2; else applyCalLevel=1;
  }
  else
  {
    applyCalLevel=1; //base cal
  }

  if (applyCalLevel==2)
  {
    installedBaseLineNumSteps=-1;   //Indicate that base line cal is not installed  ver115-1b fixed typo
    for (int i=0; i <= vars->globalSteps; i++)    //retrieve data
    {
        vars->lineCalArray[i][0]=vars->bandLineCal[i][0]; //freq    //ver115-5c
        vars->lineCalArray[i][1]=vars->bandLineCal[i][1]; //mag
        vars->lineCalArray[i][2]=vars->bandLineCal[i][2]; //Phase
    }
    installedBandLineTimeStamp=bandLineTimeStamp;    //ver115-2d
    return;
  }

    //We get here if applyCalLevel<2 or we did not have a current LineCal, so use BaseLineCal
  if (desiredCalLevel>0)
  {
    if (BaseLineCalIsCurrent()) applyCalLevel=1; else applyCalLevel=0;  //ver115-1b
  }
  else
  {
    applyCalLevel=0;
  }

  if (applyCalLevel==1)
  {
    //We want and have BaseLine cal, so install it
    if (BaseLineCalIsInstalled())
      return;   //Already installed with these sweep params

    int doPhase, doParams;
    //ver114-5p changed interpolation to use the new Interpolation Module
    if (vars->msaMode=="SA" || vars->msaMode=="ScalarTrans")
    {
      doPhase=0; doParams=1;   //do mag only
    }
    else
    {
        doPhase=1;doParams=3;   //Do both mag and phase
    }
    inter.intSetMaxNumPoints(1+qMax(baseLineNumSteps, vars->globalSteps));  //Be sure we have room ver115-9d
    inter.intClearSrc(); inter.intClearDest();
    for (int i=0; i <= baseLineNumSteps; i++) //copy cal table to intSrc
    {
        inter.intAddSrcEntry(vars->baseLineCal[i][0],vars->baseLineCal[i][1],util.NormalizePhase(vars->baseLineCal[i][2]));
    }
    for (int i=1; i <= vars->globalSteps+1; i++)
    {
      inter.intAddDestFreq(gGetPointXVal(GraphVal, MaxPoints, i));   //Install frequencies in intDest
    }

    inter.intSrcToDest(doPhase, 0, doParams);  //Do the actual interpolation into intDest()
    for (int i=0; i <= vars->globalSteps; i++)  //put the data where we want it
    {
      int f,m,p;
      inter.intGetDest(i+1,f, m, p);
      vars->lineCalArray[i][0]=f;
      //ver115-2d eliminated rounding
      vars->lineCalArray[i][1]=m;
      p=util.NormalizePhase(p);
      vars->lineCalArray[i][2]=p;
    }

    //Save the sweep params under which we installed base line cal
    installedBaseLineStartFreq=vars->startfreq;
    installedBaseLineEndFreq=vars->endfreq;
    installedBaseLineNumSteps=vars->globalSteps;
    installedBaseLineLinear=xIsLinear;
    installedBaseLineTimeStamp=baseLineTimeStamp;    //ver115-2d
    //ver115-1b xLL  deleted installedBaseLinePath$
    return;
  }  //end of applying baseLine cal

  //Here we want no type of line cal
  SignalNoCalInstalled();   //ver116-4b
}


float dialogVNACal::gGetPointXVal(Q2DfloatVector &GraphVal, int MaxPoints, float N)
{
  int x;
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
