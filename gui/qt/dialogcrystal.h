#ifndef DIALOGCRYSTAL_H
#define DIALOGCRYSTAL_H

#include <QDialog>

namespace Ui {
class dialogCrystal;
}

class dialogCrystal : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogCrystal(QWidget *parent = 0);
  ~dialogCrystal();

  void menuCrystalAnalysis();

  
private:
  Ui::dialogCrystal *ui;
  void crystFull();
  void crystSeries();
  void crystalList();
  void crystalSetID();
  void crystZoom();
  void crystalFindPoints();
  void crystalRescan();
  void crystalAnalyze();
  void crystalCancel();
  void CrystalListClosed(QString h);
  void CrystalExplain(QString h);
  void CrystalExplainEnd();
};

#endif // DIALOGCRYSTAL_H
