/********************************************************************************
** Form generated from reading UI file 'configwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGWINDOW_H
#define UI_CONFIGWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_ConfigWindow
{
public:
    QPushButton *pushButton;
    QLabel *label;
    QFrame *line;
    QPushButton *comm1_keyup_button;
    QLabel *comm1_keyup_label;
    QGroupBox *groupBox;
    QLabel *comm1_keydown_label;
    QLabel *label_3;
    QPushButton *comm1_keydown_button;
    QGroupBox *groupBox_2;
    QPushButton *comm2_keyup_button;
    QPushButton *comm2_keydown_button;
    QLabel *comm2_keyup_label;
    QLabel *comm2_keydown_label;
    QLabel *label_4;
    QLabel *label_2;
    QLabel *label_9;
    QLabel *label_7;
    QCheckBox *checkBox;
    QLabel *label_8;
    QLabel *label_6;

    void setupUi(QDialog *ConfigWindow)
    {
        if (ConfigWindow->objectName().isEmpty())
            ConfigWindow->setObjectName(QStringLiteral("ConfigWindow"));
        ConfigWindow->resize(491, 428);
        pushButton = new QPushButton(ConfigWindow);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(180, 360, 131, 23));
        label = new QLabel(ConfigWindow);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(90, 0, 331, 21));
        QFont font;
        font.setFamily(QStringLiteral("MS UI Gothic"));
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        line = new QFrame(ConfigWindow);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(10, 20, 471, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        comm1_keyup_button = new QPushButton(ConfigWindow);
        comm1_keyup_button->setObjectName(QStringLiteral("comm1_keyup_button"));
        comm1_keyup_button->setGeometry(QRect(20, 110, 111, 23));
        comm1_keyup_label = new QLabel(ConfigWindow);
        comm1_keyup_label->setObjectName(QStringLiteral("comm1_keyup_label"));
        comm1_keyup_label->setGeometry(QRect(140, 110, 91, 21));
        QFont font1;
        font1.setFamily(QStringLiteral("MS Sans Serif"));
        font1.setPointSize(10);
        comm1_keyup_label->setFont(font1);
        groupBox = new QGroupBox(ConfigWindow);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 80, 231, 121));
        comm1_keydown_label = new QLabel(groupBox);
        comm1_keydown_label->setObjectName(QStringLiteral("comm1_keydown_label"));
        comm1_keydown_label->setGeometry(QRect(130, 80, 91, 21));
        comm1_keydown_label->setFont(font1);
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 55, 201, 21));
        QFont font2;
        font2.setUnderline(true);
        label_3->setFont(font2);
        comm1_keydown_button = new QPushButton(groupBox);
        comm1_keydown_button->setObjectName(QStringLiteral("comm1_keydown_button"));
        comm1_keydown_button->setGeometry(QRect(10, 80, 111, 23));
        comm1_keydown_label->raise();
        comm1_keydown_button->raise();
        label_3->raise();
        comm1_keydown_button->raise();
        groupBox_2 = new QGroupBox(ConfigWindow);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(250, 80, 231, 121));
        comm2_keyup_button = new QPushButton(groupBox_2);
        comm2_keyup_button->setObjectName(QStringLiteral("comm2_keyup_button"));
        comm2_keyup_button->setGeometry(QRect(10, 30, 111, 23));
        comm2_keydown_button = new QPushButton(groupBox_2);
        comm2_keydown_button->setObjectName(QStringLiteral("comm2_keydown_button"));
        comm2_keydown_button->setGeometry(QRect(10, 80, 111, 23));
        comm2_keyup_label = new QLabel(groupBox_2);
        comm2_keyup_label->setObjectName(QStringLiteral("comm2_keyup_label"));
        comm2_keyup_label->setGeometry(QRect(130, 30, 91, 21));
        comm2_keyup_label->setFont(font1);
        comm2_keydown_label = new QLabel(groupBox_2);
        comm2_keydown_label->setObjectName(QStringLiteral("comm2_keydown_label"));
        comm2_keydown_label->setGeometry(QRect(130, 80, 91, 21));
        comm2_keydown_label->setFont(font1);
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 55, 201, 21));
        label_4->setFont(font2);
        label_2 = new QLabel(ConfigWindow);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 30, 461, 31));
        label_2->setWordWrap(true);
        label_9 = new QLabel(ConfigWindow);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(70, 400, 371, 21));
        label_7 = new QLabel(ConfigWindow);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(20, 280, 271, 16));
        label_7->setStyleSheet(QStringLiteral(""));
        checkBox = new QCheckBox(ConfigWindow);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(20, 230, 91, 21));
        checkBox->setStyleSheet(QStringLiteral(""));
        checkBox->setChecked(true);
        label_8 = new QLabel(ConfigWindow);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(20, 310, 441, 31));
        label_8->setStyleSheet(QStringLiteral("font-weight : bold; color : red; font-size : 12pt;"));
        label_6 = new QLabel(ConfigWindow);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(20, 200, 311, 31));
        QFont font3;
        font3.setPointSize(12);
        label_6->setFont(font3);
        label_6->setStyleSheet(QStringLiteral(""));
        groupBox_2->raise();
        groupBox->raise();
        pushButton->raise();
        label->raise();
        line->raise();
        comm1_keyup_button->raise();
        comm1_keyup_label->raise();
        label_2->raise();
        label_9->raise();
        label_7->raise();
        checkBox->raise();
        label_8->raise();
        label_6->raise();

        retranslateUi(ConfigWindow);

        QMetaObject::connectSlotsByName(ConfigWindow);
    } // setupUi

    void retranslateUi(QDialog *ConfigWindow)
    {
        ConfigWindow->setWindowTitle(QApplication::translate("ConfigWindow", "ConfigWindow", 0));
        pushButton->setText(QApplication::translate("ConfigWindow", "Close", 0));
        label->setText(QApplication::translate("ConfigWindow", "FSX Radio Plugin for Teamspeak 3", 0));
        comm1_keyup_button->setText(QApplication::translate("ConfigWindow", "COMM1 Key UP", 0));
        comm1_keyup_label->setText(QApplication::translate("ConfigWindow", "Key Not Set", 0));
        groupBox->setTitle(QApplication::translate("ConfigWindow", "COMM1 PTT Setup", 0));
        comm1_keydown_label->setText(QApplication::translate("ConfigWindow", "Key Not Set", 0));
        label_3->setText(QApplication::translate("ConfigWindow", "KEYUP/KEYDOWN KEYS DO NOT MATCH!!!", 0));
        comm1_keydown_button->setText(QApplication::translate("ConfigWindow", "COMM1 Key DOWN", 0));
        groupBox_2->setTitle(QApplication::translate("ConfigWindow", "COMM2 PTT Setup", 0));
        comm2_keyup_button->setText(QApplication::translate("ConfigWindow", "COMM2 Key UP", 0));
        comm2_keydown_button->setText(QApplication::translate("ConfigWindow", "COMM2 Key DOWN", 0));
        comm2_keyup_label->setText(QApplication::translate("ConfigWindow", "Key Not Set", 0));
        comm2_keydown_label->setText(QApplication::translate("ConfigWindow", "Key Not Set", 0));
        label_4->setText(QApplication::translate("ConfigWindow", "KEYUP/KEYDOWN KEYS DO NOT MATCH!!!", 0));
        label_2->setText(QApplication::translate("ConfigWindow", "Set Seperate PTT hotkeys for COMM1 and COMM2.  Must set the same key for the key up and Key down commands seperately.", 0));
        label_9->setText(QApplication::translate("ConfigWindow", "<html><head/><body><p>Author Jason Pharis CVW-3 Visit us at <a href=\"http://www.virtualcarrierwing3.com\">http://www.virtualcarrierwing3.com</a></p></body></html>", 0));
        label_7->setText(QApplication::translate("ConfigWindow", "Feature to add custom guard channels comming soon!", 0));
        checkBox->setText(QApplication::translate("ConfigWindow", "Enable 121.5", 0));
        label_8->setText(QApplication::translate("ConfigWindow", "Contact rollingrock16@gmail.com for support issues!", 0));
        label_6->setText(QApplication::translate("ConfigWindow", "Enable Guard channel 121.5 below:", 0));
    } // retranslateUi

};

namespace Ui {
    class ConfigWindow: public Ui_ConfigWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGWINDOW_H
