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
    QString cache = combine(QDir::homePath(), ".cache/indicator-rss/xml");
    if(!QFile::exists(cache))
        QDir::root().mkpath(cache);
    return cache;
}

QString General::iconCachePath() {
    QString cache = combine(QDir::homePath(), ".cache/indicator-rss/icon");
    if(!QFile::exists(cache))
        QDir::root().mkpath(cache);
    return cache;
}
