#include "ondemandgui.h"
#include <QAccessibleWidget>

#define COUNT_ROWCONTAIN 4
#define COUNT_COLUMNCONTAIN 3

OnDemandGui::OnDemandGui(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	loginDialog = NULL;
	playerGui    = NULL;
	player = NULL;

	if (!this->ShowAvailableMediaFromDb())
	{
		qDebug() << "\t" << "Failed to get media from Db ! exit(-1).";
		exit(-1);
	}

}

OnDemandGui::~OnDemandGui()
{
	if (loginDialog)
	{
		delete(loginDialog);
		loginDialog = NULL;
	}
	if (player)
	{
		delete(player);
		player = NULL;
	}
	if (playerGui)
	{
		delete(playerGui);
		playerGui =NULL;
	}
	if (verticalLayout)
	{
		delete(verticalLayout);
		verticalLayout = NULL;
	}
}
void OnDemandGui::on_button_login_clicked()
{
	loginDialog = new LoginDialog(this);
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
	this->ui.gridLayout->setRowStretch(2, 1);
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
			if (!this->SetMediaLayout(valMI_MPDUrl, valMI_ShowPicUrl, valMI_Name, valMI_UploadAuthor, valMI_InsertTime, valMI_ClickThroughRate, recCount/COUNT_ROWCONTAIN, recCount%COUNT_ROWCONTAIN))
			{
				return false;
			}
			recCount++;
		}
		for (int i = recCount; i<=11; i++)
		{
			QPushButton* imgButton = this->FindButtonByNameIndex(i);
			imgButton->setEnabled(false);
			imgButton->setVisible(false);
		}
	}

	return true;
}

bool OnDemandGui::SetMediaLayout(QString valMI_MPDUrl, QString valMI_ShowPicUrl, QString valMI_Name, QString valMI_UploadAuthor, QString valMI_InsertTime, QString valMI_ClickThroughRate, int row, int column)
{
	QImage* img=new QImage;
	if(! ( img->load(valMI_ShowPicUrl) ) ) //加载图像
	{
		qDebug() << "\t" <<"Load image failed ! ";
		delete(img);
		img = NULL;
		return false;
	}
	QPainter painter(this);
	QPixmap pixmapToShow = QPixmap::fromImage( img->scaled(size(), Qt::KeepAspectRatio) );
	painter.drawPixmap(0,0, pixmapToShow);

	QIcon *icon = new QIcon(pixmapToShow);
	QPushButton* imgButton = this->FindButtonByNameIndex(COUNT_ROWCONTAIN*row+column);  //为了connect连接一直保持，这里的pushbutton在界面上添加

	imgButton->setIcon(*icon);
	imgButton->setText("");
	imgButton->setParent(this);
	imgButton->setIconSize(QSize(190, 150));   
	imgButton->setFixedSize(200, 150); 
	imgButton->setAccessibleDescription(valMI_MPDUrl);

	QLabel*  labelMediaName = new QLabel(valMI_Name, this);
	QLabel*  labelAuthor = new QLabel(QString("Author: ")+valMI_UploadAuthor, this);
	QLabel*  labelUploadTime = new QLabel(valMI_InsertTime, this);
	QLabel*  labelClickThroughRate = new QLabel(valMI_ClickThroughRate, this);

	verticalLayout = new QVBoxLayout();
	verticalLayout->addWidget(imgButton);
	verticalLayout->addWidget(labelMediaName);
	verticalLayout->addWidget(labelAuthor);
	verticalLayout->addWidget(labelUploadTime);
	verticalLayout->addWidget(labelClickThroughRate);

	this->ui.gridLayout->addLayout(verticalLayout, row, column);
	connect(imgButton, SIGNAL(ButtonClicked(QString)), this, SLOT(StartPlayer(QString)));

	delete(img);
	img = NULL;
	delete(icon);
	icon = NULL;
	return true;
}

void OnDemandGui::StartPlayer(QString currMpdUrl)
{

	if (!playerGui)
	{
		playerGui = new QtSamplePlayerGui(currMpdUrl, this);
		player =new DASHPlayer(*playerGui);
		playerGui->show();
		QObject::connect(playerGui, SIGNAL(ClosePlayerGui()), this, SLOT(on_playgui_closed()));

		playerGui->ClearMpd();
		player->OnDownloadMPDPressed(currMpdUrl.toStdString());
		playerGui->ClickButtonStart();
	}
	else
	{		
		if (playerGui->IsStarted())
		{
			playerGui->ClickButtonStop();
		}

		playerGui->SetMpdUrl(currMpdUrl);
		playerGui->ClearMpd();

		player->OnDownloadMPDPressed(currMpdUrl.toStdString());
		playerGui->ClickButtonStart();
	}
}
QPushButton*  OnDemandGui::FindButtonByNameIndex(int number)
{
	QString buttonName = QString("imageButton_") + QString::number(number, 10);
	QPushButton* button = this->findChild<QPushButton*>(buttonName) ;
	if (button)
	{
		return button;
	} 
	else
	{
		return NULL;
	}

}
void OnDemandGui::on_playgui_closed()
{
	delete(player);
	player = NULL;
	delete(playerGui);
	this->playerGui = NULL;

}