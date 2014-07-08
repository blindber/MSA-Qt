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
#include "dialogcalmanager.h"
#include "ui_dialogcalmanager.h"
//SEW Added the following Calibration Module to load magnitude and frequency calibration from files
//
//===================================================================
//@==============Calibration Manager Module==========================
//@ Version 1.03
//This module displays a window for viewing and editing files for calibration-over-signal-level
//("mag cal") and calibration-over-frequency ("freq cal"). The former records actual power levels
//corresponding to given ADC readings. For VNA use, it also includes errors in phase
//measurement over frequency.
//
//To perform calibration call calManRunManager. While it is running, calManWindHndl$ will be non-blank.
//To check whether a freq and mag path 1 file exist, use "calFileExists()".
//Before calling anything other than calManRunManager, call InitFirstUse maxMagPoint, maxFreqPoints
//to initialize and to set the max number of arrays. Call calCloseWindows if the calling program quits when
//there may still be a cal manager window open.
//
//To apply the calibration tables to a measurement, use calConvertMagData or calConvertFreqError.

dialogCalManager::dialogCalManager(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogCalManager)
{
  ui->setupUi(this);

  connect(this, SIGNAL(RequireRestart()), parent, SLOT(RequireRestart()));
  connect(this, SIGNAL(PartialRestart()), parent, SLOT(PartialRestart()));

  calMagTable.mresize(802,3);
  calFreqTable.mresize(802,2);
  calMagCoeffTable.mresize(802,8);
  calFreqCoeffTable.mresize(802,4);

  calFileVersion = 0;
  calFreqPoints = 0;
  calfigModuleVersion = 0;

  QFont font("Monospace");
  font.setStyleHint(QFont::TypeWriter);
  ui->te->setFont(font);
  ui->pathList->setFont(font);
}

dialogCalManager::~dialogCalManager()
{
  delete ui;
}

void dialogCalManager::setMsaConfig(msaConfig *config)
{
  activeConfig = config;
}

void dialogCalManager::setGlobalVars(globalVars *newVars)
{
  vars = newVars;
}
void dialogCalManager::setFilePath(QString path)
{
  DefaultDir = path;
}



//This is a gosub subroutine rather than a true subroutine so that it can call [Measure] which
//in turn can call the non-subroutines that currently run the MSA hardware.
void dialogCalManager::calManRunManager(int hasVNA)
{
  calManPrepareEntry();
  calManFileList.clear();
  calManFileList.append("0 (Frequency)");

  ui->data1->setStyleSheet( "background: cyan;" );
  ui->data2->setStyleSheet( "background: cyan;" );
  ui->data3->setStyleSheet( "background: cyan;" );
  //Open window and let user proceed.

  calSetDoPhase(hasVNA);

  calManEnterAvailablePaths();  //Loads list of filter files
  ui->pathList->setCurrentRow(0);
  on_pathList_currentRowChanged(0);
  calEditorPathNum=0;
  calManFileReload();    //Loads frequency file (creates if necessary)
  exec();
}
void dialogCalManager::calManForceOn()
{
  ui->ForceVal->setVisible(true);
  ui->ForceVal->setText("0");
}
void dialogCalManager::calManForceOff()
{  
  ui->ForceVal->setVisible(false);
}
void dialogCalManager::calManSetPhase()
{
  /*
  calSetDoPhase(1);
  if (calEditorPathNum!=0)
  {
    calManClean("");
    calManPrepareEntry();
  }*/
}
void dialogCalManager::calManResetPhase()
{/*
  calSetDoPhase(0);
  if (calEditorPathNum!=0)
  {
    calManClean("");
    calManPrepareEntry();
  }*/
}
void dialogCalManager::calManBtnReturn()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  int doCan=calManWarnToSave(1); //Warn to save if data changed. Allow cancel.
     if (doCan==1)
     {
       return;  //cancelled  Exit calManRunManager
     }
     calInstallFile(0);   //Leave with freq file installed
     calCloseWindows();
     haltsweep=0;
     RequireRestart();
     */
}
void dialogCalManager::calManFinished()
{
  calManBtnReturn();
}
void dialogCalManager::calCloseWindows()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Close any windows that are open
    if calManWindHndl$<>"" then close #calman : calManWindHndl$=""
end sub
*/
}
void dialogCalManager::calManEnterAll()
{/*//button to Enter all sweep points for freq cal
        for i=0 to steps  //ver113-7e
        call calManGetFreqInput i   //Put data into boxes
        if i=steps then call calManEnter "xx" else call calManEnter "" //Enter data into table; clean up on last one
        if calManEnterError then exit for //ver114-3d Stop if error
    next i
    wait
*/
}
void dialogCalManager::calManEnter(QString btn)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Handle Enter button
    //If called by button click, btn$ will not be blank and we do clean up at the end.
    //If btn$ is blank, that is a signal to skip the clean up because we are entering
    //a mass of points.
    calManEnterError=0
    #calman.data1, "!contents? s1$" : #calman.data2, "!contents? s2$" : #calman.data3, "!contents? s3$"
    doForce$="reset"    //so it is reset for freq cal ver116-4j
    if calEditorPathNum=0 then
        //If doing freq cal, we need to update the entry for
        //freq and measured power, because user may have changed the scan. Note that this
        //means the user cannot manually enter data into the freq and power boxes, which are
        //are now disabled.
        //ver114-3d changed to update the boxes
        #calman.data1, "!contents? measPoint$"  //get point number
        measPoint=val(measPoint$)
        call calManGetFreqInput measPoint   //Get point data into boxes and wait
        #calman.data2, "!contents? s2$" : #calman.data3, "!contents? s3$"

        //Frequency cal. data2 is freq, data3 is power;
        //if first point, need to put freq in ref
        freq=val(s2$)
        if freq<0 or freq>10000 then notice "Invalid frequency." : calManEnterError=1 :exit sub //ver114-3d
        pow=val(s3$)
        f$=using("####.######",freq)
        p$=using("####.###",pow)  //ver115-1e
        if pow<-200 or pow>100 then notice "Invalid Measured Power." : calManEnterError=1 :exit sub  //ver114-3b; ver114-3d
        if calManEntryIsRef then  //Get ref freq and power first time only
            calManRefFreq=freq
            calManRefPower=pow
            truePower=pow       //ver114-3b
            print #calman.ref, p$
            print #calman.ref2,f$
        else    //after first time, get true power from .ref; ver114-3b created this else... block
            #calman.ref, "!contents? s4$"
            truePower=val(s4$)
            if truePower<-200 or truePower>100 then notice "Invalid True Power value." : calManEnterError=1 : exit sub //ver114-3d ver115-1a
        end if
        //error factor is true power minus measured power; this is added to measured power when freq cal is applied ver 114-3b
        p$=using("####.###",truePower-pow)  //SEW9 reversed the subtraction. ver114-3b used truePower  //ver115-1e
        print #calman.te, f$;" ";p$
    else
        //Mag cal. data 1 is power, data2 is ADC, data3 is phase (if we are doing phase);
        //if first point, need to be sure freq is in ref and put phase in ref2
        pow=val(s1$)
        if pow<-200 or pow>100 then notice "Invalid dBm value." : calManEnterError=1 : exit sub //ver116-1b
        adc=val(s2$)
        if adc<0 then notice "Invalid adc value." : calManEnterError=1 : exit sub //ver114-3d
        if calGetDoPhase()=0 then phase=0 else phase=val(s3$)
        if phase<-180  or phase>180 then notice "Invalid phase value." : calManEnterError=1 : exit sub //ver114-3d
        adc$=using ("#######", adc)
        v$=using ("####.###", pow)
        p$=using ("####.##", phase)
        forceVal=0 : #calman.Force, "value? doForce$"   //ver116-4j added forced phase items
        if doForce$="set" then #calman.ForceVal, "!contents? forceVal$" : forceVal=val(uCompact$(forceVal$))
        if forceVal<-180  or forceVal>180 then notice "Invalid forced phase value. Zero used." : forceVal=0
        if calManEntryIsRef then  //Get freq first time only; after that it is fixed
            #calman.ref, "!contents? freq$"
            freq=val(freq$)
            if freq<=0 or freq>10000 then notice "Invalid frequency." : calManEnterError=1 : exit sub //ver114-3d
            print #calman.ref2, using("####.##", phase)  //Fix reference phase first time only
            calManRefPhase=phase : calManRefFreq=freq : calManRefPower=pow
        end if
            //The power correction factor is phase-calManRefPhase, put into normal range
        if calGetDoPhase()=1 then
            if doForce$="set" then pCor=forceVal else pCor=phase-calManRefPhase    //ver116-4j
            while pCor<=-180 : pCor=pCor+360: wend
            while pCor>180 : pCor=pCor-360: wend
            p$=using ("####.##", pCor)
                //Enter the mag cal data, with phase
            print #calman.te, adc$;"  ";v$;"  ";p$
        else
                //Enter the mag cal data, without phase
            print #calman.te, adc$;"  ";v$
        end if
    end if

        //We have now entered the data, whether mag or freq. Now clean things up.
    if btn$<>"" or calManEntryIsRef then
        fErr$=calLoadFromEditor$("#calman.te", calEditorPathNum) //Load variables from editor
        if fErr$<>"" then
            //Error in existing data, so we won//t mess with formatting or fixing the comments
            notice "Error in data: " + fErr$
            if calManEntryIsRef then call calManEnterInstructions ""
            if doForce$="set" then #calman.Force, "set" : #calman.ForceVal, forceVal$ : #calman.ForceVal, "!show"    //gets hidden by calManEnterInstructions ver116-4j
            calManEntryIsRef=0
            exit sub
        end if
    end if

    if calManEntryIsRef=1 then
        //First point--Enter info into comments
        c2$="Calibrated " + Date$("mm/dd/yy") + " at "
        if calEditorPathNum=0 then
            //Freq
            c1$="Calibration over frequency"
            c2$=c2$ + Trim$(using ("####.###", calManRefPower)) + " dBm."  //ver116-1b
        else
            //Mag
            filtFreq=MSAFilters(calEditorPathNum,0) : filtBW=MSAFilters(calEditorPathNum,1)
            c1$="Filter Path " + str$(calEditorPathNum) + ": CenterFreq=" _
                            + Trim$(using("###.######",filtFreq)) +" MHz; Bandwidth=" _
                            + Trim$(using("####.######",filtBW)) +" kHz"    //ver116-1b
            c2$=c2$ + Trim$(using("####.######", calManRefFreq)) + " MHz."
        end if
        calFileComments$(1)=c1$
        calFileComments$(2)=c2$
        calManEntryIsRef=0
        call calManEnterInstructions ""
        if doForce$="set" then #calman.Force, "set" : #calman.ForceVal, forceVal$ : #calman.ForceVal, "!show"    //gets hidden by calManEnterInstructions ver116-4j
    end if
        //SEW8 made the following conditional
    if btn$<>"" or calManEntryIsRef then
        #calman.te, "!cls"
        call calSaveToEditor "#calman.te", calEditorPathNum   //Restore data, now with comments
    end if

    if calEditorPathNum=0 then  //SEW8 created this if... block
        call calManGetFreqInput calManLastAutoPoint //Display last retrieved point
    else    //Path calibration
        #calman.data1, "" : #calman.data2, "" : #calman.data3, ""   //SEW6
    end if

