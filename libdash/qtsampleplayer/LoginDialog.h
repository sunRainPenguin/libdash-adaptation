#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "ui_LoginDialog.h"
#include "RegisterDialog.h"

class LoginDialog : public QDialog
{
	Q_OBJECT

public:
	LoginDialog(QWidget *parent = 0);
	~LoginDialog();

private:
	Ui::LoginDialog ui;

private slots:
	 bool  eventFilter(QObject * object, QEvent * event);
	void on_button_ok_clicked();
	void on_button_cancel_clicked();
	void on_button_register_clicked();

signals:
	void enterSuccessfully(QString userID, QString userName);
};

#endif // LOGINDIALOG_H
