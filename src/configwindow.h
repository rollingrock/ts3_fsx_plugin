#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QDialog>
#include "ts3_functions.h"

namespace Ui {
class ConfigWindow;
}

class ConfigWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigWindow(QWidget *parent = 0);
    ~ConfigWindow();
	void set_ts3func(TS3Functions *ptr);
	void set_pluginID(char *ptr);
	void set_ptt_label(const char* keyword);
	void set_all_ptt_labels();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

	void on_comm1_keyup_button_clicked();
    
    void on_comm1_keydown_button_clicked();
    
    void on_comm2_keyup_button_clicked();
    
    void on_comm2_keydown_button_clicked();
    
private:
    Ui::ConfigWindow *ui;
	TS3Functions *ts3funcs;
	char* pluginID;
	void check_ptt_labels();
};

#endif // CONFIGWINDOW_H
