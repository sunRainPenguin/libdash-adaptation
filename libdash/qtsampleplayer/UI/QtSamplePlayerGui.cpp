/*
 * qtsampleplayer.cpp
 *****************************************************************************
 * Copyright (C) 2012, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#include <QtWidgets>
#include <vector>
#include <sstream>
#include "QtSamplePlayerGui.h"
#include "IDASHPlayerGuiObserver.h"
#include "libdash.h"
#include <QMultiHash>
using namespace sampleplayer;
using namespace sampleplayer::renderer;
using namespace dash::mpd;
using namespace libdash::framework::mpd;

QtSamplePlayerGui::QtSamplePlayerGui    (bool hasLogedIn, QString userID, QString Name, QString mediaID, QString mediaName, QString mpdUrl, QWidget *parent) : 
                   QMainWindow          (parent),
					   hasLogedIn	(hasLogedIn),
					   userID	(userID),
					   userName	(Name),
					   mediaID		(mediaID),
					   mediaName	(mediaName),
					   favorite	(favorite_notSet),
					   mpdUrl	(mpdUrl),
                   ui                   (new Ui::QtSamplePlayerClass),
                   mpd                  (NULL)
{
    this->ui->setupUi(this);
	this->favorite = this->GetFavoriteState(userID, mediaID);
	this->SetFavoriteHeart(this->favorite);

	this->ui->label_userName->setText(userName);
	this->ui->label_mediaName->setText(mediaName);
    this->SetVideoSegmentBufferFillState(0);
    this->SetVideoBufferFillState(0);
    this->SetAudioSegmentBufferFillState(0);
    this->SetAudioBufferFillState(0);
    this->ui->button_pause->setEnabled(false);
	this->ui->button_stop->setEnabled(false);
    this->ui->button_start->setEnabled(false);
	this->ui->progressSlider->setEnabled(false);			//2015.4.14 - php

	this->ShowCommentsFromDb(this->mediaID);

	//2015.4.26 - php
	QString qss_mainWindow;
	QFile qssFile(":/qss/qss_mainWindow.qss");
	qssFile.open(QFile::ReadOnly);

	if(qssFile.isOpen())
	{
		qss_mainWindow = QString::fromLocal8Bit(qssFile.readAll());
		this->setStyleSheet(qss_mainWindow);
		qssFile.close();
	}
}
QtSamplePlayerGui::~QtSamplePlayerGui   ()
{
    delete (this->ui);
}

void            QtSamplePlayerGui::ClearComboBoxes                                  ()
{
    this->ui->cb_period->clear();

    this->ui->cb_video_adaptationset->clear();
    this->ui->cb_video_representation->clear();

    this->ui->cb_audio_adaptationset->clear();
    this->ui->cb_audio_representation->clear();
}
QTGLRenderer*   QtSamplePlayerGui::GetVideoElement                                  ()
{
    return this->ui->videoelement;
}

void            QtSamplePlayerGui::SetGuiFields                                     (dash::mpd::IMPD* mpd)
{
    this->LockUI();
    this->ClearComboBoxes();
    this->SetPeriodComboBox(mpd, this->ui->cb_period);
    
    if (mpd->GetPeriods().size() > 0)
    {
        IPeriod *period = mpd->GetPeriods().at(0);

        this->SetVideoAdaptationSetComboBox(period, this->ui->cb_video_adaptationset);
        this->SetAudioAdaptationSetComboBox(period, this->ui->cb_audio_adaptationset);

        if (!AdaptationSetHelper::GetVideoAdaptationSets(period).empty())
        {
            IAdaptationSet *adaptationSet = AdaptationSetHelper::GetVideoAdaptationSets(period).at(0);

            this->SetRepresentationComoboBox(adaptationSet, this->ui->cb_video_representation);
        }
        if (!AdaptationSetHelper::GetAudioAdaptationSets(period).empty())
        {
            IAdaptationSet *adaptationSet = AdaptationSetHelper::GetAudioAdaptationSets(period).at(0);

            this->SetRepresentationComoboBox(adaptationSet, this->ui->cb_audio_representation);
        }
    }

    this->mpd = mpd;
    this->UnLockUI();

    this->ui->button_start->setEnabled(true);
}
void            QtSamplePlayerGui::SetVideoSegmentBufferFillState                   (int percentage)
{
    this->ui->progressBar_V->setValue(percentage);
}
void            QtSamplePlayerGui::SetVideoBufferFillState                          (int percentage)
{
    this->ui->progressBar_VF->setValue(percentage);
}
void            QtSamplePlayerGui::SetAudioSegmentBufferFillState                   (int percentage)
{
    this->ui->progressBar_A->setValue(percentage);
}
void            QtSamplePlayerGui::SetAudioBufferFillState                          (int percentage)
{
    this->ui->progressBar_AC->setValue(percentage);
}
void            QtSamplePlayerGui::AddWidgetObserver                                (IDASHPlayerGuiObserver *observer)
{
    this->observers.push_back(observer);
}
void            QtSamplePlayerGui::SetStatusBar                                     (const QString& text)
{
    this->ui->statusBar->showMessage(text);
}

//2015.4.10 - php
void			  QtSamplePlayerGui::SetProgressSlider		(int  segmentDisplayIndex)
{
		this->ui->progressSlider->setValue(segmentDisplayIndex);
}
//2015.4.18 - php
void         QtSamplePlayerGui::SetVideoProgressLabel  (int segmentDisplayIndex)
{
		this->ui->label_videoSegmentIndex->setText(QString::fromLocal8Bit("当前视频播放分段：")+ QString::number(segmentDisplayIndex, 10));
}

void         QtSamplePlayerGui::SetAudioProgressLabel  (int segmentDisplayIndex)
{
	this->ui->label_audioSegmentIndex ->setText(QString::fromLocal8Bit("当前音频播放分段：")+ QString::number(segmentDisplayIndex, 10));
}

void			QtSamplePlayerGui::SetProgressSliderRange	(int duration)
{
	this->ui->progressSlider->setRange(0, duration);
}
//2015.4.15 - php
void			   QtSamplePlayerGui::SetEndOfProgressSlider			()
{
	int maxProgress = this->ui->progressSlider->maximum();
	this->ui->progressSlider->setValue(maxProgress);
}

void    QtSamplePlayerGui::SetRateChangedLabel		        (int segmentNumber, unsigned int downloadRate){
	//std::cout << "GUI #" << segmentNumber << std::endl;
	this->ui->label_segment_speed->setText(QString("# %1 - %2 KiB/s").arg(segmentNumber).arg(downloadRate/1000.0));
}

void    QtSamplePlayerGui::SetBWChangedLabel		        (unsigned int BW){
	//std::cout << "GUI #" << segmentNumber << std::endl;
	this->ui->label_profile->setText(QString("%1 bps").arg(BW));
}

void            QtSamplePlayerGui::SetRepresentationComoboBox                       (dash::mpd::IAdaptationSet *adaptationSet, QComboBox *cb)
{
    std::vector<IRepresentation *> represenations = adaptationSet->GetRepresentation();
    cb->clear();

    for(size_t i = 0; i < represenations.size(); i++)
    {
        IRepresentation *representation = represenations.at(i);

        std::stringstream ss;
        ss << representation->GetId() << " " << representation->GetBandwidth() << " bps "  << representation->GetWidth() << "x" << representation->GetHeight();

        cb->addItem(QString(ss.str().c_str()));
    }
}
void            QtSamplePlayerGui::SetAdaptationSetComboBox                         (dash::mpd::IPeriod *period, QComboBox *cb)
{
    std::vector<IAdaptationSet *> adaptationSets = period->GetAdaptationSets();
    cb->clear();

    for(size_t i = 0; i < adaptationSets.size(); i++)
    {
        IAdaptationSet *adaptationSet = adaptationSets.at(i);

        std::stringstream ss;
        ss << "AdaptationSet " << i+1;

        cb->addItem(QString(ss.str().c_str()));
    }
}
void            QtSamplePlayerGui::SetAudioAdaptationSetComboBox                    (dash::mpd::IPeriod *period, QComboBox *cb)
{
    std::vector<IAdaptationSet *> adaptationSets = AdaptationSetHelper::GetAudioAdaptationSets(period);
    cb->clear();

    for(size_t i = 0; i < adaptationSets.size(); i++)
    {
        IAdaptationSet *adaptationSet = adaptationSets.at(i);

        std::stringstream ss;
        ss << "AdaptationSet " << i+1;

        cb->addItem(QString(ss.str().c_str()));
    }
}
void            QtSamplePlayerGui::SetVideoAdaptationSetComboBox                    (dash::mpd::IPeriod *period, QComboBox *cb)
{
    std::vector<IAdaptationSet *> adaptationSets = AdaptationSetHelper::GetVideoAdaptationSets(period);
    cb->clear();

    for(size_t i = 0; i < adaptationSets.size(); i++)
    {
        IAdaptationSet *adaptationSet = adaptationSets.at(i);

        std::stringstream ss;
        ss << "AdaptationSet " << i+1;

        cb->addItem(QString(ss.str().c_str()));
    }
}
void            QtSamplePlayerGui::SetPeriodComboBox                                (dash::mpd::IMPD *mpd, QComboBox *cb)
{
    std::vector<IPeriod *> periods = mpd->GetPeriods();
    cb->clear();

    for(size_t i = 0; i < periods.size(); i++)
    {
        IPeriod *period = periods.at(i);

        std::stringstream ss;
        ss << "Period " << i+1;

        cb->addItem(QString(ss.str().c_str()));
    }
}
void            QtSamplePlayerGui::LockUI                                           ()
{
    this->setEnabled(false);
}
void            QtSamplePlayerGui::UnLockUI                                         ()
{
    this->setEnabled(true);
}
/* Notifiers */
void            QtSamplePlayerGui::NotifySettingsChanged                            ()
{
    this->LockUI();

    int period              = this->ui->cb_period->currentIndex();
    int videoAdaptionSet    = this->ui->cb_video_adaptationset->currentIndex();
    int videoRepresentation = this->ui->cb_video_representation->currentIndex();
    int audioAdaptionSet    = this->ui->cb_audio_adaptationset->currentIndex();
    int audioRepresentation = this->ui->cb_audio_representation->currentIndex();

    for(size_t i = 0; i < this->observers.size(); i++)
        this->observers.at(i)->OnSettingsChanged(period, videoAdaptionSet, videoRepresentation, audioAdaptionSet, audioRepresentation);

    this->UnLockUI();
}
void            QtSamplePlayerGui::NotifyMPDDownloadPressed                         (const std::string &url)
{
    for(size_t i = 0; i < this->observers.size(); i++)
        this->observers.at(i)->OnDownloadMPDPressed(url);
}
void            QtSamplePlayerGui::NotifyStartButtonPressed                         (int progress)
{
    this->LockUI();

    int period              = this->ui->cb_period->currentIndex();
    int videoAdaptionSet    = this->ui->cb_video_adaptationset->currentIndex();
    int videoRepresentation = this->ui->cb_video_representation->currentIndex();
    int audioAdaptionSet    = this->ui->cb_audio_adaptationset->currentIndex();
    int audioRepresentation = this->ui->cb_audio_representation->currentIndex();

    for(size_t i = 0; i < this->observers.size(); i++)
	{
		if (progress>0)
			this->observers.at(i)->OnStartButtonPressed(period, videoAdaptionSet, videoRepresentation, audioAdaptionSet, audioRepresentation, progress);
		else
			this->observers.at(i)->OnStartButtonPressed(period, videoAdaptionSet, videoRepresentation, audioAdaptionSet, audioRepresentation);
	}

    this->UnLockUI();
}
void            QtSamplePlayerGui::NotifyPauseButtonPressed                          ()
{
	for(size_t i = 0; i < this->observers.size(); i++)
		this->observers.at(i)->OnPauseButtonPressed();
}
void            QtSamplePlayerGui::NotifyStopButtonPressed                          ()
{
	for(size_t i = 0; i < this->observers.size(); i++)
		this->observers.at(i)->OnStopButtonPressed();
}

