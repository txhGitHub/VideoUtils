#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "videocontroller.h"

enum VIDEO_SRC {
    CAPTURE_DEVICE = 0,
    NETWORK_STREAMING = 1,
    LOCAL_FILE = 2
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow ,public VideoCallback
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void onDisplayVideo(std::shared_ptr<VideoFrame> videoFrame);
    void create_connect();
    void config_window_ui();

private:
    Ui::MainWindow *ui;
    QTimer m_QTimer;
    VideoController* m_pvideoController;
    int m_x, m_y;

    // QWidget interface
protected:
    void mouseMoveEvent(QMouseEvent *event) override;


private slots:
    void on_volume_clicked();
    void select_video_src();

    void on_video_list_2_clicked();
};
#endif // MAINWINDOW_H
