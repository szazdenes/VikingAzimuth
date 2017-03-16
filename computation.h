#ifndef COMPUTATION_H
#define COMPUTATION_H

#include <QObject>
#include <QDebug>
#include <QThread>

class computation : public QObject
{
    Q_OBJECT
public:
    explicit computation(QObject *parent = 0);
    ~computation();

public slots:
    void slotCompute();
};

#endif // COMPUTATION_H
