#include "dialogoslcal.h"
#include "ui_dialogoslcal.h"

dialogOSLCal::dialogOSLCal(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogOSLCal)
{
  ui->setupUi(this);
}

dialogOSLCal::~dialogOSLCal()
{
  delete ui;
}
void dialogOSLCal::BandOSLCalIsCurrent()
{/*
function BandOSLCalIsCurrent()  'Returns 1 if band OSL cal data was taken at current sweep settings
    'Current means the band cal exists and matches current sweep params; it may not actually be installed
    call gGetXAxisRange xMin, xMax
    if globalSteps=OSLBandNumSteps and gGetXIsLinear()=OSLBandLinear and path$=OSLBandPath$ and _
                    xMin=OSLBandStartFreq and xMax=OSLBandEndFreq and _
                    OSLBandS11JigType$=S11JigType$ then isCurr=1 else isCurr=0
    if isCurr then 'do further tests for matching jig characteristics
        if S11JigType$="Trans" then  'ver115-2c
            if OSLBandS21JigAttach$<>S21JigAttach$ or OSLBandS21JigR0<>S21JigR0 then isCurr=0
        else
            if OSLBandS11BridgeR0<>S11BridgeR0 then isCurr=0
        end if
    end if
    BandOSLCalIsCurrent=isCurr
end function
*/
}

void dialogOSLCal::BaseOSLCalIsCurrent()
{/*
function BaseOSLCalIsCurrent()  'Returns 1 if OSL base cal has valid data even if different sweep params
    'Current does not mean it is actually installed
    if OSLBaseNumSteps>0 and path$=OSLBasePath$ and OSLBaseS11JigType$=S11JigType$ then _
                        isCurr=1 else isCurr=0
    if isCurr then 'do further tests for matching jig characteristics
        if S11JigType$="Trans" then
            if OSLBaseS21JigAttach$<>S21JigAttach$ or OSLBaseS21JigR0<>S21JigR0 then isCurr=0
        else
            if OSLBaseS11BridgeR0<>S11BridgeR0 then isCurr=0
        end if
    end if
    if isCurr then
       BaseOSLCalIsCurrent=1
    else
       BaseOSLCalIsCurrent=0 : installedOSLBaseNumSteps=-1 : OSLBaseNumSteps=-1 'Mark everything as not valid
    end if
end function
*/
}

void dialogOSLCal::BaseOSLCalIsInstalled()
{
  /*
function BaseOSLCalIsInstalled()  'Returns 1 if base OSL line cal is already installed at current sweep params
     'Current means the full line cal exists and was last installed with sweep params matching the current params
    if BaseOSLCalIsCurrent()=0 then BaseOSLCalIsInstalled=0 : exit function   'Don't have current base cal
         'If the installed Base cal has different time stamp from current Base cal, then current is not installed
    if installedOSLBaseTimeStamp$<>OSLBaseTimeStamp$ then BaseOSLCalIsInstalled=0 : exit function    'ver115-2d
    call gGetXAxisRange xMin, xMax
    if globalSteps=installedOSLBaseNumSteps and gGetXIsLinear()=installedOSLBaseLinear and _
                        xMin=installedOSLBaseStartFreq and xMax=installedOSLBaseEndFreq then _
                            BaseOSLCalIsInstalled=1 else BaseOSLCalIsInstalled=0
end function
*/
}

void dialogOSLCal::InstallSelectedOSLCal()
{
  /*
//Apply base or band OSL cal per applyCalLevel
    //We put the necessary data into lineCalArray and OSLx(), and set applyCalLevel to 0(None), 1 (BaseLine)
    //or 2 (Full LineCal) to indicate the level of line cal actually installed.
        //If desiredCalLevel=2 and Cal is current, install it
    //If a calibration is in progress, we don't install a cal, but use a zero line cal reference.

    if calInProgress then    //ver115-1e
            //Here we want no type of cal
        call SignalNoCalInstalled   //ver116-4b
        exit sub
    end if

    if desiredCalLevel=2 then
        isCurr=BandOSLCalIsCurrent()
            //If we are already applying cal, and it is valid and the installed time stamp matches the band cal, we are done
        if installedOSLBandTimeStamp$=OSLBandTimeStamp$ and applyCalLevel=2 and isCurr then exit sub //ver115-2d
        if isCurr then applyCalLevel=2 else applyCalLevel=1
    else
        applyCalLevel=1
    end if

    if applyCalLevel=2 then  //We have determined to install Band cal
        installedOSLBaseNumSteps=-1   //Indicate that base cal is not installed
    else
            //We get here if applyCalLevel<2 or we did not have a current band cal, so use base cal if available
        if desiredCalLevel>0 then
            if BaseOSLCalIsCurrent() then applyCalLevel=1 else applyCalLevel=0
        else
            applyCalLevel=0
        end if
    end if

    if applyCalLevel=0 then    //ver115-1e
            //Here we want no type of OSL cal
        call SignalNoCalInstalled   //ver116-4b
        exit sub    //ver116-4b
    end if

        //Note that OSLBandRef and OSLBaseRef hold data from updating the cal. Freq is set and data is cleared whenever full cal is done.
    if applyCalLevel=1 then //base cal--must interpolate. First due reference info
        if BaseOSLCalIsInstalled() then exit sub   //Already installed with these sweep params
            //Save the sweep params under which we installed base OSL cal
        installedOSLBaseStartFreq=startfreq
        installedOSLBaseEndFreq=endfreq
        installedOSLBaseNumSteps=globalSteps
        installedOSLBaseLinear=gGetXIsLinear()
        OSLRefType$=OSLBaseRefType$ : installedOSLBaseRefType$=OSLBaseRefType$  //ver116-4n
        installedOSLBaseTimeStamp$=OSLBaseTimeStamp$    //ver115-2d

            //Create source interpolation table and fill in desired dest frequencies
        call intSetMaxNumPoints 1+max(OSLBaseNumSteps, globalSteps)  //Be sure we have room ver115-9d
        call intClearSrc : call intClearDest    // ver115-2d

        for i=0 to OSLBaseNumSteps //copy cal table to intSrc ver115-2d
            call intAddSrcEntry OSLBaseRef(i,0),OSLBaseRef(i,1),gNormalizePhase(OSLBaseRef(i,2))    //reference data from any cal update
        next i
        for i=1 to globalSteps+1
            call intAddDestFreq gGetPointXVal(i)   //Install the desired frequencies intDest
        next i

            //Do the actual interpolation into intDest()
            //1 means data is polar , 0 means do linear interp, 3 means do both params
        call intSrcToDest 1, 0, 3
            //put the data where we want it; it is now in intDest
        for i=0 to globalSteps
            call intGetDest i+1,f, m, p
            lineCalArray(i,0)=f
            //ver115-2d eliminated rounding
            lineCalArray(i,1)=m
            p=gNormalizePhase(p)
            lineCalArray(i,2)=p
        next i
    else    //Band cal--no interpolation, just copy
        OSLRefType$=OSLBandRefType$
        installedOSLBandTimeStamp$=OSLBandTimeStamp$    //ver115-2d
        for i=0 to globalSteps
            lineCalArray(i,0)=OSLBandRef(i,0)
            lineCalArray(i,1)=OSLBandRef(i,1)
            lineCalArray(i,2)=OSLBandRef(i,2)
        next i
    end if

        //Next install OSL coefficients
    if applyCalLevel=1 then  //base cal--interpolate coefficients a, b, c
        //Note the desired frequencies are already in intDest
        call intClearSrc    //Clear source from which we interpolate ver115-2d
                    //First install the a coefficients into OSLa(). Note these are in rectangular form
        for i=0 to OSLBaseNumSteps //copy cal table to intSrc
            call intAddSrcEntry OSLBaseRef(i,0),OSLBaseA(i,0),OSLBaseA(i,1)
        next i

        //Do the actual interpolation into intDest()
        //First 0 means not polar, second 0 means do linear interp, 3 means do both params
        call intSrcToDest 0, 0, 3
        for i=0 to globalSteps  //put the data where we want it
            call intGetDest i+1,f, real, imag
            OSLa(i,0)=real
            OSLa(i,1)=imag
        next i

            //install the b coefficients into OSLb(). Note these are in rectangular form
        call intClearSrc    //Clear source from which we interpolate ver115-2d
        for i=0 to OSLBaseNumSteps //copy cal table to intSrc
            call intAddSrcEntry OSLBaseRef(i,0),OSLBaseB(i,0),OSLBaseB(i,1)
        next i

        //Do the actual interpolation into intDest()
        //First 0 means not polar, second 0 means do linear interp, 3 means do both params
        call intSrcToDest 0, 0, 3
        for i=0 to globalSteps  //put the data where we want it
            call intGetDest i+1,f, real, imag
            OSLb(i,0)=real
            OSLb(i,1)=imag
        next i

            //install the c coefficients into OSLc(). Note these are in rectangular form
        call intClearSrc    //Clear source from which we interpolate ver115-2d
        for i=0 to OSLBaseNumSteps //copy cal table to intSrc
            call intAddSrcEntry OSLBaseRef(i,0),OSLBaseC(i,0),OSLBaseC(i,1)
        next i

        //Do the actual interpolation into intDest()
        //First 0 means not polar, second 0 means do linear interp, 3 means do both params
        call intSrcToDest 0, 0, 3
        for i=0 to globalSteps  //put the data where we want it
            call intGetDest i+1,f, real, imag
            OSLc(i,0)=real
            OSLc(i,1)=imag
        next i
    else        //band cal--just copy the coefficients
        for i=0 to globalSteps
            OSLa(i,0)=OSLBandA(i,0) : OSLa(i,1)=OSLBandA(i,1)
            OSLb(i,0)=OSLBandB(i,0) : OSLb(i,1)=OSLBandB(i,1)
            OSLc(i,0)=OSLBandC(i,0) : OSLc(i,1)=OSLBandC(i,1)
        next i
    end if

end sub
*/
}

void dialogOSLCal::setUwork(cWorkArray *newuWork)
{
  uWork = newuWork;
}

