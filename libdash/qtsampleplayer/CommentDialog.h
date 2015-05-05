#ifndef COMMENTDIALOG_H
#define COMMENTDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "ui_CommentDialog.h"
#include "sqlconfig.h"
#include "QSelectFaceWidget.h"
#include <QDateTime>
#include <QTextCodec>

class CommentDialog : public QDialog
{
	Q_OBJECT

public:
	CommentDialog(QString MI_ID, QString User_ID, QWidget *parent = 0);
	~CommentDialog();

private slots:
	void on_button_comment_clicked();
	void on_button_smilely_clicked();

private:
	Ui::CommentDialog ui;
	QString mediaID;
	QString userID;
	bool AddCommentToDb(QString UC_CommentText, QString MI_ID, QString UC_CommentTime, QString User_ID );
	QString GetPicName(QString original);
};

#endif // COMMENTDIALOG_H
