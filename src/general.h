#ifndef GENERAL_H
#define GENERAL_H

#include <QString>
#include <QDir>
#include <QSettings>
#include <QApplication>

class General
{
public:
    static QString randomString(int charlen);
    static QString combine(const QString &path1, const QString &path2);

    static QString xmlCachePath();
    static QString iconCachePath();
    static QString cleanCache();
};

#endif // GENERAL_H
