#include "dialogreflectionrlc.h"
#include "ui_dialogreflectionrlc.h"

dialogReflectionRLC::dialogReflectionRLC(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogReflectionRLC)
{
  ui->setupUi(this);
}

dialogReflectionRLC::~dialogReflectionRLC()
{
  delete ui;
}
void dialogReflectionRLC::ReflectionRLC()
{/*//Reflection mode RLC Analysis  renamed ver115-5d
    WindowWidth = 625 : WindowHeight = 375
    call GetDialogPlacement
    BackgroundColor$="gray" : ForegroundColor$="black" : TextboxColor$ = "white"

    statictext #RLC.title, "DETERMINATION OF COMBINED RLC PARAMETERS",70, 10, 375, 20
    s$="This function determines the combined resistor, capacitor and inductor that best model the current scan data."
    s$=s$;" Analysis may be done (1) at all frequencies using the value and slope of reactance or susceptance"
    s$=s$;" or (2) at two specific frequencies identified by markers before this dialog was opened. If the slope"
    s$=s$;" method is used, the slope is determined by the specified number of points around the target point, and"
    s$=s$;" graphs will also be created with the results at all frequencies, with one frequency displayed in this dialog."
    statictext #RLC.title1, s$,20, 30, 570, 110

    controlTop=145
    checkbox #RLC.parallelRLC, "Parallel RLC", void dialogReflectionRLC::RefRLCParallelRLC(), void dialogReflectionRLC::RefRLCParallelRLC(), 10, controlTop, 150,20
    checkbox #RLC.seriesRLC, "Series RLC", void dialogReflectionRLC::RefRLCSeriesRLC(), void dialogReflectionRLC::RefRLCSeriesRLC(), 10, controlTop+20, 150,20
    //ver116-4h deleted the lossy inductor option and added Two-Frequency option
    checkbox #RLC.slope, "Use Slope Method", void dialogReflectionRLC::RefRLCSlope(), void dialogReflectionRLC::RefRLCSlope(), 200, controlTop, 250,20
    checkbox #RLC.twoFreq, "Use Two-Frequency Method", void dialogReflectionRLC::RefRLCTwoFreq(), void dialogReflectionRLC::RefRLCTwoFreq(), 200, controlTop+20, 250,20

    textbox #RLC.nPoints, 355, controlTop+50, 50, 20
    statictext #RLC.nPointsLabel, "Number of points to include:", 10, controlTop+52, 340, 20
    textbox #RLC.results, 10, controlTop+95, 575, 25      //textbox for results
    button #RLC.decFreq, "-Freq", void dialogReflectionRLC::decFreq(), UL,15,controlTop+125, 50, 20
    button #RLC.incFreq, "+Freq", void dialogReflectionRLC::incFreq(), UL,67,controlTop+125, 50, 20
    button #RLC.Analyze, "Analyze", void dialogReflectionRLC::RefRLCAnalyze(), UL,100,controlTop+170, 100, 25
    button #RLC.Analyze, "Help", RefRLCHelp, UL,300,controlTop+170, 100, 25

        //Open dialog
    open "RLC Analysis" for dialog_modal as #RLC

    #RLC, "trapclose void dialogReflectionRLC::RefRLCCancel()"
    #RLC, "font ms_sans_serif 10"
    #RLC.results, "!font Arial 11 bold"
    pCount=gPointCount()
    if pCount<2 then notice "Not enough points for analysis." : goto void dialogReflectionRLC::RefRLCCancel()
    #RLC.nPointsLabel, "Number of points to include in slope calculations (2-"; int(pCount/4);"):"
    if RefRLCLastNumPoints>=2 and RefRLCLastNumPoints<=pCount then
        defPoints=RefRLCLastNumPoints
    else
        defPoints=max(2,int(pCount/60)*2+1)     //approx pCount/30, but an odd number
    end if
    #RLC.incFreq, "!hide" : #RLC.decFreq, "!hide"
    #RLC.nPoints, defPoints
        //If we have markers, identify two of them and default to the two-frequency method
    mCount=0    //start with 0 markers found. Then look for markers with positive point numbers
    mPt1=0 : mPt2=0 : m1$="" : m2$=""   //marker point numbers and IDs
    m$="P+" : mOrd=mMarkerNum(m$) : mPt=gMarkerPointNum(mOrd)  //start with P+
    if mPt>0 then
        mCount=1 : m1$=m$ : mPt1=mPt    //save point num of P+
    else    //No P+; look for P-
        m$="P-" : mOrd=mMarkerNum(m$) : mPt=gMarkerPointNum(mOrd)
        if mPt>0 then mCount=1 : m1$=m$ : mPt1=mPt    //save point num of P- if it exists and P+ did not
    end if
    mList$="LR123456"   //search in this order
    for i=1 to 8  //check for other markers until we have two
        m$=Mid$(mList$,i,1) : mOrd=mMarkerNum(m$) : mPt=gMarkerPointNum(mOrd)
        if mPt>0 then
            if mCount=0 then m1$=m$ : mPt1=mPt else m2$=m$ : mPt2=mPt
            mCount=mCount+1 : if mCount>=2 then exit for //Done once we find two.
        end if
    next i
    if mCount=2 then
        //Make marker 1 the one with with S11 angle nearest 0 or 180
        a1=abs(ReflectArray(int(mPt1-0.5), constGraphS11Ang))    //abs val of S11 angle at point 1 (point is rounded off for simplicity)
        a2=abs(ReflectArray(int(mPt2-0.5), constGraphS11Ang))    //abs val of S11 angle at point 2 (point is rounded off for simplicity)
        d1=min(a1, abs(a1-180)) : d2=min(a2, abs(a2-180))  //minimum angle distance to horizontal
        if d1>d2 then sPt=mPt2 : sID$=m2$ : mPt2=mPt1 : m2$=m1$ : mPt1=sPt : m1$=sID$ //swap points 1 and two
        smallestAngToResonance=min(d1, d2)
        gosub void dialogReflectionRLC::RefRLCChooseTwoFreq() //start with Two Freq method if we have 2 markers
    else
        #RLC.twoFreq, "disable"   //disable two freq method if not two markers
        gosub void dialogReflectionRLC::RefRLCChooseSlope()
    end if
    if RefRLCLastConnect$="Series" or RefRLCLastConnect$=""then goto void dialogReflectionRLC::RefRLCSeriesRLC()
    goto void dialogReflectionRLC::RefRLCParallelRLC()
*/
}
//If the user double clicks on the menu when invoking this routine, the second click can be
//interpreted by LB as a click on the graph, so it will call void dialogReflectionRLC::LeftButDown(). But it doesn//t do
//so until it gets into this routine, at which point the real void dialogReflectionRLC::LeftButDown() is hidden. So
//we provide one here that just exits.
//void dialogReflectionRLC::LeftButDown()
//{/*
//    goto void dialogReflectionRLC::RefRLCCancel()
//*/
//}*/
void dialogReflectionRLC::RefRLCCancel()
{/*    close #RLC
    exit sub
*/
}
void dialogReflectionRLC::RefRLCSeriesRLC()
{/*    #RLC.seriesRLC, "set" : #RLC.parallelRLC, "reset"
    #RLC.nPoints, defPoints : #RLC.nPoints, "!enable"
    gosub void dialogReflectionRLC::RefRLCFindResonance()
    wait
*/
}
void dialogReflectionRLC::RefRLCParallelRLC()
{/*    #RLC.seriesRLC, "reset" : #RLC.parallelRLC, "set"
    #RLC.nPoints, defPoints : #RLC.nPoints, "!enable"
    gosub void dialogReflectionRLC::RefRLCFindResonance()
    wait
*/
}
void dialogReflectionRLC::RefRLCSlope()
{/*    gosub void dialogReflectionRLC::RefRLCChooseSlope():wait
*/
}
void dialogReflectionRLC::RefRLCTwoFreq()
{/*    gosub void dialogReflectionRLC::RefRLCChooseTwoFreq():wait
*/
}
void dialogReflectionRLC::RefRLCChooseSlope()
{/*    #RLC.slope, "set" : #RLC.twoFreq, "reset"
    #RLC.nPointsLabel, "!show" : #RLC.nPoints, "!show"
    return
*/
}
void dialogReflectionRLC::RefRLCChooseTwoFreq()
{/*    #RLC.slope, "reset" : #RLC.twoFreq, "set"
    #RLC.nPointsLabel, "!hide" : #RLC.nPoints, "!hide"
    return
*/
}
void dialogReflectionRLC::RefRLCFindResonance()
{/*    #RLC.seriesRLC, "value? seriesVal$"
    #RLC.parallelRLC, "value? parallelVal$"
    if seriesVal$="set" then RefRLCLastConnect$="Series" else RefRLCLastConnect$="Parallel"
            //Find where reactance or susceptance crosses zero in positive direction
    lowStep=-1 : highStep=-1
    lastVal=0   //ver115-4b
    for i=0 to pCount-1
        v=ReflectArray(i, constSerReact)
        if seriesVal$="reset" then
                //Parallel; get susceptance
            R=ReflectArray(i, constSerR)
            call cxInvert R, v, G, S //convert to admittance
            v=S
        end if
        if i=0 then
            lastVal=v
        else
            if lastVal<0 and v>=0 then lowStep=i-1 : highStep=i : exit for //found crossing ver115-4b
        end if
    next i
    if highStep<1 then
        resonStep=-1    //We don't have the resonant frequency
        analysisPoint=int(gPointCount()/2)
    else
        lowVal=ReflectArray(lowStep, constSerReact)
        highVal=ReflectArray(highStep, constSerReact)
        if highVal=lowVal then fract=0 else fract=abs(lowVal)/(highVal-lowVal)  //interpolation fraction to get to zero. Numerator is 0-lowVal ver115-4b
        resonStep=lowStep+fract
        analysisPoint=int(resonStep+0.5)+1     //Display analysis at point closest to resonant frequency
    end if
    return
*/
}
void dialogReflectionRLC::incFreq()
{/*    if analysisPoint<pCount-int(nPoints/2) then analysisPoint=analysisPoint+1
    goto void dialogReflectionRLC::RefRLCCalcAndDisplayPointValues()
*/
}
void dialogReflectionRLC::decFreq()
{/*    if analysisPoint>int(nPoints/2) then analysisPoint=analysisPoint-1
    goto void dialogReflectionRLC::RefRLCCalcAndDisplayPointValues()
*/
}
void dialogReflectionRLC::RefRLCCalcAndDisplayPointValues()
{/*//Get values for slope method; subtract 1 to get analysis step from analysis point
    if RefRLCLastConnect$="Series" then R=ReflectArray(analysisPoint-1,constSerR) _
            else R=ReflectArray(analysisPoint-1,constParR)
    L=auxGraphData(analysisPoint-1, 0) : C=auxGraphData(analysisPoint-1, 1)
    Qu=auxGraphData(analysisPoint-1, 2)
    f$="F=";str$(ReflectArray(analysisPoint-1,0));" MHz; "    //freq
    if Qu<10 then Qu$="; Qu=";using("#.###", Qu) else Qu$="; Qu=";uFormatted$(Qu, "3,3,4//UseMultiplier")
 void dialogReflectionRLC::RefRLCDisplayPointValues()     //Common to both methods
    RLab$="Rs=" : LLab$="Ls=" : CLab$="Cs="    //Assume series  //ver115-5d
    if RefRLCLastConnect$="Parallel" then RLab$="Rp=" : LLab$="Lp=" : CLab$="Cp="
    form$="3,3,4//UseMultiplier//DoCompact" //Max 3 whole dig, max 3 dec dig, max 4 total sig dig.
    resForm$="3,3,4//UseMultiplier//SuppressMilli//DoCompact" //ver115-4e
    if R<1 then R$=using("#.####", R);" " else R$=uFormatted$(R, resForm$)   //ver115-4e
    L$=uFormatted$(L,form$)
    C$=uFormatted$(C,form$)
    print #RLC.results, f$;RLab$;R$;"; ";LLab$;L$;"H; ";CLab$;C$;"F";Qu$;serR$  //ver116-4h
    wait
*/
}
void dialogReflectionRLC::RefRLCAnalyze()
{/*    #RLC.slope, "value? slope$"
    serR$=""
    if slope$="set" then    //ver116-4h added the two-frequency method
        #RLC.nPoints, "!contents? nPoints$"
        nPoints=val(nPoints$)
        if nPoints<2 or nPoints>pCount/4 then
            nPoints=defPoints : #RLC.nPoints, nPoints
            notice "Invalid number of points. ";defPoints;" used."
        end if
        RefRLCLastNumPoints=nPoints //save for next time we enter this dialog
        if nPoints=2 then
            nLeft=0 : nRight=1
        else
            nLeft=int(nPoints/3) : nRight=nPoints-nLeft-1   //skew points to right of "center"
        end if
        call ClearAuxData   //Clear auxiliary graph data by blanking graph names
        call gGetMinMaxPointNum pMin, pMax
        if RefRLCLastConnect$="Series" then
            dataType1=constSerReact : dataType2=constSerR
        else
            dataType1=constSusceptance : dataType2=constConductance
        end if
        call uBestFitLines dataType1, dataType2, nPoints,pMin-1, pMax-1 //Assemble data and slopes
        call DetermineLCEquiv RefRLCLastConnect$,pMin-1, pMax-1,resonStep
        call ChangeGraphsToAuxData constAux0, constAux1 //Graph L and C
            //Display values
        #RLC.incFreq, "!show" : #RLC.decFreq, "!show"   //Enable frequency change
        goto void dialogReflectionRLC::RefRLCCalcAndDisplayPointValues()
    else    //use two-frequency method
        freq1=gGetPointXVal(mPt1)   //get frequencies at the two marker points
        freq2=gGetPointXVal(mPt2)
        call CalcGraphDataType mPt1-1, constSerR, constSerReact, R1, X1, 0  //get marker R and X
        call CalcGraphDataType mPt2-1, constSerR, constSerReact, R2, X2, 0
        if RefRLCLastConnect$="Series" then
            call uSeriesRLCFromPoints R1, X1, freq1, R2, X2, freq2, R, L, C
        else
            call uParallelRLCFromPoints R1, X1, freq1, R2, X2, freq2, R, L, C
        end if
        f$="Markers ";m1$;" and ";m2$;": "    //display marker IDs rather than frequencies for two-point method
        #RLC.incFreq, "!hide" : #RLC.decFreq, "!hide"   //disable frequency change
        Qu$=""
        if smallestAngToResonance<=30 then
            //We calculate Qu at freq1, which is the closest to resonance, but only if its angle from resonance is not large
            LReact=2*uPi()*L*1000000*freq1
            if RefRLCLastConnect$="Series" then
                if R=0 then Qu=99999 else Qu=LReact/R   //Qu for series
            else
                if LReact=0 then Qu=99999 else Qu=R/LReact
                if Qu=0 then serR=1e9 else serR=LReact/Qu  //Equivalent series R. Works near resonance
                if comboSerR<1 then serR$=using("#.###", serR) else serR$=uFormatted$(serR, "3,3,4//UseMultiplier")
                serR$="; (Rs=";serR$;")"
            end if
            if Qu<10 then Qu$="; Qu=";using("#.###", Qu) else Qu$="; Qu=";uFormatted$(Qu, "3,3,4//UseMultiplier")
        end if
        goto void dialogReflectionRLC::RefRLCDisplayPointValues()
    end if
    wait    //Analyze is done
end sub
*/
}
void dialogReflectionRLC::RefRLCHelp(QString h)
{/*    WindowWidth = 550
    WindowHeight = 400
    call GetDialogPlacement
    BackgroundColor$="gray"
    ForegroundColor$="black"

    s$="RLC analysis done in Reflection mode will use impedance information to model the response as a combination of"
    s$=s$;" a resistor, inductor and capacitor. The simplest equivalent circuits use either an L or value to model the"
    s$=s$;" reactance at each frequency, but here we use both an L and a C value to model not only the reactance, but its change over"
    s$=s$;" frequency. An RLC circuit has varying reactance over frequency, but has constant L and C values. This analysis"
    s$=s$;" will determine those values. The components may be specified in series or in parallel."
    statictext #RLCExplain, s$, 10, 10, 515, 115

    s$="The analysis will be most accurate in the general area of resonance. If you choose the two-frequency"
    s$=s$;" method, you must already have placed markers at two frequencies. Typically, you would place the P+ marker"
    s$=s$;" at a peak or the P- marker at a notch, and a second marker near a point where phase transistions from rapid"
    s$=s$;" to slow change."
    statictext #RLCExplain, s$, 10, 135, 515, 70

    s$=" If you use the slope method, the calculation will be done at all frequencies using your specified number of"
    s$=s$;" points to calculate the slope of reactance or susceptance at each frequency, graphs will be created with the"
    s$=s$;" results, and the results for one frequency will"
    s$=s$;" be displayed in the dialog. You can change the display frequency with the +Freq and -Freq buttons. Note that slope"
    s$=s$;" calculations will be much less accurate at the edges of the scan, or in areas where slope changes rapidly."
    s$=s$;" Using a large number of points to calculate slope makes the results less susceptible to noise, but is not"
    s$=s$;" advisable where the slope changes rapidly. Typically the slope is fairly constant near resonance."
    statictext #RLCExplain, s$, 10, 215, 515, 130

    open "RLC Analysis Help" for dialog_modal as #RLCExplain
    print #RLCExplain, "font ms_sans_serif 10"
    print #RLCExplain, "trapclose void dialogReflectionRLC::RefRLCExplainEnd()"
    wait
*/
}
void dialogReflectionRLC::RefRLCExplainEnd()
{/*    close #RLCExplain
    exit sub
end sub //end RefRLCHelp
*/
}
