#include "dialogcomponentmeasure.h"
#include "ui_dialogcomponentmeasure.h"

dialogComponentMeasure::dialogComponentMeasure(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogComponentMeasure)
{
  ui->setupUi(this);
}

dialogComponentMeasure::~dialogComponentMeasure()
{
  delete ui;
}
void dialogComponentMeasure::menuComponentMeasure()
{/*
    if componentWindHndl$<>"" then close #component
    if (primaryAxis=1 and Y1DataType<>constMagDB) or (primaryAxis=2 and Y2DataType<>constMagDB) then _
                 notice "Primary axis must contain a graph of magnitude (dB)." : wait   'ver115-3b
    if haltsweep=1 then gosub void dialogComponentMeasure::FinishSweeping()
    WindowWidth = 550
    WindowHeight = 480
    call GetDialogPlacement
    BackgroundColor$="gray"
    ForegroundColor$="black"
    TextboxColor$ = "white"

    groupbox #component, "Component Type", 300, 125, 130, 90
    statictext #component, "To measure resistance, capacitance or inductance you must first calibrate.",  20,  17, 500,  20
    statictext #component " Then insert the component and click",  20,  37, 500,  20
    statictext #component, "Measure. The video filter should be set to NARROW. Other settings will be made", 20, 57, 500,  20
    statictext #component, "automatically. You can temporarily change the frequency with +Freq or -Freq.", 20, 77, 500,  20

        'Fixture info
        'Note for reflection mode the following fixture info items will exist but be hidden, and their
        'data is irrelevant
    checkbox #component.Series, "Series", void dialogComponentMeasure::ComponSeriesSet(), void dialogComponentMeasure::ComponSeriesClear(),  40, 147,  64,  25
    checkbox #component.Shunt, "Shunt", void dialogComponentMeasure::ComponShuntSet(), void dialogComponentMeasure::ComponShuntClear(),  40, 172, 58,  20
    textbox #component.R0, 130, 170, 50, 22
    if msaMode$<>"Reflection" then  'These labels not needed for reflection mode ver115-5b
        groupbox #component, "Test Fixture",  30, 125, 195, 90
        statictext #component, "R0", 135, 152, 25, 17
        statictext #component, "ohms", 180, 167, 33, 20
    end if

            'Component type
    checkbox #component.Resistor, "Resistor", ComponResistorSet, ComponResistorSet, 330, 147,  75,  20
    checkbox #component.Capacitor, "Capacitor", ComponCapacitorSet, ComponCapacitorSet, 330, 167,  83,  20
    checkbox #component.Inductor, "Inductor", ComponInductorSet, ComponInductorSet, 330, 187,  72,  20

    statictext #component.Freq, "Frequency= 0.1 MHz", 125, 275, 130,  20
    button #component.DecFreq, "-Freq", void dialogComponentMeasure::ComponDecFreq(), UL, 280, 275, 40,20
    button #component.IncFreq, "+Freq", void dialogComponentMeasure::ComponIncFreq(), UL, 321, 275, 40,20

    graphicbox #component.Value, 125, 300, 235, 40
    staticText #component.SeriesR, "Series R=",370, 300, 200, 20 'For series resistance of inductors ver115-3d
    staticText #component.Q, "Q=",370, 320, 200, 20 'For Q of inductors ver115-3d
    button #component.Calibrate,"Calibrate",void dialogComponentMeasure::ComponDoCalScan(), UL, 30, 367,  75,  35
    button #component.Measure,"Measure",void dialogComponentMeasure::ComponentAcquireAndMeasure(), UL, 150, 367,  100,  35
    button #component.Stop,"Stop",ComponentStop, UL, 150, 367,  100,  35  'Stop is in same space as Measure
    button #component.Done,"Done",void dialogComponentMeasure::ComponentDone(), UL, 295, 367,  70,  35
    button #component.Explain,"Help",void dialogComponentMeasure::ComponentExplain(), UL, 415, 367,  70,  35

    statictext #component.CalInst, "", 10, 405, 125, 20 'initially blank ver115-5b

    open "Component Meter" for dialog_modal as #component 'ver115-4c
    componentWindHndl$="#component"
    print #component, "font ms_sans_serif 10"
    print #component, "trapclose ComponentCloseBox"
    componBoxColor$="200 200 200"
    print #component.Value, "font tahoma 20 bold;down; color 0 0 255;backcolor ";componBoxColor$
    #component.Value, "cls; fill ";componBoxColor$
    #component.Stop, "!hide"
    #component.Resistor, "set"
    #component.SeriesR, "!hide" : #component.Q, "!hide"   'ver115-3d
    #component.R0, "50"
    call mClearMarkers  'Markers just cause visual confusion ver115-3b
    #component.Done, "!setfocus" 'to keep window in front
    componFirstCal=1    'True only until first cal is done
    if msaMode$="Reflection" then  'No fixture info for reflection mode ver115-5b
        #component.Series, "hide"
        #component.Shunt, "hide"
        #component.R0, "!hide"
    end if

    componLastSeriesValue$="reset"
    if msaMode$="Reflection" then  'ver115-5b
        gosub void dialogComponentMeasure::ComponentInvalidateCal()
    else
        goto void dialogComponentMeasure::ComponSeriesSet()  'will also invalidate cal
    end if
    wait
*/
  exec();
}
void dialogComponentMeasure::ComponentCloseBox(QString h)
{/*'Close box clicked. Don't do anything, because we can't quit if Measure is in progress.
    'just return and keep going
end sub
*/
}
void dialogComponentMeasure::ComponentInvalidateCal()
{/*'disable buttons so all user can do is calibrate
    #component.Measure, "!disable"
    #component.DecFreq, "!disable"
    #component.IncFreq, "!disable"
    return
*/
}
void dialogComponentMeasure::ComponIncFreq()
{/*'increase freq to next point
    if componPointNum<=steps then componPointNum=componPointNum+1
    gosub void dialogComponentMeasure::ComponentSetItemsForMeasuring()
    gosub void dialogComponentMeasure::ComponentMeasure()    'Measure at this point
    gosub void dialogComponentMeasure::ComponentSetItemsForMeasuringDone()
    wait
*/
}
void dialogComponentMeasure::ComponDecFreq()
{/*'decrease freq to previous point
    if componPointNum>1 then componPointNum=componPointNum-1
    gosub void dialogComponentMeasure::ComponentSetItemsForMeasuring()
    gosub void dialogComponentMeasure::ComponentMeasure()    'Measure at this point
    gosub void dialogComponentMeasure::ComponentSetItemsForMeasuringDone()
    wait
*/
}
void dialogComponentMeasure::ComponDoCalScan()
{/*'Do calibration scan
    #component.Series,"disable"
    #component.Shunt,"disable"
    #component.R0,"!disable"
    'Note we don't show Stop--we are only doing one sweep
    #component.Measure, "!disable"
    #component.DecFreq, "!disable"
    #component.IncFreq, "!disable"
    #component.Calibrate, "!disable"

        'Save old sweep params. Note cal will also save them, so we need our own variables
    if componFirstCal=1 then
        componFirstCal=0    'No longer the first cal
        'Set up the sweep params we want ver115-5c
        steps=8     'Note globalSteps will change in FunctChangeAndSaveSweepParams
        call FunctChangeAndSaveSweepParams  1, 0, steps, 0.1, 40, 0    'Log 100 kHz to 40 MHz; save old settings but don't transfer band cal to base cal
        'ver115-5c : FunctChangeAndSaveSweepParams now sets wate, etc.
            'Settings will be restored in ComponentDone
        call SetDefaultGraphData 'To get mag on primary axis ver115-3b

        'Note: steps are 100K,210K,450K,950K, 2M, 4.2M, 8.9M, 18.9M, 40M
    end if

    print #component.Value, "cls; fill ";componBoxColor$    'Clear last value
    if msaMode$="Reflection" then   'ver115-5b
        'Use regular cal dialog for reflection mode
        gosub void dialogComponentMeasure::PerformOSLCal()    'Sets OSLError=1 if math error occurs
    else
            'ver115-5a forced lineCalThroughDelay to zero just for the current calibration
        componSaveThroughDelay=lineCalThroughDelay
        lineCalThroughDelay=0
        gosub void dialogComponentMeasure::BandLineCal()
        lineCalThroughDelay=componSaveThroughDelay
    end if
    desiredCalLevel=2
    call RequireRestart
     'Note sweep settings are not restored until we are Done

    #component.Freq, "Frequency= 0.1 MHz"   'Start with 100 KHz just to have something there
    #component.Series,"enable"
    #component.Shunt,"enable"
    #component.R0,"!enable"
    #component.Stop, "!hide"
    if msaMode$<>"Reflection" or OSLError=0 then #component.Measure, "!enable"     'Enable Measure now that we have cal ver115-5b
    #component.Calibrate, "!enable"
    wait
*/
}
void dialogComponentMeasure::ComponSeriesSet()
{/*
    #component.Series, "set" : #component.Shunt, "reset"
    if componLastSeriesValue$<>"set" then gosub void dialogComponentMeasure::ComponentInvalidateCal() 'topology changed
    componLastSeriesValue$="set"
    #component.CalInst, "Calibrate with Short"
    wait
*/
}
void dialogComponentMeasure::ComponSeriesClear()
{/*
    #component.Series, "set"    'don't let it be cleared directly
    wait
*/
}
void dialogComponentMeasure::ComponShuntSet()
{/*
    #component.Series, "reset" : #component.Shunt, "set"
    if componLastVal$<>"set" then gosub void dialogComponentMeasure::ComponentInvalidateCal() 'topology changed
    componLastSeriesValue$="reset"
    #component.CalInst, "Calibrate with Open"
    wait
*/
}
void dialogComponentMeasure::ComponShuntClear()
{/*
    #component.Shunt, "set"     'don't let it be cleared directly
    wait
*/
}
void dialogComponentMeasure::ComponResistorSet(QString h)
{/*
    #component.Resistor, "set" : #component.Capacitor, "reset" : #component.Inductor, "reset"
    #component.SeriesR, "!hide" : #component.Q, "!hide"   'ver115-3d
end sub
*/
}
void dialogComponentMeasure::ComponCapacitorSet(QString h)
{/*
    #component.Resistor, "reset" : #component.Capacitor, "set" : #component.Inductor, "reset"
    #component.SeriesR, "!hide" : #component.Q, "!hide"   'ver115-3d
end sub
*/
}
void dialogComponentMeasure::ComponInductorSet(QString h)
{/*
    #component.Resistor, "reset" : #component.Capacitor, "reset" : #component.Inductor, "set"
    #component.SeriesR, "Series R=" : #component.Q, "Q="  'ver115-3d
    if msaMode$<>"ScalarTrans" then #component.SeriesR, "!show" : _
                                #component.Q, "!show"     'Show series res and Q for L if we have phase ver115-3d
end sub
*/
}
void dialogComponentMeasure::ComponentStop(QString h)
  {/*'This button shows while Measuring
    componStopAtEnd=1
end sub
*/
}
void dialogComponentMeasure::ComponentSetItemsForMeasuring()
{/*'Set items they way they should be during measuring
    #component.Done, "!disable"
    #component.Series,"disable"
    #component.Shunt,"disable"
    #component.R0,"!disable"
    #component.Stop, "!show"
    #component.Measure, "!hide"
    #component.DecFreq, "!disable"
    #component.IncFreq, "!disable"
    #component.Calibrate, "!disable"
    #component.Explain, "!disable" 'ver115-3d
    return
*/
}
void dialogComponentMeasure::ComponentSetItemsForMeasuringDone()
{/*'Set items the way they should be when measuring has stopped
    #component.DecFreq, "!enable"  'Enable frequency hopping now that we have data
    #component.IncFreq, "!enable"
    #component.Stop, "!hide"
    #component.Measure, "!show"
    #component.Done, "!enable"
    #component.Series,"enable"
    #component.Shunt,"enable"
    #component.R0,"!enable"
    #component.Calibrate, "!enable"
    #component.Explain, "!enable" 'ver115-3d
    return
*/
}
void dialogComponentMeasure::ComponentAcquireAndMeasure()
 {/*'button to do measurement scan
    gosub void dialogComponentMeasure::ComponentSetItemsForMeasuring()
    componStopAtEnd=0   'can be set by user during sweep
    while componStopAtEnd=0 'Loop until user clicks Stop
        specialOneSweep=1   'So we return from void dialogComponentMeasure::Restart()
        gosub void dialogComponentMeasure::Restart()     'Do actual scan to acquire data
        'Get values of all the boxes
        componPointNum=0    'Indicates to find best freq
        gosub void dialogComponentMeasure::ComponentMeasure()    'Do actual calculations and display result
    wend
    gosub void dialogComponentMeasure::ComponentSetItemsForMeasuringDone()
    wait
*/
}
void dialogComponentMeasure::ComponentMeasure()
{/*'Do actual calculations and display result
    #component.Resistor, "value? componRes$"
    #component.Capacitor, "value? componCap$"
    #component.Inductor, "value? componInd$"
    #component.Series, "value? componSeries$"
    if componSeries$="set" then componIsSeries=1 else componIsSeries=0
    #component.R0, "!contents? componR0$"   'R0
    componR0=val(componR0$)
    if componR0<0 then notice "Invalid R0. 50 ohms used."   'ver115-3f
    componType$=""
    if componRes$="set" then componType$="R"
    if componCap$="set" then componType$="C"
    if componInd$="set" then componType$="L"
    if componType$="R" then
        componUnits$="Ohms"
    else
        if componType$="C" then
            componUnits$="F"
        else    'Inductor
            componUnits$="H"
        end if
    end if
    call ComponentAnalyzeData componR0, componType$, componIsSeries, componValue, componSerRes, componPointNum 'ver115-3d
    componFreq=gGetPointXVal(componPointNum)
        'Normally print value in blue, but in red if outside range of best accuracy
    componTextColor$="blue" 'ver115-4e
    if componValue<0 then   'ver115-3e rewrote this block
        componValue$="------"     'Invalid measurement due to self-resonance
        componTextColor$="red"
        componNegativeValue=1
    else
        componNegativeValue=0
        if componType$="R" and componValue<0.001 then componValue=0  '0.001 ohms lower limit
        if componType$="L" and componValue<1e-10 then componValue=0  '0.1 nH lower limit
        if componType$="C" and componValue<5e-14 then componValue=0 '0.05 pF lower limit
        componValue$=uFormatted$(componValue, "3,3,4//UseMultiplier//SuppressMilli"); componUnits$  'ver115-4e
    end if

         'ver115-4e added color change for out-of-range
    componR0ratio=S21JigR0/50   'To adjust limits based on jig R0
    if componType$="R" then
        if S21JigAttach$="Series" then
            if componValue<2*componR0ratio or componValue>100000*componR0ratio then componTextColor$="red"
        else
            if componValue<0.1*componR0ratio or componValue>1000*componR0ratio then componTextColor$="red"
        end if
    end if
    if componType$="L" then
        if S21JigAttach$="Series" then
            if componValue<1e-8*componR0ratio or componValue>0.001*componR0ratio then componTextColor$="red"
        else
            if componValue<1e-7*componR0ratio or componValue>0.0001*componR0ratio then componTextColor$="red"
        end if
    end if
    if componType$="C" then
        if S21JigAttach$="Series" then
            if componValue<1e-12*componR0ratio or componValue>0.0000002*componR0ratio then componTextColor$="red"
        else
            if componValue<2e-11*componR0ratio or componValue>0.000002*componR0ratio then componTextColor$="red"
        end if
    end if

    #component.Freq, "Frequency=";using("###.#",componFreq); " MHz" 'display frequency at which we measured
    print #component.Value, "cls; fill ";componBoxColor$; ";color ";componTextColor$  'ver115-4d
    print #component.Value, "place 15 30;\";componValue$
    print #component.Value, "flush"
    if componType$="L" then    'display series resistance and Q for inductors ver115-3d
        if componNegativeValue then     'Negative L due to self-resonance ver115-3e
            #component.SeriesR, ""
            #component.Q, ""
        else
            componSerR$=uFormatted$(componSerRes, "3,3,4//UseMultiplier//SuppressMilli");"ohms"  'ver115-4e
            componSerX=2*uPi()*componValue*componFreq*1000000   'Inductor reactance
            if componSerRes=0 then 'ver115-5e
                componQ$="300+"
            else
                componQ=componSerX/componSerRes
                if componQ>300 then componQ$="300+" else componQ$=using("####.#",componQ)
            end if
            #component.SeriesR, "Series R=";componSerR$
            #component.Q, "Q=";componQ$
        end if
    end if
    return
*/
}
void dialogComponentMeasure::ComponentDone()
{/*
    'Note this cannot happen while Measure or Calibrate is in progress.
    close #component
    componentWindHndl$=""
        'We need to restore sweep settings if we did anything
    if componFirstCal=0 then 'if we did a calibration, then restore prior settings ver115-5c
        gosub void dialogComponentMeasure::FunctRestoreSweepParams()
        suppressHardwareInitOnRestart=1 'no need for hardware init ver116-4d
        gosub void dialogComponentMeasure::PartialRestart() 'implement changes
    end if
    wait
*/
}
void dialogComponentMeasure::ComponentExplain()
{/*
    WindowWidth = 540
    WindowHeight = 475
    UpperLeftX=int((DisplayWidth-WindowWidth)/2)+100
    UpperLeftY=int((DisplayHeight-WindowHeight)/2)+100
    BackgroundColor$="gray"
    ForegroundColor$="black"
    TextboxColor$ = "white"

    s$="Component Meter is a simple way to measure the value of components which are known"
    s$=s$;" to be relatively pure resistors, capacitors or inductors. It determines the component value"
    s$=s$;" from the attenuation caused by the component in the test fixture. You select the fixture and component"
    s$=s$;" type, run a single calibration, then insert and measure components."
    statictext #componHelp, s$,  20,  10, 500,  80

    s$="When you click Measure, the MSA will determine the component value at one of several possible frequencies"
    s$=s$;" and display the frequency of the measurement. The possible frequencies are those that the MSA automatically"
    s$=s$;" included in the calibration. You may increase/decrease the frequency of the measurement with the +Freq and -Freq"
    s$=s$;" buttons, after pushing Stop."
    statictext #componHelp, s$,  20,  100, 500,  80

    s$="The test fixture is typically an attenuator, then the component, then another attenuator. The component"
    s$=s$;" may be connected in Series between the attenuators, or may be Shunt to ground, which accounts for the"
    s$=s$;" two different fixture types. The component will see a certain resistance R0 looking at the incoming signal"
    s$=s$;" and the outgoing signal. You must specify that R0, usually 50 ohms."
    statictext #componHelp, s$,  20,  190, 500,  80

    s$="The Series fixture is calibrated with a Short (the terminals directly shorted) and can typically measure R from"
    s$=s$;" 5 ohms to 100K ohms; L from 10 nH to 1 mH, and C from 1 pF to 0.2 uF."
    statictext #componHelp, s$,  20,  280, 500,  60

    s$="The Shunt fixture is calibrated with an Open (no component attached) and can typically measure R from"
    s$=s$;" 0.25 ohms to 1 kohm; L from 100 nH to 100 uH, and C from 20 pF to 2 uF."
    statictext #componHelp, s$,  20,  340, 500, 70

    s$="For inductors, the series resistance and Q will be displayed, but if Q>30, both Q and series resistance may be unreliable."
    statictext #componHelp, s$,  20,  400, 500, 70

    open "Component Measurement Help" for dialog_modal as #componHelp
    print #componHelp, "font ms_sans_serif 10"
    print #componHelp, "trapclose void dialogComponentMeasure::ComponentHelpDone()"
    wait
*/
}
void dialogComponentMeasure::ComponentHelpDone()
{/*
    close #componHelp
    wait
*/
}
void dialogComponentMeasure::ComponentAnalyzeData()// componR0, componType$, componIsSeries, byref componValue, byref serRes, byref pointNum
 {/*'Return component value and frequency ver115-3d
    'Calculate component value at point specified by pointNum, but if it is zero find best frequency
    'Return the component value (ohms, F or H) and the point number at which we measured. For L and C,
    'we also return the series resistance, which is valid if we have phase.
    'It is possible to return a negative L or C value, which means the self-resonance has interfered and the measurement
    'is not valid.

     'Do an initial measurement
    doBestFreq=(pointNum<1 or pointNum>globalSteps+1)  'invalid point means find the best one
            'Note: steps are 100K,210K,450K,950K, 2M, 4.2M, 8.9M, 18.9M, 40M
    if doBestFreq then 'ver115-3e rewrote this block
        nTries=3    'Assume need to iterate to find best frequency
        if componType$="R" then
            pointNum=4 : nTries=0  '950K Choose a frequency high enough where LO leakages are not an issue
        else
            lowFreqDB=gGetPointYVal(1,primaryAxisNum) 'Get mag data for a low frequency
            highFreqDB=gGetPointYVal(9,primaryAxisNum) 'Get mag data for a high frequency

            if componType$="C" then
                'Low impedance at 100 kHz indicates a large capacitor. High impedance
                'at 40 MHz indicates a small capacitor. Large cap may be past its self-resonance at low freq, but
                'will still have low impedance. Small cap will not be significantly affected by self-resonance at 40 MHz.
                'We have to assume here small lead lengths on capacitors.
                pointNum=3  'For non-extreme capacitors, we will start at 450 kHz
                if componIsSeries then
                        'We can tell whether we have extreme values by looking at 100 kHz and 40 MHz
                    isLowZ=lowFreqDB>-0.1 : isHighZ=(isLowZ=0 and highFreqDB<-7) 'thresholds approx. 0.1 uF and 20 pF
                else    'shunt
                    isLowZ=lowFreqDB<-5.5 : isHighZ=(isLowZ=0 and highFreqDB>-1.4)  'thresholds approx. .1 uF and 100 pF
                end if
                if isLowZ then pointNum=1: nTries=0 'Stick with lowest frequency
                if isHighZ then pointNum=9  'start with highest frequency; may turn out hiZ is due to inductance
            else
                'Inductors are trickier, because losses can confuse the situation when just looking at S21 dB
                'So we make a guess at a starting point, but always do iteration, which separates L and R.
                'Low impedance at 40 MHz indicates a very small inductor, though a lossy small inductor
                'may be missed. It could also be large inductor that turned to a capacitor, but the iteration
                'will take care of that case.
                'A non-low impedance at 100 kHz indicates a large or lossy inductor. We will start with 100 kHz
                'and iterate from there.
                pointNum=6  'For non-extreme inductors, we will start at 4.2 MHz and iterate
                if componIsSeries then
                     isHighZ=(lowFreqDB<-1.8) :isLowZ=(isHighZ=0 and highFreqDB>-0.45)     'thresholds 100 uH and 100 nH
                else    'shunt
                    isHighZ=(lowFreqDB>-.9) : isLowZ=(isHighZ=0 and highFreqDB<-3.4)     'thresholds 100 uH and 100 nH
                end if
                if isHighZ then pointNum=1 'Start with lowest frequency
                if isLowZ then pointNum=9  'Start with highest frequency for small inductors
            end if
        end if

        for i=1 to nTries    'run through 0 or 2 times to get best freq.
                'get value and series resistance
            call ComponentGetValue componR0,componIsSeries,pointNum-1, componType$, componValue, serRes 'ver115-3d
            'See if we are at a reasonable frequency for this component value
            select componType$
                case "C"
                'ver115-3e rewrote the if... blocks as select statements
                    if componIsSeries then  'Series wants high Z, meaning lower frequencies
                        select case
                            case componValue<0 'ver115-3e
                                'The capacitor measured negative, which may be a sign it is past self-resonance, so we need to go
                                'with a low frequency; but go high if we are already low
                                if pointNum=1 then pointNum=9 else pointNum=max(1, int(pointNum/2)) 'ver115-3e
                            case  componValue>=2e-9     '5 nF and higher use low freq
                                pointNum=1  '100 KHz
                            case 5e-11<=componValue and componValue<5e-9   '50 pF to 5 nF
                                pointNum=4  '950 KHz
                            case else                           'under 50 pF
                                pointNum=9  '40 MHz
                        end select
                        'end Series C
                    else    'Shunt C wants low Z, meaning higher frequencies
                        select case 'Ranges modified by ver115-3e
                            case componValue<0 'ver115-3e
                                'The capacitor measured negative, which may be a sign it is past self-resonance, so we need to go
                                'with a low frequency; but go high if we are already low
                                if pointNum=1 then pointNum=9 else pointNum=max(1, int(pointNum/2))  'ver115-3e
                            case componValue>=5e-7     '500 nF and higher use low freq
                                pointNum=1  '100 KHz
                            case 5e-8<=componValue and componValue<5e-7   '50 nF to 500 nF
                                pointNum=2 '210 KHz
                            case 1e-9<=componValue and componValue<5e-8   '1 nF to 50 nF
                                pointNum=4  '950 KHz
                            case 1e-10<=componValue and componValue<1e-9   '100 pF to 1 nF ver115-4e
                                pointNum=6  '8.9 MHz
                            case else                           'under 100 pF
                                pointNum=9  '40 MHz
                        end select
                    end if  'end Shunt C
                case else   'Inductor
                    'Note: Inductor measurement is much less accurate without phase info, due to inductor
                    'losses. These ranges are set assuming phase is available. A prime goal is then to avoid
                    'the lowest frequencies, where LO leakage has significant effect.
                    if componIsSeries then  'Series wants high Z, meaning higher freq
                        select case
                            case componValue<0 'ver115-3e
                                'The inductor measured negative, which may be a sign it is past self-resonance, so we need to go
                                'with a low frequency; but go high if we are already low
                                if pointNum=1 then pointNum=9 else pointNum=max(1, int(pointNum/2))  'ver115-3e
                            case componValue>=0.001     '1 mH and higher these need low freq for low loss
                                pointNum=1  '100 KHz
                            case 1e-4<=componValue and componValue<0.001   '100 uH to 1 mH
                                pointNum=2  '210 KHz
                            case 1e-5<=componValue and componValue<1e-4   '10 uH to 100 uH
                                pointNum=5  '950KHz
                            case 3e-7<=componValue and componValue<1e-5   '300 nH to 10 uH
                                pointNum=7  '8.9 MHz
                            case else                           'under 300 nH
                                pointNum=9  '40 MHz
                        end select
                        'end Series L
                    else    'Shunt L wants low Z, meaning lower freq
                    'These are currently just copied from the series values, which are based on some tests
                        select case
                            case componValue<0 'ver115-3e
                                'The inductor measured negative, which may be a sign it is past self-resonance, so we need to go
                                'with a low frequency; but go high if we are already low
                                if pointNum=1 then pointNum=9 else pointNum=max(1, int(pointNum/2))  'ver115-3e
                            case componValue>=0.001     '1 mH and higher these need low freq for low loss
                                pointNum=1  '100 KHz
                            case 1e-4<=componValue and componValue<0.001   '100 uH to 1 mH
                                pointNum=2  '210 KHz
                            case 1e-5<=componValue and componValue<1e-4   '10 uH to 100 uH
                                pointNum=5  '950KHz
                            case 3e-7<=componValue and componValue<1e-5   '300 nH to 10 uH
                                pointNum=7  '8.9 MHz
                            case else                           'under 300 nH
                                pointNum=9  '40 MHz
                        end select
                    end if  'end Shunt L
            end select
        next i
    end if
        'get value and series resistance
    call ComponentGetValue componR0,componIsSeries,pointNum-1, componType$, componValue, serRes 'ver115-3d
end sub
*/
}
//ver115-3d added ComponentGetValue
void dialogComponentMeasure::ComponentGetValue()// componR0, isSeries, scanStep, componType$, byref value, byref serRes
{/*'Calculate value (ohms, F or H) of specified component
    'Returns value (Ohms, F or H) in value and, for L and C, series resistance in serRes
    'componR0 is R0 of the test fixture; ignored for reflection mode, where ReflectArray data already accounts for it
    'isSeries is 1 if fixture is Series; 0 if fixture is Shunt; ignored for reflection mode, where ReflectArray data already accounts for it
    'db is S21 or S11 db of the component in the fixture
    'phase is S21 or S11 phase, unless we are in ScalarTrans mode
    'trueFreq is frequency in Hz
    'componType$ is "R", "L" or "C"
    'scan step is the step (0...) at which we are measuring. It is an integer.
    if msaMode$="Reflection" then   'ver115-5b
        'For reflection mode, the values we need are already in ReflectArray()
        if ReflectArray(scanStep, constSerR)>0 then serRes=ReflectArray(scanStep, constSerR) else serRes=0
        if serRes<0.001 then serRes=0 else if serRes>1e9 then serRes=1e9    '0.001 to 1G ohms
        if componType$="C" then value=ReflectArray(scanStep, constSerC)
        if componType$="L" then value=ReflectArray(scanStep, constSerL)
        'For resistor, if reactance is inductive, assume small resistor with parasitic inductance and return series resistance
        'if reactance is capacitive return parallel resistance, because we are a large resistor in parallel with parasitic capacitance
        if componType$="R" then
            serX=ReflectArray(scanStep, constSerReact)
            if serX>0 then value=serRes else value=ReflectArray(scanStep, constParR)
        end if
        exit sub    'No more to do for reflection mode
    end if

    'Here for ScalarTrans or VectorTrans
    trueFreq=1000000*S21DataArray(scanStep, 0) : db=S21DataArray(scanStep, 1) : phase=S21DataArray(scanStep, 2)    'ver115-5e

    if db>0 then db=0
    if msaMode$="ScalarTrans" then
        'Calculate impedance from magnitude alone, assuming ideal phase
        componMag=10^(db/20)  'Magnitude of measured S21
        if componType$="R" then
            serX=0
            if isSeries then 'series R=(2*Ro) * (1-S21)/S21
                if componMag>0.9999 then serRes=0 else serRes=2*componR0*(1-componMag)/componMag
            else    'shunt R=(Ro/2) * S21 / (1-S21)
                if componMag>0.9999 then serRes=1e9 else serRes=componR0*componMag/(2*(1-componMag))
            end if
        else    'L and C--calculate reactance and then component value
            if isSeries then
                'For series fixture, abs(X)=+/- 2*R0*sqr(1-M21^2)/M21, where M21 is magnitude of S21
                if componMag<0.000001 then serX=0 else serX=2*componR0*sqr(1-componMag^2)/componMag
            else    'shunt
                'For shunt fixture, abs(X)=+/- R0*M21/(2*sqr(1-M21^2)), where M21 is magnitude of S21
                if componMag>0.9999 then serX=1e9 else serX=componR0*componMag/(2*sqr(1-componMag^2))
            end if
            if componType$="C" then serX=0-serX 'capacitors have negative reactance
        end if
    else    'VectorTrans
        'Calculate impedance from jig
        if isSeries then
            call uSeriesJigImpedance componR0, db, phase, serRes, serX
        else
                'We use no delay here, so we just provide a frequency of 1
            call uShuntJigImpedance componR0, db, phase, 0,1,serRes, serX  'assumes zero connector delay ver115-4a
        end if
    end if

    'Get here for ScalarTrans or VectorTrans
    'serRes and serX now have the series resistance and reactance
    if serRes<0.001 then serRes=0 else if serRes>1e9 then serRes=1e9    '0.001 to 1G ohms; avoid tiny non-zero values
    if componType$="R" then
        if serX>0 then value=serRes : exit sub 'If reactance is inductive, assume small resistor with parasitic inductance and return series resistance
        'Here we want to return parallel resistance, because we are a large resistor in parallel with parasitic capacitance
        call uEquivParallelImped serRes, serX, parRes, parX
        value=parRes
        exit sub
    end if

        'Here for L or C. Convert reactance to component value
    if componType$="C" then
        if serX=0 then value=1 else value=-1/(2*uPi()*serX*trueFreq) 'capacitance in farads
    else    'Inductor
        value=serX/(2*uPi()*trueFreq) 'inductance in henries
    end if
    if value>1 then value=1 'max of 1F or 1H
    'ver115-3e deleted the lower limit on L and C, because it is possible they end up
    'with negative values, which is a useful sign that they are past their self-resonance
    'and we need to lower the frequency.
end sub
*/
}
