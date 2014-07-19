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
#include "referencedialog.h"
#include "ui_referencedialog.h"
#include <QColorDialog>
#include "constants.h"

referenceDialog::referenceDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::referenceDialog)
{
  ui->setupUi(this);
  allowClose = false;
  QStringList traceWidths;
  traceWidths << "1" << "2" << "3";
  ui->width1->addItems(traceWidths);
  ui->width2->addItems(traceWidths);
  ui->widthSmith->addItems(traceWidths);

  // the QListWidget needs at least one item to get a click event
  ui->color1->addItem("");
  ui->color2->addItem("");
  ui->colorSmith->addItem("");

  clearReferences = false;
  createReferences = false;
  cancelled = false;
}

referenceDialog::~referenceDialog()
{
  delete ui;
}

void referenceDialog::refDisplayRLCValues()
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  /*
  //Put string of RLC values into ui->RLCInfo
    QString form="3,3,4//UseMultiplier";
    QString resForm="3,3,4//UseMultiplier//SuppressMilli";
    QString QForm="######.#";
    QString R=util.uFormatted(DialogRValue, resForm);
    QString L=util.uFormatted(DialogLValue, form);
    QString C=util.uFormatted(DialogCValue, form);
    QString QL=util.uFormatted(DialogQLValue, QForm);
    QString QL=util.uFormatted(DialogQCValue, QForm);
    QString con;
    if (DialogRLCConnect=="S)" )
      con="(Series";
    else
      con="(Parallel";
    ui->RLCInfo->setText(con+" R="+R+", L="+L+ "H, C="+C+ "F, TL="+DialogCoaxName+")";
        */
}
void referenceDialog::HideTrace1()
{
  ui->width1->setVisible(false);
  ui->color1->setVisible(false);
  ui->widthLabel1->setVisible(false);
  ui->colorLabel1->setVisible(false);
  ui->val1->setVisible(false);
  ui->valLabel1->setVisible(false);
}
void referenceDialog::ShowTrace1()
{
  bool fixed = ui->UseFixed->checkState();
  bool both = ui->both->checkState();    //See whether ref lines will be drawn or are just used for math
  if (both || fixed)    //drawing both ref and data traces, or using fixed value ref
  {
    if (both)
    {
      ui->width1->setVisible(true);
      ui->color1->setVisible(true);
      ui->widthLabel1->setVisible(true);
      ui->colorLabel1->setVisible(true);
    }
    else
    {
      ui->width1->setVisible(false);
      ui->color1->setVisible(false);
      ui->widthLabel1->setVisible(false);
      ui->colorLabel1->setVisible(false);
    }
    ui->val1->setVisible(fixed);
    ui->valLabel1->setVisible(fixed);  //Show fixed val only for fixed val ref
  }
  else
  {
    ui->width1->setVisible(false);
    ui->color1->setVisible(false);
    ui->widthLabel1->setVisible(false);
    ui->colorLabel1->setVisible(false);
    ui->val1->setVisible(false);
    ui->valLabel1->setVisible(false);
  }
}
void referenceDialog::HideTrace2()
{
  ui->width2->setVisible(false);
  ui->color2->setVisible(false);
  ui->widthLabel2->setVisible(false);
  ui->colorLabel2->setVisible(false);
  ui->val2->setVisible(false);
  ui->valLabel2->setVisible(false);
}
void referenceDialog::HideTraceSmith()
{
  ui->widthSmith->setVisible(false);
  ui->colorSmith->setVisible(false);
  ui->widthLabelSmith->setVisible(false);
  ui->colorLabelSmith->setVisible(false);
}
void referenceDialog::ShowTrace2()
{
  bool fixed = ui->UseFixed->checkState();
  bool both = ui->both->checkState(); //See whether ref lines will be drawn or are just used for math
  if (both || fixed)     //drawing both ref and data traces, or using fixed value ref
  {
    if (both)    //show width and color only if no math being done
    {
      ui->width2->setVisible(true);
      ui->color2->setVisible(true);
      ui->widthLabel2->setVisible(true);
      ui->colorLabel2->setVisible(true);
    }
    else
    {
      ui->width2->setVisible(false);
      ui->color2->setVisible(false);
      ui->widthLabel2->setVisible(false);
      ui->colorLabel2->setVisible(false);
    }

    ui->val2->setVisible(fixed);
    ui->valLabel2->setVisible(fixed);   //Show fixed val only for fixed val ref
  }
  else
  {
    ui->width2->setVisible(false);
    ui->color2->setVisible(false);
    ui->widthLabel2->setVisible(false);
    ui->colorLabel2->setVisible(false);
    ui->val2->setVisible(false);
    ui->valLabel2->setVisible(false);
  }
}
void referenceDialog::ShowTraceSmith()
{
  //Note smith reference is always drawn; never used for math. But it cannot
  //be drawn for Fixed Value.
  bool fixed = ui->UseFixed->checkState();
  if (!fixed)    //not using fixed value ref
  {
    ui->widthSmith->setVisible(true);
    ui->colorSmith->setVisible(true);
    ui->widthLabelSmith->setVisible(true);
    ui->colorLabelSmith->setVisible(true);
  }
  else
  {
    ui->widthSmith->setVisible(false);
    ui->colorSmith->setVisible(false);
    ui->widthLabelSmith->setVisible(false);
    ui->colorLabelSmith->setVisible(false);
  }
}
void referenceDialog::ShowEligibleTraces()
{
  bool clear = ui->Clear->checkState();
  if (clear) //Don't show anything if we Don't have reference lines
  {
    HideTrace1();
    HideTrace2();
    HideTraceSmith();
    ui->t1->setVisible(false);
    ui->t2->setVisible(false);
    ui->smith->setVisible(false);
    return;
  }
  if (settings.Y1DataType!=constNoGraph) // this is bad, vars is not linked to the global one
  {
    ui->t1->setVisible(true);
    bool t1 = ui->t1->checkState();
    if (t1)
      ShowTrace1();
    else
      HideTrace1();
  }
  else
  {
    HideTrace1();
    ui->t1->setVisible(false);
  }
  if (settings.Y2DataType!=constNoGraph)
  {
    ui->t2->setVisible(true);
    bool t2 = ui->t2->checkState();
    if (t2)
      ShowTrace2();
    else
      HideTrace2();
  }
  else
  {
    HideTrace2();
    ui->t2->setVisible(false);
  }
  bool fixed = ui->UseFixed->checkState();
  if (settings.msaMode==modeReflection && !fixed)
  {
    //Show smith only in reflection mode when reference lines are enabled
    //and not using fixed value reference
    ui->smith->setVisible(true);
    bool smith = ui->smith->checkState();
    if (smith)
      ShowTraceSmith();
    else
      HideTraceSmith();
  }
  else
  {
    HideTraceSmith();
    ui->smith->setVisible(false);
  }
}
void referenceDialog::refDialogFinished()
{
  //ver114-8b added reference math items
  activeSettings->referenceDoMath=0;   //assume no math
  bool both = ui->both->checkState();    //graph both ref and data; no math
  if (both)
  {
    activeSettings->referenceDoMath=0;
    activeSettings->referenceOpA=1;
    activeSettings->referenceOpB=1;
  }
  bool add = ui->add->checkState(); //graph ref+data
  if (add)
  {
    activeSettings->referenceDoMath=1;
    activeSettings->referenceOpA=1;
    activeSettings->referenceOpB=1;
  }
  if (ui->sub->checkState())  //graph data-ref
  {
    activeSettings->referenceDoMath=1;
    activeSettings->referenceOpA=-1;
    activeSettings->referenceOpB=1;
  }
  if (ui->reverseSub->checkState()) //graph ref-data
  {
    activeSettings->referenceDoMath=1;
    activeSettings->referenceOpA=1;
    activeSettings->referenceOpB=-1;
  }
  //If we have set referenceDoMath=1, we need to figure out whether it should really be
  //set to 2, meaning to do the math on the actual graph values (=2), not on raw db (=1)
  //We only allow it to be 1 for SA mode, because the same thing is effectively done in VNA modes
  //by calibration.
  //TEMPORARILY, we only allow referenceDoMath=2
  if (activeSettings->referenceDoMath==1)
    activeSettings->referenceDoMath=2;
  //        if msaMode$="SA" then
  //            ui->mathOnDB, "value? s$"   //set if do math on the db value
  //            if s$="reset" then referenceDoMath=2
  //        else
  //            referenceDoMath=2
  //        end if
  //    end if
  activeSettings->referenceTrace=0;

  if (ui->t1->checkState())
  {
    activeSettings->referenceTrace=activeSettings->referenceTrace+1;
    activeSettings->referenceColor1=settings.referenceColor1;
    int i = ui->width1->currentIndex() + 1;
    if (i<1) i=1;
    activeSettings->referenceWidth1=i;
  }

  if (ui->t2->checkState())
  {
    activeSettings->referenceTrace=activeSettings->referenceTrace+2;
    activeSettings->referenceColor2=settings.referenceColor2;
    int i = ui->width2->currentIndex() + 1;
    if (i<1) i=1;
    activeSettings->referenceWidth2=i;
  }
  bool smith = ui->smith->checkState();
  bool fixed = ui->UseFixed->checkState();
  if (smith && !fixed)  //No smith trace for fixed value ref
  {
    activeSettings->referenceTrace=activeSettings->referenceTrace+4; //set bit for smith
    activeSettings->referenceColorSmith=settings.referenceColorSmith;
    int i = ui->widthSmith->currentIndex() + 1;
    if (i<1) i=1;
    activeSettings->referenceWidthSmith=i;
  }
  //Determine reference line type. Note that even if no reference trace is currently turned on
  //reference data can still be created.
  if (ui->UseData->checkState())
  {
    //Use current data as reference
    activeSettings->referenceLineType=1;
    activeSettings->referenceLineSpec="";     //current data
  }
  else
  {
    bool useRlc = ui->UseRLC->checkState();
    if (useRlc)
    {
      //Use RLC as reference
      activeSettings->referenceLineType=2;
      activeSettings->referenceLineSpec=spec;
    }
    else
    {
      if (ui->UseFixed->checkState())
      {
        //Fixed value reference
        QString fixedVal1 = ui->val1->text();
        QString fixedVal2 = ui->val2->text();
        activeSettings->referenceLineType=3;
        //spec contains the two values separated by a semicolon in same format as markers
        activeSettings->referenceLineSpec = util.uCompact(util.uFormatted(util.uValWithMult(fixedVal1), settings.Y1DataFormat))+";"
            + util.uCompact(util.uFormatted(util.uValWithMult(fixedVal2), settings.Y2DataFormat));
      }
      else
      {
        //No reference lines
        activeSettings->referenceLineType=0;
        activeSettings->referenceLineSpec="";
        activeSettings->referenceTrace=0;
        activeSettings->referenceDoMath=0;
        clearReferences = true;
        createReferences = false;
        cancelled = false;
        allowClose = true;
        close();
      }
    }
  }

  //Create reference data. But if we are using "current data" and user wants
  //to keep the prior data, Don't update.
  if (noUpdate==0 || activeSettings->referenceLineType!=1)
  {
    clearReferences = false;
    createReferences = true;
    cancelled = false;
  }
  allowClose = true;
  close();
}
void referenceDialog::ReferenceDialog(referDialog *newSettings)
{
  allowClose = false;
  activeSettings = newSettings;

  settings.referenceColor1 = activeSettings->referenceColor1;
  settings.referenceWidth1 = activeSettings->referenceWidth1;

  settings.referenceColor2 = activeSettings->referenceColor2;
  settings.referenceWidth2 = activeSettings->referenceWidth2;

  settings.referenceColorSmith = activeSettings->referenceColorSmith;
  settings.referenceWidthSmith = activeSettings->referenceWidthSmith;


  settings.referenceLineSpec = activeSettings->referenceLineSpec;
  settings.referenceLineType = activeSettings->referenceLineType;
  settings.referenceTrace = activeSettings->referenceTrace;

  settings.msaMode = activeSettings->msaMode;
  settings.referenceDoMath = activeSettings->referenceDoMath;
  settings.referenceOpA = activeSettings->referenceOpA;
  settings.referenceOpB = activeSettings->referenceOpB;
  settings.Y1DataFormat = activeSettings->Y1DataFormat;
  settings.Y2DataFormat = activeSettings->Y2DataFormat;
  settings.Y1DataType = activeSettings->Y1DataType;
  settings.Y2DataType = activeSettings->Y2DataType;


  load();
  exec();
}
void referenceDialog::load()
{
/*
  GetDialogPlacement(); //set UpperLeftX and UpperLeftY ver115-1c
  #refDialog, "font ms_sans_serif 10"
*/
  ui->width1->setCurrentIndex(settings.referenceWidth1 - 1);
  ui->width2->setCurrentIndex(settings.referenceWidth2 - 1);
  ui->widthSmith->setCurrentIndex(settings.referenceWidthSmith + 1);
  ui->color1->setStyleSheet( QString("background: %1;").arg(settings.referenceColor1.name()) );
  ui->color2->setStyleSheet( QString("background: %1;").arg(settings.referenceColor2.name()) );
  ui->colorSmith->setStyleSheet( QString("background: %1;").arg(settings.referenceColorSmith.name()) );

  if (settings.referenceLineType==0) settings.referenceTrace=0;
  if (settings.msaMode == modeSA) ui->UseRLC->setVisible(false); //Can't do RLC in SA mode

  if (settings.referenceTrace & 2)   //Do trace 2
  {
    ui->t2->setChecked(true);
  }
  else
  {
    ui->t2->setChecked(false);
    ui->width2->setVisible(false);
    ui->color2->setVisible(false);
    ui->widthLabel2->setVisible(false);
    ui->colorLabel2->setVisible(false);
  }
  if (settings.referenceTrace & 1)     //Do trace 1
  {
    ui->t1->setChecked(true);
  }
  else
  {
    ui->t1->setChecked(false);
    ui->width1->setVisible(false);
    ui->color1->setVisible(false);
    ui->widthLabel1->setVisible(false);
    ui->colorLabel1->setVisible(false);
  }
  if (settings.referenceTrace & 4)    //Do smith
  {
    ui->smith->setChecked(true);
  }
  else
  {
    ui->smith->setChecked(false);
    ui->widthSmith->setVisible(false);
    ui->colorSmith->setVisible(false);
    ui->widthLabelSmith->setVisible(false);
    ui->colorLabelSmith->setVisible(false);
  }
  noUpdate=0;  //whether to update ref data when we close
  hadData=0;   //whether we had reference data for UseData when we opened  //ver114-8b
  if (settings.referenceDoMath>0)       //ver115-5d
  {
    if (settings.referenceOpA==1 && settings.referenceOpB==1) ui->add->setChecked(true);   //Ref+data
    if (settings.referenceOpA==-1 && settings.referenceOpB==1) ui->sub->setChecked(true);  //data-ref
    if (settings.referenceOpA==1 && settings.referenceOpB==-1) ui->reverseSub->setChecked(true);   //ref-data
  }
  else
  {
    ui->both->setChecked(true); //Graph ref and data without doing math
  }
  if (settings.msaMode==modeSA)   //Only SA has these boxes ver115-5d
  {
    if (settings.referenceDoMath==2)
    {
      ui->mathOnDB->setChecked(false);
      ui->mathOnGraph->setChecked(true);
    }
    else
    {
      ui->mathOnDB->setChecked(true);
      ui->mathOnGraph->setChecked(false);
    }
    if (settings.referenceDoMath==0)
    {
      ui->mathOnDB->setVisible(false);
      ui->mathOnGraph->setVisible(false);
    }
    ui->mathOnDB->setVisible(false);
    ui->mathOnGraph->setVisible(false); //THESE ARE TEMPORARILY DISABLED ver115-5d
  }

  if (settings.msaMode!=modeReflection)
  {
    ui->smith->setVisible(false);
    ui->colorLabelSmith->setVisible(false);
    ui->colorSmith->setVisible(false);
    ui->widthLabelSmith->setVisible(false);
    ui->widthSmith->setVisible(false);
    ui->smithInst->setVisible(false);
  }

  if (settings.referenceLineType==1)
  {
    ui->UseData->setChecked(true);
    hadData=1;
    on_UseData_clicked(true);
    return;
  }
  if (settings.referenceLineType==2)
  {
    ui->UseRLC->setVisible(true);
    spec=settings.referenceLineSpec;
    refDisplayRLCValues();
    on_UseRLC_clicked(true);
    return;
  }
  if (settings.referenceLineType==3)
  {
    //Insert existing values for fixed value reference
    QString s=settings.referenceLineSpec;
    float v1, v2, v3;
    util.uExtractNumericItems(2,s,";",v1,v2,v3);   //Get two value; destroys s$

    ui->val1->setText(util.uFormatted(v1, settings.Y1DataFormat));  //Y1
    ui->val2->setText(util.uFormatted(v2, settings.Y2DataFormat));  //Y2
    on_UseFixed_clicked(true);
    return;
  }
  ui->Clear->setChecked(true);
  on_Clear_clicked(true);
}

