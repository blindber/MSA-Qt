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
#include "dialogconfigman.h"
#include "ui_dialogconfigman.h"
#include <math.h>
#include <QTextStream>



//===================================================================
//@===============Configuration Manager Module=======================
//===================================================================
//Configuration Manager Version 1.1, as of 4-16-09
//This module combines the user interface of the Configuration Manager and the
//underlying routines, which previously were a separate module.
//All the user needs to do is call configRunManager. The window will be opened and
//the existing config.txt file will be read. If that file or its folder, MSA_Info,
//does not exist, they will be created, using default values for the file.
//The user enters configuration data in the window. When the user reads or saves the
//file, the data is also loaded into the MSA variables, so upon completion those
//variables will reflect the state of the configuration file. If the user closes the
//window without saving the file, the data will reflect the state of the window,
//but the existing file will not be up to date.
//
//All variables, functions and subroutines in this module begin with the prefix "config".
//The only other variables affected are the MSA variables being configured. Those can
//be seen in configReadFile$().

dialogConfigMan::dialogConfigMan(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::dialogConfigMan)
{
  allowClose = false;
  ui->setupUi(this);


  DefaultDir = QApplication::applicationDirPath();
  moduleVersion=" 1.10";
  fileFullName = DefaultDir + "/MSA_Info/config.txt";

}

QString dialogConfigMan::configmoduleVersion()
{
  return moduleVersion;
}

QString dialogConfigMan::configFileFullName()
{
  return fileFullName;
}

dialogConfigMan::~dialogConfigMan()
{
  delete ui;
}

void dialogConfigMan::closeEvent(QCloseEvent *e)
{
  if (!allowClose)
    e->ignore();
}


void dialogConfigMan::load()
{
  blockChangeSignals(true);
  QStringList configPLLtypes;
  configPLLtypes <<  "0" << "2325" << "2326,4118"; //ver116-4k
  configPLLtypes << "2350" << "2353" << "4112,4113";  //ver116-4k

  ui->PLL1type->clear();
  ui->PLL2type->clear();
  ui->PLL3type->clear();

  ui->PLL1type->addItems(configPLLtypes);
  ui->PLL2type->addItems(configPLLtypes);
  ui->PLL3type->addItems(configPLLtypes);

  QStringList configPLLpol;
  configPLLpol << "0(invert)" << "1(non-inv)";  //ver116-4j

  ui->PLL1pol->clear();
  ui->PLL2pol->clear();
  ui->PLL3pol->clear();

  ui->PLL1pol->addItems(configPLLpol);
  ui->PLL2pol->addItems(configPLLpol);
  ui->PLL3pol->addItems(configPLLpol);

  QStringList configPLLmodes;
  configPLLmodes << "0(Integer)" << "1(Fract)";

  ui->PLL1mode->clear();
  ui->PLL3mode->clear();

  ui->PLL1mode->addItems(configPLLmodes);
  ui->PLL3mode->addItems(configPLLmodes);

  //DDS
  ui->DDS1freq->setStyleSheet( "background: cyan;" );
  ui->DDS3freq->setStyleSheet( "background: cyan;" );
  ui->DDS1bw->setStyleSheet( "background: cyan;" );
  ui->DDS3bw->setStyleSheet( "background: cyan;" );


  QStringList configParsers;
  configParsers << "0(parallel)" << "1(serial)";
  ui->DDS1parse->clear();
  ui->DDS1parse->addItems(configParsers);

  ui->mast->setStyleSheet( "background: cyan;" );


      //Implemented switches ver116-1b

//ver116-1b modified video filters
  ui->FiltWideMag->setStyleSheet( "background: cyan;" );
  ui->FiltWidePhase->setStyleSheet( "background: cyan;" );
  ui->FiltMidMag->setStyleSheet( "background: cyan;" );
  ui->FiltMidPhase->setStyleSheet( "background: cyan;" );
  ui->FiltNarrowMag->setStyleSheet( "background: cyan;" );
  ui->FiltNarrowPhase->setStyleSheet( "background: cyan;" );
  ui->FiltXNarrowMag->setStyleSheet( "background: cyan;" );
  ui->FiltXNarrowPhase->setStyleSheet( "background: cyan;" );
  ui->Inv->setStyleSheet( "background: cyan;" );

  QStringList configADCs;
  configADCs << "8(orig 8-bit)" << "12(ladder)" << "16(serial 16-bit)" << "22(serial 12-bit)";
  ui->adconv->clear();
  ui->adconv->addItems(configADCs);
                  //TG topology
  QStringList configTGtops;
  configTGtops << "0(None)" << "1(orig)" << "2(DDS3/PLL3)";
  ui->TGtop->clear();
  ui->TGtop->addItems(configTGtops);
                  //Control Board

  QStringList configControlBoards;
  configControlBoards << "0(Old)" << "1(Old, new harness)" << "2(SLIM original)" <<  "3(USB V1.0)"; //USB;01-08-2010
  ui->cb->clear();
  ui->cb->addItems(configControlBoards);

                    //Final Filters
  ui->filt->setStyleSheet( "background: cyan;" );
  ui->filtFreq->setStyleSheet( "background: cyan;" );
  ui->filtBW->setStyleSheet( "background: cyan;" );

                  //Port Address
  ui->LPT->setStyleSheet( "background: cyan;" );
  QStringList configLPTs;
  configLPTs << "Hex 378" << "Hex 175";
  ui->LPT->clear();
  ui->LPT->addItems(configLPTs);

  setWindowTitle("MSA/VNA Configuration Manager Version " + configVersion() );

  ui->PLL1type->setCurrentIndex(ui->PLL1type->findText("2326"));
  ui->PLL2type->setCurrentIndex(ui->PLL1type->findText("2326"));
  ui->PLL3type->setCurrentIndex(ui->PLL1type->findText("2326"));

  ui->LPT->setCurrentIndex(0);

  if (autoRun==1) ui->noSaveReturn->setVisible(false);

  configNumDisplayedFilters=0;
  configCreateLoadFile();    //Load file data. Creates one if necessary
  configDisplayData(&tempConfig);
  configAdjustDisplayedItems();
  blockChangeSignals(false);
}

