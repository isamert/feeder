#include "general.h"

QString General::combine(const QString &path1, const QString &path2) {
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}

QString General::randomString(int charlen) {
    QString str;
    static QString alphanumeric = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < charlen; ++i)
        str[i] = alphanumeric[rand() / (RAND_MAX / alphanumeric.count())];

    return str;
}

QString General::xmlCachePath() {
    QString cache = combine(QDir::homePath(), ".cache/" + qApp->applicationName() + "/xml");
    if(!QFile::exists(cache))
        QDir::root().mkpath(cache);
    return cache;
}

QString General::iconCachePath() {
    QString cache = combine(QDir::homePath(), ".cache/" + qApp->applicationName() + "/icon");
    if(!QFile::exists(cache))
        QDir::root().mkpath(cache);
    return cache;
}

QString General::autostartFile() {
    return combine(combine(QDir::homePath(), ".config/autostart"), "feeder.desktop");
}

void General::cleanXmlCache() {
    QDir dir(xmlCachePath());
    QStringList paths = dir.entryList(QDir::NoDotAndDotDot | QDir::Files);
    foreach (QString path, paths) {
        QFile::remove(combine(xmlCachePath(), path));
    }
}

void General::cleanIconCache() {
    QDir dir(iconCachePath());
    QStringList paths = dir.entryList(QDir::NoDotAndDotDot | QDir::Files);
    foreach (QString path, paths)
        QFile::remove(combine(iconCachePath(), path));
}

QStringList General::getFeeds() {
    QSettings set;
    set.beginGroup("Feeds");
    QStringList feeds = set.childKeys();
    set.endGroup();

    return feeds;
}
