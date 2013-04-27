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
#include "dialogsavecustappearance.h"
#include "ui_dialogsavecustappearance.h"

#include <QMessageBox>

DialogSaveCustAppearance::DialogSaveCustAppearance(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogSaveCustAppearance)
{
  ui->setupUi(this);
  saveCancelled = true;
}

DialogSaveCustAppearance::~DialogSaveCustAppearance()
{
  delete ui;
}

void DialogSaveCustAppearance::setNameList(QStringList names)
{
  ui->List->clear();
  ui->List->addItems(names);
}
/*
QStringList DialogSaveCustAppearance::getNameList()
{
  QStringList names;
  for(int ii=0; ii<ui->List->count(); ii++)
  {
    names.append(ui->List->item(ii)->text();
  }
  return names;
}
*/
QString DialogSaveCustAppearance::getPresetName()
{
  return ui->Name->text().trimmed();
}

int DialogSaveCustAppearance::getPresetNumber()
{
  //need to return number 1 referenced, so add 1
  return ui->List->currentRow() + 1;
}

void DialogSaveCustAppearance::on_Delete_clicked()
{
  if(ui->List->currentRow() == -1)
  {
    QMessageBox::information(this, "",tr("No Appearance is selected."));
      return;
  }
  ui->List->currentItem()->setText("Empty");
  ui->Name->setText("Empty");
}

void DialogSaveCustAppearance::on_Save_clicked()
{
  if(ui->List->currentRow() == -1)
  {
    QMessageBox::information(this, "",tr("No Appearance is selected."));
      return;
  }
  QString name = ui->Name->text().trimmed();
  if (name == "" || name == "Empty")
  {
    QMessageBox::information(this, "",tr("You must provide a Name."));
    return;
  }
  saveCancelled = false;
  close();
}

void DialogSaveCustAppearance::on_Cancel_clicked()
{
  saveCancelled = true;
  close();
}

void DialogSaveCustAppearance::on_List_currentRowChanged(int currentRow)
{
  ui->Name->setText(ui->List->item(currentRow)->text());
}
