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
    explicit computation(QString filename = NULL, QList<int> elevList = QList<int>());
    ~computation();

signals:
    void singalReady(QMap<int, QPair<double, double> >);

public slots:
    void slotCompute();

private:
    QString fileName;
    QList<int> elevationList;
    double centralAngleCorrection(double num);
};

#endif // COMPUTATION_H
