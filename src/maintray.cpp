#include "maintray.h"

MainTray::MainTray(QWidget *parent)
    : QSystemTrayIcon(parent)
{
    //TODO: order feed list
    //TODO: show notifications


    this->sd = new SettingsDialog();
    this->ad = new AboutDialog();
    connect(this->sd, SIGNAL(reloadRequested()), this, SLOT(reloadFromCache()));

    this->menu = new Menu();
    connect(this->menu, SIGNAL(triggered(QAction*)), this, SLOT(openClickedItem(QAction*)));

    this->setIcon(QIcon(":/images/feed-32x32.png"));
    this->setContextMenu(this->menu);

    QSettings set;
    set.beginGroup("General");
    int minute = set.value("invertal", 30).toInt();
    set.endGroup();

    this->timer = new QTimer();
    connect(this->timer, SIGNAL(timeout()), this, SLOT(refreshAll()));
    this->timer->start((1000 * 60) * minute);

    this->refreshAll();
}

MainTray::~MainTray() {
    delete this->menu;
    delete this->sd;
    delete this->ad;
}

QStringList MainTray::getFeeds() {
    QSettings set;
    set.beginGroup("Feeds");
    QStringList feeds = set.childKeys();
    set.endGroup();

    return feeds;
}

void MainTray::reloadFromCache() {
    this->loadFromCache();
    this->addDefaultItems();
}

void MainTray::loadFromCache() {
    this->menu->clear();

    QSettings set;
    set.beginGroup("General");
    set.value("invertal", 30).toInt();
    int maxLength = set.value("maxlength", 35).toInt();
    set.endGroup();

    foreach (QString feed, this->getFeeds()) {
        set.beginGroup("Feed_" + feed);

        QString type = set.value("type", "").toString();
        QString title = set.value("title", "").toString();
        QString cache = set.value("cache", "").toString();
        QString icon = set.value("icon", "").toString();
        QStringList readitems = set.value("readitems", "").toString().split("|||");
        bool submenu = set.value("submenu", false).toBool();
        int limit = set.value("limit").toInt();

        QMenu *mainAct = this->menu->addMenu(QIcon(icon), title);

        if(type == "atom") {
            AtomReader ar;
            ar.setDocument(cache);

            QList<AtomReader::AtomItem> entries = ar.getAtomEntries();
            for(int i = 0; i < limit; ++i) {
                if(entries.count() > i) {
                    QString subTitle = entries[i].title;

                    if(readitems.contains(subTitle))
                        continue;

                    if(subTitle.count() > maxLength)
                        subTitle.remove(maxLength, subTitle.count() - maxLength).append("...");

                    QAction *act;

                    if(submenu)
                        act = mainAct->addAction(QIcon(), subTitle);
                    else
                        act = this->menu->addAction(QIcon(), "\t" + subTitle);

                    act->setToolTip(entries[i].content);
                    act->setData(entries[i].link);
                    mainAct->setTitle(QString("(%1) " + title).arg(ar.totalCount()));
                }
            }
        }
        else if(type == "rss") {
            RssReader rr;
            rr.setDocument(cache);

            QList<RssReader::RssItem> items = rr.getRssItems();
            for(int i = 0; i < limit; ++i) {
                if(items.count() > i) {
                    QString subTitle = items[i].title;

                    if(readitems.contains(subTitle))
                        continue;

                    if(subTitle.count() > maxLength)
                        subTitle.remove(maxLength, subTitle.count() - maxLength).append("...");

                    QAction *act;

                    if(submenu)
                        act = mainAct->addAction(QIcon(), subTitle);
                    else
                        act = this->menu->addAction(QIcon(), "\t" + subTitle);

                    act->setToolTip(items[i].description);
                    act->setData(items[i].link);
                    mainAct->setTitle(QString("(%1) " + title).arg(rr.totalCount()));
                }
            }
        }

        if(!mainAct->actions().isEmpty())
            mainAct->addSeparator();

        QAction *actRead = mainAct->addAction(trUtf8("Mark as read"));
        QAction *actUnread = mainAct->addAction(trUtf8("Mark as unread"));
        actRead->setData("__READ__" + feed);
        actUnread->setData("__UNREAD__" + feed);

        connect(mainAct, SIGNAL(triggered(QAction*)), this, SLOT(markFeedFromAction(QAction*)));

        this->menu->addSeparator();
        set.endGroup();
    }
}

void MainTray::addDefaultItems() {
    this->menu->addAction(trUtf8("Refresh all"), this, SLOT(refreshAll()));
    this->menu->addAction(trUtf8("Mark all as read"), this, SLOT(markAllAsRead()));
    this->menu->addAction(trUtf8("Mark all as unread"), this, SLOT(markAllAsUnread()));
    this->menu->addSeparator();
    this->menu->addAction(trUtf8("Settings"), this->sd, SLOT(show()));
    this->menu->addAction(trUtf8("About"), this->ad, SLOT(show()));
    this->menu->addAction(trUtf8("Quit"), qApp, SLOT(quit()));
}

void MainTray::updateCache() {
    General::cleanXmlCache();

    foreach (QString feed, this->getFeeds()) {
        FeedSource *fs;
        fs = new FeedSource();
        fs->updateFeed(feed);
        connect(fs, SIGNAL(feedUpdated(QString)), this, SLOT(reloadFromCache()));
    }
}

void MainTray::refreshAll() {
    this->updateCache();
}

void MainTray::markFeedAsRead(const QString &feed) {
    QSettings set;
    set.beginGroup("Feed_" + feed);
    QString cache = set.value("cache", "").toString();
    QString type = set.value("type", "").toString();
    int limit = set.value("limit", 10).toInt();
    QString readlist;

    if(type == "rss") {
        RssReader rr;
        rr.setDocument(cache);

        QList<RssReader::RssItem> items = rr.getRssItems();
        for(int i = 0; i < limit; ++i) {
            if(items.count() > i)
                readlist += items[i].title + "|||";
        }
    }
    else if(type == "atom") {
        AtomReader ar;
        ar.setDocument(cache);

        QList<AtomReader::AtomItem> entries = ar.getAtomEntries();
        for(int i = 0; i < limit; ++i) {
            if(entries.count() > i)
                readlist += entries[i].title + "|||";
        }
    }
    set.setValue("readitems", readlist);
    set.endGroup();
}

void MainTray::markFeedAsUnread(const QString &feed) {
    QSettings set;
    set.beginGroup("Feed_" + feed);
    set.remove("readitems");
    set.endGroup();
}

void MainTray::markAllAsRead() {
    foreach (QString feed, this->getFeeds())
        this->markFeedAsRead(feed);

    this->reloadFromCache();
}

void MainTray::markAllAsUnread() {
    foreach (QString feed, this->getFeeds())
        this->markFeedAsUnread(feed);

    this->reloadFromCache();
}

void MainTray::markFeedFromAction(QAction *act) {
    QString data = act->data().toString();

    if(data.startsWith("__READ__"))
        this->markFeedAsRead(data.remove("__READ__"));
    else if(data.startsWith("__UNREAD__"))
        this->markFeedAsUnread(data.remove("__UNREAD__"));

    this->reloadFromCache();
}

void MainTray::openClickedItem(QAction *act) {
    QString data = act->data().toString();

    if(!data.startsWith("__READ__") || !data.startsWith("__UNREAD__"))
        QDesktopServices::openUrl(QUrl(act->data().toString()));
}

