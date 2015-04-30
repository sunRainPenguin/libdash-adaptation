#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QPushButton>
#include <QWidget>

class MyPushButton : public QPushButton
{
	Q_OBJECT

public:
	explicit MyPushButton(const QIcon &icon, const QString &text,  QWidget *parent):
			QPushButton(icon, text, parent)
	{
		connect(this, SIGNAL(clicked()), this, SLOT(OnClicked()));
	}

signals:
	void ButtonClicked(const QString& msg);

private slots:
	void OnClicked()
	{
		emit this->ButtonClicked(this->accessibleDescription());
	}


private:
	
};

#endif // MYPUSHBUTTON_H