end sub
*/
}
void dialogCalManager::calFileExists()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Return 1 if both the freq file and path 1 mag file exist
    On Error goto [calNoFile]
    open calFilePath$()+calFileName$(0) for input as #calIn //Frequency cal file
    close #calIn
    open calFilePath$()+calFileName$(1) for input as #calIn //Path 1 mag cal
    close #calIn
    calFileExists=1
    exit function
*/
}
void dialogCalManager::calNoFile()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Error means it isn//t there
    calFileExists=0
end function
*/
}
void dialogCalManager::calManBtnNextFreqPoint()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Handles button to retrieve next point in automated freq cal.
    #calman.data1, "!contents? measPoint$"  //get point number
    measPoint=val(measPoint$)
    if measPoint<0 or measPoint>steps-1 then notice "Invalid point number" : wait
    measPoint=measPoint+1   //Next point
    #calman.data1, str$(measPoint)  //Display point number
    call calManGetFreqInput measPoint   //Get point data into boxes and wait
    wait
*/
}
 //SEW8 added calManBtnPrevFreqPoint
void dialogCalManager::calManBtnPrevFreqPoint()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Handles button to retrieve previous point in automated freq cal.
    #calman.data1, "!contents? measPoint$"  //get point number
    measPoint=val(measPoint$)
    if measPoint<1 or measPoint>steps then notice "Invalid point number" : wait
    measPoint=measPoint-1   //Next point
    #calman.data1, str$(measPoint)  //Display point number
    call calManGetFreqInput measPoint   //Get point data into boxes and wait
    wait
*/
}
void dialogCalManager::calManGetFreqInput(int measStep)
{/*//Enter user scanned data for point measStep (0...steps)
        //The user has performed a scan covering the desired range with the desired number of points.
        //We enter the points from the MSA arrays into the calibration table.
        //This is valid if the Path calibration for the currently installed filter is current.
        //We retrieve freq and power (dbm) for each point and put it into calman.data2 (freq)
        //and calman.data3 (power). We put point number into calman.data1.
    calManLastAutoPoint=measStep       //SEW8 Record number of point of the user-established sweep
    #calman.data1, using("####", measStep)  //Enter step number into box
    #calman.data2, using("####.######", datatable(measStep,1))    //Enter frequency into box
    #calman.data3, using("####.###", datatable(measStep, 2))     //Enter power into box  //ver115-1e
end sub
*/
}
void dialogCalManager::calManMenuMeasure()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//enters here when the "Measure" button is clicked. ver113-7d
    gosub [calManMeasure]
    wait
*/
}
void dialogCalManager::calManMeasure()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*        //Measure a point for calibration over signal strength (mag cal)
        //data 1 is power, data2 is ADC, data3 is phase (if we are doing phase);
        //freq is in ref. We measure power and phase at this frequency and enter into boxes.
        //It is assumed that the user has set up the scan at the desired frequency.Scotty-yes, in zero sweep width
    #calman.ref, centfreq  //ver113-7d
    if centfreq<0 or centfreq>1000 then notice "Invalid frequency." : return  //ver113-7d
    if centfreq=0 then notice Chr$(13);"Center Frequency of 0 is not allowed.";Chr$(13);"Return to MSA and Change Center Frequency" : return  //ver113-7d
    if sweepwidth>0 then notice Chr$(13);"Sweep Width is not 0.";Chr$(13);"Return to MSA and Change Sweep Width to 0" : return  //ver113-7d
        //We have to average 30 readings.  For mag, we just average ADC, since we are
        //just now determining the calibration factors needed to convert ADC to power.
        //For phase, if needed, we convert from ADC bits to phase and average that.
        //Averaging phase is a little tricky.
        //A -179 degree angle and a 179 degree angle should average to 180, not 0 degrees.
        //We expect only a few degrees variation in phase. We save the first phase reading and
        //average the difference (this reading-first reading). If a difference exceeds 180 degrees,
        //we decrease it by 360 because we know we must take the difference in the opposite
        //direction. E.g. 179 minus -179 gives a difference of 358; we subtract 360 to get the true difference.
        //of -2 degrees. Likewise, if the difference is less than -180 degrees, we add 360.
    measADCSum=0 : measPhaseDifSum=0
            //Average the mag ADC readings and the phase degrees.
    saveMode$=msaMode$ //Save MSA current setting
    saveThreshold=validPhaseThreshold :validPhaseThreshold=0    //So PDM inversions will be done even at low levels ver116-1b
    if calGetDoPhase()=1 then msaMode$="VectorTrans" else msaMode$="SA"  //Include phase if possible //ver114-5n
    suppressPDMInversion=0  //ver115-1a
    suppressPhase=0 //If user has suppressed phase, undo it ver116-4j
    #calman.data2, "" : #calman.data3, ""   //Blank out ADC and phase so user sees Measure is in progress ver115-1a
    //Wait time is accomplished primarily by the time between the user setting the input level and
    //our first read, but just in case we delay the first read. We set the wait time based on the time constants.
    //Note that once we take the first read, there are no more glitches from changing PLLs, etc. since freq is fixed.
    saveWate=wate : saveUseAutoWait=useAutoWait   //ver116-4j
    useAutoWait=0 : if calGetDoPhase() then wate=max(videoMagTC, videoPhaseTC) else wate=videoMagTC //ver116-4j
    if wate<150 then wate=150 //ver116-4j
    thisstep=0 : gosub [CommandCurrentStep] //ver116-4j  to be sure we are set to proper freq, etc.; assumes [PartialRestart] has been done
    glitchhlt = 8*wate  //add extra settling time before first read. [ReadStep] uses this.
    for measStep=0 to 29  //ver116-4j
            //Read phase and mag.
        gosub [ReadStep]    //Read phase and mag
            //magdata now has ADC bits for magnitude; if MSA can measure phase phadata has ADC bits for phase.
        measADCSum=measADCSum+magdata
        if calGetDoPhase()=1 then
                //phaarray(thisstep,0) is 1 if inverted otherwise 0; phadata is phase ADC reading
            measPhase=360*phadata/maxpdmout-invdeg*phaarray(thisstep,0) //ver115-1a
            if measStep=0 then measFirstPhase=measPhase //Save first phase reading
            measPhaseDif=measPhase-measFirstPhase   //phase dif
            if (phadata < pdmlowlim or phadata > pdmhighlim) then   //Phase in crap zone even with inversion ver114-7k
                suppressPDMInversion=1  //don't waste time inverting for remaining points
            end if
            if measPhaseDif<-180 then measPhaseDif=measPhaseDif+360
            if measPhaseDif>180 then measPhaseDif=measPhaseDif-360
            measPhaseDifSum=measPhaseDifSum+measPhaseDif    //Accumulate phase difference
        end if
    next measStep
    #calman.data2, using("######", measADCSum/30)   //Enter averaged ADC into box ver116-4j
    if calGetDoPhase()=1 then   //this block rewritten by ver115-1a
        if suppressPDMInversion then
            measPhase=calManRefPhase+180 //Choose value so phase correction comes out to +/-180
            if measPhase>180 then measPhase=measPhase-360
        else
            measPhase=measFirstPhase + measPhaseDifSum/30   //First phase measurement plus average difference ver116-4j
            while measPhase<=-180 : measPhase=measPhase+360 : wend  //Put into range -180 to 180
            while measPhase>180 : measPhase=measPhase-360 : wend
        end if
    else
        measPhase=0
    end if
    suppressPDMInversion=0  //ver115-1a
    #calman.data3, using("####.##", measPhase)     //Enter averaged phase into box
    msaMode$=saveMode$ //Restore settings
    wate=saveWate : useAutoWait=saveUseAutoWait   //ver116-4j
    validPhaseThreshold=saveThreshold   //ver116-1b
return
*/
}

void dialogCalManager::calManClean()
{
  /*
  'Format and Sort list
      'We just get the data and put it back
      fErr$=calLoadFromEditor$("#calman.te", calEditorPathNum) 'This also sorts
      if fErr$<>"" then notice "Bad Data. "+fErr$ : exit sub
      if calEditorPathNum=0 then nPoints=calNumFreqPoints() else nPoints=calNumMagPoints()
      #calman.te, "!cls"
      call calSaveToEditor  "#calman.te", calEditorPathNum
      if nPoints=0 then
          'If there are no data points, start over with a clean header
          call calCreateDefaults calEditorPathNum,"#calman.te", 0 'zero signals no points
      end if
          */
}

void dialogCalManager::calManDisplayDefault(QString btn)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
   if calManWarnToSave(1)=1 then exit sub
   call calCreateDefaults calEditorPathNum,"#calman.te", 1 //1 signals to do points
   call calManClean ""
   call calManPrepareEntry