void referenceDialog::on_Clear_clicked(bool checked)
{
  if (checked)
  {
    ui->Clear->setChecked(true);
    ui->UseData->setChecked(false);
    ui->UseRLC->setChecked(false);
    ui->UseFixed->setChecked(false);
    ui->RLCInfo->setVisible(false);
    ui->t1->setVisible(false);
    ui->NoUpdate->setVisible(false);
    ui->OK->setText("OK");
    HideTrace1();
    ui->t2->setVisible(false);
    HideTrace2();
    ui->smith->setVisible(false);
    HideTraceSmith();
  }
  else
  {
    ui->Clear->setChecked(true);  //Prevents reset
  }
}

void referenceDialog::on_UseData_clicked(bool checked)
{
  if (checked)
  {
    ui->UseData->setChecked(true);
    ui->Clear->setChecked(false);
    ui->UseRLC->setChecked(false);
    ui->UseFixed->setChecked(false);
    ui->RLCInfo->setVisible(false);

    if (hadData) //ver114-8b
    {
      ui->NoUpdate->setVisible(false);
      ui->OK->setText("Close"); // Update Data"
    }
    ShowEligibleTraces();
  }
  else
  {
    ui->UseData->setChecked(true);  //Prevents reset
  }
}

void referenceDialog::on_UseFixed_clicked(bool checked)
{
  if(checked)
  {
    ui->UseFixed->setChecked(true);
    ui->Clear->setChecked(false);
    ui->UseRLC->setChecked(false);
    ui->UseData->setChecked(false);
    ui->RLCInfo->setVisible(false);
    ui->NoUpdate->setVisible(false);
    ui->OK->setText("OK");
    ShowEligibleTraces();
  }
  else
  {
    ui->UseFixed->setChecked(true);  //Prevents reset
  }
}

