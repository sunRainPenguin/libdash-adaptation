#include "MyLineEdit.h"

MyLineEdit::MyLineEdit(QWidget  *parent)
	: QLineEdit(parent)
{

}

MyLineEdit::~MyLineEdit()
{

}

void MyLineEdit::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		int length = this->text().length();
		this->setSelection(0,length);
	}
}
