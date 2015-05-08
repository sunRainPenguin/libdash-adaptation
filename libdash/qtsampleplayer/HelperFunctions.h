#pragma once
#include <QString>
#include "sqlconfig.h"

inline QString GetEmotionPath()
{
	QString emotionIconPath;
	QSqlQuery sql_query; 
	QSqlRecord rec;

	QString select_sql = "SELECT * FROM smilelyfaces LIMIT 1";
	sql_query.prepare(select_sql);
	if(!sql_query.exec())
	{
		qWarning() << "\t" <<"Select user failed! ";
		return "";
	}
	else
	{
		rec = sql_query.record();
		if (sql_query.next())
		{
			int index = rec.indexOf(QString("iconDir"));
			emotionIconPath = sql_query.value(index).toString();
			return emotionIconPath;
		}
		return "";
	}
}
