#include "sinslider.h"
#include "log.h"
#include "utils.h"

#include <QEvent>

SinSlider::SinSlider(QWidget *parent)
{

}

SinSlider::SinSlider(Qt::Orientation orientation, QWidget *parent)
{
    this->setAttribute(Qt::WA_Hover, true);
    this->installEventFilter(this);       //安装事件过滤器

    connect(this, SIGNAL(friendListEleHover(bool)), this, SLOT(friendListEleHoverSlot(bool)));
}

bool SinSlider::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::HoverEnter)
    {
        emit friendListEleHover(true);
                return true;
    }
    else if( event->type() == QEvent::HoverLeave )
    {
        emit friendListEleHover(false);
                return true;
    }

    //最好是调用父类方法，避免重写后，导致原来的逻辑改变
    return QWidget::eventFilter(watched, event);
}

void SinSlider::friendListEleHoverSlot(bool isHover)
{
    if(!isHover)
    {
//        mSleep(500);
        this->setVisible(false);
    }
}


