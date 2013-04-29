#include "dialogFreqAxisPreference.h"
#include "ui_dialogFreqAxisPreference.h"

#include <QMessageBox>

sweepStruct::sweepStruct()
{
  msaMode = 0;
  gentrk = 0;
  calCanUseAutoWait = 0;
  useAutoWait = 0;
  alternateSweep = 0;
  centfreq = 0;
  sweepwidth = 0;
  startfreq = 0;
  endfreq = 0;
  baseFrequency = 0;
  globalSteps = 0;
  wate = 0;
  path = "";
  doSpecialGraph = 0;
  videoFilter = "";
  spurcheck = 0;
  normrev = 0;
  sgout = 0;
  offset = 0;
  switchFR = 0;
  userFreqPref = 0;
  planeadj = 0;
  prevPlaneAdj = 0;
  freqBand = 0;
  LO2 = 0;
  autoWaitPrecision = "";


  MSANumFilters = 0;
  MSAFiltStrings.clear();
  videoFilterNames.clear();
  TGtop = 0;
  appxLO2 = 0;
  invdeg = 0;

  S11GraphR0 = 0;
  prevS11GraphR0 = 0;
  S11JigType = "";
  S21JigAttach = "";

  appearances.clear();
  lastPresetColor = "";
  customPresetNames.clear();

  sweepDir = 0;
  XIsLinear = 0;
  HorDiv = 0;
  VertDiv = 0;
  refreshEachScan = 0;
  displaySweepTime = 0;

}


dialogFreqAxisPreference::dialogFreqAxisPreference(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogFreqAxisPreference)
{
  allowClose = false;
  ui->setupUi(this);
  int  x, y, h1, h2;
  x = parent->x();
  y = parent->y();
  h1 = parent->height();
  h2 = height();
  move(x,y + h1 - h2);
}

