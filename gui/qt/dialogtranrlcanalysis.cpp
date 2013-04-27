#include "dialogtranrlcanalysis.h"
#include "ui_dialogtranrlcanalysis.h"

dialogTranRLCAnalysis::dialogTranRLCAnalysis(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogTranRLCAnalysis)
{
  ui->setupUi(this);
}

dialogTranRLCAnalysis::~dialogTranRLCAnalysis()
{
  delete ui;
}
void dialogTranRLCAnalysis::TranRLCAnalysis()
{/*//perform RLC analysis, transmission mode
    //We determine Q from resonant frequency and -3 dB bandwidth, and directly measure Rs
    //at resonance. From Q and Rs we can calculate L and C.

    if (primaryAxis=1 and Y1DataType<>constMagDB) or (primaryAxis=2 and Y2DataType<>constMagDB) then _
                 notice "Primary axis must contain a graph of magnitude (dB)." : exit sub
    WindowWidth = 550
    WindowHeight = 400
    call GetDialogPlacement
    BackgroundColor$="gray"
    ForegroundColor$="black"
    TextboxColor$ = "white"

    statictext #tranRLC.title, "DETERMINATION OF COMBINED RLC PARAMETERS",70, 10, 400, 20
    s$="Determines individual components of an RLC combination from resonance and 3 dB points."
    s$=s$;" The scan must include the resonance and at least one of the 3 dB points."
    s$=s$;" High resolution improves accuracy."   //ver115-5f
    statictext #tranRLC.title1, s$, 20, 30, 500, 32
    s$="The resistor, inductor and/or capacitor are in PARALLEL."
    checkbox #tranRLC.parallelRLC, "", void dialogTranRLCAnalysis::tranRLCParallelRLC(), void dialogTranRLCAnalysis::tranRLCParallelRLC(), 10, 65, 20,20
    statictext #tranRLC.inst, s$, 30, 68, 475, 18
    s$="The resistor, inductor and/or capacitor are in SERIES."
    checkbox #tranRLC.seriesRLC, "", void dialogTranRLCAnalysis::tranRLCSeriesRLC(), void dialogTranRLCAnalysis::tranRLCSeriesRLC(), 10, 90, 20,20
    statictext #tranRLC.inst2, s$, 30, 93, 500, 20

    tranRLCFixTop=120
    groupbox #tranRLC.Fixture, "Fixture", 100,tranRLCFixTop,210,75
        //Provision is made for series/shunt, R0, and shunt connector delay
    checkbox #tranRLC.seriesFix, "Series", void dialogTranRLCAnalysis::tranRLCSeriesFix(), void dialogTranRLCAnalysis::tranRLCSeriesFix(), 120, tranRLCFixTop+20, 50, 20
    checkbox #tranRLC.shuntFix, "Shunt", void dialogTranRLCAnalysis::tranRLCShuntFix(), void dialogTranRLCAnalysis::tranRLCShuntFix(),120, tranRLCFixTop+45, 50, 20
    textbox #tranRLC.R0, 240, tranRLCFixTop+20, 50, 20
    statictext #tranRLC, "R0 (ohms)", 188, tranRLCFixTop+22, 50, 20
    textbox #tranRLC.Delay, 240, tranRLCFixTop+45, 50, 20
    statictext #tranRLC.DelayLabel, "Delay (ns)", 188, tranRLCFixTop+47, 50, 20
            //ver116-4h added notch items and 3 dB explanation.
    tranRLCNotchTop=tranRLCFixTop+90
    checkbox #tranRLC.useNotchTop, "Use points at absolute -3 dB. (Best for narrow notches.)", void dialogTranRLCAnalysis::tranRLCUseTop(),void dialogTranRLCAnalysis::tranRLCUseBot(), 10, tranRLCNotchTop, 400, 18
    checkbox #tranRLC.useNotchBot, "Use points +3 dB from notch bottom. (Notch depth should exceed 20 dB.)", void dialogTranRLCAnalysis::tranRLCUseBot(),void dialogTranRLCAnalysis::tranRLCUseTop(), 10, tranRLCNotchTop+20, 400, 18
    statictext #tranRLC.3dBExplain "", 40, tranRLCNotchTop+45, 400, 40

            //Buttons
    tranRLCBtnTop=tranRLCNotchTop+90
    button #tranRLC.analyze, "Analyze", void dialogTranRLCAnalysis::tranRLCAnalyze(), UL,50, tranRLCBtnTop, 80,30
    button #tranRLC.Help, "Help", tranRLCExplain, UL,250, tranRLCBtnTop, 80,30
    textbox #tranRLC.results, 10, tranRLCBtnTop+40, 520, 20      //textbox for results

        //Open dialog
    open "RLC Analysis" for dialog_modal as #tranRLC    //ver115-4a made this a modal dialog  //ver115-4a
    #tranRLC, "trapclose void dialogTranRLCAnalysis::tranRLCCancel()" //ver115-3b
    #tranRLC, "font ms_sans_serif 9"   //ver116-4h
    #tranRLC.3dBExplain, "!font Arial 10 bold"   //ver116-4h

    call mClearMarkers  //Extra Markers just cause visual confusion ver115-3b
    #tranRLC.analyze, "!setfocus" //to keep window in front
    if S21JigAttach$="Series" then #tranRLC.seriesFix, "set" else #tranRLC.shuntFix, "set"  //ver115-3f
    #tranRLC.R0, S21JigR0  //ver115-3f
    #tranRLC.Delay, S21JigShuntDelay //ver115-4a
    //ver115-4b deleted save/restore of S21 Jig items
    lastComputedDelay=0
    if tranRLCLastNotchWasTop then #tranRLC.useNotchTop, "set" : #tranRLC.useNotchBot, "reset" _
                        else #tranRLC.useNotchTop, "reset" : #tranRLC.useNotchBot, "set"   //ver116-4h
    if tranRLCLastRLCWasSeries then #tranRLC.seriesRLC, "set" : #tranRLC.parallelRLC, "reset" _
                else #tranRLC.seriesRLC, "reset" : #tranRLC.parallelRLC, "set"  //ver116-4h
    if S21JigAttach$="Series" then goto void dialogTranRLCAnalysis::tranRLCSeriesFix() else goto void dialogTranRLCAnalysis::tranRLCShuntFix()  //ver115-4b
*/
}
void dialogTranRLCAnalysis::tranRLCUseTop()
 {/*
    #tranRLC.useNotchTop, "set" : #tranRLC.useNotchBot, "reset" : goto void dialogTranRLCAnalysis::tranRLCExplainNotchAndDB()
*/
}
void dialogTranRLCAnalysis::tranRLCUseBot()
{/*
    #tranRLC.useNotchTop, "reset" : #tranRLC.useNotchBot, "set" : goto void dialogTranRLCAnalysis::tranRLCExplainNotchAndDB()
*/
}
void dialogTranRLCAnalysis::tranRLCSeriesFix()
{/*//mod116-4h
    #tranRLC.seriesFix, "set" : #tranRLC.shuntFix, "reset"
    #tranRLC.Delay, "!hide" : #tranRLC.DelayLabel, "!hide"
    #tranRLC.seriesRLC, "value? tranRLCSeriesRLC$" : if tranRLCSeriesRLC$="set" then tranRLCNotch=0 else tranRLCNotch=1   //ver116-4h
    goto void dialogTranRLCAnalysis::tranRLCExplainNotchAndDB()
*/
}
void dialogTranRLCAnalysis::tranRLCShuntFix()
          {/*//mod116-4h
    #tranRLC.shuntFix, "set" : #tranRLC.seriesFix, "reset"
    if msaMode$<>"ScalarTrans" then #tranRLC.Delay, "!show" : #tranRLC.DelayLabel, "!show"  //Only meaninful with phase ver115-4a
    #tranRLC.seriesRLC, "value? tranRLCSeriesRLC$" : if tranRLCSeriesRLC$="set" then tranRLCNotch=1 else tranRLCNotch=0   //ver116-4h
    goto void dialogTranRLCAnalysis::tranRLCExplainNotchAndDB()
*/
}
void dialogTranRLCAnalysis::tranRLCSeriesRLC()
{/*//mod116-4h
    #tranRLC.seriesRLC, "set" : #tranRLC.parallelRLC, "reset"
    #tranRLC.seriesFix, "value? tranRLCSeries$" : if tranRLCSeries$="set" then tranRLCNotch=0 else tranRLCNotch=1   //ver116-4h
    goto void dialogTranRLCAnalysis::tranRLCExplainNotchAndDB()
*/
}
void dialogTranRLCAnalysis::tranRLCParallelRLC()
{/*//mod116-4h
    #tranRLC.seriesRLC, "reset" : #tranRLC.parallelRLC, "set"
    #tranRLC.seriesFix, "value? tranRLCSeries$" : if tranRLCSeries$="set" then tranRLCNotch=1 else tranRLCNotch=0   //ver116-4h
    goto void dialogTranRLCAnalysis::tranRLCExplainNotchAndDB()
*/
}
void dialogTranRLCAnalysis::tranRLCExplainNotchAndDB()
{/*
    //Show notch items if using notch response (series RLC in parallel fixture or parallel RLC in series fixture)
    if tranRLCNotch then
        #tranRLC.useNotchTop, "show" : #tranRLC.useNotchBot, "show"
        #tranRLC.useNotchTop, "value? tranRLCUseTop$"
        if tranRLCUseTop$="set" then
            #tranRLC.3dBExplain, "Scan must show resonant notch and at least one point at absolute -3 dB level."
        else
            #tranRLC.3dBExplain, "Scan must show resonant notch and at least one point 3 dB above notch bottom."
        end if
    else
        #tranRLC.3dBExplain, "Scan must show resonant peak and at least one point 3 dB below peak."
        #tranRLC.useNotchTop, "hide" : #tranRLC.useNotchBot, "hide"
    end if
    wait
*/
}
void dialogTranRLCAnalysis::tranRLCAnalyze()
{/*//Analyze button was pushed
    #tranRLC.seriesFix, "value? tranRLCSeries$"  //ver115-3f
    if tranRLCSeries$="set" then S21JigAttach$="Series" else S21JigAttach$="Shunt"  //ver115-3f
    #tranRLC.R0, "!contents? componR0$"   //R0  //ver115-3f
    S21JigR0=val(componR0$)  //ver115-3f
    if S21JigR0<0 then S21JigR0=50 : #tranRLC.R0, "50" :notice "Invalid R0. 50 ohms used."   //ver115-3f
    analysisAxis$=str$(primaryAxisNum)    //ver115-3b
    #tranRLC.seriesRLC, "value? tranRLCSeriesRLC$"  //"set" if RLC circuit is series
    tranRLCLastRLCWasSeries=(tranRLCSeriesRLC$="set")   //For next time we open ver116-4h
    if S21JigAttach$="Shunt" then //ver115-4a
        #tranRLC.Delay, "!contents? tranRLCDelay$"   //Delay of shunt connection
        S21JigShuntDelay=val(tranRLCDelay$)
        if S21JigShuntDelay<>lastComputedDelay then
            //If using shunt fixture with delay, we must reconstruct the graphs after compensating
            //for the delay. We do this by transforming S21 into impedance while accounting for the delay,
            //and recalculating S21 with that new impedance but with no delay.
            for i=0 to gPointCount()-1
                currFreq=S21DataArray(i,0)*1000000  //Has actual graph frequency
                    //Use datatable data but put new data only into S21DataArray, so if we do Analyze
                    //twice, the second time we start again with unchanged data.
                newDB=datatable(i, 2) : newDeg=datatable(i,3)
                call uAdjustS21ForConnectorDelay currFreq, newDB, newDeg
                //Change the raw data; graph will be changed by RefreshGraph, called below
                S21DataArray(i,1)=newDB
                S21DataArray(i,2)=newDeg
                if primaryAxisNum=1 then call gChangePoints i+1, newDB, newDeg _
                        else call gChangePoints i+1, newDeg, newDB  //so graph module has the new data
            next i
            refreshTracesDirty=1    //So RefreshGraph recalculates Y values.
            lastComputedDelay=S21JigShuntDelay
        end if
    end if

    //We analyze a positive peak if series RLC in series fixture or parallel RLC in shunt fixture.
    //tranRLCNotch will already be set to 0 if doing a peak and 1 if doing a notch
    if tranRLCNotch then//ver116-4h
        #tranRLC.useNotchTop, "value? tranRLCUseTop$"   //set if we want top 3 dB points
        call mAddMarker "P-", 1, analysisAxis$   //place P- marker on primary trace ver115-4a
        doLRRelativeTo$="P-"    //Indicate to place L and R at +3db points around P-
        if tranRLCUseTop$="set" then
            doLRRelativeAmount=-3 : doLRAbsolute=1  //mark absolute -3 dB  //ver116-4h
            tranRLCLastNotchWasTop=1   //For next time we open
        else
            doLRRelativeAmount=3 : doLRAbsolute=0  //mark 3 dB above P-   //ver116-4h
            tranRLCLastNotchWasTop=0   //For next time we open
        end if
    else    //Here we analyze a peak
        call mAddMarker "P+", 1, analysisAxis$   //place P+ marker on primary trace ver115-4a
        doLRRelativeTo$="P+"    //Indicate to place L and R at -3db points around P+
        doLRRelativeAmount=-3
        doLRAbsolute=0  //ver115-4a
    end if

    call RefreshGraph 0 //To place the peak markers properly
    doLRRelativeTo$=""  //disable auto placement now that we have done it
    doLRAbsolute=0      //User never does absolute values //ver115-3f
    //We now have markers at all the critical points. We just have to get the marker info.
    //The main resonance is a peak if we have a series RLC in a series fixture, or parallel RLC in parallel fixture.
    if (S21JigAttach$="Series" and tranRLCSeriesRLC$="set") or _
                (S21JigAttach$="Shunt" and tranRLCSeriesRLC$="reset") then  //ver115-3f
        tranRLCPeakOrdinal=mMarkerNum("P+")   //Ordinal of P+ point; graph module finds info from ordinal
    else
        tranRLCPeakOrdinal=mMarkerNum("P-")   //Ordinal of P- point; graph module finds info from ordinal
    end if

    tranRLCFs=gMarkerCurrXVal(tranRLCPeakOrdinal)   //We now have the exact Fs

    tranRLCPeakPoint=gMarkerPointNum(tranRLCPeakOrdinal)    //Point number of the main peak or notch
    tranRLCPeakdb=gGetPointYVal(tranRLCPeakPoint, primaryAxisNum)    //Get S21 db value at peak (primary trace)
    tranRLCLNum=mMarkerNum("L")
    tranRLCRNum=mMarkerNum("R")
    tranRLC3dbA=gMarkerCurrXVal(tranRLCLNum)
    tranRLC3dbB=gMarkerCurrXVal(tranRLCRNum)
        //ver116-4h revised checking for 3 dB points
    tranLPoint=gMarkerPointNum(tranRLCLNum) //Point num of left 3 dB point, or <=0 if it does not exist
    tranRPoint=gMarkerPointNum(tranRLCRNum) //Point num of right 3 dB point, or <=0 if it does not exist
    if tranLPoint<=0 and tranRPoint<=0 then notice "Scan does not include any 3 dB points." : wait
    //We have at least one 3dB point. If we don't have the other, we calculate its frequency with formula that works for low loss
    if tranLPoint<=0 then tranRLC3dbA=tranRLCFs^2/tranRLC3dbB   //makes resonance the geometric mean
    if tranRPoint<=0 then tranRLC3dbB=tranRLCFs^2/tranRLC3dbA   //makes resonance the geometric mean

    if (tranRLCNotch=1) and (tranRLCUseTop$="reset") then  //ver116-4h
        //analyze bottom of notch. Q determined from the 3 dB bandwidth is assumed to be Qu
        if S21JigAttach$="Series" then
           comboR=2*S21JigR0*(10^(0-tranRLCPeakdb/20)-1)    //This is the effective series or parallel resistance at resonance, not counting source and load
        else    //shunt fixture
           comboR=(S21JigR0/2)/(10^(0-tranRLCPeakdb/20)-1)
        end if
        if comboR<0.001 then comboR=0.001
        BW=tranRLC3dbB-tranRLC3dbA
        if BW<=0 or tranRLCFs<=0 then notice "Invalid frequency data." : wait
        comboQu=tranRLCFs/BW : if comboQu>99999 then comboQu=99999  //Qu at resonance. Accurate if notch is deep.
        if tranRLCSeriesRLC$="set" then
            //series RLC in shunt fixture
           resonReact=comboR*comboQu : comboQL=resonReact/(S21JigR0/2) : comboSerR=comboR
        else    //parallel RLC in series fixture
           resonReact=comboR/comboQu : comboQL=resonReact/(S21JigR0*2) : comboSerR=resonReact/comboQu
        end if
        twoPiF=2*uPi()*tranRLCFs*1000000
        comboL=resonReact/twoPiF   //in H
        comboC=1/(twoPiF*resonReact) //in F
    else    //Analyze top of peak or notch
            //For a low-loss RLC circuit, the resonant frequency is the geometric mean of the two 3 dB points. The upper 3 dB
            //point may be at a dramatically higher frequency than the lower one and in that case is subject to a lot of inaccuracies.
            //So if it is too high, we will redetermine it mathematically, assuming low loss.
        if (transRLCNotch=0 or tranRLCUseTop$="set") and tranRLC3dbB>2*tranRLCFs and tranRLC3dbB>1e8 then  //upper point is above 100 MHz and more than twice Fs
            tranRLC3dbB=tranRLCFs^2/tranRLC3dbA   //makes Fs the geometric mean
        end if
        if tranRLCSeriesRLC$="set" then
                //For series RLC we use the crystal routine; crystFp and crystCp are bogus
            crystFp=tranRLCFs*1.5 //just to be valid
            call uCrystalParameters tranRLCFs, crystFp, tranRLCPeakdb, tranRLC3dbA, tranRLC3dbB, comboR, comboC, comboL, crystCp, comboQu, comboQL
            comboL=comboL/1000000   //It was returned in uH
            comboC=comboC*1e-12     //It was returned in pF
        else
                //Parallel RLC
            call uParallelRLCFromScalarS21 tranRLCFs, tranRLCPeakdb, tranRLC3dbA, tranRLC3dbB, comboR, comboL, comboC, comboQu, comboQL, comboSerR
        end if
    end if
    tranRLCForm$="3,3,4//UseMultiplier"
    if tranRLCSeriesRLC$="set" then f$="Fs=" else f$="Fp="
    if abs(comboR)<1 then comboR$=using("#.###", comboR) else comboR$=uFormatted$(comboR, tranRLCForm$);"ohms"
    comboL$=uFormatted$(comboL, tranRLCForm$);"H"
    comboC$=uFormatted$(comboC, tranRLCForm$);"F"
    if comboQu<1 then comboQu$=using("#.###", comboQu) else comboQu$=uFormatted$(comboQu, tranRLCForm$)
    if comboQL<1 then comboQL$=using("#.###", comboQL) else comboQL$=uFormatted$(comboQL, tranRLCForm$)
    if tranRLCSeriesRLC$="set" then
        serR$=""    //For series RLC, comboR is already series R
    else
        if comboSerR<1 then serR$=using("#.###", comboSerR) else serR$=uFormatted$(comboSerR, tranRLCForm$) //ver115-4b
        serR$="; (Rs="; serR$; ")"  //Enclose in parens
    end if
    #tranRLC.results, f$; tranRLCFs;" MHz; R=";comboR$;"; L="; comboL$;"; C="; comboC$; _
                            "; Qu="; comboQu$; "; QL="; comboQL$; serR$
    wait
*/
}
void dialogTranRLCAnalysis::tranRLCCancel()
{/*//We always leave TranRLCAnalysis through here ver115-3f
    close #tranRLC
    exit sub

end sub //end of TranRLCAnalysis
*/
}
void dialogTranRLCAnalysis::tranRLCExplain(QString h)
{/*//Help window for transmission mode RLC analysis
    WindowWidth = 550
    WindowHeight = 325
    call GetDialogPlacement //determine position on screen   ver115-4b
    BackgroundColor$="gray"
    ForegroundColor$="black"

    s$="RLC analysis will determine the R, L and C values for resistor, inductor and capacitor combinations."
    s$=s$;" The components may be in series or in parallel, and either way they may be mounted in a series or"
    s$=s$;" shunt fixture. The values of Q will also be determined."
    statictext #tranRLCExplain, s$, 10, 10, 515, 60

    s$="For the shunt fixture, you may enter the time delay of the connection between the actual fixture and"
    s$=s$;" the components; typically on the order of 0.125 ns per inch."
    statictext #tranRLCExplain, s$, 10, 75, 515, 40

    s$="You must enter the RLC Analysis function with a Transmission scan already existing, showing the resonance"
    s$=s$;" peak (for series RLC in series fixture, or parallel RLC in parallel fixture) or notch (for series RLC in"
    s$=s$;" parallel fixture or parallel RLC in series fixture). For resonance peaks, you should normally include the 3 dB"
    s$=s$;" points (3 dB below a peak, or 3 dB above a dip). It is permissible,"    //116-4g
    s$=s$;" however, to exclude one of those points. For resonant notches, you may analyze the scan be using either"
    s$=s$;" the absolute -3 dB points (most suitable for narrow notches) or the points 3 dB above the notch bottom"
    s$=s$;" (most suitable for notches over 20 dB deep)."       //ver116-4h
    statictext #tranRLCExplain, s$, 10, 125, 515,  130

    open "RLC Analysis Help" for dialog_modal as #tranRLCExplain
    print #tranRLCExplain, "font ms_sans_serif 10"
    print #tranRLCExplain, "trapclose void dialogTranRLCAnalysis::tranRLCExplainEnd()"

    wait
*/
}
void dialogTranRLCAnalysis::tranRLCExplainEnd()
{/*
    close #tranRLCExplain
    exit sub

end sub //end tranRLCExplain
*/
}
