#include "TimerTest.h"

TimerTest::TimerTest(QObject *parent)
    : QObject(parent)
    , timer_(new QTimer(parent))
{
    connect(timer_,
            SIGNAL(timeout()),
            this,
            SLOT(slotTimeOut()));
}
