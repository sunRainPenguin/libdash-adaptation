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
void CommentDialog::on_button_smilely_clicked()
{
	QString emotionIconPath;
	QSqlQuery sql_query; 
	QSqlRecord rec;

	QString select_sql = "SELECT * FROM smilelyfaces LIMIT 1";
	sql_query.prepare(select_sql);
	if(!sql_query.exec())
	{
		qWarning() << "\t" <<"Select user failed! ";
		return;
	}
	else
	{
		rec = sql_query.record();
		if (sql_query.next())
		{
			int index = rec.indexOf(QString("iconDir"));
			emotionIconPath = sql_query.value(index).toString();
		}
	}

	QSelectFaceWidget* selectFace = new QSelectFaceWidget(emotionIconPath, this);

	QPoint GlobalPoint(this->ui.button_smilely->mapToGlobal(QPoint(0, 0)));//获取控件在窗体中的坐标
	int y = GlobalPoint.y();
	selectFace->popUp(GlobalPoint, this->ui.button_smilely->height()+10);

	QObject::connect(selectFace, &QSelectFaceWidget::FaceSelected, [=](const QString &filename){
		QString iconName = GetPicName(filename);
		if (iconName.size()>0)
		{
			QString temp = "</";
			temp.append(iconName + ">");
			iconName = temp;
		}
		this->ui.TextEdit_commentText->insertPlainText(iconName);
		this->ui.TextEdit_commentText->moveCursor(QTextCursor::End);
		
	});


};
QString CommentDialog::GetPicName(QString original)
{
	int indexOfJpg = original.indexOf(".jpg");
	int indexOfPng = original.indexOf(".png");
	int indexOfDelimiter = original.lastIndexOf("/");

	QString picName;
	int charCount = 0;

	if (indexOfJpg>=0)
		charCount = indexOfJpg - indexOfDelimiter - 1;
	else if (indexOfPng>=0)
		charCount = indexOfPng - indexOfDelimiter - 1;
	else
		return "";
	
	picName = original.mid(indexOfDelimiter+1, charCount);
	return picName;
}