dialogFreqAxisPreference::~dialogFreqAxisPreference()
{
  delete ui;
}
int dialogFreqAxisPreference::DisplayAxisXPreference(sweepStruct *config)
{
  cfg = config;
  QStringList specialGraphs;
  specialGraphs << "0(Normal Operation)";
  specialGraphs << "1(Graph Mag Cal)";
  specialGraphs << "2(Graph Freq Cal)";
  specialGraphs << "3(Graph Noisy Sine)";
  specialGraphs << "4(Graph 1MHz Peak)";
  if (cfg->msaMode == modeSA)
  {
    specialGraphs << "5(1MHz Square Wave)";
  }
  else
  {
    specialGraphs << "5(Simulate RLC/Coax)";
  }
  ui->OpMode->clear();
  ui->OpMode->addItems(specialGraphs);
  ui->FiltList->addItems(cfg->MSAFiltStrings);

  ui->VideoFilt->addItems(cfg->videoFilterNames);

  ui->Appearance->clear();
  ui->Appearance->addItems(cfg->appearances);

  ui->trackingGroup->setVisible(false);
  ui->freqoffbox->setVisible(false);
  ui->sigGenLab->setVisible(false);
  ui->sigFreqLab->setVisible(false);

  if (cfg->msaMode == modeSA)
  {
    if (cfg->gentrk == 1)
    {
      ui->trackingGroup->setVisible(true);
      ui->freqoffbox->setVisible(true);
    }
    else
    {
      if (cfg->TGtop > 0)
      {
        ui->freqoffbox->setVisible(true);
        ui->sigGenLab->setVisible(true);
        ui->sigFreqLab->setVisible(true);
      }
    }
  }
  if (cfg->msaMode != modeSA && cfg->msaMode != modeScalarTrans)  //modes with phase
  {
    ui->pdminvert->setVisible(true);
    ui->invdegbox->setVisible(true);
    ui->planeadj->setVisible(true);
  }
  else
  {
    ui->pdminvert->setVisible(false);
    ui->invdegbox->setVisible(false);
    ui->planeadj->setVisible(false);
  }
  //Graph R0
  if (cfg->msaMode == modeRefelection)
  {
    ui->R0->setVisible(true);
    ui->R0Lab1->setVisible(true);
    ui->R0Lab2->setVisible(true);
  }
  else
  {
    ui->R0->setVisible(false);
    ui->R0Lab1->setVisible(false);
    ui->R0Lab2->setVisible(false);
  }
  if (cfg->msaMode != modeSA)
  {
    ui->DirectionF->setVisible(true);
    ui->DirectionR->setVisible(true);
  }
  else
  {
    ui->DirectionF->setVisible(false);
    ui->DirectionR->setVisible(false);
  }
  QStringList waitPrecisions;
  waitPrecisions << "Fast";
  waitPrecisions << "Normal";
  waitPrecisions << "Precise";
  ui->waitPrecision->clear();
  ui->waitPrecision->addItems(waitPrecisions);

  QStringList FreqModes;
  int IF1 = cfg->appxLO2-10.7;   //Approx. IF1 frequency; assumes 10.7 MHz final filter

  FreqModes << "Auto";
  FreqModes << "1G; approx. 0-" + QString::number(IF1) + " MHz";
  FreqModes << "2G; approx. " + QString::number(IF1-100) + "-" + QString::number(2*IF1+100) + " MHz";
  FreqModes << "3G; approx. " + QString::number(2*IF1+2) + "-" + QString::number(3*IF1) + " MHz";

  ui->FreqMode->clear();
  ui->FreqMode->addItems(FreqModes);

  QStringList NumHorDiv;
  NumHorDiv << "4";
  NumHorDiv << "6";
  NumHorDiv << "8";
  NumHorDiv << "10";
  NumHorDiv << "12";
  ui->NDiv->clear();
  ui->NDiv->addItems(NumHorDiv);

  //now the other init stuff
  if (cfg->calCanUseAutoWait)
  {
    cfg->useAutoWait = 0;
  }

  if (cfg->useAutoWait)
  {
    ui->WaitLab->setVisible(false);
    ui->SweepWait->setVisible(false);
    ui->waitPrecision->setVisible(true);
    ui->cbAutoWait->setChecked(true);
  }
  else
  {
    ui->WaitLab->setVisible(true);
    ui->SweepWait->setVisible(true);
    ui->waitPrecision->setVisible(false);
    ui->cbAutoWait->setChecked(false);
  }
  if (cfg->alternateSweep)
  {
    ui->Alternate->setChecked(true);
  }
  else
  {
    if (cfg->sweepDir==1)
    {
      ui->LR->setChecked(true);
    }
    else
    {
      ui->RL->setChecked(true);
    }
  }
  if (cfg->XIsLinear)
  {
    ui->linear->setChecked(true);
  }
  else
  {
    ui->log->setChecked(true);
    ui->DivLab1->setVisible(false);
    ui->NDiv->setVisible(false);
  }
  QString xForm = "#####.######";
  ui->SweepCent->setText(util.uFormatted(cfg->centfreq,xForm));
  ui->SweepSpan->setText(util.uFormatted(cfg->sweepwidth, xForm));
  ui->SweepStart->setText(util.uFormatted(cfg->startfreq, xForm));
  ui->SweepStop->setText(util.uFormatted(cfg->endfreq, xForm));
  ui->baseFreq->setText(util.uFormatted(cfg->baseFrequency, xForm));
  ui->SweepSteps->setText(QString::number(cfg->globalSteps));

  ui->FreqMode->setCurrentIndex(cfg->freqBand); //freqBand is 0(auto),1, 2 or 3
  ui->FreqMode->setFocus();
  ui->SweepWait->setText(QString::number(cfg->wate));

  ui->NDiv->setCurrentIndex(ui->NDiv->findText(QString::number(cfg->HorDiv)));

  int filtIndex=util.Word(cfg->path, 2).toInt();
  ui->FiltList->setCurrentIndex(filtIndex-1);

  ui->OpMode->setCurrentIndex(cfg->doSpecialGraph);

  ui->VideoFilt->setCurrentIndex(ui->VideoFilt->findText(cfg->videoFilter));

  //We want to select the last preset that was used. If it is in the form CustomN, we want to
  //translate it to the name the user gave it when it was defined. ver115-2a
  QString lastColorPreset=cfg->lastPresetColor;
  if (lastColorPreset.left(6).toUpper()=="CUSTOM")
  {
    int customPresetNum=lastColorPreset.mid(7).toInt();
    lastColorPreset=cfg->customPresetNames[customPresetNum];
  }
  ui->Appearance->setCurrentIndex(ui->Appearance->findText(lastColorPreset));

  if (cfg->refreshEachScan)
  {
    ui->Refresh->setChecked(true);
  }
  else
  {
    ui->Refresh->setChecked(false);
  }
  if (cfg->displaySweepTime)
  {
    ui->SweepTime->setChecked(true);
  }
  else
  {
    ui->SweepTime->setChecked(false);
  }

  changeAppearance=0;

  if (cfg->spurcheck==1)
  {
    ui->spurbox->setChecked(true);
  }
  else
  {
    ui->spurbox->setChecked(false);
  }

  if (cfg->msaMode==modeSA)
  {
    if (cfg->TGtop == 2) //print proper label; button only exists for SA mode with TGtop=2
    {
      if (cfg->normrev == 1)
      {
        ui->normReverse->setText("Reverse");
      }
    }
    if (cfg->TGtop>0)
    {
      //Do if we have the TG--print either SG freq or TG offset
      if (cfg->gentrk==0)
      {
        ui->freqoffbox->setText(QString::number(cfg->sgout));
      }
      else
      {
        ui->freqoffbox->setText(QString::number(cfg->offset));
      }
    }
  }
  else
  {
    if (cfg->switchFR==0)
    {
      ui->DirectionF->setChecked(true);
      ui->DirectionR->setChecked(false);
    }
    else
    {
      ui->DirectionF->setChecked(false);
      ui->DirectionR->setChecked(true);
    }
    if (cfg->msaMode != modeScalarTrans)   //modes with phase
    {
      ui->invdegbox->setText(QString::number(cfg->invdeg));
      ui->planeadjbox->setText(QString::number(cfg->planeadj));
    }
  }

      //R0 for Reflection
  if (cfg->msaMode == modeRefelection)
  {
    ui->R0->setText(util.uFormatted(cfg->S11GraphR0,"3,4,5//UseMultiplier//DoCompact//SuppressMilli"));
  }
  if (cfg->userFreqPref==0)  //This is based on last user setting
  {
    ui->btnCentSpan->setChecked(true);    //Start in center/span mode
    enableCentSpan();
  }
  else
  {
    ui->btnStartStop->setChecked(true);   //Start in start/stop mode
    enableStartStop();
  }

  exec();

  return closeValue;
}