//2015.4.13 - php
void   QtSamplePlayerGui::NotifyProgressSliderReleased  (int progress)
{
	for (size_t i = 0; i < this->observers.size(); i++)
		this->observers.at(i)->OnProgressSliderReleased(progress);
}
void QtSamplePlayerGui::NotifyProgressSliderPressed  ()
{
	for (size_t i = 0; i < this->observers.size(); i++)
		this->observers.at(i)->OnProgressSliderPressed();
}

void            QtSamplePlayerGui::on_cb_period_currentIndexChanged                 (int index)
{
    if(index == -1 || this->mpd == NULL)
        return; // No Item set

    this->LockUI();

    this->SetAudioAdaptationSetComboBox(mpd->GetPeriods().at(index), ui->cb_audio_adaptationset);
    this->SetVideoAdaptationSetComboBox(mpd->GetPeriods().at(index), ui->cb_video_adaptationset);

    this->NotifySettingsChanged();

    this->UnLockUI();
}
void            QtSamplePlayerGui::on_cb_video_adaptationset_currentIndexChanged    (int index)
{
    if(index == -1 || this->mpd == NULL)
        return; // No Item set

    this->LockUI();

    IPeriod *period = this->mpd->GetPeriods().at(this->ui->cb_period->currentIndex());

    this->SetRepresentationComoboBox(AdaptationSetHelper::GetVideoAdaptationSets(period).at(index), this->ui->cb_video_representation);

    this->NotifySettingsChanged();

    this->UnLockUI();
}
void            QtSamplePlayerGui::on_cb_video_representation_currentIndexChanged   (int index)
{
    if(index == -1)
        return; // No Item set

    this->NotifySettingsChanged();
}
void            QtSamplePlayerGui::on_cb_audio_adaptationset_currentIndexChanged    (int index)
{
    if(index == -1 || this->mpd == NULL)
        return; // No Item set

    this->LockUI();

    IPeriod *period = this->mpd->GetPeriods().at(this->ui->cb_period->currentIndex());

    this->SetRepresentationComoboBox(AdaptationSetHelper::GetAudioAdaptationSets(period).at(index), this->ui->cb_audio_representation);

    this->NotifySettingsChanged();

    this->UnLockUI();
}
void            QtSamplePlayerGui::on_cb_audio_representation_currentIndexChanged   (int index)
{
    if(index == -1)
        return; // No Item set

    this->NotifySettingsChanged();
}
void            QtSamplePlayerGui::on_button_start_clicked                          ()
{
    this->ui->button_start->setEnabled(false);
    this->ui->button_pause->setEnabled(true);
	this->ui->button_stop->setEnabled(true);
	this->ui->progressSlider->setEnabled(true);				//2015.4.14 - php
    this->NotifyStartButtonPressed();
}
void            QtSamplePlayerGui::on_button_pause_clicked                           ()
{
    this->ui->button_start->setEnabled(true);
    this->ui->button_pause->setEnabled(false);
	this->ui->button_stop->setEnabled(false);
	this->ui->progressSlider->setEnabled(false);			//2015.4.14 - php
    this->NotifyPauseButtonPressed();
}

