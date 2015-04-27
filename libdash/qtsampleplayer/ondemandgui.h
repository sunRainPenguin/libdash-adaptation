#ifndef ONDEMANDGUI_H
#define ONDEMANDGUI_H

#include <QWidget>
#include "ui_ondemandgui.h"
#include "LoginDialog.h"

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

public slots:
	void SetLoginState(QString username);

};

#endif // ONDEMANDGUI_H
