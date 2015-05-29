#pragma once
#include <QString>
#include <QDebug>
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
	static QString QueryUserFromDb(QString username, QString password);
	static bool QueryUserFromDb(QString username);
	static bool AddUserToDb(QString username, QString password);
};

