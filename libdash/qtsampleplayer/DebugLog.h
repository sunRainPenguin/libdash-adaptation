#pragma once
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

void CustomMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	static QMutex mutex;
	mutex.lock();

	QString txt;

	switch (type) {
	case QtDebugMsg:
		txt = QString("Debug: ");
		break;

	case QtWarningMsg:
		txt = QString("Warning: ");
		break;
	case QtCriticalMsg:
		txt = QString("Critical: ");
		break;
	case QtFatalMsg:
		txt = QString("Fatal: ");
		abort();
	}

	QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
	QString currentDateTime = QDateTime::currentDateTime().toString();
	QString message = QString("%1 %2 %3 %4").arg(currentDateTime).arg(txt).arg(context_info).arg(msg);

	QFile outFile("./debuglog/debuglog.txt");
	outFile.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream ts(&outFile);
	ts << message << "\r\n";
	outFile.flush();
	outFile.close();

	mutex.unlock();
}

bool ClearDebugLog ()
{
	static QMutex mutex;
	mutex.lock();
	QFile outFile("./debuglog/debuglog.txt");
	if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;
	
	outFile.flush();
	outFile.close();
	mutex.unlock();
	return true;
}