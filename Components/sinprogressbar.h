#ifndef SINPROGRESSBAR_H
#define SINPROGRESSBAR_H
#include <QProgressBar>

class SinProgressBar : public QProgressBar
{
    Q_OBJECT
public:
   explicit  SinProgressBar(QWidget* parent = nullptr);
};

#endif // SINPROGRESSBAR_H