void dialogOSLCal::PerformOSLCal()
{/*
[PerformOSLCal]     'Perform band OSL cal and return
'We ask the user whether to perform O, S or L. When he clicks Done, we use the available data
'to create the calibration results. Any combo of O,S and L is allowed so long as it includes
'O or S. We put the data into OSLcalOpen(),OSLcalOpen() and/or OSLcalOpen for the caller to process.
'We also set flags OSLdoneO, OSLdoneS and OSLdoneL to let the caller know which were performed.
    OSLdoneO=0 : OSLdoneL=0 : OSLdoneS=0    'For the caller's info. Set to 1 when specified cal is done.
    WindowWidth = 580 : WindowHeight = 400
    UpperLeftX = 0  'Relative to main cal window, since it is a dialog
    UpperLeftY = -100

    BackgroundColor$="buttonface" : ForegroundColor$="black"
    TextboxColor$ = "white" : ComboboxColor$="white"

            '--Instructions
    s$="Specify the fixture used. For Shunt fixture, you may specify a delay time."
    statictext #OSLcal.Inst1, s$,20,15,200,50

    s$="Specify the desired calibration. For OSL, specify characteristics of the standards."
    statictext #OSLcal.Inst2, s$,290,15,200,50

        'Option to choose full OSL or reference cal 'added by ver115-1g
    checkbox #OSLcal.Full, "", [OSLCheckFull], [OSLCheckFull], 300, 100, 20,20
    statictext #OSLcal, "Full OSL", 321, 102, 70,20
    checkbox #OSLcal.Ref, "", [OSLCheckRef], [OSLCheckRef], 300, 120, 20,20
    statictext #OSLcal, "Reference Cal", 321, 122, 70,16

    OSLBtnTop=155 : OSLBtnLeft=280
    button #OSLcal.O, "Perform Open", [OSLdoOpen],UL,OSLBtnLeft, OSLBtnTop, 85,25
    button #OSLcal.S, "Perform Short", [OSLdoShort],UL,OSLBtnLeft, OSLBtnTop+45, 85,25
    button #OSLcal.L, "Perform Load", [OSLdoLoad],UL,OSLBtnLeft, OSLBtnTop+90, 85,25
    statictext #OSLcal.doneO,"", OSLBtnLeft-33,OSLBtnTop+2,32,20
    statictext #OSLcal.doneS,"", OSLBtnLeft-33,OSLBtnTop+47,32,20
    statictext #OSLcal.doneL,"", OSLBtnLeft-33,OSLBtnTop+92,32,20

        'Preset cal standard sets
    call OSLGetCalSets  'Loads cal set names into OSLCalSetNames$() ver115-7a
    OSLCalSetNames$(OSLCalSetNumber)="Custom"   'Add Custom, which has no file
    OSLCalSetNumber=OSLCalSetNumber+1
    combobox #OSLcal.stdSet, OSLCalSetNames$(),[OSLSelectSet], OSLBtnLeft+90,OSLBtnTop+140, 180, 120  'ver115-7a
    statictext #OSLcal.stdLab, "Calibration Standards",OSLBtnLeft+120, OSLBtnTop+120, 140, 18
    statictext #OSLcal.stdDescrip, "",OSLBtnLeft+50, OSLBtnTop+170, 220, 40    'ver116-4i

        'Open, Short and Load Specs
    textbox #OSLcal.OpenSpecs, OSLBtnLeft+90, OSLBtnTop, 200, 20
    textbox #OSLcal.ShortSpecs, OSLBtnLeft+90, OSLBtnTop+47, 200, 20
    textbox #OSLcal.LoadSpecs, OSLBtnLeft+90, OSLBtnTop+94, 200, 20

                'R0
    statictext #OSLcal.statictext15, "Fixture R0 (ohms)", 20, OSLBtnTop-40,  90,  20
    textbox #OSLcal.R0, 110, OSLBtnTop-40,  40,  20

        'Fixture type
    groupbox #OSLcal.FixGroup, "Fixture Type", 20, OSLBtnTop, 190,  120
    checkbox #OSLcal.Bridge, "", [OSLcalSetBridge], [OSLcalSetBridge], 30, OSLBtnTop+20,  15,  20
    checkbox #OSLcal.Series, "", [OSLcalSetSeries], [OSLcalSetSeries], 30, OSLBtnTop+45,  15,  20
    checkbox #OSLcal.Shunt, "", [OSLcalSetShunt], [OSLcalSetShunt], 30, OSLBtnTop+70,  15,  20
    statictext #OSLcal.BridgeLabel, "Reflection Bridge", 50, OSLBtnTop+21, 150,  20
    statictext #OSLcal.SeriesLabel, "Series", 50, OSLBtnTop+46, 50,  20
    statictext #OSLcal.ShuntLabel, "Shunt", 50, OSLBtnTop+71, 40,  16
    statictext #OSLcal.DelayLabel, "Connect Delay (ns)", 100, OSLBtnTop+73, 110,  16
    textbox #OSLcal.ShuntDelay, 110, OSLBtnTop+90, 65,  20

            'Done, Cancel and Help buttons
    button #OSLcal.Done, "Done", [OSLDone],UL, 30, OSLBtnTop+140, 70, 30
    button #OSLcal.Cancel, "Cancel", [OSLCancel],UL, 120, OSLBtnTop+140, 70, 30
    button #OSLcal.Help, "Help", ExplainJigType,UL, 90, 70, 40, 20
    button #OSLcal.Help2, "Help", ExplainOSL,UL, 360, 70, 40, 20

            'Open the dialog
    open "Reflection Calibration" for dialog_modal as #OSLcal
    print #OSLcal, "trapclose [OSLFinished]"
    #OSLcal, "font ms_sans_serif 9"
    #OSLcal.Inst1, "!font Arial 10"
    #OSLcal.Inst2, "!font Arial 10"
    OSLSaveLastSelectedCalSet=OSLLastSelectedCalSet 'In case we need to restore on cancel
    gosub [OSLEnterPresetValues]
    if OSLCalSetNames$(OSLLastSelectedCalSet-1)="Custom" then    'ver115-4i
        'If we start with Custom, enter the most recently used values
        #OSLcal.OpenSpecs, OSLOpenSpec$ : #OSLcal.ShortSpecs, OSLShortSpec$ : #OSLcal.LoadSpecs, OSLLoadSpec$
    end if

    #OSLcal.stdSet, "selectindex "; OSLLastSelectedCalSet   'Select proper cal set
    #OSLcal.stdSet, "setfocus "

    #OSLcal.ShuntDelay, S21JigShuntDelay
    #OSLcal.ShuntDelay, "!hide" : #OSLcal.DelayLabel, "!hide"
    #OSLcal.R0, S21JigR0    'We display either S21JigR0 or S11BridgeR0
    if S11JigType$="Reflect" then
        #OSLcal.Bridge, "set" : #OSLcal.R0, S11BridgeR0
    else    'ver115-2a
        if S21JigAttach$="Series" then #OSLcal.Series, "set"
        if S21JigAttach$="Shunt" then #OSLcal.Shunt, "set" : #OSLcal.ShuntDelay, "!show" : #OSLcal.DelayLabel, "!show"
    end if
    if OSLcalLastUsedFull then goto [OSLCheckFull] else goto [OSLCheckRef]   'to match last time dialog was open
    'can't get here
*/
}

void dialogOSLCal::OSLEnterPresetValues()
{/*
[OSLEnterPresetValues]    'Enter values for preset standard set number presetNum and set box status
    if OSLCalSetNumber<OSLLastSelectedCalSet then OSLLastSelectedCalSet=0
    if OSLLastSelectedCalSet=0 then
        'This is first time we have entered this dialog, so use "Ideal 50 ohms" if available
        OSLLastSelectedCalSet=1 'In case we don't find Ideal 50 ohms
        for i=1 to OSLCalSetNumber
            if upper$(OSLCalSetNames$(i-1))="IDEAL 50 OHMS" then OSLLastSelectedCalSet=i
        next
    end if

    if OSLCalSetNames$(OSLLastSelectedCalSet-1)="Custom" then status$="!enable" else status$="!disable"
    #OSLcal.OpenSpecs, status$ : #OSLcal.ShortSpecs, status$ : #OSLcal.LoadSpecs, status$
    if OSLCalSetNames$(OSLLastSelectedCalSet-1)="Custom" then #OSLcal.stdDescrip, "" : return    'Don't enter any values for custom--leave what is there

    fHndl$=OSLOpenFile$(DefaultDir$;"\MSA_Info\OperatingCal\CalSets\";OSLCalSetFileNames$(OSLLastSelectedCalSet-1), 1)    'Open this file for input
    if fHndl$="" then notice "Error opening cal set file" : return    'Blank means error,
    call OSLGetFileData$ fHndl$ 'Get data into variables OSLFile...
    close #fHndl$   'Close this file
    #OSLcal.OpenSpecs, OSLFileOpenSpec$ : #OSLcal.ShortSpecs, OSLFileShortSpec$ : #OSLcal.LoadSpecs, OSLFileLoadSpec$
    #OSLcal.stdDescrip, OSLFileCalSetDescription$   'ver116-4i
    return
*/
}

void dialogOSLCal::OSLcalSetBridge()
{/*
[OSLcalSetBridge]
    #OSLcal.Bridge, "set" : #OSLcal.Series, "reset" : #OSLcal.Shunt, "reset"
    #OSLcal.ShuntDelay, "!hide" : #OSLcal.DelayLabel, "!hide"
    #OSLcal.Full, "value? OSLFullval$"
    if OSLFullval$="set" then
        #OSLcal.O, "!show" : #OSLcal.S, "!show" : #OSLcal.L, "!show"
        #OSLcal.doneO, "!show" : #OSLcal.doneS, "!show" : #OSLcal.doneL, "!show"     'ver116-4k
        #OSLcal.OpenSpecs, "!show" : #OSLcal.ShortSpecs, "!show" : #OSLcal.LoadSpecs, "!show" 'ver116-4k
    else
        #OSLcal.O, "!show" : #OSLcal.S, "!show" : #OSLcal.L, "!hide"
        #OSLcal.doneO, "!show" : #OSLcal.doneS, "!show" : #OSLcal.doneL, "!hide"     'ver116-4ka
        #OSLcal.OpenSpecs, "!show" : #OSLcal.ShortSpecs, "!show" : #OSLcal.LoadSpecs, "!hide" 'ver116-4k
    end if
    #OSLcal.R0, S11BridgeR0
    wait
*/
}

void dialogOSLCal::OSLcalSetSeries()
{/*
[OSLcalSetSeries]
    #OSLcal.Series, "set" : #OSLcal.Bridge, "reset" : #OSLcal.Shunt, "reset"
    #OSLcal.ShuntDelay, "!hide" : #OSLcal.DelayLabel, "!hide"
    #OSLcal.Full, "value? OSLFullval$"
    if OSLFullval$="set" then
        #OSLcal.O, "!show" : #OSLcal.S, "!show" : #OSLcal.L, "!show"
        #OSLcal.doneO, "!show" : #OSLcal.doneS, "!show" : #OSLcal.doneL, "!show"    'ver116-4k
        #OSLcal.OpenSpecs, "!show" : #OSLcal.ShortSpecs, "!show" : #OSLcal.LoadSpecs, "!show" 'ver116-4k
    else
        #OSLcal.O, "!hide" : #OSLcal.S, "!show" : #OSLcal.L, "!hide"
        #OSLcal.doneO, "!hide" : #OSLcal.doneS, "!show" : #OSLcal.doneL, "!hide"    'ver115-3a
        #OSLcal.OpenSpecs, "!hide" : #OSLcal.ShortSpecs, "!show" : #OSLcal.LoadSpecs, "!hide" 'ver116-4k
    end if
    #OSLcal.R0, S21JigR0
    wait
*/
}

void dialogOSLCal::OSLcalSetShunt()
{/*
[OSLcalSetShunt]
    #OSLcal.Shunt, "set" : #OSLcal.Bridge, "reset" : #OSLcal.Series, "reset"
    #OSLcal.ShuntDelay, "!show" : #OSLcal.DelayLabel, "!show"
    #OSLcal.Full, "value? OSLFullval$"
    if OSLFullval$="set" then
        #OSLcal.O, "!show" : #OSLcal.S, "!show" : #OSLcal.L, "!show"
        #OSLcal.doneO, "!show" : #OSLcal.doneS, "!show" : #OSLcal.doneL, "!show"    'ver116-4k
        #OSLcal.OpenSpecs, "!show" : #OSLcal.ShortSpecs, "!show" : #OSLcal.LoadSpecs, "!show" 'ver116-4k
    else
        #OSLcal.O, "!show" : #OSLcal.S, "!hide" : #OSLcal.L, "!hide"
        #OSLcal.doneO, "!show" : #OSLcal.doneS, "!hide" : #OSLcal.doneL, "!hide"    'ver115-3a
        #OSLcal.OpenSpecs, "!show" : #OSLcal.ShortSpecs, "!hide" : #OSLcal.LoadSpecs, "!hide" 'ver116-4k
    end if
    #OSLcal.R0, S21JigR0
    wait
*/
}

void dialogOSLCal::OSLCheckFull()
{/*
[OSLCheckFull]
    OSLcalLastUsedFull=1    'ver116-4n
    #OSLcal.Full, "set" : #OSLcal.Ref, "reset"
    #OSLcal.OpenSpecs, "!show" : #OSLcal.ShortSpecs, "!show" : #OSLcal.LoadSpecs, "!show" 'ver116-4k
    #OSLcal.O, "!show" : #OSLcal.S, "!show" : #OSLcal.L, "!show"
    #OSLcal.doneO, "!show" : #OSLcal.doneS, "!show" : #OSLcal.doneL, "!show"    'ver115-4j

    #OSLcal.FixGroup, "!hide" : #OSLcal.Bridge, "hide" : #OSLcal.Series, "hide"
    #OSLcal.Shunt, "hide"
    #OSLcal.BridgeLabel, "!hide" : #OSLcal.SeriesLabel, "!hide" : #OSLcal.ShuntLabel, "!hide"
    #OSLcal.DelayLabel, "!hide" : #OSLcal.ShuntDelay, "!hide"
    #OSLcal.stdSet, "show"  'show selection of cal standards ver116-4i
    #OSLcal.stdLab "!show" : #OSLcal.stdDescrip, "!show"
    wait
*/
}

