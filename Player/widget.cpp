#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QTime>
#include <QTimer>
#include <string>
#include <iostream>
#include <QFloat16>
#include <QPainter>
//支持 .mp4  .mkv  .ts  .flv 封装格式播放


using namespace std;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget),m_currentMusicName(""),m_MusicNum(0)
    ,m_bIsPlayer(false),m_nowVolume(30),m_lastVolume(30),m_bMusicValueChanged(false)
    ,m_playStyle(orderPlayer),lrcUi(new lrc(this)),m_pos(0)
{ 
    //new 一个数据库对象
    m_sql = new CSqlite;
    //new 一个QMediaPlayer对象
    m_player = new QMediaPlayer;
    //网络
    m_tcpKernel = new TCPKernel;
    //播放视频
    mPlayer = new VideoPlayer;
    //播放MV对话框
    m_MVWidget = new MVWidget;



    initClient();
}

bool Widget::initClient()
{
    ui->setupUi(this);
    lrcUi->hide();
    //设置随机数种子
    srand(time(0));
    //加载歌曲列表
    loadMusicList();
    m_player->setVolume(20);
    ui->slider_volume->setValue(20);
    lrcUi->setVolumeValue(20);
    //设置定时器  定时器到了执行函数
    QTimer *timer = new QTimer(this);//this表示由基类回收资源
    //连接槽函数
    connect(timer,&QTimer::timeout,this,&Widget::slot_moveString);
    timer->start(1000);//设置定时器时间为1秒
    connect(m_player,&QMediaPlayer::positionChanged,this,&Widget::slot_musicPosChanged);
    //连接服务器
    if(!m_tcpKernel->connectToServer(_DEF_SERVERIP,_DEF_PORT))
        return false;
    //与解码线程绑定连接槽函数
    //connect(mPlayer,SIGNAL(sig_GetOneFrame(QImage)),this,SLOT(slotGetOneFrame(QImage)));
    connect(mPlayer,&VideoPlayer::sig_GetOneFrame,this,&Widget::slotGetOneFrame);

    //添加一张背景图片
    mImage.load(":/images/background.jpg");


    return true;
}

Widget::~Widget()
{
    delete ui;
    //释放m_sql
    if(m_sql){
        delete m_sql;
        m_sql = nullptr;
    }
    //释放m_player
    if(m_player){
        delete m_player;
        m_player = nullptr;
    }
    //释放m_tcpKernel
    if(m_tcpKernel){
        delete m_tcpKernel;
        m_tcpKernel = nullptr;
    }
    //释放mPlayer
    if(mPlayer){
        delete mPlayer;
        mPlayer = nullptr;
    }
    if(m_MVWidget){
        delete m_MVWidget;
        m_MVWidget = nullptr;
    }
}
//加载歌单
void Widget::loadMusicList()
{
    //获得当前路径
    QString DBpath = QString(QDir::currentPath());
    //数据库名
    QString fileName("music.db");
    // 查看该路径下是否存在 sql,lrc,music,images 文件夹
    QDir tempPath;
    //如果sql 路径不存在
    if(!tempPath.exists(DBpath+"/sql/")){
        //不存在就创建
        qDebug()<<"创建/sql/文件夹";
        tempPath.mkdir(DBpath+"/sql/");
    }
    //如果 lrc 路径不存在
    if(!tempPath.exists(DBpath+"/lrc/")){
        //不存在就创建
        qDebug()<<"创建/lrc/文件夹";
        tempPath.mkdir(DBpath+"/lrc/");
    }
    //如果 music 路径不存在
    if(!tempPath.exists(DBpath+"/music/")){
        //不存在就创建
        qDebug()<<"创建/music/文件夹";
        tempPath.mkdir(DBpath+"/music/");
    }
    //如果 images 路径不存在
    if(!tempPath.exists(DBpath+"/images/")){
        //不存在就创建
        qDebug()<<"创建/images/文件夹";
        tempPath.mkdir(DBpath+"/images/");
    }

    QFile file(DBpath+"/sql/" + fileName);
    if(file.exists()){//如果数据库存在
        //将数据库里的数据添加到歌单 并设置音量为退出前音量
        QStringList strlst;
        //连接数据库
        m_sql->ConnectSql(DBpath+"/sql/" + fileName);
        //数据库里的歌添加到歌单
        QString sqlstr("SELECT musicName,musicPath FROM t_musicList");
        if(m_sql->SelectSql(sqlstr,2,strlst)){
            while(!strlst.empty()){
                ui->lw_musicList->addItem(strlst[0]);
                //将歌曲添加到歌单列表
                lrcUi->showMusicName(strlst[0]);
                strlst.pop_front();
                if(m_MusicNum < MAXMUSICSIZE){
                    m_musicName[m_MusicNum++] = strlst[0];
                    strlst.pop_front();
                }else{
                    QMessageBox::about(this,"提示","以达到添加的最大歌曲数量");
                    break;
                }
            }

        }else{
            QMessageBox::about(this,"提示","搜索数据库音乐列表获取歌单失败");
        }
        //音量
        sqlstr = "SELECT volume FROM t_volume";
        if(m_sql->SelectSql(sqlstr,1,strlst) && strlst.size() > 0){
            m_player->setVolume(strlst[0].toInt());
            //设置音量滚动条
            ui->slider_volume->setValue(strlst[0].toInt());
            lrcUi->setVolumeValue(strlst[0].toInt());
        }

    }else{//数据库不存在
        if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){
            QMessageBox::about(this,"提示","数据库创建失败!");
        }else{//连接数据库：创建 歌曲表，音量表
            file.close();
            m_sql->ConnectSql(DBpath+"/sql/" + fileName);
            QString sqlstr("CREATE TABLE t_musicList(musicName VARCHAR(260),musicPath VARCHAR(260))");//创建歌曲表
            m_sql->UpdateSql(sqlstr);
            //创建音量表
            sqlstr = "CREATE TABLE t_volume(volume int)";
            m_sql->UpdateSql(sqlstr);
            sqlstr = "UPDATE t_volume SET volume = 30";
            m_sql->UpdateSql(sqlstr);
            m_player->setVolume(30);
        }
    }
}

