#include "infoplane.h"
#include "ui_infoplane.h"

InfoPlane::InfoPlane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoPlane)
{
    ui->setupUi(this);
}

InfoPlane::~InfoPlane()
{
    delete ui;
}
