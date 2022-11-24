#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "log.h"
#include <stdio.h>
#include <QToolButton>
#include <QSlider>
#include <QWidgetAction>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
//    this->installEventFilter(this);
//    connect(&m_QTimer, &QTimer::timeout , this, &MainWindow::slotUpdate);
//    m_QTimer.start(40);
    m_pvideoController = new VideoController();
    m_pvideoController->setVideoCallback(this);
    std::string path = "C:\\Users\\pc\\Desktop\\kongfu.mp4";
//    std::string path = "C:\\Users\\pc\\Desktop\\CXX\\VideoTools\\videosrc\\VID20211109055043.mp4";
    m_pvideoController->startPlay(path);
    m_x = 0;
    m_y = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotUpdate()
{
//    QImage image;
//    image.load("E:/tt/ll.jpeg");
//    ui->widget->setImage(image);
}

void MainWindow::onDisplayVideo(std::shared_ptr<VideoFrame> videoFrame)
{
    ui->widget->showInputFrame(videoFrame);
}


void MainWindow::on_volume_clicked()
{
    //设置控件的声明周期
    QSlider* slider = new QSlider(Qt::Vertical);
    slider->setRange(0, 100);
    slider->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint );
//    slider->setLayout(ui->playLayout);
    slider->setGeometry(ui->volume->mapToGlobal(QPoint(0, 0)).x() + (ui->volume->width() - (slider->width() / 35))/2,
                        ui->volume->mapToGlobal(QPoint(0, 0)).y() -  (slider->height() / 5) - 6,  (slider->width() / 35), (slider->height() / 5));
    slider->show();
    fflush(stdout);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->type() == QEvent::MouseMove)
      {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        ui->label->setText(QString("Mouse move (%1,%2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y()));
//        statusBar()->showMessage();
        ui->label->show();
      }
}
