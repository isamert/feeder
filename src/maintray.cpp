#include "maintray.h"

MainTray::MainTray(QWidget *parent)
    : QSystemTrayIcon(parent)
{
    //TODO: show notifications
    //TODO: add light theme and dark theme icon
    //TODO: edit feeds

#ifdef Q_WS_X11
    SPACE = "\t";
#elif Q_OS_WIN32
    SPACE = "     ";
#endif

    this->menu = new Menu();
    connect(this->menu, SIGNAL(triggered(QAction*)), this, SLOT(openClickedItem(QAction*)));

    this->sd = new SettingsDialog();
    this->ad = new AboutDialog();
    connect(this->sd, SIGNAL(reloadFromCacheRequested()), this, SLOT(reloadFromCache()));
    this->setIcon(QIcon(":/images/feed-32x32.png"));

    this->setContextMenu(this->menu);

    QSettings set;
    set.beginGroup("General");
    int minute = set.value("invertal", 30).toInt();
    set.endGroup();

    this->refreshAll();

    this->timer = new QTimer();
    connect(this->timer, SIGNAL(timeout()), this, SLOT(refreshAll()));
    this->timer->start((1000 * 60) * minute);
}

MainTray::~MainTray() {
    delete this->menu;
    delete this->sd;
    delete this->ad;
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

    foreach (QString feed, General::getFeeds()) {
        set.beginGroup("Feed_" + feed);

        QString type = set.value("type", "").toString();
        QString title = set.value("title", "").toString();
        QString cache = set.value("cache", "").toString();
        QString icon = set.value("icon", "").toString();
        QString _readitems = set.value("readitems", "").toString();
        QStringList readitems =  _readitems == "" ? QStringList() : _readitems.split("|||");
        bool submenu = set.value("submenu", false).toBool();
        int limit = set.value("limit").toInt();

        //if(title.count() > maxLength)
        //    title.remove(maxLength, title.count() - maxLength).append("...");

        QMenu *mainAct = this->menu->addMenu(QIcon(icon), title);
        QString mainLink = "";

        if(type == "atom") {
            AtomReader ar;
            ar.setDocument(cache);

            mainLink = ar.getChannelInfo().link;

            QList<AtomReader::AtomItem> entries = ar.getAtomEntries();
            for(int i = 0; i < limit; ++i) {
                if(entries.count() > i) {
                    QString subTitle = entries[i].title;

                    QAction *act;

                    if(readitems.contains(subTitle))
                        continue;

                    if(subTitle.count() > maxLength)
                        subTitle.remove(maxLength, subTitle.count() - maxLength).append("...");

                    if(submenu)
                        act = mainAct->addAction(QIcon(), subTitle);
                    else
                        act = this->menu->addAction(QIcon(), SPACE + subTitle);

                    QStringList data; //(feedname, link)
                    data << feed << entries[i].link;
                    act->setData(data);

                    act->setToolTip(entries[i].title);

                    int unreadCount = readitems.count() == 0 ? ar.totalCount() - readitems.count():ar.totalCount() - readitems.count() + 1;
                    mainAct->setTitle(QString("(%1) " + title).arg(unreadCount));
                }
            }
        }
        else if(type == "rss") {
            RssReader rr;
            rr.setDocument(cache);

            mainLink = rr.getChannelInfo().link;

            QList<RssReader::RssItem> items = rr.getRssItems();
            for(int i = 0; i < limit; ++i) {
                if(items.count() > i) {
                    QString subTitle = items[i].title;

                    QAction *act;

                    if(readitems.contains(subTitle))
                        continue;

                    if(subTitle.count() > maxLength)
                        subTitle.remove(maxLength, subTitle.count() - maxLength).append("...");

                    if(submenu)
                        act = mainAct->addAction(QIcon(), subTitle);
                    else
                        act = this->menu->addAction(QIcon(), SPACE + subTitle);

                    QStringList data; //(feedname, link)
                    data << feed << items[i].link;
                    act->setData(data);

                    act->setToolTip(items[i].title);

                    int unreadCount = readitems.count() == 0 ? rr.totalCount() - readitems.count():rr.totalCount() - readitems.count() + 1;
                    mainAct->setTitle(QString("(%1) " + title).arg(unreadCount));
                }
            }
        }

        if(!mainAct->actions().isEmpty())
            mainAct->addSeparator();

        QAction *actGoto = mainAct->addAction(trUtf8("Open"));
        actGoto->setData(mainLink);
        mainAct->addSeparator();
        QAction *actRead = mainAct->addAction(trUtf8("Mark as read"));
        QAction *actUnread = mainAct->addAction(trUtf8("Mark as unread"));
        actRead->setData(QStringList("__READ__" + feed));
        actUnread->setData(QStringList("__UNREAD__" + feed));

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
    this->menu->addAction(trUtf8("Add Feed Source"), this->sd, SLOT(showAddDialog()));
    this->menu->addAction(trUtf8("Settings"), this->sd, SLOT(show()));
    this->menu->addAction(trUtf8("About"), this->ad, SLOT(show()));
    this->menu->addAction(trUtf8("Quit"), qApp, SLOT(quit()));
}

void MainTray::updateCache() {
    General::cleanXmlCache();
    QStringList feeds = General::getFeeds();
    if(!feeds.isEmpty()) {
        foreach (QString feed, feeds) {
            FeedSource *fs;
            fs = new FeedSource();
            fs->updateFeed(feed);
            connect(fs, SIGNAL(feedUpdated(QString)), this, SLOT(reloadFromCache()));
        }
    }
    else
        this->reloadFromCache();
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
    foreach (QString feed, General::getFeeds())
        this->markFeedAsRead(feed);

    this->reloadFromCache();
}

void MainTray::markAllAsUnread() {
    foreach (QString feed, General::getFeeds())
        this->markFeedAsUnread(feed);

    this->reloadFromCache();
}

void MainTray::markFeedFromAction(QAction *act) {
    QStringList data = act->data().toStringList();
    QString action = data[0];

    if(action.startsWith("__READ__"))
        this->markFeedAsRead(action.remove("__READ__"));
    else if(action.startsWith("__UNREAD__"))
        this->markFeedAsUnread(action.remove("__UNREAD__"));

    this->reloadFromCache();
}

void MainTray::markFeedItemAsRead(const QString &feed, const QString &itemName) {
    QSettings set;
    set.beginGroup("Feed_" + feed);

    QString readlist = set.value("readitems", QStringList()).toString();
    readlist += itemName + "|||";
    set.setValue("readitems", readlist);
    set.endGroup();

    this->reloadFromCache();
}

void MainTray::openClickedItem(QAction *act) {
    const QStringList data = act->data().toStringList();

    if(data.count() <= 1)
        return;

    QString url = data[1];
    QString feed = data[0];

    if(!feed.startsWith("__READ__") || !feed.startsWith("__UNREAD__")) {
        QDesktopServices::openUrl(QUrl(url));

        QString fullName = act->toolTip();
        this->markFeedItemAsRead(feed, fullName);
    }
}