void            QtSamplePlayerGui::on_button_stop_clicked                           ()
{
	this->ui->button_start->setEnabled(true);
	this->ui->button_pause->setEnabled(false);
	this->ui->button_stop->setEnabled(false);
	this->ui->progressSlider->setEnabled(false);			//2015.4.14 - php
	this->NotifyStopButtonPressed();
}

void QtSamplePlayerGui::on_progressSlider_sliderReleased()
{
	int progress = this->ui->progressSlider->value();
	this->NotifyProgressSliderReleased (progress);
}

void  QtSamplePlayerGui::on_progressSlider_sliderPressed()
{
	this->NotifyProgressSliderPressed();
}

void QtSamplePlayerGui::SetMpdUrl(QString mpdUrl)
{
	this->mpdUrl = mpdUrl;
}
void QtSamplePlayerGui::SetCurrMediaInfo(QString mediaID, QString mediaName)
{
	this->mediaID = mediaID;
	this->mediaName = mediaName;
	this->ui->label_mediaName->setText(mediaName);
	this->favorite = this->GetFavoriteState(userID, mediaID);
	this->SetFavoriteHeart(this->favorite);
	this->ShowCommentsFromDb(mediaID);
}
void QtSamplePlayerGui::ClearMpd()
{
	this->mpd = NULL;
}
void QtSamplePlayerGui::ClickButtonStop()
{
	this->ui->button_start->setEnabled(true);
	this->ui->button_pause->setEnabled(false);
	this->ui->button_stop->setEnabled(false);
	this->ui->progressSlider->setEnabled(false);			//2015.4.14 - php
	this->NotifyStopButtonPressed();
}
bool QtSamplePlayerGui::IsStarted()
{
	if (!this->ui->button_start->isEnabled())
	{
		return true;
	} 
	else
	{
		return false;
	}
}
void QtSamplePlayerGui::ClickButtonStart()
{
	this->ui->button_start->setEnabled(false);
	this->ui->button_pause->setEnabled(true);
	this->ui->button_stop->setEnabled(true);
	this->ui->progressSlider->setEnabled(true);				//2015.4.14 - php
	int progressMax = 0;
	int progress = this->GetProgressFromDb (this->userID, this->mediaID, progressMax);
	if (progress>0)
	{
		double  dpercent = (double)progress/(double)progressMax;
		int ipercent = dpercent*100;
		this->NotifyStartButtonPressed(progress);
		this->SetProgressSlider(progress);
		this->SetStatusBar(QString::fromLocal8Bit("从上次播放的 ")+ QString::number(ipercent, 10)+QString::fromLocal8Bit("%  继续播放。。。"));
	}
	else
		this->NotifyStartButtonPressed();
}
QString QtSamplePlayerGui::GetMediaID()
{
	return this->mediaID;
}
void QtSamplePlayerGui::closeEvent( QCloseEvent * event )
{
	int progress = this->ui->progressSlider->value();
	int progressMax = this->ui->progressSlider->maximum();
	this->ClickButtonStop();
	emit ClosePlayerGui(progress, progressMax);
	event->accept();
}

