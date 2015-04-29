#ifndef ONDEMANDGUI_H
#define ONDEMANDGUI_H

#include <QWidget>
#include "ui_ondemandgui.h"

#include "sqlconfig.h"
#include "LoginDialog.h"
#include <QMessageBox>
#include <QtGui>
#include <QMap>

class OnDemandGui : public QWidget
{
	Q_OBJECT

public:
	OnDemandGui(QWidget *parent = 0);

	~OnDemandGui();

private:
	Ui::OnDemandGui ui;
	LoginDialog*  loginDialog;

private slots:
	void on_button_login_clicked();
	bool ShowAvailableMediaFromDb();

public slots:
	void SetLoginState(QString username);
	bool SetMediaLayout(QString valMI_ShowPicUrl, QString valMI_Name, QString valMI_UploadAuthor, QString valMI_InsertTime, QString valMI_ClickThroughRate, int row, int column);
};

#endif // ONDEMANDGUI_H
