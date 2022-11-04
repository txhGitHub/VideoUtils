#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&m_QTimer, &QTimer::timeout , this, &MainWindow::slotUpdate);
    m_QTimer.start(40);
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

