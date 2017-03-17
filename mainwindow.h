#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QFile>
#include <QTextStream>

#include "computation.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slotComputingReady(QMap<int, QPair<double, double> > aveStD, QString filename);

private:
    Ui::MainWindow *ui;
    QList<int> elevationList;
};

#endif // MAINWINDOW_H
