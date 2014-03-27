#include "configwindow.h"
#include "ui_configwindow.h"

ConfigWindow::ConfigWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigWindow)
{
    ui->setupUi(this);
	ui->label_3->setText("");
	ui->label_4->setText("");

}

ConfigWindow::~ConfigWindow()
{
    delete ui;
}

void ConfigWindow::on_pushButton_clicked()
{
    this->close();
}

void ConfigWindow::set_ts3func(TS3Functions *ptr)
{
	this->ts3funcs = ptr;
}

void ConfigWindow::set_pluginID(char *ptr)
{
	this->pluginID = ptr;
}

void ConfigWindow::on_pushButton_2_clicked()
{
    ui->comm1_keydown_label->setStyleSheet("background-color : red;");
}

void ConfigWindow::on_comm1_keyup_button_clicked()
{
	this->ts3funcs->requestHotkeyInputDialog(this->pluginID, "FSX_COMM1_PTT_RELEASE", 0, this);
}

void ConfigWindow::on_comm1_keydown_button_clicked()
{
	this->ts3funcs->requestHotkeyInputDialog(this->pluginID, "FSX_COMM1_PTT_PRESS", 1, this); 
}

void ConfigWindow::on_comm2_keyup_button_clicked()
{
	this->ts3funcs->requestHotkeyInputDialog(this->pluginID, "FSX_COMM2_PTT_RELEASE", 0, this);    
}

void ConfigWindow::on_comm2_keydown_button_clicked()
{
	this->ts3funcs->requestHotkeyInputDialog(this->pluginID, "FSX_COMM2_PTT_PRESS", 1, this);  
}

void ConfigWindow::set_ptt_label(const char* keyword)
{
	size_t bufSize = 40;
	size_t arrLen = 1;
	unsigned int result = 0;
	const char *keyword_arr[1] = {keyword};
	char **hotkey_buf;

	hotkey_buf = new char *[1];

	hotkey_buf[0] = new char[40];

	result = this->ts3funcs->getHotkeyFromKeyword(this->pluginID, keyword_arr, hotkey_buf, arrLen, bufSize);

	if (hotkey_buf[0][0] == NULL)
	{
		if (!strcmp(keyword, "FSX_COMM1_PTT_RELEASE")) {
			ui->comm1_keyup_label->setStyleSheet("background-color : red; font-weight : bold; padding-left : 3px;");
			ui->comm1_keyup_label->setText("Key Not Set");
		}
		else if (!strcmp(keyword, "FSX_COMM1_PTT_PRESS")) {
			ui->comm1_keydown_label->setStyleSheet("background-color : red; font-weight : bold; padding-left : 3px;");
			ui->comm1_keydown_label->setText("Key Not Set");
		}
		else if (!strcmp(keyword, "FSX_COMM2_PTT_RELEASE")) {
			ui->comm2_keyup_label->setStyleSheet("background-color : red; font-weight : bold; padding-left : 3px;");
			ui->comm2_keyup_label->setText("Key Not Set");
		}
		else if (!strcmp(keyword, "FSX_COMM2_PTT_PRESS")) {
			ui->comm2_keydown_label->setStyleSheet("background-color : red; font-weight : bold; padding-left : 3px;");
			ui->comm2_keydown_label->setText("Key Not Set");
		}
	}
	else
	{
		if (!strcmp(keyword, "FSX_COMM1_PTT_RELEASE")) {
			ui->comm1_keyup_label->setStyleSheet("background-color : transparent; font-weight : bold; padding-left : 3px;");
			ui->comm1_keyup_label->setText(hotkey_buf[0]);
		}
		else if (!strcmp(keyword, "FSX_COMM1_PTT_PRESS")) {
			ui->comm1_keydown_label->setStyleSheet("background-color : transparent; font-weight : bold; padding-left : 3px;");
			ui->comm1_keydown_label->setText(hotkey_buf[0]);
		}
		else if (!strcmp(keyword, "FSX_COMM2_PTT_RELEASE")) {
			ui->comm2_keyup_label->setStyleSheet("background-color : transparent; font-weight : bold; padding-left : 3px;");
			ui->comm2_keyup_label->setText(hotkey_buf[0]);
		}
		else if (!strcmp(keyword, "FSX_COMM2_PTT_PRESS")) {
			ui->comm2_keydown_label->setStyleSheet("background-color : transparent; font-weight : bold; padding-left : 3px;");
			ui->comm2_keydown_label->setText(hotkey_buf[0]);
		}
	}

	this->check_ptt_labels();

	delete [] *hotkey_buf;
	delete [] hotkey_buf;
}

void ConfigWindow::set_all_ptt_labels()
{
	this->set_ptt_label("FSX_COMM1_PTT_RELEASE");
	this->set_ptt_label("FSX_COMM1_PTT_PRESS");
	this->set_ptt_label("FSX_COMM2_PTT_RELEASE");
	this->set_ptt_label("FSX_COMM2_PTT_PRESS");
}

void ConfigWindow::check_ptt_labels()
{
	if (!QString::compare(ui->comm1_keydown_label->text(), ui->comm1_keyup_label->text()))
	{
		ui->label_3->setText("");
	}
	else
	{
		ui->label_3->setText("KEYUP/KEYDOWN KEYS DO NOT MATCH!!!");
	}

	if (!QString::compare(ui->comm2_keydown_label->text(), ui->comm2_keyup_label->text()))
	{
		ui->label_4->setText("");
	}
	else
	{
		ui->label_4->setText("KEYUP/KEYDOWN KEYS DO NOT MATCH!!!");
	}
}