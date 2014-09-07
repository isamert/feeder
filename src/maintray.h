#ifndef MAINTRAY_H
#define MAINTRAY_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>

#include "menu.h"
#include "rssreader.h"
#include "atomreader.h"
#include "settingsdialog.h"

class MainTray : public QSystemTrayIcon
{
    Q_OBJECT

public:
    MainTray(QWidget *parent = 0);
    ~MainTray();

    Menu *menu;
    SettingsDialog *sd;

    void loadItems();
    void addDefaultItems();

public slots:
    void reloadFromCache();
    void reloadAll();
    void openClickedItem();
};

#endif // MAINTRAY_H