void dialogConfigMan::configGetDisplayData()
{
  //Transfer data from display to MSA variables
  tempConfig.PLL1 = configPLLnumber(ui->PLL1type->currentText());
  tempConfig.PLL2 = configPLLnumber(ui->PLL2type->currentText());
  tempConfig.PLL3 = configPLLnumber(ui->PLL3type->currentText());

  tempConfig.PLL1phasepolarity = ui->PLL1pol->currentIndex();
  tempConfig.PLL2phasepolarity = ui->PLL2pol->currentIndex();
  tempConfig.PLL3phasepolarity = ui->PLL3pol->currentIndex();

  tempConfig.PLL1phasefreq = ui->PLL1Ref->text().toFloat();
  tempConfig.PLL2phasefreq = ui->PLL2Ref->text().toFloat();
  tempConfig.PLL3phasefreq = ui->PLL3Ref->text().toFloat();

  tempConfig.PLL1mode = ui->PLL1mode->currentIndex();
  tempConfig.PLL3mode = ui->PLL3mode->currentIndex();

  tempConfig.appxdds1 = ui->DDS1freq->text().toFloat();
  tempConfig.appxdds3 = ui->DDS3freq->text().toFloat();

  tempConfig.dds1filbw = ui->DDS1bw->text().toFloat();
  tempConfig.dds3filbw = ui->DDS3bw->text().toFloat();

  tempConfig.dds1parser = ui->DDS1parse->currentIndex();

  tempConfig.appxLO2 = ui->LO2->text().toFloat();

  tempConfig.masterclock = ui->mast->text().toFloat();

  tempConfig.maxpdmout = ui->PDM->text().toInt();
  tempConfig.invdeg = ui->Inv->text().toFloat();

  tempConfig.switchHasRBW = ui->switchRBW->isChecked() ? 1 : 0;
  tempConfig.switchHasVideo = ui->switchVideo->isChecked() ? 1 : 0;
  tempConfig.switchHasBand = ui->switchBand->isChecked() ? 1 : 0;
  tempConfig.switchHasTR = ui->switchTR->isChecked() ? 1 : 0;
  tempConfig.switchHasFR = ui->switchFR->isChecked() ? 1 : 0;

  float magCap;
  float phaseCap;

  magCap = ui->FiltWideMag->text().toFloat();
  phaseCap = ui->FiltWidePhase->text().toFloat();

  //Must have wide filter, so if cap is zero make it the default value
  if (magCap<=0)
    magCap=0.002;
  if (phaseCap<=0)
    phaseCap=0.011;
  tempConfig.videoFilterNames[1]="Wide";
  tempConfig.videoFilterCaps[1][0]=magCap;
  tempConfig.videoFilterCaps[1][1]=phaseCap;

  magCap = ui->FiltMidMag->text().toFloat();
  phaseCap = ui->FiltMidPhase->text().toFloat();

  if (magCap<=0 || phaseCap<=0)
    tempConfig.videoFilterNames[2]="";
  else
    tempConfig.videoFilterNames[2]="Mid";
  tempConfig.videoFilterCaps[2][0]=magCap;
  tempConfig.videoFilterCaps[2][1]=phaseCap;

  magCap = ui->FiltNarrowMag->text().toFloat();
  phaseCap = ui->FiltNarrowPhase->text().toFloat();

  if (magCap<=0 || phaseCap<=0)
    tempConfig.videoFilterNames[3]="";
  else
    tempConfig.videoFilterNames[3]="Narrow";
  tempConfig.videoFilterCaps[3][0]=magCap;
  tempConfig.videoFilterCaps[3][1]=phaseCap;

  magCap = ui->FiltXNarrowMag->text().toFloat();
  phaseCap = ui->FiltXNarrowPhase->text().toFloat();
  if (magCap<=0 || phaseCap<=0)
    tempConfig.videoFilterNames[4]="";
  else
    tempConfig.videoFilterNames[4]="XNarrow";
  tempConfig.videoFilterCaps[4][0]=magCap;
  tempConfig.videoFilterCaps[4][1]=phaseCap;

  int i = ui->adconv->currentIndex();
  switch (i)
  {
  case 0:
    tempConfig.adconv=8;
    break;
  case 1:
    tempConfig.adconv=12;
    break;
  case 2:
    tempConfig.adconv=16;
    break;
  default:
    tempConfig.adconv=22;
    break;
  }

  QString port = ui->LPT->currentText();
  bool ok;
  tempConfig.globalPort=util.Word(port,2).toInt(&ok, 16);    //e.g. port$ is "Hex 378")
  configClearFilters(&tempConfig);
  float freq=0, bw=0;
  for (int ii = 0; ii < ui->filt->count(); ii++)
  {
    QString thisFilt = ui->filt->item(ii)->text();
    freq=util.Word(thisFilt,1).toFloat();    //first word is frequency
    bw=util.Word(thisFilt,2).toFloat();  //bandwidth is second word
    configAddFilter(freq, bw, &tempConfig);
    if (ii == 0)
    {
      tempConfig.finalfreq=freq;
      tempConfig.finalbw=bw;
    }
  }

  tempConfig.TGtop = ui->TGtop->currentIndex();
  tempConfig.cb = ui->cb->currentIndex();
  //Original control board can only handle the RBW switch.
  if (tempConfig.cb==0)
  {
    tempConfig.switchHasVideo=0;
    tempConfig.switchHasBand=0;
    tempConfig.switchHasTR=0;
    tempConfig.switchHasFR=0;
  }
}

void dialogConfigMan::configDisplayData(msaConfig *tConfig)
{

  //Enter data from MSA variables into window
  ui->PLL1type->setCurrentIndex(ui->PLL1type->findText(configPLLlabel(tConfig->PLL1)));
  ui->PLL2type->setCurrentIndex(ui->PLL2type->findText(configPLLlabel(tConfig->PLL2)));

  ui->PLL1pol->setCurrentIndex(tConfig->PLL1phasepolarity);
  ui->PLL2pol->setCurrentIndex(tConfig->PLL2phasepolarity);

  ui->PLL1Ref->setText(QString::number(tConfig->PLL1phasefreq));
  ui->PLL2Ref->setText(QString::number(tConfig->PLL2phasefreq));


  ui->PLL1mode->setCurrentIndex(tConfig->PLL1mode);

  ui->DDS1freq->setText(QString::number(tConfig->appxdds1));

  ui->DDS1bw->setText(QString::number(tConfig->dds1filbw));

  ui->DDS1parse->setCurrentIndex(tConfig->dds1parser);
  ui->LO2->setText(QString::number(tConfig->appxLO2));
  ui->mast->setText(QString::number(tConfig->masterclock));

  //ver116-1b added switches
  ui->switchRBW->setChecked((bool)tConfig->switchHasRBW);
  ui->switchVideo->setChecked((bool)tConfig->switchHasVideo);
  ui->switchBand->setChecked((bool)tConfig->switchHasBand);
  ui->switchTR->setChecked((bool)tConfig->switchHasTR);
  ui->switchFR->setChecked((bool)tConfig->switchHasFR);

  //ver116-1b added video filter caps
  ui->FiltWideMag->setText(QString::number(tConfig->videoFilterCaps[1][0]));
  ui->FiltWidePhase->setText(QString::number(tConfig->videoFilterCaps[1][1]));
  ui->FiltMidMag->setText(QString::number(tConfig->videoFilterCaps[2][0]));
  ui->FiltMidPhase->setText(QString::number(tConfig->videoFilterCaps[2][1]));
  ui->FiltNarrowMag->setText(QString::number(tConfig->videoFilterCaps[3][0]));
  ui->FiltNarrowPhase->setText(QString::number(tConfig->videoFilterCaps[3][1]));
  ui->FiltXNarrowMag->setText(QString::number(tConfig->videoFilterCaps[4][0]));
  ui->FiltXNarrowPhase->setText(QString::number(tConfig->videoFilterCaps[4][1]));

  switch(tConfig->adconv)
  {
    case 8:
      ui->adconv->setCurrentIndex(0);
      break;
    case 12:
      ui->adconv->setCurrentIndex(1);
      break;
    case 16:
      ui->adconv->setCurrentIndex(2);
      break;
    default:
      ui->adconv->setCurrentIndex(3);
  }
  ui->cb->setCurrentIndex(tConfig->cb);

  //Note configDisplayedFilters$() runs from index 0,...; MSAFilters() runs from 1....
  configNumDisplayedFilters=tConfig->MSANumFilters;
  for (int i=1; i <= 40; i++)   //refresh filter list
  {
    float freq, bw;
    configGetFilter(i, freq, bw, tConfig);
    if (i<=configNumDisplayedFilters)
    {
      tConfig->configDisplayedFilters[i-1]=configFormatFilter(freq, bw);    //Assemble freq and bw
    }
    else
    {
      tConfig->configDisplayedFilters[i-1]="";     //Blank if no filter
    }
  }
  ui->filt->clear();
  ui->filt->blockSignals(true);
  for(int ii = 0; ii < 40; ii++)
  {
    QString filter = tConfig->configDisplayedFilters[ii].trimmed();
    if (filter != "")
      ui->filt->addItem(filter);
  }
  ui->filt->blockSignals(false);

  configFilterSelected("");    //Arrange buttons

  configDisplayVNAData(tConfig); //Do this even if they are hidden
  configDisplayTGData(tConfig);

}