void dialogFreqAxisPreference::closeEvent(QCloseEvent *e)
{
  if (!allowClose)
    e->ignore();
}
/*
void dialogFreqAxisPreference::FreqAxisPreference()
{
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

}
*/

void dialogFreqAxisPreference::setDUTForward(bool checked)
{
  ui->DirectionF->setChecked(checked);
  ui->DirectionR->setChecked(!checked);
}

void dialogFreqAxisPreference::setDUTReverse(bool checked)
{
  ui->DirectionF->setChecked(!checked);
  ui->DirectionR->setChecked(checked);
}
void dialogFreqAxisPreference::setAutoWait()
{
  ui->WaitLab->setVisible(false);
  ui->SweepWait->setVisible(false);
  ui->waitPrecision->setVisible(true);
}
void dialogFreqAxisPreference::clearAutoWait()
{
  ui->WaitLab->setVisible(true);
  ui->SweepWait->setVisible(true);
  ui->waitPrecision->setVisible(false);
 }
void dialogFreqAxisPreference::axisLRon()
{
  ui->RL->setChecked(false);
  ui->Alternate->setChecked(false);
}
void dialogFreqAxisPreference::axisLRoff()
{
  ui->LR->setChecked(true);
}
void dialogFreqAxisPreference::axisRLon()
{
  ui->LR->setChecked(false);
  ui->Alternate->setChecked(false);
}
void dialogFreqAxisPreference::axisRLoff()
{
  ui->RL->setChecked(true);
}
void dialogFreqAxisPreference::axisALTon()
{
  ui->RL->setChecked(false);
  ui->LR->setChecked(false);
}
void dialogFreqAxisPreference::axisALToff()
{
  ui->Alternate->setChecked(true);
}
void dialogFreqAxisPreference::axisXSelLinear()
{
  //Start with 10 divisions when switching from log to linear ver114-2c
  ui->DivLab1->setVisible(true);
  ui->NDiv->setVisible(true);
  ui->NDiv->setCurrentIndex(ui->NDiv->findText("10"));
  ui->log->setChecked(false);
  ui->linear->setChecked(true);
}
void dialogFreqAxisPreference::axisXSelLog()
{
  //Can't choose number of divisions for log sweep ver114-5p
  ui->NDiv->setVisible(false);
  ui->linear->setChecked(false);
  ui->log->setChecked(true);
  if (!ui->btnStartStop->isChecked())
  {
    ui->btnStartStop->setChecked(true);
    setStartStop();  //Log should be in start/stop mode
    return;
  }
}
void dialogFreqAxisPreference::setCentSpan()
{
  //Select Center/Span mode; use Start/Stop to fill in Center/Span
  //embedded in DisplayAxisXPreference
  ui->btnStartStop->setChecked(false);
  ui->btnCentSpan->setChecked(true);
  float currStart = util.uCompact(ui->SweepStart->text()).toFloat();
  float currStop = util.uCompact(ui->SweepStop->text()).toFloat();
  //currStart=val(uCompact$(currStart$)) : currStop=val(uCompact$(currStop$))
  ui->SweepCent->setText(util.usingF("#####.######",(currStart+currStop)/2));    //Enter center
  ui->SweepSpan->setText(util.usingF("#####.######",currStop-currStart));       //Enter span
  enableCentSpan();
}
void dialogFreqAxisPreference::enableCentSpan()
{
  ui->SweepCent->setEnabled(true);
  ui->SweepSpan->setEnabled(true);
  ui->SweepStart->setEnabled(false);
  ui->SweepStop->setEnabled(false);
  ui->SweepCent->setFocus();
  ui->SweepCent->selectAll();
}
void dialogFreqAxisPreference::setStartStop()
{
  //Select Start/Stop mode; use Center/Span to fill in Start/Stop
  //embedded in DisplayAxisXPreference
    ui->btnCentSpan->setChecked(false);
    ui->btnStartStop->setChecked(true);
    float currCent = util.uCompact(ui->SweepCent->text()).toFloat();
    float currSpan = util.uCompact(ui->SweepSpan->text()).toFloat();
    //currCent=val(util.uCompact$(currCent$)) : currSpan=val(uCompact$(currSpan$))
    ui->SweepStart->setText(util.usingF("#####.######",currCent-currSpan/2));    //Enter start
    ui->SweepStop->setText(util.usingF("#####.######",currCent+currSpan/2));    //Enter stop
    enableStartStop();
}
void dialogFreqAxisPreference::enableStartStop()
{
  ui->SweepCent->setEnabled(false);
  ui->SweepSpan->setEnabled(false);
  ui->SweepStart->setEnabled(true);
  ui->SweepStop->setEnabled(true);
  ui->SweepStart->setFocus();
  ui->SweepStart->selectAll();
}
void dialogFreqAxisPreference::axisXCancel()
{
  //User has cancelled; 'embedded in DisplayAxisXPreference
  // fix me axisPrefHandle$=""
  closeValue=0;   //signals no restart needed
  cancelled = true;
  allowClose = true;
  close();
}
void dialogFreqAxisPreference::axisSetupNOP()
{/*    wait
*/
}
//ver115-2b added void dialogFreqAxisPreference::RecalcPlaneExt() modver115-2d
void dialogFreqAxisPreference::RecalcPlaneExtAndR0()
{
  // button to Recalculate and draw data with new plane extension or new S11 graph R0
  //R0 transform is done only for reflection mode
  float planeadj=util.uCompact(ui->planeadjbox->text()).toFloat();  //Get new planeadj
  if (cfg->msaMode==modeRefelection)
  {
    cfg->S11GraphR0=util.uValWithMult(util.uCompact(ui->R0->text()));  //Get R0
  }
  VerifyPlaneExtension();    //See if we can do plane ext.
  ui->planeadjbox->setText(QString::number(planeadj));     //In case it got zeroed
  //fix me RecalcPlaneExtendAndR0AndRedraw(); //Do actual recalc and redraw
  cfg->prevPlaneAdj=planeadj;   //because we are making the adjustment here; DetectChanges doesn't have to do anything
  cfg->prevS11GraphR0=cfg->S11GraphR0;
}
void dialogFreqAxisPreference::VerifyPlaneExtension()
{
  //See if we can do plane extension ver115-4j
  if (cfg->planeadj!=0 && cfg->msaMode==modeRefelection)
  {
    if (cfg->S11JigType!="Reflect")      //Reflect means bridge
    {
      if (cfg->S21JigAttach=="Shunt")
      {
        QMessageBox::information(this, "Notice", "Can't do plane extension for shunt fixture.");
        cfg->planeadj=0;   //we could but we don't
      }
      else
      {
        QMessageBox::information(this, "Notice", "Can't do plane extension for series fixture in Reflection Mode.");
        cfg->planeadj=0;
      }
    }
  }
  //Note: in transmission mode, plane extension with the shunt fixture requires fancier calculations
  //than we do, but we can//t be sure what fixture is being used so we can//t warn.
}

