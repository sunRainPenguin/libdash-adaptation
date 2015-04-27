#include "LoginDialog.h"
#include <QMessageBox>
#include <QSqlQuery>
#include  <QSqlError>
#include <QtWidgets>

LoginDialog::LoginDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

}

LoginDialog::~LoginDialog()
{

}
void LoginDialog::on_button_ok_clicked()
{
	QString username = this->ui.lineEdit_userName->text();
	QString password = this->ui.lineEdit_password->text();

	if (username == "")
	{
		QMessageBox::warning(this, QString("Warning"), QString("Please enter username!"), QMessageBox::Yes);
		ui.lineEdit_userName->setFocus();
	}
	if (password == "")
	{
		QMessageBox::warning(this, QString("Warning"), QString("Please enter password!"), QMessageBox::Yes);
		ui.lineEdit_password->setFocus();
	}

	if (this->QueryUserFromDb(username, password))
	{
		QMessageBox::information(this, QString("Welcome"), QString("Log in successfully!"), QMessageBox::Yes); 
		emit enterSuccessfully(username);
		this->close();
	} 
	else
	{
		QMessageBox::warning(this, QString("Warning"), QString("Invalid username or password!"), QMessageBox::Yes);
		ui.lineEdit_userName->setFocus();
	}

}
bool LoginDialog::QueryUserFromDb(QString username, QString password)
{
		QString select_sql = "SELECT * FROM clientuser WHERE CU_Name = '" + username + "' AND CU_Password = '" + password + "'";
		QSqlQuery sql_query; 
		sql_query.prepare(select_sql);
		if(!sql_query.exec())
		{
			return false;
		}
		else
		{
			if (sql_query.next())
			{
				return true;
			}
			return false;
		}
}
void LoginDialog::on_button_cancel_clicked()
{
	this->close();
}
void LoginDialog:: on_lineEdit_userName_returnPressed()
{
	this->close();
}
bool LoginDialog::eventFilter(QObject * object, QEvent * event)
{
	if (event->type() == QEvent::KeyPress )
	{
		QKeyEvent* key_event = static_cast<QKeyEvent*>(event);
		bool username_focus = this->ui.lineEdit_userName->hasFocus();
		bool password_focus = this->ui.lineEdit_password->hasFocus();
		if (username_focus)
		{
			int length = this->ui.lineEdit_userName->text().length();
			this->ui.lineEdit_userName->setSelection(0,length);
		}
		if (password_focus)
		{
			int length = this->ui.lineEdit_password->text().length();
			this->ui.lineEdit_password->setSelection(0,length);
		}
		return true;
	}
	return QDialog::eventFilter(object, event);
}
void LoginDialog::on_button_register_clicked()
{
	QString username = this->ui.lineEdit_userName->text();
	QString password = this->ui.lineEdit_password->text();

	if (username == "")
	{
		QMessageBox::warning(this, QString("Warning"), QString("Please enter username!"), QMessageBox::Yes);
		ui.lineEdit_userName->setFocus();
	}
	if (password == "")
	{
		QMessageBox::warning(this, QString("Warning"), QString("Please enter password!"), QMessageBox::Yes);
		ui.lineEdit_password->setFocus();
	}

	if (this->QueryUserFromDb(username))
	{
		QMessageBox::warning(this, QString("Warning"), QString("This username already exists!"), QMessageBox::Yes);
		ui.lineEdit_userName->setFocus();
	}else
	{
		if (this->AddUserToDb(username, password))
		{
			QMessageBox::information(this, QString("Register"),QString("Register successfully, Log in again please!"));
			ui.lineEdit_userName->setFocus();
		} 
		else
		{
			QMessageBox::warning(this, QString("Warning"), QString("Register failed, please check it again!"));
		}
		
	}

}

bool LoginDialog::QueryUserFromDb(QString username)
{
	QString select_sql = "SELECT * FROM clientuser WHERE CU_Name = '" + username + "'";
	QSqlQuery sql_query; 
	sql_query.prepare(select_sql);
	if(!sql_query.exec())
	{
		return false;
	}
	else
	{
		if (sql_query.next())
		{
			return true;
		}
		return false;
	}
}

bool LoginDialog::AddUserToDb(QString username, QString password)
{
	QSqlQuery sql_query;

	int userCount = 0;
	QString select_all_sql = "SELECT COUNT(*) FROM clientuser ";
	sql_query.prepare(select_all_sql);
	if(!sql_query.exec())
	{
		return false;
	}
	else
	{
		if (sql_query.next())
		{
			userCount = sql_query.value(0).toInt();
		}
	}

	
	sql_query.prepare("INSERT INTO clientuser(CU_ID, CU_Name, CU_Password) VALUES(:CU_ID,:CU_Name,:CU_Password)");

	sql_query.bindValue(":CU_ID",userCount+1);
	sql_query.bindValue(":CU_Name", username);
	sql_query.bindValue(":CU_Password", password);

	if (!sql_query.exec())
		return false;
	else
		return true;
}