void dialogConfigMan::configInitializeVNADefaults(msaConfig *tConfig)
{
  //Enter default values for variables relating to VNA
  tConfig->hasVNA=1;
  tConfig->maxpdmout = 65535;
  tConfig->invdeg = 180;
}

QString dialogConfigMan::configDataError()
{
  QString retVal = "";
  //Return error message if retrieved data has error
  if (configDisplayHasTG==0)
  {
   hasDDS3=0;
  }
  else
  {
   int i = ui->TGtop->currentIndex();
   if (i==3) hasDDS3=1; else hasDDS3=0;    //Only SLIM TG has DDS3
  }
   //For some variables, we try to catch crazy values. ver116-4s deleted some commented-out provisions
  if (tempConfig.appxLO2<=100 || tempConfig.appxLO2>3000)
       return "Invalid LO2 freq.";
  if (tempConfig.appxLO2!=tempConfig.PLL2phasefreq*(int)(tempConfig.appxLO2/tempConfig.PLL2phasefreq))
    return "LO2 is not a multiple of PLL2phasefreq.";

  return retVal;
}

void dialogConfigMan::configAdjustDisplayedItems()
{
  //Adjust displayed items to reflect state of TG and VNA

  if (configDisplayHasTG)
  {
    ui->PLL3type->setVisible(true);
    ui->PLL3pol->setVisible(true);
    ui->PLL3mode->setVisible(true);
    ui->PLL3Ref->setVisible(true);
    ui->DDS3freq->setVisible(true);
    ui->DDS3bw->setVisible(true);
    ui->TG->setText("Delete TG");
    ui->TGtop->setVisible(true);
    ui->TGTopLabel->setVisible(true);

    ui->DDS3label->setVisible(true);
    ui->PLL3label->setVisible(true);
  }
  else
  {
    ui->TGtop->setVisible(false);
    ui->PLL3type->setVisible(false);
    ui->PLL3pol->setVisible(false);
    ui->PLL3mode->setVisible(false);
    ui->PLL3Ref->setVisible(false);
    ui->DDS3freq->setVisible(false);
    ui->DDS3bw->setVisible(false);
    ui->PDM->setVisible(false);
    ui->Inv->setVisible(false);
    ui->TG->setText("Add TG");

    ui->TGTopLabel->setVisible(false);
    ui->DDS3label->setVisible(false);
    ui->PLL3label->setVisible(false);

  }
  if (tempConfig.hasVNA)
  {
    ui->PDMLabel->setVisible(true);
    ui->InvDegLabel->setVisible(true);
    ui->PDM->setVisible(true);
    ui->Inv->setVisible(true);
    ui->VNA->setText("Delete VNA");
  }
  else
  {
    ui->PDMLabel->setVisible(false);
    ui->InvDegLabel->setVisible(false);
    ui->PDM->setVisible(false);
    ui->Inv->setVisible(false);
    ui->VNA->setText("Add VNA");
  }
}

void dialogConfigMan::configDisplayVNAData(msaConfig *tConfig)
{
  //Enter VNA related variables into window
  ui->Inv->setText(QString::number(tempConfig.invdeg));
  ui->PDM->setText(QString::number(tempConfig.maxpdmout));
}

void dialogConfigMan::configDisplayTGData(msaConfig *tConfig)
{
  // disable signal that would have been emmited when the combo boxes changed index
  blockChangeSignals(true);

  //Enter TG related variables into window
  ui->PLL3type->setCurrentIndex(ui->PLL3type->findText(configPLLlabel(tempConfig.PLL3)));

  ui->PLL3pol->setCurrentIndex(tempConfig.PLL3phasepolarity);
  ui->PLL3Ref->setText(QString::number(tempConfig.PLL3phasefreq));
  ui->PLL3mode->setCurrentIndex(ui->PLL3mode->findText(ui->PLL3mode->itemText(tempConfig.PLL1mode)));
  ui->DDS3freq->setText(QString::number(tempConfig.appxdds3));
  ui->DDS3bw->setText(QString::number(tempConfig.dds3filbw));
  ui->TGtop->setCurrentIndex(tempConfig.TGtop);
  if (tempConfig.TGtop==0) configDisplayHasTG=0; else configDisplayHasTG=1;

  blockChangeSignals(false);
}

void dialogConfigMan::configInitializeTGDefaults(msaConfig *tConfig)
{
  //Enter default values for variables relating to TG
  tConfig->TGtop = 2;
  tConfig->PLL3 = 2326;
  tConfig->appxdds3 = 10.7;
  tConfig->dds3filbw = .015;
  tConfig->PLL3phasepolarity = 0;
  tConfig->PLL3mode = 0;
  tConfig->PLL3phasefreq = .974;
}

