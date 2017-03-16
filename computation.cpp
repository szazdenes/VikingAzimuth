#include "computation.h"

computation::computation(QString filename, QList<int> elevList)
{
    fileName = filename;
    elevationList = elevList;
}

computation::~computation()
{

}

void computation::slotCompute()
{
    /*read azimuth data*/
    QMap<int, QStringList> azimuthMap;
    if(!fileName.isEmpty() && !fileName.isNull()){
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug("Opening error.");
        }
        QTextStream stream(&file);

        int i = 0;
        while(!stream.atEnd()){
            QString line = stream.readLine();
            azimuthMap[elevationList.at(i)] = line.split("\t");
            i++;
        }
        file.close();
    }
    else{
        qDebug("Not valid filename.");
        return;
    }

    /*read true azimuth data from file*/
    QMap<int, double> trueAzimuthMap;
    QFile azimuthFile("../trueAzimuth.dat");
    if(!azimuthFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug("Opening error.");
    }
    QTextStream stream(&azimuthFile);

    int j = 0;
    while(!stream.atEnd()){
        QString line = stream.readLine();
        trueAzimuthMap[elevationList.at(j)] = QString(line.split("\t").last()).toDouble();
        j++;
    }
    azimuthFile.close();


    /*convert string list to double list and use central angle correction*/
    QMap<int, QList<double> > azimuthDoubleMap, deltaAzimuthMap;
    if(!azimuthMap.isEmpty() && azimuthMap.size() == elevationList.size()){
        foreach(int currentKey, azimuthMap.keys()){
            QList<double> currentList, deltaList;
            foreach(QString current, azimuthMap[currentKey]){
                double correctedValue = centralAngleCorrection(current.toDouble());
                currentList.append(correctedValue);
                deltaList.append(correctedValue - trueAzimuthMap[currentKey]);
            }
            azimuthDoubleMap[currentKey] = currentList;
            deltaAzimuthMap[currentKey] = deltaList;
        }
    }
    else{
        qDebug("Error with azimuthMap");
        return;
    }
}

double computation::centralAngleCorrection(double num)
{
    double result, x1, x2, x3, x4;
    double cosB = qCos(num*M_PI/180.0);

    x1 = qAsin((1.0/130.0)*(-24*qPow(cosB, 2) - 1) - (1.0/13.0)*qSqrt(qPow(cosB, 2) - qPow(cosB, 4))
            -(1.0/130.0)*qSqrt(476*qPow(cosB, 4)-16076*qPow(cosB, 2)+(7220*qPow(cosB, 4)-6740*qPow(cosB, 2)-480*qPow(cosB, 6))/qSqrt(qPow(cosB, 2)-qPow(cosB, 4)) + 16225))*180.0/M_PI;

    x2 = qAsin((1.0/130.0)*(-24*qPow(cosB, 2) - 1) - (1.0/13.0)*qSqrt(qPow(cosB, 2) - qPow(cosB, 4))
            +(1.0/130.0)*qSqrt(476*qPow(cosB, 4)-16076*qPow(cosB, 2)+(7220*qPow(cosB, 4)-6740*qPow(cosB, 2)-480*qPow(cosB, 6))/qSqrt(qPow(cosB, 2)-qPow(cosB, 4)) + 16225))*180.0/M_PI;

    x3 = qAsin((1.0/130.0)*(-24*qPow(cosB, 2) - 1) - (1.0/13.0)*qSqrt(qPow(cosB, 2) - qPow(cosB, 4))
            -(1.0/130.0)*qSqrt(476*qPow(cosB, 4)-16076*qPow(cosB, 2)+(-7220*qPow(cosB, 4)+6740*qPow(cosB, 2)+480*qPow(cosB, 6))/qSqrt(qPow(cosB, 2)-qPow(cosB, 4)) + 16225))*180.0/M_PI;

    x4 = qAsin((1.0/130.0)*(-24*qPow(cosB, 2) - 1) - (1.0/13.0)*qSqrt(qPow(cosB, 2) - qPow(cosB, 4))
            +(1.0/130.0)*qSqrt(476*qPow(cosB, 4)-16076*qPow(cosB, 2)+(-7220*qPow(cosB, 4)+6740*qPow(cosB, 2)+480*qPow(cosB, 6))/qSqrt(qPow(cosB, 2)-qPow(cosB, 4)) + 16225))*180.0/M_PI;

    if(num == 0)
        result = 0;
    if(num < 0)
        result = x1;
    if(num > 0)
        result = x4;

    return result;
}