void dialogOSLCal::OSLCheckRef()
{/*
[OSLCheckRef]
    OSLcalLastUsedFull=0    'ver116-4n
    #OSLcal.Full, "reset" : #OSLcal.Ref, "set"
    #OSLcal.O, "!hide" : #OSLcal.S, "!hide" : #OSLcal.L, "!hide"    'will change below
    #OSLcal.Bridge, "value? bridgeVal$"
    if bridgeVal$="set" then
        #OSLcal.O, "!show" : #OSLcal.S, "!show"    'Can do open or short as ref
        #OSLcal.DelayLabel, "!hide"
        #OSLcal.ShuntDelay, "!hide"
        #OSLcal.OpenSpecs, "!show" : #OSLcal.ShortSpecs, "!show" : #OSLcal.LoadSpecs, "!hide" 'ver116-4k
    else
        #OSLcal.Series, "value? seriesVal$"
        if seriesVal$="set" then 'ver115-5c
            #OSLcal.S, "!show"          'short for series, open for shunt
            #OSLcal.OpenSpecs, "!hide" : #OSLcal.ShortSpecs, "!show" : #OSLcal.LoadSpecs, "!hide" 'ver116-4k
            #OSLcal.DelayLabel, "!hide"
            #OSLcal.ShuntDelay, "!hide"
        else 'shunt fixture
            #OSLcal.O, "!show" : #OSLcal.DelayLabel, "!show" : #OSLcal.ShuntDelay, "!show"
            #OSLcal.OpenSpecs, "!show" : #OSLcal.ShortSpecs, "!hide" : #OSLcal.LoadSpecs, "!hide" 'ver116-4k
        end if
    end if
    #OSLcal.FixGroup, "!show"   'ver115-4a
    #OSLcal.Bridge, "show" : #OSLcal.Series, "show" : #OSLcal.Shunt, "show"
    #OSLcal.BridgeLabel, "!show" : #OSLcal.SeriesLabel, "!show" : #OSLcal.ShuntLabel, "!show"
    'ver116-4k let cal standard selection items remain showing
    wait
*/
}

void dialogOSLCal::SetOSLCalCheckboxStatus()
{/*
sub SetOSLCalCheckboxStatus stat$ 'Set checkboxes to disable or enable
    #OSLcal.Bridge, stat$ : #OSLcal.Series, stat$ : #OSLcal.Shunt, stat$
    #OSLcal.Full, stat$ : #OSLcal.Ref, stat$
end sub
*/
}

void dialogOSLCal::OSLCancel()
{/*
[OSLCancel]
    OSLdoneO=0 : OSLdoneL=0 : OSLdoneS=0
    OSLLastSelectedCalSet=OSLSaveLastSelectedCalSet
    close #OSLcal
    return    'We will be back in the basic cal window ver115-5b
*/
}

void dialogOSLCal::OSLFinished()
{/*
[OSLFinished]
    wait    'user must explicitly click Done or Cancel
*/
}

void dialogOSLCal::OSLDone()
{/*
[OSLDone]
    'User has performed the desired calibrations.
    'The caller must now calculate/set up the calibration data.
    'But either the Open or Short must have been performed.
    'We know what was done based on OSLdoneO, OSLdoneS and OSdoneL
        'Retrieve test fixture info
    #OSLcal.Full, "value? OSLFullval$"  'ver116-4k
    'if doing full OSL, we treat all fixtures as bridges
    if OSLFullval$="set" then #OSLcal.Bridge, "set" : #OSLcal.Series, "reset" : #OSLcal.Shunt, "reset"
    #OSLcal.Bridge, "value? bridgeVal$"
    isBridge=0
    isSeries=0
    if bridgeVal$="set" then
        isBridge=1
        S11JigType$="Reflect"   'bridge
    else    'must be series or shunt
        #OSLcal.Series, "value? attachVal$"
        if attachVal$="set" then
            S21JigAttach$="Series"
            isSeries=1
        else
            S21JigAttach$="Shunt"
            #OSLcal.ShuntDelay, "!contents? delayVal$"  'connector delay (ns)
            S21JigShuntDelay=val(uCompact$(delayVal$))
        end if
        S11JigType$="Trans"
    end if
    #OSLcal.R0, "!contents? R0$"
    S11BridgeR0=uValWithMult(R0$) : if S11BridgeR0<=0 then S11BridgeR0=50 : notice "Invalid R0. 50 ohms used."
        'We now set both S11BridgeR0 and S21JigR0 to the same value. When doing full OSL, the user does not choose the
        'fixture type so we wouldn't know which one to set if we didn't set both. Certain routines refer to one or the other,
        'depending on fixture type, but they will get the same value either way. ver116-4j
    S21JigR0=S11BridgeR0

    'Note S11GraphR0 is set in the sweep parameters window
    OSLcalSum=OSLdoneO+OSLdoneL+OSLdoneS

    'If full OSL, we require all 3 cals to be done. If reference, we quit as soon as one is done. This means
    'we never leave with 2 cals done.
    if OSLFullval$="set" then   'set means full OSL. Note if not full we quit as soon as one is done.
        if OSLcalSum<>3 then notice "You are missing one of the necessary calibrations for full OSL." : wait
    else
        if isBridge then    'Be sure reference cal was done
            didRef=(OSLdoneO or OSLdoneS)
        else
            if isSeries then didRef=OSLdoneS else didRef=OSLdoneO
        end if
        if didRef=0 then notice "You have not performed the necessary calibration" : wait
    end if
        'ver115-3f moved the disables to here, after a possible wait occurs
    call SetOSLCalCheckboxStatus "disable"    'So nothing gets clicked before dialog closes ver115-3a
    call SetOSLCalButtonStatus "!disable"

    #OSLcal.stdSet, "selectionindex? OSLLastSelectedCalSet"
    #OSLcal.OpenSpecs, "!contents? OSLOpenSpec$"
    #OSLcal.ShortSpecs, "!contents? OSLShortSpec$"
    #OSLcal.LoadSpecs, "!contents? OSLLoadSpec$"

    if OSLError then OSLBandNumSteps=-1 'Invalid cal data if math error occurred 'ver116-4b
    call SignalNoCalInstalled   'ver116-4b 'So new cal will get installed on Restart
    desiredCalLevel=2   'desire Band Sweep since we just did it
    call RequireRestart 'So cal gets installed before user proceeds, but we don't install it here
         'In earlier versions this line was put here.
        'because we sometimes returned to a strange place. Doesn't seem to happen now.
    cursor hourglass    'ver116-4b
    gosub [ProcessOSLBandCal]   'ver115-1g
    cursor normal   'ver116-4b
    close #OSLcal
    'ver115-5b moved update of band cal display to the routines for the basic cal window
    return 'we will be back in the basic cal window, or whoever called us
*/
}

void dialogOSLCal::OSLSelectSet()
{/*
[OSLSelectSet]   'User Selected preset cal standard
        'This is separate from OSLEnterPresetValues because when the dialog is first opened we just
        'call that routine, since doing the selectionindex? seems to mess up the box.
    #OSLcal.stdSet, "selectionindex? s"
    'ver116-4i allowed any completed cal data to remain
    OSLLastSelectedCalSet=s
    gosub [OSLEnterPresetValues]
    wait
*/
}

void dialogOSLCal::OSLdoOpen()
{/*
[OSLdoOpen]
    if calInProgress then gosub [OSLCalAborted] : wait  'Button says abort when cal is running
    call SetOSLCalButtonStatus "!disable"
    #OSLcal.O, "!enable"
    #OSLcal.O, "Abort Cal"
    #OSLcal.doneO, ""   'ver115-7a
    gosub [OSLdoCal]    'get cal data by doing one sweep
        'For S11JigType$="Reflect", the data in ReflectArray will be reflection per the bridge R0 w/o OSL adjustment.
        'It is normally adjusted for planeadj, but in cal we force planeadj to 0.
        'For S11JigType$="Trans", the data in ReflectArray is "S21" of the fixture (unadjusted for any reference)
        'which we are treating as the tentative estimate of reflection, even though it is far from true reflection.
    for i=0 to steps    'retrieve data re bridge R0
        OSLcalOpen(i,0)=ReflectArray(i,constIntermedS11DB)  'was put here during scan ver115-7a
        OSLcalOpen(i,1)=ReflectArray(i,constIntermedS11Ang) 'ver115-7a
    next i
    call SetOSLCalButtonStatus "!enable"
    #OSLcal.O, "Perform Open"
    #OSLcal.doneO, "Done"
    OSLdoneO=1
    #OSLcal.Full, "value? OSLFullval$"
    if OSLFullval$="reset" then goto [OSLDone]  'If Reference cal, quit after one is done
    wait
*/
}

void dialogOSLCal::OSLdoShort()
{/*
[OSLdoShort]
    if calInProgress then gosub [OSLCalAborted] : wait  'Button says abort when cal is running
    call SetOSLCalButtonStatus "!disable"
    #OSLcal.S, "!enable"
    #OSLcal.S, "Abort Cal"
    #OSLcal.doneS, ""   'ver115-7d
    gosub [OSLdoCal]   'get cal data by doing one sweep
    for i=0 to steps    'retrieve data re bridge R0
        OSLcalShort(i,0)=ReflectArray(i,constIntermedS11DB)  'was put here during scan 'ver115-7a
        OSLcalShort(i,1)=ReflectArray(i,constIntermedS11Ang)    'ver115-7a
    next i
    call SetOSLCalButtonStatus "!enable"
    #OSLcal.S, "Perform Short"
    #OSLcal.doneS, "Done"
    OSLdoneS=1
    #OSLcal.Full, "value? OSLFullval$"
    if OSLFullval$="reset" then goto [OSLDone]  'If Reference cal, quit after one is done
    wait
*/
}

void dialogOSLCal::OSLdoLoad()
{/*
[OSLdoLoad]
    if calInProgress then gosub [OSLCalAborted] : wait  'Button says abort when cal is running
    call SetOSLCalButtonStatus "!disable"
    #OSLcal.L, "!enable"
    #OSLcal.L, "Abort Cal"
    #OSLcal.doneL, ""   'ver115-7a
    gosub [OSLdoCal]   'get cal data by doing one sweep
    for i=0 to steps    'retrieve data re bridge R0
        OSLcalLoad(i,0)=ReflectArray(i,constIntermedS11DB)  'was put here during scan 'ver115-7a
        OSLcalLoad(i,1)=ReflectArray(i,constIntermedS11Ang) 'ver115-7a
    next i
    call SetOSLCalButtonStatus "!enable"
    #OSLcal.L, "Perform Load"
    #OSLcal.doneL, "Done"
    OSLdoneL=1          'ver115-2c
    #OSLcal.Full, "value? OSLFullval$"
    if OSLFullval$="reset" then goto [OSLDone]  'If Reference cal, quit after one is done
    wait
*/
}

void dialogOSLCal::SetOSLCalButtonStatus()
{/*
sub SetOSLCalButtonStatus stat$ 'Set button status. stat$ is either "!enable" or "!disable"
    #OSLcal.O, stat$
    #OSLcal.S, stat$
    #OSLcal.L, stat$
    #OSLcal.Done, stat$
    #OSLcal.Cancel, stat$
    #OSLcal.Help, stat$    'ver115-3a
    #OSLcal.Help2, stat$    'ver115-3a
end sub
*/
}

void dialogOSLCal::OSLCalAborted()
{/*
[OSLCalAborted]
    'OSL cal was aborted. Restore status and let user proceed
    call SetOSLCalCheckboxStatus "enable"   'ver116-4i
    call SetOSLCalButtonStatus "!enable"
    #OSLcal.O, "Perform Open"   'Restore button names--one was set to Abort Cal
    #OSLcal.S, "Perform Short"
    #OSLcal.L, "Perform Load"
    calInProgress=0
    specialOneSweep=0
    sweepDir=saveSweepDir : alternateSweep=saveAlternate
    planeadj=savePlaneAdj : wate=saveWate
    message$="Cal Aborted"  'ver115-2d
    call PrintMessage
    wait
*/
}

