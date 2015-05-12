#include "ondemandgui.h"
#include <QAccessibleWidget>

#define COUNT_ROWCONTAIN 4
#define COUNT_COLUMNCONTAIN 3

OnDemandGui::OnDemandGui(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->ui.button_logout->setEnabled(false);
	hasLogedIn = false;
	loginDialog = NULL;
	playerGui    = NULL;
	playergu = NULL;

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
	if (playergu)
	{
		delete(playergu);
		playergu = NULL;
	}
	if (playerGui)
	{
		delete(playerGui);
		playerGui =NULL;
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
	this->ui.button_logout->setEnabled(true);
	if (playerGui)
	{
		emit enterSuccessfully(userID, username);
	}
}

bool OnDemandGui::ShowAvailableMediaFromDb(bool updating)
{
	 int indexMI_ID, indexMI_MPDUrl, indexMI_ShowPicUrl, indexMI_Name, indexMI_UploadAuthor, indexMI_InsertTime, indexMI_ClickThroughRate;
	QString MI_ID, MI_MPDUrl, MI_ShowPicUrl, MI_Name, MI_UploadAuthor, MI_InsertTime, MI_ClickThroughRate;

	QSqlQuery sql_query; 
	QString select_sql;
	QSqlRecord rec;

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
			MI_InsertTime = temp.toString("yyyy-MM-dd hh:mm:ss");

			indexMI_ClickThroughRate = rec.indexOf(QString("MI_ClickThroughRate"));
			MI_ClickThroughRate = sql_query.value(indexMI_ClickThroughRate).toString();

			//插入时，默认第一个为Name，第二个为MPDUrl
			mediaInfo.insert(MI_ID, MI_Name);
			mediaInfo.insert(MI_ID, MI_MPDUrl);
			qDebug() << "\t" <<QString("Succeed Query! MI_ID:  %1  MI_MPDUrl:  %2   MI_ShowPicUrl:   %3  MI_Name:  %4   MI_UploadAuthor:  %5  MI_InsertTime:  %6 MI_ClickThroughRate:  %7").arg(MI_ID).arg(MI_MPDUrl).arg(MI_ShowPicUrl).arg(MI_Name).arg(MI_UploadAuthor).arg(MI_InsertTime).arg(MI_ClickThroughRate);

			if (!this->SetMediaLayout(MI_ID, MI_MPDUrl, MI_ShowPicUrl, MI_Name, MI_UploadAuthor, MI_InsertTime, MI_ClickThroughRate, recCount/COUNT_ROWCONTAIN, recCount%COUNT_ROWCONTAIN, updating))
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

bool OnDemandGui::SetMediaLayout(QString MI_ID, QString MI_MPDUrl, QString MI_ShowPicUrl, QString MI_Name, QString MI_UploadAuthor, QString MI_InsertTime, QString MI_ClickThroughRate, int row, int column, bool updating)
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
	//imgButton->setParent(this);
	imgButton->setIconSize(QSize(190, 150));   
	imgButton->setFixedSize(200, 150); 
	imgButton->setAccessibleDescription(MI_ID);

	QLabel*  labelMediaName =FindLabelByNameIndex(LabelType::mediaName, COUNT_ROWCONTAIN*row+column);
	labelMediaName->setText(MI_Name);
	QLabel*  labelAuthor = FindLabelByNameIndex(LabelType::mediaAuthor, COUNT_ROWCONTAIN*row+column);
	labelAuthor->setText(QString("Author: ")+MI_UploadAuthor);
	QLabel*  labelUploadTime = FindLabelByNameIndex(LabelType::mediaTime, COUNT_ROWCONTAIN*row+column);
	labelUploadTime->setText(MI_InsertTime);
	QLabel*  labelClickThroughRate = FindLabelByNameIndex(LabelType::clickThroughRate, COUNT_ROWCONTAIN*row+column);
	labelClickThroughRate->setText(MI_ClickThroughRate);

	if (!updating)
	{
		connect(imgButton, SIGNAL(ButtonClicked(QString)), this, SLOT(StartPlayer(QString)));
	}
		
	
	delete(img);
	img = NULL;
	delete(icon);
	icon = NULL;
	return true;
}

void OnDemandGui::StartPlayer(QString currMediaID)
{
	
	if (this->playerGui  &&   this->playerGui->IsStarted() && this->playerGui->GetMediaID()==currMediaID )
	{
		QString lastMediaID = this->playerGui->GetMediaID();
		return;
	}
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
	UpdateClickThroughRateToDb(currMediaID);
	ShowAvailableMediaFromDb();

	if (!playerGui)
	{
		playerGui = new QtSamplePlayerGui(hasLogedIn, this->userID, this->userName, currMediaID, currMediaName, currMpdUrl, this);
		playergu =new DASHPlayer(*playerGui);
		playerGui->show();
		QObject::connect(this, SIGNAL(enterSuccessfully(QString, QString)), playerGui, SLOT(SetLoginState(QString, QString)));
		QObject::connect(this, SIGNAL(LogOut()), playerGui, SLOT(SetLogoutState()));
		QObject::connect(playerGui, SIGNAL(ClosePlayerGui()), this, SLOT(on_playgui_closed()));
		QObject::connect(playerGui, SIGNAL(LoginBeforeComment()), this, SLOT(LoginBeforeComment()));

		playerGui->ClearMpd();
		playergu->OnDownloadMPDPressed(currMpdUrl.toStdString());
		playerGui->ClickButtonStart();
	}
	else
	{		
		if (playerGui->IsStarted())
			playerGui->ClickButtonStop();

		playerGui->SetMpdUrl(currMpdUrl);
		playerGui->SetCurrMediaInfo(currMediaID,  currMediaName);

		playerGui->ClearMpd();
		playergu->OnDownloadMPDPressed(currMpdUrl.toStdString());
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
QLabel* OnDemandGui::FindLabelByNameIndex(int type,  int number)
{
	QString labelName;
	switch(type)
	{
	case 0:
		labelName = QString("ll_Name_") + QString::number(number, 10);
		break;
	case 1:
		labelName = QString("ll_Author_") + QString::number(number, 10);
		break;
	case 2:
		labelName = QString("ll_Time_") + QString::number(number, 10);
		break;
	case 3:
		labelName = QString("ll_Rate_") + QString::number(number, 10);
		break;
	default:
		break;
	}
	QLabel* label = this->findChild<QLabel*>(labelName) ;
	if (label)
		return label;
	else
		return NULL;
}

void OnDemandGui::on_playgui_closed()
{
	if (playerGui->IsStarted())
		playerGui->ClickButtonStop();
	delete(playergu);
	playergu = NULL;
	delete(playerGui);
	this->playerGui = NULL;

}
void OnDemandGui::LoginBeforeComment()
{
	on_button_login_clicked();
}
void OnDemandGui::on_button_logout_clicked()
{
	this->hasLogedIn = false;
	this->userID = "";
	this->userName = "";
	this->ui.button_login->setText("Log in");
	this->ui.button_logout->setEnabled(false);
	if (playerGui)
	{
		emit LogOut();
	}
}
void OnDemandGui::UpdateClickThroughRateToDb(QString mediaID)
{
	QSqlQuery sql_query; 
	QString select_sql;

	select_sql = "UPDATE mediainfo SET MI_ClickThroughRate=MI_ClickThroughRate+1 WHERE MI_ID =  " + mediaID;
	sql_query.prepare(select_sql);
	if (!sql_query.exec())
	{
		qCritical() << "\t" <<"Update click through rate failed! ";
		return;
	} 
}

bool    OnDemandGui::StartUpdateOndemandGui        ()
{

	this->updateOnDemandGuiHandle = CreateThreadPortable (UpdateGui, this);

	if(this->updateOnDemandGuiHandle == NULL)
		return false;

	return true;
}

void*   OnDemandGui::UpdateGui        (void *data)
{
	OnDemandGui *gui = (OnDemandGui*) data;

	while(gui)
	{
		gui->ShowAvailableMediaFromDb(true);
		PortableSleep(5);
	}


	return NULL;
}