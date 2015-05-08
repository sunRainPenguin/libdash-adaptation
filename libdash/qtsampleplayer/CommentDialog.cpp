#include "CommentDialog.h"

CommentDialog::CommentDialog(QString MI_ID, QString User_ID, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->mediaID = MI_ID;
	this->userID = User_ID;

	this->ui.TextEdit_commentText->setFocus();

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
	if (this->ui.TextEdit_commentText->toPlainText().size()==0)
	{
		QMessageBox::warning(this, QString("Warning"), QString("Add comment failed, comment can't be empty"), QMessageBox::Yes);
		return;
	}
	if (this->ui.TextEdit_commentText->toPlainText().size()>300)
	{
		QMessageBox::warning(this, QString("Warning"), QString("In the comment 300 characters limited!"), QMessageBox::Yes);
		return;
	}
	QString cmmt = this->ui.TextEdit_commentText->toHtml();

	if (!AddCommentToDb(cmmt, this->mediaID, current_date, userID))
	{
		QMessageBox::warning(this, QString("Warning"), QString("Add comment failed!"), QMessageBox::Yes);
		return;
	}else
	{
		QMessageBox::information(this, QString("Comment"),QString("Comment successfully!"));
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
	QString emotionIconPath = GetEmotionPath();

	QSelectFaceWidget* selectFace = new QSelectFaceWidget(emotionIconPath, this);

	QPoint GlobalPoint(this->ui.button_smilely->mapToGlobal(QPoint(0, 0)));//获取控件在窗体中的坐标
	int y = GlobalPoint.y();
	selectFace->popUp(GlobalPoint, this->ui.button_smilely->height()+10);

	QObject::connect(selectFace, &QSelectFaceWidget::FaceSelected, [=](const QString &filename){
 		QString iconName = GetPicName(filename);
// 		if (iconName.size()>0)
// 		{
// 			QString temp = "[/";
// 			temp.append(iconName + "]");
// 			iconName = temp;
// 		}
		QTextDocument *document =this->ui.TextEdit_commentText->document();
		QTextCursor cursor = this->ui.TextEdit_commentText->textCursor();
		QFileInfo fiPic(filename);
		if (fiPic.exists())
		{
			QImage image(filename);
			image = image.scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			cursor.movePosition(QTextCursor::End);
			document->addResource(QTextDocument::ImageResource, QUrl(iconName), image);
			cursor.insertImage(iconName);
		}

		this->ui.TextEdit_commentText->setFocus();
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
