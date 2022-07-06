#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QMediaPlayer>
#include "csqlite.h"
#include "lrc.h"
#include "Kernel/TCPKernel.h"
#include "videoplayer.h"
#include <QImage>
#include <QPaintEvent>
#include "mvwidget.h"


#define MAXMUSICSIZE 500
#define orderPlayer  0
#define randomPlayer 1
#define SinglePlayer 2

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
public:
    //友元类
    friend class lrc;
    bool initClient();
private slots:
    void on_pb_addMusic_clicked();

    void on_pb_pauseAndPlayer_clicked();

    void on_lw_musicList_doubleClicked(const QModelIndex &index);

    //void on_lw_musicList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void loadMusicList();
    void on_pb_delMusic_clicked();

    void on_pb_lastMusic_clicked();

    void on_pb_nextMusic_clicked();

    void slot_musicPosChanged(qint64 val);
    void on_slider_volume_valueChanged(int value);

    void on_pb_volume_clicked();

    void on_slider_music_valueChanged(int value);

    void on_pb_circulationStyle_clicked();

    void slot_moveString();
    void on_pb_lrc_clicked();

    //void on_pb_userLogin_clicked();


    void on_pb_playMV_clicked();
    void on_pb_userLogin_clicked();

public slots:
    void slotGetOneFrame(QImage img);

protected:
    void paintEvent(QPaintEvent *event);

private:

    Ui::Widget *ui;

    QString m_currentMusicName;//记录当前的音乐名
    QString m_currentSinger;//记录当前的歌手名
    QString m_musicName[MAXMUSICSIZE];//定义一个数组来存放音乐名，后面的MAXMUSICSIZE为宏，记录可以添加的最大音乐数
    int m_MusicNum;//记录总共有多少首音乐
    QMediaPlayer *m_player;//音乐播放类
    bool m_bIsPlayer;//音乐是否播放标志

    CSqlite *m_sql;
    int m_nowVolume;
    int m_lastVolume;
    bool m_bMusicValueChanged;//进度条值改变标志
    int m_playStyle;
    lrc *lrcUi;
    int m_pos;//记录字符串移动的位置
    IKernel *m_tcpKernel;//网络事件处理
    //============以下变量为处理图像
    VideoPlayer *mPlayer; //播放线程
    QImage mImage; //记录当前的图像
    MVWidget *m_MVWidget;
};
#endif // WIDGET_H
