#include "ondemandgui.h"

OnDemandGui::OnDemandGui(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	if (!this->ShowAvailableMediaFromDb())
	{
		qDebug() << "\t" << "Failed to get media from Db ! exit(-1).";
		exit(-1);
	}
	
}

OnDemandGui::~OnDemandGui()
{

}
void OnDemandGui::on_button_login_clicked()
{
	loginDialog = new LoginDialog();
	QObject::connect(loginDialog, SIGNAL(enterSuccessfully(QString)),this, SLOT(SetLoginState(QString)));
	loginDialog->exec();
}
void OnDemandGui::SetLoginState(QString username)
{
	this->ui.button_login->setText(username);
}

bool OnDemandGui::ShowAvailableMediaFromDb()
{
	 int indexMI_MPDUrl, indexMI_ShowPicUrl, indexMI_Name, indexMI_UploadAuthor, indexMI_InsertTime, indexMI_ClickThroughRate;
	QString valMI_MPDUrl, valMI_ShowPicUrl, valMI_Name, valMI_UploadAuthor, valMI_InsertTime, valMI_ClickThroughRate;
	QMap<QString, QString> mpdUrlMap;

	QSqlQuery sql_query; 
	QString select_sql;
	QSqlRecord rec;

	this->ui.gridLayout->setRowStretch(0, 1);
	this->ui.gridLayout->setRowStretch(1, 1);
	this->ui.gridLayout->setRowStretch(3, 1);
	this->ui.gridLayout->setColumnStretch(0, 1);
	this->ui.gridLayout->setColumnStretch(1, 1);
	this->ui.gridLayout->setColumnStretch(2, 1);
	this->ui.gridLayout->setColumnStretch(3, 1);

	select_sql = "SELECT * FROM mediainfo";
	sql_query.prepare(select_sql);
	if (!sql_query.exec())
	{
		qDebug() << "\t" <<"Select sql failed! ";
		return false;
	} 
	else
	{
		rec = sql_query.record();
		int recCount = 0;
		while (sql_query.next())
		{
			indexMI_MPDUrl = rec.indexOf(QString("MI_MPDUrl"));
			valMI_MPDUrl = sql_query.value(indexMI_MPDUrl).toString();

			indexMI_ShowPicUrl = rec.indexOf(QString("MI_ShowPicUrl"));
			valMI_ShowPicUrl = sql_query.value(indexMI_ShowPicUrl).toString();

			indexMI_Name = rec.indexOf(QString("MI_Name"));
			valMI_Name = sql_query.value(indexMI_Name).toString();

			indexMI_UploadAuthor = rec.indexOf(QString("MI_UploadAuthor"));
			valMI_UploadAuthor = sql_query.value(indexMI_UploadAuthor).toString();

			indexMI_InsertTime = rec.indexOf(QString("MI_InsertTime"));
			QDateTime temp = sql_query.value(indexMI_InsertTime).toDateTime();
			valMI_InsertTime = temp.toString("yyyy-mm-dd hh:mm:ss");

			indexMI_ClickThroughRate = rec.indexOf(QString("MI_ClickThroughRate"));
			valMI_ClickThroughRate = sql_query.value(indexMI_ClickThroughRate).toString();

			mpdUrlMap.insert(valMI_Name, valMI_MPDUrl);
			qDebug() << "\t" <<QString("Succeed Query! MI_MPDUrl:  %1   MI_ShowPicUrl:   %2  MI_Name:  %3   MI_UploadAuthor:  %4  MI_InsertTime:  %5 MI_ClickThroughRate:  %6").arg(valMI_MPDUrl).arg(valMI_ShowPicUrl).arg(valMI_Name).arg(valMI_UploadAuthor).arg(valMI_InsertTime).arg(valMI_ClickThroughRate);
			if (!this->SetMediaLayout(valMI_ShowPicUrl, valMI_Name, valMI_UploadAuthor, valMI_InsertTime, valMI_ClickThroughRate, recCount/4, recCount%4))
			{
				return false;
			}
			recCount++;
		}
	}

	return true;
}

bool OnDemandGui::SetMediaLayout(QString valMI_ShowPicUrl, QString valMI_Name, QString valMI_UploadAuthor, QString valMI_InsertTime, QString valMI_ClickThroughRate, int row, int column)
{
	QImage* img=new QImage;
	if(! ( img->load(valMI_ShowPicUrl) ) ) //º”‘ÿÕºœÒ
	{
		qDebug() << "\t" <<"Load image failed ! ";
		delete img;
		return false;
	}
	QPainter painter(this);
	QPixmap pixmapToShow = QPixmap::fromImage( img->scaled(size(), Qt::KeepAspectRatio) );
	painter.drawPixmap(0,0, pixmapToShow);
	QIcon *icon = new QIcon(pixmapToShow);
	QPushButton *imgButton = new QPushButton(*icon, "", this);
	imgButton->setIconSize(QSize(190, 150));   
	imgButton->setFixedSize(200, 150); 

	QLabel*  labelMediaName = new QLabel(valMI_Name, this);
	QLabel*  labelAuthor = new QLabel(QString("Author: ")+valMI_UploadAuthor, this);
	QLabel*  labelUploadTime = new QLabel(valMI_InsertTime, this);
	QLabel*  labelClickThroughRate = new QLabel(valMI_ClickThroughRate, this);

	QVBoxLayout* verticalLayout = new QVBoxLayout();
	verticalLayout->addWidget(imgButton);
	verticalLayout->addWidget(labelMediaName);
	verticalLayout->addWidget(labelAuthor);
	verticalLayout->addWidget(labelUploadTime);
	verticalLayout->addWidget(labelClickThroughRate);


	this->ui.gridLayout->addLayout(verticalLayout, row, column);
	return true;
}