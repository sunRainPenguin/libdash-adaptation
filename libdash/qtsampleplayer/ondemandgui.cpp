#include "ondemandgui.h"

OnDemandGui::OnDemandGui(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->ShowAvailableMediaFromDb();
}

OnDemandGui::~OnDemandGui()
{

}
void OnDemandGui::on_button_login_clicked()
{
	loginDialog = new LoginDialog();
	QObject::connect(loginDialog, SIGNAL(enterSuccessfully(QString)),this, SLOT(SetLoginState(QString)));
	loginDialog->exec();
}
void OnDemandGui::SetLoginState(QString username)
{
	this->ui.button_login->setText(username);
}

bool OnDemandGui::ShowAvailableMediaFromDb()
{
	 int indexMI_MPDUrl, indexMI_ShowPicUrl, indexMI_Name, indexMI_UploadAuthor, indexMI_InsertTime, indexMI_ClickThroughRate;
	QString valMI_MPDUrl, valMI_ShowPicUrl, valMI_Name, valMI_UploadAuthor, valMI_InsertTime, valMI_ClickThroughRate;

	QSqlQuery sql_query; 
	QString select_sql;
	QSqlRecord rec;

	select_sql = "SELECT * FROM mediainfo";
	sql_query.prepare(select_sql);
	sql_query.exec();
	rec = sql_query.record();
	while (sql_query.next())
	{
		rec = sql_query.record();

		indexMI_MPDUrl = rec.indexOf(QString("MI_MPDUrl"));
		valMI_MPDUrl = sql_query.value(indexMI_MPDUrl).toString();

		indexMI_ShowPicUrl = rec.indexOf(QString("MI_ShowPicUrl"));
		valMI_ShowPicUrl = sql_query.value(indexMI_ShowPicUrl).toString();

		indexMI_Name = rec.indexOf(QString("MI_Name"));
		valMI_Name = sql_query.value(indexMI_Name).toString();

		indexMI_UploadAuthor = rec.indexOf(QString("MI_UploadAuthor"));
		valMI_UploadAuthor = sql_query.value(indexMI_UploadAuthor).toString();

		indexMI_InsertTime = rec.indexOf(QString("MI_InsertTime"));
		valMI_InsertTime = sql_query.value(indexMI_InsertTime).toString();

		indexMI_ClickThroughRate = rec.indexOf(QString("MI_ClickThroughRate"));
		valMI_ClickThroughRate = sql_query.value(indexMI_ClickThroughRate).toString();

		QMessageBox::information(0,"",QString("Succeed Query! MI_MPDUrl:  %1   MI_ShowPicUrl:   %2  MI_Name:  %3   MI_UploadAuthor:  %4  MI_InsertTime:  %5 MI_ClickThroughRate:  %6").arg(valMI_MPDUrl).arg(valMI_ShowPicUrl).arg(valMI_Name).arg(valMI_UploadAuthor).arg(valMI_InsertTime).arg(valMI_ClickThroughRate));
	}

	QImage* img=new QImage;
	if(! ( img->load(valMI_ShowPicUrl) ) ) //¼ÓÔØÍ¼Ïñ
	{
		QMessageBox::information(this,
			QString("Load image failed!"),
			QString("Load image failed!"));
		delete img;
		return false;
	}

    QPainter painter(this);
	QPixmap pixmapToShow = QPixmap::fromImage( img->scaled(size(), Qt::KeepAspectRatio) );
	painter.drawPixmap(0,0, pixmapToShow);

	//QPixmap *pixmap =new QPixmap(200, 150);
	QIcon *icon = new QIcon(pixmapToShow);
	QPushButton *button = new QPushButton(*icon, "", this);
	button->setIconSize(QSize(190, 150));   
	button->setFixedSize(200, 150); 

	QGridLayout *layout=new QGridLayout; 
	layout->addWidget(this->ui.button_login,0,0);
	layout->addWidget(button, 0, 1);
	this->setLayout(layout);
	//this->showNormal();

	//ui->label->setPixmap(QPixmap::fromImage(*img));
	//this->ui.button_mediaurl->setStyleSheet("QPushButton#button_mediaurl{background-image: url(:/QtSamplePlayer/pic/88.jpg);}")
	//return true;
}