void dialogOSLCal::OSLdoCal()
{/*
[OSLdoCal]   'Run OSL Calibration for specified standard
    calInProgress=1 : call SignalNoCalInstalled   'ver116-4n
    call SetOSLCalCheckboxStatus "disable"    'ver115-3a
    haltsweep=0     'So Restart will actually restart. ver114-4f
    specialOneSweep=1
    saveAlternate=alternateSweep : saveSweepDir=sweepDir : savePlaneAdj=planeadj
    alternateSweep=0 : sweepDir=1
    call FunctSetVideoAndAutoWait 1    '1 means save old settings    'ver116-4b
    planeadj=0  'So phase will not be affected
        'Note with calInProgress=1, the cal installation routine will not install anything and sets applyCalLevel=0
    gosub [Restart] 'Perform one sweep and fill datatable(,)
    specialOneSweep=0
    sweepDir=saveSweepDir : alternateSweep=saveAlternate
    planeadj=savePlaneAdj
    call FunctRestoreVideoAndAutoWait   'ver116-4b
    'Cal data is now in datatable
    calInProgress=0
    call SetOSLCalCheckboxStatus "enable"    'ver115-3a
return
*/
}

void dialogOSLCal::ProcessOSLBandCal()
{/*
[ProcessOSLBandCal]   'Process already gathered band cal data
    call ProcessOSLCal
    desiredCalLevel=2   'opt for using band cal since we just did it
    if OSLError then BandSweepCalDone=0 : OSLBandNumSteps=-1 : return    'If error, nullify the cal and return
                'Save the conditions under which the cal was done
    OSLBandStartFreq=startfreq
    OSLBandEndFreq=endfreq
    OSLBandNumSteps=steps
    OSLBandLinear=gGetXIsLinear()
    OSLBandPath$=path$
    OSLBandS11JigType$=S11JigType$
    OSLBandS21JigAttach$=S21JigAttach$
    OSLBandS11BridgeR0=S11BridgeR0
    OSLBandS21JigR0=S21JigR0
    OSLBandTimeStamp$=date$("mm/dd/yy"); "; ";time$()
    BandSweepCalDone=1
    return
*/
}

void dialogOSLCal::ExplainOSL()
{/*
sub ExplainOSL h$ 'Put up dialog explaining reflection jig type
    WindowWidth = 635
    WindowHeight = 200
    UpperLeftX=0    'Doesn't seem to matter what is specified here
    UpperLeftY=150
    BackgroundColor$="gray"
    ForegroundColor$="black"
    statictext #Explain, "All measurements in Reflection mode require some form of calibration. The simplest calibration is",  10, 10, 630,  20
    statictext #Explain, "Reference Calibration, which uses either an Open or a Short calibration standard, which essentially",  10, 30, 630,  20
    statictext #Explain, "allows the MSA to adjust to set a reference signal level. This works well for precise fixtures at",  10, 50, 630,  20
    statictext #Explain, "relatively low frequencies. OSL Calibration involves more steps but is more accurate. OSL involves",  10, 70, 630,  20
    statictext #Explain, "sequentially measuring Open, Short and Load calibration standards, in any order. ",  10, 90, 630,  20
    statictext #Explain, "For OSL Calibration, you generally specify the electrical characteristics of each of the standards.",  10, 110, 630,  20
    statictext #Explain, "At low frequencies it may work well just to treat them as Ideal.",  10, 130, 630,  20
    open "Reflection Calibration Help" for dialog_modal as #Explain
    #Explain, "trapclose [OSLExplainFinished]"
    print #Explain, "font ms_sans_serif 10"
    wait
*/
}

void dialogOSLCal::OSLExplainFinished()
{/*
[OSLExplainFinished]
    close #Explain
end sub
*/
}

void dialogOSLCal::ExplainJigType()
{/*
sub ExplainJigType h$ 'Put up dialog explaining reflection jig type
    WindowWidth = 600
    WindowHeight = 370
    UpperLeftX=0    'Doesn't seem to matter what is specified here
    UpperLeftY=150
    BackgroundColor$="gray"
    ForegroundColor$="black"
    s$="Reflection measurements use one of several types of test fixtures to which the device under test (DUT)"
    s$=s$;" is attached. Many fixtures are Reflection Bridges, which make an attempt to make their output signal"
    s$=s$;" correspond closely to the actual reflection from the DUT. Such fixtures can use Reference Calibration"
    s$=s$;" with either the Open or Short, or for better accuracy can use full OSL calibration."
    statictext #JigExplain, s$, 20, 20, 550, 80

    s$="Other fixtures produce an output which does not directly correspond to the DUT reflection, but can be"
    s$=s$;" mathematically transformed into a reflection measurement. These test fixtures"
    s$=s$;" typically consist of an attenuator, then the component, then another attenuator. The component"
    s$=s$;" may be connected in Series between the attenuators, or may be Shunt to ground, which accounts for the"
    s$=s$;" two different fixture types. The component will see a certain resistance R0 looking at the incoming signal"
    s$=s$;" and the outgoing signal. You must specify that R0, usually 50 ohms. The Series fixture is calibrated"
    s$=s$;" with just the Short or with full OSL. The Shunt fixture is calibrated with just the Open or with full OSL."
    statictext #JigExplain, s$,  20,  120, 550,  140

    s$="If you specify the Shunt Fixture, then you may also specify the one-way connection delay time,"
    s$=s$;" which is used for compensation when you do not use full OSL. A delay of 0.115 ns is typical."
    s$=s$;" For low frequencies a delay of 0 ns may be adequate."
    statictext #JigExplain, s$,  20, 270, 550,  60

    open "Reflection Fixture Help" for dialog_modal as #JigExplain
    #JigExplain, "trapclose [JigExplainFinished]"
    print #JigExplain, "font ms_sans_serif 10"
    wait
*/
}

void dialogOSLCal::JigExplainFinished()
{/*
[JigExplainFinished]
    close #JigExplain
end sub
*/
}

void dialogOSLCal::OSLOpenFile()
{/*
function OSLOpenFile$(fName$, asInput)  'Open file with OSL standards
    'fName is full file name, with path and extension
    'Returns LB file handle, or blank if error
    'asInput=1 means open input file; otherwise open output file
    On Error goto [noFile]
    if asInput then open fName$ for input as #calSetFile else open fName$ for output as #calSetFile
    OSLOpenFile$="#calSetFile"
    exit function
[noFile]
    OSLOpenFile$=""
end function
*/
}

void dialogOSLCal::OSLGetFileData()
{/*
sub OSLGetFileData$ fHndl$  'Get description of OSL standard modver116-4i
    'The file handle is in fHndl
    'The data is put into global variables describing the current OSL standards
    'The file consists of the following lines:
    'Name=xxx     the short name to enter in the combo box
    'Description=...    long description to display when this set is selected (optional)
    'Open=...
    'Short=...
    'Load=...
    'Each ... spec is a simplified standard RLC spec Example: RLC[S or P, R25, L10n, C200p,D35f]
    'S or P indicates series or parallel. R, L and C are ohms, H, and F. D is delay in seconds.
    'Only the relevant components need be specified.
    hadName=0: hadOpen=0 : hadShort=0 : hadLoad=0 : OSLCalSetDescription$=""
    while EOF(#fHndl$)=0
        Line Input #fHndl$, tLine$
        equalPos=instr(tLine$,"=")
        if equalPos>2 then
            'Note that lines without valid tags are allowed; we ignore them
            OSLid$=Upper$(Trim$(Left$(tLine$, equalPos-1))) 'everything left of the equal sign is the ID
            tLine$=Mid$(tLine$, equalPos+1) 'We will use everything right of the equal sign
            bracketPos=inStr(tLine$,"[")    '0 if no bracket on this line--we will add brackets for backward compatibility ver116-4i
            select case OSLid$
                case "NAME"
                    OSLFileCalSetName$=Trim$(tLine$) : hadName=1
                case "DESCRIPTION"  'ver116-4i
                    OSLFileCalSetDescription$=Trim$(tLine$) 'optional
                case "OPEN"
                    hadOpen=1 : if bracketPos=0 then tLine$="RLC[";tLine$;"]"
                    OSLFileOpenSpec$=tLine$ 'Entire remainder of line is the RLC spec
                case "SHORT"
                    hadShort=1 : if bracketPos=0 then tLine$="RLC[";tLine$;"]"
                    OSLFileShortSpec$=tLine$ 'Entire remainder of line is the RLC spec
                case "LOAD"
                    hadLoad=1 : if bracketPos=0 then tLine$="RLC[";tLine$;"]"
                    OSLFileLoadSpec$=tLine$ 'Entire remainder of line is the RLC spec
                case else 'Ignore other lines
            end select
        end if
    wend
    if hadName=0 or hadOpen=0 or hadShort=0 or hadLoad=0 then
        if hadName then n$=OSLFileCalSetName$ else n$="unnamed standard"
        notice "Incomplete cal standard file for ";n$;"."
    end if
end sub
*/
}

void dialogOSLCal::OSLSaveCalSetFile()
{/*
sub OSLSaveCalSetFile fName$ 'Save current cal set data as a file named fName$ (includes full path) modver116-4i
    fHndl$=OSLOpenFile$(fName$,0)   'Open file for output
    if fHndl$="" then notice "Error creating cal set file.": exit sub
    print #fHndl$, "Name=";OSLFileCalSetName$
    print #fHndl$, "Description=";OSLFileCalSetDescription$
    print #fHndl$, "Open=";OSLFileOpenSpec$
    print #fHndl$, "Short=";OSLFileShortSpec$
    print #fHndl$, "Load=";OSLFileLoadSpec$
    close #fHndl$
end sub
*/
}

void dialogOSLCal::OSLGetCalSets()
{/*
sub OSLGetCalSets 'Load cal set names
    'Loads file and set names for all files in the folder \MSA_Info\OperatingCal\CalSets folder
    filePath$=DefaultDir$;"\MSA_Info\OperatingCal\CalSets"
    files DefaultDir$;"\MSA_Info\OperatingCal", "", fileInfo$()  'get directory list
    nFolders=val(fileInfo$(0,1))
    folderFound=0
    for i=1 to nFolders
        if fileInfo$(i,1)="CalSets" then folderFound=1 : exit for    'We found the CalSets folder
    next i
    if folderFound=0 then   'create file for Ideal, if no folder found exists
        if mkDir(filePath$)<>0 then notice "Error creating CalSets folder.": exit sub  'Folder doesn't exist, so make one
        'ver116-4i consolidated the data for each standard into one spec
        OSLFileCalSetName$="Ideal 50 ohms"
        OSLFileCalSetDescription$="50 ohms, good for cal plane on back of SMA connectors"
        OSLFileOpenSpec$="RLC[S, C0, D0]"
        OSLFileShortSpec$="RLC[P, R0, L0, D0]" : OSLFileLoadSpec$="RLC[P,R50, C0]"
        call OSLSaveCalSetFile filePath$;"\Ideal50.txt"    'Save Ideal 50 ohms file per the above values 'ver116-4i
    end if
    files filePath$, "*.txt", fileInfo$()  'get list of files in CalSets folder
    nFiles=val(fileInfo$(0,0))
    redim OSLCalSetNames$(nFiles+1) : redim OSLCalSetFileNames$(nFiles+1)    'Clear and Make room for list plus "Custom"
    OSLCalSetNumber=0
    for i=1 to nFiles  'Read cal set name of each file
        fName$=fileInfo$(i,0) 'Name of this file, without path
        fHndl$=OSLOpenFile$(filePath$;"\";fName$, 1)    'Open this file for input
        if fHndl$<>"" then      'Blank means error, so we ignore file
            OSLCalSetFileNames$(i-1)=fName$ 'store file name (without path)
            call OSLGetFileData$ fHndl$ 'name is the only data we will use here
            OSLCalSetNames$(i-1)=OSLFileCalSetName$ 'store name that was set in OSLGetFileData$
            OSLCalSetNumber=OSLCalSetNumber+1
            close #fHndl$   'Close this file
        end if
    next i
end sub
*/
}

