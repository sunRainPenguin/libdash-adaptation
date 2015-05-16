#ifndef ONDEMANDGUI_H
#define ONDEMANDGUI_H

#include <QWidget>
#include "ui_ondemandgui.h"
#include "sqlconfig.h"
#include "LoginDialog.h"
#include "MyPushButton.h"
#include <QMessageBox>
#include <QtGui>
#include <QMultiHash>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include "UI/DASHPlayer.h"
using namespace sampleplayer;

#define  subjectType "Subject"
#define  gradeType   "Grade"
enum LabelType{
	mediaName,
	mediaAuthor,
	mediaTime,
	clickThroughRate
};

class OnDemandGui : public QMainWindow
{
	Q_OBJECT

public:
	OnDemandGui(QWidget *parent = 0);

	~OnDemandGui();

private:
	Ui::OnDemandGui ui;
	LoginDialog*  loginDialog;
	QtSamplePlayerGui* playerGui;
	DASHPlayer* player;
	QStandardItemModel *treeModel;
	QString   currentSearchKey;
	QMultiHash<QString, QString> mediaInfo;
	bool hasLogedIn;
	QString userName;
	QString userID;
	bool SetMediaLayout(QString MI_ID, QString MI_MPDUrl, QString MI_ShowPicUrl, QString MI_Name, QString MI_UploadAuthor, QString MI_InsertTime, QString MI_ClickThroughRate, int row, int column);
	bool ShowAvailableMediaFromDb		(QString SearchKey, QString subject="", QString grade="");
	QPushButton* FindButtonByNameIndex		(int number);
	QLabel* FindLabelByNameIndex		(int type,  int number);
	void UpdateClickThroughRateToDb		(QString mediaID);
	bool StartUpdateOndemandGui		();
	void ShowTreeView		();
	bool LoadTreeViewData	(QString type, QStandardItemModel * treeModel);

private slots:
	void on_button_login_clicked		();
	void on_button_logout_clicked		();
	void on_button_search_clicked		();
	void on_treeView_clicked				(QModelIndex modelIndex);
	void replyFinished							(QNetworkReply *reply);
	bool eventFilter								(QObject * object, QEvent * event);

public slots:
	void	SetLoginState		(QString userID, QString usesrName);
	void StartPlayer						(QString currMediaID);
	void on_playgui_closed				();
	void LoginBeforeComment		();

signals:
	void enterSuccessfully(QString userID, QString userName);
	void LogOut();
};

#endif // ONDEMANDGUI_H
