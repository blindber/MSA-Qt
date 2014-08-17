#ifndef DIALOGSPECIALTESTS_H
#define DIALOGSPECIALTESTS_H

#include <QDialog>

namespace Ui {
class dialogSpecialTests;
}

class dialogSpecialTests : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogSpecialTests(QObject *dad, QWidget *parent = 0);
  ~dialogSpecialTests();
  
private slots:
  void on_DDS1_clicked();

  void on_DDS3_clicked();

  void on_dds3track_clicked();

  void on_dds1track_clicked();

  void on_pdminv_clicked();

  void on_insert_clicked();

  void on_prevnalin_clicked();

  void on_lpttest_clicked();

  void on_cftest_clicked();


  void on_pushButtonSetSwitches_clicked();

  void on_pushButtonSelectFilter_clicked();

private:
  Ui::dialogSpecialTests *ui;

signals:
  void setSwitches(int );
  void CommandFilterSlimCBUSB(int);
  void setDDS1(double );
};

#endif // DIALOGSPECIALTESTS_H
