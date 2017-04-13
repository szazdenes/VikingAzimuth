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
    qRegisterMetaType<QMap<int, double>>("QMap<int, double>");

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

    QTextStream out(&outfile);
    out << "#elevation" << "\t" << "average" << "\t" << "StD" << "\n";

    foreach(int currentkey, aveStD.keys()){
        out << QString::number(currentkey) << "\t" << QString::number(aveStD[currentkey].first) << "\t" << QString::number(aveStD[currentkey].second) << "\n";
    }
    outfile.close();

    filename.remove(".dat");
    QString dataFilename = filename;
    filename.insert(3, "results/");
    QString outFilename = filename;
    if(ui->perspectiveCheckBox->isChecked())
        outFilename.append("_elev_corr");

    plotGnuPlot(outFilename + ".png", outFilename + ".ps", dataFilename + ".dat", QString("elevation"), QString("azimuth error"), QString("5:60"), QString("-10:10"));

    emit signalOutFileReady(filename);
}

void MainWindow::slotCorrectedAzimuth(QMap<int, QPair<double, double> > corrAzimuth, QMap<int, double> trueAzimuth)
{
    QFile outfile("../correctedAzimuth.dat");

    if(!outfile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug("Opening error.");
    }

    QTextStream out(&outfile);
    out << "#elevation" << "\t" << "corrected azimuth" << "\t" << "corrected azimuth + elevation correction" << "\t" << "True azimuth" << "\n";

    foreach(int currentkey, corrAzimuth.keys()){
        out << QString::number(currentkey) << "\t" << QString::number(corrAzimuth[currentkey].first) << "\t" << QString::number(corrAzimuth[currentkey].second) << "\t" << trueAzimuth[currentkey] << "\n";
    }
    outfile.close();

    QString filename = outfile.fileName();
    filename.remove(".dat");

    plotCorrectedAzimuth(filename + ".png", filename + ".ps", filename + ".dat", QString("elevation"), QString("azimuth"), QString("5:60"), QString("-60:60"));

}


void MainWindow::on_loadPushButton_clicked()
{
   QString filename = QFileDialog::getOpenFileName(this, "Open file", "../results", "*.png");
   refreshImage(filename);
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

void MainWindow::plotCorrectedAzimuth(QString outfileName, QString outfileNameps, QString plotfileName, QString xlabel, QString ylabel, QString xrange, QString yrange)
{
    QString com = QString("reset; set terminal pngcairo size 1200, 900 enhanced font 'Times-Roman,12'; set output '%1'; set style line 1 lc rgb '#0000ff' pt 7 ps 1.5 lt 1 lw 0.6; set style line 2 lc rgb '#ff0000' pt 7 ps 1.5 lt 1 lw 0.6; set style line 3 lc rgb '#000000' pt 7 ps 1.5 lt 1 lw 0.6; set xlabel '%2'; set ylabel '%3'; set xrange[%4]; set yrange [%5]; set arrow from 5, 0 to 60, 0 nohead lt 0 lc 'black' lw 1; plot '%6' using 1:2 with lp ls 1 notitle, '' using 1:3 with lp ls 2 notitle, '' using 1:4 with lp ls 3 notitle; exit").arg(outfileName).arg(xlabel).arg(ylabel).arg(xrange).arg(yrange).arg(plotfileName);
    QString com2 = QString("reset; set terminal postscript enhanced landscape font 'Times-Roman,12'; set output '%1'; set style line 1 lc rgb '#0000ff' pt 7 ps 1.5 lt 1 lw 0.6; set style line 2 lc rgb '#ff0000' pt 7 ps 1.5 lt 1 lw 0.6; set style line 3 lc rgb '#000000' pt 7 ps 1.5 lt 1 lw 0.6; set xlabel '%2'; set ylabel '%3'; set xrange[%4]; set yrange [%5]; set arrow from 5, 0 to 60, 0 nohead lt 0 lc 'black' lw 1; plot '%6' using 1:2 with lp ls 1 notitle, '' using 1:3 with lp ls 2 notitle, '' using 1:4 with lp ls 3 notitle; exit").arg(outfileNameps).arg(xlabel).arg(ylabel).arg(xrange).arg(yrange).arg(plotfileName);
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
    if(!QDir("../results").exists())
        QDir().mkdir("../results");

    QStringList fileNames = QStringList() << "../EA.dat" << "../FA.dat" << "../HG.dat" << "../KB.dat" << "../PA.dat" << "../PI.dat" << "../SD.dat" << "../SM.dat" << "../SS.dat" << "../ST.dat" << "../TP.dat" << "../allazimuth.dat" << "../allazimuth_noEA.dat";

    bool checkstate;
    if(ui->perspectiveCheckBox->isChecked())
        checkstate = true;
    if(!ui->perspectiveCheckBox->isChecked())
        checkstate = false;

    QThread *thread = new QThread;
    computation *work;
    foreach(QString currentFile, fileNames){
        work = new computation(currentFile, elevationList, checkstate);
        work->moveToThread(thread);
        connect(thread, &QThread::started, work, &computation::slotCompute);
        connect(work, &computation::signalReady, this, &MainWindow::slotComputingReady);
        connect(thread, &QThread::finished, work, &computation::deleteLater);
    }

    connect(work, &computation::signalSendCorrectedAzimuth, this, &MainWindow::slotCorrectedAzimuth);
    thread->start();
    thread->quit();

    ui->listWidget->addItem("--------------------------------------");
    ui->listWidget->scrollToBottom();
}
