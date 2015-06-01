#pragma once
#include <QString>
#include <QDebug>
#include <QList>
#include <QUrl>
#include "sqlconfig.h"

class Global
{
public:
	static QString subjectType;
	static QString gradeType;
	static QString allType;
	static QString myFavorite;
	static QString recentVideos;
	static QString avatarIconTemp;
	static QUrl uploadPicServlet;
	static QString avatarPath;
	static QString currUserTempPath;
	static QString emotionPathServer;
	static QString emotionPathTemp;
	static void InitEmotionNameList(QList<QString>& emotionNameList);
	static QString QueryUserFromDb(QString username, QString password);
	static bool QueryUserFromDb(QString username);
	static bool AddUserToDb(QString username, QString password);
};