void dialogOSLCal::ProcessOSLCal()
{/*
sub ProcessOSLCal     'Calc coefficients and reference data from raw OSL scan data
    'OSLdoneO, OSLdoneSand OSLdoneS indicate which of Open, Short, and Load were performed.
    'The relevant calibration data is in OSLcalOpen(), OSLcalShort() and OSLcalLoad()
    'Calibration data will be the raw S11 for reflection bridge, and raw S21 for the transmission jigs.
    'In the latter case we must convert to S11 before calculating OSL coefficients.
    '
    'The measured cal data is in OSLcalOpen(), etc., as dB/angle. It is reflection data for the reflection bridge,
    'For S11JigType$="Reflect", the data in OSLcalOpen(), etc., will be reflection per the bridge w/o OSL adjustment,
    'and without being adjusted for planeadj (which we force to 0 in cal).
    'For S11JigType$="Trans", the data in OSLcalOpen(), etc., is "S21" of the fixture (unadjusted for any reference)
    'which we are treating as the tentative estimate of reflection, even though it is far from true reflection.

    'When raw data is collected in future scans, it will be adjusted by a reference installed into the line
    'cal array. The reference will be the calibration data from the Open, except for the series jig it is the
    'calibration data from the Short. In the case of a Reflection Bridge if we don't have the relevant
    'Open data, we compute it from the Short. We assume Ideal values for the non-reference data in order to calc coefficients.
    '
    'We put the OSL coefficients into OSLBandA(), OSLBandB() and OSLBandC()

'DEBUG
'    for i=0 to globalSteps
'        OSLcalOpen(i,0)=0 : OSLcalOpen(i,1)=0
'        OSLcalShort(i,0)=42.638 : OSLcalShort(i,1)=-93.4375
'        OSLcalLoad(i,0)=39.166 : OSLcalLoad(i,1)=-92.76
'    next i
    calSum=OSLdoneO+OSLdoneS+OSLdoneL   'number of cals done. Will be 1 or 3
    if calSum=0 then notice "No calibration performed" : OSLError=1 : exit sub
    if calSum=2 then notice "Incomplete calibration performed" : OSLError=1 : exit sub   'Our OSL procedure actually never allows this to happen
    if calSum=1 and OSLdoneL then notice "Incomplete calibration performed" : OSLError=1  : exit sub   'Our OSL procedure actually never allows this to happen

    'Calc cal standard data for computing coefficients. For reference cal, we don't need all of them,
    'but there is no harm in calculating them.
    call CalcOSLStandards   'ver116-4k

    'If we have just one standard it will be reference cal with Open (bridge or shunt) or Short(bridge or series),
    'but may not be ideal standard. We make up data for the missing standards so we can do regular OSL. ver116-4n
    if calSum=1 then
        'Either the Open or Short was used as the reference. Even though the cal standard may not be ideal, we use this
        'as the (line cal) reference, because it is easy to repeat when doing Update Cal. We also need to create the two missing standards.
        'They will be created as ideal, so we must also be sure that the description of the standard is ideal, so we may override
        'CalcOSLStandards that was just performed. We need to find how the used standard would have performed if ideal, in order
        'to create the missing standards.
        if OSLdoneO then OSLBandRefType$="Open" else OSLBandRefType$="Short"
        for i=0 to globalSteps 'modver116-4n
            if OSLdoneO then
                'Note the measured values are the raw measurements of the data, which are the fixture transmission
                'values but are not adjusted to any reference.
                S11R=OSLstdOpen(i,0) : S11I=OSLstdOpen(i,1)  'Actual standard S11, real/imag
                measDB=OSLcalOpen(i,0) : measDeg=OSLcalOpen(i,1)    'measured standard
            else 'short
                S11R=OSLstdShort(i,0) : S11I=OSLstdShort(i,1) 'Actual standard S11, real/imag
                measDB=OSLcalShort(i,0) : measDeg=OSLcalShort(i,1)    'measured standard
            end if
            S11Mag=sqr(S11R^2+S11I^2) : S11dB=20*uSafeLog10(S11Mag) : S11Deg=uATan2(S11R, S11I) 'The calibration standard S11 spec in dB/angle format

            if S11JigType$="Reflect" then
                'Using bridge with reference cal. Reference may be Open or Short
                if S11dB<-40 then notice "Reference calibration standard is too close to "; S11BridgeR0; " ohms." : OSLError=1 : exit sub
                if OSLdoneO then
                    OSLcalOpen(i,0)=0 : OSLcalOpen(i,1)=0  'Open adjusted by itself as reference
                    OSLstdShort(i,0)=-1 : OSLstdShort(i,1)=0   'Force spec of Short to be ideal  (real/imag)
                    OSLcalShort(i,0)=0-S11dB : OSLcalShort(i,1)=uNormalizeDegrees(-180-S11Deg)  'Pretend we measured the ideal Short and subtracted Open as reference (dB/ang form)
                else    'short is the reference
                    OSLcalShort(i,0)=0 : OSLcalShort(i,1)=0  'Adjust short with itself as reference, resulting in 0 (dB/ang form)
                    OSLstdOpen(i,0)=1 : OSLstdOpen(i,1)=0   'Force spec of Open to be ideal  (real/imag)
                    OSLcalOpen(i,0)=0-S11dB : OSLcalOpen(i,1)=uNormalizeDegrees(0-S11Deg) 'Pretend we measured the ideal Open and subtracted Short as reference (dB/ang form)
                end if
                OSLstdLoad(i,0)=0 : OSLstdLoad(i,1)=0  'Force spec of load to be ideal (real/imag)
                OSLcalLoad(i,0)=-99-S11dB : OSLcalLoad(i,1)=0  'Pretend we measured the ideal load (dB/ang form); don't worry about angle
            else
                if S21JigAttach$="Shunt" then   'calc S21 that this refco would produce
                    'For shunt fixture with no delay and a perfect Open, the ideal Load response would be 3.52 dB below the Open response and
                    'the ideal Short response would be zero. A non-zero delay or imperfect Open require us to calculate how the response of the actual Open,
                    'the ideal Load and the ideal Short would be transformed by the delay. We will use the measured Open as the reference for future
                    'measurements (i.e. it will be subtracted from the raw data the same as line cal). Therefore, whatever S21 we calculate for the actual Open,
                    'ideal Short or ideal Load, would produce reference-adjusted readings equal to that S21 minus that open's calculated S21. That is, use of the
                    'measured open as a reference makes its net reading zero, and the ideal Load and Short need to be similarly adjusted
                        'The delay is assumed to be in a line of 50-ohms, not S21JigR0. modver116-4k
                    phaseDelay=ReflectArray(i,0)*0.36*S21JigShuntDelay   'one way delay in degrees
                    call uS11DBToImpedance S21JigR0, S11dB, S11Deg, impR, impX       'calc impedance
                    call uPhaseShiftImpedance 50, phaseDelay, impR, impX    'Calculate effective impedance of Open with 50-ohm delay
                    call uImpedanceToRefco S21JigR0, impR, impX, S11Mag, S11Deg   'calc actual Open S11 re S21JigR0 taking into account the delay
                    call uRefcoToShuntS21DB S11Mag, S11Deg, S21dB, S21Deg   'calc S21 from the refco of the effective impedance
                    if S11dB>-0.25 and (S11Deg<-165 or S11Deg>165) then _
                        notice "Reference calibration standard is too close to a Short." : OSLError=1 : exit sub

                    impR=0 : impX=0 'impedance of ideal Short
                    call uPhaseShiftImpedance 50, phaseDelay, impR, impX    'Calculate effective impedance of ideal Short
                    call uImpedanceToRefco S21JigR0, impR, impX, idealShortS11Mag, idealShortS11Deg   'calc ideal Short response taking into account the delay
                    call uRefcoToShuntS21DB idealShortS11Mag, idealShortS11Deg, idealShortS21dB, idealShortS21Deg   'calc S21 from the refco of the effective impedance
                        'Adjust by the measured Open reading
                    idealShortS21dB=idealShortS21dB - S21dB : idealShortS21Deg=uNormalizeDegrees(idealShortS21Deg - S21Deg)

                    impR=S21JigR0 : impX=0  'impedance of ideal Load
                    call uPhaseShiftImpedance 50, phaseDelay, impR, impX    'Calculate effective impedance of ideal Load
                    call uImpedanceToRefco S21JigR0, impR, impX, idealLoadS11Mag, idealLoadS11Deg   'calc ideal Load response taking into account the delay
                    call uRefcoToShuntS21DB idealLoadS11Mag, idealLoadS11Deg, idealLoadS21dB, idealLoadS21Deg   'calc S21 from the refco of the effective impedance
                       'Adjust by the same amount that the measured Open was high
                    idealLoadS21dB=idealLoadS21dB - S21dB : idealLoadS21Deg=uNormalizeDegrees(idealLoadS21Deg  - S21Deg)

                    OSLcalOpen(i,0)=0 : OSLcalOpen(i,1)=0  'Adjust open with itself as reference, resulting in 0 (dB/ang form)
                    OSLstdShort(i,0)=-1 : OSLstdShort(i,1)=0   'Force spec of short to be ideal (real/imag)
                    OSLcalShort(i,0)=idealShortS21dB : OSLcalShort(i,1)=idealShortS21Deg  'Pretend we measured the ideal short
                    OSLstdLoad(i,0)=0 : OSLstdLoad(i,1)=0   'Force spec of load to be ideal (real/imag)
                    OSLcalLoad(i,0)=idealLoadS21dB : OSLcalLoad(i,1)=idealLoadS21Deg  'Pretend we measured the ideal load
                else    'series fixture
                    'Series is similar to shunt, except cal is with Short, and we don't deal with delays so everything is simpler.
                    call uRefcoToSeriesS21DB S11Mag, S11Deg, S21dB, S21Deg      'S21 that the actual short would produce
                    if S11dB>-0.25 and S11Deg<5 and S11Deg>-5 then _
                        notice "Reference calibration standard is too close to an Open." : OSLError=1 : exit sub
                    OSLcalShort(i,0)=0 : OSLcalShort(i,1)=0  'Adjust short with itself as reference, resulting in 0 (dB/ang form)
                    OSLstdOpen(i,0)=1 : OSLstdOpen(i,1)=0   'Force spec of Open to be ideal  (real/imag)
                    OSLcalOpen(i,0)=-99-S21dB : OSLcalOpen(i,1)=uNormalizeDegrees(0-S21Deg)   'Pretend we measured the ideal Open (dB/ang form)
                    OSLstdLoad(i,0)=0 : OSLstdLoad(i,1)=0  'Force spec of load to be ideal (real/imag)
                    'Load would be 3.52 dB below ideal short; actual short would produce S21dB@S21Deg but we set measured short to zero,
                    'so we also have to subtract S21dB@S21Deg from ideal load.
                    OSLcalLoad(i,0)=-3.52-S21dB : OSLcalLoad(i,1)=uNormalizeDegrees(0-S21Deg)  'Pretend we measured the ideal load (dB/ang form)
                end if
            end if
            OSLBandRef(i,0)=ReflectArray(i,0)    'freq--actual tuning freq, not equiv 1G freq
            OSLBandRef(i,1)=measDB :OSLBandRef(i,2)=measDeg 'save reference   'Save the measured reference
        next i
    else    'All three standards used   'ver116-4n
        'We need to determine what to use for the reference. It will be Open, Short or Load, whichever has the largest value.
        'The goal is to avoid the Open with a series fixture or a Short with a shunt fixture.
        open1=OSLcalOpen(0,0) : load1=OSLcalLoad(0,0) : short1=OSLcalShort(0,0)
        if open1>=load1 then    'choose biggest
            if open1>=short1 then OSLBandRefType$="Open" else OSLBandRefType$="Short"
        else
            if load1>short1 then OSLBandRefType$="Load" else OSLBandRefType$="Short"
        end if

        for i=0 to globalSteps  'copy reference and adjust measurements per reference
            OSLBandRef(i,0)=ReflectArray(i,0)    'freq--actual tuning freq, not equiv 1G freq
            select case OSLBandRefType$
                case "Open" : refdB=OSLcalOpen(i,0) : refDeg=OSLcalOpen(i,1)
                case "Short" : refdB=OSLcalShort(i,0) : refDeg=OSLcalShort(i,1)
                case else : refdB=OSLcalLoad(i,0) : refDeg=OSLcalLoad(i,1)
            end select
            OSLBandRef(i,1)=refdB :OSLBandRef(i,2)=refDeg 'save reference
            OSLcalOpen(i,0)=OSLcalOpen(i,0)-refdB : OSLcalOpen(i,1)=uNormalizeDegrees(OSLcalOpen(i,1)-refDeg)   'Adjust measurements per reference
            OSLcalLoad(i,0)=OSLcalLoad(i,0)-refdB : OSLcalLoad(i,1)=uNormalizeDegrees(OSLcalLoad(i,1)-refDeg)
            OSLcalShort(i,0)=OSLcalShort(i,0)-refdB : OSLcalShort(i,1)=uNormalizeDegrees(OSLcalShort(i,1)-refDeg)
        next i
    end if

    'ver116-4k deleted creation of missing standard when we have two. Current procedure never allows us to end up with two.
    OSLError=0  'ver116-4k

    'We want to convert the OSLcalxxx() data into S11 in rectangular form (real, imaginary), and calc OSL coefficients.
    'We leave the reference data in db, ang (degrees) format
    kDegToRad=uRadsPerDegree()
    for i=0 to globalSteps
        rho=10^(OSLcalOpen(i,0)/20) : rad=OSLcalOpen(i,1)*kDegToRad 'to polar, radians
        OSLcalOpen(i,0)=cos(rad)*rho : OSLcalOpen(i,1)=sin(rad)*rho 'polar to rectangular
        rho=10^(OSLcalLoad(i,0)/20) : rad=OSLcalLoad(i,1)*kDegToRad
        OSLcalLoad(i,0)=cos(rad)*rho : OSLcalLoad(i,1)=sin(rad)*rho
        rho=10^(OSLcalShort(i,0)/20) : rad=OSLcalShort(i,1)*kDegToRad
        OSLcalShort(i,0)=cos(rad)*rho : OSLcalShort(i,1)=sin(rad)*rho
    next i

    call CalcOSLCoeff   'Calculate A, B, C coefficients; set OSLError to 1 if math error
end sub

    'Here we use the measured reflection coefficients for the open, load and short
    'to determine OSL coefficients a, b and c. These in turn can be used
    'to calculate actual reflection coefficients from measured reflection coefficients
    'by the following formulas:
    '       M=(S+a)/(b*S+c)
    '       S = (a - cM)/(bM - 1)
    'where S is the actual reflection coefficient and M is the measured reflection coefficient.
    'For subscripting variables in code and in the comments, the suffixes O, L and S mean
    'Open, Load and Short, respectively. For example, SL is the actual
    'reflection coefficient of the load; MS is the measured reflection loss of the short.
    '
    'The measured return losses for the open, load and short are determined
    'at each frequency step before invoking this method, and placed into the arrays
    'OSXOpenR, OSXOpenI, OSXLoadR, OSXLoadI, OSXShortR and OSXShortI. Variable names
    'use the suffixes R and I for the real and imaginary parts, respectively.
    'We could first do a line calibration with the Open
    'and then measure just the load and short, assigning a perfect "1" to the Open.
    'But to keep this calculation generic, if that is done the entries for the measured
    'Open should just be assigned a value of 1 before invoking CalcOSL.
    'We allow for the actual return losses of the open and short
    'to be something other than ideal, by using arrays to hold
    'their values at each frequency step.

*/
}

