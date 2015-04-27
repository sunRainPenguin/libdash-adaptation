#include "ondemandgui.h"


OnDemandGui::OnDemandGui(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

OnDemandGui::~OnDemandGui()
{

}
void OnDemandGui::on_button_login_clicked()
{
	loginDialog = new LoginDialog();
	QObject::connect(loginDialog, SIGNAL(enterSuccessfully(QString)),this, SLOT(SetLoginState(QString)));
	loginDialog->show();
}
void OnDemandGui::SetLoginState(QString username)
{
	ui.button_login->setText(username);
}