end sub
*/
}
void dialogCalManager::calManEnterAvailablePaths()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  //Create list of available paths
    //The list is based on the MSA info in MSAFilters(), which lists
    //freq and bw for each calibration path, starting at path 1=index 1
    //We create a list where index 0 is the frequency cal file, and index N (1...)
    //is the Nth mag cal path.

  calManFileList.clear();
  calManFileList.append("0 (Frequency)");

  QString bwPad;
  QString bwStr;
  QString thisFilt;
  QString freq;
  float bw;
  for (int i=1; i <= activeConfig->MSANumFilters; i++)
  {
    freq = QString::number(activeConfig->MSAFilters[i][0]);
    bw = activeConfig->MSAFilters[i][1];

    freq = freq + util.Space(qMax(0,7-freq.length()));   //makes freq a fixed width
    if (bw==0)
      bwPad="   ";
    else
      bwPad = util.Space(5-((int)(log(bw)/log(10.0)))); //Aligns bw at decimal;
    bwStr = bwPad + QString::number(bw);
    thisFilt = freq + bwStr;
    calManFileList.append(QString::number(i) + " (" + thisFilt.trimmed() + ")");   //Path num plus freq and bw
  }
  ui->pathList->blockSignals(true);
  //Clear list
  ui->pathList->clear();
  //Loads new strings into listbox
  ui->pathList->addItems(calManFileList);
  ui->pathList->setCurrentRow(0);
  ui->pathList->blockSignals(false);
}
void dialogCalManager::calManPrepareEntry()
{
//hide all entry boxes, labels and buttons
  ui->Lref->setVisible(false);
  ui->ref->setVisible(false);
  ui->ref2->setVisible(false);
  ui->Lref2->setVisible(false);
  ui->data1->setVisible(false);
  ui->Ldata1->setVisible(false);
  ui->data2->setVisible(false);
  ui->Ldata2->setVisible(false);
  ui->data3->setVisible(false);
  ui->Ldata3->setVisible(false);
  ui->Measure->setVisible(false);
  ui->Enter->setVisible(false);
  ui->NextPoint->setVisible(false);
  ui->PrevPoint->setVisible(false);
  ui->EnterAll->setVisible(false);
  ui->StartEntry->setVisible(true);
  ui->Force->setVisible(false);
  ui->ForceVal->setVisible(false);
  ui->enterInstruct->clear();
  //ui->enterInstruct, "place 10 15"
  ui->enterInstruct->insertPlainText("To begin entry of calibration data, click Start Entry.\n");
  ui->enterInstruct->insertPlainText("Alternatively, you may enter, alter and delete data in the text editor.");
  calManEntryIsRef=1;    //We are set for the first point
}
void dialogCalManager::calManEnterInstructions(QString btn)
{
}
int dialogCalManager::calManWarnToSave(int allowCancel)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  //Give warning to save if data has changed. Return 1 to cancel
  //calManOldText$ contains the old data to compare against. This method is used
  //because the !modified? command cannot be reset at the time of a Save.
  //If allowCancel=1, the message box will allow the user to cancel

  if (teTextChanged == false)
  {
    return 0;
  }

  //Put up a message box.
  QString msg = "Unsaved calibration changes will be lost. Do you want to SAVE first?";
  QString ans=util.uPrompt("Warning", msg, 1, allowCancel);    //yes, no, possibly cancel
  if (ans=="no")
  {
    return 0;    //Proceed, no save
  }
  if (ans=="cancel")
  {
    return 1;
  }
  QString errMsg=calLoadFromEditor(ui->te, calEditorPathNum);   //Get current text editor data ver116-1b
  if (errMsg!="")
  {
    QMessageBox::information(this, "Notice", "Data Error: " + errMsg + "; file not saved.");
    return allowCancel;  //cancel if possible ver116-1b
  }
  calSaveToFile(calEditorPathNum);     //User wants to save so do it
  calInstallFile(calEditorPathNum);     //Re-read to check for errors SEWcal2,ver113-7g
  teTextChanged = false;
  return 0;
}
//@====================Original Calibration Manager Ended Here=====================

//==========================Start of Mag/Freq Calibration Module==========================
//Mag/Freq Calibration module version 1.03
//
//This module is now a subset of the Configuration Manager Module. This portion
//manages calibration files and applies them upon request to raw data.
//calInitFirstUse must be called before any other routines, but that is
//taken care of if calManRunManager has already been run.
//The principal routines of interest to the user are:
//
//function calConvertMagError(magdata) //Returns dbm level given raw ADC reading
//function calConvertFreqError(freq) //Returns correction factor for this freq,
                                    //to be added to raw power to get final

//

//---------------Start Global Variables for Mag/Freq Calibration Module---------
        //calMagTable contains calibration data for the current signal path
        //The zero entry for the first dimension is not used.
        //magCalTable(i, v) gives the ith entry; v=0 gives ADC reading; v=1 gives actual db value.


void dialogCalManager::calInitFirstUse(int maxMagPoints, int maxFreqPoints, int doPhase)
{
  if (maxMagPoints<=800)
    calMaxMagPoints=801;
  else
    calMaxMagPoints=maxMagPoints+1;
  if (maxFreqPoints<=800)
    calMaxFreqPoints=801;
  else
    calMaxFreqPoints=maxFreqPoints+1;
  calFileCommentChar="*";        //Use asterisk as comment char

  calMagTable.mresize(calMaxMagPoints+1,3);
  calFreqTable.mresize(calMaxFreqPoints+1,2);
  calMagCoeffTable.mresize(calMaxMagPoints+1,8);
  calFreqCoeffTable.mresize(calMaxFreqPoints+1,4);

  calMagFileHadPhase=0;
  calDoPhase=doPhase;  //ver114-5q
  calModuleVersion=1.03;         //Change this code to update version

}
int dialogCalManager::calVersion()
{
  //Version of this module
  return calModuleVersion;
}
void dialogCalManager::calSetMaxPoints(int maxMagPoints, int maxFreqPoints)
{
  //Call to change max points; data is lost
  calMaxMagPoints=qMax(801, maxMagPoints+1); //ver114-4b
  calMaxFreqPoints=qMax(801, maxFreqPoints+1); //ver114-4b
  calMagTable.mresize(calMaxMagPoints+1,3);
  calMagCoeffTable.mresize(calMaxMagPoints+1,8);   //SEWcal
  calFreqTable.mresize(calMaxFreqPoints+1,2);
  calFreqCoeffTable.mresize(calMaxFreqPoints+1,4);   //SEWcal

}
void dialogCalManager::calClearMagPoints()
{
  //Clear calMagTable() to zero points
  //It's not necessary to zero the points, but it keeps things clean
  for (int i=0; i < calMaxMagPoints+1; i++)
  {
   calMagTable[i][0]=0;
   calMagTable[i][1]=0;
   calMagTable[i][2]=0;
  }
  calMagPoints=0;
}
void dialogCalManager::calClearFreqPoints()
{
  //Clear calFreqTable() to zero points
  //It's not necessary to zero the points, but it keeps things clean
  for (int i=0; i < calMaxFreqPoints+1; i++)
  {
    calFreqTable[i][0]=0;
    calFreqTable[i][1]=0;
  }
  calFreqPoints=0;
}
int dialogCalManager::calDataHadPhase()
{
  //Returns 1 if last file or editor data had phase correction data
  return calMagFileHadPhase;
}
void dialogCalManager::calSetDoPhase(int doPhase)
{
  //Set calDoPhase to 1 or 0, indicating whether output should include phase
    calDoPhase=doPhase;
}
int dialogCalManager::calGetDoPhase()
{
  //Get calDoPhase:or 0, indicating whether output should include phase
  return calDoPhase;
}
void dialogCalManager::calClearComments()
{
  calFileComments[0]="";
  calFileComments[1]="";
}
int  dialogCalManager::calNumMagPoints()
{
  //Get number of points in calMagTable
  return calMagPoints;
}
int  dialogCalManager::calNumFreqPoints()
{
  //Get number of points in calFreqTable
  return calFreqPoints;
}
void dialogCalManager::calGetMagPoint(int N, float &adc, float &db, float &phase)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub calGetMagPoint N, byref adc, byref db, byref phase       'Get mag data for point N
    if N<=0 or N>calMagPoints then notice "Invalid mag cal point number."
    adc=calMagTable(N,0)
    db=calMagTable(N,1)
    phase=calMagTable(N,2)
end sub*/
}
void dialogCalManager::calGetFreqPoint(int N, float &f, float &db)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub calGetFreqPoint N, byref f, byref db       'Get mag data for point N
    if N<=0 or N>calFreqPoints then notice "Invalid frequency cal point number."
    f=calFreqTable(N,0)
    db=calFreqTable(N,1)
end sub*/
}
int dialogCalManager::calAddMagPoint(float  adc, float db, float phase)
{
  //Add mag cal point; return 1 if error
  //if (calMagPoints>=calMaxMagPoints)  calAddFreqPoint=1: exit function
  if (calMagPoints>=calMaxMagPoints)
  {
    return 1;
  }
  calMagPoints=calMagPoints+1;
  calMagTable[calMagPoints][0]=adc;
  calMagTable[calMagPoints][1]=db;
  calMagTable[calMagPoints][2]=phase;
  return 0;        //No error
}
int dialogCalManager::calAddFreqPoint(float f, float db)
{
  //Add freq point; return 1 if error
  if (calFreqPoints>=calMaxFreqPoints) return 1;
  calFreqPoints=calFreqPoints+1;
  calFreqTable[calFreqPoints][0]=f;
  calFreqTable[calFreqPoints][1]=db;
  return 0;        //No error
}
void dialogCalManager::calSortMag()    //Sort mag cal table in ascending order of ADC reading
{/*
    sort calMagTable(),1, calMagPoints,0
end sub
*/
}
void dialogCalManager::calSortFreq()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Sort freq cal table in ascending order of frequency.
    sort calFreqTable(),1, calFreqPoints,0
