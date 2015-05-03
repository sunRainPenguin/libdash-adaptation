#include "ondemandgui.h"
#include <QAccessibleWidget>

#define COUNT_ROWCONTAIN 4
#define COUNT_COLUMNCONTAIN 3

OnDemandGui::OnDemandGui(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	hasLogedIn = false;
	loginDialog = NULL;
	playerGui    = NULL;
	player = NULL;
	verticalLayout = NULL;

	if (!this->ShowAvailableMediaFromDb())
	{
		qCritical() << "\t" << "Failed to get media from Db ! exit(-1).";
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
	mediaInfo.clear();

}
void OnDemandGui::on_button_login_clicked()
{
	loginDialog = new LoginDialog(this);
	QObject::connect(loginDialog, SIGNAL(enterSuccessfully(QString, QString)), this, SLOT(SetLoginState(QString, QString)));
	loginDialog->exec();
}

void OnDemandGui::SetLoginState(QString userID, QString username)
{
	this->hasLogedIn = true;
	this->userID = userID;
	this->userName = username;
	this->ui.button_login->setText(username);
	if (playerGui)
	{
		emit enterSuccessfully(userID, username);
	}
}

bool OnDemandGui::ShowAvailableMediaFromDb()
{
	 int indexMI_ID, indexMI_MPDUrl, indexMI_ShowPicUrl, indexMI_Name, indexMI_UploadAuthor, indexMI_InsertTime, indexMI_ClickThroughRate;
	QString MI_ID, MI_MPDUrl, MI_ShowPicUrl, MI_Name, MI_UploadAuthor, MI_InsertTime, MI_ClickThroughRate;

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
		qDebug() << "\t" <<"Select mediainfo failed! ";
		return false;
	} 
	else
	{
		rec = sql_query.record();
		int recCount = 0;

		while (sql_query.next())
		{
			indexMI_ID = rec.indexOf(QString("MI_ID"));
			MI_ID = sql_query.value(indexMI_ID).toString();

			indexMI_MPDUrl = rec.indexOf(QString("MI_MPDUrl"));
			MI_MPDUrl = sql_query.value(indexMI_MPDUrl).toString();

			indexMI_ShowPicUrl = rec.indexOf(QString("MI_ShowPicUrl"));
			MI_ShowPicUrl = sql_query.value(indexMI_ShowPicUrl).toString();

			indexMI_Name = rec.indexOf(QString("MI_Name"));
			MI_Name = sql_query.value(indexMI_Name).toString();

			indexMI_UploadAuthor = rec.indexOf(QString("MI_UploadAuthor"));
			MI_UploadAuthor = sql_query.value(indexMI_UploadAuthor).toString();

			indexMI_InsertTime = rec.indexOf(QString("MI_InsertTime"));
			QDateTime temp = sql_query.value(indexMI_InsertTime).toDateTime();
			MI_InsertTime = temp.toString("yyyy-mm-dd hh:mm:ss");

			indexMI_ClickThroughRate = rec.indexOf(QString("MI_ClickThroughRate"));
			MI_ClickThroughRate = sql_query.value(indexMI_ClickThroughRate).toString();

			//插入时，默认第一个为Name，第二个为MPDUrl
			mediaInfo.insert(MI_ID, MI_Name);
			mediaInfo.insert(MI_ID, MI_MPDUrl);
			qDebug() << "\t" <<QString("Succeed Query! MI_ID:  %1  MI_MPDUrl:  %2   MI_ShowPicUrl:   %3  MI_Name:  %4   MI_UploadAuthor:  %5  MI_InsertTime:  %6 MI_ClickThroughRate:  %7").arg(MI_ID).arg(MI_MPDUrl).arg(MI_ShowPicUrl).arg(MI_Name).arg(MI_UploadAuthor).arg(MI_InsertTime).arg(MI_ClickThroughRate);

			if (!this->SetMediaLayout(MI_ID, MI_MPDUrl, MI_ShowPicUrl, MI_Name, MI_UploadAuthor, MI_InsertTime, MI_ClickThroughRate, recCount/COUNT_ROWCONTAIN, recCount%COUNT_ROWCONTAIN))
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

bool OnDemandGui::SetMediaLayout(QString MI_ID, QString MI_MPDUrl, QString MI_ShowPicUrl, QString MI_Name, QString MI_UploadAuthor, QString MI_InsertTime, QString MI_ClickThroughRate, int row, int column)
{
	QImage* img=new QImage;
	if(! ( img->load(MI_ShowPicUrl) ) ) //加载图像
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
	imgButton->setAccessibleDescription(MI_ID);

	QLabel*  labelMediaName = new QLabel(MI_Name, this);
	QLabel*  labelAuthor = new QLabel(QString("Author: ")+MI_UploadAuthor, this);
	QLabel*  labelUploadTime = new QLabel(MI_InsertTime, this);
	QLabel*  labelClickThroughRate = new QLabel(MI_ClickThroughRate, this);

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

void OnDemandGui::StartPlayer(QString currMediaID)
{
	QString currMpdUrl;
	QString currMediaName;

	QList<QString> valMediaInfo = mediaInfo.values(currMediaID);
	if (valMediaInfo.size()<2)
	{
		qWarning() << "\t" << "Retrieve mediainfo failed!";
		return;
	} 
	else
	{
		//插入时间长短排序
		currMpdUrl = valMediaInfo.at(0);
		currMediaName = valMediaInfo.at(1);
	}

	if (!playerGui)
	{
		playerGui = new QtSamplePlayerGui(hasLogedIn, this->userID, this->userName, currMediaID, currMediaName, currMpdUrl, this);
		player =new DASHPlayer(*playerGui);
		playerGui->show();
		QObject::connect(this, SIGNAL(enterSuccessfully(QString, QString)), playerGui, SLOT(SetLoginState(QString, QString)));
		QObject::connect(playerGui, SIGNAL(ClosePlayerGui()), this, SLOT(on_playgui_closed()));
		QObject::connect(playerGui, SIGNAL(LoginBeforeComment()), this, SLOT(LoginBeforeComment()));
		playerGui->ClearMpd();
		player->OnDownloadMPDPressed(currMpdUrl.toStdString());
		playerGui->ClickButtonStart();
	}
	else
	{		
		if (playerGui->IsStarted())
			playerGui->ClickButtonStop();

		playerGui->SetMpdUrl(currMpdUrl);
		playerGui->SetCurrMediaInfo(currMediaID,  currMediaName);

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
		return button;
	else
		return NULL;

}
void OnDemandGui::on_playgui_closed()
{
	if (playerGui->IsStarted())
		playerGui->ClickButtonStop();
	delete(player);
	player = NULL;
	delete(playerGui);
	this->playerGui = NULL;

}
void OnDemandGui::LoginBeforeComment()
{
	on_button_login_clicked();
}
