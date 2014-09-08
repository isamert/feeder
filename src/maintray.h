#ifndef MAINTRAY_H
#define MAINTRAY_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>
#include <QTimer>

#include "menu.h"
#include "rssreader.h"
#include "atomreader.h"
#include "settingsdialog.h"
#include "aboutdialog.h"
#include "feedsource.h"

class MainTray : public QSystemTrayIcon
{
    Q_OBJECT

public:
    MainTray(QWidget *parent = 0);
    ~MainTray();

    Menu *menu;
    SettingsDialog *sd;
    AboutDialog *ad;
    QTimer *timer;

    void loadFromCache();
    void addDefaultItems();
    void updateCache();

public slots:
    void reloadFromCache();
    void refreshAll();
    void openClickedItem(QAction *act);
};

#endif // MAINTRAY_H
