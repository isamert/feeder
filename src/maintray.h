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

    QString SPACE;

    void loadFromCache(const QString &notifyFeed = "");
    void addDefaultItems();
    void updateCache();
    void markFeedAsRead(const QString &feed);
    void markFeedAsUnread(const QString &feed);
    void markFeedItemAsRead(const QString &feed, const QString &itemName);

public slots:
    void reloadFromCache();
    void refreshAll();
    void openClickedItem(QAction *act);

    void markAllAsRead();
    void markAllAsUnread();
    void feedMenuClicked(QAction *act);

    void onFeedUpdated(const QString &feed);
};

#endif // MAINTRAY_H