void dialogConfigMan::configDoFilt(QString btn)
{
  //Add, delete or replace filter in list
  int ind = ui->filt->currentRow();

  //using the listbox to store the filter, they will be loaded
  // back into the arry on before saving the file
  if (btn=="DeleteFilt")        //Delete selected filter
  {
    if (ind==-1) return;      //no selection
    ui->filt->takeItem(ind);

    configFilterSelected("");
    return;
  }

  if (ui->filt->count() >= 39) {QMessageBox::warning(this, "Error", "Too many filters."); return;}

  float freq = ui->filtFreq->text().toFloat();
  float bw = ui->filtBW->text().toFloat();
  //Make sure values aren't crazy
  if (freq<5 || freq>15 || bw<.01 || bw>5000) { QMessageBox::warning(this, "Error", "Invalid Values"); return;}
  QString s=configFormatFilter(freq, bw);    //Assemble freq and bw

  if (btn=="ReplaceFilt")    //Replace selected filter
  {
    if (ind==-1) return;  //No selection
    ui->filt->item(ind)->setText(s);
  }
  else if (btn=="AddFiltAfter")
  {
    ui->filt->insertItem(ind+1,s);
  }
    else
  {
    ui->filt->insertItem(ind-1,s);
  }
  ui->filt->setCurrentRow(-1);

  configFilterSelected("");
}

void dialogConfigMan::configNoSave()
{
  //Return without save button clicked
  //This button is displayed only when we are invoked by the user,
  //not automatically on startup
  //if configHelpHandle$<>"" then close #configHelpHandle$
  //if configLPThelpHandle$<>"" then close #configLPThelpHandle$
  //We need to reload the data from the file to make it active
  //call configCreateLoadFile
  //close #config : configWindHndl$=""
  configRunManagerResult=1;  //signal cancellation
  //exit function   //exits configRunManager and returns to MSA
  allowClose = true;
  close();
}