void dialogOSLCal::CalcOSLCoeff1()
{/*
sub CalcOSLCoeff1   'Calc coeff a, b, c for base or band OSL cal
    'OSLcalOpen(), OSLcalShort() and OSLcalLoad() have the raw calibration data     (real, imag form)
    'OSLstdOpen and OSLstdOpen() have the S11 data for the Open and Short standards (real, imag form)
    'MO,ML, MS are the measured responses with the open,  load and short attached
    'SO, SL, SS are the actual reflection coeff. of the open, short and load standards
    '(SL is assumed=0)
    'All must be in real, imaginary form
    'The adjustment is made by the following formula:
    '      S = (a - cM)/(bM - 1)  This model is different from that used in CalcOSLCoeff
    'where S is the actual reflection coefficient and M is the measured reflection coefficient.
    '
    'The coefficients a, b and c are calculated according to the following formulas:
    'K1= ML-MO; approx. -1
    'K2= MS-ML; appro.x -1
    'K3= MO-MS; approx. 2
    '
    'D=MS*SS*K1 + MO*SO*K2; approx -2
    '
    'c=SO*SS*K3 / D
    'a=ML*c
    'b=[SO*K2 + SS*K1] / D

    for calStep=0 to globalSteps
        MOr=OSLcalOpen(calStep,0) : MOi=OSLcalOpen(calStep,1)     'Measured open, real and imag
        MLr=OSLcalLoad(calStep,0) : MLi=OSLcalLoad(calStep,1)     'Measured load, real and imag
        MSr=OSLcalShort(calStep,0) : MSi=OSLcalShort(calStep,1)     'Measured short, real and imag
        SOr=OSLstdOpen(calStep,0) : SOi=OSLstdOpen(calStep,1)     'Open standard, real and imag
        SSr=OSLstdShort(calStep,0) : SSi=OSLstdShort(calStep,1)     'Short standard, real and imag

            'Compute Ks
        K1r=MLr-MOr : K1i=MLi-MOi   'K1, real and imag
        K2r=MSr-MLr : K2i=MSi-MLi   'K2, real and imag
        K3r=MOr-MSr : K3i=MOi-MSi   'K3, real and imag

            'Compute 1/D
        Wr=MSr*SSr-MSi*SSi : Wi=MSr*SSi+MSi*SSr     'MS*SS
        Xr=Wr*K1r-Wi*K1i : Xi=Wr*K1i+Wi*K1r     'MS*SS*K1
        Yr=MOr*SOr-MOi*SOi : Yi=MOr*SOi+MOi*SOr     'MO*SO
        Zr=Yr*K2r-Yi*K2i : Zi=Yr*K2i+Yi*K2r     'MO*SO*K2
        Dr=Xr+Zr : Di=Xi+Zi     'D=MS*SS*K1 + MO*SO*K2
        if Dr=0 and Di=0 then notice "Divide by zero in calculating OSL coefficients." : OSLError=1 : exit sub  'ver115-4j
        call cxInvert Dr, Di, DinvR, DinvI       'Invert of D is in Dinv
            'Compute c
        Wr=SOr*SSr-SOi*SSi : Wi=SOr*SSi+SOi*SSr     'SO*SS
        Xr=Wr*K3r-Wi*K3i : Xi=Wr*K3i+Wi*K3r     'X=SO*SS*K3
        cr=DinvR*Xr-DinvI*Xi : ci=DinvR*Xi+DinvI*Xr     'c=X/D
            'Compute a
        ar=MLr*cr-MLi*ci : ai=MLr*ci+MLi*cr     'a=ML*c
            'Compute b
        Wr=SOr*K2r-SOi*K2i : Wi=SOr*K2i+SOi*SSr     'SO*K2
        Xr=SSr*K1r-SSi*K1i : Xi=SSr*K1i+SSi*K1r     'SS*K1
        Yr=Wr+Xr : Yi=Wi+Xi                    'Y=SO*K2 + SS*K1
        br=DinvR*Yr-DinvI*Yi : bi=DinvR*Yi+DinvI*Yr     'b=Y/D

        'Put coefficients into OSLBandx()
        OSLBandA(calStep,0)=ar : OSLBandA(calStep,1)=ai
        OSLBandB(calStep,0)=br : OSLBandB(calStep,1)=bi
        OSLBandC(calStep,0)=cr : OSLBandC(calStep,1)=ci
    next calStep
end sub
*/
}

void dialogOSLCal::CalcOSLCoeff()
{/*
sub CalcOSLCoeff   'Calc coeff a, b, c for base or band OSL cal
    'We calculate OSL coefficients for the most general case, where no advance assumptions are
    'made about the cal standards.
    'OSLcalOpen(), OSLcalShort() and OSLcalLoad() have the raw calibration data     (real, imag form)
    'OSLstdOpen and OSLstdOpen() have the S11 data for the Open and Short standards (real, imag form)
    'MO,ML, MS are the measured responses with the open,  load and short attached
    'SO, SL, SS are the actual reflection coeff. of the open, short and load standards
    'The error model equation is as follows, where S is the actual S11 and M is
    'the measured S11:
    '  S = (M b) / (a c*M)
    'Using S and M for the Open, Short and Load, we can calculate the
    'coefficients a, b and c.
    'The double letter variables Mx and Sx are the measured and actual values, with
    'the second letter (O, S or L) indicating open, short or load.
    '
    '   K1 = ML-MS
    '   K2 = MS-MO
    '   K3 = MO-ML
    '   K4= SL*SS*K1
    '   K5= SO*SS*K2
    '   K6= SL*SO*K3
    '   K7=SO*K1
    '   K8=SL*K2
    '   K9=SS*K3
    '
    '   D = K4 + K5 + K6
    '
    '   a= (MO*K7 + ML*K8 + MS*K9)/D
    '   b= (MO*K4 + ML*K5 +MS*K6)/D
    '   c = (K7 + K8 + K9)/D

    for calStep=0 to globalSteps
        MOr=OSLcalOpen(calStep,0) : MOi=OSLcalOpen(calStep,1)     'Measured open, real and imag
        MLr=OSLcalLoad(calStep,0) : MLi=OSLcalLoad(calStep,1)     'Measured load, real and imag
        MSr=OSLcalShort(calStep,0) : MSi=OSLcalShort(calStep,1)     'Measured short, real and imag
        SOr=OSLstdOpen(calStep,0) : SOi=OSLstdOpen(calStep,1)     'Open standard, real and imag
        SLr=OSLstdLoad(calStep,0) : SLi=OSLstdLoad(calStep,1)     'Load standard, real and imag
        SSr=OSLstdShort(calStep,0) : SSi=OSLstdShort(calStep,1)     'Short standard, real and imag

        K1r=MLr-MSr : K1i=MLi-MSi   'K1=ML-MS, real and imag
        K2r=MSr-MOr : K2i=MSi-MOi   'K2=MS-MO, real and imag
        K3r=MOr-MLr : K3i=MOi-MLi   'K3=MO-ML, real and imag

        Wr=SLr*SSr-SLi*SSi : Wi=SLr*SSi+SLi*SSr   'W=SL*SS
        K4r=Wr*K1r-Wi*K1i : K4i=Wr*K1i+Wi*K1r     'K4=SL*SS*K1

        Wr=SOr*SSr-SOi*SSi : Wi=SOr*SSi+SOi*SSr   'W=SO*SS
        K5r=Wr*K2r-Wi*K2i : K5i=Wr*K2i+Wi*K2r     'K5=SO*SS*K2

        Wr=SLr*SOr-SLi*SOi : Wi=SLr*SOi+SLi*SOr   'W=SL*SO
        K6r=Wr*K3r-Wi*K3i : K6i=Wr*K3i+Wi*K3r     'K6=SL*SO*K3

        K7r=SOr*K1r-SOi*K1i : K7i=SOr*K1i+SOi*K1r     'K7=SO*K1

        K8r=SLr*K2r-SLi*K2i : K8i=SLr*K2i+SLi*K2r     'K8=SL*K2

        K9r=SSr*K3r-SSi*K3i : K9i=SSr*K3i+SSi*K3r     'K9=SS*K3

        Dr=K4r+K5r+K6r : Di=K4i+K5i+K6i    'D = K4 + K5 + K6
        if Dr=0 and Di=0 then notice "Divide by zero in calculating OSL coefficients." : OSLError=1 : exit sub  'ver115-4j
        call cxInvert Dr, Di, invDr, invDi   'invD= 1/D

        'Now calculate coefficient a
        Wr=MOr*K7r-MOi*K7i : Wi=MOr*K7i+MOi*K7r   'W=MO*K7
        Xr=MLr*K8r-MLi*K8i : Xi=MLr*K8i+MLi*K8r   'X=ML*K8
        Yr=MSr*K9r-MSi*K9i : Yi=MSr*K9i+MSi*K9r   'Y=MS*K9
        Zr=Wr+Xr+Yr : Zi=Wi+Xi+Yi                  'Z=MO*K7 + ML*K8 + MS*K9
        ar=Zr*invDr-Zi*invDi : ai=Zr*invDi+Zi*invDr   'a=(MO*K7 + ML*K8 + MS*K9)/D

        'The procedure for calculating b is identical to that for a,
        'just changing the K values.
        Wr=MOr*K4r-MOi*K4i : Wi=MOr*K4i+MOi*K4r   'W=MO*K4
        Xr=MLr*K5r-MLi*K5i : Xi=MLr*K5i+MLi*K5r   'X=ML*K5
        Yr=MSr*K6r-MSi*K6i : Yi=MSr*K6i+MSi*K6r   'Y=MS*K6
        Zr=Wr+Xr+Yr : Zi=Wi+Xi+Yi                  'Z=MO*K4 + ML*K5 + MS*K6
        br=Zr*invDr-Zi*invDi : bi=Zr*invDi+Zi*invDr   'b=(MO*K4 + ML*K5 + MS*K6)/D

        'Calculate coefficient c.
        Wr=K7r+K8r+K9r : Wi=K7i+K8i+K9i     'W = K7 + K8 + K9
        cr=Wr*invDr-Wi*invDi : ci=Wr*invDi+Wi*invDr   'c = (K7 + K8 + K9)/D

            'Put coefficients into OSLBandx()
        OSLBandA(calStep,0)=ar : OSLBandA(calStep,1)=ai
        OSLBandB(calStep,0)=br : OSLBandB(calStep,1)=bi
        OSLBandC(calStep,0)=cr : OSLBandC(calStep,1)=ci
    next calStep
end sub
*/
}

