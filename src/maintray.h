#ifndef MAINTRAY_H
#define MAINTRAY_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>
#include <QTimer>
#include <QDesktopServices>

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
    void markFeedAsRead(const QString &feed);
    void markFeedAsUnread(const QString &feed);

public slots:
    void reloadFromCache();
    void refreshAll();
    void openClickedItem(QAction *act);

    void markAllAsRead();
    void markAllAsUnread();
    void markFeedFromAction(QAction *act);
};

#endif // MAINTRAY_H
