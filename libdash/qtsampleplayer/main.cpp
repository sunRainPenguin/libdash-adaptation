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


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	if (!ClearDebugLog())
	{
		qDebug() << "\t" <<"Clear debug log failed!";
	}

	qInstallMessageHandler(CustomMessageHandler);

	QStringList drivers = QSqlDatabase::drivers();    
	foreach(QString driver, drivers) 
		qDebug() << "\t" << driver;    
	QSqlDatabase data_base = QSqlDatabase::addDatabase("QMYSQL");
	data_base.setHostName("localhost");  //设置主机地址192.168.23.3
	data_base.setPort(3306);  //设置端口
	data_base.setDatabaseName("videomanagement");  //设置数据库名称
	data_base.setUserName("root");  //设置用户名
	data_base.setPassword("qwe123");  //设置密码
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
