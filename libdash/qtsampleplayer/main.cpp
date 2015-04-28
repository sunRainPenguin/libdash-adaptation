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
#include "UI/DASHPlayer.h"
#include "ondemandgui.h"
using namespace sampleplayer;

#include <QStringList>  
#include <QString>
#include <QDebug> 


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	QStringList drivers = QSqlDatabase::drivers();    
	foreach(QString driver, drivers) 
		qDebug() << "\t" << driver;    
	QSqlDatabase data_base = QSqlDatabase::addDatabase("QMYSQL");
	data_base.setHostName("localhost");  //����������ַ192.168.23.3
	data_base.setPort(3306);  //���ö˿�
	data_base.setDatabaseName("videomanagement");  //�������ݿ�����
	data_base.setUserName("root");  //�����û���
	data_base.setPassword("qwe123");  //��������
	if(!data_base.open())
		qDebug()<<"failed to connect to mysql";
	else
		qDebug()<<"success";
	

	OnDemandGui onDemandWindow;
	onDemandWindow.show();	

	/*QtSamplePlayerGui player;

	DASHPlayer p(player);

	player.show();*/

    return a.exec();
}
