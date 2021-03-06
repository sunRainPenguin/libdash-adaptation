#ifndef QSELECTFACEWIDGET_H
#define QSELECTFACEWIDGET_H

#include <QWidget>
#include "ui_QSelectFaceWidget.h"
#include <QDir>
#include <QDebug>
#include <QGridLayout>
#include <QPainter>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include "./Helpers/Global.h"
#include "MyPushButton.h"

class QSelectFaceWidget : public QWidget
{
	Q_OBJECT

public:
	explicit QSelectFaceWidget(const QString &faceIconDir, QWidget *parent = 0);
	~QSelectFaceWidget();
	void popUp(QPoint &pos, int offsetY = 0);


public slots:
		void on_face_selected(QString IconDir);

private:
	Ui::QSelectFaceWidget ui;
	QString faceIconDir;
	int iconNumber;
	void DownloadEmotionIcons(QString iconName);
	void SetEmotionGridLayout();

private slots:
	void OnEmotionDownloaded(QNetworkReply* reply);

signals:
	void FaceSelected(QString IconDir);

};

#endif // QSELECTFACEWIDGET_H
