#include "RegisterDialog.h"

RegisterDialog::RegisterDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->ui.lineEdit_password->setEchoMode(QLineEdit::Password);
	this->ui.lineEdit_password_reenter->setEchoMode(QLineEdit::Password);
}

RegisterDialog::~RegisterDialog()
{

}
void RegisterDialog::on_button_register_cpt_clicked()
{
	QString username = this->ui.lineEdit_userName->text();
	QString password = this->ui.lineEdit_password->text();
	QString reEnterPassword = this->ui.lineEdit_password_reenter->text();
	const QPixmap* headPic = this->ui.label_picture->pixmap();

	//Error detecting
	if (username == "" || password == "" || reEnterPassword=="")
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�û���������Ϊ��!"), QMessageBox::Yes);
		this->ui.lineEdit_userName->setFocus();
		return;
	}
	if (headPic->isNull())
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�û�ͷ����Ϊ��!"), QMessageBox::Yes);
		this->ui.button_picture->setFocus();
		return;
	}
	if (reEnterPassword!=password)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�����������벻һ�£�������!"), QMessageBox::Yes);
		this->ui.lineEdit_password->setText("");
		this->ui.lineEdit_password->setFocus();
		this->ui.lineEdit_password_reenter->setText("");
		return;
	}

	if (Global::QueryUserFromDb(username))
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("���û����Ѵ���!"), QMessageBox::Yes);
		this->ui.lineEdit_userName->setFocus();
		return;
	}else
	{
		//Add user to Db and send picture to server
		if (Global::AddUserToDb(username, password))
		{
			QMessageBox::information(this, QString::fromLocal8Bit("ע��ɹ�"),QString::fromLocal8Bit("ע��ɹ��������µ�¼!"));
		} 
		else
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("ע��ʧ��"), QString::fromLocal8Bit("ע��ʧ�ܣ�����������!"));
			return;
		}	

		QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

		QHttpPart imagePart;
		imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
		QString header = QString("form-data; name=\"image\"; filename=\""+ username + ".jpg\"");
		imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(header));
		QFile *file = new QFile(Global::avatarIconTemp);
		file->open(QIODevice::ReadOnly);
		imagePart.setBodyDevice(file);
		file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart

		multiPart->append(imagePart);

		QNetworkAccessManager *uploadManager = new QNetworkAccessManager(this);
		QNetworkReply *reply;
		QObject::connect(uploadManager,SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
		QNetworkRequest request(Global::uploadPicServlet); 
		reply = uploadManager->post(request, multiPart);
		multiPart->setParent(reply); // delete the multiPart with the reply

		this->accept();
	}
}

void RegisterDialog::on_button_picture_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"", tr("Images (*.png *.jpeg *.jpg)"));
	qDebug() << "Picture name:  " <<fileName;
	QPixmap pixmap(fileName);
	pixmap = pixmap.scaled(this->ui.label_picture->size(), Qt::KeepAspectRatio, Qt::FastTransformation);
	this->ui.label_picture->setPixmap(pixmap);
	pixmap.save(Global::avatarIconTemp, "JPG"); 
	this->ui.lineEdit_picture->setText(fileName);
}
void RegisterDialog::replyFinished(QNetworkReply* reply)
{
	int result = reply->error();
	if(reply->error() == QNetworkReply::NoError)
	{
		qDebug() << "\t" <<"upload no error!";
	}
}
