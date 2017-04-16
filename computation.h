#ifndef COMPUTATION_H
#define COMPUTATION_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QtMath>

class computation : public QObject
{
    Q_OBJECT
public:
    explicit computation(QString filename = NULL, QList<int> elevList = QList<int>(), bool checkstate = false, bool linearcheck = false);
    ~computation();

signals:
    void signalReady(QMap<int, QPair<double, double> > aveStD, QString filename);
    void signalSendCorrectedAzimuth(QMap<int, QPair<double, double> > corrAzimuth, QMap<int, double> trueAzimuth);

public slots:
    void slotCompute();

private:
    QString fileName;
    QList<int> elevationList;
    bool checkState, linearCheck;
    double centralAngleCorrection(double num);
    double azimuthCorrection(double azimuth, double elevation, bool isElevationCorrOn, bool isLinearCheckOn);
    double getAverage(QList<double> &list);
    double getStD(QList<double> &list);
    double linearCorrection(double elev);
};

#endif // COMPUTATION_H
