#include <QCoreApplication>
#include <QTimer>
#include <iostream>
#include <QObject>
#include "TimerTest.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TimerTest* tt = new TimerTest(&a);
    tt->start();
    return a.exec();
}

