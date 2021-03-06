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
	QString cmmtHtml = this->ui.TextEdit_commentText->toHtml();
	qDebug() << "\t" <<cmmtHtml;

	//replace image
	int indexImgFrom = 0;
	int indexImgEnd = 0;
	int imgnameCharCount = 0;
	QString imgName;
	while (cmmtHtml.indexOf("<img src=", indexImgFrom)>=0 && cmmtHtml.indexOf(" />", indexImgFrom)>=0)
	{
		indexImgFrom = cmmtHtml.indexOf("<img src=", indexImgFrom);
		indexImgEnd = cmmtHtml.indexOf(" />", indexImgFrom);
		imgnameCharCount = indexImgEnd-indexImgFrom-1-8-2;
		imgName = cmmtHtml.mid(indexImgFrom+10, imgnameCharCount);
		cmmtHtml.replace(indexImgFrom, indexImgEnd-indexImgFrom+3, QString("[/")+imgName+QString("]"));
		
		indexImgFrom = indexImgFrom+imgnameCharCount+3;
	}
	qDebug() << "\t" <<cmmtHtml;
	
	//get comment content from paragraphs
	int indexFrom = 0;
	int indexEnd = 0;
	QString comment;
	while(cmmtHtml.indexOf("<p", indexFrom)>=0)
	{
			int indexTemp = cmmtHtml.indexOf(QString("<p"), indexFrom);
			if (cmmtHtml.indexOf(">", indexTemp)>=0 && cmmtHtml.indexOf("</p>", indexTemp)>=0)
			{
				indexFrom = cmmtHtml.indexOf(QString(">"), indexTemp);
				indexEnd = cmmtHtml.indexOf("</p>", indexTemp);	
				QString strTemp = cmmtHtml.mid(indexFrom+1, indexEnd-indexFrom-1);
				comment = comment + strTemp;
				indexFrom = indexEnd+4;
			}
			else
			{
				qCritical() << "\t" << "Error html!";
				return;
			}
	}

	comment = comment.replace("<br />", " ");
	comment = comment.simplified();
	qDebug() << "\t" <<comment;
	if (comment.size()>300)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("评论超过300个字符！"), QMessageBox::Yes);
		return;
	}
	if (comment.size()==0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("评论不能为空！"), QMessageBox::Yes);
		return;
	}

	//add to database
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString currentDateTime = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
	if (!AddCommentToDb(comment, this->mediaID, currentDateTime, userID))
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("插入评论失败！"), QMessageBox::Yes);
		return;
	}else
	{
		QMessageBox::information(this, QString::fromLocal8Bit("评论"),QString::fromLocal8Bit("评论成功！"));
		emit CommentSuccessfully();
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
	QSelectFaceWidget* selectFace = new QSelectFaceWidget(Global::emotionPathTemp, this);
	selectFace->setStyleSheet("QLabel{color: #000000}QPushButton{background-color: transparent}");
	QPoint GlobalPoint(this->ui.button_smilely->mapToGlobal(QPoint(0, 0)));//获取控件在窗体中的坐标
	int y = GlobalPoint.y();
	selectFace->popUp(GlobalPoint, this->ui.button_smilely->height()+10);

	QObject::connect(selectFace, &QSelectFaceWidget::FaceSelected, [=](const QString &filename){
 		QString iconName = GetPicName(filename);
		QTextDocument *document =this->ui.TextEdit_commentText->document();
		QTextCursor cursor = this->ui.TextEdit_commentText->textCursor();
		QFileInfo fiPic(filename);
		if (fiPic.exists())
		{
			QImage image(filename);
			image = image.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			document->addResource(QTextDocument::ImageResource, QUrl(iconName), image);
			cursor.insertImage(iconName);
		}

		this->ui.TextEdit_commentText->setFocus();
		//this->ui.TextEdit_commentText->moveCursor(QTextCursor::NoMove);
	});
	

};
QString CommentDialog::GetPicName(QString original)
{
	int indexOfJpg = original.indexOf(".jpg");
	int indexOfJpeg = original.indexOf(".jpeg");
	int indexOfPng = original.indexOf(".png");
	int indexOfDelimiter = original.lastIndexOf("/");

	QString picName;
	int charCount = 0;

	if (indexOfJpg>=0)
		charCount = indexOfJpg - indexOfDelimiter - 1;
	else if (indexOfJpeg>=0)
		charCount = indexOfJpeg - indexOfDelimiter - 1;
	else if (indexOfPng>=0)
		charCount = indexOfPng - indexOfDelimiter - 1;
	else
		return "";
	
	picName = original.mid(indexOfDelimiter+1, charCount);
	return picName;
}
