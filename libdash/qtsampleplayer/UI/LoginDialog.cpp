#include "LoginDialog.h"
#include <QtWidgets>

LoginDialog::LoginDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.lineEdit_password->setEchoMode(QLineEdit::Password);
	ui.lineEdit_userName->installEventFilter(this);
	ui.lineEdit_password->installEventFilter(this);
	QString qss_logindialog;
	QFile qssFile(":/qss/qss_loginDialog.qss");
	qssFile.open(QFile::ReadOnly);

	if(qssFile.isOpen())
	{
		qss_logindialog = QString::fromLocal8Bit(qssFile.readAll());
		this->setStyleSheet(qss_logindialog);
		qssFile.close();
	}
}

LoginDialog::~LoginDialog()
{

}

void LoginDialog::on_button_ok_clicked()
{
	QString username = this->ui.lineEdit_userName->text().trimmed();
	QString password = this->ui.lineEdit_password->text().trimmed();
	if (username == "" || password == "")
	{
			QMessageBox::warning(this, QString::fromLocal8Bit("登录失败"), QString::fromLocal8Bit("用户名或密码为空!"), QMessageBox::Yes);
			this->ui.lineEdit_userName->setFocus();
	} 
	else
	{
		QString userID = Global::QueryUserFromDb(username, password);
		if (userID!= QString(""))
		{
			emit enterSuccessfully(userID, username);
			QMessageBox::information(this, QString::fromLocal8Bit("欢迎"), QString::fromLocal8Bit("登录成功!"), QMessageBox::Yes); 
			this->accept();
		} 
		else
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("登录失败"), QString::fromLocal8Bit("无效的用户名或密码!"), QMessageBox::Yes);
			this->ui.lineEdit_userName->setFocus();
		}
	}
	
}

void LoginDialog::on_button_cancel_clicked()
{
	this->reject();
}
bool LoginDialog::eventFilter(QObject * object, QEvent * event)
{
	if (event->type() == QEvent::KeyPress )
	{
		QKeyEvent* key_event = static_cast<QKeyEvent*>(event);
		bool username_focus = this->ui.lineEdit_userName->hasFocus();
		bool password_focus = this->ui.lineEdit_password->hasFocus();

		if((object == this->ui.lineEdit_userName && username_focus) || (object == this->ui.lineEdit_password && password_focus))
		{
			if(key_event->key() == Qt::Key_Enter  || key_event->key() == Qt::Key_Return){
				this->ui.button_ok->setFocus();
				this->on_button_ok_clicked();
				return true;
			}	
		}
	}
	return QDialog::eventFilter(object, event);
}

void LoginDialog::on_button_register_clicked()
{
	RegisterDialog* registerDialog = new RegisterDialog(this);
	registerDialog->exec();
}

