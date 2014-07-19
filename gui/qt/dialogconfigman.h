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
#ifndef DIALOGCONFIGMAN_H
#define DIALOGCONFIGMAN_H

#include <QDialog>
#include <QMessageBox>
#include <QFile>
#include <QCloseEvent>
#include "msautilities.h"
#include "constants.h"

class msaConfig
{
public:
  msaConfig();
  int globalPort;
  int hasVNA;

  int PLL1;
  float PLL1phasefreq;
  int PLL1phasepolarity;
  int PLL1mode;

  int PLL2;
  float PLL2phasefreq;
  int PLL2phasepolarity;

  int PLL3;
  float PLL3phasefreq;
  int PLL3phasepolarity;
  int PLL3mode;

  float appxdds1;
  float dds1filbw;

  float appxdds3;
  float dds3filbw;

  int dds1parser;
  int appxLO2;
  double masterclock;

  int switchHasRBW;
  int switchHasVideo;
  int switchHasBand;
  int switchHasTR;
  int switchHasFR;


  QString configDisplayedFilters[41];

  int maxpdmout;
  float invdeg;

  int adconv;
  int TGtop;
  int cb;

  float MSAFilters[41][2];
  int MSANumFilters; //Number of filters in list
  QString MSAFiltStrings[40];  //Same info as MSAFilters(), but freq and bw are combined in a string; zero entry is used


  float videoFilterCaps[5][3];  //Capacitance(uf) for Wide(1), Mid(2), Narrow(3) and XNarrow(4) video filters   //ver116-1b
                             //Second index is 0 for magnitude and 1 for phase filters.
  QString videoFilterNames[5]; //Names of each video filter, or blank if no filter. Index matches videoFilterCaps   //ver116-1b

  float finalfreq;
  float finalbw;

};

namespace Ui {
class dialogConfigMan;
}

class dialogConfigMan : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogConfigMan(QWidget *parent = 0);
  QString configmoduleVersion();
  void configInitializeDefaults(msaConfig *tConfig);
  QString configFormatFilter(float freq, float bw);
  QString configReadFile(msaConfig *tConfig);
  void configSaveFile(const msaConfig tConfig);
  QString configRestoreHardwareContext(QString &s, int startPos, msaConfig *config);
  QString configHardwareContext(const msaConfig tConfig);
  int configRunManager(int autoR);
  ~dialogConfigMan();


  //   ---------Globals for Config Module----
  QString configModuleVersion;//     'Version of configuration module
  QString fileFullName;//   'path name for configuration file
  QString configFileInfo[11][4];   // 'Used to get file info
  QString configFileVersion;       //'File version of data input from file


  QString configVersion();
  QString getConfigFileVersion();
  bool configFileExists();
  void configNoFile();

  QString configLoadData();
  QString configRestoreHardwareContext(QString &s, int startPos);
  QString configFileFullName();

  
protected:
  virtual void closeEvent(QCloseEvent *e);

private slots:
  void on_SLIMDefault_clicked();

  void on_test_clicked();

  void on_TG_clicked();

  void on_help_clicked();

  void on_saveReturn_clicked();

  void on_noSaveReturn_clicked();

  void on_AddFiltPrior_clicked();

  void on_AddFiltAfter_clicked();

  void on_DeleteFilt_clicked();

  void on_ReplaceFilt_clicked();

  void on_LPThelp_clicked();

  void on_PLL3type_currentIndexChanged(int index);

  void on_adconv_currentIndexChanged(int index);

  void on_TGtop_currentIndexChanged(int index);

  void on_VNA_clicked();

  void on_filt_currentRowChanged(int currentRow);

private:
  Ui::dialogConfigMan *ui;
  msaUtilities util;
  bool allowClose;
  int configRunManagerResult;

  void load();
  void configGetDisplayData();
  void configDisplayData(msaConfig *tConfig);
  void configInitializeVNADefaults(msaConfig *tConfig);
  QString configDataError();
  void configAdjustDisplayedItems();
  void configDisplayVNAData(msaConfig *tConfig);
  void configDisplayTGData(msaConfig *tConfig);
  void configInitializeTGDefaults(msaConfig *tConfig);
  void configDoFilt(QString btn);
  void configNoSave();
  void configSaveAndReturn();
  void configDoDefaults();
  void configDoLoad();
  void configFilterSelected(QString btn);
  void configSelPLL3();
  void configDoTG();
  void configSelADconv();
  QString  configPLLlabel(int PLL);
  int configPLLnumber(QString PLL);
  void configDoVNA();
  void configDoHelp();
  void configDoLPTHelp();
  void configSelTGtop();

  void blockChangeSignals(bool enable);
  void configCreateLoadFile();
  void configCreateDefaultFile(msaConfig *config);
  void configGetFilter(int N, float &freq, float &bw, msaConfig *tConfig);


  void configAddFilter(float freq, float bw, msaConfig *tConfig);
  bool configValidCB(int cb);
  bool configValidPLL(int pll);
  bool configValidTGtop(int tg);
  void configClearFilters(msaConfig *tConfig);

  bool autoRun;
  int configNumDisplayedFilters;
  int hasDDS3;
  int configDisplayHasTG;
  QString moduleVersion;
  QString DefaultDir;

  msaConfig tempConfig;

};

#endif // DIALOGCONFIGMAN_H
