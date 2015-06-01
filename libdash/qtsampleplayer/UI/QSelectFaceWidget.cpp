#include "QSelectFaceWidget.h"

QSelectFaceWidget::QSelectFaceWidget(const QString &faceIconDir,  QWidget *parent)
	: QWidget(parent),
	faceIconDir(faceIconDir),
	iconNumber(0)
{
	this->setWindowFlags(Qt::Popup);
	QFileInfo iconDir(faceIconDir);
	if (!iconDir.isDir())
		return;
	QFileInfoList InfoList = QDir(faceIconDir).entryInfoList();

	if (InfoList.size()<12)
	{
		QList<QString> emotionNameList;
		Global::InitEmotionNameList(emotionNameList);
		QString emotionName;
		foreach(emotionName, emotionNameList)
			this->DownloadEmotionIcons(emotionName);
		return ;
	}

	this->SetEmotionGridLayout();
	qDebug() << "\t" << "Get faces done!";

}

QSelectFaceWidget::~QSelectFaceWidget()
{

}
void QSelectFaceWidget::popUp(QPoint &pos, int offsetY)
{
	pos.setY(pos.y() + offsetY);
	this->move(pos);
	this->show();
}
void QSelectFaceWidget::on_face_selected(QString IconDir)
{
	emit FaceSelected(IconDir);
	this->close();
}
void QSelectFaceWidget::DownloadEmotionIcons(QString iconName)
{
	//save the select result to the temp dir
	QString urlStr =  Global::emotionPathServer + iconName;

	//show picture on the server
	QNetworkAccessManager *manager;
	manager = new QNetworkAccessManager(this);
	QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(urlStr)));
	reply->setProperty("iconName", iconName);
	connect(manager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(OnEmotionDownloaded(QNetworkReply*)));

}
void QSelectFaceWidget::OnEmotionDownloaded(QNetworkReply* reply)
{
	int error = reply->error();
	if(reply->error() == QNetworkReply::NoError)
	{
		QPixmap* pixIcon= new QPixmap;
		QByteArray picData = reply->readAll(); 
		pixIcon->loadFromData(picData); 

		QString iconName = reply->property("iconName").toString();
		if (iconName!="")
		{
			QString fileName = faceIconDir + iconName;
			QFileInfo file(faceIconDir);
			if (file.isDir())
			{
				pixIcon->save(fileName);
				this->iconNumber++;
			}
			if(this->iconNumber==12)
				this->SetEmotionGridLayout();
		}

		delete(pixIcon);
		pixIcon=NULL;
	}
}
void QSelectFaceWidget::SetEmotionGridLayout()
{
	QFileInfoList InfoList = QDir(this->faceIconDir).entryInfoList();
	QFileInfo fileInfo;
	QImage* img=new QImage;

	//gridLayout
	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setMargin(2);
	gridLayout->setRowStretch(0, 1);
	gridLayout->setRowStretch(1, 1);
	gridLayout->setRowStretch(2, 1);
	gridLayout->setColumnStretch(0, 1);
	gridLayout->setColumnStretch(1, 1);
	gridLayout->setColumnStretch(2, 1);
	gridLayout->setColumnStretch(3, 1);

	//get picures
	int picCount = 0;
	foreach(fileInfo, InfoList)
	{
		if(!fileInfo.isFile()) continue;

		if(0==fileInfo.suffix().compare("jpg") || 0==fileInfo.suffix().compare("png"))
		{
			qDebug() << fileInfo.filePath();
			if(! ( img->load(fileInfo.filePath()) ) ) 
			{
				qCritical() << "\t" <<"Load image failed ! ";
				delete(img);
				img = NULL;
				return ;
			}
			QPainter painter(this);
			QPixmap pixmapToShow = QPixmap::fromImage( img->scaled(size(), Qt::KeepAspectRatio) );
			painter.drawPixmap(0,0, pixmapToShow);
			QIcon *icon = new QIcon(pixmapToShow);

			MyPushButton* imgButton = new MyPushButton(this);
			imgButton->setIcon(*icon);
			imgButton->setText("");
			imgButton->setIconSize(QSize(60, 60));   
			imgButton->setFixedSize(60, 60); 
			imgButton->setAccessibleDescription(fileInfo.filePath());

			connect(imgButton, SIGNAL(ButtonClicked(QString)), this, SLOT(on_face_selected(QString)));
			gridLayout->addWidget(imgButton, picCount/4, picCount%4);

			picCount++;
		}
	}
}