void dialogFreqAxisPreference::axisSetupSelectAppearance()
{
  //Select graph appearance. Invoked by user click on combo box
  //Note that unless the user changes the appearance, we do not activate the selection upon
  //exit. This is because it will override previously selected trace colors.
  changeAppearance=1;
}

void dialogFreqAxisPreference::NormRevbutton()
{
  //when in Tracking Mode, selects either Normal or Reverse tracking 'ver111-17
  //this button does not exist in old TG topology. Only in new TG topology.
  if (cfg->normrev == 0)
  {
    ui->normReverse->setText("Reverse");
    cfg->normrev = 1;
    return;
  }
  if (cfg->normrev == 1)
  {
    ui->normReverse->setText("Normal");
    cfg->normrev = 0;
    return;
  }
}

void dialogFreqAxisPreference::axisXFinished()
{
 //Window is being closed; record choices; //embedded in DisplayAxisXPreference
    //ver115-2b moved planeadj here so any necessary redraw can be done prior to anything else changing.
    //If planeadj and other items are changed, there may be a superfluous redraw here.
  cfg->globalSteps = ui->SweepSteps->text().toInt(); //Get user specified number of steps

  /*
   move to main function
   if globalSteps<>prevSteps then
            //SEWgraph2 Note we set globalSteps here; steps is set from globalSteps
            //after we exit back to [FreqAxisPreference]
        if globalSteps<1 then globalSteps=1 else if globalSteps>maxNumSteps then globalSteps=maxNumSteps    //ver115-1b
        if globalSteps>2000 then    //ver116-4k
            confirm "Processing with more than 2,000 steps will be slow. Proceed anyway?"; stepsAns$
            if stepsAns$="no" then #axis.SweepSteps, 2000 : wait
        end if
        //Note we are called by [FreqAxisPreference], which will transfer globalSteps to steps.
        //Resize arrays if necessary for all steps and a few to spare
        if globalSteps>=gMaxNumPoints() then call ResizeArrays globalSteps+10      //ver114-2a
        call gSetNumDynamicSteps globalSteps    //Tell graph module ver114-1f
    end if
*/
  int R0Changed=0;
  if (cfg->msaMode==modeRefelection)
  {
    float newR0= util.uValWithMult(ui->R0->text());
    if (newR0<0)
    {
      QMessageBox::warning(this, "Notice", "Invalid Graph R0. 50 ohms used.");
      newR0=50;
      ui->R0->setText("50");
    }
    if (cfg->S11GraphR0!=newR0)
    {
      R0Changed=1;
    }
    cfg->S11GraphR0=newR0;
  }

        //ver115-4j moved retrieval of doSpecialGraph before doing plane adj.
  cfg->doSpecialGraph = ui->OpMode->currentIndex();
  /*if (doSpecialGraph<1)
    {
      doSpecialGraph=0;
    }
    else
    {
      doSpecialGraph=doSpecialGraph-1; //e.g. selection 1 is doSpecialGraph=0
    }*/
  if (cfg->doSpecialGraph>0 && cfg->doSpecialGraph!=5 && cfg->msaMode==modeVectorTrans)
  {
    cfg->S21JigAttach="Series";
  }

  if (cfg->msaMode==modeVectorTrans || cfg->msaMode==modeRefelection)
  {
    cfg->planeadj=util.uCompact(ui->planeadjbox->text()).toFloat();
    VerifyPlaneExtension();    //See if we can do plane ext. ver115-4j
    if (cfg->planeadj!=cfg->prevPlaneAdj || R0Changed)     //prevPlaneAdj was saved by RememberState
    {
      // fix me RecalcPlaneExtendAndR0AndRedraw(); //Redo planeadj adjustment from intermediate data and redraw graph
      cfg->prevPlaneAdj=cfg->planeadj;   //because we are making the adjustment here; DetectChanges doesn//t have to do anything
      cfg->prevS11GraphR0=cfg->S11GraphR0;
    }
  }

  //DUT Direction
  if (cfg->msaMode!=modeSA)
  {
    if (ui->DirectionF->isChecked())
    {
      cfg->switchFR=0;
    }
    else
    {
      cfg->switchFR=1;
    }
  }

  if (ui->Alternate->isChecked())
  {
    cfg->alternateSweep=1;
  }
  else
  {
    cfg->alternateSweep=0;
  }
  int dir;
  if (cfg->alternateSweep)
  {
    dir=1;   //if alternating, start with forward
  }
  else
  {
    if (ui->LR->isChecked())
    {
      dir=1;
    }
    else
    {
      dir=-1;
    }//set forward or reverse
  }
  cfg->sweepDir = dir;

  cfg->freqBand = ui->FreqMode->currentIndex();
  if (cfg->freqBand<0)
  {
    cfg->freqBand=0;
  }
  if (cfg->freqBand>3)
  {
    cfg->freqBand=3;
  }

  //Get base freq. Note that graph module knows nothing about this; it has an effect only when
  //commanding hardware and applying frequency calibration.

  cfg->baseFrequency=(int)((util.uCompact(ui->baseFreq->text())).toFloat() +0.5 );   //round to Hz; Not perfect for negative, but OK
  if (cfg->baseFrequency<0)
  {
    QMessageBox::warning(this, "Notice", "Base Frequency cannot be negative; zero Hz used.");
    cfg->baseFrequency=0;
  }
  //Get frequency range
  if (ui->btnCentSpan->isChecked())
  {
    //Use Center/Span to determine centfreq, sweepwidth, startfreq and endfreq
    cfg->userFreqPref=0;  //Save as user preference ver115-1d
    //uCompact deletes spaces, which can mess up negative numbers
    float temp1 = util.uCompact(ui->SweepCent->text()).toFloat();
    float temp2 = util.uCompact(ui->SweepSpan->text()).toFloat();
    //Enter new freq, but only if there is a material change. Otherwise,
    //tiny differences between using center/span and start/stop will trigger a restart.  //ver115-1b
    if (fabs(temp1-cfg->centfreq) > 1e-12 || fabs(temp2-cfg->sweepwidth)>1e-12)
    {
      SetCenterSpanFreq(temp1, temp2); //new center and span
    }
  }
  else
  {
    //Use Start/Stop to determine centfreq, sweepwidth, startfreq and endfreq
    cfg->userFreqPref=1;  //Save as user preference ver115-1d
    //uCompact deletes spaces, which can mess up negative numbers
    float temp1 = util.uCompact(ui->SweepStart->text()).toFloat();
    float temp2 = util.uCompact(ui->SweepStop->text()).toFloat();
    //Enter new freq, but only if there is a material change.
    if (fabs(temp1-cfg->startfreq) > 1e-12 || fabs(temp2-cfg->endfreq) > 1e-12)
    {
      SetStartStopFreq(temp1, temp2); //new center and span
    }
  }

  //We retrieve both the manual wait time and auto wait precision,
  //even though user only wants to use one at a time.  //ver116-1b
  cfg->wate = ui->SweepWait->text().toInt();
  if (cfg->wate<0)
  {
    cfg->wate=0;
  }
  cfg->autoWaitPrecision = ui->waitPrecision->currentText();
  if (ui->cbAutoWait->isChecked())
  {
    cfg->useAutoWait=1;
  }
  else
  {
    cfg->useAutoWait=0;
  }

  //Set sweep to log or linear. We do this after first setting the new range, since log sweep
  //gets automatically changed to linear if the span is too small.
  int linearF;
  if (ui->linear->isChecked())
  {
    linearF=1;
  }
  else
  {
    linearF=0;
  }
  if (linearF==0)
  {
    //ver116-4k:for log sweeps, X range can be negative and can include zero, but cannot cross zero.
    //If it includes zero, that value will be changed to a small value based on the "blackHoleRadius".
    if (cfg->startfreq<0 && cfg->endfreq>0)
    {
      QMessageBox::warning(this, "Notice", "Log sweep cannot cross zero Hz. Range changed.");
      cfg->endfreq=qMax(cfg->endfreq, (float)1);
      SetStartStopFreq(cfg->endfreq/100000, cfg->endfreq);
    }
    bool includesZero=(cfg->startfreq<=0 && cfg->endfreq>=0);
    bool allNegative=(cfg->startfreq<0 && cfg->endfreq<0);
    if (includesZero) //includes but does not cross zero
    {
      if (cfg->startfreq==0)
      {
        if (cfg->endfreq==0)   //both limits are zero
        {
          linearF=1;
          QMessageBox::warning(this, "Notice", "Changed to linear sweep because span is small.");
          ui->NDiv->setCurrentIndex(ui->NDiv->findText("10")); //set to 10 divisions
        }
        else //starts at zero; ends above zero
        {
          QMessageBox::warning(this, "Notice", "0 Hz not allowed in log sweep; range changed");
          SetStartStopFreq(0.0001, qMax((float)0.001, cfg->endfreq));
        }
      }
      else    //starts below 0; ends at 0
      {
        QMessageBox::warning(this, "Notice", "0 Hz not allowed in log sweep; range changed");
        SetStartStopFreq(qMin((float)-0.001, cfg->startfreq), (float)-0.0001);
      }
    }
    else    //all positive or all negative
    {
      float span;
      if (allNegative)
      {
        span=util.uSafeLog10(cfg->startfreq/cfg->endfreq);
      }
      else
      {
        span=util.uSafeLog10(cfg->endfreq/cfg->startfreq);
      }
      if (span <0.7)
      {
        linearF=1;
        QMessageBox::warning(this, "Notice", "Changed to linear sweep because span is small.");
        ui->NDiv->setCurrentIndex(ui->NDiv->findText("10"));    //set to 10 divisions
      }
      if (span>9)
      {
        if (allNegative)
        {
          SetStartStopFreq(cfg->startfreq, cfg->startfreq/1000000000);
        }
        else
        {
          SetStartStopFreq(cfg->endfreq/1000000000, cfg->endfreq);
          QMessageBox::warning(this, "Notice", "Log span cannot exceed 9 decades; sweep limits changed.");
        }
      }
    }
  }
  cfg->XIsLinear = linearF;    //Set freq linearity

  cfg->HorDiv = ui->NDiv->currentText().toInt();    //Get user specified number of divisions
  if (cfg->HorDiv<2)
  {
    cfg->HorDiv=2;
  }
  else if (cfg->HorDiv>12)
  {
    cfg->HorDiv=10;
  }
  cfg->HorDiv=2*(int)(cfg->HorDiv/2);    //Make the number even
  //Set new number of hor divisions, with old number of vert div
  //gSetNumDivisions(HorDiv,VertDiv);

  if (ui->Refresh->isChecked())
  {
    cfg->refreshEachScan=1;
  }
  else
  {
    cfg->refreshEachScan=0;
  }

  if (ui->SweepTime->isChecked())
  {
    cfg->displaySweepTime=1;
  }
  else
  {
    cfg->displaySweepTime=0;
  }

  if (changeAppearance)
  {
    /*
       *fix me
      QString graphAppearance = ui->Appearance->currentText();
        if (graphAppearance != "DARK" && graphAppearance != "LIGHT")
        {
            //We are using a custom color set, and must create a name in the form "CustomN"
            QString customName="";
            for i=1 to 5
                thisPreset$=customPresetNames$(i)
                if thisPreset$=graphAppearance$ then customName$="Custom";i : exit for
            next i
            if customName$="" then graphAppearance$="DARK" else graphAppearance$=customName$
        }
        call gUsePresetColors graphAppearance$  //Set color scheme
        call SetCycleColors //ver116-4s
        #axis.OK, "!setfocus"   //Take focus off combo box to protect from scroll wheel
        */
  }
  /*
    #axis.FiltList, "selectionindex? filtIndex"   //Filter N is at index N in list
    if filtIndex=0 then //filtIndex can be 0 if user typed something in the combo box
        filtIndex=1
        #axis.FiltList, "select ";MSAFiltStrings$(0)   //Select default
        #axis.FiltList, "setfocus"  //SEW2  Needed to activate the highlight
    end if
    */
  cfg->path = "Path "+ QString::number(ui->FiltList->currentIndex()+1); //Name of filter path; Path 1, ver113-7c
  //RBW filter will actually be selected after we return and load the RBW path cal data

  //#axis.VideoFilt, "contents? videoFilter$" //get selected video filter  ver114-5p
  cfg->videoFilter = ui->VideoFilt->currentText();
  //Note DetectChanges will take care of actually selecting the video filter

  if (ui->spurbox)
  {
    cfg->spurcheck=1;
  }
  else
  {
    cfg->spurcheck=0;
  }

  if (cfg->msaMode==modeSA)
  {
    QString freqoff;
    if (cfg->TGtop > 0)
    {
      freqoff = ui->freqoffbox->text();   //Sig Gen Freq or TG offset box data
    }
    if (cfg->gentrk == 0)
    {
      cfg->sgout = util.usingF("####.######",util.uCompact(freqoff).toFloat()).toFloat();
      if (cfg->sgout<-75)
      {
        QMessageBox::warning(this, "Notice", "LO3 for Sig Gen Frequency may be too low");
      }
      if (cfg->sgout>3*cfg->LO2)
      {
        QMessageBox::warning(this, "Notice", "LO3 for Sig Gen Frequency may be too high");
      }
    }
    if (cfg->gentrk == 1)
    {
      cfg->offset = util.usingF("####.######",util.uCompact(freqoff).toFloat()).toFloat();
    }
  }
  if (cfg->msaMode == modeVectorTrans ||  cfg->msaMode == modeRefelection)  //modes with phase
  {
    cfg->invdeg = util.uCompact(ui->invdegbox->text()).toInt();
  }

  if (cfg->doSpecialGraph==5 && cfg->msaMode!=modeSA)
  {
    /*
       *fix me
        //Simultated RLC/Transmission line data--give user chance to change
        int parseErr=util.uParseRLC(doSpecialRLCSpec, DialogRLCConnect, DialogRValue, DialogLValue,
                            DialogCValue, DialogQLValue, DialogQCValue, dumD, DialogCoaxSpecs);
        if parseErr then
            doSpecialRLCSpec$="RLC[S,R0,L0,C";constMaxValue;",QL10000,QC10000]"    //ver115-4b
            DialogRLCConnect$="S" : DialogRValue=0 : DialogLValue=0
            DialogQLValue=10000 : DialogQCValue=10000
            DialogCValue=constMaxValue
            DialogCoaxSpecs$=""  //ver115-4a
        end if
        DialogCoaxName$=doSpecialCoaxName$  //ver115-4b
        close #axis : #handle, "disable"   //ver116-4k
        call RLCDialog  //Get desired circuit values
        #handle, "enable"   //ver116-4k
        //Assemble the values into a spec string
                //Get the new values; assemble them into spec
        doSpecialCoaxName$=DialogCoaxName$  //ver115-4b
        form$="3,3,4//UseMultiplier"
        resForm$="3,3,4//UseMultiplier//SuppressMilli" //ver115-4e
        QForm$="######.###"   //ver115-5f
        R$=uFormatted$(DialogRValue, resForm$)
        L$=uFormatted$(DialogLValue, form$)
        C$=uFormatted$(DialogCValue, form$)
        QL$=uFormatted$(DialogQLValue, QForm$) //ver115-4b
        QC$=uFormatted$(DialogQCValue, QForm$) //ver115-4b
        doSpecialRLCSpec$="RLC[";DialogRLCConnect$;",R";uCompact$(R$);",L";uCompact$(L$); ",C";uCompact$(C$); _
                        ",QL";uCompact$(QL$);",QC";uCompact$(QC$); "], Coax[";DialogCoaxSpecs$;"]"
            */
  }

  //    axisPrefHandle$=""
  /*if (continueCode==3)
  {
    closeValue=1;
  }
  else
  {
    continueCode=0;
    closeValue=0;
  }*/
}

