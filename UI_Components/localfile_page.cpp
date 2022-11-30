#include "localfile_page.h"
#include "ui_localfile_page.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDebug>
#include <QPainter>

LocalFile_Page::LocalFile_Page(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LocalFile_Page)
{
    ui->setupUi(this);
    setAcceptDrops(true);//set this widget able to accept drop events
}

LocalFile_Page::~LocalFile_Page()
{
    delete ui;
}

void LocalFile_Page::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())//判断数据类型
   {
       event->acceptProposedAction();//接收该数据类型拖拽事件
       //画边框

   }
   else
   {
       event->ignore();//忽略
   }
   is_draw_boder = true;
   repaint();
}

void LocalFile_Page::dropEvent(QDropEvent *event)
{

    foreach (const QUrl &url, event->mimeData()->urls()) {
        QString fileName = url.toLocalFile();
        qDebug() << "file--:" << fileName;
    }
    is_draw_boder = false;
    repaint();
}

void LocalFile_Page::paintEvent(QPaintEvent *event)
{

    if(is_draw_boder == true) {
        QPainter p(this);
        p.setPen(QColor("blue")); //设置画笔记颜色
        p.drawRect(0, 0, width() -1, height() -1); //绘制边框
    } else {
                 QPainter p(this);
                 p.setPen(QColor(255, 255, 255, 0)); //设置画笔记颜色
                 p.drawRect(0, 0, width() -1, height() -1); //绘制边框
    }
    QWidget::paintEvent(event);
}

void LocalFile_Page::dragLeaveEvent(QDragLeaveEvent *event)
{
    is_draw_boder = false;
    repaint();
}
