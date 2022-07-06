#include "mvwidget.h"
#include "ui_mvwidget.h"
#include "widget.h"
#include <QPainter>
//支持 .mp4  .mkv  .ts  .flv 封装格式播放

MVWidget::MVWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MVWidget)
{
    ui->setupUi(this);
    //播放视频
    //mPlayer = new VideoPlayer;
    //connect(mPlayer,&VideoPlayer::sig_GetOneFrame,parent,&Widget::slotGetOneFrame);

    //this->show();
    //mPlayer->setFileName("1.mp4");
    //mPlayer->setFileName("1.mkv");
    //mPlayer->setFileName("1.ts");
    //mPlayer->setFileName("1.flv");
    //mPlayer->startPlay();
}

MVWidget::~MVWidget()
{
    delete ui;

    //释放mPlayer
//    if(mPlayer){
//        delete mPlayer;
//        mPlayer = nullptr;
//    }
}

void MVWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.drawRect(0, 0, this->width(), this->height()); //先画成黑色

    if (mImage.size().width() <= 0) return;

    ///将图像按比例缩放成和窗口一样大小
    QImage img = mImage.scaled(this->size(),Qt::KeepAspectRatio);

    int x = this->width() - img.width();
    int y = this->height() - img.height();

    x /= 2;
    y /= 2;

    painter.drawImage(QPoint(x,y),img); //画出图像

}

void MVWidget::slotGetOneFrame(QImage img)
{
    mImage = img;
    update(); //调用update将执行 paintEvent函数
}