void dialogOSLCal::CalcOSLStandards()
{/*
sub CalcOSLStandards         'Calculate ref. coeff. of OSL standards
    'The Open, Short and Load are each characterized as either a series or parallel RLC circuit to ground at the end
    'of a coax delay line. This follows the normal format of an RLC spec used by uRLCComboResponse, though it
    'is usually fairly simple.
   'The Open is generally characterized as a time delay plus a fringe capacitance. The Short is generally characterized
   'as a time delay plus a parallel resistance/inductance shunted to ground. The Load is generally characterized
   'as a delay and resistance, and either a parallel capacitance or a series inductance.

   'For each frequency
   'in datatable, calculate the Open and Short response. Put the open response into OSLstdOpen() and
   'Short response into OSLstdShort(), both in real, imag form.
   'The response is calculated relative to the jig or bridge R0, not the graph R0
    if S11JigType$="Trans" then R0=S21JigR0 else R0=S11BridgeR0

    for i=0 to globalSteps :uWorkArray(i+1,0)=ReflectArray(i,0) : next i 'load frequencies into uWorkArray
    uWorkNumPoints=globalSteps+1    'number of points is one more than number of steps
    isErr=uRLCComboResponse(OSLOpenSpec$, R0, "S11") 'Calculate open standard response at all frequencies
    if isErr then notice "Error in OSL standards specifications" : OSLError=1 : exit sub
    radsPerDeg=uRadsPerDegree() 'For conversion from degrees to rads
    for i=0 to globalSteps
        rho=uTenPower(uWorkArray(i+1,1)/20) : theta=uWorkArray(i+1, 2)*radsPerDeg
        OSLstdOpen(i, 0)=rho*cos(theta) : OSLstdOpen(i,1)=rho*sin(theta)    'real, imag format
    next i
    isErr=uRLCComboResponse(OSLShortSpec$, R0, "S11") 'Calculate short standard response at all frequencies
    if isErr then notice "Error in OSL standards specifications" : OSLError=1 : exit sub
    for i=0 to globalSteps
        rho=uTenPower(uWorkArray(i+1,1)/20) : theta=uWorkArray(i+1, 2)*radsPerDeg
        OSLstdShort(i, 0)=rho*cos(theta) : OSLstdShort(i,1)=rho*sin(theta)    'real, imag format
    next i
    isErr=uRLCComboResponse(OSLLoadSpec$, R0, "S11") 'Calculate load standard response at all frequencies
    if isErr then notice "Error in OSL standards specifications" : OSLError=1 : exit sub
    for i=0 to globalSteps
        rho=uTenPower(uWorkArray(i+1,1)/20) : theta=uWorkArray(i+1, 2)*radsPerDeg
        OSLstdLoad(i, 0)=rho*cos(theta) : OSLstdLoad(i,1)=rho*sin(theta)    'real, imag format
    next i
end sub
*/
}

void dialogOSLCal::RunCalUpdate()
{/*
[RunCalUpdate]  'menu item to update calibration ver116-4b
    if haltsweep=1 then gosub [FinishSweeping]
    WindowWidth = 475 : WindowHeight = 235
    call GetDialogPlacement 'set UpperLeftX and UpperLeftY ver115-1c
    UpperLeftY=UpperLeftY-100
    BackgroundColor$="buttonface"
    ForegroundColor$="black"
    TextboxColor$ = "white"
    ComboboxColor$="white"
        'We can update band cal if we have current cal, even if not yet installed.
        'TO DO: For base cal, change the sweep settings to match the cal data. DO a scan and update the base cal data, and
        'save it to file. Change desired cal level to 1 and install cal. For band cal, be sure it matches current sweep params,
        'update the band cal data, change desired cal level to 2 and install cal.
        'To update both reflection and transmission base cal, they must both exist but don't need matching freq range.
        'Note: Currently only band cal update is implemented.
    if msaMode$="Reflection" then bandUpdateIsCurrent=(desiredCalLevel=2 and BandOSLCalIsCurrent()) : baseUpdateIsCurrent=BaseOSLCalIsCurrent() _
                        else bandUpdateIsCurrent=(desiredCalLevel=2 and BandLineCalIsCurrent()) : baseUpdateIsCurrent=BaseOSLCalIsCurrent()
    if bandUpdateIsCurrent=0 then Notice "Must have current band calibration to update. Cannot update base cal." : wait 'ver116-4n
        'update band cal
    if msaMode$="Reflection" then stdType$=OSLBandRefType$;" standard" else stdType$="Through connection"
    s$="To update the currently band calibration, attach the "; stdType$
    s$=s$;" and click Perform Update. This will update the currently installed reference to partially adjust for drift "
    s$=s$;"occurring since the full calibration was performed. "

    statictext #OSLupdate.inst, s$,10,1,450, 70

    updateCalsMatch=0   'Whether other VNA mode has matching calibration
    if BandOSLCalIsCurrent() and BandLineCalIsCurrent() then updateCalsMatch=1

    if msaMode$="VectorTrans" then
        statictext #OSLupdate.DelayLabel, "Delay of Calibration Through Connection (ns):",10,90,270, 18
        textbox #OSLupdate.Delay, 282,90,50, 20
        checkbox #OSLupdate.applyAlso, "Apply update to Reflection Cal as well.",[updateNil], [updateNil],50,115,260, 18
    else    'Reflection
        checkbox #OSLupdate.applyAlso, "Apply update to Transmission Cal as well.",[updateNil], [updateNil],50,115,260, 18
    end if

    button #OSLupdate.Perform, "Perform Open",[PerformCalUpdate], UL, 75, 160, 120,25    'ver114-5f
    button #OSLupdate.Done, "Done",[CalUpdateFinished], UL, 220, 160, 100,25

    open "Update Calibration" for dialog_modal as #OSLupdate  'ver114-3g
    print #OSLupdate, "trapclose [CalUpdateFinished]"   'goto [OSLCalUpdateFinished] if xit is clicked
    print #OSLupdate, "font ms_sans_serif 10"
    if msaMode$="VectorTrans" then #OSLupdate.Perform, "Perform Through" : #OSLupdate.Delay, lineCalThroughDelay  'Delay in ns
    calUpdatePerformed=0
    if updateCalsMatch=0 then #OSLupdate.applyAlso, "disable"   'gray it out in reset mode if not applicable
    wait
*/
}

void dialogOSLCal::updateNil()
{/*
[updateNil] 'nil handler
    wait
*/
}

void dialogOSLCal::PerformCalUpdate()
{/*
[PerformCalUpdate]  'update button clicked
        'We will re-scan the reference data, which should in theory match the current reference data. If a current
        'reading minus the original is D dB @ A degrees, this means we need to subtract that extra
        'amount from all future readings. Essentially, the new data becomes our new reference,
        'which we need to transfer to lineCalArray. In addition, if we have Band cal, we should
        'replace lineCalBandRef() or OSLBandRef(). For Base cal, it is not feasible to adust the Base cal array reference, so
        'we can only adjust the currently installed data in lineCalArray.
    if calInProgress then goto [CalUpdateAborted]   'Perform button is actually Abort while cal in progress
    calUpdatePerformed=1    'set this even if we abort ver116-4b
    #OSLupdate.Perform, "Abort Cal" : #OSLupdate.Done, "!disable" : #OSLupdate.applyAlso, "disable"
    if msaMode$<>"Reflection" then gosub [PerformLineCalUpdate] else gosub [PerformOSLCalUpdate]
    wait
*/
}

void dialogOSLCal::PerformOSLCalUpdate()
{/*
[PerformOSLCalUpdate]
    cursor hourglass
    calInProgress=1 : specialOneSweep=1
    saveAlternate=alternateSweep : saveSweepDir=sweepDir : savePlaneAdj=planeadj : saveWate=wate
    alternateSweep=0 : sweepDir=1
    call FunctSetVideoAndAutoWait 1  'set video filter to narrow and autowait to Normal or Precise; 1 means save settings
    planeadj=0  'So phase will not be affected
        'Note with calInProgress=1, the cal installation routine will not install anything and sets applyCalLevel=0
    gosub [Restart] 'Perform one sweep and fill datatable(,)
    specialOneSweep=0
    calInProgress=0
    call FunctRestoreVideoAndAutoWait
    sweepDir=saveSweepDir : alternateSweep=saveAlternate
    planeadj=savePlaneAdj : wate=saveWate
        'New Cal reference data is now in datatable
    #OSLupdate.applyAlso, "value? applyAlso$"
    for i=0 to steps
        m=datatable(i,2) : p=datatable(i,3) 'get mag and phase; freq doesn't change
        deltaM=m-OSLBandRef(i,1) : deltaP=p-OSLBandRef(i,2) 'change in mag and phase
        OSLBandRef(i,1)=m : OSLBandRef(i,2)=p   'update OSLBandRef
        if applyAlso$="set" then _        'update line cal data if required
                bandLineCal(i,1)=bandLineCal(i,1)+deltaM : bandLineCal(i,2)=gNormalizePhase(bandLineCal(i,2)+deltaP)
    next i
    desiredCalLevel=2 : installedOSLBandTimeStamp$=""   'so installation will occur
    call InstallSelectedOSLCal 'So new data gets installed and installation variables get updated

    OSLBandTimeStamp$=date$("mm/dd/yy"); "; ";time$()
    if applyAlso$="set" then bandLineTimeStamp$=OSLBandTimeStamp$
    cursor normal
    beep
    #OSLupdate.Perform, "Perform Update" : #OSLupdate.Done, "!enable"
    if updateCalsMatch then #OSLupdate.applyAlso, "enable"
    return
*/
}

void dialogOSLCal::PerformLineCalUpdate()
{/*
[PerformLineCalUpdate]
    cursor hourglass
    if msaMode$="VectorTrans" then   'ver115-5a
        #OSLupdate.Delay, "!contents? s$"
        lineCalThroughDelay=val(uCompact$(s$))      'Store in ns; used by [BandLineCal]
    else
        lineCalThroughDelay=0
    end if
        'We use BandLineCal rather than just doing our own sweep because it adjusts for lineCalThroughDelay
        'It will automatically update bandLineCal, so in order for us to have access to the old info
        'we have to be sure it is saved into lineCalArray.
        'lineCalArray data will not be disturbed (and will not be used) during the sweep conducted by [BandLineCal].
    for i=0 to steps : lineCalArray(i,1)=bandLineCal(i,1) : lineCalArray(i,2)=bandLineCal(i,2) : next i
    gosub [BandLineCal] 'Peforms sweep and loads into bandLineCal; saves/restores necessary settings
        'new data is in datatable and bandLineCal; old data is still in lineCalArray
    #OSLupdate.applyAlso, "value? applyAlso$"
    if applyAlso$="set" then   'update OSL data if required
        for i=0 to steps
            m=bandLineCal(i,1) : p=bandLineCal(i,2) 'get mag and phase; freq doesn't change
            deltaM=m-lineCalArray(i,1) : deltaP=p-lineCalArray(i,2) 'change in mag and phase
            OSLBandRef(i,1)=OSLBandRef(i,1)+deltaM : OSLBandRef(i,2)=gNormalizePhase(OSLBandRef(i,2)+deltaP)
        next i
    end if
    desiredCalLevel=2 : installedBandLineTimeStamp$=""   'so installation will occur
    call InstallSelectedLineCal 'So new data gets installed and installation variables get updated
        '[BandLineCal] updates bandLineTimeStamp$
    if applyAlso$="set" then OSLBandTimeStamp$=bandLineTimeStamp$
    #OSLupdate.Perform, "Perform Update" : #OSLupdate.Done, "!enable"
    if updateCalsMatch then #OSLupdate.applyAlso, "enable"
    cursor normal
    return
*/
}

