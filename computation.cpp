#include "computation.h"

computation::computation(QObject *parent)
{

}

computation::~computation()
{

}

void computation::slotCompute()
{
    qDebug() << "filename" << QThread::currentThreadId();
}
