#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gnuplot-cpp/gnuplot_i.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this, &MainWindow::signalOutFileReady, this, &MainWindow::slotOutFileReady);

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
    out << "#elevation" << "\t" << "average" << "\t" << "StD" << "\n";

    foreach(int currentkey, aveStD.keys()){
        out << QString::number(currentkey) << "\t" << QString::number(aveStD[currentkey].first) << "\t" << QString::number(aveStD[currentkey].second) << "\n";
    }
    outfile.close();
    emit signalOutFileReady(filename);
}


void MainWindow::on_loadPushButton_clicked()
{
   QString filename = QFileDialog::getOpenFileName(this, "Open file", "../", "*result.dat");

   filename.remove(".dat");

   plotGnuPlot(filename + ".png", filename + ".ps", filename + ".dat", QString("elevation"), QString("azimuth error"), QString("5:60"), QString("-10:10"));
   refreshImage(filename + ".png");
}

void MainWindow::slotOutFileReady(QString filename)
{
    ui->listWidget->addItem(filename + " processed.");
    ui->listWidget->scrollToBottom();
}

void MainWindow::plotGnuPlot(QString outfileName, QString outfileNameps, QString plotfileName, QString xlabel, QString ylabel, QString xrange, QString yrange)
{
    QString com = QString("reset; set terminal pngcairo size 1200, 900 enhanced font 'Times-Roman,12'; set output '%1'; set style line 1 lc rgb '#000000' pt 7 ps 1.5 lt 1 lw 0.6; set xlabel '%2'; set ylabel '%3'; set xrange[%4]; set yrange [%5]; set arrow from 5, 0 to 60, 0 nohead lt 0 lc 'black' lw 1; plot '%6' using 1:2 with lp ls 1 notitle, '' using 1:2:3 with yerrorbars ls 1 notitle; exit").arg(outfileName).arg(xlabel).arg(ylabel).arg(xrange).arg(yrange).arg(plotfileName);
    QString com2 = QString("reset; set terminal postscript enhanced landscape font 'Times-Roman,12'; set output '%1'; set style line 1 lc rgb '#000000' pt 7 ps 1.5 lt 1 lw 0.6; set xlabel '%2'; set ylabel '%3'; set xrange[%4]; set yrange [%5]; set arrow from 5, 0 to 60, 0 nohead lt 0 lc 'black' lw 1; plot '%6' using 1:2 with lp ls 1 notitle, '' using 1:2:3 with yerrorbars ls 1 notitle; exit").arg(outfileNameps).arg(xlabel).arg(ylabel).arg(xrange).arg(yrange).arg(plotfileName);
    Gnuplot gp, gp2;
    gp.cmd(com.toStdString());
    gp2.cmd(com2.toStdString());
    QApplication::processEvents();
}

void MainWindow::refreshImage(QString imageName)
{
    ui->graphicsView->setScene(&scene);
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorViewCenter);

    QImage currentImage(imageName);
    QImage image = QImage(currentImage.size(), QImage::Format_ARGB32_Premultiplied);

    QPainter painter(&image);
    painter.drawImage(0, 0, currentImage);
    painter.end();

    scene.clear();
    scene.addPixmap(QPixmap::fromImage(image));
    fitImage(image, ui->graphicsView);
}

void MainWindow::fitImage(QImage &image, QGraphicsView *view)
{
    QRectF imageRect = image.rect();
    QRectF rect = view->viewport()->rect();
    double fitSize = qMin<double>(rect.width() / imageRect.width(), rect.height() / imageRect.height());

    QMatrix matrix = view->matrix().inverted();
    QRectF visibleRect = matrix.mapRect(view->viewport()->rect());
    double zoom = qMin<double>(visibleRect.width() / rect.width(), visibleRect.height() / rect.height());
    zoom *= fitSize;

    view->scale(zoom, zoom);
}

void MainWindow::on_processingPushButton_clicked()
{
    QStringList fileNames = QStringList() << "../EA.dat" << "../FA.dat" << "../HG.dat" << "../KB.dat" << "../PA.dat" << "../PI.dat" << "../SD.dat" << "../SM.dat" << "../SS.dat" << "../ST.dat" << "../TP.dat";

    QThread *thread = new QThread;

    foreach(QString currentFile, fileNames){
        computation *work = new computation(currentFile, elevationList);
        work->moveToThread(thread);
        connect(thread, &QThread::started, work, &computation::slotCompute);
        connect(work, &computation::signalReady, this, &MainWindow::slotComputingReady);
        connect(thread, &QThread::finished, work, &computation::deleteLater);
    }

    thread->start();
    thread->quit();
}
