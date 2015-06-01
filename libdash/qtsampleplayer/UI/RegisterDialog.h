#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include "./Helpers/Global.h"
#include "ui_RegisterDialog.h"

class RegisterDialog : public QDialog
{
	Q_OBJECT

public:
	RegisterDialog(QWidget *parent = 0);
	~RegisterDialog();

private:
	Ui::RegisterDialog ui;

private slots:
	void on_button_register_cpt_clicked();
	void on_button_picture_clicked();
	void replyFinished(QNetworkReply* reply);
};

#endif // REGISTERDIALOG_H