end sub
*/
}
void dialogCalManager::calCreateMagCubicCoeff()
{
  //Create table of cubic coefficients for mag cal table and set validPhaseThreshold
  //Each entry of the mag coefficient table will have 8 numbers. 0-3 are the A,B,C,D
  //coefficients for interpolating the real part; 4-7 are for the phase correction.

  inter.intSetMaxNumPoints(calMagPoints);  //Be sure we have room
  inter.intClearSrc();
  vars->validPhaseThreshold=0;
  float leftADC=-1;
  float leftDB=-150;
  int centerSlopeFound=0;
  int highPointNumOfCenterSlope = 0;
  for (int i=1; i <= calMagPoints; i++) //copy cal table to intSrc
  {
    //Phase corrections of 180 or -180 indicate phase is invalid at this ADC value for magnitude
    //We set validPhaseThreshold to 1 greater than the highest ADC value with such a correction.
    float thisPhaseCorrection=calMagTable[i][2];
    float thisADC=calMagTable[i][0];
    float thisDB=calMagTable[i][1];
    if (thisPhaseCorrection==180 || thisPhaseCorrection==180)
    {
      vars->validPhaseThreshold=calMagTable[i][0]+1;
    }
    inter.intAddSrcEntry(thisADC, thisDB ,thisPhaseCorrection);
    //Save first point at -60 dBm or higher
    if (leftADC==-1 && thisDB>=-60)
    {
      leftADC=thisADC;
      leftDB=thisDB;
    }
    //Save slope of ADC vs. dB line for a centrally located slope for at least a 20 dB segment.
    //This is used for adaptive wait times
    if (leftADC!=-1 && centerSlopeFound==0 && ((thisDB-leftDB) >=20))
    {
      vars->calCenterSlope=(thisADC-leftADC)/(thisDB-leftDB);
      highPointNumOfCenterSlope=i; //This will be refined below ver116-1b
      centerSlopeFound=1;
    }
  }

  //Signal that our "imaginary" part is an angle, and not to "favor flat", because
  //we expect the db as a function of ADC to become vertical near the ends.
  //Do phase only if calGetDoPhase()=1
  int favorFlat=0;
  int isAngle=1;
  int doPhase=calGetDoPhase();
  //First "1" means do magnitude; second 1 means we are doing phase correction table //ver116-1b
  inter.intCreateCubicCoeffTable(1,doPhase,isAngle, favorFlat, 1);
  for (int i=1; i <= calMagPoints; i++)  //put the data where we want it
  {
    calMagCoeffTable[i][0]=inter.intSrcCoeff[i][0];
    calMagCoeffTable[i][1]=inter.intSrcCoeff[i][1];
    calMagCoeffTable[i][2]=inter.intSrcCoeff[i][2];
    calMagCoeffTable[i][3]=inter.intSrcCoeff[i][3];
    calMagCoeffTable[i][4]=inter.intSrcCoeff[i][4];
    calMagCoeffTable[i][5]=inter.intSrcCoeff[i][5];
    calMagCoeffTable[i][6]=inter.intSrcCoeff[i][6];
    calMagCoeffTable[i][7]=inter.intSrcCoeff[i][7];
  }


  //We re-iterate the calibration table to determine a couple other items. For calculating
  //adaptive wait times, we need to know the approximate slope (delta ADC)/(delta dB).
  //We potentially divide the response into three sections:
  //For ADC<calLowADCofCenterSlope the slope is calLowEndSlope, calculated from at least a 5 dB segment near the
  //       low end, but not so low as to have slopes less than 7% of the center slope
  //for calLowADCofCenterSlope < ADC < calHighADCofCenterSlope the slope is calCenterSlope, calculated
  //        from a 20 dB or greater segment somewhere in the center area
  //for ADC>calHighADCofCenterSlope the slope is calHighEndSlope calculated from a 7 dB or greater segment at top end
  //We need a reasonable range of values to be able to do this.  ver116-1b
  vars->calCanUseAutoWait=1;
  int slopeErr=(calMagPoints<5);
  if (slopeErr==0)
  {
    if (calMagTable[1][1]>-60 || calMagTable[calMagPoints][1]<-40)
    {
      slopeErr=1;
    }
  }
  if (slopeErr==1)
  {
    //ver116-4b deleted irritating error message
    vars->calCanUseAutoWait=0;
    return;
  }
  int foundBoundary=0;
  for (int i=highPointNumOfCenterSlope; i <= calMagPoints-1; i++)
  {
    //find point where slope after that point drops below 3/4 of the center slope
    float thisSlope=(calMagTable[i+1][0]-calMagTable[i][0])/(calMagTable[i+1][1]-calMagTable[i][1]);   //delta ADC over delta dB
    if (thisSlope<0.75*vars->calCenterSlope)
    {
      highPointNumOfCenterSlope=i;
      foundBoundary=1;
      break;
    }
  }
  if (foundBoundary==0)
  {
    //Slope never dropped off much so we don't need separate high end slope
    vars->calHighADCofCenterSlope=calMagTable[calMagPoints][0];
    vars->calHighEndSlope=vars->calCenterSlope;
  }
  else
  {
    //highPointNumOfCenterSlope is the point that divides the use of the center slope and use of the high end slope
    vars->calHighADCofCenterSlope=calMagTable[highPointNumOfCenterSlope][0];
    float rightADC=calMagTable[calMagPoints][0];
    float rightDB=calMagTable[calMagPoints][1];  //final ADC and dB entries  //ver116-1b
    //find the high end slope
    for (int i=calMagPoints-1; i >= highPointNumOfCenterSlope; i--) // step -1   //ver116-1b
    {
      float thisADC=calMagTable[i][0];
      float thisDB=calMagTable[i][1];
      if (i==highPointNumOfCenterSlope || (rightDB-thisDB>=7))
      {
        float slopeDenom=rightDB-thisDB;
        if (slopeDenom==0)
        {
          QMessageBox::warning(this, "Notice", "Error in determining high end slope for Auto Wait.");
          vars->calHighADCofCenterSlope=calMagTable[calMagPoints][0];
          vars->calHighEndSlope=vars->calCenterSlope;
          vars->calCanUseAutoWait=0;
        }
        else
        {
          vars->calHighEndSlope=(rightADC-thisADC)/slopeDenom;   //Slope of top 7 dB
        }
      }
    }
  }
  float thisSlope;
  if (vars->calCanUseAutoWait==0)
  {
    vars->useAutoWait=0;
  }
  //We now have to find the lowest level at which calCenterSlope can be used. This is the point where it drops off
  //at least 60%
  foundBoundary=0;
  int lowPointNumOfCenterSlope;
  for (int i=highPointNumOfCenterSlope; i >= 2; i--)  // step -1 //ver116-1b
  {
    //find point where slope after that point drops below 3/4 of the center slope
    //But don't mess with DB values above -60 dB.
    float thisADC=calMagTable[i][0];
    float thisDB=calMagTable[i][1];
    float slopeDenom=thisDB-calMagTable[i-1][1];
    if (slopeDenom==0)
    {
      thisSlope=0;
    }
    else
    {
      thisSlope=(thisADC-calMagTable[i-1][0])/slopeDenom;   //delta ADC over delta dB
    }
    if (thisSlope<0.60*vars->calCenterSlope)
    {
      lowPointNumOfCenterSlope=i;
      foundBoundary=1;
      break;
    }
  }
  if (foundBoundary==0)
  {
    //Slope never dropped off much so we don't need separate low end slope
    vars->calLowADCofCenterSlope=0;
    vars->calLowEndSlope=vars->calCenterSlope;
  }
  else
  {
    //lowPointNumOfCenterSlope is the point that divides the use of the center slope and use of the low end slope
    vars->calLowADCofCenterSlope=calMagTable[lowPointNumOfCenterSlope][0];
    //Now find the low end slope and boundary
    int firstSlopePointNum=0;
    vars->calLowEndSlope=vars->calCenterSlope;    //In case it can//t be determined, use center slope value
    for (int i=1; i <= calMagPoints; i++)
    {
      //find point where slope after that is at least 1/10 of the center slope
      float thisADC=calMagTable[i][0];
      float thisDB=calMagTable[i][1];
      if (thisDB>-60)
      {
        QMessageBox::warning(this,  "Notice", "Error in determining low end slope for Auto Wait.");    //Cal is messed up.
        vars->calCanUseAutoWait=0;
        break;
      }
      float slopeDenom=calMagTable[i+1][1]-thisDB;
      if (slopeDenom==0)
      {
        thisSlope=vars->calCenterSlope;
      }
      else
      {
        thisSlope=(calMagTable[i+1][0]-thisADC)/slopeDenom;   //delta ADC over delta dB
      }
      if (thisSlope>=0.07*vars->calCenterSlope)
      {
        firstSlopePointNum=i;
        break;
      }
    }
    if (firstSlopePointNum==0)
    {
      QMessageBox::warning(this, "Notice", "Error in determining low end slope for Auto Wait--all slopes too small");
      vars->calCanUseAutoWait=0;
      vars->calADCofLowFringe=0;
    }
    else
    {
      float leftADC=calMagTable[firstSlopePointNum][0];
      float leftDB=calMagTable[firstSlopePointNum][1];
      vars->calADCofLowFringe=leftADC;   //below this the slopes are tiny.
      //Find low end slope
      for (int i=firstSlopePointNum+1; i <= calMagPoints; i++)
      {
        float thisADC=calMagTable[i][0];
        float thisDB=calMagTable[i][1];
        if (thisDB>-60)
        {
          QMessageBox::warning(this, "Notice", "Error in determining low end slope for Auto Wait.");    //Cal is messed up. Use center slope to very bottom.
          vars->calCanUseAutoWait=0;
          break;
        }
        if ((thisDB-leftDB)>=5) //Segment at least 5 dB long
        {
          vars->calLowEndSlope=(thisADC-leftADC)/(thisDB-leftDB); //Slope of low end
          break;
        }
      }
    }
  }
  //a1=calLowEndSlope;
  //a2=calCenterSlope;
  //a3=calHighEndSlope;
  //a4=calLowADCofCenterSlope;
  //a5=calHighADCofCenterSlope;
}
void dialogCalManager::calCreateFreqCubicCoeff()
{
  //Create table of cubic coefficients for mag cal table
  //Each entry of the freq coefficient table will have 4 numbers. 0-3 are the A,B,C,D
  //coefficients for interpolating the frequency power correction, which is a scalar.
  inter.intSetMaxNumPoints(calFreqPoints);  //Be sure we have room ver114-5q
  inter.intClearSrc();
  for (int i=1; i <= calFreqPoints; i++) //copy cal table to intSrc
  {
    inter.intAddSrcEntry(calFreqTable[i][0],calFreqTable[i][1],0);
  }
    //Signal that this is not an angle, and  to "favor flat", because
    //we expect the freq response not to become near-vertical
  int favorFlat=1; int isAngle=0;
  inter.intCreateCubicCoeffTable(1,0,isAngle, favorFlat, 0); //Final 0 means we are not doing phase correction ver116-1b
  for (int i=1; i <= calFreqPoints; i++)  //put the data where we want it
  {
    calFreqCoeffTable[i][0]=inter.intSrcCoeff[i][0];
    calFreqCoeffTable[i][1]=inter.intSrcCoeff[i][1];
    calFreqCoeffTable[i][2]=inter.intSrcCoeff[i][2];
    calFreqCoeffTable[i][3]=inter.intSrcCoeff[i][3];
  }
}
int dialogCalManager::calBinarySearch(int dataType, float searchVal)
{
//Perform search to find the lowest entry whose lookup value is >=searchVal
//Entries are numbered 1...
//If dataType=0 then the lookup is for ADC value in calMagTable; otherwise it is for freq in in calFreqTable
//Uses primarily a binary search with recursive calls.
//If searchVal is beyond the highest entry, we will return top+1
  int nPoints;
  int thisVal;
  if (dataType==0)
  {
    nPoints=calMagPoints;
    float thisVal=calMagTable[1][0];
    if (searchVal<=thisVal)
    {
      return 1;    //off bottom
    }
    thisVal=calMagTable[nPoints][0];
    if (searchVal>thisVal)
      return nPoints+1;    //off top
  }
  else
  {
    nPoints=calFreqPoints;
    thisVal=calFreqTable[1][0];
    if (searchVal<=thisVal)
    {
      return 1;    //off bottom
    }
    thisVal=calFreqTable[nPoints][0];
    if (searchVal>thisVal)
    {
      return nPoints+1;    //off top
    }
  }
  //Here we know searchVal is >first entry and <=final entry
  int top=nPoints;
  int bot=1;
  int span=top-bot+1;
  while (span>4)  //Do preliminary search to narrow the search area to no more than 4 entries
  {
    int halfSpan=span/2;
    int mid=bot+halfSpan;
    if (dataType==0)
      thisVal = calMagTable[mid][0];
    else
      thisVal = calFreqTable[mid][0];
    if (thisVal == searchVal)
      return mid;   //exact hit
    if (thisVal<searchVal)
      bot=mid+1;
    else
      top=mid;  //Narrow to whichever half thisVal is in
    span=top-bot+1;  //Repeat with either bot or top endpoints changed
  }
  //Here searchVal is > entry bot, and <= entry top, and there are at most 4 entries to search, starting with bot
  //Start with bot entry and find first entry >= searchVal. Even if there are fewer than 4 entries left, it
  //is guaranteed we will not keep searching past the top of the table.
  int ceil=bot;
  if (dataType==0)
    thisVal=calMagTable[ceil][0];
  else
    thisVal=calFreqTable[ceil][0];

  if (thisVal>=searchVal)
    return ceil; //compare first of possible 4
  ceil=ceil+1;     //ver115-9b deleted check for ceil>nPoints, which is dealt with above
  if (dataType==0)
    thisVal=calMagTable[ceil][0];
  else
    thisVal=calFreqTable[ceil][0];
  if (thisVal>=searchVal)
    return ceil; //compare second of possible 4
  ceil=ceil+1;
  if (dataType==0)
    thisVal=calMagTable[ceil][0];
  else
    thisVal=calFreqTable[ceil][0];
  if (thisVal>=searchVal)
    return ceil; //compare third of possible 4
  return ceil+1;     //return index of fourth, which is the only remaining possibility.
}
void dialogCalManager::calConvertMagPhase(float magdata, int wantPhase, float &magDB, float &phaseCorrect)
{
  //This function returns the magnitude (magDB) and the phase correction (phaseCorrect)
  //based on the ADC signal strength reading magdata. The phase correction is returned
  //only if wantPhase=1; otherwise zero is returned.

  //It uses cubic interpolation on the calibration entries of calMagTable.
  //Note: calMagTable is organized with the smallest ADC values first
  //SEW8 revised call to calBinarySearch
  int index=calBinarySearch(0, magdata);   //search calMagTable
  //index now is the first entry >= magdata, except that if no entry meets that test,
  //index will be one past the end.
  phaseCorrect=0;
  if (index>calMagPoints)
  {
    //Off top end;use largest mag and phase correction for largest ADC entry
    magDB=calMagTable[calMagPoints][1];
    if (wantPhase)
    {
      phaseCorrect=calMagTable[calMagPoints][2];
    }
    return;
  }
  if (index==1)
  {
    //Off bottom end;use mag and phase correction for smallest ADC entry
    magDB=calMagTable[1][1];
    if (wantPhase)
    {
      phaseCorrect=calMagTable[1][2];
    }
    return;
  }

  //Evaluate cubic at x=magdata
  float dif = magdata - calMagTable[index][0];
  float A=calMagCoeffTable[index][0];
  float B=calMagCoeffTable[index][1];
  float C=calMagCoeffTable[index][2];
  float D=calMagCoeffTable[index][3];
  magDB = A+dif*(B+dif*(C+dif*D));

  if (wantPhase==1)
  {
    float A=calMagCoeffTable[index][4];
    float B=calMagCoeffTable[index][5];
    float C=calMagCoeffTable[index][6];
    float D=calMagCoeffTable[index][7];
    phaseCorrect = A+dif*(B+dif*(C+dif*D));
    if (phaseCorrect<=-180)
    {
      phaseCorrect=phaseCorrect+360;
    }
    if (phaseCorrect>180)
    {
      phaseCorrect=phaseCorrect-360;
    }
  }
}
float dialogCalManager::calConvertFreqError(float freq)
{
  //Find power correction for frequency error contribution.
  //needed: datatable, frequency error calibration table; creates average freqerror

  //This routine uses cubic interpolation on calFreqTable to determine the frequency
  //error for thisfreq. The calibration table consists of entries numbered from 1, each
  //containing a frequency and the error at that frequency. The entries must be in order of
  //increasing, with the first entry being the lowest frequency. If the frequency is less than the
  //first entry, or greater than the final entry,the error for that entry will be used.
  //The final entry need not be for the maximum possible frequency.
  //SEW8 revised call to calBinarySearch
  int index=calBinarySearch(1,freq);    //Search calFreqTable
  //index now is the first entry >= freq, except that if no entry meets that test,
  //index will be one past the end.
  if (index>calFreqPoints)
  {
    return calFreqTable[calFreqPoints][1];  //use largest value
  }
  if (index==1)
  {
    return calFreqTable[1][1];    //Use smallest value
  }
  //(5)Evaluate cubic at x=magdata
  float dif=freq-calFreqTable[index][0];
  float A=calFreqCoeffTable[index][0];
  float B=calFreqCoeffTable[index][1];
  float C=calFreqCoeffTable[index][2];
  float D=calFreqCoeffTable[index][3];
  return A+dif*(B+dif*(C+dif*D));

}
void dialogCalManager::calCreateDefaults(int pathNum, QString editor, int doPoints)
{

  //Create default data in file or text editor.
  //if editor$<>"", it contains the destination text editor; otherwise we do a file,
  //which replaces any existing file. For a file, we assume any necessary folders exist.
  //If pathNum=0 we create a freq cal file; othwerwise a mag cal file for path pathNum
  //if doPoints=1 we create two data points; otherwise no points
  int doFile;
  QStringList list;
  QString c1, c2;
  if (editor=="")
  {
    doFile=1;
    //open calFilePath$()+calFileName$(pathNum) for output as #calOut
    //editor$="#calOut"
  }
  else
  {
    doFile=0;
    //#editor$, "!cls"    //Clear existing data in text editor
  }

    //Start with comments
  if (pathNum==0)
  {
        //Freq
    c1="Calibration over frequency";
    c2="Calibrated " + QDateTime::currentDateTime().toString("MM/dd/yy") + " at -zz dBm."; //ver116-1b
  }
  else
  {
        //Mag
    c1="Path"+QString::number(pathNum)+" CenterFreq= xxx; Bandwidth=yyy";
    c2="Calibrated " + QDateTime::currentDateTime().toString("MM/dd/yy") + " at zz MHz.";
  }

  calFileComments[0]=c1  ; calFileComments[1]=c2;   //Save comments in our array
  list.append(calFileCommentChar+c1);
  list.append(calFileCommentChar+c2);   //print comments
  list.append("CalVersion="+util.usingF("##.##", calModuleVersion));
  if (pathNum==0)
  {
    list.append("FreqTable=");
    list.append(calFileCommentChar+"    MHz        dB   in increasing order of MHz");    //ver116-4L
    if (doPoints==1)
    {
        list.append("0           0");
        list.append("1000        0");
    }
  }
  else
  {
    list.append("MagTable=");
    list.append(calFileCommentChar+"  ADC      dBm  in increasing order of ADC");    //ver116-1b
    QString p;
    if (calDoPhase==1) p="      0"; else p="";
    if (doPoints==1)
    {
        //ver115_1a changed defaults to be based on ADC type  ver115-1b made max half of maxbits
        //AtoD topology."8" for original 8 bit,"12" for optional 12 bit ladder,"16" for serial 16 bit AtoD, or "22" for serial 12 bit AtoD
        int maxADC=(int)(65535/2) ; int maxADCval=0;          //16 bit ADC ver115-4e
        if (activeConfig->adconv==8) maxADC=(int)(255/2);            //8 bit ADC
        if (activeConfig->adconv==12 || activeConfig->adconv==22) { maxADC=4095 ; maxADCval=10;}   //12 bit ADC ver115-4e
        list.append("0        -120");
        list.append(QString::number(maxADC) + "      " +QString::number(maxADCval)); //ver115-4e
    }
  }
  //For now, we don't use the EndCalFile tag, and don't require it on input either
  //print #editor$, "EndCalFile="
  //if doFile then close #calOut
  QString settings = list.join("\n") + "\n";
  QString fff = calFilePath()+calFileName(pathNum);;
  QFile fOut(fff);
  if (fOut.open(QFile::WriteOnly | QFile::Text))
  {
    QTextStream s(&fOut);
    s << settings;

    fOut.close();
  }

}
void dialogCalManager::calAvailableFiles(int pathNum, QString editor, int doPoints)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
sub calCreateDefaults pathNum, editor$, doPoints 'Create default data in file or text editor.
    'if editor$<>"", it contains the destination text editor; otherwise we do a file,
    'which replaces any existing file. For a file, we assume any necessary folders exist.
    'If pathNum=0 we create a freq cal file; othwerwise a mag cal file for path pathNum
    'if doPoints=1 we create two data points; otherwise no points
    if editor$="" then
        doFile=1
        open calFilePath$()+calFileName$(pathNum) for output as #calOut
        editor$="#calOut"
    else
        doFile=0
        #editor$, "!cls"    'Clear existing data in text editor
    end if

        'Start with comments
    if pathNum=0 then
            'Freq
        c1$="Calibration over frequency"
        c2$="Calibrated " + Date$("mm/dd/yy") + " at -zz dBm." 'ver116-1b
    else
            'Mag
        c1$="Path"+str$(pathNum)+" CenterFreq= xxx; Bandwidth=yyy"
        c2$="Calibrated " + Date$("mm/dd/yy") + " at zz MHz."
    end if
    calFileComments$(1)=c1$  : calFileComments$(2)=c2$   'Save comments in our array
    print #editor$, calFileCommentChar$+c1$
    print #editor$, calFileCommentChar$+c2$   'print comments
    print #editor$, "CalVersion=";using("##.##", calModuleVersion)
    if pathNum=0 then
        print #editor$, "FreqTable="
        print #editor$, calFileCommentChar$+"    MHz        dB   in increasing order of MHz"    'ver116-4L
        if doPoints=1 then
            print #editor$, "0           0"
            print #editor$, "1000        0"
        end if
    else
        print #editor$, "MagTable="
        print #editor$, calFileCommentChar$+"  ADC      dBm  in increasing order of ADC"    'ver116-1b
        if calDoPhase=1 then p$="      0" else p$=""
        if doPoints=1 then
            'ver115_1a changed defaults to be based on ADC type  ver115-1b made max half of maxbits
            'AtoD topology."8" for original 8 bit,"12" for optional 12 bit ladder,"16" for serial 16 bit AtoD, or "22" for serial 12 bit AtoD
            maxADC=int(65535/2) : maxADCval=0          '16 bit ADC ver115-4e
            if adconv=8 then maxADC=int(255/2)            '8 bit ADC
            if adconv=12 or adconv=22 then maxADC=4095 : maxADCval=10   '12 bit ADC ver115-4e
            print #editor$, "0        -120"
            print #editor$, maxADC; "      ";maxADCval 'ver115-4e
        end if
    end if
    'For now, we don't use the EndCalFile tag, and don't require it on input either
    'print #editor$, "EndCalFile="
    if doFile then close #calOut