void referenceDialog::on_UseRLC_clicked(bool checked)
{
  qDebug() << "Unconverted code called" << __FILE__ << " " << __FUNCTION__;
  if(checked)
  {
    ui->Clear->setChecked(false);
    ui->UseData->setChecked(false);
    ui->UseRLC->setChecked(true);
    ui->UseFixed->setChecked(false);

    ui->RLCInfo->setVisible(false);
    ui->NoUpdate->setVisible(false);
    ui->OK->setText("OK");
    ShowEligibleTraces();
    refDisplayRLCValues();
    /*
    //Get current RLC spec; if invalid do defaults
    parseErr=uParseRLC(spec$, DialogRLCConnect$, DialogRValue, DialogLValue, _
                              DialogCValue, DialogQLValue, DialogQCValue, dumD, DialogCoaxSpecs$)    //ver116-4i added delay D, whichis not used
    if parseErr then
        spec$="RLCvoid referenceDialog::S,R0,L0,C";constMaxValue;"()"
        DialogRLCConnect$="S" : DialogRValue=0 : DialogLValue=0
        DialogCValue=constMaxValue : DialogQLValue=constMaxValue : DialogQCValue=constMaxValue //ver115-4b
        DialogCoaxSpecs$=""  //ver115-4a
    end if
    call RLCDialog
    if DialogCancelled then wait
        //Get the new values; display them
    refDisplayRLCValues()'
    ui->RLCInfo, "!show"
        //Create the RLC spec string with RLC and Coax sections
    spec$="RLCvoid referenceDialog::";DialogRLCConnect$;",R";uCompact$(R$);",L";uCompact$(L$); ",C";uCompact$(C$); _
                     ",QL";uCompact$(QL$); ",QC";uCompact$(QC$);"(), Coaxvoid referenceDialog::";DialogCoaxSpecs$;"()" //ver115-4b
  */
  }

  else
  {
    ui->RLCInfo->setVisible(false);
  }
}

