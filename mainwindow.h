#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QDebug>

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

signals:
    void signalOutFileReady(QString filename);

public slots:
    void slotComputingReady(QMap<int, QPair<double, double> > aveStD, QString filename);

private slots:

    void on_loadPushButton_clicked();
    void slotOutFileReady(QString filename);

private:
    Ui::MainWindow *ui;
    QList<int> elevationList;
    void plotGnuPlot(QString outfileName, QString outfileNameps, QString plotfileName, QString xlabel, QString ylabel, QString xrange, QString yrange);
    void refreshImage(QString imageName);
    void fitImage(QImage &image, QGraphicsView *view);

    QGraphicsScene scene;
};

#endif // MAINWINDOW_H
