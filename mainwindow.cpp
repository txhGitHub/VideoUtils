#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "log.h"
#include <stdio.h>
#include <QToolButton>
#include <QSlider>
#include <QWidgetAction>
#include <QMouseEvent>
#include <sinslider.h>
#include <infoplane.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    m_pvideoController = new VideoController();
    m_pvideoController->setVideoCallback(this);
    config_window_ui();
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

void MainWindow::onDisplayVideo(std::shared_ptr<VideoFrame> videoFrame)
{
    ui->video_widget->showInputFrame(videoFrame);
}

void MainWindow::on_volume_clicked()
{
    //设置控件的声明周期
    SinSlider* slider = new SinSlider(Qt::Vertical);
    slider->setRange(0, 100);
    slider->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint );
    slider->setGeometry(ui->volume_button->mapToGlobal(QPoint(0, 0)).x() + (ui->volume_button->width() - (slider->width() / 35))/2,
                        ui->volume_button->mapToGlobal(QPoint(0, 0)).y() -  (slider->height() / 5) - 10,  (slider->width() / 35), (slider->height() / 5));
    slider->show();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
//    if (event->type() == QEvent::MouseMove)
//      {
//        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
//        ui->label->setText(QString("Mouse move (%1,%2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y()));
////        statusBar()->showMessage();
//        ui->label->show();
//      }
}

void MainWindow::on_video_list_2_clicked()
{
    //显示视频信息面板
    InfoPlane* infoPlane = new InfoPlane();
    infoPlane->setGeometry(ui->centralwidget->mapToGlobal(QPoint(0, 0)).x() + ui->centralwidget->width(),
                        ui->centralwidget->mapToGlobal(QPoint(0, 0)).y(),  infoPlane->width(), infoPlane->height());
    infoPlane->show();
}
