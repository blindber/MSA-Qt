#ifndef DIALOGANALYZEQ_H
#define DIALOGANALYZEQ_H

#include <QDialog>

namespace Ui {
class dialogAnalyzeQ;
}

class dialogAnalyzeQ : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogAnalyzeQ(QWidget *parent = 0);
  ~dialogAnalyzeQ();
  void AnalyzeQ();
  
private:
  Ui::dialogAnalyzeQ *ui;
  void analyzeQDone();
  void analyzeQAnalyze();

};

#endif // DIALOGANALYZEQ_H
