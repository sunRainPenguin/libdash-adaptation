/*
 * qtsampleplayer.h
 *****************************************************************************
 * Copyright (C) 2012, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#ifndef QTSAMPLEPLAYER_H
#define QTSAMPLEPLAYER_H

#include <QtMultimedia/qmediaplayer.h>
#include <QtMultimediaWidgets/qvideowidget.h>
#include <QtGui/QMovie>
#include <QtWidgets/QMainWindow>
#include "ui_qtsampleplayer.h"
#include "libdash.h"
#include "../libdashframework/MPD/AdaptationSetHelper.h"

#include "sqlconfig.h"
#include "CommentDialog.h"



namespace sampleplayer
{
	enum IsFavorite{
		favorite_unchecked,
		favorite_checked,
		favorite_notSet
	};
    class IDASHPlayerGuiObserver;

    class QtSamplePlayerGui : public QMainWindow
    {
        Q_OBJECT

        public:
            QtSamplePlayerGui           (bool hasLogedIn, QString userID, QString userName, QString mediaID, QString mediaName, QString mpdUrl,  QWidget *parent = 0);
            virtual ~QtSamplePlayerGui  ();

            void                                    SetGuiFields            (dash::mpd::IMPD* mpd);
            virtual void                            AddWidgetObserver       (IDASHPlayerGuiObserver* observer);
            virtual void                            SetStatusBar            (const std::string& text);
            sampleplayer::renderer::QTGLRenderer*   GetVideoElement         ();
			void									SetMpdUrl(QString mpdUrl);				//2015.4.30 - php
			void									SetCurrMediaInfo(QString mediaID, QString mediaName);	//2015.5.2 - php
			void									ClearMpd();					//2015.5.1 - php
			void									ClickButtonStop();			//2015.5.1 - php
			void									ClickButtonStart();		//2015.5.1 - php
			bool									IsStarted();					//2015.5.1 - php
			QString							GetMediaID();				//2015.5.12 - php
	
        private slots:
            void on_cb_period_currentIndexChanged               (int index);
            void on_cb_video_adaptationset_currentIndexChanged  (int index);
            void on_cb_video_representation_currentIndexChanged (int index);
            void on_cb_audio_adaptationset_currentIndexChanged  (int index);
            void on_cb_audio_representation_currentIndexChanged (int index);
            void on_button_start_clicked                         ();
            void on_button_pause_clicked                       ();
			void on_button_stop_clicked                         ();
			void on_progressSlider_sliderReleased           ();			//2015.4.13 - php
			void on_progressSlider_sliderPressed            ();			//2015.4.13 - php
			void on_button_comment_clicked					();			//2015.5.2 - php
			void on_button_refresh_comment_clicked    ();			//2015.5.13 - php
			void on_button_favorite_clicked					();			//2015.5.25 - php

        public slots:
            virtual void    SetVideoSegmentBufferFillState  (int percentage);
            virtual void    SetVideoBufferFillState         (int percentage);
            virtual void    SetAudioSegmentBufferFillState  (int percentage);
            virtual void    SetAudioBufferFillState         (int percentage);
			virtual void	 SetProgressSlider		(int  segmentDisplayIndex);				//2015.4.10 - php
			virtual void    SetVideoProgressLabel  (int segmentDisplayIndex);			//2015.4.18 - php
			virtual void    SetAudioProgressLabel  (int segmentDisplayIndex);			//2015.4.18 - php
			virtual void	 SetProgressSliderRange	(int duration);			//2015.4.10 - php
			virtual void    SetEndOfProgressSlider  ();			//2015.4.15 - php
			virtual void    SetRateChangedLabel		        (int segmentNumber, unsigned int downloadRate);
			virtual void    SetBWChangedLabel		        (unsigned int BW);
			void closeEvent				( QCloseEvent * event );
			void	SetLoginState		(QString userID, QString usesrName);									//2015.5.2 - php
			void SetLogoutState    ();				//2015.5.10 - php
			void UpdateComment		();   //2015.5.13 - php

        private:
            std::map<std::string, std::string>                  keyValues;
            std::map<std::string, int>                          keyIndices;
            std::map<std::string, std::vector<std::string> >    video;
            std::map<std::string, std::vector<std::string> >    audio;

            Ui::QtSamplePlayerClass                 *ui;
			CommentDialog*							commentDialog;//2015.5.3 - php
            std::vector<IDASHPlayerGuiObserver *>   observers;
            dash::mpd::IMPD                         *mpd;
			QString										mpdUrl;			//2015.4.30 - php
			QString										mediaName;		//2015.5.2 - php
			QString										mediaID;			//2015.5.2 - php
			bool											    hasLogedIn;		//2015.5.2 - php
			QString										userName;		//2015.5.2 - php
			QString										userID;				//2015.5.3 - php
			IsFavorite										favorite;			//2015.5.25 - php
            void LockUI                     ();
            void UnLockUI                   ();

            void SetPeriodComboBox              (dash::mpd::IMPD *mpd, QComboBox *cb);
            void SetAdaptationSetComboBox       (dash::mpd::IPeriod *period, QComboBox *cb);
            void SetVideoAdaptationSetComboBox  (dash::mpd::IPeriod *period, QComboBox *cb);
            void SetAudioAdaptationSetComboBox  (dash::mpd::IPeriod *period, QComboBox *cb);
            void SetRepresentationComoboBox     (dash::mpd::IAdaptationSet *adaptationSet, QComboBox *cb);

            void ClearComboBoxes            ();

            void NotifySettingsChanged      ();
            void NotifyMPDDownloadPressed   (const std::string &url);
            void NotifyStartButtonPressed   ();
            void NotifyPauseButtonPressed    ();
			void NotifyStopButtonPressed		();
			void NotifyProgressSliderReleased  (int progress);			//2015.4.13 - php
			void NotifyProgressSliderPressed   ();			//2015.4.13 - php
			void ShowCommentsFromDb		(QString MI_ID);			//2015.5.25 - php
			IsFavorite  GetFavoriteState				(QString userID, QString mediaID);		//2015.5.25 - php
			QString GetEmotionPath();		//2015.5.25 - php
			void SetFavoriteHeart	(IsFavorite favorite);		//2015.5.25 - php
			void SetFavoriteToDb	(IsFavorite favorite);

	signals:
			void ClosePlayerGui();			//2015.5.1 - php
			void LoginBeforeComment();				//2015.5.2 - php 
			void MyFavoriteChanged();			//2015.5.25 - php
    };
}

#endif // QTSAMPLEPLAYER_H
