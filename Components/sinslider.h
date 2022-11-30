#ifndef SINSLIDER_H
#define SINSLIDER_H

#include <QSlider>



class SinSlider : public QSlider
{
    Q_OBJECT
public:
    explicit SinSlider(QWidget *parent = nullptr);
    explicit SinSlider(Qt::Orientation orientation, QWidget *parent = nullptr);


    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
    void friendListEleHoverSlot(bool isHover);

    //信号没有返回值，没有定义
signals:
    void friendListEleHover(bool isHover);
};

#endif // SINSLIDER_H