end sub
*/
}
void dialogCalManager::calInstallFile(int pathNum)
{
  //Open cal file and load data. Create folder and file if necessary
  //If pathNum=0 we want a freq cal file; otherwise a mag cal file for path pathNum
  //Find out whether the necessary folders exist for config and calibration
  //information. If no proper cal file exists, then create one with
  //default values, but ask user before replacing an existing file
  //SEWcal Also calculates cubic interpolation coefficients for the newly loaded file

  //Create the name of the desired file
  QString fileName=calFileName(pathNum);
  //See if we have the folder MSA_Info
/*  files DefaultDir$, "", calFileInfo$()
  numFolders=val(calFileInfo$(0,1))
  haveFolder=0
  for i=1 to numFolders
      if calFileInfo$(i,1)="MSA_Info" then haveFolder=1: exit for
  next  i
  //Create folders and file if necessary, or just open file
  if haveFolder=0 then
          //Create MSA_Info folder
      if 0<>mkDir("MSA_Info") then notice "Cannot access files."
  end if
      */
  if (!QDir().mkpath(DefaultDir + "/MSA_Info"))
    return;

      //We have the MSA_Info folder. See if we have MSA_Cal folder
  /*
  files DefaultDir$+"\MSA_Info", "", calFileInfo$()
  numFolders=val(calFileInfo$(0,1))
  haveFolder=0
  for i=1 to numFolders
      if calFileInfo$(i,1)="MSA_Cal" then haveFolder=1: exit for
  next  i


  if haveFolder=0 then
          //Create MSA_Cal folder
      if 0<>mkDir("MSA_Info\MSA_Cal") then notice "Cannot access files."
  end if
      */
  if (!QDir().mkpath(DefaultDir + "/MSA_Info/MSA_Cal"))
  {
    QMessageBox::critical(0,"Error", "Cannot access files.");
    return;
  }
      //We have the MSA_Info\MSA_CAL folder. See if it has proper file
  if (!QFile::exists(DefaultDir + "/MSA_Info/MSA_Cal/" + fileName))
  {
    calCreateDefaults(pathNum, "",1);  //No file. Create one.
  }

      //Get data from file
  QString fErr=calLoadFromFile(pathNum);
  if (fErr!="")
  {
    //File is there, but has a problem
    //Confirm "File error: "+fErr$+"; Replace with default file?"; response$

    if (QMessageBox::question(0,"File Error", "Replace with default file?",QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
    {
      QMessageBox::about(0,"","File not replaced. Internal cal data cleared.");
      calClearComments();
      if (pathNum==0) calClearFreqPoints(); else calClearMagPoints();
      return;
    }

    calCreateDefaults(pathNum, "", 1);
    //Try once more. Any error now is a system error
    //if (""!=calLoadFromFile(pathNum)) {QMessageBox::warning(0, "", "Cannot access file"); return; }
  }

  if (calFileVersion > calfigModuleVersion)
  {
    QMessageBox::warning(0,"", "Warning: Calibration file format is later version than the sofware");
  //Here we should update the file if calFileVersion is old. But at this
  //time we have no old file versions
  }

  //SEWcal Calculate interpolation coefficients for this file, and for path file determine validPhaseThreshold
  if (pathNum==0)
    calCreateFreqCubicCoeff();
  else
    calCreateMagCubicCoeff();

}
void dialogCalManager::calSaveToFile(int pathNum)
{
  //Save a cal file for path number pathNum. We assume the necessary
  //folders are already in place
  //If pathNum=0 we are dealing with a freq cal file.
  //File output replaces any existing file of the same name.
  //calFileComments$() are placed at the beginning of the file, marked
  //with calFileCommentChar$

  QString calFile = calFilePath() + calFileName(pathNum);
  QFile fOut(calFile);
  int startLine=1;
  if (fOut.open(QFile::WriteOnly | QFile::Text))
  {
    QStringList toSave;
    calOutputData(toSave, pathNum, startLine);

    QTextStream s(&fOut);
    QString settings = toSave.join("\n") + "\n";
    s << settings;

    fOut.close();
  }
}
void dialogCalManager::calSaveToEditor(QPlainTextEdit *editor, int pathNum)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;

//Save to text editor
    //Save variables and point array into text editor, in same format as a file
    //calFileComments$() are placed at the beginning of the data, marked
    //with calFileCommentChar$
    int startLine=1;
    QStringList newText;
    calOutputData(newText, pathNum, startLine);
    editor->clear();
    editor->insertPlainText(newText.join("\n"));
    editor->moveCursor(QTextCursor::Start);
    teTextChanged = false;
//    #editor$, "!select 1,1"
//    #editor$, "!copy" : #editor$, "!paste"  //This scrolls the window to the top
}
void dialogCalManager::calOutputData(QStringList &lines, int pathNum, int &startLine)
{
  //Output data to file or text editor
  //Save data as a frequency cal file. Replaces any existing file.
  //The data is sent to  a file already opened, whose handle is in calFile$
  //or from a text editor whose handle is in calFile$. The type of source is
  //indicated by isFile (1 for file, 0 for textEditor).
  //calFileComments$() are placed at the beginning of the file, marked
  //with calFileCommentChar$
  //We update startLine to one past the last line we write

  //Start with comments

  for (int i=1; i <= 2; i++)
  {
    QString com = calFileComments[i];
    if (com!="")
      lines.append(calFileCommentChar + com);
  }

  lines.append("CalVersion=" + util.usingF("##.##", calModuleVersion));
  if (pathNum==0)
  {
    lines.append("FreqTable=");
    lines.append(calFileCommentChar+"    MHz        db   in increasing order of MHz");
    for (int i=1; i <= calFreqPoints; i++)
    {
      //float f=calFreqTable[i][0];
      QString f = util.usingF("####.######", calFreqTable[i][0]);
      QString v = util.usingF("####.###", calFreqTable[i][1]);
      lines.append(f+"  "+v);  //output data line
    }
  }
  else
  {
    lines.append("MagTable=");
    if (calDoPhase==0)
    {
      lines.append(calFileCommentChar + "  ADC      dBm   in increasing order of ADC");
    }
    else
    {
      lines.append(calFileCommentChar + "  ADC      dBm      Phase   in increasing order of ADC");
    }
    for (int i=1; i <= calMagPoints; i++)
    {
      QString adc = util.usingF("#######", calMagTable[i][0]);
      QString v = util.usingF("####.###", calMagTable[i][1]);
      QString p = util.usingF("####.##", calMagTable[i][2]);
      if (calDoPhase==0)
      {
        lines.append(adc + "  " +v);  //output data line
      }
      else
      {
        lines.append(adc + "  " + v + "  " + p); //output data line
      }
    }
  }
  //For now, we don't print the end tag, nor do we require it on input
  //print #calFile$, "EndCalFile="
}
QString dialogCalManager::calOpenFile(int pathNum)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
   'Open calibration file for path number pathNum; return its handle
    'if pathNum=0 then we want freq cal file
    'If file does not exist, return "".
    fName$=calFileName$(pathNum)
    On Error goto [noFile]
    open fName$ for input as #calFile
    calOpenFile$="#calFile"
    exit function
[noFile]
    fName$=""
    calOpenFile$="" 'ver114-2f
*/
  return "fix me";
}
QString dialogCalManager::calFileName(int pathNum)
{
  //Return file name for the specified pathNum
  //If pathNum=0 we want the freq cal file; otherwise mag cal for path number pathNum
  if (pathNum==0)
    return "MSA_CalFreq.txt";
  else
    return "MSA_CalPath" + QString::number(pathNum) + ".txt";

}
QString dialogCalManager::calFilePath()
{
  //Return path name for cal files, ending in "\"
  return DefaultDir + "/MSA_Info/MSA_Cal/";
}
QString dialogCalManager::calLoadFromEditor(QPlainTextEdit *editor, int pathNum)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;

  //Read data from text editor. Return error message
  //editor$ is the handle of the text editor. We also sort.
  int startLine=0;
  int isFile=0;
  QString fErr = calReadFile (NULL, editor, isFile, pathNum, startLine);
  if (pathNum==0)
  {
    calSortFreq();
  }
  else
  {
    calSortMag();
  }

  if (pathNum!=0)
  {
    activeConfig->finalfreq = activeConfig->MSAFilters[pathNum][0];
    activeConfig->finalbw = activeConfig->MSAFilters[pathNum][1];
  }

  return fErr;
}
QString dialogCalManager::calLoadFromFile(int pathNum)
{
 QString retVal;
  //Open and read file. Return error message
  //This is called only after we know the file exists
  QString fileName=calFilePath() + calFileName(pathNum);
  QFile *calIn = new QFile(fileName);
  if (calIn->open(QFile::ReadOnly))
  {
  }
  //open fileName$ for input as #calIn
  int startLine=1; int isFile=1;
  retVal = calReadFile(calIn, NULL, isFile, pathNum, startLine);
  calIn->close();
  if (pathNum!=0)
  {
    activeConfig->finalfreq=activeConfig->MSAFilters[pathNum][0];
    activeConfig->finalbw=activeConfig->MSAFilters[pathNum][1];
  }   //SEW7 made conditional
  return retVal;

}
QString dialogCalManager::calReadFile(QFile *calFile, QPlainTextEdit *editor, int isFile, int pathNum, int &startLine)
{
  //Reads calibration data, starting at line startLine and continuing until
  //we find the tag "EndCalFile=" (case insensitive). We update startLine
  //to one past the line containing that tag.
  //If pathNum=1... we read magnitude calibration data for a
  //particular signal path. There is one path for each RBW filter. if pathNum=0 then
  //we read frequency calibration data, for which there is only one data set.
  //The data is read from a file already opened, whose handle is in calFile$
  //or from a text editor whose handle is in calFile$. The type of source is
  //indicated by isFile (1 for file, 0 for textEditor).
  //The data is in the following format; comments are allowed and are marked
  //by calFileCommentChar$. First two comment lines preceding tags are put into calFileComments$
  //Each data tag ends with "=", and is case insensitive.
  //---Magnitude calibration file format---
  //VERSION=xxx    //Version of the module that wrote the file
  //MagTable=      //Start of magnitude cal data; no data on this line
  //xxxx xxxx      //Up to calMaxMagPoints lines of data pairs; ADC reading(increasing) and mag(db)
  //xxxx xxxx      //Table is put into calMagTable, with ADC reading as x value
  //ENDCALFILE=    //marks end of data
  //
  //---Frequency calibration file format---
  //VERSION=xxx    //Version of the module that wrote the file
  //FreqTable=      //Start of frequency cal data; no data on this line
  //xxxx xxxx      //Up to to calMaxPoints lines of data pairs;frequency(MHz)(increasing) and magitude(db)
  //xxxx xxxx      //Table is put into calFreqTable with frequency as x value
  //ENDCALFILE=    //marks end of data
  //---end of file formats
  //
  //The number of entries in calMagTable is put into calMagPoints
  //The number of entries in calFreqTable is put into calFreqPoints
  //If error, we return a string describing the error
  //If no error, we return ""
  QString retVal;
  int doMag=0;     //=1 while we are processing mag cal data
  int doFreq=0;     //=1 while we are processing freq cal data
  if (pathNum>0)      //Reset number of points
  {
    calClearMagPoints();
  }
  else
  {
    calClearFreqPoints();
  }
  int ncalFileComments=0;
  int accumComments=1;
  int nLines;
  calFileComments[0]="";
  calFileComments[1]="";
  QStringList lines;
  if (isFile==0)
  {
    //then #calFile$, "!lines nLines"
    QString text = editor->toPlainText();
    lines = text.split("\n");
    nLines = lines.count();

  }
  else
    nLines=100000;
  int prevXVal=-100000;      //SEW7; Used to check for increasing values
  QString p="Path "+QString::number(pathNum)+"; ";   //For error messages
  for (int fileLine=startLine; fileLine < nLines; fileLine++)
  {
    QString tLine;
        //Loop until we reach nLines (if text editor) or end of file (if file)
    if (isFile)
    {
        if (calFile->atEnd())
          break;  //exit for
        tLine = util.readLine(calFile, '\r'); // Read one line
    }
    else
    {
      qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
      tLine = lines.at(fileLine);

    //    print #calFile$, "!line ";fileLine;" tLine$" //Get line number fileLine
    }

    startLine=startLine+1;   //one past where we are
    tLine=tLine.trimmed();    //drop extra blanks
    QString startChar=tLine.left(1);  //first real character of line

    int isComment;
    if (startChar==calFileCommentChar) isComment=1; else isComment=0;
    if (isComment || startChar=="")
    {
        //Save first two comment lines, w/0 the comment character
        if (accumComments && isComment && ncalFileComments<2)
        {
          ncalFileComments=ncalFileComments+1;
          calFileComments[ncalFileComments]=tLine.mid(1);
        }
    }
    else    //valid non-comment line
    {
        accumComments=0;     //no more comments put into calFileComments$$
        int equalPos=tLine.indexOf("=");     //equal sign marks end of tag
        if (equalPos!=-1) //Equal sign found
        {
            QString tag=tLine.left(equalPos).toUpper();
            QString item=tLine.mid(equalPos+1);  //stuff after equal sign
            int commentPos=item.indexOf(calFileCommentChar);
            if (commentPos>=0)
              item=item.left(commentPos-1);   //drop comments
            item=item.trimmed();
            doMag=0; doFreq=0; int isErr=0;

            if (tag == "CALVERSION")
            {
              int a = ceil(item.toFloat()*100);
              int b = ceil(calModuleVersion*100);
              if (a>b)
                retVal="Warning; " + p + "File format is later version than the sofware";
            }
            else if (tag == "MAGTABLE")
            {
              if (pathNum==0) isErr=1; else doMag=1;
            }
            else if (tag == "FREQTABLE")
            {
              if (pathNum>0)
                isErr=1;
              else
                doFreq=1;
            }
            else if (tag == "ENDCALFILE")
            {
              //This marks the end of the file
              //SEW7 deleted check to see that data points existed
              retVal="";   //no error
              return retVal;
            }
            else
            {
              isErr=true;
            }
            if (isErr)
            {
              retVal = p + "Line "+QString::number(fileLine);
              return retVal;
            }  //invalid tag
        }
        else    //No equal sign; must be data
        {
            //Now retrieve the numbers from this data line. There will always
            //be at least two numbers; for mag calibration of VNA there may also be phase.
             //Numbers are separated by comma, tab or space
            QString delims=" ,\t";    //space, comma and tab are delimiters
            float data1, data2, data3, data4, data5;
            int isErr=util.uExtractNumericItems(2, tLine, delims,data1, data2, data3);
                //If not numeric, signal error
            if (isErr==1) { retVal =p+"Line "+QString::number(fileLine); return retVal; }
            if (doMag)
            {
                int calMagDataHadPhase=0;
                if (tLine!="")  //If there is data left, we must have a third number
                {
                    isErr=util.uExtractNumericItems(1, tLine, delims,data3, data4, data5);
                    if (isErr==1) {retVal=p+"Line "+QString::number(fileLine); return retVal;}
                    calMagDataHadPhase=1;
                }
                else
                {
                    data3=0;
                }
            }
            //SEWcal2 modified the next approx 17 lines to eliminate
            //duplicate entries in some cases. ver113-7g
            int skipData=0;
            int prevY1 = 0; int prevY2 = 0;
            if (data1<=prevXVal)  //SEWcal2 created this if...else block
            {
                //Data in file must have increasing x values; in text editor we let it slide
                //If we have absolutely identical entries, we skip the duplicates.
                if (data1==prevXVal && data2==prevY1 && data3==prevY2)
                {
                    skipData=1;
                }
                else
                {
                    if (isFile==1) {retVal=p+"Line "+QString::number(fileLine); return retVal;} //not increasing
                }
            }
            prevXVal=data1 ; prevY1=data2 ; prevY2=data3; //SEWcal2
            int tooMany=0;
            if (doMag)   //Add mag point; quit if error (i.e. too many points)
            {
                if (skipData==0) tooMany=calAddMagPoint(data1, data2, data3);   //SEWcal2
            }
            else    //Add frequency point
            {
                if (skipData==0) tooMany=calAddFreqPoint(data1, data2);   //SEWcal2
            }
            if (tooMany==1)
            {
              retVal=p+"Line "+QString::number(fileLine)+"; Too many points.";
              return retVal;
            }
        }
    }  //end of processing non-comment line
  }

  //We can only get here if we did not find the EndCalFile tag, which is really
  //only needed if this file is embedded in another. So for now we let it go.
  //calReadFile$="Proper end of data not found."
  return "";

}