//添加歌曲
void Widget::on_pb_addMusic_clicked()
{
    //注：完整的音乐名是包含路径＋歌曲名
    //打开文件框A
    QStringList files = QFileDialog::getOpenFileNames(this,"打开歌曲","./music");
    //将音乐文件添加到音乐名数组
    bool hasFlag = false;//去重标志
    for(int i = 0;i < files.size();i++){
        //去重
        for(int j = 0;j < m_MusicNum;j++){
            if(m_musicName[j] == files[i]){
                hasFlag = true;
                break;
            }
        }
        if(!hasFlag){
            m_musicName[m_MusicNum++] = files[i];
            //显示在UI歌单列表上
            QStringList list = files[i].split("/");
            ui->lw_musicList->addItem(list[list.size()-1]);
            lrcUi->showMusicName(list[list.size()-1]);
            //lrcUi->ui->lw_lrcMusicList->addItem(list[list.size()-1]);
            //写入到数据库
            QString sqlstr = QString("insert into t_musicList(musicName , musicPath) values ('%1','%2')")
                    .arg(list[list.size()-1]).arg( files[i]);
            m_sql->UpdateSql(sqlstr);
        }else{
            hasFlag = false;
        }
    }
}
//选中歌曲播放：播放/暂停交替切换
void Widget::on_pb_pauseAndPlayer_clicked()
{
    //获取选中的位置
    ui->lb_musicName->setText(m_currentMusicName);
    int row = ui->lw_musicList->currentRow();
    //异常分析：如果选中的位置不在范围内
    if(row < 0)
        return ;
    //播放位置置零
    m_pos = 0;
    //判断是切换歌曲还是播放/暂停
    if(m_currentMusicName != ui->lw_musicList->currentItem()->text()){//如果是切换歌曲
        //设置播放
        m_player->setMedia(QUrl::fromLocalFile(m_musicName[row]));
        m_player->play();
        m_currentMusicName = ui->lw_musicList->currentItem()->text();
        //获取作者名
        QStringList strlst = m_currentMusicName.split('-');
        //如果未匹配成功
        if(strlst.size() == 1 && strlst[0] == "-")
            m_currentSinger = "未知";
        else{
            m_currentSinger = strlst[0];
        }
        ui->lb_singer->setText(m_currentSinger);
        //设置播放图标
        ui->pb_pauseAndPlayer->setIcon(QIcon(":/images/bfzn_pause.png"));
        //设置歌词播放图标
        lrcUi->setPlayerIcon(":/images/bfzn_pause.png");
        //播放标志设置为播放
        m_bIsPlayer = true;
    }else{//如果是播放/暂停
        if(m_bIsPlayer){//暂停歌曲
            m_player->pause();
            //切换图标
             ui->pb_pauseAndPlayer->setIcon(QIcon(":/images/bfzn_play.png"));
             //设置歌词播放图标
             lrcUi->setPlayerIcon(":/images/bfzn_play.png");
             //切换播放状态
             m_bIsPlayer = false;
        }else{//继续播放
            m_player->play();
            //切换UI按钮图标
             ui->pb_pauseAndPlayer->setIcon(QIcon(":/images/bfzn_pause.png"));
             //设置歌词播放图标
             lrcUi->setPlayerIcon(":/images/bfzn_pause.png");
             //切换播放状态
             m_bIsPlayer = true;
        }

    }

}
//双击播放
void Widget::on_lw_musicList_doubleClicked(const QModelIndex &index)
{
    //获取点击位置
    int row = index.row();
    ui->lw_musicList->setCurrentRow(row);
    //设置歌词歌单行
    lrcUi->setMusicRow(row);
    //异常处理
    if(row < 0)
        return;
    ui->lb_musicName->setText(m_currentMusicName);
    //播放位置置零
    m_pos = 0;
    //判断当前歌曲状态：播放--》双击播放  暂停--》双击播放
    if(m_bIsPlayer){//播放状态
        m_player->setMedia(QUrl::fromLocalFile(m_musicName[row]));
        qDebug()<<"play  row:"<<row;
        m_player->play();
        //设置当前播放歌曲名
        m_currentMusicName = ui->lw_musicList->currentItem()->text();
    }else{//暂停状态
        m_player->setMedia(QUrl::fromLocalFile(m_musicName[row]));
        m_player->play();
        //设置当前播放歌曲名
        m_currentMusicName = ui->lw_musicList->currentItem()->text();
        //设置播放图标
        ui->pb_pauseAndPlayer->setIcon(QIcon(":/images/bfzn_pause.png"));
        //设置歌词播放图标
        lrcUi->setPlayerIcon(":/images/bfzn_pause.png");
        //切换播放状态
        m_bIsPlayer = true;
    }
    //获取作者名
    QStringList strlst = m_currentMusicName.split(" - ");
    //如果未匹配成功
    if(strlst[0] == m_currentMusicName)
        m_currentSinger = "未知";
    else{
        m_currentSinger = strlst[0];
    }
    ui->lb_singer->setText(m_currentSinger);
}

