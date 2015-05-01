#ifndef ONDEMANDGUI_H
#define ONDEMANDGUI_H

#include <QWidget>
#include "ui_ondemandgui.h"
#include "sqlconfig.h"
#include "LoginDialog.h"
#include "MyPushButton.h"
#include <QMessageBox>
#include <QtGui>
#include <QMap>
#include <QVector>

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

	bool SetMediaLayout(QString valMI_MPDUrl, QString valMI_ShowPicUrl, QString valMI_Name, QString valMI_UploadAuthor, QString valMI_InsertTime, QString valMI_ClickThroughRate, int row, int column);
	bool ShowAvailableMediaFromDb();
	QPushButton* FindButtonByNameIndex(int number);

private slots:
	void on_button_login_clicked();

public slots:
	void SetLoginState(QString username);
	void StartPlayer(QString currMpdUrl);
	void on_playgui_closed();

};

#endif // ONDEMANDGUI_H
