#include "QSelectFaceWidget.h"

QSelectFaceWidget::QSelectFaceWidget(const QString &faceIconDir,  QWidget *parent)
	: QWidget(parent)
{
	this->setWindowFlags(Qt::Popup);
	QFileInfoList InfoList = QDir(faceIconDir).entryInfoList();
	QFileInfo fileInfo;
	QImage* img=new QImage;

	//gridLayout
	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setMargin(2);
	gridLayout->setRowStretch(0, 1);
	gridLayout->setRowStretch(1, 1);
	gridLayout->setRowStretch(2, 1);
	gridLayout->setColumnStretch(0, 1);
	gridLayout->setColumnStretch(1, 1);
	gridLayout->setColumnStretch(2, 1);

	//get picures
	int picCount = 0;
	foreach(fileInfo, InfoList)
	{
		if(!fileInfo.isFile()) continue;

		if(0==fileInfo.suffix().compare("jpg") || 0==fileInfo.suffix().compare("png"))
		{
			qDebug() << fileInfo.filePath();
			if(! ( img->load(fileInfo.filePath()) ) ) 
			{
				qDebug() << "\t" <<"Load image failed ! ";
				delete(img);
				img = NULL;
				return ;
			}
			QPainter painter(this);
			QPixmap pixmapToShow = QPixmap::fromImage( img->scaled(size(), Qt::KeepAspectRatio) );
			painter.drawPixmap(0,0, pixmapToShow);
			QIcon *icon = new QIcon(pixmapToShow);

			MyPushButton* imgButton = new MyPushButton(this);
			imgButton->setIcon(*icon);
			imgButton->setText("");
			imgButton->setIconSize(QSize(50, 60));   
			imgButton->setFixedSize(60, 60); 
			imgButton->setAccessibleDescription(fileInfo.filePath());

			connect(imgButton, SIGNAL(ButtonClicked(QString)), this, SLOT(on_face_selected(QString)));
			gridLayout->addWidget(imgButton, picCount/3, picCount%3);

			picCount++;
		}
	}
			qDebug() << "\t" << "Get faces done!";

}

QSelectFaceWidget::~QSelectFaceWidget()
{

}
void QSelectFaceWidget::popUp(QPoint &pos, int offsetY)
{
	pos.setY(pos.y() + offsetY);
	this->move(pos);
	this->show();
}
void QSelectFaceWidget::on_face_selected(QString IconDir)
{
	emit FaceSelected(IconDir);
	this->close();
}