#ifndef DIALOGVNAREF_H
#define DIALOGVNAREF_H

#include <QDialog>

namespace Ui {
class dialogVNARef;
}

class dialogVNARef : public QDialog
{
  Q_OBJECT
  
public:
  explicit dialogVNARef(QWidget *parent = 0);
  ~dialogVNARef();
  
private:
  Ui::dialogVNARef *ui;

  void menuVNARef();
  void VNARefFinished();
  void VNAapplyLineOn();
  void VNAapplyLineOff();
  void VNAapplyBaseLineOn();
  void VNAapplyBaseLineOff();
  void VNAapplyNoneOn();
  void VNAapplyNoneOff();

};

#endif // DIALOGVNAREF_H
