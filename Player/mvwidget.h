#ifndef MVWIDGET_H
#define MVWIDGET_H

#include <QWidget>
#include "videoplayer.h"
#include <QImage>
#include <QPaintEvent>


namespace Ui {
class MVWidget;
}

class MVWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MVWidget(QWidget *parent = nullptr);
    ~MVWidget();
public slots:
    void slotGetOneFrame(QImage img);
public:
    void paintEvent(QPaintEvent *event);
private:
    Ui::MVWidget *ui;
    //============以下变量为处理图像
    VideoPlayer *mPlayer; //播放线程
    QImage mImage; //记录当前的图像
};

#endif // MVWIDGET_H
