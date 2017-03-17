#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<QMap<int, QPair<double, double> > >("QMap<int, QPair<double, double> >");

    QFile file("../okol_sorrend.dat");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug("Opening error.");
    }
    elevationList.clear();
    QTextStream stream(&file);
    while(!stream.atEnd()){
        QString line = stream.readLine();
        elevationList.append(QString(line.split("\t").last()).toInt());
    }
    file.close();

    QThread *thread = new QThread();
    computation *work = new computation("../SD.dat", elevationList);
    work->moveToThread(thread);

    connect(thread, &QThread::started, work, &computation::slotCompute);
    connect(work, &computation::signalReady, this, &MainWindow::slotComputingReady);
    connect(thread, &QThread::finished, work, &computation::deleteLater);

    thread->start();
    QApplication::processEvents();
    thread->quit();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotComputingReady(QMap<int, QPair<double, double> > aveStD, QString filename)
{
    filename.remove(".dat").append("_result.dat");
    QFile outfile(filename);

    if(!outfile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug("Opening error.");
    }
    elevationList.clear();
    QTextStream out(&outfile);
    out << "elevation" << "\t" << "average" << "StD" << "\n";

    foreach(int currentkey, aveStD.keys()){
        out << QString::number(currentkey) << "\t" << QString::number(aveStD[currentkey].first) << "\t" << QString::number(aveStD[currentkey].second) << "\n";
    }
    outfile.close();
}
