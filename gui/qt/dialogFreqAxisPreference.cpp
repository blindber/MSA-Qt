#include "dialogFreqAxisPreference.h"
#include "ui_dialogFreqAxisPreference.h"

dialogFreqAxisPreference::dialogFreqAxisPreference(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogFreqAxisPreference)
{
  ui->setupUi(this);
}

dialogFreqAxisPreference::~dialogFreqAxisPreference()
{
  delete ui;
}
void dialogFreqAxisPreference::menuFreqAxisPreference()
{/*
'ver114-5o added [menuFreqAxisPreference] as wrapper to be invoked by menu;
'ends in wait instead of return
[menuFreqAxisPreference]
    gosub [FreqAxisPreference]
    wait
*/
}

void dialogFreqAxisPreference::FreqAxisPreference()
{/*
[FreqAxisPreference]    'called from menu
'Call routine to display frequency axis preference window. We do this in two stages because
'the subroutine cannot access steps directly (it is not global, for speed reasons).
'ver114-5o modified to require restart only when absolutely necessary
'ver114-6e revised
    if haltsweep then gosub [FinishSweeping]    'ver115-8d
    needRestart=DisplayAxisXPreference()
    steps=globalSteps   'transfer to non-global
    'ver116-4j deleted call to selectFilter, leaving it to DetectChanges
    sweepDir=gGetSweepDir()  'transfer to non-global
    if (calCanUseAutoWait=0) and useAutoWait then   'ver116-4e
        useAutoWait=0 : wate=100
    end if
    if needRestart=1 then gosub [PartialRestart]
    continueCode=0  'signal to keep going ver115-8d
    if multiscanIsOpen then call multiscanSaveContexts 0 'zero means main graph  ver115-8d
    wait
*/
}