void referenceDialog::on_color1_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor(settings.referenceColor1, this);
  if (color.isValid())
  {
    settings.referenceColor1 = color.name();
    ui->color1->setStyleSheet( QString("background: %1;").arg(settings.referenceColor1.name()) );
  }
  ui->color1->setCurrentRow(-1);
}

void referenceDialog::on_color2_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor(settings.referenceColor2, this);
  if (color.isValid())
  {
    settings.referenceColor2 = color.name();
    ui->color2->setStyleSheet( QString("background: %1;").arg(settings.referenceColor2.name()) );
  }
  ui->color2->setCurrentRow(-1);
}

void referenceDialog::on_colorSmith_clicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  QColor color = QColorDialog::getColor(settings.referenceColorSmith, this);
  if (color.isValid())
  {
    settings.referenceColorSmith = color.name();
    ui->colorSmith->setStyleSheet( QString("background: %1;").arg(settings.referenceColorSmith.name()) );
  }
  ui->colorSmith->setCurrentRow(-1);
}

void referenceDialog::on_NoUpdate_clicked()
{
  noUpdate=1;
  refDialogFinished();
}

void referenceDialog::on_OK_clicked()
{
  refDialogFinished();
}

void referenceDialog::on_Cancel_clicked()
{
  clearReferences = false;
  createReferences = false;
  cancelled = true;

  allowClose = true;
  close();
}

