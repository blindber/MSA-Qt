#ifndef DIALOGFREQAXISPREFERENCE_H
#define DIALOGFREQAXISPREFERENCE_H

#include <QDialog>

namespace Ui {
class dialogFreqAxisPreference;
}

class dialogFreqAxisPreference : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogFreqAxisPreference(QWidget *parent = 0);
  ~dialogFreqAxisPreference();

  void menuFreqAxisPreference();
  

private:
  Ui::dialogFreqAxisPreference *ui;
  void FreqAxisPreference();
  void DisplayAxisXPreference();
  //void LeftButDouble();
  //void axisXDoNothing();
  void setDUTForward();
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

};

#endif // DIALOGFREQAXISPREFERENCE_H