int dialogCalManager::calManFileReload()
{
  if (calEditorPathNum<0)
  {
    QMessageBox::information(this, "", "You must select a calibration file to save.");
    return 0;
  }
  if (calEditorPathNum==0)
  {
    //Before loading the frequency file we must be sure that the mag cal
    //data is valid for the MSA//s active filter
    int match=0;
    for (int i=1; i <= activeConfig->MSANumFilters; i++)
    {
      float f = activeConfig->MSAFilters[i][0];
      float bw = activeConfig->MSAFilters[i][1];
      if (f == activeConfig->finalfreq && bw == activeConfig->finalbw)
      {
        match=i;
        break; //exit for
      }
    }
    if (match==0)
    {
      QMessageBox::information(this, "notice", "Can't find active filter; don't use Measure.");    //only possible in debugging
    }
    calInstallFile(match);  //Loads mag cal file
  }
  calInstallFile(calEditorPathNum);
  ui->te->clear();
  calSaveToEditor(ui->te, calEditorPathNum); //display data

  calManPrepareEntry(); //Prepare buttons and instructions for data entry
}

void dialogCalManager::on_Reload_clicked()
{  
  calManFileReload();
}

void dialogCalManager::on_Save_clicked()
{
  if (calEditorPathNum<0)
  {
    QMessageBox::information(this, "", "You must select a calibration file to save.");
    return;
  }

  QString errMsg = calLoadFromEditor(ui->te, calEditorPathNum);
  if (errMsg!="")
  {
    QMessageBox::information(this, "notice", "Data Error: " + errMsg + "; file not saved.");
    return;
  }
  calSaveToFile(calEditorPathNum);
  calManFileReload();   //Reload to update editor and verify data
}