void dialogFreqAxisPreference::DisplayAxisXPreference()
{/*
function DisplayAxisXPreference()   'Display dialog to select axis preferences
    'Returns 1 if a restart is needed--only if frequency points or direction changed
    'Also sets continueCode by calling DetectChanges
    if axisPrefHandle$<>"" then close #axisPrefHandle$  'If pref window already open; close it

    WindowWidth = 800 : WindowHeight = 220
    call GetDialogPlacement 'set UpperLeftX and UpperLeftY ver115-1c
    UpperLeftX=1   'Window is very wide, so align to left side of graph window ver115-1c
    BackgroundColor$="buttonface"   'colors changed ver116-1b
    ForegroundColor$="black"
    TextboxColor$="white"
    ComboboxColor$="white"

                'Graph types
    statictext #axis.modeLab, "Data Mode", 15, 10, 90, 15
    specialGraphs$(0)="0(Normal Operation)"
    specialGraphs$(1)="1(Graph Mag Cal)"
    specialGraphs$(2)="2(Graph Freq Cal)"
    specialGraphs$(3)="3(Graph Noisy Sine)"
    specialGraphs$(4)="4(Graph 1MHz Peak)"
    if msaMode$="SA" then specialGraphs$(5)="5(1MHz Square Wave)" else specialGraphs$(5)="5(Simulate RLC/Coax)" 'ver115-4c
    Stylebits #axis.OpMode, _CBS_DROPDOWNLIST, 0, 0, 0   'ver115-1a
    combobox #axis.OpMode, specialGraphs$(), [axisSetupNOP], 5, 25, 130, 130    'Operating Mode ver115-1a
            'RBW Filter List
    statictext #axis.filter, "Select Final Filter Path:", 5, 55, 130, 12
    Stylebits #axis.FiltList, _CBS_DROPDOWNLIST, 0, 0, 0   'ver115-1a
    combobox #axis.FiltList, MSAFiltStrings$(), [axisSetupNOP],5,68,140,100 'Filter list ver115-1b
            'Video Filter
    statictext #axis.vidLab, "Video Filter BW", 15,100,80,15
    Stylebits #axis.VideoFilt, _CBS_DROPDOWNLIST, 0, 0, 0   'ver115-1a
    combobox #axis.VideoFilt, videoFilterNames$(), [axisSetupNOP], 5, 115, 100, 100    'Video Filter ver116-1b

            'Graph Appearance
    statictext #axis, "Graph Appearance",5,140,130,15
    'ver115-2a moved creating Appearances to top
    call FillAppearancesArray
    combobox #axis.Appearance, Appearances$(), [axisSetupSelectAppearance], 5, 155, 110,100 'Appearance List 'ver115-4c
            'Refresh; sweep time
    checkbox #axis.Refresh, "", [axisSetupNOP], [axisSetupNOP], 160, 10, 20, 15 'ver115-4c
    statictext #axis, "Refresh Screen Each Scan", 180, 10, 140, 15  'ver115-4c
    checkbox #axis.SweepTime, "", [axisSetupNOP], [axisSetupNOP], 160, 30, 20, 15  'ver115-4c
    statictext #axis, "Display Sweep Time",  180, 30, 140, 15  'ver115-4c

        'ver115-4g applied spurbox to all modes
    checkbox #axis.spurbox, "",[axisSetupNOP], [axisSetupNOP], 160, 50, 20, 15 'ver115-4g
    statictext #axis, "Spur Test", 180, 50, 75, 15 'ver115-4g

    if msaMode$="SA" then   'SA mode, maybe with TG
        if gentrk=1 then    'Tracking gen mode ver115-4f
            groupbox #axis, "Tracking Generator",160,70,145,70
            button #axis.normReverse, "Normal", [NormRevbutton], UL, 211, 90, 50, 20 'ver115-4f
            statictext #axis, "Offset", 167, 115, 30, 15 'ver115-4f
            textbox #axis.freqoffbox, 200, 113, 72, 20   'TG offset
            statictext #axis, "MHz", 275, 115, 24, 15
        else
            if TGtop>0 then 'Have TG hardware but not in TG mode; use if for signal gen
                'ver115-4f deleted SG/TG button
                statictext #axis, "Sig Gen Freq", 208, 92, 75, 15 'ver115-4f
                textbox #axis.freqoffbox, 200, 110, 72, 20   'Sig Gen freq
                statictext #axis, "MHz", 275, 112, 24, 15
            end if
        end if
    end if

    if msaMode$<>"SA" and msaMode$<>"ScalarTrans" then   'modes with phase   'ver115-4e
        statictext #axis.pdminvert, "PDM Inversion (deg)", 320,15,105,15
        textbox #axis.invdegbox, 335, 30, 50, 20
            'ver115-2b modified to use groupbox and button
        groupbox #axis.planeadj, "Plane Extension", 315, 60, 100, 70 'ver115-2b
        textbox #axis.planeadjbox, 335, 80, 50, 20  'create "plane extension"  box
        button #axis.PlaneRecalc,"Recalc",[RecalcPlaneExtAndR0], UL 335, 105, 50,20 'ver115-2d
        statictext #axis, "ns", 386, 81,20,20
    end if

            'Graph R0   added by ver115-2c moved ver116-1b
    if msaMode$="Reflection" then
        statictext #axis, "Graph R0", 240, 60, 65, 17
        textbox #axis.R0, 245, 76, 30, 20
        statictext #axis "ohms", 277, 78, 35, 20
    end if

            'DUT Direction
    if msaMode$<>"SA" then  'ver116-1b
        checkbox #axis.DirectionF, "DUT Forward", [setDUTForward], [setDUTReverse], 160, 105, 110, 12
        checkbox #axis.DirectionR, "DUT Reverse", [setDUTReverse], [setDUTForward], 160, 123, 110, 12
    end if

    sweepParamLeft=450 : sweepParamTop=20   'ver116-4k
        'Center/Span frequencies and Start/Stop frequencies, with radio buttons to select one pair
    groupbox #axis.ParamGroup, "", sweepParamLeft-2, sweepParamTop-23, 310, 56
    checkbox #axis.btnCentSpan, "", [setCentSpan], [setStartStop], sweepParamLeft+2,sweepParamTop, 14, 12
    statictext #axis.CentLab, "Cent", sweepParamLeft+18, sweepParamTop-12, 27,15
    statictext #axis.SpanLab, "Span", sweepParamLeft+18, sweepParamTop+11, 27,15
    statictext #axis.MHzLabA, "MHz", sweepParamLeft+125, sweepParamTop-12, 25,15
    statictext #axis.MHzLabB, "MHz", sweepParamLeft+125, sweepParamTop+12, 25,15
    textbox #axis.SweepCent, sweepParamLeft+46, sweepParamTop-12, 75,20
    textbox #axis.SweepSpan, sweepParamLeft+46, sweepParamTop+9, 75,20
    checkbox #axis.btnStartStop, "", [setStartStop], [setCentSpan], sweepParamLeft+160,sweepParamTop, 14, 12
    statictext #axis.StartLab, "Start", sweepParamLeft+175, sweepParamTop-12, 25,15
    statictext #axis.StopLab, "Stop", sweepParamLeft+175, sweepParamTop+11, 25,15
    statictext #axis.MHzLabC, "MHz", sweepParamLeft+278, sweepParamTop-12, 25,15
    statictext #axis.MHzLabD, "MHz", sweepParamLeft+278, sweepParamTop+12, 25,15
    textbox #axis.SweepStart, sweepParamLeft+201, sweepParamTop-12, 75,20
    textbox #axis.SweepStop, sweepParamLeft+201, sweepParamTop+8, 75,20
            'Steps per sweep
    statictext #axis.StepsLab, "Steps/Sweep", sweepParamLeft-10, sweepParamTop+40, 68,15
    textbox #axis.SweepSteps, sweepParamLeft, sweepParamTop+55, 40,20

        'Wait time
    statictext #axis.WaitLab, "Wait (ms)", sweepParamLeft-3, sweepParamTop+80, 45,13
    textbox #axis.SweepWait, sweepParamLeft, sweepParamTop+93, 40,20    'manual text entry
    waitPrecisions$(1)="Fast" : waitPrecisions$(2)="Normal" : waitPrecisions$(3)="Precise"  'ver116-1b
    Stylebits #axis.waitPrecision, _CBS_DROPDOWNLIST, 0, 0, 0    'Prevent direct user typing  'ver116-1b
    combobox #axis.waitPrecision, waitPrecisions$(), [axisXDoNothing],sweepParamLeft, sweepParamTop+89, 70,90   'list of precisions  'ver116-1b
    checkbox #axis.cbAutoWait, "", [setAutoWait], [clearAutoWait], sweepParamLeft+85,sweepParamTop+93, 14, 12  'ver116-4q
    statictext #axis.cbAutoWaitLab, "Auto Wait", sweepParamLeft+100, sweepParamTop+93, 60,15  'ver116-4q

        'Frequency Band added by ver115-1c
    statictext #axis, "Frequency Band", sweepParamLeft+15, sweepParamTop+126, 120,15    'ver114-3a
    IF1=int(appxLO2-10.7)   'Approx. IF1 frequency; assumes 10.7 MHz final filter
    FreqModes$(0)="Auto"    'ver116-4s
    FreqModes$(1)="1G; approx. 0-";IF1;" MHz"   'ver116-4s
    FreqModes$(2)="2G; approx. ";IF1-100; "-";2*IF1+100;" MHz"   'ver116-4s
    FreqModes$(3)="3G; approx. ";2*IF1+2; "-";3*IF1;" MHz"   'ver116-4s
    Stylebits #axis.FreqMode, _CBS_DROPDOWNLIST, 0, 0, 0    'Prevent direct user typing
    combobox #axis.FreqMode, FreqModes$(), [axisXDoNothing],sweepParamLeft-3, sweepParamTop+141, 160, 90

            'Number of horizontal divisions
    staticText #axis.DivLab1 "Hor. Div.", sweepParamLeft+80,sweepParamTop+40,60,15
    NumHorDiv$(0)="4" : NumHorDiv$(1)="6" : NumHorDiv$(2)="8"   'ver115-1b changed to NumHorDiv$
    NumHorDiv$(3)="10" : NumHorDiv$(4)="12"
    combobox #axis.NDiv, NumHorDiv$(), [axisXDoNothing],sweepParamLeft+80, sweepParamTop+54, 50, 120

            'Base Frequency (added by ver116-4k)
    staticText #axis, "Base Freq(MHz)",sweepParamLeft+190, sweepParamTop+40, 90, 15
    textbox #axis.baseFreq, sweepParamLeft+190, sweepParamTop+54, 75,20

        'Groupbox with "radio" buttons for linear/log
    groupbox #axis.group, "Sweep", sweepParamLeft+160, sweepParamTop+80, 165, 60
       'ver115-1b separated the captions from the checkboxes so the text color comes out right
    checkbox #axis.linear, "", [axisXSelLinear],[axisXSelLog],sweepParamLeft+215, sweepParamTop +95, 15, 15 'ver115-1c
    staticText #axis, "Linear",sweepParamLeft+230, sweepParamTop +95, 40, 15
    checkbox #axis.log, "", [axisXSelLog],[axisXSelLinear],sweepParamLeft+275, sweepParamTop +95, 15, 15 'ver115-1c
    staticText #axis, "Log", sweepParamLeft+290, sweepParamTop +95, 30, 15

                'ver114-4k added reverse
    checkbox #axis.LR, "", [axisLRon],[axisLRoff],sweepParamLeft+165, sweepParamTop +120, 15, 15 'ver115-1c
    staticText #axis, "L-R",sweepParamLeft+180, sweepParamTop +120, 20, 15
    checkbox #axis.RL, "", [axisRLon],[axisRLoff],sweepParamLeft+210, sweepParamTop +120, 15, 15 'ver115-1c
    staticText #axis, "R-L", sweepParamLeft+225, sweepParamTop +120, 20, 15
    checkbox #axis.Alternate, "", [axisALTon],[axisALToff],sweepParamLeft+255, sweepParamTop +120, 15, 15 'ver115-1c
    staticText #axis, "Alternate", sweepParamLeft+270, sweepParamTop +120, 50, 15

        'OK and Cancel buttons
    button #axis.OK, "OK", [axisXFinished],UL, 150, 155,90,30
    button #axis.Cancel, "Cancel", [axisXCancel],UL, 300, 155,90,30

            'Open preference dialog
    open "Sweep Parameters" for dialog_modal as #axis
    axisPrefHandle$="#axis"
    #axis, "trapclose [axisXFinished]"
    #axis, "font ms_sans_serif 9" 'ver116-1b
    #axis.OK, "!font Arial 10 bold"
    #axis.Cancel, "!font Arial 10 bold"

    #axis.waitPrecision "select ";autoWaitPrecision$   'ver116-1b
    if calCanUseAutoWait=0 then useAutoWait=0   'ver116-4e
    if useAutoWait then   'ver116-1b
        #axis.WaitLab, "!hide"
        #axis.SweepWait, "!hide"
        #axis.waitPrecision, "show"
        #axis.cbAutoWait, "set"
    else
        #axis.WaitLab, "!show"
        #axis.SweepWait, "!show"
        #axis.waitPrecision, "hide"
        #axis.cbAutoWait, "reset"
        if calCanUseAutoWait=0 then #axis.cbAutoWait, "hide"    'Hide checkbox if auto wait disabled.
    end if

    'Fill in preference choices with current values
    if alternateSweep then 'ver114-5a added alternateSweep
        #axis.Alternate, "set"
    else
        if gGetSweepDir()=1 then #axis.LR, "set" else #axis.RL, "set"
    end if
    linearF=gGetXIsLinear()   'Get freq linearity
    if linearF then     'modver115-1d
        #axis.linear, "set"
    else
        #axis.log, "set"
        #axis.DivLab1, "!hide"  'Can't choose number of divisions for log sweep
        #axis.NDiv, "hide"
    end if

    xForm$="#####.######"   'ver116-4k
    print #axis.SweepCent, uFormatted$(centfreq, xForm$)
    print #axis.SweepSpan, uFormatted$(sweepwidth, xForm$)
    print #axis.SweepStart, uFormatted$(startfreq, xForm$)
    print #axis.SweepStop, uFormatted$(endfreq, xForm$)
    print #axis.baseFreq, uFormatted$(baseFrequency, xForm$)    'ver116-4k
    print #axis.SweepSteps, globalSteps

    #axis.FreqMode, "selectindex ";freqBand+1  'freqBand is 0(auto),1, 2 or 3 ver116-4s
    #axis.FreqMode, "setfocus"  'ver115-1c
    print #axis.SweepWait, "";wate     'Wait time ver114-4d

    call gGetNumDivisions nHorDiv, nVertDiv
    #axis.NDiv, "!";nHorDiv
    #axis.NDiv, "setfocus" 'So selection is visually active
    #axis.SweepCent, "!setfocus"

    filtIndex=val(Word$(path$, 2))-1   'Index is one less than filter number, moved ver113-7a
    #axis.FiltList, "select ";MSAFiltStrings$(filtIndex)    'SEW2 Select filter for path$
    #axis.FiltList, "setfocus"

    #axis.OpMode, "selectindex ";doSpecialGraph+1
    #axis.OpMode, "setfocus"

    #axis.VideoFilt, "select "; videoFilter$  'ver114-5p
    #axis.VideoFilt, "setfocus"  'ver114-5p

    'We want to select the last preset that was used. If it is in the form CustomN, we want to
    'translate it to the name the user gave it when it was defined. ver115-2a
    lastColorPreset$=gGetLastPresetColors$()
    if Upper$(Left$(lastColorPreset$, 6))="CUSTOM" then
        customPresetNum=val(Mid$(lastColorPreset$,7))
        lastColorPreset$=customPresetNames$(customPresetNum)
    end if
    #axis.Appearance, "select ";lastColorPreset$  'ver115-2a

    #axis.Appearance, "setfocus"  'Makes sure selection is visually activated
    if refreshEachScan then #axis.Refresh, "set" else #axis.Refresh, "reset"
    if displaySweepTime then #axis.SweepTime, "set" else #axis.SweepTime, "reset"   'ver114-4f

    changeAppearance=0
    call RememberState  'Remember various variables so we can see if they change

    if spurcheck=1 then #axis.spurbox, "set" else #axis.spurbox, "reset"  'ver115-4g

    if msaMode$="SA" then 'ver115-4f
        if TGtop = 2 then 'ver114-4j print proper label; button only exists for SA mode with TGtop=2
            if normrev = 1 then print #axis.normReverse, "Reverse"
        end if
        if TGtop>0 then 'Do if we have the TG--print either SG freq or TG offset
            if gentrk=0 then print #axis.freqoffbox, "";sgout else print #axis.freqoffbox, "";offset 'ver115-4f
        end if
    else
        if switchFR=0 then  'ver116-1b
            #axis.DirectionF, "set" : #axis.DirectionR, "reset"
        else
            #axis.DirectionF, "reset" : #axis.DirectionR, "set"
        end if
        if msaMode$<>"ScalarTrans" then 'modes with phase ver115-4f
            #axis.invdegbox, invdeg
            #axis.planeadjbox, planeadj
        end if
    end if
                'R0 for Reflection
    if msaMode$="Reflection" then #axis.R0 uFormatted$(S11GraphR0,"3,4,5//UseMultiplier//DoCompact//SuppressMilli")   'ver116-4k

    if userFreqPref=0 then 'This is based on last user setting  'ver115-1d
        #axis.btnCentSpan, "set"   'Start in center/span mode
        gosub [enableCentSpan]
    else
        #axis.btnStartStop, "set"   'Start in start/stop mode
        gosub [enableStartStop]
    end if

    wait    'Wait for user to make choices
*/
}
void dialogFreqAxisPreference::setDUTForward()
{/*
[setDUTReverse]  'ver116-1b
    #axis.DirectionF, "reset" : #axis.DirectionR, "set"
    wait
*/
}
void dialogFreqAxisPreference::setAutoWait()
{/*
[setAutoWait]   'ver116-1b
    #axis.WaitLab, "!hide"
    #axis.SweepWait, "!hide"
    #axis.waitPrecision, "show"
    wait
    */
}
void dialogFreqAxisPreference::clearAutoWait()
{/*
[clearAutoWait]   'ver116-1b
    #axis.WaitLab, "!show"
    #axis.SweepWait, "!show"
    #axis.waitPrecision, "hide"
    wait
*/
}
void dialogFreqAxisPreference::axisLRon()
{/*    #axis.RL, "reset"
    #axis.Alternate, "reset"
    wait
*/
}
void dialogFreqAxisPreference::axisLRoff()
{/*    #axis.LR, "set"
    wait
*/
}
void dialogFreqAxisPreference::axisRLon()
{/*    #axis.LR, "reset"
    #axis.Alternate, "reset"
    wait
*/
}
void dialogFreqAxisPreference::axisRLoff()
{/*    #axis.RL, "set"
    wait
*/
}
void dialogFreqAxisPreference::axisALTon()
{/*    #axis.RL, "reset"
    #axis.LR, "reset"
    wait
*/
}
void dialogFreqAxisPreference::axisALToff()
{/*    #axis.Alternate, "set"
    wait
*/
}
void dialogFreqAxisPreference::axisXSelLinear()
{/*    #axis.DivLab1, "!show"
    #axis.NDiv, "show"
    #axis.NDiv, "select 10"     'Start with 10 divisions when switching from log to linear ver114-2c
    #axis.log, "reset"
    #axis.linear, "set" 'ver115-1c
    wait
*/
}
void dialogFreqAxisPreference::axisXSelLog()
{/*    #axis.DivLab1, "!hide"  'Can't choose number of divisions for log sweep ver114-5p
    #axis.NDiv, "hide"
    #axis.linear, "reset"
    #axis.log, "set"    'ver115-1c
    #axis.btnStartStop, "value? ssVal$"
    if ssVal$="reset" then #axis.btnStartStop, "set" : goto void dialogFreqAxisPreference::setStartStop()  'Log should be in start/stop mode
    wait
*/
}
void dialogFreqAxisPreference::setCentSpan()
{/*'Select Center/Span mode; use Start/Stop to fill in Center/Span
'embedded in DisplayAxisXPreference
    #axis.btnStartStop, "reset"
    #axis.btnCentSpan, "set"    'ver115-1d
    #axis.SweepStart, "!contents? currStart$"
    #axis.SweepStop, "!contents? currStop$"
    currStart=val(uCompact$(currStart$)) : currStop=val(uCompact$(currStop$))
    print #axis.SweepCent, using("#####.######",(currStart+currStop)/2)    'Enter center
    print #axis.SweepSpan, using("#####.######",currStop-currStart)    'Enter span
    gosub void dialogFreqAxisPreference::enableCentSpan()
    wait
*/
}
void dialogFreqAxisPreference::enableCentSpan()
{/*    #axis.SweepCent, "!enable" : #axis.SweepSpan, "!enable"
    #axis.SweepStart, "!disable" : #axis.SweepStop, "!disable"
    #axis.SweepCent, "!setfocus" : call uHighlightText "#axis.SweepCent" 'ver115-1d
    return
*/
}
void dialogFreqAxisPreference::setStartStop()
{/*'Select Start/Stop mode; use Center/Span to fill in Start/Stop
'embedded in DisplayAxisXPreference
    #axis.btnCentSpan, "reset"
    #axis.btnStartStop, "set"    'ver115-1d
    #axis.SweepCent, "!contents? currCent$"
    #axis.SweepSpan, "!contents? currSpan$"
    currCent=val(uCompact$(currCent$)) : currSpan=val(uCompact$(currSpan$))
    print #axis.SweepStart, using("#####.######",currCent-currSpan/2)    'Enter start
    print #axis.SweepStop, using("#####.######",currCent+currSpan/2)    'Enter stop
    gosub void dialogFreqAxisPreference::enableStartStop()
    wait
*/
}
void dialogFreqAxisPreference::enableStartStop()
{/*    #axis.SweepCent, "!disable" : #axis.SweepSpan, "!disable"
    #axis.SweepStart, "!enable" : #axis.SweepStop, "!enable"
    #axis.SweepStart, "!setfocus" : call uHighlightText "#axis.SweepStart" 'ver115-1d
    return
*/
}
void dialogFreqAxisPreference::axisXCancel()
{/*'User has cancelled; 'embedded in DisplayAxisXPreference
    close #axis
    axisPrefHandle$=""
    DisplayAxisXPreference=0    'signals no restart needed
    exit function
*/
}
void dialogFreqAxisPreference::axisSetupNOP()
{/*    wait
*/
}
//ver115-2b added void dialogFreqAxisPreference::RecalcPlaneExt() modver115-2d
void dialogFreqAxisPreference::RecalcPlaneExtAndR0()
{/*' button to Recalculate and draw data with new plane extension or new S11 graph R0
    'R0 transform is done only for reflection mode
    #axis.planeadjbox, "!contents? planeadj$"
    planeadj=val(uCompact$(planeadj$))  'Get new planeadj
    if msaMode$="Reflection" then
        #axis.R0, "!contents? R0$"
        S11GraphR0=uValWithMult(uCompact$(R0$))  'Get R0 ver116-4k
    end if
    gosub void dialogFreqAxisPreference::VerifyPlaneExtension()    'See if we can do plane ext. ver115-4j
    #axis.planeadjbox, planeadj     'In case it got zeroed  'ver115-4j
    call RecalcPlaneExtendAndR0AndRedraw 'Do actual recalc and redraw
    prevPlaneAdj=planeadj   'because we are making the adjustment here; DetectChanges doesn't have to do anything
    prevS11GraphR0=S11GraphR0
    wait
*/
}
void dialogFreqAxisPreference::VerifyPlaneExtension()
{/*'See if we can do plane extension ver115-4j
    if planeadj<>0 and msaMode$="Reflection" then
        if S11JigType$<>"Reflect" then      'Reflect means bridge
            if S21JigAttach$="Shunt" then
                notice "Can't do plane extension for shunt fixture." : planeadj=0   'we could but we don't
            else
                notice "Can't do plane extension for series fixture in Reflection Mode." : planeadj=0
            end if
        end if
        'Note: in transmission mode, plane extension with the shunt fixture requires fancier calculations
        'than we do, but we can't be sure what fixture is being used so we can't warn.
    end if
return
*/
}

