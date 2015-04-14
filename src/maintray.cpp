#include "maintray.h"

MainTray::MainTray(QWidget *parent)
    : QSystemTrayIcon(parent)
{
    //TODO: add light theme and dark theme icon
    //TODO: opml ile eklenen feedleri Feeds kısmına ekleyiver
    //FIXME: open menu when clicked on windows

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

    this->setContextMenu(this->menu);

    QSettings set;
    set.beginGroup("General");
    int minute = set.value("invertal", 30).toInt();
    QString iconName = set.value("icon", "feeder-standart").toString();
    this->setIcon(QIcon::fromTheme(iconName, QIcon(":/images/" + iconName +".png")));
    set.endGroup();

    this->refreshAll();

    this->timer = new QTimer();
    connect(this->timer, SIGNAL(timeout()), this, SLOT(refreshAll()));
    this->timer->start((1000 * 60) * minute);
}

MainTray::~MainTray() {
    //delete this->menu;
    delete this->sd;
    delete this->ad;
}

void MainTray::reloadFromCache() {
    this->loadFromCache();
    this->addDefaultItems();
}

void MainTray::loadFromCache(const QString &notifyFeed) {
    this->menu->clear();
    QStringList listNotify;

    QSettings set;
    set.beginGroup("General");
    const bool subcategory = set.value("subcategory", false).toBool();
    int maxLength = set.value("maxlength", 35).toInt();
    set.endGroup();

    //add categories
    QHash<QString, QMenu*> catmenus;
    if(subcategory) {
        set.beginGroup("Categories");
        QStringList cats = set.childKeys();
        set.endGroup();
        foreach (QString cat, cats) {
            if(!(cat.isEmpty() || cat == "" || cat == " "))
                catmenus[cat] = this->menu->addMenu(cat); //TODO: add icon to categories
        }
    }

    foreach (QString feed, General::getFeeds()) {
        bool subcategory_temp = subcategory;

        set.beginGroup("Feed_" + feed);

        QString type = set.value("type", "").toString();
        QString title = set.value("title", "").toString();
        QString cache = set.value("cache", "").toString();
        QString icon = set.value("icon", "").toString();
        QString category = set.value("category", "").toString();
        bool notifications = set.value("notifications", false).toBool();
        QString _readitems = set.value("readitems", "").toString();
        QStringList readitems =  _readitems == "" ? QStringList() : _readitems.split("|||");
        bool submenu = set.value("submenu", false).toBool();
        int limit = set.value("limit").toInt();

        if(category.isEmpty() || category == "" || category == " ")
            subcategory_temp = false;

        QMenu *mainAct;
        if(subcategory_temp)
            mainAct = catmenus[category]->addMenu(QIcon(icon), title);
        else
            mainAct = this->menu->addMenu(QIcon(icon), title);

        QString mainLink = "";

        if(type == "atom") {
            AtomReader ar;
            ar.setDocument(cache);

            mainLink = ar.getChannelInfo().link;

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
                    else {
                        if(subcategory_temp)
                            act = catmenus[category]->addAction(QIcon(), SPACE + subTitle);
                        else
                            act = this->menu->addAction(QIcon(), SPACE + subTitle);
                    }

                    QStringList data; //(feedname, link)
                    data << feed << entries[i].link;
                    act->setData(data);

                    act->setToolTip(entries[i].title);

                    int unreadCount = readitems.count() == 0 ? ar.totalCount() - readitems.count():ar.totalCount() - readitems.count() + 1;
                    mainAct->setTitle(QString("(%1) " + title).arg(unreadCount));

                    if(notifications && feed == notifyFeed && !notifyFeed.isEmpty())
                        listNotify.append(QString("[%1] - %2").arg(title, subTitle));
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

                    if(readitems.contains(subTitle))
                        continue;

                    if(subTitle.count() > maxLength)
                        subTitle.remove(maxLength, subTitle.count() - maxLength).append("...");

                    QAction *act;

                    if(submenu)
                        act = mainAct->addAction(QIcon(), subTitle);
                    else {
                        if(subcategory_temp)
                            act = catmenus[category]->addAction(QIcon(), SPACE + subTitle);
                        else
                            act = this->menu->addAction(QIcon(), SPACE + subTitle);
                    }

                    QStringList data; //(feedname, link)
                    data << feed << items[i].link;
                    act->setData(data);

                    act->setToolTip(items[i].title);

                    int unreadCount = readitems.count() == 0 ? rr.totalCount() - readitems.count():rr.totalCount() - readitems.count() + 1;
                    mainAct->setTitle(QString("(%1) " + title).arg(unreadCount));

                    if(notifications && feed == notifyFeed && !notifyFeed.isEmpty())
                        listNotify.append(QString("[%1] - %2").arg(title, subTitle));
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
        mainAct->addSeparator();
        QAction *actEdit = mainAct->addAction(trUtf8("Edit"));

        actRead->setData(QStringList("__READ__" + feed));
        actUnread->setData(QStringList("__UNREAD__" + feed));
        actEdit->setData(QStringList("__EDIT__" + feed));

        connect(mainAct, SIGNAL(triggered(QAction*)), this, SLOT(feedMenuClicked(QAction*)));

        this->menu->addSeparator();

        if(subcategory_temp)
            catmenus[category]->addSeparator();

        set.endGroup();
    }

    QString strNotify = listNotify.join("\n").replace('"', "").replace("(", "").replace(")", "").replace("'", "");
    if(!strNotify.isEmpty())
        General::sendNotify(trUtf8("Feeder"), strNotify);
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
            connect(fs, SIGNAL(feedUpdated(QString)), this, SLOT(onFeedUpdated(QString)));
        }
    }
    else
        this->reloadFromCache();
}

void MainTray::onFeedUpdated(const QString &feed) {
    this->loadFromCache(feed);
    this->addDefaultItems();
    //do some amazing stuff
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

void MainTray::feedMenuClicked(QAction *act) {
    QStringList data = act->data().toStringList();
    QString action = data[0];

    if(action.startsWith("__READ__"))
        this->markFeedAsRead(action.remove("__READ__"));
    else if(action.startsWith("__UNREAD__"))
        this->markFeedAsUnread(action.remove("__UNREAD__"));
    else if(action.startsWith("__EDIT__"))
        this->sd->openEditFeedTab(action.remove("__EDIT__"));


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

