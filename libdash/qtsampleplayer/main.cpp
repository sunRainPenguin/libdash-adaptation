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
#include "ondemandgui.h"
using namespace sampleplayer;

#include <QStringList>  
#include <QString>
#include "DebugLog.h"

/*#define  serverAddress   "http://192.168.23.2:8080/"*/
/*#define  serverAddress   "http://192.168.253.5:8080/"*/
#define serverAddress "http://localhost:8081/"
#define dbHostName "localhost"
#define  dbName "videomanagement"
#define dbUserName "root"
#define dbPassword "qwe123"

void ClearTemp()
{
	QDir tempDir("./Temp/");
	tempDir.setFilter(QDir::Files);
	int i = 0, FileCount=tempDir.count();
	if (FileCount>0)
	{
		for (i=0; i<=FileCount-1; i++)
			tempDir.remove(tempDir[i]);
	}
	
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	if (!ClearDebugLog())
	{
		qDebug() << "\t" <<"Clear debug log failed!";
	}
	ClearTemp();
	qInstallMessageHandler(CustomMessageHandler);

	QUrl url(serverAddress);
	QNetworkAccessManager manager;
	QEventLoop loop;
	QNetworkReply *reply = manager.get(QNetworkRequest(url));
	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();
	QString replyUrl = reply->request().url().toString();
	if (reply->error() != QNetworkReply::NoError)
	{
		qCritical() << "\t" <<"Failed to connect to server! exit(-2).";
		QMessageBox::warning(NULL, QString("Warning"), QString("Can't connect the server!"), QMessageBox::Yes);
		exit(-2);
	}

	QStringList drivers = QSqlDatabase::drivers();    
	foreach(QString driver, drivers) 
		qDebug() << "\t" << driver;    
	QSqlDatabase data_base = QSqlDatabase::addDatabase("QMYSQL");
	data_base.setHostName(dbHostName);  //设置主机地址192.168.23.3    localhost
	data_base.setPort(3306);  //设置端口
	data_base.setDatabaseName(dbName);  //设置数据库名称
	data_base.setUserName(dbUserName);  //设置用户名
	data_base.setPassword(dbPassword);  //设置密码qwe123
	if(!data_base.open())
	{
		qCritical() << "\t" <<"Failed to connect to database! exit(1).";
		exit(1);
	}
	else
		qDebug() << "\t" <<"Success";
	

	OnDemandGui onDemandWindow;
	onDemandWindow.show();	


	//QtSamplePlayerGui player;

	//DASHPlayer p(player);

	//player.show();

    return a.exec();
}