void dialogCalManager::on_Return_clicked()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*    doCan=calManWarnToSave(1) //Warn to save if data changed. Allow cancel.
      if doCan=1 then return  //cancelled  Exit calManRunManager
      call calInstallFile 0   //Leave with freq file installed
      //call calInstallFile 1   //Leave with path 1 installed  del ver116-1b
      call calCloseWindows
      haltsweep=0     //SEW6
      call RequireRestart //ver116-4j
      return      //Returns to caller of calManRunManager
  */
}

void dialogCalManager::on_StartEntry_clicked()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Display instructions for entering data
      //calManEntryIsRef=1 for entering the first (reference) point; =0 for other points
      #calman.enterInstruct, "cls"
      #calman.enterInstruct, "place 10 15"
      #calman.StartEntry, "!hide"
      #calman.Enter, "!show"
      #calman.Lref, "!hide"   //Reference and its label hide
      #calman.ref, "!hide"
      #calman.data1, "!show"   //data 1 and its label show
      #calman.Ldata1, "!show"
      #calman.data2, "!show"    //data2 and its label show
       #calman.Ldata2, "!show"
      #calman.data2, "!enable"  //data2 enable  ver 114-3d
      #calman.data3, "!show"   //data3 and its label show
      #calman.Ldata3, "!show"
      #calman.data3, "!enable"  //data3 enable  ver 114-3d
      #calman.ref2, "!hide"  //ref2 and its label hide
      #calman.Lref2, "!hide"
      #calman.data2, ""   //Clear data boxes
      #calman.data3, ""

      if calEditorPathNum=0 then
          //Here we are doing freq cal. We need frequency and measured power for each point.
          //These are entered from sweep data, so the boxes are disabled to prevent the user
          //from directly entering data.
          #calman.data2, "!disable"  //disable freq    //ver 114-3d
          #calman.data3, "!disable"  //disable power   //ver 114-3d
          #calman.Lref, "True Power(dBm)" //ver116-1b
          #calman.Ldata1, "Point Number"  //SEW8
          #calman.Ldata2, "Freq (MHz)"
          #calman.Ldata3, "Measured Power (dBm)" //ver116-1b
          #calman.ref, "!enable" //ver114-3b
          #calman.ref2, "!disable"
          #calman.Measure, "!hide"      //SEW8 Hide Measure and show NextPoint, PrevPoint instead
          #calman.NextPoint, "!show"
          #calman.PrevPoint, "!show"
          #calman.EnterAll, "!show"
          #calman.Force, "hide" : #calman.ForceVal, "!hide"  //ver116-4j

              //ver113-7e altered the following instructions for each "#calman.enterInstruct" modver115-1a
          #calman.enterInstruct, "\Frequency Calibration should be performed with an input power of -20 dBm to -40 dBm."
          #calman.enterInstruct, "\A calibration list has been created from the previous sweep, with the number of Points"
          #calman.enterInstruct, "\equaling the number of steps in the sweep, +1. Data of each Point can be accessed by"
          #calman.enterInstruct, "\clicking the Next Point button.  You must change the value in the True Power box to"
          #calman.enterInstruct, "\the Known Input Power. You may modify any data by highlighting it and retyping it."
          #calman.enterInstruct, "\You may insert data from a single Point by clicking the Enter button. You may insert"
          #calman.enterInstruct, "\the data of every Point in the sweep by clicking the Enter All button. You may insert"
          #calman.enterInstruct, "\data by typing the data directly into the Frequency Calibration Table, after the last"
          #calman.enterInstruct, "\data Point. Data may be entered in any order. Clean Up to sort the data at any time."
          #calman.enterInstruct, "\The Magnitude Error vs Frequency Correction Factor = True Power - Measured Power."

          if calManEntryIsRef=1 then
                  //first point for frequency cal
                  //SEW8 Rewrote the following message
              notice "Notice" +chr$(13)+"Automatic entry of points assumes you have performed " _
                                  +chr$(13)+"a sweep over the desired frequency range before entering the" _
                                  +chr$(13)+"Calibration Mangager. Numbers (0...) refer to points of that sweep."
              #calman.ref, ""
              #calman.ref2, ""
              call calManGetFreqInput 0   //SEW8 Display data for point 0
          else
              //2nd+ points for frequency cal
              #calman.ref, "!show"       //show power ref
              #calman.Lref, "!show"
          end if
      else
          //Here we are doing mag cal. We need Input power, ADC and possibly phase for each point
          //For first point we need frequency.
          #calman.data1, ""   //Clear data box
          #calman.Lref2, "Ref Phase"
          #calman.Ldata2, "ADC value"
          #calman.Ldata3, "Phase (degrees)" //ver114-3b Note label is two lines high
          #calman.Ldata1, "Input (dBm)"   //ver116-1b
          #calman.ref2, "!disable"     //don't allow reference phase to be changed
          #calman.Force, "show" : #calman.Force, "reset" : #calman.ForceVal, "0"  : #calman.ForceVal, "!hide"  //ver116-4j
          if calGetDoPhase() =0 then
              #calman.data3, "!hide"  //data3 and its label hide (not doing phase)
              #calman.Ldata3, "!hide"
              #calman.Force, "hide" : #calman.ForceVal, "!hide"  //ver116-4j
          end if
          #calman.ref, "!show"           //ref will hold reference frequency
          #calman.Lref, "!show"
          #calman.Lref, "Ref Freq (MHz)"
          #calman.Measure, "!show"      //SEW8 Hide Measure and show NextPoint, PrevPoint instead
          #calman.Measure, "!enable"
          #calman.NextPoint, "!hide"
          #calman.PrevPoint, "!hide"
          #calman.EnterAll, "!hide"
          if calGetDoPhase() =0 then
              #calman.enterInstruct, "\For each point, enter the input power level and the ADC reading, then click Enter."
              #calman.enterInstruct, "\For the first point also enter the reference frequency at which the measurements"
              #calman.enterInstruct, "\made. The power level may be entered manually or automatically measured with Measure."
              #calman.enterInstruct, "\or automatically measured with Measure."
              #calman.enterInstruct, "\Data may be entered in any order; to sort the data click Clean Up"
              #calman.enterInstruct, "\You may add, alter or delete lines directly in the text editor if desired."

          else  //ver113-7 changed the following instructions:
              #calman.enterInstruct, "\The Spectrum Analyzer must be configured for zero sweep width. Center Frequency must"
              #calman.enterInstruct, "\be higher than 0 MHz. The first data Point will become the Reference data for all"
              #calman.enterInstruct, "\other data Points. Click Measure button to display the data measurements for ADC"
              #calman.enterInstruct, "\value and Phase. Manually, enter the Known Power Level into the Input (dBm) box."
              #calman.enterInstruct, "\Click the Enter button to insert the data into the Path Calibration Table."
              #calman.enterInstruct, "\Subsequent Data may be entered in any order, and sorted by clicking Clean Up. ADC"
              #calman.enterInstruct, "\bits MUST increase in order and no two can be the same. You may alter the Data in"
              #calman.enterInstruct, "\the table, or boxes, by highlighting and retyping. The Phase Data (Phase Error vs"
              #calman.enterInstruct, "\Input Power) = Measured Phase - Ref Phase, is Correction Factor used in VNA."
              #calman.enterInstruct, "\Phase is meaningless for the Basic MSA, or MSA with TG."

          end if
          if calManEntryIsRef=1 then
              //first point for mag cal
              #calman.ref, "!enable"    //Allow ref freq to be changed on first point only
              #calman.ref, ""
              #calman.ref2, ""
                  //Measure can be used only if this mag file matches active filter path
              f=MSAFilters(calEditorPathNum,0) : bw=MSAFilters(calEditorPathNum,1)
              if f=finalfreq and bw=finalbw then haveProperFilt=1 _
                                  else haveProperFilt=0
              if haveProperFilt=1 then
                  #calman.Measure, "!enable"
              else
                  #calman.Measure, "!disable"
                  notice "Notice"+chr$(13)+"Measurements made at this time will not be accurate, because " _
                                  +chr$(13)+"the selected filter is not the currently installed filter."
              end if
          else
              //2nd+ points for mag cal
              #calman.ref, "!disable"    //Allow ref freq to be changed on first point only
              if calGetDoPhase() =1 then
                  #calman.ref2, "!show"
                  #calman.Lref2, "!show"   //ref2 and its label show (phase ref)
                  #calman.Lref2, "Ref Phase(deg)"
              end if
          end if
      end if
      #calman.enterInstruct, "flush"  //Make graphics stick
  end sub
  */
}

