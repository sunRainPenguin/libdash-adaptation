#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QLineEdit>
#include <QLineEdit>
#include <QMouseEvent>

class MyLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	explicit MyLineEdit(QWidget * parent);
	~MyLineEdit();

protected:
	//ÖØÐ´mousePressEventÊÂ¼þ
	virtual void mousePressEvent(QMouseEvent *event);

private:
	
};

#endif // MYLINEEDIT_H
