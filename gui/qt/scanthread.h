#ifndef SCANTHREAD_H
#define SCANTHREAD_H

#include <QThread>

class scanThread : public QThread
{
  Q_OBJECT
public:
  explicit scanThread(QObject *parent = 0);
  
signals:
  
public slots:
 // void Restart();
  //void Halted();
  //void StartSweep();

};

#endif // SCANTHREAD_H