void QtSamplePlayerGui::on_button_comment_clicked()
{
	if (!hasLogedIn)
	{
		emit LoginBeforeComment();
	} 
	else
	{
		commentDialog = new CommentDialog(this->mediaID, this->userID,  this);
		QObject::connect(commentDialog, SIGNAL(CommentSuccessfully()), this, SLOT(UpdateComment()));
		commentDialog->exec();
	}
}

void QtSamplePlayerGui::SetLoginState(QString userID, QString userName)
{
	this->hasLogedIn = true;
	this->userName = userName;
	this->userID = userID;
	this->ui->label_userName->setText(userName);
	this->favorite = this->GetFavoriteState(userID, mediaID);
	this->SetFavoriteHeart(this->favorite);
}

void QtSamplePlayerGui::ShowCommentsFromDb		(QString MI_ID)
{
	int indexUsername, indexCommentTime, indexCommentText;
	QString userName, commentTime, commentText, comment;

	QSqlQuery sql_query; 
	QString select_sql;
	QSqlRecord rec;

	select_sql =QString("SELECT user.username, uc.UC_CommentTime, uc.UC_CommentText FROM user, usercomment AS uc WHERE uc.MI_ID = ") + MI_ID + QString(" and uc.User_ID = user.ID ORDER BY uc.UC_ID DESC LIMIT 15");
	sql_query.prepare(select_sql);
	if (!sql_query.exec() || sql_query.size()<1)
	{
		qDebug() << "\t" <<"Select comment failed! ";
		return;
	} 

	rec = sql_query.record();
	while (sql_query.next())
	{
		indexUsername = rec.indexOf(QString("username"));
		userName = sql_query.value(indexUsername).toString();

		indexCommentTime = rec.indexOf(QString("UC_CommentTime"));
		QDateTime temp = sql_query.value(indexCommentTime).toDateTime();
		commentTime = temp.toString("yyyy-MM-dd hh:mm:ss");

		indexCommentText = rec.indexOf(QString("UC_CommentText"));
		commentText = sql_query.value(indexCommentText).toString();

		comment = comment + userName + QString(" (") + commentTime + QString("):  ") + QString( "\r\n") + commentText + QString( "\r\n");

	}

	int vx = this->ui->textEdit_comment->verticalScrollBar()->value();			//显示完成后保留滚动条的值，防止刷新滚动
	
	QTextDocument *document = this->ui->textEdit_comment->document();
	QTextCursor cursor = this->ui->textEdit_comment->textCursor();
	this->ui->textEdit_comment->setReadOnly(true);
	this->ui->textEdit_comment->setText("");  

	/****************************text****************************/
	int firstEmotionIndex =  comment .indexOf(QString("[/"));
    this->ui->textEdit_comment->insertPlainText(comment.mid(0,firstEmotionIndex));

	int from = 0;
	while(from<=comment.size())
	{
		/****************************picture****************************/
		int indexPrefix =comment .indexOf(QString("[/"), from);
		int indexSuffix = comment.indexOf(QString("]"), from);
		int charCount =  indexSuffix - indexPrefix - 2;
		QString emotionName = comment.mid(indexPrefix+2, charCount);
		QString emotionJpg = Global::emotionPathTemp + QString("/") + emotionName +QString(".jpg");
		QString emotionJpeg = Global::emotionPathTemp+ QString("/") + emotionName +QString(".jpeg");
		QString emotionPng= Global::emotionPathTemp + QString("/") + emotionName +QString(".png");
		 QFileInfo fiJpg(emotionJpg);
		 QFileInfo fiJpeg(emotionJpeg);
		 QFileInfo fiPng(emotionPng);

		 if (fiJpg.isFile())
		 {
			 QImage image(emotionJpg);
			 image = image.scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			 cursor.movePosition(QTextCursor::End);
			 document->addResource(QTextDocument::ImageResource, QUrl(emotionJpg), image);
			 cursor.insertImage(emotionJpg);
		 }
		 else if (fiPng.isFile())
		 {
			 QImage image(emotionPng);
			 image = image.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			 cursor.movePosition(QTextCursor::End);
			 document->addResource(QTextDocument::ImageResource, QUrl(emotionPng), image);
			 cursor.insertImage(emotionPng);
		 }
		 else if (fiJpeg.isFile())
		 {
			 QImage image(emotionJpeg);
			 image = image.scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			 cursor.movePosition(QTextCursor::End);
			 document->addResource(QTextDocument::ImageResource, QUrl(emotionJpeg), image);
			 cursor.insertImage(emotionJpeg);
		 }

		 /*****************************text****************************/
		 int indexNextEmotion = comment.indexOf(QString("[/"), indexSuffix+1);
		 if (indexNextEmotion>=0)
		 {
			  int textCount = indexNextEmotion-indexSuffix-1;
			  QString text = comment.mid(indexSuffix+1, textCount);
			  this->ui->textEdit_comment->insertPlainText(text);
		 }else
		 {
			 //no emotion any more
			 int textCount = comment.size() + 1 - indexSuffix;
			 QString text = comment.mid(indexSuffix+1, textCount);
			 this->ui->textEdit_comment->insertPlainText(text);
			 break;
		 }

		 from = indexSuffix+1;
	}
		this->ui->textEdit_comment->verticalScrollBar()->setValue(vx);

}