void referenceDialog::on_both_clicked(bool checked)
{
  if (checked)
  {
    ui->both->setChecked(true);
    ui->add->setChecked(false);
    ui->sub->setChecked(false);
    ui->reverseSub->setChecked(false);
    ShowEligibleTraces();
  }
  else
  {
    ui->both->setChecked(true);
  }
}

void referenceDialog::on_add_clicked(bool checked)
{
  if (checked)
  {
    ui->both->setChecked(false);
    ui->add->setChecked(true);
    ui->sub->setChecked(false);
    ui->reverseSub->setChecked(false);
    ShowEligibleTraces();
  }
  else
  {
    ui->add->setChecked(true); //prevents reset
  }
}

void referenceDialog::on_reverseSub_clicked(bool checked)
{
  if (checked)
  {
    ui->both->setChecked(false);
    ui->add->setChecked(false);
    ui->sub->setChecked(false);
    ui->reverseSub->setChecked(true);
    ShowEligibleTraces();
  }
  else
  {
    ui->reverseSub->setChecked(true); //prevents reset
  }
}

void referenceDialog::on_sub_clicked(bool checked)
{
  if (checked)
  {
    ui->both->setChecked(false);
    ui->add->setChecked(false);
    ui->sub->setChecked(true);
    ui->reverseSub->setChecked(false);
    ShowEligibleTraces();
  }
  else
  {
    ui->sub->setChecked(true); //prevents reset
  }
}

