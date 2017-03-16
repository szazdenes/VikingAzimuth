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


    /*convert string list to double list, use central angle correction, calculate delta azimuth with average and StD*/
    QMap<int, QList<double> > azimuthDoubleMap, deltaAzimuthMap;
    QMap<int, QPair<double, double> >  aveStDMap;
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
            aveStDMap[currentKey].first = getAverage(deltaList); //average
            aveStDMap[currentKey].second = getStD(deltaList); //StD
        }
    }
    else{
        qDebug("Error with azimuthMap");
        return;
    }

    emit signalReady(aveStDMap);


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
    else{
        QMap<QString, double> xMap, xMapValue;
        xMapValue["x1"] = x1;
        xMapValue["x2"] = x2;
        xMapValue["x3"] = x3;
        xMapValue["x4"] = x4;

        if(num/qAbs(num) == x1/qAbs(x1)){
            xMap["x1"] = qAbs(qAbs(num)-qAbs(x1));
        }
        if(num/qAbs(num) == x2/qAbs(x2)){
            xMap["x2"] = qAbs(qAbs(num)-qAbs(x2));
        }
        if(num/qAbs(num) == x3/qAbs(x3)){
            xMap["x3"] = qAbs(qAbs(num)-qAbs(x3));
        }
        if(num/qAbs(num) == x4/qAbs(x4)){
            xMap["x4"] = qAbs(qAbs(num)-qAbs(x4));
        }

        QList<double> xList = xMap.values();
        qSort(xList);

        QString key = xMap.key(xList.at(0));
        result = xMapValue[key];
    }

    return result;
}

double computation::getAverage(QList<double> &list)
{
    double result = 0;
    for (int i = 0; i < list.size(); i++)
        result += (double)list.at(i) / (double)list.size();
    return result;
}

double computation::getStD(QList<double> &list)
{
    double result = 0;
    double average = getAverage(list);
    for (int i = 0; i < list.size(); i++)
        result += (((double)list.at(i) - average)*((double)list.at(i) - average)) / (double)list.size();
    result = qSqrt(result);
    return result;
}
