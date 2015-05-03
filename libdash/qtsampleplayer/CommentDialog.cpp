#include "CommentDialog.h"

CommentDialog::CommentDialog(QString MI_ID, QString User_ID, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->mediaID = MI_ID;
	this->userID = User_ID;

	QString qss_commentDialog;
	QFile qssFile(":/qss/qss_commentDialog.qss");
	qssFile.open(QFile::ReadOnly);

	if(qssFile.isOpen())
	{
		qss_commentDialog = QLatin1String(qssFile.readAll());
		this->setStyleSheet(qss_commentDialog);
		qssFile.close();
	}
}

CommentDialog::~CommentDialog()
{

}
void CommentDialog::on_button_comment_clicked()
{
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");

	if (!AddCommentToDb(this->ui.TextEdit_commentText->toPlainText(), this->mediaID, current_date, userID))
	{
		QMessageBox::warning(this, QString("Warning"), QString("Add comment failed!"), QMessageBox::Yes);
	}
	this->accept();
}
bool CommentDialog::AddCommentToDb(QString UC_CommentText, QString MI_ID, QString UC_CommentTime, QString User_ID )
{
	QSqlQuery sql_query;

	sql_query.prepare("INSERT INTO usercomment(UC_CommentText, MI_ID, UC_CommentTime, User_ID) VALUES(:commentText,:mediaID,:commentTime,:userID)");

	sql_query.bindValue(":commentText", UC_CommentText);
	sql_query.bindValue(":mediaID", MI_ID);
	sql_query.bindValue(":commentTime", UC_CommentTime);
	sql_query.bindValue(":userID", User_ID);

	if (!sql_query.exec())
		return false;
	else
		return true;
}