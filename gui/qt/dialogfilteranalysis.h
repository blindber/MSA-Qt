#ifndef DIALOGFILTERANALYSIS_H
#define DIALOGFILTERANALYSIS_H

#include <QDialog>

namespace Ui {
class dialogFilterAnalysis;
}

class dialogFilterAnalysis : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogFilterAnalysis(QWidget *parent = 0);
  ~dialogFilterAnalysis();
  
private:
  Ui::dialogFilterAnalysis *ui;
  void SetFilterAnalysis();
  void LeftButDown();
  void filtNOP();
  void filtCancel();
  void filtFinished();

};

#endif // DIALOGFILTERANALYSIS_H
