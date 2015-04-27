#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "ui_LoginDialog.h"

class LoginDialog : public QDialog
{
	Q_OBJECT

public:
	LoginDialog(QWidget *parent = 0);
	~LoginDialog();

private:
	Ui::LoginDialog ui;
	bool QueryUserFromDb(QString username, QString password);
	bool QueryUserFromDb(QString username);
	bool AddUserToDb(QString username, QString password);

protected:
	bool  eventFilter(QObject * object, QEvent * event);

private slots:
	void on_button_ok_clicked();
	void on_button_cancel_clicked();
	void on_lineEdit_userName_returnPressed();
	void on_button_register_clicked();

signals:
	void enterSuccessfully(QString username);
};

#endif // LOGINDIALOG_H
