#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QPushButton>
#include <QWidget>

class MyPushButton : public QPushButton
{
	Q_OBJECT

public:
			explicit MyPushButton( QWidget *parent):
			QPushButton(parent)
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
