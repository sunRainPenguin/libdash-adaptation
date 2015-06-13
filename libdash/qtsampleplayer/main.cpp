/*
 * main.cpp
 *****************************************************************************
 * Copyright (C) 2012, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#include <QApplication>
#include <qapplication.h>
#include "UI/DASHPlayer.h"
#include "UI/ondemandgui.h"
using namespace sampleplayer;

#include <QStringList>  
#include <QString>
#include "./debuglog/DebugLog.h"

/*#define  serverAddress   "http://192.168.23.2:8080/"*/
/*#define  serverAddress   "http://192.168.253.5:8080/"*/
#define serverAddress "http://localhost:8081/"
#define dbHostName "localhost"
#define  dbName "videomanagement"
#define dbUserName "root"
#define dbPassword "qwe123"

void ClearTemp()
{
	QDir tempDir;
	if (!tempDir.exists("./Temp/"))
		tempDir.mkpath("./Temp/");
	tempDir.setPath("./Temp/");

	int i = 0, FileCount=tempDir.count();
	if (FileCount>0)
	{
		for (i=0; i<=FileCount-1; i++)
		{
			QString temp = tempDir[i];
			tempDir.remove(tempDir[i]);			//移除所有在Temp目录下的视频图片
			if (tempDir[i] == "Users" || tempDir[i] == "emotion" ||  tempDir[i] == "HeadPicture")
			{
				QDir childDir;
				if (tempDir[i] == "Users")
					childDir.setPath("./Temp/Users/");
				else if (tempDir[i] == "emotion")
					childDir.setPath("./Temp/emotion/");
				else if (tempDir[i] == "HeadPicture")
					childDir.setPath("./Temp/HeadPicture/");

				childDir.setFilter(QDir::Files);
				int childFileCnt = childDir.count();
				for (int i=0; i<=childFileCnt-1; i++)
					childDir.remove(childDir[i]);			//移除所有Temp目录下的子目录中的文件
			}
		}
	}
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	a.setWindowIcon(QIcon("project.ico"));
	if (!ClearDebugLog())
		qDebug() << "\t" <<"Clear debug log failed!";
	else
		qDebug() << "\t" <<"Clear debug log successfully!";
	ClearTemp();				//清空上一次使用缓存
	qInstallMessageHandler(CustomMessageHandler);

	//检测是否能连接上服务器
	QUrl url(serverAddress);
	QNetworkAccessManager manager;
	QEventLoop loop;
	QNetworkReply *reply = manager.get(QNetworkRequest(url));
	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();
	QString replyUrl = reply->request().url().toString();
	if (reply->error() != QNetworkReply::NoError)
	{
		qCritical() << "\t" <<"Failed to connect server! exit(-2).";
		QMessageBox::warning(NULL, QString("Warning"), QString("Can't connect the server!"), QMessageBox::Yes);
		exit(-2);
	}
	else
		qDebug() << "\t" <<"Connect server successfully!";

	//检测是否能连接上数据库
	QSqlDatabase data_base = QSqlDatabase::addDatabase("QMYSQL");
	data_base.setHostName(dbHostName);  //设置主机地址
	data_base.setPort(3306);  //设置端口
	data_base.setDatabaseName(dbName);  //设置数据库名称
	data_base.setUserName(dbUserName);  //设置用户名
	data_base.setPassword(dbPassword);  //设置密码
	if(!data_base.open())
	{
		qCritical() << "\t" <<"Failed to connect database! exit(1).";
		exit(1);
	}
	else
		qDebug() << "\t" <<"Connect database successfully!";
	

	OnDemandGui onDemandWindow;
	onDemandWindow.show();	

    return a.exec();
}