void QtSamplePlayerGui::SetLogoutState()
{
	this->hasLogedIn = false;
	this->userName = "";
	this->userID = "";
	this->ui->label_userName->setText("");
	this->favorite = this->GetFavoriteState(userID, mediaID);
	this->SetFavoriteHeart(this->favorite);
}

void QtSamplePlayerGui::UpdateComment()
{
	this->ShowCommentsFromDb(this->mediaID);
}

void QtSamplePlayerGui::on_button_refresh_comment_clicked()
{
	this->ShowCommentsFromDb(this->mediaID);
}
IsFavorite QtSamplePlayerGui::GetFavoriteState				(QString userID, QString mediaID)
{
	QSqlQuery sql_query; 
	QString select_sql;

	if (userID=="" || mediaID=="")
		return favorite_notSet;
	select_sql =QString(" SELECT * FROM UserFavorite WHERE UF_UserID=") + userID +QString(" AND MI_ID=") + mediaID + QString(" ") ;
	sql_query.prepare(select_sql);
	if (!sql_query.exec() || sql_query.size()<1)
	{
		qDebug() << "\t" <<"Select UserFavorite failed! ";
		return favorite_unchecked;
	} 
	else
		return favorite_checked;

}
void QtSamplePlayerGui::SetFavoriteHeart(IsFavorite favorite)
{
	if (favorite==favorite_checked)
	{
		QIcon icon("./pic/favorite_checked.ico");
		this->ui->button_favorite->setIcon(icon);
		this->ui->button_favorite->setIconSize(QSize(30, 30));
		this->ui->button_favorite->setEnabled(true);
	}
	else if(favorite==favorite_unchecked)
	{
		QIcon icon("./pic/favorite_unchecked.ico");
		this->ui->button_favorite->setIcon(icon);
		this->ui->button_favorite->setIconSize(QSize(30, 30));
		this->ui->button_favorite->setEnabled(true);
	}
	else if(favorite==favorite_notSet)
	{
		this->ui->button_favorite->setIcon(QIcon());
		this->ui->button_favorite->setDisabled(true);
	}

}
void QtSamplePlayerGui::on_button_favorite_clicked					()
{
	if (favorite==favorite_unchecked)
	{
		this->favorite = favorite_checked;
		SetFavoriteHeart(favorite_checked);
		SetFavoriteToDb(favorite_checked);
	}
	else if (favorite==favorite_checked)
	{
		this->favorite = favorite_unchecked;
		SetFavoriteHeart(favorite_unchecked);
		SetFavoriteToDb(favorite_unchecked);
	}
	emit MyFavoriteChanged();
}
void QtSamplePlayerGui::SetFavoriteToDb	(IsFavorite favorite)
{
	if (userID!="" && mediaID!="")
	{
		QSqlQuery sql_query; 
		QString select_sql;
		if (favorite==favorite_checked)
		{
			select_sql =QString(" INSERT INTO UserFavorite(UF_UserID, MI_ID) VALUES(") + userID + "," + mediaID + ") ";
			sql_query.prepare(select_sql);
			if (!sql_query.exec())
			{
				qWarning() << "\t" << "Insert favorite data failed! ";
				return;
			}
		}
		else if (favorite==favorite_unchecked)
		{
			select_sql =QString(" DELETE FROM UserFavorite WHERE UF_UserID=") + userID + " AND MI_ID=" + mediaID + " ";
			sql_query.prepare(select_sql);
			if (!sql_query.exec())
			{
				qWarning() << "\t" << "Delete favorite data failed! ";
				return;
			}
		}
	}
}
int QtSamplePlayerGui::GetProgress()
{
	return this->ui->progressSlider->value();
}
int QtSamplePlayerGui::GetProgressMax()
{
	return this->ui->progressSlider->maximum();
}

int QtSamplePlayerGui::GetProgressFromDb				(QString userID, QString MI_ID, int& progressMax)
{
	QSqlQuery sql_query;
	QString sql;

	sql = QString("SELECT DISTINCT MI_ID, UW_Progress, UW_ProgressMAX FROM UserWatched WHERE UW_UserID=") + userID + " AND MI_ID =" + MI_ID;
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		qDebug() << "\t" << "Select progress from UserWatched failed! ";
		return -1;
	}
	else if (sql_query.next())
	{
		QSqlRecord rec = sql_query.record();
		int indexProgress = rec.indexOf("UW_Progress");
		int progress = sql_query.value(indexProgress).toInt();
		int indexProgressMax = rec.indexOf("UW_ProgressMAX");
		progressMax = sql_query.value(indexProgressMax).toInt();

		return progress;
	}
	return -1;
}