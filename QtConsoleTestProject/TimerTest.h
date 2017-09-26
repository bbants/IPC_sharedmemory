#ifndef TIMERTEST_H
#define TIMERTEST_H

#include <QObject>
#include <iostream>
#include <QString>
#include <QTimer>

class TimerTest : public QObject
{
    Q_OBJECT
public:
    explicit TimerTest(QObject *parent = nullptr);

signals:

public slots:
    inline void slotTimeOut(){
        static int k = 0;
        std::cout << "pulse: " << QString::number(k++).toStdString() << std::endl;
    }
public:
    inline void start() {
        timer_->start(1000);
    }
private:
    QTimer* timer_;
};

#endif // TIMERTEST_H
