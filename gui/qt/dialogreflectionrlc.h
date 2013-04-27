#ifndef DIALOGREFLECTIONRLC_H
#define DIALOGREFLECTIONRLC_H

#include <QDialog>

namespace Ui {
class dialogReflectionRLC;
}

class dialogReflectionRLC : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogReflectionRLC(QWidget *parent = 0);
  ~dialogReflectionRLC();
  
private:
  Ui::dialogReflectionRLC *ui;
  void ReflectionRLC();
  //void LeftButDown();
  void RefRLCCancel();
  void RefRLCSeriesRLC();
  void RefRLCParallelRLC();
  void RefRLCSlope();
  void RefRLCTwoFreq();
  void RefRLCChooseSlope();
  void RefRLCChooseTwoFreq();
  void RefRLCFindResonance();
  void incFreq();
  void decFreq();
  void RefRLCCalcAndDisplayPointValues();
  void RefRLCAnalyze();
  void RefRLCHelp(QString h);
  void RefRLCExplainEnd();
};

#endif // DIALOGREFLECTIONRLC_H