void dialogOSLCal::CalUpdateAborted()
{/*
[CalUpdateAborted]    'abort calibration
    gosub [FinishSweeping] 'Finish nicely
    #OSLupdate.Perform, "Perform Update" : #OSLupdate.Done, "!enable"
    if updateCalsMatch then #OSLupdate.applyAlso, "enable"
    cursor normal
    specialOneSweep=0 : calInProgress=0
    sweepDir=saveSweepDir : alternateSweep=saveAlternate
    planeadj=savePlaneAdj : wate=saveWate
    wait
*/
}

void dialogOSLCal::CalUpdateFinished()
{/*
[CalUpdateFinished]
    if calInProgress=1 then goto [PostScan] 'Don't allow quit in middle of cal
    'Note we do not update the time stamp of the base, band or installed cal. That is needed only to trigger a
    're-install of cal info on Restart. We just installed what is necessary. If Base cal, on Restart no new install
    'will be done so what we just did won't be overridden. If Band cal, it wouldn't matter if a re-install were done.
    cursor normal
    close #OSLupdate
    if calUpdatePerformed then
        call SignalNoCalInstalled   'ver116-4b
        desiredCalLevel=2   'desire BandSweep since we just did it
        call RequireRestart   'so cal will get installed before proceeding
    end if
    wait
'--end of [RunCalUpdate]
*/
}
int dialogOSLCal::OSLCalContextAsTextArray(int isBand)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  //Put  cal points into uTextPointArray$, with header info
  //isBand=1 means we do band cal; =0 means base cal
  //return number of lines placed into uTextPointArray$
  //First line begins with ! and is line 1 of the title: !Log Sweep Path N. "Log" may instead by "Linear"; N=path number
  //First 3 lines are title, each preceded by !
  //Next line is sweep info
  //Next is Touchstone options line
  //Next is comment data headings
  //Then comes each point as its own string

  uWork->uTextPointArray[1]="! OSL Calibration Data";
  uWork->uTextPointArray[2]="!";
  /*
  if (vars->isBand)
  {
    if OSLBandLinear then sweep$="!Linear Sweep; " else sweep$="!Log Sweep "
    uTextPointArray$(3)="!";OSLBandTimeStamp$
    //Save sweep info: log/linear ; path info (in form Path N); Jig attachment and R0
    uTextPointArray$(4)=sweep$;OSLBandPath$;"; S11Jig=";OSLBandS11JigType$; "; S11BridgeR0=";S11BridgeR0; _ //ver115-9d
                "; S21Jig=";OSLBandS21JigAttach$; "; S21JigR0="; OSLBandS21JigR0 //ver115-1b, ver115-1g
  }
  else
  {
    if OSLBaseLinear then sweep$="!Linear Sweep; " else sweep$="!Log Sweep "
     uTextPointArray$(3)="!";OSLBaseTimeStamp$
    //Save sweep info: log/linear ; path info (in form Path N); Jig attachment and R0
    uTextPointArray$(4)=sweep$;OSLBasePath$;"; S11Jig=";OSLBaseS11JigType$; "; S11BridgeR0=";S11BridgeR0; _ //ver115-9d
                "; S21Jig=";OSLBaseS21JigAttach$; "; S21JigR0="; OSLBaseS21JigR0  //ver115-1b, ver115-1g
  }
  uTextPointArray$(5)="!"
  aSpace$=" "
  uTextPointArray$(6)="!  MHz    A_real   A_imag   B_real   B_imag     C_Real     C_Imag     RefDB     RefDeg"
  for i=0 to globalSteps
  {
    //save freq, mag and phase
    if isBand then  //Band cal
    {
        s$=OSLBandA(i,0);aSpace$;OSLBandA(i,1);aSpace$;OSLBandB(i,0);_
                                aSpace$;OSLBandB(i,1);aSpace$; OSLBandC(i,0);aSpace$;OSLBandC(i,1);aSpace$
        uTextPointArray$(i+7)=OSLBandRef(i,0);aSpace$;s$;OSLBandRef(i,1);aSpace$;OSLBandRef(i,2)
    }
    else    //Base cal
    {
        s$=OSLBaseA(i,0);aSpace$;OSLBaseA(i,1);aSpace$;OSLBaseB(i,0);_
                                aSpace$;OSLBaseB(i,1);aSpace$; OSLBaseC(i,0);aSpace$;OSLBaseC(i,1);aSpace$
        uTextPointArray$(i+7)=OSLBaseRef(i,0);aSpace$;s$;OSLBaseRef(i,1);aSpace$;OSLBaseRef(i,2)
    }
  }
  return vars->globalSteps+7; //Number of lines

*/
  return 0;
}
void dialogOSLCal::OSLCalContextToFile(QFile *fHndl, int isBand)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub OSLCalContextToFile fHndl$, isBand   'save OSL cal points to file
    'isBand=1 means we do band cal; =0 means base cal
    'We do not include StartContext or EndContext lines
         'fHndl$ is the handle of an already open file. We output our data
        'but do not close the file.
    nLines=OSLCalContextAsTextArray(isBand)   'Assemble strings into uTextPointArray$
    for i=1 to nLines
        print #fHndl$, uTextPointArray$(i)
    next i
end sub
*/
}
void dialogOSLCal::OSLGetCalContextFromFile(QFile *fHndl, int isBand)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
function OSLGetCalContextFromFile(fHndl$, isBand)   'get points from file; return number of points
    'isBand=1 means we do band cal; =0 means base cal
    'We return number of points read, or -1 for file error
    'fHndl$ is the handle of an already open file. We read our data
    'but do not close the file. The last line we read will be "!EndContext"
    isErr=uArrayFromFile(fHndl$,100) 'Get data, 100 means number per line to be determined
    if isErr then
        OSLGetCalContextFromFile=-1
        if isBand then OSLBandNumSteps=-1 else OSLBaseNumSteps=-1 'Indicates no data
        exit function
    end if
        'Move the data from uWorkArray to gGraphVal
    if isBand then  'Band Cal
        for i=1 to uWorkNumPoints   'ver116-4n
            OSLBandRef(i-1,0)=uWorkArray(i, 0) 'freq
            OSLBandA(i-1,0)=uWorkArray(i, 1) : OSLBandA(i-1,1)=uWorkArray(i, 2) 'Coeff A real and imag
            OSLBandB(i-1,0)=uWorkArray(i, 3) : OSLBandB(i-1,1)=uWorkArray(i, 4) 'Coeff B real and imag
            OSLBandC(i-1,0)=uWorkArray(i, 5) : OSLBandC(i-1,1)=uWorkArray(i, 6) 'Coeff C real and imag
            OSLBandRef(i-1,1)=uWorkArray(i, 7) : OSLBandRef(i-1,2)=uWorkArray(i, 8) 'Mag and phase for ref
        next i
        OSLGetCalContextFromFile=uWorkNumPoints
            'Now derive the sweep parameters from the array and the title ver114-5h
        if uWorkNumPoints<1 then exit function
        OSLBandNumSteps=uWorkNumPoints-1
        OSLBandStartFreq=OSLBandRef(0,0) : OSLBandEndFreq=OSLBandRef(OSLBandNumSteps,0)
        OSLBandTimeStamp$=uWorkTitle$(3)   'uArrayFromFile put date/time stamp here
        sweep$=uWorkTitle$(4)   'uArrayFromFile put sweep info here
        if instr(sweep$,"Linear")>0 then OSLBandLinear=1 else OSLBandLinear=0
                'ver115-1b added the retrieval of the following sweep info
            'Get remaining sweep info from sweep$. Each item has a keyword and ends with a semi-colon or end of line
        calPathNum$=uGetParamText$(sweep$, "Path ", ";")  'Gets text after "Path " to semicolon or end
        if calPathNum$="" then OSLBandPath$="Path 1" else OSLBandPath$="Path ";calPathNum$
        OSLBandS11JigType$=uGetParamText$(sweep$, "S11Jig=", ";")  'Gets text after S11Jig= to semicolon or end ver115-1g
        if OSLBandS11JigType$="" then OSLBandS11JigType$="Reflect"  'ver115-1g
        if OSLBandS11JigType$="Reflect" then    'ver115-9d
            'If using bridge, get bridge R0
            OSLBandS11BridgeR0=val(uGetParamText$(sweep$, "S11BridgeR0=", ";"))  'Gets text after S11BridgeR0= to semicolon or end
            if OSLBandS11BridgeR0<=0 then OSLBandS11BridgeR0=50
        else
            'If using S21 fixture for reflection measurements, get attachment and R0
            OSLBandS21JigAttach$=uGetParamText$(sweep$, "S21Jig=", ";")  'Gets text after S21Jig= to semicolon or end
            if OSLBandS21JigAttach$="" then OSLBandS21JigAttach$="Series"
            OSLBandS21JigR0=val(uGetParamText$(sweep$, "S21JigR0=", ";"))  'Gets text after S21Jig= to semicolon or end
            if OSLBandS21JigR0<=0 then OSLBandS21JigR0=50
        end if
    else    'Base
        for i=1 to uWorkNumPoints   'ver116-4n
            OSLBaseRef(i-1,0)=uWorkArray(i, 0) 'freq
            OSLBaseA(i-1,0)=uWorkArray(i, 1) : OSLBaseA(i-1,1)=uWorkArray(i, 2) 'Coeff A real and imag
            OSLBaseB(i-1,0)=uWorkArray(i, 3) : OSLBaseB(i-1,1)=uWorkArray(i, 4) 'Coeff B real and imag
            OSLBaseC(i-1,0)=uWorkArray(i, 5) : OSLBaseC(i-1,1)=uWorkArray(i, 6) 'Coeff C real and imag
            OSLBaseRef(i-1,1)=uWorkArray(i, 7) : OSLBaseRef(i-1,2)=uWorkArray(i, 8) 'Mag and phase for ref
        next i
        OSLGetCalContextFromFile=uWorkNumPoints
            'Now derive the sweep parameters from the array and the title ver114-5h
        if uWorkNumPoints<1 then exit function
        OSLBaseNumSteps=uWorkNumPoints-1
        OSLBaseStartFreq=OSLBaseRef(0,0) : OSLBaseEndFreq=OSLBaseRef(OSLBaseNumSteps,0)
        OSLBaseTimeStamp$=uWorkTitle$(3)   'uArrayFromFile put date/time stamp here
        sweep$=uWorkTitle$(4)   'uArrayFromFile put sweep info here
        if instr(sweep$,"Linear")>0 then OSLBaseLinear=1 else OSLBaseLinear=0
                'ver115-1b added the retrieval of the following sweep info
            'Get remaining sweep info from sweep$. Each item has a keyword and ends with a semi-colon or end of line
        calPathNum$=uGetParamText$(sweep$, "Path ", ";")  'Gets text after "Path " to semicolon or end
        OSLBaseS11JigType$=uGetParamText$(sweep$, "S11Jig=", ";")  'Gets text after S11Jig= to semicolon or end ver115-1g
        if OSLBaseS11JigType$="" then OSLBaseS11JigType$="Reflect"  'ver115-1g
        if calPathNum$="" then OSLBasePath$="Path 1" else OSLBasePath$="Path ";calPathNum$

        if OSLBandS11JigType$="Reflect" then    'ver115-9d
            'If using bridge, get bridge R0
            OSLBaseS11BridgeR0=val(uGetParamText$(sweep$, "S11BridgeR0=", ";"))  'Gets text after S11BridgeR0= to semicolon or end
            if OSLBaseS11BridgeR0<=0 then OSLBaseS11BridgeR0=50
        else
            'If using S21 fixture for reflection measurements, get attachment and R0
            OSLBaseS21JigAttach$=uGetParamText$(sweep$, "S21Jig=", ";")  'Gets text after S21Jig= to semicolon or end
            if OSLBaseS21JigAttach$="" then OSLBaseS21JigAttach$="Series"
            OSLBaseS21JigR0=val(uGetParamText$(sweep$, "S21JigR0=", ";"))  'Gets text after S21Jig= to semicolon or end
            if OSLBaseS21JigR0<=0 then OSLBaseS21JigR0=50
        end if
    end if
end function
*/
}
