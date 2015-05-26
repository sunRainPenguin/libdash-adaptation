#include "ondemandgui.h"
#include <QAccessibleWidget>

#define COUNT_ROWCONTAIN 4
#define COUNT_COLUMNCONTAIN 3

OnDemandGui::OnDemandGui(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->ui.button_logout->setEnabled(false);
	hasLogedIn = false;
	this->selectItem = NULL;
	SetSearchInfo();
	loginDialog = NULL;
	playerGui    = NULL;
	player = NULL;

	this->ShowTreeView();
	if (this->ShowAvailableMediaFromDb()==-1)
	{
		qCritical() << "\t" << "Failed to get media from Db ! exit(-1).";
		exit(-1);
	}
	ShowSearchResult("Hot movies  ...  ");
	this->ui.lineEdit_search->setFocus();
	this->ui.lineEdit_search->installEventFilter(this);
	QString qss_OnDemandGui;
	QFile qssFile(":/qss/qss_OnDemandGui.qss");
	qssFile.open(QFile::ReadOnly);

	if(qssFile.isOpen())
	{
		qss_OnDemandGui = QLatin1String(qssFile.readAll());
		this->setStyleSheet(qss_OnDemandGui);
		qssFile.close();
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
	if (treeModel)
	{
		delete(treeModel);
		treeModel = NULL;
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
	this->refreshMyFavoriteUI();
	this->refreshRecentVideoUI();
	if (playerGui)
	{
		emit enterSuccessfully(userID, username);
	}
}
int OnDemandGui::ShowAvailableMediaFromDb(QString SearchKey, QString typeValue, QString searchType)
{
	 int indexMI_ID, indexMI_MPDUrl, indexMI_ShowPicUrl, indexMI_Name, indexMI_UploadAuthor, indexMI_InsertTime, indexMI_ClickThroughRate, indexSI_ID, indexGI_ID, indexProgress, indexProgressMax, progress, progressMax;
	QString MI_ID, MI_MPDUrl, MI_ShowPicUrl, MI_Name, MI_UploadAuthor, MI_InsertTime, MI_ClickThroughRate, SI_ID, GI_ID;
	QString whereSI_ID, whereGI_ID, whereAndSI_ID, whereAndGI_ID, whereMI_ID, whereAndMI_ID;

	QMultiHash<QString, int>	progressInfo;

	int recCount = 0;
	bool noVideoToShow=false;
	QSqlQuery sql_query; 
	QString select_sql;
	QSqlRecord rec;

	//Get the sql statement
	/**************************myFavorite/recentVideos**************************/
	if (searchType==myFavorite || searchType==recentVideos)
	{
		if (userID!="")
		{
			if (searchType==myFavorite)
				select_sql = QString("SELECT DISTINCT MI_ID FROM UserFavorite WHERE UF_UserID=") + userID + " ";
			if (searchType==recentVideos)
				select_sql = QString("SELECT DISTINCT MI_ID, UW_Progress, UW_ProgressMax FROM UserWatched WHERE UW_UserID=") + userID + " ";
			sql_query.prepare(select_sql);
			if (!sql_query.exec())
			{
				qWarning() << "\t" <<"Select UserFavorite failed! Return -3";
				return -1;
			} 
			if (sql_query.size()<1)
			{
				qDebug() << "\t" << "This user has no favorite video! ";
				noVideoToShow=true;
			}

			whereMI_ID = QString(" MI_ID IN (");
			rec = sql_query.record();
			while (sql_query.next())
			{
				indexMI_ID = rec.indexOf("MI_ID");
				MI_ID = sql_query.value(indexMI_ID).toString();
				indexProgress = rec.indexOf("UW_Progress");
				progress = sql_query.value(indexProgress).toInt();
				indexProgressMax = rec.indexOf("UW_ProgressMax");
				progressMax = sql_query.value(indexProgressMax).toInt();

				progressInfo.insert(MI_ID, progress);
				progressInfo.insert(MI_ID, progressMax);
				whereMI_ID = whereMI_ID + MI_ID +",";
			}
			whereMI_ID.remove(whereMI_ID.size()-1, 1);
			whereMI_ID.append(") ");
			whereAndMI_ID = whereMI_ID;
			whereAndMI_ID.insert(0, "AND ");
			whereMI_ID.insert(0, "WHERE ");
			
		}
		else
			noVideoToShow=true;
	}
	/**************************subjectType**************************/
	if (searchType==subjectType)
	{
		select_sql = "SELECT SI_ID FROM subjectinfo WHERE SI_Name = '" + typeValue + "'  ";
		sql_query.prepare(select_sql);
		if (sql_query.exec() && sql_query.next())
		{
			rec = sql_query.record();
			indexSI_ID = rec.indexOf("SI_ID");
			SI_ID = sql_query.value(indexSI_ID).toString();
			whereSI_ID = " WHERE SI_ID = " + SI_ID + " ";
			whereAndSI_ID = " AND SI_ID = " + SI_ID + " ";
		}
	}
	/**************************gradeType**************************/
	if (searchType==gradeType)
	{
		select_sql = "SELECT GI_ID FROM gradeinfo WHERE GI_Name = '" + typeValue + "'  ";
		sql_query.prepare(select_sql);
		if (sql_query.exec() && sql_query.next())
		{
			rec = sql_query.record();
			indexGI_ID = rec.indexOf("GI_ID");
			GI_ID = sql_query.value(indexGI_ID).toString();
			whereGI_ID = " WHERE GI_ID = " + GI_ID + " ";
			whereAndGI_ID = " AND GI_ID = " + GI_ID + " ";
		}
	}
	/************************** prepare the final sql state **************************/
	if (!noVideoToShow)
	{
		if (SearchKey=="")
		{
			if        (searchType==subjectType && SI_ID!="")
				select_sql = QString("SELECT * FROM mediainfo " + whereSI_ID + " ORDER BY MI_ClickThroughRate DESC LIMIT 12 ") ;
			else if (searchType==gradeType && GI_ID!="")
				select_sql = QString("SELECT * FROM mediainfo " + whereGI_ID + " ORDER BY MI_ClickThroughRate DESC LIMIT 12 ") ;
			else if (searchType==myFavorite || searchType==recentVideos)
				select_sql = QString("SELECT * FROM mediainfo " + whereMI_ID + " ORDER BY MI_ClickThroughRate DESC LIMIT 12 ");
			else
				select_sql = QString("SELECT * FROM mediainfo ORDER BY MI_ClickThroughRate DESC LIMIT 12 ") ;
		} 
		else
		{
			if        (searchType==subjectType && SI_ID!="")
				select_sql = QString("SELECT * FROM mediainfo WHERE MI_Name LIKE '%") + SearchKey + "%' " + whereAndSI_ID + " ORDER BY MI_ClickThroughRate DESC LIMIT 12  ";
			else if (searchType==gradeType && GI_ID!="")
				select_sql = QString("SELECT * FROM mediainfo WHERE MI_Name LIKE '%") + SearchKey + "%' " + whereAndGI_ID + " ORDER BY MI_ClickThroughRate DESC LIMIT 12  ";
			else if (searchType==myFavorite || searchType==recentVideos)
				select_sql = QString("SELECT * FROM mediainfo WHERE MI_Name LIKE '%") + SearchKey + "%' " + whereAndMI_ID + " ORDER BY MI_ClickThroughRate DESC LIMIT 12  ";
			else
				select_sql = QString("SELECT * FROM mediainfo WHERE MI_Name LIKE '%") + SearchKey + "%' ORDER BY MI_ClickThroughRate DESC LIMIT 12  ";	
		}

		/************************** show videos **************************/
		sql_query.prepare(select_sql);
		if (!sql_query.exec())
		{
			qDebug() << "\t" <<"Select mediainfo failed! ";
			return -1;
		} 
		else
		{
			rec = sql_query.record();
			if (sql_query.size()==0)
				qDebug() << "\t" <<"There's no mediainfo! ";
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
				int progressMax=-1, progress=-1;
				if (searchType== recentVideos && progressInfo.size()!=0)
				{
					QList<int> progressInfoList = progressInfo.values(MI_ID);
					progressMax = progressInfoList.at(0);
					progress = progressInfoList.at(1);
				}
				
				if (!this->SetMediaLayout(MI_ID, MI_MPDUrl, MI_ShowPicUrl, MI_Name, MI_UploadAuthor, MI_InsertTime, MI_ClickThroughRate, recCount/COUNT_ROWCONTAIN, recCount%COUNT_ROWCONTAIN, progress, progressMax))
					return -1;

				//save the select result to the temp dir
				QFileInfo file( "./Temp/" + MI_ID + ".png");
				if (!file.isFile())
				{
					//show picture on the server
					QNetworkAccessManager *manager;
					manager = new QNetworkAccessManager(this);
					QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(MI_ShowPicUrl)));
					reply->setProperty("buttonNumber", recCount);
					reply->setProperty("MI_ID", MI_ID);
					connect(manager, SIGNAL(finished(QNetworkReply*)),
						this, SLOT(replyFinished(QNetworkReply*)));
				}
				recCount++;
			}
		}
	}

	//Hide the widget which has no video to show
	for (int i = recCount; i<=11; i++)
	{
		//hide the invisible widgets
		QPushButton* imgButton = this->FindButtonByNameIndex(i);
		imgButton->setEnabled(false);
		imgButton->setVisible(false);
		QLabel* labelMediaName = this->FindLabelByNameIndex(label_mediaName, i);
		labelMediaName->setEnabled(false);
		labelMediaName->setVisible(false);
		QLabel* labelMediaAuthor = this->FindLabelByNameIndex(label_mediaAuthor, i);
		labelMediaAuthor->setEnabled(false);
		labelMediaAuthor->setVisible(false);
		QLabel* labelMediaTime = this->FindLabelByNameIndex(label_mediaTime, i);
		labelMediaTime->setEnabled(false);
		labelMediaTime->setVisible(false);
		QLabel* labelClickRate = this->FindLabelByNameIndex(label_clickThroughRate, i);
		labelClickRate->setEnabled(false);
		labelClickRate->setVisible(false);
		QLabel* labelProgress = this->FindLabelByNameIndex(label_progress, i);
		labelProgress->setEnabled(false);
		labelProgress->setVisible(false);

		QString verticalLayoutName = QString("vl_") + QString::number(i, 10);
		QVBoxLayout* verticalLayout = this->findChild<QVBoxLayout*>(verticalLayoutName) ;
		verticalLayout->addStretch();
	}
	
	return recCount;
}
bool OnDemandGui::SetMediaLayout(QString MI_ID, QString MI_MPDUrl, QString MI_ShowPicUrl, QString MI_Name, QString MI_UploadAuthor, QString MI_InsertTime, QString MI_ClickThroughRate, int row, int column, int progress, int progressMax)
{
	//load image from the temp
	bool loadImgfailed = false;
	QString imgDir;
	QFileInfo file( "./Temp/" + MI_ID + ".png");
	if (file.isFile())
	{
		imgDir = "./Temp/" + MI_ID + ".png";
	}
	QImage* img=new QImage;
	QPixmap pixmapToShow;
 	if( imgDir=="" || !( img->load(imgDir) ) ) 
 	{
 		qDebug() << "\t" <<"Load image failed ! ";
		loadImgfailed = true;
 	}else
	{
 		QPainter painter(this);
 		pixmapToShow = QPixmap::fromImage( img->scaled(size(), Qt::KeepAspectRatio) );
 		painter.drawPixmap(0,0, pixmapToShow);
	}

	QPushButton* imgButton = this->FindButtonByNameIndex(COUNT_ROWCONTAIN*row+column);  //为了connect连接一直保持，这里的pushbutton在界面上添加
	if (!loadImgfailed)
	{
		QIcon *icon = new QIcon(pixmapToShow);
		imgButton->setIcon(*icon);
		imgButton->setIconSize(QSize(190, 150)); 
		delete(icon);
		icon = NULL;
	}
	
	imgButton->setText("");
	imgButton->setFixedSize(200, 150); 
	imgButton->setAccessibleDescription(MI_ID);
	imgButton->setEnabled(true);
	imgButton->setVisible(true);

	QLabel*  labelMediaName =FindLabelByNameIndex(label_mediaName, COUNT_ROWCONTAIN*row+column);
	labelMediaName->setText(MI_Name);
	labelMediaName->setEnabled(true);
	labelMediaName->setVisible(true);
	QLabel*  labelAuthor = FindLabelByNameIndex(label_mediaAuthor, COUNT_ROWCONTAIN*row+column);
	labelAuthor->setText(QString("Author: ")+MI_UploadAuthor);
	labelAuthor->setEnabled(true);
	labelAuthor->setVisible(true);
	QLabel*  labelUploadTime = FindLabelByNameIndex(label_mediaTime, COUNT_ROWCONTAIN*row+column);
	labelUploadTime->setText(MI_InsertTime);
	labelUploadTime->setEnabled(true);
	labelUploadTime->setVisible(true);
	QLabel*  labelClickThroughRate = FindLabelByNameIndex(label_clickThroughRate, COUNT_ROWCONTAIN*row+column);
	labelClickThroughRate->setText(MI_ClickThroughRate);
	labelClickThroughRate->setEnabled(true);
	labelClickThroughRate->setVisible(true);

	connect(imgButton, SIGNAL(ButtonClicked(QString)), this, SLOT(StartPlayer(QString)));
	delete(img);
	img = NULL;

	QLabel*  labelProgress = FindLabelByNameIndex(label_progress, COUNT_ROWCONTAIN*row+column);
	labelProgress->setText("");
	labelProgress->setEnabled(true);
	labelProgress->setVisible(true);
	if (progress>=0 && progressMax >0)
	{
		double dpercent = (double)progress/(double)progressMax;
		int ipersent = dpercent*100;
		labelProgress->setText("Watched " + QString::number(ipersent,10) + "% ");
	}
	return true;
}
void OnDemandGui::StartPlayer(QString currMediaID)
{
	if (this->playerGui  &&   this->playerGui->IsStarted() && this->playerGui->GetMediaID()==currMediaID )
		return;

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
	ShowAvailableMediaFromDb(currentSearchKey, selectItemText, selectType);

	if (!playerGui)
	{
		playerGui = new QtSamplePlayerGui(hasLogedIn, this->userID, this->userName, currMediaID, currMediaName, currMpdUrl, this);
		player =new DASHPlayer(*playerGui);
		playerGui->show();
		QObject::connect(this, SIGNAL(enterSuccessfully(QString, QString)), playerGui, SLOT(SetLoginState(QString, QString)));
		QObject::connect(this, SIGNAL(LogOut()), playerGui, SLOT(SetLogoutState()));
		QObject::connect(playerGui, SIGNAL(ClosePlayerGui(int, int)), this, SLOT(on_playgui_closed(int, int)));
		QObject::connect(playerGui, SIGNAL(LoginBeforeComment()), this, SLOT(LoginBeforeComment()));
		QObject::connect(playerGui, SIGNAL(MyFavoriteChanged()), this, SLOT(refreshMyFavoriteUI()));

		playerGui->ClearMpd();
		player->OnDownloadMPDPressed(currMpdUrl.toStdString());
		playerGui->ClickButtonStart();
	}
	else
	{		
		if (userID!="" && this->playerGui->GetMediaID()!="")
		{
			int progress= this->playerGui->GetProgress();
			int progressMax = this->playerGui->GetProgressMax();
			this->AddUserLastAccessVideoToDb(userID, this->playerGui->GetMediaID(), progress, progressMax);		//Record the video information before ending the last video
			this->refreshRecentVideoUI();
		}
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
	case 4:
		labelName = QString("ll_progress_") + QString::number(number, 10);
	default:
		break;
	}
	QLabel* label = this->findChild<QLabel*>(labelName) ;
	if (label)
		return label;
	else
		return NULL;
}
void OnDemandGui::on_playgui_closed(int progress, int progressMax)
{
	if (userID!="" && this->playerGui->GetMediaID()!="")
	{
		this->AddUserLastAccessVideoToDb(userID, this->playerGui->GetMediaID(), progress, progressMax);
		this->refreshRecentVideoUI();
	}

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
void OnDemandGui::on_button_logout_clicked()
{
	this->hasLogedIn = false;
	this->userID = "";
	this->userName = "";
	this->ui.button_login->setText("Log in");
	this->ui.button_logout->setEnabled(false);
	this->refreshMyFavoriteUI();
	this->refreshRecentVideoUI();
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
void OnDemandGui::on_button_search_clicked()
{
	QString searchWord = this->ui.lineEdit_search->text().simplified();
 	searchWord = searchWord.remove(" ");

	if (searchWord=="")
	{
		ShowAvailableMediaFromDb(searchWord, selectItemText, selectType);
		ShowSearchResult("Hot movies  ...  ");
		this->repaint();
		return;
	}

	int searchKeySize = searchWord.size();
	QString searchKey;

	while (searchKeySize>0)
	{
		for (int i=0; i<=searchWord.size()-searchKeySize; i++)
		{
			searchKey=searchWord.mid(i, searchKeySize);
			int resultCount = ShowAvailableMediaFromDb(searchKey, selectItemText, selectType);
			if (resultCount>=0)
			{
				SetSearchInfo(searchKey);
				ShowSearchResult("", searchWord, selectItemText, QString::number(resultCount, 10));
				return;
			}
		}
		searchKeySize--;
	}
}
void OnDemandGui::ShowTreeView()
{
	treeModel = new QStandardItemModel();  
	this->ui.treeView->setModel(treeModel);
	LoadTreeViewData(allType, treeModel);
	LoadTreeViewData(recentVideos, treeModel);
	LoadTreeViewData(myFavorite, treeModel);
	LoadTreeViewData(subjectType, treeModel);
	LoadTreeViewData(gradeType, treeModel);
	this->ui.treeView->expandAll();
	this->ui.treeView->setHeaderHidden(TRUE);
	this->ui.treeView->setEditTriggers(0);
}
bool OnDemandGui::LoadTreeViewData(QString type, QStandardItemModel * treeModel)
{

	QList<QStandardItem *> items;
	QStandardItem *item = new QStandardItem(type);  
	items.push_back(item);
	treeModel->appendRow(items);
	if (type == allType || type == myFavorite || type == recentVideos)
		return true;

	QList<QStandardItem *> childItems;
	QSqlQuery sql_query; 
	QString select_sql;
	QSqlRecord rec;

	if (type==subjectType)
	{
		select_sql = "SELECT * FROM subjectinfo ";
	} 
	else if (type==gradeType)
	{
		select_sql = "SELECT * FROM gradeinfo ";
	}

	sql_query.prepare(select_sql);
	if (!sql_query.exec())
	{
		qDebug() << "\t" <<"Select treeView infomation failed! ";
		return false;
	} 
	else
	{
		rec = sql_query.record();
		int recCount = 0;
		if (sql_query.size()==0)
		{
			qDebug() << "\t" <<"Select treeView infomation failed! ";
			return false;
		}

		if (type==subjectType)
		{
			while(sql_query.next())
			{
				int indexSubject;
				QString subject;

				indexSubject = rec.indexOf("SI_Name");
				subject = sql_query.value(indexSubject).toString();

				QStandardItem *item = new QStandardItem(subject);  
				childItems.push_back(item);
			}
		}
		if (type==gradeType)
		{
			while(sql_query.next())
			{
				int indexGrade;
				QString grade;

				indexGrade = rec.indexOf("GI_Name");
				grade = sql_query.value(indexGrade).toString();

				QStandardItem *item = new QStandardItem(grade);  
				childItems.push_back(item);
			}
		}
	}

items.at(0)->appendRows(childItems);
return true;
}
void OnDemandGui::replyFinished(QNetworkReply *reply)
{
	if(reply->error() == QNetworkReply::NoError)
	{
		QPixmap* currentPicture= new QPixmap;
		int buttonNumber = reply->property("buttonNumber").toInt();
		QString MI_ID = reply->property("MI_ID").toString();
		
		QByteArray jpegData = reply->readAll(); 
		currentPicture->loadFromData(jpegData); 
		if (MI_ID!="")
		{
			QFileInfo file("./Temp/");
			if (file.isDir())
				currentPicture->save("./Temp/" + MI_ID + ".png");
		}
		
 		QIcon *icon = new QIcon(*currentPicture);
 		QPushButton* imgButton = this->FindButtonByNameIndex(buttonNumber);
 		imgButton->setIcon(*icon);
 		imgButton->setIconSize(QSize(190, 150));   
		delete(currentPicture);
		currentPicture=NULL;
	}
}
bool OnDemandGui::eventFilter(QObject * object, QEvent * event)
{
	if (event->type() == QEvent::KeyPress )
	{
		QKeyEvent* key_event = static_cast<QKeyEvent*>(event);
		bool lineEdit_search = this->ui.lineEdit_search->hasFocus();

		if(object == this->ui.lineEdit_search && lineEdit_search)
		{
			if(key_event->key() == Qt::Key_Enter  || key_event->key() == Qt::Key_Return){
				this->ui.button_search->setFocus();
				this->on_button_search_clicked();
				return true;
			}	
		}
	}
	return QMainWindow::eventFilter(object, event);
}
void OnDemandGui::on_treeView_clicked				(QModelIndex modelIndex)
{
	 selectItem = treeModel->itemFromIndex(modelIndex);
	selectItemText = selectItem->text();

	if (selectItemText == myFavorite || selectItemText == recentVideos || selectItemText == allType)
		selectType = selectItemText;			
	if (selectItem->parent())
		selectType = selectItem->parent()->text();

	//Select the parent node
	 if (!selectItem->parent())
	 {
		 SetSearchInfo();
		 if (selectItem->text()==myFavorite)
		 {
			 ShowAvailableMediaFromDb("", "", myFavorite);
			 ShowSearchResult("My Favorite ...  ");
			 if (this->userName=="")
				 QMessageBox::warning(this, QString("Warning"), QString("Please log in before watching your favorite videos!"), QMessageBox::Yes);
			 return;
		 }
		 else if (selectItem->text()==recentVideos)
		 {
			 ShowAvailableMediaFromDb("", "", recentVideos);
			 ShowSearchResult("Recent Videos ...  ");
			 if (this->userName=="")
				 QMessageBox::warning(this, QString("Warning"), QString("Please log in before watching your recent videos!"), QMessageBox::Yes);
			 return;
		 }
		 else
		 {
			 ShowAvailableMediaFromDb();
			 ShowSearchResult("Hot movies  ...  ");
		 }
		 if (this->ui.treeView->isExpanded(modelIndex))
			 this->ui.treeView->collapse(modelIndex);
		 else
			 this->ui.treeView->expand(modelIndex);
		 return;
	 }

	 //Select the child node
	 if (selectType == subjectType)
	 {
		 ShowAvailableMediaFromDb("", selectItemText, subjectType);
		 SetSearchInfo();
		 ShowSearchResult("Hot movies  ...  ");
	 }
	 else if (selectType == gradeType)
	 {
		 ShowAvailableMediaFromDb("", selectItemText, gradeType);
		 SetSearchInfo();
		 ShowSearchResult("Hot movies  ...  ");
	 }
	 
}
void OnDemandGui::SetSearchInfo	    (QString currentSearchKey)
{
	if (currentSearchKey=="")
		this->ui.lineEdit_search->setText("");
	this->currentSearchKey = currentSearchKey;
}
void OnDemandGui::ShowSearchResult(QString text,  QString searchWord, QString searchCategory, QString count)
{
	if (text!="")
	{
		this->ui.label_search_state->setText(text);
	} 
	else
	{
		QString categoryText;
		if (searchCategory!="")
			categoryText = "in  '" + searchCategory + "' "; 
		QString searchResult = "Search '" + searchWord +"'  "+ categoryText + " :  " + count + " results ...";
		this->ui.label_search_state->setText(searchResult);
	}
}
void OnDemandGui::refreshMyFavoriteUI		()
{
	if (this->selectItemText == myFavorite)
		this->ShowAvailableMediaFromDb(currentSearchKey, selectItemText, selectType);
}
void OnDemandGui::AddUserLastAccessVideoToDb		(QString userID, QString mediaID, int progress, int progressMax)
{
	int recCount=0;
	QString earliestUW_ID, updateUW_ID;
	QSqlQuery sql_query;
	QString sql;

	/********************************Add/update record to Db*****************************/
	//Select the repeat one among the recent videos
	sql = QString("SELECT UW_ID FROM userwatched WHERE UW_UserID=" + userID + " AND MI_ID = " + mediaID + " ");
	sql_query.prepare(sql);
	if(!sql_query.exec())
	{
		qDebug() << "\t" << "Delete repeat record from userwatched failed! ";
		return;
	}
	else  if (sql_query.next())
	{
		updateUW_ID = sql_query.value(0).toString();
	}
	//Add or update record
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString currentDateTime = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
	if (updateUW_ID=="")
		sql = QString("INSERT INTO userwatched(UW_UserID, MI_ID, UW_Progress, UW_ProgressMax, UW_LastAccessTime) VALUES(" + userID + ", " +  mediaID + ", "+ QString::number(progress, 10) + ", " + QString::number(progressMax) + ", '" + currentDateTime +  "' ) ");
	else
		sql = QString("UPDATE userwatched SET UW_Progress = " + QString::number(progress, 10) + ", UW_ProgressMax =" +QString::number(progressMax) + ", UW_LastAccessTime='" + currentDateTime + "' WHERE UW_ID=" + updateUW_ID + " ");
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		qDebug() << "\t" << "Add record to userwatched failed!";
		return;
	}

	//If it's an insert...
	if (updateUW_ID=="")
	{
		//Get the video count this user has watched
		sql = QString("SELECT UW_ID FROM userwatched WHERE UW_UserID=" + userID +" ORDER BY UW_LastAccessTime");
		sql_query.prepare(sql);
		if (!sql_query.exec())
		{
			qDebug() << "\t" << "Select count from userwatched failed!";
			return;
		}
		else if (sql_query.next())
		{
			earliestUW_ID = sql_query.value(0).toString();
			recCount = sql_query.size();
		}

		//Delete record from Db if more than 3 records are restored
		if (recCount>=3)
		{
			sql = QString("DELETE FROM userwatched WHERE UW_ID = " + earliestUW_ID + " ");
			sql_query.prepare(sql);
			if (!sql_query.exec())
			{
				qDebug() << "\t" << "Delete from userwatched failed! ";
				return;
			}
		}
	}

}
void OnDemandGui::refreshRecentVideoUI			()
{
	if (this->selectItemText==recentVideos)
		this->ShowAvailableMediaFromDb(currentSearchKey, selectItemText, selectType);
}
