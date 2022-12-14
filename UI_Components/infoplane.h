#ifndef INFOPLANE_H
#define INFOPLANE_H

#include <QWidget>

namespace Ui {
class InfoPlane;
}

class InfoPlane : public QWidget
{
    Q_OBJECT

public:
    explicit InfoPlane(QWidget *parent = nullptr);
    ~InfoPlane();

private:
    Ui::InfoPlane *ui;
};

#endif // INFOPLANE_H
