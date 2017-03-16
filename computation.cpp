#include "computation.h"

computation::computation(QObject *parent, QString filename, QList<int> elevList)
{
    fileName = filename;
    elevationList = elevList;
}

computation::~computation()
{

}

void computation::slotCompute()
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug("Opening error.");
    }
    QTextStream stream(&file);

    QMap<int, QStringList> azimuthMap;

    int i = 0;
    while(!stream.atEnd()){
        QString line = stream.readLine();
        qDebug("alma");
        azimuthMap[elevationList.at(i)] = line.split("\t");
        i++;
    }
    file.close();

}