void dialogFreqAxisPreference::on_DirectionF_clicked(bool checked)
{
  setDUTForward(checked);
}

void dialogFreqAxisPreference::on_DirectionR_clicked(bool checked)
{
  setDUTReverse(checked);
}

void dialogFreqAxisPreference::on_cbAutoWait_clicked(bool checked)
{
  if (checked)
  {
    setAutoWait();
  }
  else
  {
    clearAutoWait();
  }
}

void dialogFreqAxisPreference::on_LR_clicked(bool checked)
{
  if (checked)
  {
    axisLRon();
  }
  else
  {
    axisLRoff();
  }
}

void dialogFreqAxisPreference::on_RL_clicked(bool checked)
{
  if (checked)
  {
    axisRLon();
  }
  else
  {
    axisRLoff();
  }

}

void dialogFreqAxisPreference::on_Alternate_clicked(bool checked)
{
  if (checked)
  {
    axisALTon();
  }
  else
  {
    axisALToff();
  }
}

void dialogFreqAxisPreference::on_linear_clicked(bool checked)
{
  if (checked)
  {
    axisXSelLinear();
  }
  else
  {
    axisXSelLog();
  }
}

void dialogFreqAxisPreference::on_log_clicked(bool checked)
{
  if (checked)
  {
    axisXSelLog();
  }
  else
  {
    axisXSelLinear();
  }
}

