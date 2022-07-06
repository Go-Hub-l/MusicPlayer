#include "lrc.h"
#include "ui_lrc.h"
#include "widget.h"

lrc::lrc(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::lrc)
{
    ui->setupUi(this);
    //m_Ui = parent;
}
lrc::lrc(Widget *parent) :
    m_Ui(parent),
    ui(new Ui::lrc),m_bIsDragBar(false)
{
    ui->setupUi(this);

    //m_Ui = parent;
}

lrc::~lrc()
{
    delete ui;
}


void lrc::showMusicName(QString music)
{
    ui->lw_lrcMusicList->addItem(music);
}
//下一首
void lrc::on_pb_lrcNextMusic_clicked()
{
    m_Ui->on_pb_nextMusic_clicked();
}
//播放风格
void lrc::on_pushButton_5_clicked()
{
    m_Ui->on_pb_circulationStyle_clicked();
}
//上一首
void lrc::on_pb_lrcLastMusic_clicked()
{
    m_Ui->on_pb_lastMusic_clicked();
}
//开始暂停
void lrc::on_pb_lrcPlayerAndPause_clicked()
{
    m_Ui->on_pb_pauseAndPlayer_clicked();
}

//双击播放
void lrc::on_lw_lrcMusicList_doubleClicked(const QModelIndex &index)
{
    //获取当前行
    //m_Ui->m_row = ui->lw_lrcMusicList->currentRow();
    //m_Ui->ui->lw_musicList->setCurrentRow(ui->lw_lrcMusicList->currentRow());
    m_Ui->on_lw_musicList_doubleClicked(index);

}
//设置音乐进度条值
void lrc::setMusicBarValue(int value)
{
    ui->slider_lrcMusic->setValue(value);
}
//设置音乐进度条时间
void lrc::setMusicBarTime(const QString &time)
{
    ui->lb_lrcMusicTime->setText(time);
}
//设置歌单行
void lrc::setMusicRow(int row)
{
    ui->lw_lrcMusicList->setCurrentRow(row);
}
//设置播放图标
void lrc::setPlayerIcon(const QString &path)
{
    ui->pb_lrcPlayerAndPause->setIcon(QIcon(path));
}
//设置音量图标
void lrc::setVolumeIcon(const QString &path)
{
    ui->pb_lrcVolume->setIcon(QIcon(path));
}
//设置播放方式图标
void lrc::setPlayerStytleIcon(const QString &path)
{
    ui->pb_lrcPlayerStytle->setIcon(QIcon(path));
}
//设置声音进度条的值
void lrc::setVolumeValue(int value)
{
    ui->slider_lrcVolume->setValue(value);
}

//点击音量键时切换音量状态
void lrc::on_pb_lrcVolume_clicked()
{
    m_Ui->on_pb_volume_clicked();
}
//切换循环方式
void lrc::on_pb_lrcPlayerStytle_clicked()
{
    m_Ui->on_pb_circulationStyle_clicked();
}
//音量值改变
void lrc::on_slider_lrcVolume_valueChanged(int value)
{
    m_Ui->on_slider_volume_valueChanged(value);
}
//音乐值改变
void lrc::on_slider_lrcMusic_valueChanged(int value)
{
    if(!m_bIsDragBar){
        m_Ui->on_slider_music_valueChanged(value);
        m_bIsDragBar = true;
    }
    else
        m_bIsDragBar = false;
}
//隐藏歌词，显示主对话框
void lrc::on_pb_hideLrc_clicked()
{
    this->hide();
    m_Ui->show();
}
//显示歌词
void lrc::showLrc(QString musicName)
{
    //打开歌词文件：成功？  失败？

    //将歌词文件读入内存


}
