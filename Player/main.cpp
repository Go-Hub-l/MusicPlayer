#include "widget.h"

#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8"); //设置编码格式为UTF-8
    QTextCodec::setCodecForLocale(codec);
    Widget w;
    w.show();
    return a.exec();
}
