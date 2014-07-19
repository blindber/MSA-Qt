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
#ifndef REFERENCEDIALOG_H
#define REFERENCEDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include "msautilities.h"
#include "globalvars.h"


namespace Ui {
class referenceDialog;
}

struct referDialog
{
  QColor referenceColor1;
  int referenceWidth1;

  QColor referenceColor2;
  int referenceWidth2;

  QColor referenceColorSmith;
  int referenceWidthSmith;


  QString referenceLineSpec;
  int referenceLineType;
  int referenceTrace;

  int msaMode;
  int referenceDoMath;
  int referenceOpA;
  int referenceOpB;

  int Y1DataType;
  QString Y1DataFormat;
  int Y2DataType;
  QString Y2DataFormat;
};

class referenceDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit referenceDialog(QWidget *parent = 0);
  ~referenceDialog();
  void ReferenceDialog(referDialog *newSettings);

  bool clearReferences;
  bool createReferences;
  bool cancelled;
  
private slots:
  void on_Clear_clicked(bool checked);

  void on_UseData_clicked(bool checked);

  void on_UseFixed_clicked(bool checked);

  void on_UseRLC_clicked(bool checked);

  void on_color1_clicked(const QModelIndex &index);

  void on_color2_clicked(const QModelIndex &index);

  void on_colorSmith_clicked(const QModelIndex &index);

  void on_NoUpdate_clicked();

  void on_OK_clicked();

  void on_Cancel_clicked();

  void on_both_clicked(bool checked);

  void on_add_clicked(bool checked);

  void on_reverseSub_clicked(bool checked);

  void on_sub_clicked(bool checked);

  void on_t1_clicked(bool checked);

  void on_t2_clicked(bool checked);

  void on_smith_clicked(bool checked);

  void on_mathOnDB_clicked(bool checked);

  void on_mathOnGraph_clicked(bool checked);

private:
  Ui::referenceDialog *ui;
  msaUtilities util;
  globalVars vars;
  referDialog *activeSettings;

  referDialog settings;



  void refDisplayRLCValues();
  void HideTrace1();
  void ShowTrace1();
  void HideTrace2();
  void HideTraceSmith();
  void ShowTrace2();
  void ShowTraceSmith();
  void ShowEligibleTraces();
  void refDialogFinished();
  void load();


  int noUpdate;
  int hadData;
  QString spec;
  bool allowClose;


protected:
  virtual void closeEvent(QCloseEvent *e);

};

#endif // REFERENCEDIALOG_H