void referenceDialog::on_t1_clicked(bool checked)
{
  if(checked)
  {
    ShowEligibleTraces();
  }
  else
  {
    ShowEligibleTraces();
  }
}

void referenceDialog::on_t2_clicked(bool checked)
{
  if(checked)
  {
    ShowEligibleTraces();
  }
  else
  {
    ShowEligibleTraces();
  }

}

void referenceDialog::on_smith_clicked(bool checked)
{
  if(checked)
  {
    ShowEligibleTraces();
  }
  else
  {
    ShowEligibleTraces();
  }
}

void referenceDialog::on_mathOnDB_clicked(bool checked)
{
  if(checked)
  {
    ui->mathOnDB->setChecked(true);
    ui->mathOnGraph->setChecked(false);
  }
  else
  {
    //turn graph math off
      ui->mathOnDB->setChecked(false);    //Can turn off only by turning on mathOnGraph
  }
}

void referenceDialog::on_mathOnGraph_clicked(bool checked)
{
  if(checked)
  {
    //turn graph value math on
    ui->mathOnGraph->setChecked(true);
    ui->mathOnDB->setChecked(false);
  }
  else
  {
    //turn graph value math off
    ui->mathOnGraph->setChecked(false);    //Can turn off only by turning on mathOnDB
  }
}


void referenceDialog::closeEvent(QCloseEvent *e)
{
  if (!allowClose)
    e->ignore();
}
