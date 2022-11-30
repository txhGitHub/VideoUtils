#ifndef LOCALFILE_PAGE_H
#define LOCALFILE_PAGE_H

#include <QWidget>

namespace Ui {
class LocalFile_Page;
}

class LocalFile_Page : public QWidget
{
    Q_OBJECT

public:
    explicit LocalFile_Page(QWidget *parent = nullptr);
    ~LocalFile_Page();

private:
    Ui::LocalFile_Page *ui;
    bool is_draw_boder = false;

    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;

    // QWidget interface
protected:
    void dragLeaveEvent(QDragLeaveEvent *event) override;
};

#endif // LOCALFILE_PAGE_H
