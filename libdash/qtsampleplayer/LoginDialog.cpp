#include "LoginDialog.h"
#include <QMessageBox>
#include <QtWidgets>

LoginDialog::LoginDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.lineEdit_password->setEchoMode(QLineEdit::Password);
	ui.lineEdit_userName->installEventFilter(this);
	ui.lineEdit_password->installEventFilter(this);
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
			QMessageBox::warning(this, QString("Warning"), QString("Empty username or password!"), QMessageBox::Yes);
			this->ui.lineEdit_userName->setFocus();
	} 
	else
	{
		QString userID = this->QueryUserFromDb(username, password);
		if (userID!= QString(""))
		{
			QMessageBox::information(this, QString("Welcome"), QString("Log in successfully!"), QMessageBox::Yes); 
			emit enterSuccessfully(userID, username);
			this->accept();
		} 
		else
		{
			QMessageBox::warning(this, QString("Warning"), QString("Invalid username or password!"), QMessageBox::Yes);
			this->ui.lineEdit_userName->setFocus();
		}
	}
	
}
QString LoginDialog::QueryUserFromDb(QString username, QString password)
{
		int indexUserID;
		QString userID;

		QSqlQuery sql_query; 
		QSqlRecord rec;

		QString select_sql = "SELECT * FROM user WHERE BINARY username = '" + username + "' AND BINARY password = '" + password + "'";
		sql_query.prepare(select_sql);
		if(!sql_query.exec())
		{
			qWarning() << "\t" <<"Select user failed! ";
			return QString("");
		}
		else
		{
			rec = sql_query.record();
			if (sql_query.next())
			{
				indexUserID = rec.indexOf(QString("ID"));
				userID = sql_query.value(indexUserID).toString();
				return userID;
			}
				return QString("");
		}
}
void LoginDialog::on_button_cancel_clicked()
{
	this->reject();
}
//void LoginDialog:: on_lineEdit_userName_returnPressed()
//{
//	this->reject();
//}
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
	QString username = this->ui.lineEdit_userName->text();
	QString password = this->ui.lineEdit_password->text();

	if (username == "" || password == "")
	{
		QMessageBox::warning(this, QString("Warning"), QString("Empty username or password!"), QMessageBox::Yes);
		this->ui.lineEdit_userName->setFocus();
	}
	
	if (this->QueryUserFromDb(username))
	{
		QMessageBox::warning(this, QString("Warning"), QString("This username already exists!"), QMessageBox::Yes);
		this->ui.lineEdit_userName->setFocus();
	}else
	{
		if (this->AddUserToDb(username, password))
		{
			QMessageBox::information(this, QString("Register"),QString("Register successfully, Log in again please!"));
			this->ui.lineEdit_userName->setFocus();
		} 
		else
		{
			QMessageBox::warning(this, QString("Warning"), QString("Register failed, please check it again!"));
		}	
	}

}

bool LoginDialog::QueryUserFromDb(QString username)
{
	QString select_sql = "SELECT * FROM user WHERE BINARY username = '" + username + "'";
	QSqlQuery sql_query; 
	sql_query.prepare(select_sql);
	if(!sql_query.exec() || sql_query.size()<1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool LoginDialog::AddUserToDb(QString username, QString password)
{
	QSqlQuery sql_query;

	sql_query.prepare("INSERT INTO user(username, password, flag) VALUES(:username,:password,:flag)");

	sql_query.bindValue(":username", username);
	sql_query.bindValue(":password", password);
	sql_query.bindValue(":flag", 1);

	if (!sql_query.exec())
		return false;
	else
		return true;
}