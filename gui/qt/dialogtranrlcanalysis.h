#ifndef DIALOGTRANRLCANALYSIS_H
#define DIALOGTRANRLCANALYSIS_H

#include <QDialog>

namespace Ui {
class dialogTranRLCAnalysis;
}

class dialogTranRLCAnalysis : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogTranRLCAnalysis(QWidget *parent = 0);
  ~dialogTranRLCAnalysis();
  
private:
  Ui::dialogTranRLCAnalysis *ui;
  void TranRLCAnalysis();
  void tranRLCUseTop();
  void tranRLCUseBot();
  void tranRLCSeriesFix();
  void tranRLCShuntFix();
  void tranRLCSeriesRLC();
  void tranRLCParallelRLC();
  void tranRLCExplainNotchAndDB();
  void tranRLCAnalyze();
  void tranRLCCancel();
  void tranRLCExplain(QString h);
  void tranRLCExplainEnd();

};

#endif // DIALOGTRANRLCANALYSIS_H