void dialogFreqAxisPreference::on_btnCentSpan_clicked(bool checked)
{
  if (checked)
  {
    setCentSpan();
  }
}

void dialogFreqAxisPreference::on_btnStartStop_clicked(bool checked)
{
  if (checked)
  {
    setStartStop();
  }
}

void dialogFreqAxisPreference::on_pushButton_2_clicked()
{
  axisXCancel();
}
void dialogFreqAxisPreference::SetStartStopFreq(float startF, float stopF)
{
  //Use Start/Stop to determine centfreq, sweepwidth, startfreq and endfreq
  //A centralized routine is used so all these related variables can be kept in sync.
  //Note gCalcGraphParams must be called to fully update graph module
  cfg->startfreq=startF;
  cfg->endfreq=stopF;
  if (cfg->startfreq>cfg->endfreq)
  {
    float dum=cfg->startfreq;
    cfg->startfreq=cfg->endfreq;
    cfg->endfreq=dum;    //Swap to get in right order
  }
  cfg->centfreq=(util.usingF("####.######",(cfg->startfreq+cfg->endfreq)/2)).toFloat();
  cfg->sweepwidth=cfg->endfreq-cfg->startfreq;
  //gSetXAxisRange(vars->startfreq, vars->endfreq);

}
void dialogFreqAxisPreference::SetCenterSpanFreq(float cent, float span)
{

  //Use Center/Span to determine centfreq, sweepwidth, startfreq, endfreq
  //A centralized routine is used so all these related variables can be kept in sync.
  //Note gCalcGraphParams must be called to fully update graph module
  cfg->centfreq=cent;
  cfg->sweepwidth=span;
  if (cfg->sweepwidth<0)
    cfg->sweepwidth=0-cfg->sweepwidth;
  cfg->startfreq=(util.usingF("####.######",cfg->centfreq-cfg->sweepwidth/2)).toFloat();
  cfg->endfreq=(util.usingF("####.######",cfg->centfreq+cfg->sweepwidth/2)).toFloat();
  //ver115-1b deleted calc of stepfreq
//  gSetXAxisRange(cfg->startfreq, cfg->endfreq);  //ver114-6d

}
void dialogFreqAxisPreference::on_pushButton_clicked()
{
  axisXFinished();
  cancelled = false;
  allowClose = true;
  close();
}
