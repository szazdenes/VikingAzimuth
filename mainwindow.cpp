#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
    computation *work = new computation(this, "../SD.dat", elevationList);
    work->moveToThread(thread);

    connect(thread, &QThread::started, work, &computation::slotCompute);
    connect(thread, &QThread::finished, work, &computation::deleteLater);

    thread->start();

}

MainWindow::~MainWindow()
{
    delete ui;
}