void dialogCalManager::on_Clean_clicked()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*//Format and Sort list
      //We just get the data and put it back
      fErr$=calLoadFromEditor$("#calman.te", calEditorPathNum) //This also sorts
      if fErr$<>"" then notice "Bad Data. "+fErr$ : exit sub
      if calEditorPathNum=0 then nPoints=calNumFreqPoints() else nPoints=calNumMagPoints()
      #calman.te, "!cls"
      call calSaveToEditor  "#calman.te", calEditorPathNum
      if nPoints=0 then
          //If there are no data points, start over with a clean header
          call calCreateDefaults calEditorPathNum,"#calman.te", 0 //zero signals no points
      end if
  end sub
  */
}

void dialogCalManager::on_DispDefault_clicked()
{
  if (calManWarnToSave(1)==1)
  {
    return;   //Cancelled
  }
  calCreateDefaults(calEditorPathNum, "#calman.te", 1);
  calManClean();
  calManPrepareEntry();
}

void dialogCalManager::on_Measure_clicked()
{

}

void dialogCalManager::on_Enter_clicked()
{

}

void dialogCalManager::on_NextPoint_clicked()
{

}

void dialogCalManager::on_EnterAll_clicked()
{

}

void dialogCalManager::on_pathList_currentRowChanged(int currentRow)
{
  //Handle user selection of calibration file ver116-1b made this a branch label menu item
  //Can be called only from menu
  //Set current path number. Will be negative if there is no selection
  if (calManWarnToSave(1)==1)
    return;

  int prevPathNum=calEditorPathNum;
  if (currentRow == -1)
  {
    QMessageBox::information(this, "", "No File Selected.");
    return;  //Must be error
  }
  calEditorPathNum=currentRow;    //calEditorPathNum starts at 0
  if (calManFileReload()==1)  //Load new file
  {
    //User cancelled the switch
    ui->pathList->setCurrentRow(prevPathNum);
    calEditorPathNum=prevPathNum;
  }

  if (calEditorPathNum==0)
  {
    ui->teLabel->setText("Frequency Calibration Table");
  }
  else
  {
    ui->teLabel->setText("Path Calibration Table");
    vars->path = "Path " + QString::number(calEditorPathNum);   //set main MSA path variable
    // fix me SelectFilter(vars->filtbank);
    PartialRestart();
  }
  calManRefPhase=0;

}

void dialogCalManager::on_te_textChanged()
{
  teTextChanged = true;
}
