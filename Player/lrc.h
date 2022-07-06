#ifndef LRC_H
#define LRC_H

#include <QDialog>

namespace Ui {
class lrc;
}

class Widget;

class lrc: public QDialog
{
    Q_OBJECT

public:
    explicit lrc(QWidget *parent = nullptr);
    lrc(Widget *parent = nullptr);
    ~lrc();

    friend class Widget;
public:
    void showMusicName(QString music);


    void setMusicBarValue(int value);
    void setMusicBarTime(const QString &);
    void setMusicRow(int row);
    void setPlayerIcon(const QString &path);
    void setVolumeIcon(const QString &path);
    void setPlayerStytleIcon(const QString &path);
    void setVolumeValue(int value);
    void showLrc(QString musicName);
private slots:
    void on_pb_lrcNextMusic_clicked();

    void on_pushButton_5_clicked();

    void on_pb_lrcLastMusic_clicked();

    void on_pb_lrcPlayerAndPause_clicked();

    void on_lw_lrcMusicList_doubleClicked(const QModelIndex &index);

    void on_pb_lrcVolume_clicked();

    void on_pb_lrcPlayerStytle_clicked();

    void on_slider_lrcVolume_valueChanged(int value);

    void on_slider_lrcMusic_valueChanged(int value);

    void on_pb_hideLrc_clicked();

private:
    Ui::lrc *ui;

    Widget *m_Ui;
    bool m_bIsDragBar;
};

#endif // LRC_H
