#include "configwindow.h"
#include "ui_configwindow.h"
#include "ts3_functions.h"

static struct TS3Functions ts3Funcs;

extern char* pluginID;

ConfigWindow::ConfigWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigWindow)
{
    ui->setupUi(this);
}

ConfigWindow::~ConfigWindow()
{
    delete ui;
}

void ConfigWindow::on_pushButton_clicked()
{
    this->close();
}

void ConfigWindow::on_pushButton_2_clicked()
{
	ts3Funcs.requestHotkeyInputDialog(pluginID, "FSX_COMM2_PTT_PRESS", 0, NULL);
//	this->close();
}
