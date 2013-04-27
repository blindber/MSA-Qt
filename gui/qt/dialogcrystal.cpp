#include "dialogcrystal.h"
#include "ui_dialogcrystal.h"

dialogCrystal::dialogCrystal(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogCrystal)
{
  ui->setupUi(this);
}

dialogCrystal::~dialogCrystal()
{
  delete ui;
}
void dialogCrystal::menuCrystalAnalysis()
{/*//Determine Crystal Parameters
    //We are in SNA or VNA mode. The user must have set up a scan that shows the series and parallel
    //resonances.
    if haltsweep=1 then gosub void MainWindow::FinishSweeping()  //Halt
    if crystalWindHndl$<>"" then close #crystalWindHndl$    //ver115-5f
    if (primaryAxis=1 and Y1DataType<>constMagDB) or (primaryAxis=2 and Y2DataType<>constMagDB) then _
                 notice "Primary axis must contain a graph of magnitude (dB)." : wait   //ver115-3b
    WindowWidth = 550
    WindowHeight = 350
    call GetDialogPlacement
    BackgroundColor$="gray"
    ForegroundColor$="black"
    TextboxColor$ = "white"

    statictext #crystal.title, "DETERMINATION OF CRYSTAL PARAMETERS",130, 10, 375, 20
    s$="There must be an existing S21 scan of the crystal in a Series Fixture. Enter the fixture R0."
    s$=s$; " Select the type of scan. If desired, click Zoom to Fs to improve the scan resolution."
    statictext #crystal.intro, s$, 20, 30, 515, 40
    crystRes=(endfreq-startfreq)/globalSteps*1000000    //Step size in Hz
    s$="The current step size is ";using("#####.#",crystRes);" Hz/step."
    statictext #crystal.step, s$, 20, 70, 515, 20   //Will have step size info

    crystCheckTop=95
    s$="The current scan extends from below the series resonance peak to above the parallel resonance dip."
    checkbox #crystal.fullSweep, "", void MainWindow::crystFull(), void MainWindow::crystFull(), 10, crystCheckTop, 20,20 //ver115-3f
    statictext #crystal.inst, s$, 30, crystCheckTop+3, 500, 18 //ver115-4a
    s$="The scan includes the series resonance peak only; the parallel resonant frequency Fp is stated below."
    checkbox #crystal.seriesSweep, "", void MainWindow::crystSeries(), void MainWindow::crystSeries(), 10, crystCheckTop+25, 20,20  //ver115-3f
    statictext #crystal.inst2, s$, 30, crystCheckTop+28, 500, 20
    textbox #crystal.Fp, 150, crystCheckTop+50, 75, 20   //ver115-3f
    statictext #crystal.FpLabel,"MHz", 230, crystCheckTop+53, 50, 20

            //R0
    textbox #crystal.R0, 125, crystCheckTop+95, 50, 20
    statictext #crystal, "Fixture R0 (ohms)", 35, crystCheckTop+97, 88, 20

        //Buttons
    button #crystal.zoom, "Zoom To Fs", void MainWindow::crystZoom(), UL,225, crystCheckTop+90, 80,30 //ver115-5c
    crystButTop=crystCheckTop+155
    button #crystal.analyze, "Analyze", void MainWindow::crystalAnalyze(), UL,20, crystButTop, 80,30
    button #crystal.rescan, "Rescan", void MainWindow::crystalRescan(), UL,100, crystButTop, 80,30   //ver115-5g
    button #crystal.add, "Add To List", void MainWindow::crystalList(), UL,200, crystButTop, 80,30
    button #crystal.set, "Set ID Num", void MainWindow::crystalSetID(), UL,280, crystButTop, 80,30
    button #crystal.Help, "Help", CrystalExplain, UL,380, crystButTop, 80,30

    textbox #crystal.results, 10, crystButTop+40, 510, 20      //textbox for results
        //Open dialog
    open "Crystal Analysis" for dialog_modal as #crystal    //ver115-5g
    crystalWindHndl$="#crystal" //ver115-5f
    #crystal, "trapclose void MainWindow::crystalCancel()" //ver115-3b
    #crystal.intro, "!font ms_sans_serif 10"
    #crystal.step, "!font ms_sans_serif 10"

    #crystal.add, "!disable"    //disable until we have done an analysis ver115-4a
    call mClearMarkers  //Extra Markers just cause visual confusion ver115-3b
    #crystal.analyze, "!setfocus" //to keep window in front
    #crystal.R0, S21JigR0  //ver115-3f
    lastComputedDelay=0
    crystRedidScan=0
    crystError=0 : crystAnalyzeDone=0
    crystalScanning=0   //Set to 1 during Zoom scan or Rescan
    goto void MainWindow::crystFull()
*/
  exec();
}
void dialogCrystal::crystFull()
{/*
    #crystal.fullSweep, "set"
    #crystal.seriesSweep, "reset"
    #crystal.Fp, "!hide"
    #crystal.FpLabel "!hide"
    wait
*/
}
void dialogCrystal::crystSeries()
{/*
    #crystal.fullSweep, "reset"
    #crystal.seriesSweep, "set"
    #crystal.Fp, "!show"
    #crystal.FpLabel "!show"
    wait
*/
}
void dialogCrystal::crystalList()
{/*//Add current results to list
    if crystalListHndl$="" then //need to create window
        UpperLeftX=0 : UpperLeftY=400  //0 for X centers it on #crystal window
        WindowWidth=450
        open "Crystal List" for text as #crystalList
        #crystalList, "!trapclose CrystalListClosed"
        crystalListHndl$="#crystalList"
        print #crystalList,"!font Arial 10"  //ver115-1h
        print #crystalList," ID    Fs(MHz)       Fp(MHz)  Rm(ohms)   Lm(mH)      Cm(fF)     Cp(pF)"  //Print heading info for list ver115-9c
    end if
    crystalLastUsedID=crystalLastUsedID+1   //increment ID
    crystFreqInfo$=using("####.######",crystFs);"  "; using("####.######",crystFp)
    crystParamInfo$= using("###.##", crystRm);"  ";using("###.######", crystLm);"  ";using("###.######", crystCm);"  ";using("###.##", crystCp);"  "
    crystalID$=str$(crystalLastUsedID)
    print #crystalList, space$(4-len(crystalID$));crystalID$;"  "; crystFreqInfo$; "    ";crystParamInfo$    //Add results to list
    wait
*/
}
void dialogCrystal::crystalSetID()
{/*
    prompt "Enter numeric ID for this crystal"; crystalID$
    if crystalID$<>"" then crystalLastUsedID=val(crystalID$)-1    //Enter new value of last used ID if prompt was not cancelled
    wait
*/
}
void dialogCrystal::crystZoom()
{/*//button--zoom in to show Fs and the -3 dB points
    if crystalScanning then goto void MainWindow::crystalCancel()    //Zoom becomes Cancel during Zoom scan
    gosub void MainWindow::crystalFindPoints()   //Find Fs and -3 db points
    if crystError then goto void MainWindow::crystalCancel()
    call mDeleteMarker "P-"
    crystRes=(endfreq-startfreq)/globalSteps    //current MHz/step
        //If coarse scan, shoot for 12 Hz/step, otherwise for 5 Hz/step
    if crystRes>0.000150 then crystTargetRes=0.000012 else crystTargetRes=0.000005  //ver115-9c
    if crystRes<=crystTargetRes then wait   //don't zoom any further ver115-9c

    if crystFullSweep$="set" then   //set or reset in crystalFindPoints
        //If we had a full sweep, indicate that it now has just the series peak, and enter
        //the Fp value
        #crystal.fullSweep, "reset"
        #crystal.seriesSweep, "set"
        #crystal.Fp, "!show"
        #crystal.FpLabel "!show"
        #crystal.Fp, using("####.######", crystFp)
    end if

    doLRRelativeTo$="P+"    //To locate L and R relative to P+
    db3Range=cryst3dbB-cryst3dbA
    crystMargin=max(2*crystRes, db3Range/20) //Allow extra range in case 3 dB points change with the higher resolution  ver115-9c
    crystZoomRange=db3Range+2*crystMargin
    crystNewStartFreq=cryst3dbA-crystMargin
    crystNewEndFreq=crystNewStartFreq+crystZoomRange
        //Note globalSteps will change in FunctChangeAndSaveSweepParams
    steps=int(1+crystZoomRange/(2*crystTargetRes))*2    //To get step size near crystTargetRes, and have even number of steps
    if steps<50 then steps=50
    if steps>500 then steps=500 //ver115-9c
    crystSaveExisting=(crystRedidScan=0) //save existing settings and transfer band cal to base cal only first time we zoom
    crystBandToBase=(crystRedidScan=0 and applyCalLevel=2)  //Make band cal into a base cal, but only first time and only if previous scan used band cal.
        //Rescan Linear between -3 dB points; 10% excess on each end
    call FunctChangeAndSaveSweepParams  crystSaveExisting, crystBandToBase, steps, crystNewStartFreq, crystNewEndFreq, 1
        //Disable buttons so they aren//t clicked while we scan
    #crystal.analyze, "!disable"
    #crystal.zoom, "Cancel"     //Zoom becomes Cancel during Zoom scan
    #crystal.set, "!disable" : #crystal.Help, "!disable"
    #crystal.add, "!disable" : #crystal.rescan, "!disable"
    crystalScanning=1
    specialOneSweep=1   //So we return from void MainWindow::Restart()
    crystRedidScan=1
    gosub void MainWindow::Restart()     //Scan to get Fs and -3 dB points more precisely
    crystalScanning=0
    if refreshEachScan=0 then call RefreshGraph 0 //To place the P+, L and R markers properly
        //Re-enable buttons
    #crystal.analyze, "!enable"
    #crystal.zoom, "Zoom to Fs"
    #crystal.set, "!enable" : #crystal.Help, "!enable" : #crystal.rescan, "!enable"
    if crystAnalyzeDone then #crystal.add, "!enable"    //Enable only if we have done an analysis
    doLRRelativeTo$=""  //To cancel automatic LR placement
    crystRes=(endfreq-startfreq)/globalSteps*1000000    //Step size in Hz
    #crystal.step,"The current step size is ";using("#####.#",crystRes);" Hz/step."
    wait
*/
}
void dialogCrystal::crystalFindPoints()
{/*//Find Fs, -3 dB points, and Fp if applicable
    S21JigAttach$="Series"
    #crystal.R0, "!contents? componR0$"   //R0  //ver115-3f
    S21JigR0=val(componR0$)  //ver115-3f
    if S21JigR0<0 then S21JigR0=50 : #crystal.R0, "50" :notice "Invalid R0. 50 ohms used."   //ver115-3f
    analysisAxis$=str$(primaryAxisNum)    //ver115-3b
    #crystal.fullSweep, "value? crystFullSweep$"    //see if we have full or partial sweep ver115-4a

    //We analyze a positive peak if doing crystals
    call mAddMarker "P+", 1, analysisAxis$   //place P+ marker on primary trace ver115-4a
    if crystFullSweep$="set" then call mAddMarker "P-", 1, analysisAxis$   //place P- marker on primary trace ver115-4a
    doLRRelativeTo$="P+"    //Indicate to place L and R at -3db points around P+
    doLRRelativeAmount=-3
    doLRAbsolute=0  //ver115-4a

    call RefreshGraph 0 //To place the peak markers properly
    doLRRelativeTo$=""  //disable auto placement now that we have done it
    doLRAbsolute=0      //User never does absolute values //ver115-3f
    //We now have markers at all the critical points. We just have to get the marker info.
    //The main resonance is a peak if we have a crystal or a series RLC in a series fixture, or parallel RLC in parallel fixture.
    crystPeakOrdinal=mMarkerNum("P+")   //Ordinal of P+ point; graph module finds info from ordinal

    crystFs=gMarkerCurrXVal(crystPeakOrdinal)   //We now have the exact Fs

        //we need the parallel resonance frequency also ver115-4a
    //For crystal we may also need to zoom in more closely around the series peak to get a good read on Fs.
    if crystFullSweep$="reset" then //crystFullSweep$ was set by void MainWindow::crystFindPoints()
        #crystal.Fp, "!contents? crystFp$"
        crystFp=val(crystFp$)
        if crystFs>=crystFp then _
        notice "Parallel resonant frequency must exceed series resonant frequency." : crystError=1
    else    //We need to find Fp ourselves
        crystFp=gMarkerCurrXVal(mMarkerNum("P-"))
        if crystFp>endfreq-0.00005 then _
                notice "Sweep does not include enough of parallel resonance." : crystError=1 //ver115-4a
        if crystFs>=crystFp then _
                notice "Sweep does not show proper series resonance followed by parallel resonance." : crystError=1 //ver115-4a
    end if

    crystPeakPoint=gMarkerPointNum(crystPeakOrdinal)    //Point number of the main peak
    crystPeakdb=gGetPointYVal(crystPeakPoint, primaryAxisNum)    //Get S21 db value (primary trace)
    crystLNum=mMarkerNum("L")
    crystRNum=mMarkerNum("R")
    cryst3dbA=gMarkerCurrXVal(crystLNum)
    cryst3dbB=gMarkerCurrXVal(crystRNum)
    if gMarkerPointNum(crystLNum)<=0 or gMarkerPointNum(crystRNum)<=0 then _
        notice "Sweep does not contain necessary -3 dB points." : crystError=1 //ver115-5c
return
*/
}
void dialogCrystal::crystalRescan()
{/*//Rescan per original sweep params, presumably for a new crystal. ver115-5g
    if crystalScanning then goto void MainWindow::crystalCancel()    //Rescan becomes Cancel during Rescan
    if crystRedidScan then
        call mClearMarkers  //because we are zapping the data anyway
        gosub void MainWindow::FunctRestoreSweepParams()
        suppressHardwareInitOnRestart=1 //no need for hardware init ver116-4d
        gosub void MainWindow::PartialRestart()
    end if
    crystRedidScan=0

    #crystal.fullSweep, "disable"
    #crystal.seriesSweep, "disable"
    #crystal.zoom, "!disable"
    #crystal.analyze, "!disable"
    #crystal.rescan, "Cancel"   //Rescan becomes Cancel
    #crystal.add, "!disable"
    #crystal.set, "!disable"
    #crystal.Help, "!disable"
    #crystal.results,""
    specialOneSweep=1
    crystalScanning=1
    gosub void MainWindow::Restart()     //Scan once and return
    crystalScanning=0
    #crystal.fullSweep, "enable"
    #crystal.seriesSweep, "enable"
    #crystal.zoom, "!enable"
    #crystal.analyze, "!enable"
    #crystal.rescan, "Rescan"
    #crystal.add, "!enable"
    #crystal.set, "!enable"
    #crystal.Help, "!enable"
        //The fullsweep box could have been reset by Zoom. We always set it when the window opens,
        //so we set it here to start fresh. Rescan is unlikely to be used if the original scan did
        //not include Fp, since the user then has to figure out the Fp for the new crystal being scanned.
    goto void MainWindow::crystFull()
*/
}
void dialogCrystal::crystalAnalyze()
{/*//Analyze button was pushed
    gosub void MainWindow::crystalFindPoints()
    if crystError then goto void MainWindow::crystalCancel()

    if msaMode$="VectorTrans" or msaMode$="Reflection" then //ver115-5c
        //Refine the value of Fs by finding the point with zero phase or reactance. Reactance is best for reflection
        //mode. The main advantage of this approach is that transmission phase and reflection reactance are linear near
        //series resonance, so interpolation can find Fs precisely even with a coarse scan.
        //Note: At the moment, we don't do crystal analysis in reflection mode.
        if msaMode$="VectorTrans" then crystZeroType=constSerAngle else crystZeroType=consSerReact
        crystZero=StepWithValue(crystZeroType, max(crystPeakOrdinal-4,0), 1, 0) //Search forward for zero, starting just left of peak.
            //Use the frequency of the point with zero value, if that point is close to the peak point. If not,
            //something went wrong.
        if abs(crystPeakOrdinal-crystZero)<=1 then
            crystFs=gGetPointXVal(crystZero)
            call mDeleteMarker "P+"
            call mAddMarker "1", crystZero, str$(primaryAxisNum)
            call RefreshGraph 0 //Redraw to show markers.
        end if
    end if
    call uCrystalParameters crystFs, crystFp, crystPeakdb, cryst3dbA, cryst3dbB, crystRm, crystCm, crystLm, crystCp, crystQu, crystQL
    crystCm=crystCm*1000    //pF to fF ver115-9c
    crystLm=crystLm/1000    //uH to mH ver115-9c
    crystForm$="3,3,5//UseMultiplier"   //for out-of-bounds values

    if crystRm<100 then crystRm$=using("##.##", crystRm);" ohms" else crystRm$=uFormatted$(crystRm, crystForm$);"ohms"
    if crystLm<999 then crystLm$=using("###.######", crystLm);" mH" else crystLm$=uFormatted$(crystLm/1000, crystForm$);"H"
    if crystCm<999 then crystCm$=using("###.######", crystCm);" fF" else crystCm$=uFormatted$(crystCm*1e-15, crystForm$);"F"
    if crystCp<9.9 then crystCp$=using("#.###", crystCp);" pF" else crystCp$=uFormatted$(crystCp*1e-12, crystForm$);"F"
    #crystal.results, "Fs="; crystFs;" MHz; Fp="; crystFp;" MHz; Rm=";crystRm$;"; Cm="; crystCm$;"; Lm="; _
                        crystLm$; "; Cp=";crystCp$
    #crystal.add, "!enable"    //enable now that we have data ver115-4a
    wait
*/
}
void dialogCrystal::crystalCancel()
{/*//We always leave void MainWindow::menuCrystalAnalysis() through here ver115-3f
    specialOneSweep=0
    if crystalScanning then crystalScanning=0 : gosub void MainWindow::FinishSweeping()   //ver115-5g
    if crystRedidScan then
        call mClearMarkers  //because we are zapping the data anyway
        gosub void MainWindow::FunctRestoreSweepParams()
        suppressHardwareInitOnRestart=1 //no need for hardware init ver116-4d
        gosub void MainWindow::PartialRestart()
    end if
    close #crystal : crystalWindHndl$=""
    wait
//end of void MainWindow::menuCrystalAnalysis()
*/
}
void dialogCrystal::CrystalListClosed(QString h)
{/*//Crystal list window has been closed. This is sub so it can be called from within other subs
    close #crystalList
    crystalListHndl$=""
end sub
*/
}
void dialogCrystal::CrystalExplain(QString h)
{/*
    WindowWidth = 550
    WindowHeight = 285
    UpperLeftX=int((DisplayWidth-WindowWidth)/2)
    UpperLeftY=int((DisplayHeight-WindowHeight)/2)
    BackgroundColor$="gray"
    ForegroundColor$="black"

    s$="Crystal analysis will determine the motional parameters (Rm, Cm and Lm) for a crystal."
    s$=s$;" It will also determine the parallel capacitance from lead to lead (Cp), and the series and"
    s$=s$;" parallel resonant frequencies."
    statictext #crystExplain, s$,  10,   10, 515,  60

    s$="The crystal must be mounted in a series fixture, and you must specify the R0 of the fixture. A"
    s$=s$;" regular 50-ohm fixture is fine, but the standard for crystal analysis is 12.5 ohms."
    statictext #crystExplain, s$,  10,  65, 515,  40

    s$="You must enter the Crystal Analysis function with a Transmission scan already existing, including the series resonance"
    s$=s$;" peak and the -3 dB points around it. You may also include the parallel resonance dip, or you may elect to"
    s$=s$;" explicitly specify the parallel resonant frequency, which is needed to determine Cp."   //ver115-5c
    statictext #crystExplain, s$,  10,  100, 515,  75

    if msaMode$="VectorTrans" or msaMode$="Reflection" then
        s$="Fs is the parameter needing the most precision, and it will be located by interpolation to find zero"
        s$=s$;" phase, so a step size of 100 Hz or less likely provides sufficient accuracy."   //ver115-5c
    else
        s$="A small scan step size is important to locating Fs accurately so you likely need a step size"
        s$=s$;" in the range 5-50 Hz. "   //ver115-5c
    end if
    s$=s$;" You can reduce the step size by using the Zoom to Fs button, which will rescan the area around Fs."   //ver115-5c
    statictext #crystExplain, s$,  10,  180, 515,  85

    open "Crystal Analysis Help" for dialog_modal as #crystExplain
    print #crystExplain, "font ms_sans_serif 10"
    print #crystExplain, "trapclose void MainWindow::CrystalExplainEnd()"

    wait
*/
}
void dialogCrystal::CrystalExplainEnd()
    {/*
    close #crystExplain //ver115-5c
    exit sub

end sub //end CrystalExplain
*/
}
