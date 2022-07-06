#Makefile 三要素：目标、依赖、命令

#定义变量
TARGET=threadpool
CC=g++
INCLUDE_PATH=../include
INSTALL_PATH=/usr/bin/
LIBRARY_PATH=../lib
CFLAGS=-I$(INCLUDE_PATH) -c -g -Wall
CLIBS=-lssl -lcrypto -lpthread -lmysqlclient
CPPFLAGS=-D -L -l -01 -02 -03


RM=sudo rm -rf
COPY=sudo cp

SFILE=$(wildcard *.cpp)
DFILE=$(patsubst %.cpp,%.o,$(SFILE))


$(TARGET):$(DFILE)
	$(CC) $^ $(CLIBS) -o $@

%.o:%.cpp
	$(CC) $(CFLAGS) $<

#功能性目标
#清除.o文件
clean:
	$(RM) $(TARGET) $(DFILE)
#显示
output:
	echo "var_name"
#安装
install:
	$(COPY) $(TARGET) $(INSTALL_PATH)
#卸载
disclean:
	$(RM) $(INSTALL_PATH)$(TARGET) 
