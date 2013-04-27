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
#ifndef DIALOGSAVECUSTAPPEARANCE_H
#define DIALOGSAVECUSTAPPEARANCE_H

#include <QDialog>

namespace Ui {
class DialogSaveCustAppearance;
}

class DialogSaveCustAppearance : public QDialog
{
  Q_OBJECT
  
public:
  explicit DialogSaveCustAppearance(QWidget *parent = 0);
  ~DialogSaveCustAppearance();
  void setNameList(QStringList names);
  //QStringList getNameList();
  bool saveCancelled;
  QString getPresetName();
  int getPresetNumber();

  
private slots:


  void on_Delete_clicked();

  void on_Save_clicked();

  void on_Cancel_clicked();

  void on_List_currentRowChanged(int currentRow);

private:
  Ui::DialogSaveCustAppearance *ui;
};

#endif // DIALOGSAVECUSTAPPEARANCE_H