void dialogConfigMan::configSaveAndReturn()
{
  //Save Config button clicked
  // if configHelpHandle$<>"" then close #configHelpHandle$
  ///if configLPThelpHandle$<>"" then close #configLPThelpHandle$
  //We want to leave with all variables and the file updated
  configGetDisplayData();
  QString errStr=configDataError();
  if (errStr!="")
  {
    QMessageBox::warning(this, "Error", "File Error: "+errStr+ " File not saved.");
    //Cancel the return to MSA due to error; wait for user action
  }
  if (autoRun==0)
  {
    QString msg="You are about to change the MSA configuration file.\n"
        "MSA will close. The changed file will be loaded\n"
        "the next time you run the MSA.\n";
    if (QMessageBox::question(this,"Notice", msg,QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
    {
      return;
    }
    //We were invoked by the user, so allow cancellation
    //Post message with OK and cancel
    allowClose = true;
  }

  configSaveFile(tempConfig);
  configRunManagerResult=0;  //signal no cancellation
  allowClose = true;
  close(); //exits configRunManager and returns to MSA
}

void dialogConfigMan::configDoDefaults()
{
  //Defaults for standard SLIM build
  configAdjustDisplayedItems();
  configInitializeDefaults(&tempConfig);
  configDisplayData(&tempConfig);
}

void dialogConfigMan::configDoLoad()
{
  //Read existing file

  QString errStr = configLoadData();   //SEW5--somehow got deleted in original
  if (errStr!="")
  {
    QMessageBox::warning(this, "Error", "File Error: "+errStr);
  }
  else
  {
    if (tempConfig.TGtop==0)
    {
      configDisplayHasTG=0;
    }
    else
    {
      configDisplayHasTG=1;
    }
    configAdjustDisplayedItems();
    configDisplayData(&tempConfig);
  }
}

void dialogConfigMan::configFilterSelected(QString btn)
{
  //Called when a filter in the list has been selected
  //Put the buttons in the proper state and display filter values in text boxes.
  int ind = ui->filt->currentRow();
  if (ind==-1)    //No selection; restricts options
  {
    ui->AddFiltPrior->setText("Add");
    ui->AddFiltAfter->setVisible(false);// "!hide"
    ui->DeleteFilt->setVisible(false);//  "!hide"
    ui->ReplaceFilt->setVisible(false);// "!hide"
  }
  else
  {
    ui->AddFiltAfter->setVisible(true);// "!show"
    ui->DeleteFilt->setVisible(true);//  "!show"
    ui->ReplaceFilt->setVisible(true);// "!show"
    ui->AddFiltPrior->setText("AddPrior");
    ui->AddFiltAfter->setText("AddAfter");
    ui->DeleteFilt->setText("Delete");
    ui->ReplaceFilt->setText("Replace");
  }

  //Doesn't work to get selected text from the box, so get it
  //from the array
  QString thisFilt;
  if (ind==-1)
    thisFilt="";
  else
  {
    thisFilt = ui->filt->item(ind)->text();
  }
  ui->filtFreq->setText(thisFilt.left(thisFilt.indexOf(" ")).trimmed());
  ui->filtBW->setText(thisFilt.right(thisFilt.length() - thisFilt.indexOf(" ")).trimmed());
}

QString dialogConfigMan::configFormatFilter(float freq, float bw)
{
  QString sfreq = QString::number(freq);
  QString space;
  space.fill(' ', qMax(0,7-sfreq.length()));
  sfreq=sfreq+space;   //makes freq$ a fixed width
  QString bwPad;
  if (bw==0)
  {
      bwPad="   ";
  }
  else
  {
    space.fill(' ', 5-(int)(log((double)bw)/log((double)10)));
    bwPad=space; //Aligns bw at decimal;
  }
  QString sbw = bwPad+QString::number(bw);
  return sfreq+sbw;
}

void dialogConfigMan::configSelPLL3()
{
  if (!ui->PLL1type->isVisible())
      return;
  int i = ui->PLL3type->currentIndex();
  if (i==0) configDoTG(); //no PLL3; turn off TG
}

void dialogConfigMan::configDoTG()
{
  //Add or delete TG
  //Restore defaults even if we are hiding them, because
  //we may end up saving the file
  configGetDisplayData();
  QString errStr=configDataError();
  if (errStr!="") {QMessageBox::warning(this, "Error", errStr); return;}
  configInitializeTGDefaults(&tempConfig);
  configInitializeVNADefaults(&tempConfig);

  if (configDisplayHasTG==1) //Reverse state of configDisplayHasTG
  {
    configDisplayHasTG=0; tempConfig.hasVNA=0;
  }
  else
  {
    configDisplayHasTG=1;
  }
  if (configDisplayHasTG==0) { tempConfig.TGtop=0; tempConfig.PLL3=0; } //Indicate no TG and no PLL3
  configDisplayTGData(&tempConfig);
  configDisplayVNAData(&tempConfig);
  configAdjustDisplayedItems();
}



void dialogConfigMan::configSelADconv()
{
  //Select ADC and automatically change maxpdmoutout to match

  int i = ui->adconv->currentIndex();
  double maxP;
  switch(i)
  {
    case 0:  //8 bit
        maxP=pow(2.0,8.0)-1;
        break;
    case 1:  //12 bit
        maxP=pow(2.0,12.0)-1;
        break;
    case 2:  //16 bit
        maxP=pow(2.0,16.0)-1;
        break;
    default:  //12 bit
        maxP=pow(2.0,12.0)-1;
  }
  ui->PDM->setText(QString::number(maxP));
}
QString  dialogConfigMan::configPLLlabel(int PLL)
{
  QString s=QString::number(PLL);
  if (PLL==2326) s=s+",4118";  //code 2326 is usef for both LMX2326 and ADF4118
  if (PLL==4112) s=s+",4113"; //code 4112 is used for both 4112 and 4113
  return s;
}

void dialogConfigMan::on_SLIMDefault_clicked()
{
  configDoDefaults();
}

void dialogConfigMan::on_test_clicked()
{
  configDoLoad();
}

void dialogConfigMan::on_TG_clicked()
{
  configDoTG();
}
void dialogConfigMan::on_help_clicked()
{
  configDoHelp();
}

void dialogConfigMan::on_saveReturn_clicked()
{
  configSaveAndReturn();
}

void dialogConfigMan::on_noSaveReturn_clicked()
{
  configNoSave();
}

void dialogConfigMan::on_AddFiltPrior_clicked()
{
  configDoFilt("AddFiltPrior");
}

void dialogConfigMan::on_AddFiltAfter_clicked()
{
  configDoFilt("AddFiltAfter");
}

void dialogConfigMan::on_DeleteFilt_clicked()
{
  configDoFilt("DeleteFilt");
}

void dialogConfigMan::on_ReplaceFilt_clicked()
{
  configDoFilt("ReplaceFilt");
}

void dialogConfigMan::on_LPThelp_clicked()
{
  configDoLPTHelp();
}

void dialogConfigMan::on_PLL3type_currentIndexChanged(int index)
{
  configSelPLL3();
}

void dialogConfigMan::on_adconv_currentIndexChanged(int index)
{
  configSelADconv();
}

void dialogConfigMan::on_TGtop_currentIndexChanged(int index)
{
  configSelTGtop();
}

void dialogConfigMan::on_VNA_clicked()
{
  configDoVNA();
}

void dialogConfigMan::on_filt_currentRowChanged(int currentRow)
{
  configFilterSelected("");
}

int dialogConfigMan::configPLLnumber(QString PLL)
{
  //ver116-4k
  int pos=PLL.indexOf(",");
  if (pos==-1) return PLL.toInt();  //single number
  //For multiple numbers, such as 4112,4113 and 2316,4118, use only the first one
  return PLL.left(pos).toInt();   //everything to right of comma
}

void dialogConfigMan::configDoVNA()
{

  //Add or delete VNA
  //Restore defaults even if we are hiding them, because
  //we may end up saving the file
  configGetDisplayData();
  QString errStr=configDataError();
  if (errStr!="") {QMessageBox::warning(this, "Error", errStr); return;}
  int hadVNA=tempConfig.hasVNA;
  configInitializeVNADefaults(&tempConfig);

  if (hadVNA==1)    //Reverse state of hasVNA
  {
    tempConfig.hasVNA=0;   //turn VNA off
  }
  else
  {
    if (configDisplayHasTG==0) configInitializeTGDefaults(&tempConfig);   //add default TG
    tempConfig.hasVNA=1; configDisplayHasTG=1;
  }
  if (configDisplayHasTG==0) { tempConfig.TGtop=0; tempConfig.PLL3=0;} //Indicate no TG and no PLL3
  configDisplayTGData(&tempConfig);
  configDisplayVNAData(&tempConfig);
  configAdjustDisplayedItems();
}

void dialogConfigMan::configDoHelp()
{
  QString Text = "Enter configuration data for your machine.\n"
      "With a standard SLIM build, the items in WHITE likely need no change.\n"
      "CYAN items and Auto Switch checkboxes generally must be customized.";

  QMessageBox::about(0, "Configuration Assistance", Text );
}

void dialogConfigMan::configDoLPTHelp()
{
  QString Text =     "The LPT port address is needed to access the parallel port.\n"
      "This information is maintained for each port by the Device Manager.\n"
      "You can locate the Device Manager through a series of selections similar"
      "to Start/Settings/Control Panel/System/Device Manager. From there make"
      "selections similar to Ports/Printer Port(LPT1)/Properties/Resources.\n"
      "Look for the item Input/Output Range, which will display a range of numbers"
      "in hexidecimal. The first number in that range is the port address you need.\n"
      "Enter the number as it is in hexidecimal; do not convert to base 10.\n"
      "If you have multiple parallel ports, you will have to determine which to use.\n";
  QMessageBox::about(0, "Configuration Assistance", Text );
}

void dialogConfigMan::configSelTGtop()
{
  if (!ui->TGtop->isVisible())
    return;
  int index = ui->TGtop->currentIndex() + 1;
  if (index == 1)
  {
    on_TG_clicked();
  }
  else if (index == 2)
  {
    tempConfig.appxdds3=0;
  }
  else
  {
    tempConfig.appxdds3=10.7;
  }
}

void dialogConfigMan::blockChangeSignals(bool enable)
{
  ui->PLL3type->blockSignals(enable);
  ui->PLL3mode->blockSignals(enable);
  ui->TGtop->blockSignals(enable);
  ui->adconv->blockSignals(enable);
}

void dialogConfigMan::configCreateLoadFile()
{
  if (!QDir().mkpath((DefaultDir + "/MSA_Info")))
  {
    QMessageBox::critical(this, "Error", "Cannot create MSA_Info directory");
  }

  if (!QFile::exists(DefaultDir + "/MSA_Info/config.txt"))
  {
    configCreateDefaultFile(&tempConfig);
  }
  else
  {
    configLoadData();
  }
}

void dialogConfigMan::configCreateDefaultFile(msaConfig *config)
{
  //Create default file. Replaces any existing file
  //Creates a file with default values. Assumes we know the MSA_Info folder exists
  configInitializeDefaults(config);
  configSaveFile(*config);
}

void dialogConfigMan::configInitializeDefaults(msaConfig *tConfig)
{
  tConfig->masterclock = 64;
  tConfig->adconv = 16;
  tConfig->cb = 2;
  tConfig->dds1parser = 1;
  tConfig->appxdds1 = 10.7;
  tConfig->dds1filbw = .015;
  tConfig->PLL1 = 2326;
  tConfig->PLL1phasefreq = .974;
  tConfig->PLL1mode = 0;
  tConfig->PLL1phasepolarity = 0;
  tConfig->PLL2 = 2326;
  tConfig->appxLO2 = 1024;
  tConfig->PLL2phasefreq = 4;
  tConfig->PLL2phasepolarity = 1;
  for (int i=1;i<=40;i++)   //clear filter freq and bw
  {
    tConfig->MSAFilters[i][0]=0;
    tConfig->MSAFilters[i][1]=0;
  }

  tConfig->MSAFilters[1][0]=10.7;
  tConfig->MSAFilters[1][1]=15;  //Add one filter
  tConfig->MSANumFilters=1;

  tConfig->globalPort = 0x378;      //parallel port

  //video filter caps
  tConfig->videoFilterCaps[1][0]=0.002;
  tConfig->videoFilterCaps[2][0]=0.2;
  tConfig->videoFilterCaps[3][0]=10;
  tConfig->videoFilterCaps[4][0]=100;  //Video mag Filters ver116-1b
  tConfig->videoFilterCaps[1][1]=0.011;
  tConfig->videoFilterCaps[2][1]=0.2;
  tConfig->videoFilterCaps[3][1]=2.2;
  tConfig->videoFilterCaps[4][1]=10;  //Video phase filters ver116-4g
  tConfig->videoFilterNames[1] = "Wide";
  tConfig->videoFilterNames[2] = "Mid";
  tConfig->videoFilterNames[3] = "Narrow";
  tConfig->videoFilterNames[4] = "XNarrow"; //names ver116-1b
          //Automated switches. Assume has RBW but nothing else.
  tConfig->switchHasRBW=1;
  tConfig->switchHasVideo=0;
  tConfig->switchHasBand=0;
  tConfig->switchHasTR=0;
  tConfig->switchHasFR=0;

  configInitializeVNADefaults(tConfig);
  configInitializeTGDefaults(tConfig);

}

void dialogConfigMan::configSaveFile(const msaConfig tConfig)
{
  QString settings = configHardwareContext(tConfig);
  QFile fOut(fileFullName);
  if (fOut.open(QFile::WriteOnly | QFile::Text))
  {
    QTextStream s(&fOut);
    s << settings;

    fOut.close();
  }
}

QString dialogConfigMan::configHardwareContext(const msaConfig tConfig)
{
  QStringList list;
  QString retVal;

  //ver114-3h added configHardwareContext$
  //Return hardware context as string
  //successive lines are separated by chr$(13)
  list.append("ConfigVersion=" + moduleVersion);
  for (int i=1; i <= tConfig.MSANumFilters; i++)
  {
    //for each filter, print a line with frequency and bandwidth
    list.append(QString("Filter=%1 %2").arg(tConfig.MSAFilters[i][0], 5,'f',6,'0').arg(tConfig.MSAFilters[i][1], 5,'f',5,' '));
  }

  list.append(QString("LPT=&H%1").arg(tConfig.globalPort,0,16));    //save port as hexidecimal
  list.append(QString("masterclock=%1").arg(tConfig.masterclock, 5,'f',5,'0'));
  list.append(QString("adconv=%1").arg(tConfig.adconv));
  list.append(QString("cb=%1").arg(tConfig.cb));
  list.append(QString("dds1parser=%1").arg(tConfig.dds1parser));
  list.append(QString("appxdds1=%1").arg(tConfig.appxdds1));
  list.append(QString("dds1filbw=%1").arg(tConfig.dds1filbw));
  list.append(QString("PLL1=%1").arg(tConfig.PLL1));
  list.append(QString("PLL1phasefreq=%1").arg(tConfig.PLL1phasefreq));
  list.append(QString("PLL1mode=%1").arg(tConfig.PLL1mode));
  list.append(QString("PLL1phasepolarity=%1").arg(tConfig.PLL1phasepolarity));
  list.append(QString("PLL2=%1").arg(tConfig.PLL2));
  list.append(QString("appxLO2=%1").arg(tConfig.appxLO2));
  list.append(QString("PLL2phasefreq=%1").arg(tConfig.PLL2phasefreq));
  list.append(QString("PLL2phasepolarity=%1").arg(tConfig.PLL2phasepolarity));
  list.append(QString("TGtop=%1").arg(tConfig.TGtop));
  list.append(QString("PLL3=%1").arg(tConfig.PLL3));
  list.append(QString("appxdds3=%1").arg(tConfig.appxdds3));
  list.append(QString("dds3filbw=%1").arg(tConfig.dds3filbw));
  list.append(QString("PLL3phasepolarity=%1").arg(tConfig.PLL3phasepolarity));
  list.append(QString("PLL3mode=%1").arg(tConfig.PLL3mode));
  list.append(QString("PLL3phasefreq=%1").arg(tConfig.PLL3phasefreq));
  list.append(QString("hasVNA=%1").arg(tConfig.hasVNA));
  list.append(QString("maxpdmout=%1").arg(tConfig.maxpdmout));
  list.append(QString("invdeg=%1").arg(tConfig.invdeg));
  //ver116-1b added videoFilterCaps for the capacitances of four possible filters
  //List name, mag cap value and phase cap value
  //Prior software used "VideoFilterCaps" which is now "VideoFilters"
  list.append(QString("VideoFilters=%1, %2, %3; %4, %5, %6; %7, %8, %9; %10, %11, %12")
              .arg(tConfig.videoFilterNames[1])
              .arg(tConfig.videoFilterCaps[1][0])
              .arg(tConfig.videoFilterCaps[1][1])
              .arg(tConfig.videoFilterNames[2])
              .arg(tConfig.videoFilterCaps[2][0])
              .arg(tConfig.videoFilterCaps[2][1])
              .arg(tConfig.videoFilterNames[3])
              .arg(tConfig.videoFilterCaps[3][0])
              .arg(tConfig.videoFilterCaps[3][1])
              .arg(tConfig.videoFilterNames[4])
              .arg(tConfig.videoFilterCaps[4][0])
              .arg(tConfig.videoFilterCaps[4][1]));


 list.append(QString("Switches=%1, %2, %3, %4, %5")
              .arg(tConfig.switchHasRBW)
              .arg(tConfig.switchHasVideo)
              .arg(tConfig.switchHasBand)
              .arg(tConfig.switchHasTR)
              .arg(tConfig.switchHasFR));    //ver116-1b added switches


  retVal = list.join("\r") + "\n";
  return retVal;

}

int dialogConfigMan::configRunManager(int autoR)
{
  autoRun = autoR;
  load();
  exec();
  return configRunManagerResult;
}

QString dialogConfigMan::configLoadData()
{
  return configReadFile(&tempConfig);
}

void dialogConfigMan::configGetFilter(int N, float &freq, float &bw, msaConfig *tConfig)
{
  //Get freq and bw for Nth filter
  if (N<1 || N>40) { freq=0; bw=0; return;}
  freq=tConfig->MSAFilters[N][0]; bw=tConfig->MSAFilters[N][1];
}

QString dialogConfigMan::configReadFile(msaConfig *tConfig)
{
  QString retVal = "";
  QFile textFile(fileFullName);
  if (textFile.open(QFile::ReadOnly))
  {
    //... (open the file for reading, etc.)
    QTextStream textStream(&textFile);
    while (true)
    {
      QString line = textStream.readLine();
      if (line.isNull())
          break;
      else
      {
        retVal = retVal + line + "\r";
      }
    }
    textFile.close();
    retVal = configRestoreHardwareContext(retVal, 0, tConfig);
  }
  return retVal;
}

QString dialogConfigMan::configRestoreHardwareContext(QString &s, int startPos, msaConfig *config)
{
  //function configRestoreHardwareContext$(s$, byRef startPos) //Restore Hardware Context from string
  //Returns error message or ""
  //Reads Hardware Context from s$, starting at startPos. Updates startPos to the beginning
  //of the line following the last one we process, or one past end of string. Lines are separated
  //by chr$(13). The last line we read is "EndContext", if it exists.
  //For legacy reasons, we allow certain tags which we do not process.
  //The data is read from a file already opened, whose handle is in configFile$.
  //The data is in the following format;
  //Each data tag ends with "=", and is case insensitive. Items may appear in any
  //order and may be ommitted (default values will be used)
  //
  //Each variable in configInitializeDefaults, configInitializeTGDefaults and
  //configInitializeVNADefaults can be used as a tag,
  //such as Masterclock=63.99; note case does not matter
  //If error, we return a string describing the error
  //If no error, we return ""
  configInitializeDefaults(config);    //Omitted variables will end up with default values
  int numFiltersFound=0;
  int sLen = s.length();
  QString tLine;
  QString startChar;
  while (startPos<sLen)
  {
    tLine = util.uGetLine(s, startPos);   //Get line and update startPos to next line
    tLine = tLine.trimmed();    //drop extra blanks
    startChar = tLine.left(1);  //first real character of line
    bool isErr = false;
    if (startChar!="")
    {
      //valid line
      if (tLine.left(10).toUpper() == "ENDCONTEXT") break;  //then exit while
      int equalPos = tLine.indexOf("=");     //equal sign marks end of tag
      if (equalPos==-1)
      {
        s = "Line without = sign";//+str$(fileLine) //tag without equal sign
        configInitializeDefaults(config);
        return s;
      }
      QString tag = tLine.left(equalPos ).toUpper();
      QString item = tLine.mid(equalPos+1,tLine.length()); //stuff after equal sign
      item = item.trimmed();

      float fval = item.toFloat();
      int nval = item.toInt();
      if (tag == "CONFIGVERSION")
      {
        configFileVersion = item;
      }
      else if (tag =="FILTER")
      {
        numFiltersFound=numFiltersFound+1;
        // fix me
        float filtFreq=util.Word(item,1).toFloat();
        float filtBW=util.Word(item,2).toFloat();
        if (filtFreq<=0)
        {
            isErr = true;
        }
        else
        {
          if (numFiltersFound==1)
          {
            //If we have only one filter now, it is the default filter
            //and we replace it with the first real one we find
            config->MSAFilters[1][0]=filtFreq;
            config->MSAFilters[1][1]=filtBW;
          }
          else
          {
            configAddFilter(filtFreq, filtBW, config);
          }
        }
      }
      else if (tag =="LPT")
      {
        config->globalPort = item.remove("&").remove("H").toInt(0,16);      //parallel port; saved as hex
      }
      else if (tag =="MASTERCLOCK")
      {
          config->masterclock = fval; if (config->masterclock<=0) isErr=true;
      }
      else if (tag =="CENTFREQ" || tag == "SWEEPWIDTH" || tag == "WATE" || tag == "GLITCHTIME")
      {
          //Legacy items ver114-3h
      }
      else if (tag =="ADCONV")
      {
        config->adconv=nval; if (nval<=0) isErr = true;
      }
      else if (tag =="TOPREF" || tag == "BOTREF")
      {
          //Legacy items ver114-3h
      }
      else if (tag =="CB")
      {
        config->cb=nval; if (configValidCB(nval)==0) isErr = true;
      }
      else if (tag =="DDS1PARSER")
      {
        config->dds1parser=nval; if (nval!=0 && nval!=1) isErr = true;
      }
      else if (tag =="APPXDDS1")
      {
        config->appxdds1=fval; if (fval<=0) isErr = true;
      }
      else if (tag =="DDS1FILBW")
      {
        config->dds1filbw=fval; if (fval<=0) isErr = true;
      }
      else if (tag =="PLL1")
      {
        config->PLL1=nval; if (configValidPLL(nval)==0) isErr = true;
      }
      else if (tag =="PLL1PHASEFREQ")
      {
        config->PLL1phasefreq=fval; if (fval<=0) isErr = true;
      }
      else if (tag =="PLL1MODE")
      {
        //fix me
        config->PLL1mode=nval; if (nval!=0 && nval!=1) isErr = true;
      }
      else if (tag =="PLL1PHASEPOLARITY")
      {
        config->PLL1phasepolarity=nval; if (nval!=0 && nval!=1) isErr = true;
      }
      else if (tag =="PLL2")
      {
        config->PLL2=nval; if (configValidPLL(nval)==0) isErr = true;
      }
      else if (tag =="APPXLO2")
      {
        config->appxLO2=fval; if (fval<=0) isErr = true;
      }
      else if (tag =="PLL2PHASEFREQ")
      {
        config->PLL2phasefreq=fval; if (fval<=0) isErr = true;
      }
      else if (tag =="PLL2PHASEPOLARITY")
      {
        config->PLL2phasepolarity=nval; if (nval!=0 && nval!=1) isErr = true;
      }
      else if (tag =="TGTOP")
      {
        config->TGtop=nval; if (configValidTGtop(nval)==0) isErr = true;
      }
      else if (tag =="PLL3")
      {
        //fix me, looks strange
        config->PLL3=nval; if (nval!=0 && configValidPLL(nval)==0) isErr = true;
      }
      else if (tag =="APPXDDS3")
      {
        config->appxdds3=fval; if (fval<=0) isErr = true;
      }
      else if (tag =="DDS3FILBW")
      {
        config->dds3filbw=fval; if (fval<=0) isErr = true;
      }
      else if (tag =="PLL3PHASEPOLARITY")
      {
        config->PLL3phasepolarity=nval;
        if (nval!=0 && nval!=1)
          isErr = true;
      }
      else if (tag =="PLL3MODE")
      {
        config->PLL3mode=nval;
        if (nval!=0 && nval!=1)
          isErr = true;
      }
      else if (tag =="PLL3PHASEFREQ")
      {
        config->PLL3phasefreq=fval; if (fval<=0) isErr = true;
      }
      else if (tag =="SGPRESET" || tag == "OFFSET")
      {
          //Legacy items ver114-3h
      }
      else if (tag =="HASVNA")
      {
          if (nval==0)
            config->hasVNA=0;
          else
            config->hasVNA=1;
      }
      else if (tag =="MAXPDMOUT")
      {
        config->maxpdmout=fval; if (fval<=0) isErr = true;
      }
      else if (tag =="INVDEG")
      {
        config->invdeg=nval;
      }
      else if (tag =="VIDEOFILTERCAPS") //ver114-5p; modver114-6b obsolete ver116-1b
      {
        config->videoFilterCaps[1][0]=util.Word(item,1).toFloat(); config->videoFilterCaps[1][1]=config->videoFilterCaps[1][0];
        config->videoFilterCaps[2][0]=util.Word(item,2).toFloat(); config->videoFilterCaps[2][1]=config->videoFilterCaps[2][0];
        config->videoFilterCaps[3][0]=util.Word(item,3).toFloat(); config->videoFilterCaps[3][1]=config->videoFilterCaps[3][0];
        config->videoFilterNames[4]=""; //no XNarrow filter in original version
        config->videoFilterCaps[4][0]=0 ; config->videoFilterCaps[4][1]=0;
      }
      else if (tag =="VIDEOFILTERS") //updated version of VideoFilterCaps, which is obsolete //ver116-1b
      {
        for (int fNum=1; fNum <= 4; fNum++)
        {
          QString v=util.uExtractTextItem(item, ";"); //get video filter spec--name, mag cap, phase cap
          if (v=="")  //fewer than four filters listed; blank the rest
          {
            for (int j=fNum; j <= 4; j++)
            {
              config->videoFilterNames[j] = "";
              config->videoFilterCaps[j][0]=0;
              config->videoFilterCaps[j][1]=0;
            }
            break;
          }
          QString filtName=util.uExtractTextItem(v,",").trimmed();    //name is text up to comma
          float v1, v2, v3;
          int isErr = util.uExtractNumericItems(2, v, ",", v1, v2, v3);    //get cap values returns 1 if error
          if (filtName!="" &&filtName!="Wide" && filtName!="Mid"
              && filtName!= "Narrow" && filtName!= "XNarrow") isErr = true;
          if (isErr==1) break;
          config->videoFilterNames[fNum] = filtName;
          config->videoFilterCaps[fNum][0]=v1;
          config->videoFilterCaps[fNum][1]=v2;
        }
      }
      else if (tag =="SWITCHES") //Automated switches ver116-1b
      {
        float v1, v2, v3, v4, v5, v6;
        isErr=util.uExtractNumericItems(3,item, ",",v1, v2, v3); //get first 3
        if (isErr==0) isErr=util.uExtractNumericItems(2,item, ",",v4, v5, v6); //get final 2
        if (isErr==0)
        {
          config->switchHasRBW=v1 ; config->switchHasVideo=v2 ; config->switchHasBand=v3 ;
          config->switchHasTR=v4 ; config->switchHasFR=v5;
        }
      }
      else if (tag =="ENDCONFIGFILE")
      {
        //This used to mark the end of the configuration file
      }
      if (isErr)
      {
        configInitializeDefaults(config);
        return "Error in:" + tLine;
      }
    }    //go to next line
  }
  return "";
}

void dialogConfigMan::configAddFilter(float freq, float bw, msaConfig *tConfig)
{
  //Add a filter
  if (tConfig->MSANumFilters>38)
  {
    QMessageBox::warning(0,"Error", "Too many filters");
    return;
  }
  tConfig->MSANumFilters=tConfig->MSANumFilters+1;
  tConfig->MSAFilters[tConfig->MSANumFilters][0]=freq;
  tConfig->MSAFilters[tConfig->MSANumFilters][1]=bw;
}

bool dialogConfigMan::configValidCB(int cb)
{
  bool valid;
  //Return 1 if cb is configValid; otherwise 0
  switch(cb)
  {
    case 0:
    case 1:
    case 2:
    case 3:
     valid = true;
     break;

    default:
      valid = false;
  }
  return valid;
}

bool dialogConfigMan::configValidPLL(int pll)
{
  bool valid;
  switch (pll)
  {
    case 0:
    case 2325:
    case 2326:
    case 2350:
    case 2353:
    case 4112:
      valid = true;
      break;
    default:
      valid = false;
  }
  return valid;

}

bool dialogConfigMan::configValidTGtop(int tg)
{
  bool valid;
  //Return 1 if top is configValid; otherwise 0
  switch(tg)
  {
    case 0:
    case 1:
    case 2:
     valid = true;
     break;

    default:
      valid = false;
  }
  return valid;

}

void dialogConfigMan::configClearFilters(msaConfig *tConfig)
{
  tConfig->MSANumFilters=0;
  for (int i=1; i <= 40; i++)
  {
    tConfig->MSAFilters[i][0]=0; tConfig->MSAFilters[i][1]=0;
  }
}
//------------From here to the end of the Configuration Manager Module is----------
//------------the original Configuration Module, which lacked the user interface---


QString dialogConfigMan::configVersion()
{
  //Version of this module
  return configModuleVersion;

}

QString dialogConfigMan::getConfigFileVersion()
{
  //Version of the file we have loaded
  return configFileVersion;
}
bool dialogConfigMan::configFileExists()
{
  return QFile::exists(fileFullName);
}

msaConfig::msaConfig()
{
  globalPort = 0;
  hasVNA = 0;

  PLL1 = 0;
  PLL1phasefreq = 0;
  PLL1phasepolarity = 0;
  PLL1mode = 0;

  PLL2 = 0;
  PLL2phasefreq = 0;
  PLL2phasepolarity = 0;

  PLL3 = 0;
  PLL3phasefreq = 0;
  PLL3phasepolarity = 0;
  PLL3mode = 0;

  appxdds1 = 0;
  dds1filbw = 0.015;

  appxdds3 = 0;
  dds3filbw = 0;

  dds1parser = 0;
  appxLO2 = 0;
  masterclock = 0;

  switchHasRBW = 0;
  switchHasVideo = 0;
  switchHasBand = 0;
  switchHasTR = 0;
  switchHasFR = 0;



  maxpdmout = 0;
  invdeg = 0;

  adconv = 0;
  TGtop = 0;
  cb = 0;
  /*
  QString configDisplayedFilters[41];
  float MSAFilters[41][2];
  */
  MSANumFilters = 0;
/*
  float videoFilterCaps[5][3];  //Capacitance(uf) for Wide(1), Mid(2), Narrow(3) and XNarrow(4) video filters   //ver116-1b
                             //Second index is 0 for magnitude and 1 for phase filters.
  QString videoFilterNames[5]; //Names of each video filter, or blank if no filter. Index matches videoFilterCaps   //ver116-1b
*/
  finalfreq = 0;
  finalbw = 0;

}
