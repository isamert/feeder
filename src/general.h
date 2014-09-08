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
};

#endif // GENERAL_H
