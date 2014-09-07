#ifndef GENERAL_H
#define GENERAL_H

#include <QString>
#include <QDir>
#include <QSettings>

class General
{
public:
    static QString randomString(int charlen);
    static QString combine(const QString &path1, const QString &path2);

    static QString xmlCachePath();
    static QString iconCachePath();
};

#endif // GENERAL_H
