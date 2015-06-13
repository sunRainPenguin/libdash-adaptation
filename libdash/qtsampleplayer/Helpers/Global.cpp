#include "Global.h"
QString Global::subjectType = QString::fromLocal8Bit("科目");
QString Global::gradeType = QString::fromLocal8Bit("年级");
QString Global::allType = QString::fromLocal8Bit("热门视频");
QString Global::myFavorite = QString::fromLocal8Bit("我喜爱的视频");
QString Global::recentVideos = QString::fromLocal8Bit("我最近观看的视频");
QString Global::registerAvatarTemp = QString("./Temp/HeadPicture/HeadPicture.jpg");
QString Global::registerAvatarTempPath = QString("./Temp/HeadPicture/");
QUrl Global::uploadPicServlet = QUrl("http://localhost:8081/UploadPic/servlet/UploadPicServlet");
QString Global::avatarPath =QString("http://localhost:8081/DDUVideoUsers/");
QString Global::UsersTempPath = QString("./Temp/Users/");
QString Global::emotionPathServer = QString("http://localhost:8081/emotion/");
QString Global::emotionPathTemp = QString("./Temp/emotion/");

void Global::InitEmotionNameList(QList<QString>& emotionNameList)
{
	emotionNameList<<"dm.png"<<"eh.png"<<"kl.png"<<"mm.png"<<"oh.png"<<"qd.png"<<"qzy.png"<<"sj.png"<<"sk.png"<<"tx.png"<<"wi.png"<<"yi.png";
}
QString Global::QueryUserFromDb(QString username, QString password)
{
	int indexUserID;
	QString userID;

	QSqlQuery sql_query; 
	QSqlRecord rec;

	QString select_sql = "SELECT * FROM user WHERE BINARY username = '" + username + "' AND BINARY password = '" + password + "'";
	sql_query.prepare(select_sql);
	if(!sql_query.exec())
	{
		qWarning() << "\t" <<"Select user failed! ";
		return QString("");
	}
	else
	{
		rec = sql_query.record();
		if (sql_query.next())
		{
			indexUserID = rec.indexOf(QString("ID"));
			userID = sql_query.value(indexUserID).toString();
			return userID;
		}
		return QString("");
	}
}

bool Global::QueryUserFromDb(QString username)
{
	QString select_sql = "SELECT * FROM user WHERE username = '" + username + "'";
	QSqlQuery sql_query; 
	sql_query.prepare(select_sql);
	if(!sql_query.exec() || sql_query.size()<1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Global::AddUserToDb(QString username, QString password)
{
	QSqlQuery sql_query;

	sql_query.prepare("INSERT INTO user(username, password, flag) VALUES(:username,:password,:flag)");

	sql_query.bindValue(":username", username);
	sql_query.bindValue(":password", password);
	sql_query.bindValue(":flag", 1);

	if (!sql_query.exec())
		return false;
	else
		return true;
}