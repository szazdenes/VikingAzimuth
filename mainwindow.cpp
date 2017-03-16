#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QThread *thread = new QThread();
    computation *work = new computation();
    work->moveToThread(thread);

    connect(thread, &QThread::started, work, &computation::slotCompute);
    connect(thread, &QThread::finished, work, &computation::deleteLater);

    work->slotCompute();
    thread->start();

}

MainWindow::~MainWindow()
{
    delete ui;
}
