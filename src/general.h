#ifndef GENERAL_H
#define GENERAL_H

#include <QString>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QApplication>
#include <QDebug>

class General
{
public:
    static QString randomString(int charlen);
    static QString combine(const QString &path1, const QString &path2);

    static QString xmlCachePath();
    static QString iconCachePath();
    static QString autostartFile();
    static void cleanXmlCache();
    static void cleanIconCache();

    static QStringList getFeeds();
    static bool addFeed(const QString &url, const QString &title, const QString &type, const QString &cache, const QString catname, const QString &limit, bool notifications, bool submenu, const QString ico);
    static void addCategory(const QString &str);
    static void sendNotify(const QString &title, const QString &message, const int second = 3, const QString image = "");
};

#endif // GENERAL_H
