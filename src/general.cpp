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
    set.beginGroup("FeedOrder");
    QStringList feeds_ = set.childKeys();
    feeds_.sort();

    QStringList feeds;
    foreach (QString feed, feeds_)
        feeds.append(set.value(feed).toString());
    set.endGroup();
    return feeds;
}

bool General::addFeed(const QString &url, const QString &title, const QString &type, const QString &cache, const QString catname, const QString &limit, bool notifications, bool submenu, const QString icon) {
    QString group = QString(title).replace(" ", "").toUpper();

    foreach (QString var, General::getFeeds()) {
        if(var == group) {
            return false;
        }
    }

    addCategory(catname); //add the category if does not exists

    QSettings set;

    set.beginGroup("Feeds");
    set.setValue(group, title);
    set.endGroup();

    //add to order
    set.beginGroup("FeedOrder");
    QStringList orders = set.childKeys();
    orders.sort();

    int itemOrder;
    if(orders.isEmpty())
        itemOrder = 0;
    else
        itemOrder = orders.last().toInt() + 1;

    set.setValue(QString::number(itemOrder), group);
    set.endGroup();

    set.beginGroup("Feed_" + group);
    set.setValue("url", url);
    set.setValue("title", title);
    set.setValue("type", type);
    set.setValue("cache", cache);
    set.setValue("category", catname);
    set.setValue("limit", limit);
    set.setValue("notifications", notifications);
    set.setValue("submenu", submenu);
    set.setValue("icon", icon);
    set.endGroup();

    return true;
}

void General::addCategory(const QString &str) {
    QSettings set;
    set.beginGroup("Categories");
    if(!set.childKeys().contains(str))
        set.setValue(str, "");
    set.endGroup();
}

void General::sendNotify(const QString &title, const QString &message, const int second /* = 3 */, const QString image /* = "" */) {
#ifdef Q_WS_X11
    QString imageCommand = "";
    if(!image.isEmpty() && QFile::exists(image))
        imageCommand = " -i " + image;

    int ms = second * 1000;

    QString cmd = "notify-send '" + title + "' '" + message + "' -t " +  QString::number(ms) + imageCommand;
    std::string stdstr = cmd.toUtf8().constData();
    system(stdstr.c_str());
#elif Q_OS_WIN32
    //TODO: do stuff here
#endif
}

