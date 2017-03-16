#ifndef COMPUTATION_H
#define COMPUTATION_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class computation : public QObject
{
    Q_OBJECT
public:
    explicit computation(QObject *parent = 0, QString filename = NULL, QList<int> elevList = QList<int>());
    ~computation();

signals:
    void singalReady(QMap<int, QPair<double, double> >);

public slots:
    void slotCompute();

private:
    QString fileName;
    QList<int> elevationList;
};

#endif // COMPUTATION_H
