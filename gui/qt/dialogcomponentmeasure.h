#ifndef DIALOGCOMPONENTMEASURE_H
#define DIALOGCOMPONENTMEASURE_H

#include <QDialog>

namespace Ui {
class dialogComponentMeasure;
}

class dialogComponentMeasure : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogComponentMeasure(QWidget *parent = 0);
  ~dialogComponentMeasure();
  void menuComponentMeasure();
  
private:
  Ui::dialogComponentMeasure *ui;
  void ComponentCloseBox(QString h);
  void ComponentInvalidateCal();
  void ComponIncFreq();
  void ComponDecFreq();
  void ComponDoCalScan();
  void ComponSeriesSet();
  void ComponSeriesClear();
  void ComponShuntSet();
  void ComponShuntClear();
  void ComponResistorSet(QString h);
  void ComponCapacitorSet(QString h);
  void ComponInductorSet(QString h);
  void ComponentStop(QString h);
  void ComponentSetItemsForMeasuring();
  void ComponentSetItemsForMeasuringDone();
  void ComponentAcquireAndMeasure();
  void ComponentMeasure();
  void ComponentDone();
  void ComponentExplain();
  void ComponentHelpDone();
  void ComponentAnalyzeData();// componR0, componType$, componIsSeries, byref componValue, byref serRes, byref pointNum
  void ComponentGetValue();// componR0, isSeries, scanStep, componType$, byref value, byref serRes


};

#endif // DIALOGCOMPONENTMEASURE_H
