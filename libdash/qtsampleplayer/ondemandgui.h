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

#include "UI/DASHPlayer.h"
using namespace sampleplayer;

class OnDemandGui : public QWidget
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
	QVBoxLayout* verticalLayout;
	QMultiHash<QString, QString> mediaInfo;
	bool hasLogedIn;
	QString userName;

	bool SetMediaLayout(QString MI_ID, QString MI_MPDUrl, QString MI_ShowPicUrl, QString MI_Name, QString MI_UploadAuthor, QString MI_InsertTime, QString MI_ClickThroughRate, int row, int column);
	bool ShowAvailableMediaFromDb();
	QPushButton* FindButtonByNameIndex(int number);

private slots:
	void on_button_login_clicked();

public slots:
	void	SetLoginState		(QString usesrName);
	void StartPlayer						(QString currMediaID);
	void on_playgui_closed				();
	void LoginBeforeComment		();

signals:
	void enterSuccessfully(QString userName);
};

#endif // ONDEMANDGUI_H
