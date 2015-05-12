/********************************************************************************
** Form generated from reading UI file 'qtsampleplayer.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTSAMPLEPLAYER_H
#define UI_QTSAMPLEPLAYER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "Renderer/QTGLRenderer.h"

QT_BEGIN_NAMESPACE

class Ui_QtSamplePlayerClass
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QWidget *widget;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_3;
    sampleplayer::renderer::QTGLRenderer *videoelement;
    QSlider *progressSlider;
    QPushButton *button_stop;
    QPushButton *button_start;
    QPushButton *button_pause;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_videoSegmentIndex;
    QLabel *label_audioSegmentIndex;
    QWidget *layoutWidget2;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_4;
    QLabel *label_profile;
    QWidget *layoutWidget3;
    QVBoxLayout *verticalLayout_6;
    QLabel *label_3;
    QLabel *label_segment_speed;
    QLabel *label_mediaName;
    QPushButton *button_comment;
    QLabel *label_userName;
    QWidget *w_controls;
    QVBoxLayout *verticalLayout;
    QWidget *w_boxes;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_period;
    QComboBox *cb_period;
    QLabel *label;
    QComboBox *cb_video_adaptationset;
    QLabel *label_2;
    QComboBox *cb_video_representation;
    QLabel *label_audio_adaptationset;
    QComboBox *cb_audio_adaptationset;
    QLabel *label_audio_representation;
    QComboBox *cb_audio_representation;
    QLabel *label_5;
    QProgressBar *progressBar_V;
    QLabel *label_6;
    QProgressBar *progressBar_VF;
    QLabel *label_7;
    QProgressBar *progressBar_A;
    QLabel *label_8;
    QProgressBar *progressBar_AC;
    QTextEdit *textEdit_comment;
    QSpacerItem *verticalSpacer;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtSamplePlayerClass)
    {
        if (QtSamplePlayerClass->objectName().isEmpty())
            QtSamplePlayerClass->setObjectName(QStringLiteral("QtSamplePlayerClass"));
        QtSamplePlayerClass->setEnabled(true);
        QtSamplePlayerClass->resize(1085, 702);
        QtSamplePlayerClass->setAutoFillBackground(false);
        centralWidget = new QWidget(QtSamplePlayerClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        widget = new QWidget(centralWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setEnabled(true);
        widget->setMinimumSize(QSize(600, 0));
        layoutWidget = new QWidget(widget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(9, 39, 781, 561));
        verticalLayout_3 = new QVBoxLayout(layoutWidget);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        videoelement = new sampleplayer::renderer::QTGLRenderer(layoutWidget);
        videoelement->setObjectName(QStringLiteral("videoelement"));
        videoelement->setEnabled(true);
        videoelement->setMinimumSize(QSize(100, 100));
        QFont font;
        font.setStyleStrategy(QFont::PreferDefault);
        videoelement->setFont(font);
        videoelement->setAutoFillBackground(false);

        verticalLayout_3->addWidget(videoelement);

        progressSlider = new QSlider(layoutWidget);
        progressSlider->setObjectName(QStringLiteral("progressSlider"));
        progressSlider->setStyleSheet(QStringLiteral("background: transparent;"));
        progressSlider->setOrientation(Qt::Horizontal);

        verticalLayout_3->addWidget(progressSlider);

        button_stop = new QPushButton(widget);
        button_stop->setObjectName(QStringLiteral("button_stop"));
        button_stop->setGeometry(QRect(173, 623, 75, 23));
        button_start = new QPushButton(widget);
        button_start->setObjectName(QStringLiteral("button_start"));
        button_start->setGeometry(QRect(11, 623, 75, 23));
        button_start->setStyleSheet(QStringLiteral(""));
        button_pause = new QPushButton(widget);
        button_pause->setObjectName(QStringLiteral("button_pause"));
        button_pause->setGeometry(QRect(92, 623, 75, 23));
        layoutWidget1 = new QWidget(widget);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(540, 620, 141, 32));
        verticalLayout_4 = new QVBoxLayout(layoutWidget1);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_videoSegmentIndex = new QLabel(layoutWidget1);
        label_videoSegmentIndex->setObjectName(QStringLiteral("label_videoSegmentIndex"));
        label_videoSegmentIndex->setMaximumSize(QSize(182, 49));
        label_videoSegmentIndex->setScaledContents(true);

        verticalLayout_4->addWidget(label_videoSegmentIndex);

        label_audioSegmentIndex = new QLabel(layoutWidget1);
        label_audioSegmentIndex->setObjectName(QStringLiteral("label_audioSegmentIndex"));
        label_audioSegmentIndex->setMaximumSize(QSize(182, 62));
        label_audioSegmentIndex->setScaledContents(true);

        verticalLayout_4->addWidget(label_audioSegmentIndex);

        layoutWidget2 = new QWidget(widget);
        layoutWidget2->setObjectName(QStringLiteral("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(430, 620, 98, 32));
        verticalLayout_5 = new QVBoxLayout(layoutWidget2);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(layoutWidget2);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout_5->addWidget(label_4);

        label_profile = new QLabel(layoutWidget2);
        label_profile->setObjectName(QStringLiteral("label_profile"));

        verticalLayout_5->addWidget(label_profile);

        layoutWidget3 = new QWidget(widget);
        layoutWidget3->setObjectName(QStringLiteral("layoutWidget3"));
        layoutWidget3->setGeometry(QRect(260, 621, 161, 32));
        verticalLayout_6 = new QVBoxLayout(layoutWidget3);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(layoutWidget3);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAutoFillBackground(false);

        verticalLayout_6->addWidget(label_3);

        label_segment_speed = new QLabel(layoutWidget3);
        label_segment_speed->setObjectName(QStringLiteral("label_segment_speed"));

        verticalLayout_6->addWidget(label_segment_speed);

        label_mediaName = new QLabel(widget);
        label_mediaName->setObjectName(QStringLiteral("label_mediaName"));
        label_mediaName->setGeometry(QRect(10, 0, 671, 31));
        button_comment = new QPushButton(widget);
        button_comment->setObjectName(QStringLiteral("button_comment"));
        button_comment->setGeometry(QRect(700, 620, 71, 21));
        label_userName = new QLabel(widget);
        label_userName->setObjectName(QStringLiteral("label_userName"));
        label_userName->setGeometry(QRect(693, 1, 151, 31));

        horizontalLayout->addWidget(widget);

        w_controls = new QWidget(centralWidget);
        w_controls->setObjectName(QStringLiteral("w_controls"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(w_controls->sizePolicy().hasHeightForWidth());
        w_controls->setSizePolicy(sizePolicy);
        w_controls->setAutoFillBackground(false);
        w_controls->setStyleSheet(QStringLiteral(""));
        verticalLayout = new QVBoxLayout(w_controls);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(-1, 0, -1, -1);
        w_boxes = new QWidget(w_controls);
        w_boxes->setObjectName(QStringLiteral("w_boxes"));
        w_boxes->setMinimumSize(QSize(200, 0));
        w_boxes->setMaximumSize(QSize(200, 800));
        verticalLayout_2 = new QVBoxLayout(w_boxes);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(-1, 0, -1, -1);
        label_period = new QLabel(w_boxes);
        label_period->setObjectName(QStringLiteral("label_period"));

        verticalLayout_2->addWidget(label_period);

        cb_period = new QComboBox(w_boxes);
        cb_period->setObjectName(QStringLiteral("cb_period"));

        verticalLayout_2->addWidget(cb_period);

        label = new QLabel(w_boxes);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_2->addWidget(label);

        cb_video_adaptationset = new QComboBox(w_boxes);
        cb_video_adaptationset->setObjectName(QStringLiteral("cb_video_adaptationset"));

        verticalLayout_2->addWidget(cb_video_adaptationset);

        label_2 = new QLabel(w_boxes);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout_2->addWidget(label_2);

        cb_video_representation = new QComboBox(w_boxes);
        cb_video_representation->setObjectName(QStringLiteral("cb_video_representation"));
        cb_video_representation->setMinimumSize(QSize(0, 0));

        verticalLayout_2->addWidget(cb_video_representation);

        label_audio_adaptationset = new QLabel(w_boxes);
        label_audio_adaptationset->setObjectName(QStringLiteral("label_audio_adaptationset"));

        verticalLayout_2->addWidget(label_audio_adaptationset);

        cb_audio_adaptationset = new QComboBox(w_boxes);
        cb_audio_adaptationset->setObjectName(QStringLiteral("cb_audio_adaptationset"));

        verticalLayout_2->addWidget(cb_audio_adaptationset);

        label_audio_representation = new QLabel(w_boxes);
        label_audio_representation->setObjectName(QStringLiteral("label_audio_representation"));

        verticalLayout_2->addWidget(label_audio_representation);

        cb_audio_representation = new QComboBox(w_boxes);
        cb_audio_representation->setObjectName(QStringLiteral("cb_audio_representation"));

        verticalLayout_2->addWidget(cb_audio_representation);

        label_5 = new QLabel(w_boxes);
        label_5->setObjectName(QStringLiteral("label_5"));

        verticalLayout_2->addWidget(label_5);

        progressBar_V = new QProgressBar(w_boxes);
        progressBar_V->setObjectName(QStringLiteral("progressBar_V"));
        progressBar_V->setValue(24);

        verticalLayout_2->addWidget(progressBar_V);

        label_6 = new QLabel(w_boxes);
        label_6->setObjectName(QStringLiteral("label_6"));

        verticalLayout_2->addWidget(label_6);

        progressBar_VF = new QProgressBar(w_boxes);
        progressBar_VF->setObjectName(QStringLiteral("progressBar_VF"));
        progressBar_VF->setValue(24);

        verticalLayout_2->addWidget(progressBar_VF);

        label_7 = new QLabel(w_boxes);
        label_7->setObjectName(QStringLiteral("label_7"));

        verticalLayout_2->addWidget(label_7);

        progressBar_A = new QProgressBar(w_boxes);
        progressBar_A->setObjectName(QStringLiteral("progressBar_A"));
        progressBar_A->setValue(24);

        verticalLayout_2->addWidget(progressBar_A);

        label_8 = new QLabel(w_boxes);
        label_8->setObjectName(QStringLiteral("label_8"));

        verticalLayout_2->addWidget(label_8);

        progressBar_AC = new QProgressBar(w_boxes);
        progressBar_AC->setObjectName(QStringLiteral("progressBar_AC"));
        progressBar_AC->setValue(24);

        verticalLayout_2->addWidget(progressBar_AC);


        verticalLayout->addWidget(w_boxes);

        textEdit_comment = new QTextEdit(w_controls);
        textEdit_comment->setObjectName(QStringLiteral("textEdit_comment"));

        verticalLayout->addWidget(textEdit_comment);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addWidget(w_controls);

        QtSamplePlayerClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QtSamplePlayerClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtSamplePlayerClass->setStatusBar(statusBar);

        retranslateUi(QtSamplePlayerClass);

        QMetaObject::connectSlotsByName(QtSamplePlayerClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtSamplePlayerClass)
    {
        QtSamplePlayerClass->setWindowTitle(QApplication::translate("QtSamplePlayerClass", "libdash player", 0));
#ifndef QT_NO_ACCESSIBILITY
        QtSamplePlayerClass->setAccessibleDescription(QString());
#endif // QT_NO_ACCESSIBILITY
        button_stop->setText(QApplication::translate("QtSamplePlayerClass", "stop", 0));
        button_start->setText(QApplication::translate("QtSamplePlayerClass", "start", 0));
        button_pause->setText(QApplication::translate("QtSamplePlayerClass", "pause", 0));
        label_videoSegmentIndex->setText(QApplication::translate("QtSamplePlayerClass", "Video Segment Index :", 0));
        label_audioSegmentIndex->setText(QApplication::translate("QtSamplePlayerClass", "Audio Segment Index :", 0));
        label_4->setText(QApplication::translate("QtSamplePlayerClass", "Current profile:", 0));
        label_profile->setText(QApplication::translate("QtSamplePlayerClass", "default", 0));
        label_3->setText(QApplication::translate("QtSamplePlayerClass", "Downloading segment speed:", 0));
        label_segment_speed->setText(QApplication::translate("QtSamplePlayerClass", "0", 0));
        label_mediaName->setText(QApplication::translate("QtSamplePlayerClass", "TextLabel", 0));
        button_comment->setText(QApplication::translate("QtSamplePlayerClass", "Comment", 0));
        label_userName->setText(QApplication::translate("QtSamplePlayerClass", "TextLabel", 0));
        label_period->setText(QApplication::translate("QtSamplePlayerClass", "Period", 0));
        label->setText(QApplication::translate("QtSamplePlayerClass", "Video AdaptationSet", 0));
        label_2->setText(QApplication::translate("QtSamplePlayerClass", "Video Representation", 0));
        label_audio_adaptationset->setText(QApplication::translate("QtSamplePlayerClass", "Audio AdaptationSet", 0));
        label_audio_representation->setText(QApplication::translate("QtSamplePlayerClass", "Audio Representation", 0));
        label_5->setText(QApplication::translate("QtSamplePlayerClass", "Video Segment Buffer", 0));
        label_6->setText(QApplication::translate("QtSamplePlayerClass", "Video Frame Buffer", 0));
        label_7->setText(QApplication::translate("QtSamplePlayerClass", "Audio Segment Buffer", 0));
        label_8->setText(QApplication::translate("QtSamplePlayerClass", "Audio Chunk Buffer", 0));
#ifndef QT_NO_ACCESSIBILITY
        statusBar->setAccessibleDescription(QString());
#endif // QT_NO_ACCESSIBILITY
    } // retranslateUi

};

namespace Ui {
    class QtSamplePlayerClass: public Ui_QtSamplePlayerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTSAMPLEPLAYER_H
