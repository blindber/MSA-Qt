#ifndef DIALOGFREQAXISPREFERENCE_H
#define DIALOGFREQAXISPREFERENCE_H

#include <QDialog>
#include "msautilities.h"
#include "constants.h"
#include <QCloseEvent>

namespace Ui {
class dialogFreqAxisPreference;
}

class sweepStruct
{
public:
  sweepStruct();
  int msaMode;
  int gentrk;
  int calCanUseAutoWait;
  int useAutoWait;
  int alternateSweep;
  float centfreq;
  float sweepwidth;
  float startfreq;
  float endfreq;
  float baseFrequency;
  int globalSteps;
  int wate;
  QString path;
  int doSpecialGraph;
  QString videoFilter;
  int spurcheck;
  int normrev;
  float sgout;
  float offset;
  int switchFR;
  int userFreqPref;
  int planeadj;
  int prevPlaneAdj;
  int freqBand;
  float LO2;
  QString autoWaitPrecision;


  int MSANumFilters;
  QStringList MSAFiltStrings;
  QStringList videoFilterNames;
  int TGtop;
  float appxLO2;
  int invdeg;



  int S11GraphR0;
  int prevS11GraphR0;
  QString S11JigType;
  QString S21JigAttach;

  QStringList appearances;
  QString lastPresetColor;
  QStringList customPresetNames;

  int sweepDir;
  int XIsLinear;
  int HorDiv;
  int VertDiv;
  int refreshEachScan;
  int displaySweepTime;
};

class dialogFreqAxisPreference : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogFreqAxisPreference(QWidget *parent = 0);
  ~dialogFreqAxisPreference();

  int DisplayAxisXPreference(sweepStruct *config);
  bool cancelled;

protected:
  virtual void closeEvent(QCloseEvent *e);


private slots:
  void on_DirectionF_clicked();

  void on_DirectionR_clicked();


  void on_cbAutoWait_clicked(bool checked);

  void on_LR_clicked(bool checked);

  void on_RL_clicked(bool checked);

  void on_Alternate_clicked(bool checked);

  void on_linear_clicked(bool checked);

  void on_log_clicked(bool checked);

  void on_btnCentSpan_clicked(bool checked);

  void on_btnStartStop_clicked(bool checked);

  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

private:
  Ui::dialogFreqAxisPreference *ui;
  //void FreqAxisPreference();
  //void LeftButDouble();
  //void axisXDoNothing();
  void setDUTForward();
  void setDUTReverse();
  void setAutoWait();
  void clearAutoWait();
  void axisLRon();
  void axisLRoff();
  void axisRLon();
  void axisRLoff();
  void axisALTon();
  void axisALToff();
  void axisXSelLinear();
  void axisXSelLog();
  void setCentSpan();
  void enableCentSpan();
  void setStartStop();
  void enableStartStop();
  void axisXCancel();
  void axisSetupNOP();
  void RecalcPlaneExtAndR0();
  void VerifyPlaneExtension();
  void axisSetupSelectAppearance();
  void NormRevbutton();
  void axisXFinished();

  void SetCenterSpanFreq(float cent, float span);
  void SetStartStopFreq(float startF, float stopF);


  msaUtilities util;

  sweepStruct *cfg;

  //  cWorkArray *uWork;

  int changeAppearance;
  int closeValue;
  bool allowClose;
};

#endif // DIALOGFREQAXISPREFERENCE_H
