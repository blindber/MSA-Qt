#ifndef DIALOGMANAGETESTSETUPS_H
#define DIALOGMANAGETESTSETUPS_H

#include <QDialog>

namespace Ui {
class dialogManageTestSetups;
}

class dialogManageTestSetups : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogManageTestSetups(QWidget *parent = 0);
  ~dialogManageTestSetups();
  void ManageTestSetups();
  
private:
  Ui::dialogManageTestSetups *ui;
  void setupSave();
  void setupLoad();
  void setupDelete();
  void setupDone();
  void setupFillList();
  void setupListClick();
  void setupCreateName();
  QString SetupName();
  void SetupOpenFile();
  void SetupGetDescription();

};

#endif // DIALOGMANAGETESTSETUPS_H
