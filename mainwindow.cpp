#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "log.h"
#include <stdio.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&m_QTimer, &QTimer::timeout , this, &MainWindow::slotUpdate);
    m_QTimer.start(40);
    m_pvideoController = new VideoController();
    m_pvideoController->setVideoCallback(this);
    std::string path = "C:\\Users\\pc\\Desktop\\kongfu.mp4";
//    std::string path = "C:\\Users\\pc\\Desktop\\CXX\\VideoTools\\videosrc\\VID20211109055043.mp4";
    m_pvideoController->startPlay(path);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotUpdate()
{
    QImage image;
    image.load("E:/tt/ll.jpeg");
    ui->widget->setImage(image);
}

void MainWindow::onDisplayVideo(std::shared_ptr<VideoFrame> videoFrame)
{
    DEBUG_INFO("start paly");
    static int count = 0;
    if(count <  20 && count > 15)
    {
        std::string str = "kongfu_yuv_" + std::to_string(videoFrame->getWidth()) + "x" + std::to_string(videoFrame->getHeight()) + "_" +std::to_string(count) + ".yuv";
        std::string path_str = "C:\\Users\\pc\\Desktop\\image\\";
        FILE *fp = fopen((path_str+str).c_str(), "w");
        if(fp == nullptr)
            DEBUG_INFO("file open failed!");
        fwrite(videoFrame->buffer() , (videoFrame->getWidth() * videoFrame->getHeight() * 3 /2), 1, fp);
        fclose(fp);
    }
    count++;
}