//删除歌曲
void Widget::on_pb_delMusic_clicked()
{
    //获取选中的歌曲
    int row = ui->lw_musicList->currentRow();
    //异常检测
    if(row < 0)
        return ;
    //如果歌曲正在播放
    if(m_bIsPlayer && !m_currentMusicName.compare(ui->lw_musicList->currentItem()->text())){
        ui->lw_musicList->setCurrentRow(-1);
        //设置歌词歌单行
        lrcUi->setMusicRow(row);
        QMessageBox::about(this,"提示","不能删除正在播放的歌曲");
        return ;
    }
    //将歌曲从UI上删除
    ui->lw_musicList->takeItem(row);
    //将歌曲从数据库中删除
    QString sqlstr(QString("DELETE FROM t_musicList WHERE musicPath = '%1';").arg(m_musicName[row]));
    m_sql->UpdateSql(sqlstr);
    //删除歌曲后面的歌曲前移
    for(int i = row + 1;i < m_MusicNum;i++){
        m_musicName[i-1] = m_musicName[i];
    }
    --m_MusicNum;//计数器--

    ui->lw_musicList->setCurrentRow(-1);
    //设置歌词歌单行
    lrcUi->setMusicRow(row);
}
//上一曲
void Widget::on_pb_lastMusic_clicked()
{
    //如果当前没有播放歌曲
    if(!m_bIsPlayer)
        return;
    //获取当前歌曲行
    int row = ui->lw_musicList->currentRow();
    //当前正在播放歌曲:且是第一首
    if(row == 0)
        row = m_MusicNum-1;
    else
        --row;
    //当前歌曲不是第一首
    //播放位置置零
    m_pos = 0;
    //设置播放歌曲
    m_player->setMedia(QUrl::fromLocalFile(m_musicName[row]));
    //播放
    m_player->play();
    ui->lw_musicList->setCurrentRow(row);
    //设置歌词歌单行
    lrcUi->setMusicRow(row);
    m_currentMusicName = ui->lw_musicList->currentItem()->text();
    //获取作者名
    QStringList strlst = m_currentMusicName.split('-');
    //如果未匹配成功
    if(strlst.size() == 1 && strlst[0] == "-")
        m_currentSinger = "未知";
    else{
        m_currentSinger = strlst[0];
    }
    ui->lb_singer->setText(m_currentSinger);
    ui->lb_musicName->setText(m_currentMusicName);
}
//下一曲
void Widget::on_pb_nextMusic_clicked() //播放完成后，直接调用下一曲会失败
{

    //如果当前没有播放歌曲
    if(!m_bIsPlayer)
        return;
    //获取当前歌曲行
    int row = ui->lw_musicList->currentRow();
    //如果是顺序播放
    if(m_playStyle == orderPlayer){
        //循环选中
        row = (row + 1)%(m_MusicNum);
    }else if(m_playStyle == randomPlayer){//如果是随机播放
        row = rand()%m_MusicNum;
    }//如果是当前歌曲则不发生变化
    //当前歌曲不是第一首
    //播放位置置零
    m_pos = 0;
    //设置播放歌曲
    m_player->setMedia(QUrl::fromLocalFile(m_musicName[row]));
    //播放
    m_player->play();
    ui->lw_musicList->setCurrentRow(row);
    //设置歌词歌单行
    lrcUi->setMusicRow(row);
    m_currentMusicName = ui->lw_musicList->currentItem()->text();
    //获取作者名
    QStringList strlst = m_currentMusicName.split('-');
    //如果未匹配成功
    if(strlst.size() == 1 && strlst[0] == "-")
        m_currentSinger = "未知";
    else{
        m_currentSinger = strlst[0];
    }
    ui->lb_singer->setText(m_currentSinger);
    ui->lb_musicName->setText(m_currentMusicName);
}
//歌曲音乐位置改变
void Widget::slot_musicPosChanged(qint64 val)
{
    //判断是否要下一首的时候val不能等于0，val等于0时说明刚开始
    if(m_bIsPlayer && val){
        //判断传过来的值是否大于歌曲的持续时间来决定是否下一首：注意 判断下一首需要在歌曲播放的时候判断
        if(val >= m_player->duration()){
            qDebug()<<"val ::"<<val;
            on_pb_nextMusic_clicked();
        }

    }
    //判断当前歌曲是否正在播放
    if(m_player->state() == QMediaPlayer::PlayingState){
        //歌曲持续
        if(m_player->duration()){


            m_bMusicValueChanged = true;
            lrcUi->m_bIsDragBar = true;
            //设置进度条的值
            //更新进度条
            ui->slider_music->setValue(m_player->position()*100/m_player->duration());
            //设置歌词音乐进度条
            lrcUi->setMusicBarValue(m_player->position()*100/m_player->duration());
                    //slider_music->setValue(m_player->position()*100/m_player->duration());
            //显示歌曲播放时间：position() 歌曲当前播放的时间  duration() 歌曲总时长
           // qRound()
            //有点问题
            QTime duration1(0,m_player->position()/1000/60,
                            m_player->position()/1000%60,0);
            QTime duration2(0,m_player->duration()/1000/60,
                            m_player->duration()/1000%60,0);
            //更新时间
            ui->lb_musicTime->setText(QString("%1/%2").arg(duration1.toString("mm:ss")).arg(duration2.toString("mm:ss")));
            //设置歌词音乐进度条时间
            lrcUi->setMusicBarTime(QString("%1/%2").arg(duration1.toString("mm:ss")).arg(duration2.toString("mm:ss")));
            //播放结束  切换下一首  已在这个槽函数里做：slot_musicPosChanged
//            if(duration1 == duration2){
//                on_pb_nextMusic_clicked();
//            }


        }
    }
}
//拖动进度条调整音量
void Widget::on_slider_volume_valueChanged(int value)
{
    lrcUi->setVolumeValue(value);
    ui->slider_volume->setValue(value);
    //获取进度条音量
    m_player->setVolume(value);
    //如果当前音量有变化
    if(value != m_nowVolume){
        m_lastVolume = m_nowVolume;
        m_nowVolume = value;
    }
    //如果当前音量是0
    if(m_nowVolume == 0){
        //更换图标
        ui->pb_volume->setIcon(QIcon(":/images/voice_close.png"));
        lrcUi->setVolumeIcon(":/images/voice_close.png");
    }else{
        ui->pb_volume->setIcon(QIcon(":/images/voice_open.png"));
        lrcUi->setVolumeIcon(":/images/voice_open.png");
    }


}
//按下音量按钮
void Widget::on_pb_volume_clicked()
{
   //如果当前有声音
    if(m_nowVolume){// 静音：更新图标
        m_lastVolume = m_nowVolume;
        m_nowVolume = 0;
        m_player->setVolume(m_nowVolume);
        ui->pb_volume->setIcon(QIcon(":/images/voice_close.png"));
        lrcUi->setVolumeIcon(":/images/voice_close.png");
    }else{// 无声音
        // 有声音：更新图标
        m_nowVolume = m_lastVolume;
        m_lastVolume = 0;
        m_player->setVolume(m_nowVolume);
        ui->pb_volume->setIcon(QIcon(":/images/voice_open.png"));
        lrcUi->setVolumeIcon(":/images/voice_open.png");
    }
}
//音乐进度条值改变槽函数
void Widget::on_slider_music_valueChanged(int value)
{
    //将播放的歌曲设置为当前进图条所对应的播放时间
    if(!m_bMusicValueChanged){
        //先将歌曲暂停
        m_player->pause();
        //设置歌曲位置
        m_player->setPosition(value*m_player->duration()/100);
        //设置时间
        QTime duration1(0,m_player->position()/1000/60%60,m_player->position()/1000%60);
        QTime duration2(0,m_player->duration()/1000/60%60,m_player->duration()/1000%60);
        //更新时间
        ui->lb_musicTime->setText(QString("%1/%2").arg(duration1.toString("mm:ss")).arg(duration2.toString("mm:ss")));

        //播放歌曲
        if(m_bIsPlayer)
        m_player->play();
        m_bMusicValueChanged = true;
    }else
        m_bMusicValueChanged = false;
}
//显示歌词
//切换歌曲播放风格
void Widget::on_pb_circulationStyle_clicked()
{
    m_playStyle = (m_playStyle+1)%3;
    //设置图标
    switch (m_playStyle){
    case 0:ui->pb_circulationStyle->setIcon(QIcon(":/images/orderPlayer.png"));
           lrcUi->setPlayerStytleIcon(":/images/orderPlayer.png");
        break;
    case 1:ui->pb_circulationStyle->setIcon(QIcon(":/images/randomPlayer.png"));
        lrcUi->setPlayerStytleIcon(":/images/randomPlayer.png");
        break;
    case 2:ui->pb_circulationStyle->setIcon(QIcon(":/images/singlePlayer.png"));
        lrcUi->setPlayerStytleIcon(":/images/singlePlayer.png");
        break;
    }
}
//移动字符串
void Widget::slot_moveString()
{
    //如果已经移动到末尾则从开始
    if(m_pos == m_currentMusicName.size() - 1)
        m_pos = 0;
    else
        ++m_pos;

    QString newStr = m_currentMusicName.mid(m_pos) + "       " + m_currentMusicName.mid(0,m_pos);
    ui->lb_musicName->setText(newStr);
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.drawRect(0, 0, this->width(), this->height()); //先画成黑色

    if (mImage.size().width() <= 0) return;

    //将图像按比例缩放成和窗口一样大小
    QImage img;

    img = mImage.scaled(this->size(),Qt::KeepAspectRatio);



    int x = this->width() - img.width();
    int y = this->height() - img.height();

    x /= 2;
    y /= 2;//保证图像居中显示

    painter.drawImage(QPoint(x,y),img); //画出图像
    m_MVWidget->slotGetOneFrame(mImage);
}

void Widget::slotGetOneFrame(QImage img)
{

    mImage = img;
    update(); //调用update将执行 paintEvent函数
}

//显示歌词
void Widget::on_pb_lrc_clicked()
{
    //显示歌词对话框
    this->hide();
    lrcUi->show();
}
//用户登录
//void Widget::on_pb_userLogin_clicked()
//{
//}

//播放当前选中的MV
void Widget::on_pb_playMV_clicked()
{
    m_MVWidget->show();

    //MV请求头
    STRU_DOWNLOAD_MV_RQ sdmr;
    //sdmr.m_nType = DEF_PACK_PLAY_MV_RQ;
    strcpy(sdmr.m_szMVName,"波妞.mp4");
    if(!m_tcpKernel->sendData((char*)&sdmr,sizeof(sdmr))){
        qDebug()<<"send message failed";
        return;
    }
    //接收数据
    m_tcpKernel->recvData();
    char file_path[MAX_PATH] = {0};
    sprintf(file_path,"./MV/%s",sdmr.m_szMVName);
    //设置名字并播放视频
    mPlayer->setFileName(file_path);
    mPlayer->startPlay();
    //m->hide();
}

//用户登录
void Widget::on_pb_userLogin_clicked()
{

}