void dialogFreqAxisPreference::axisSetupSelectAppearance()
{/*
[axisSetupSelectAppearance]  'Select graph appearance. Invoked by user click on combo box
    'Note that unless the user changes the appearance, we do not activate the selection upon
    'exit. This is because it will override previously selected trace colors.
    changeAppearance=1
    wait
*/
}

void dialogFreqAxisPreference::NormRevbutton()
{/*
[NormRevbutton]'when in Tracking Mode, selects either Normal or Reverse tracking 'ver111-17
    'this button does not exist in old TG topology. Only in new TG topology.
    if normrev = 0 then print #axis.normReverse, "Reverse":normrev = 1:wait
    if normrev = 1 then print #axis.normReverse, "Normal":normrev = 0:wait
*/
}

void dialogFreqAxisPreference::axisXFinished()
{/*
[axisXFinished]  'Window is being closed; record choices; 'embedded in DisplayAxisXPreference
    'ver115-2b moved planeadj here so any necessary redraw can be done prior to anything else changing.
    'If planeadj and other items are changed, there may be a superfluous redraw here.
    #axis.SweepSteps, "!contents? steps$"  'Get user specified number of steps
    globalSteps=val(steps$)
    if globalSteps<>prevSteps then
            'SEWgraph2 Note we set globalSteps here; steps is set from globalSteps
            'after we exit back to [FreqAxisPreference]
        if globalSteps<1 then globalSteps=1 else if globalSteps>maxNumSteps then globalSteps=maxNumSteps    'ver115-1b
        if globalSteps>2000 then    'ver116-4k
            confirm "Processing with more than 2,000 steps will be slow. Proceed anyway?"; stepsAns$
            if stepsAns$="no" then #axis.SweepSteps, 2000 : wait
        end if
        'Note we are called by [FreqAxisPreference], which will transfer globalSteps to steps.
        'Resize arrays if necessary for all steps and a few to spare
        if globalSteps>=gMaxNumPoints() then call ResizeArrays globalSteps+10      'ver114-2a
        call gSetNumDynamicSteps globalSteps    'Tell graph module ver114-1f
    end if

    R0Changed=0
    if msaMode$="Reflection" then   'ver115-2d
        #axis.R0, "!contents? newR0$"
        newR0=uValWithMult(newR0$)  'ver116-4k
        if newR0<0 then notice "Invalid Graph R0. 50 ohms used." : newR0=50 :#axis.R0, "50"
        if S11GraphR0<>newR0 then R0Changed=1   'for below
        S11GraphR0=newR0
    end if

        'ver115-4j moved retrieval of doSpecialGraph before doing plane adj.
    #axis.OpMode, "selectionindex? doSpecialGraph"
    if doSpecialGraph<1 then doSpecialGraph=0 else doSpecialGraph=doSpecialGraph-1 'e.g. selection 1 is doSpecialGraph=0
    if doSpecialGraph>0 and doSpecialGraph<>5 and msaMode$="VectorTrans" then S21JigAttach$="Series" 'ver115-4k ver115-5a

    if msaMode$="VectorTrans" or msaMode$="Reflection" then
        print #axis.planeadjbox, "!contents? planeadj$"
        planeadj=val(uCompact$(planeadj$))
        gosub [VerifyPlaneExtension]    'See if we can do plane ext. ver115-4j
        if planeadj<>prevPlaneAdj or R0Changed then     'prevPlaneAdj was saved by RememberState
            call RecalcPlaneExtendAndR0AndRedraw 'Redo planeadj adjustment from intermediate data and redraw graph
            prevPlaneAdj=planeadj   'because we are making the adjustment here; DetectChanges doesn't have to do anything
            prevS11GraphR0=S11GraphR0
        end if
    end if

        'DUT Direction
    if msaMode$<>"SA" then  'ver116-1b
        #axis.DirectionF, "value? DUTDirect$"
        if DUTDirect$="set" then switchFR=0 else switchFR=1
    end if

    #axis.Alternate, "value? s$"    'ver114-5a
    if s$="set" then alternateSweep=1 else alternateSweep=0 'ver114-5a
    'ver114-5a added this if... block
    if alternateSweep then
        dir=1   'if alternating, start with forward
    else
        #axis.LR, "value? s$"
        if s$="set" then dir=1 else dir=-1  'set forward or reverse
    end if
    call gSetSweepDir dir  'save in graph module; caller must transfer it to sweepDir 'ver114-4n

    #axis.FreqMode, "selectionindex? freqBand"   'Get user specified frequency Mode ver115-1c
    freqBand=freqBand-1 'first value is zero but first index is one. ver116-4s
    if freqBand<0 then freqBand=0   'ver115-1c ver116-4s
    if freqBand>3 then freqBand=3   'ver115-1c

        'Get base freq. Note that graph module knows nothing about this; it has an effect only when
        'commanding hardware and applying frequency calibration.
    #axis.baseFreq, "!contents? baseFrequency$"
    baseFrequency=int(val(uCompact$(baseFrequency$))+0.5)   'round to Hz; Not perfect for negative, but OK
    if baseFrequency<0 then notice "Base Frequency cannot be negative; zero Hz used." : baseFrequency=0
        'Get frequency range
    #axis.btnCentSpan, "value? CentSpan$"
    if CentSpan$="set" then
        'Use Center/Span to determine centfreq, sweepwidth, startfreq and endfreq
        userFreqPref=0  'Save as user preference ver115-1d
        #axis.SweepCent, "!contents? currCent$"
        #axis.SweepSpan, "!contents? currSpan$"
            'uCompact deletes spaces, which can mess up negative numbers
        temp1= val(uCompact$(currCent$)): temp2=val(uCompact$(currSpan$)) 'ver115-1b
            'Enter new freq, but only if there is a material change. Otherwise,
            'tiny differences between using center/span and start/stop will trigger a restart.  'ver115-1b
        if abs(temp1-centfreq)>1e-12 or abs(temp2-sweepwidth)>1e-12 then _
                    call SetCenterSpanFreq temp1, temp2 'new center and span
    else
        'Use Start/Stop to determine centfreq, sweepwidth, startfreq and endfreq
        userFreqPref=1  'Save as user preference ver115-1d
        #axis.SweepStart, "!contents? currStart$"
        #axis.SweepStop, "!contents? currStop$"
            'uCompact deletes spaces, which can mess up negative numbers
        temp1= val(uCompact$(currStart$)): temp2=val(uCompact$(currStop$))  'ver115-1b
            'Enter new freq, but only if there is a material change.  'ver115-1b
        if abs(temp1-startfreq)>1e-12 or abs(temp2-endfreq)>1e-12 then _
                    call SetStartStopFreq temp1, temp2 'new center and span
    end if

        'We retrieve both the manual wait time and auto wait precision,
        'even though user only wants to use one at a time.  'ver116-1b
    #axis.SweepWait, "!contents? wate$";
    wate = val(wate$)
    if wate<0 then wate=0
    #axis.waitPrecision, "selection? autoWaitPrecision$"
    #axis.cbAutoWait, "value? waitType$"
    if waitType$="set" then useAutoWait=1 else useAutoWait=0

        'Set sweep to log or linear. We do this after first setting the new range, since log sweep
        'gets automatically changed to linear if the span is too small.
    #axis.linear, "value? lin$"
    if lin$="set" then linearF=1 else linearF=0
    if linearF=0 then
        'ver116-4k:for log sweeps, X range can be negative and can include zero, but cannot cross zero.
        'If it includes zero, that value will be changed to a small value based on the "blackHoleRadius".
        if (startfreq<0) and (endfreq>0) then _ 'ver116-4k
                    notice "Log sweep cannot cross zero Hz. Range changed." : _
                        endfreq=max(endfreq, 1) : call SetStartStopFreq endfreq/100000, endfreq
        includesZero=(startfreq<=0) and (endfreq>=0) : allNegative=(startfreq<0) and (endfreq<0)    'ver116-4k
        if includesZero then 'includes but does not cross zero
            if startfreq=0 then
                if endfreq=0 then   'both limits are zero
                    linearF=1 : notice "Changed to linear sweep because span is small."
                    #axis.NDiv, "select 10" 'set to 10 divisions ver116-1b
                else 'starts at zero; ends above zero
                    notice "0 Hz not allowed in log sweep; range changed" : call SetStartStopFreq 0.0001, max(0.001, endfreq)
                end if
            else    'starts below 0; ends at 0
                notice "0 Hz not allowed in log sweep; range changed" : call SetStartStopFreq min(-0.001, startfreq), -0.0001
            end if
        else    'all positive or all negative
            if allNegative then span=uSafeLog10(startfreq/endfreq) else span=uSafeLog10(endfreq/startfreq)    'ver116-4k
            if span <0.7 then
                linearF=1 : notice "Changed to linear sweep because span is small."  'ver114-6e
                #axis.NDiv, "select 10" 'set to 10 divisions ver116-1b
            end if
            if span>9 then
                if allNegative then call SetStartStopFreq startfreq, startfreq/1000000000 else call SetStartStopFreq endfreq/1000000000, endfreq 'ver116-4k
                notice "Log span cannot exceed 9 decades; sweep limits changed."  'ver114-6e
            end if
        end if
    end if
    call gSetXIsLinear linearF   'Set freq linearity

    #axis.NDiv, "contents? nDiv$"   'Get user specified number of divisions
    nHorDiv=val(nDiv$) : if nHorDiv<2 then nHorDiv=2 else if nHorDiv>12 then nHorDiv=10 'ver116-1b
    nHorDiv=2*int(nHorDiv/2)    'Make the number even
        'Set new number of hor divisions, with old number of vert div
    call gSetNumDivisions nHorDiv,nVertDiv

    #axis.Refresh, "value? doRefresh$"
    if doRefresh$="set" then refreshEachScan=1 else refreshEachScan=0

    #axis.SweepTime, "value? doSweepTime$"  'ver114-4f
    if doSweepTime$="set" then displaySweepTime=1 else displaySweepTime=0

    if changeAppearance then
        #axis.Appearance, "contents? graphAppearance$"
        if graphAppearance$<>"DARK" and graphAppearance$<>"LIGHT" then 'ver115-2a
            'We are using a custom color set, and must create a name in the form "CustomN"
            customName$=""
            for i=1 to 5
                thisPreset$=customPresetNames$(i)
                if thisPreset$=graphAppearance$ then customName$="Custom";i : exit for
            next i
            if customName$="" then graphAppearance$="DARK" else graphAppearance$=customName$
        end if
        call gUsePresetColors graphAppearance$  'Set color scheme
        call SetCycleColors 'ver116-4s
        #axis.OK, "!setfocus"   'Take focus off combo box to protect from scroll wheel
    end if
    #axis.FiltList, "selectionindex? filtIndex"   'Filter N is at index N in list
    if filtIndex=0 then 'filtIndex can be 0 if user typed something in the combo box
        filtIndex=1
        #axis.FiltList, "select ";MSAFiltStrings$(0)   'Select default
        #axis.FiltList, "setfocus"  'SEW2  Needed to activate the highlight
    end if
    path$="Path "+str$(filtIndex) 'Name of filter path; Path 1, ver113-7c
    'RBW filter will actually be selected after we return and load the RBW path cal data

    #axis.VideoFilt, "contents? videoFilter$" 'get selected video filter  ver114-5p
        'Note DetectChanges will take care of actually selecting the video filter

    #axis.spurbox, "value? doSpur$"  'ver115-4g
    if doSpur$="set" then spurcheck=1 else spurcheck=0 'ver115-4g

    if msaMode$="SA" then 'ver115-4g
        if TGtop > 0 then print #axis.freqoffbox, "!contents? freqoff$"   'Sig Gen Freq or TG offset box data ver116-4q
        if gentrk = 0 then
            sgout = val(using("####.######",val(uCompact$(freqoff$)))) 'ver115-4g
'delete 'ver116-4r        if (sgout<=0) or (sgout>3*LO2) then sgout=10: Notice "Sig Gen Frequency was out-of-range. Changed to 10 MHz"    'ver116-4p
            if sgout<-75 then Notice "LO3 for Sig Gen Frequency may be too low"    'ver116-4r
            if sgout>3*LO2 then Notice "LO3 for Sig Gen Frequency may be too high"    'ver116-4r
        end if
        if gentrk = 1 then offset = val(using("####.######",val(uCompact$(freqoff$)))) 'ver115-4g
    end if
    if msaMode$="VectorTrans" or msaMode$="Reflection" then 'modes with phase ver115-4g
        print #axis.invdegbox, "!contents? invdeg$";
        invdeg = val(uCompact$(invdeg$))
        'ver115-2b moved planeadj further above
    end if

    if doSpecialGraph=5 and msaMode$<>"SA" then 'ver116-4k moved this to be last thing done
        'Simultated RLC/Transmission line data--give user chance to change
        parseErr=uParseRLC(doSpecialRLCSpec$, DialogRLCConnect$, DialogRValue, DialogLValue, _
                            DialogCValue, DialogQLValue, DialogQCValue, dumD, DialogCoaxSpecs$)
        if parseErr then
            doSpecialRLCSpec$="RLC[S,R0,L0,C";constMaxValue;",QL10000,QC10000]"    'ver115-4b
            DialogRLCConnect$="S" : DialogRValue=0 : DialogLValue=0
            DialogQLValue=10000 : DialogQCValue=10000
            DialogCValue=constMaxValue
            DialogCoaxSpecs$=""  'ver115-4a
        end if
        DialogCoaxName$=doSpecialCoaxName$  'ver115-4b
        close #axis : #handle, "disable"   'ver116-4k
        call RLCDialog  'Get desired circuit values
        #handle, "enable"   'ver116-4k
        'Assemble the values into a spec string
                'Get the new values; assemble them into spec
        doSpecialCoaxName$=DialogCoaxName$  'ver115-4b
        form$="3,3,4//UseMultiplier"
        resForm$="3,3,4//UseMultiplier//SuppressMilli" 'ver115-4e
        QForm$="######.###"   'ver115-5f
        R$=uFormatted$(DialogRValue, resForm$)
        L$=uFormatted$(DialogLValue, form$)
        C$=uFormatted$(DialogCValue, form$)
        QL$=uFormatted$(DialogQLValue, QForm$) 'ver115-4b
        QC$=uFormatted$(DialogQCValue, QForm$) 'ver115-4b
        doSpecialRLCSpec$="RLC[";DialogRLCConnect$;",R";uCompact$(R$);",L";uCompact$(L$); ",C";uCompact$(C$); _
                        ",QL";uCompact$(QL$);",QC";uCompact$(QC$); "], Coax[";DialogCoaxSpecs$;"]"
    else
        close #axis 'ver116-4k
    end if

    axisPrefHandle$=""
    call DetectChanges 0   'Do necessary redrawing and set continueCode ver114-6e
    if continueCode=3 then  'ver114-6e
        DisplayAxisXPreference=1
    else
        continueCode=0
        DisplayAxisXPreference=0
    end if
end function 'end of DisplayAxisXPreference

*/
}
