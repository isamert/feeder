#include "maintray.h"

MainTray::MainTray(QWidget *parent)
    : QSystemTrayIcon(parent)
{
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

    set.beginGroup("Feeds");
    QStringList feeds = set.childKeys();
    set.endGroup();

    foreach (QString feed, feeds) {
        set.beginGroup("Feed_" + feed);

        QString type = set.value("type", "").toString();
        QString title = set.value("title", "").toString();
        QString cache = set.value("cache", "").toString();
        QString icon = set.value("icon", "").toString();
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

                    if(subTitle.count() > maxLength)
                        subTitle.remove(maxLength, subTitle.count() - maxLength).append("...");

                    QAction *act;

                    if(submenu)
                        act = mainAct->addAction(QIcon(), "\t" + subTitle);
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

                    if(subTitle.count() > maxLength)
                        subTitle.remove(maxLength, subTitle.count() - maxLength).append("...");

                    QAction *act;

                    if(submenu)
                        act = mainAct->addAction(QIcon(), "\t" + subTitle);
                    else
                        act = this->menu->addAction(QIcon(), "\t" + subTitle);

                    act->setToolTip(items[i].description);
                    act->setData(items[i].link);
                    mainAct->setTitle(QString("(%1) " + title).arg(rr.totalCount()));
                }
            }
        }
        this->menu->addSeparator();
        set.endGroup();
    }
}

void MainTray::addDefaultItems() {
    this->menu->addAction(trUtf8("Refresh All"), this, SLOT(refreshAll()));
    this->menu->addAction(trUtf8("Settings"), this->sd, SLOT(show()));
    this->menu->addAction(trUtf8("About"), this->ad, SLOT(show()));
    this->menu->addAction(trUtf8("Quit"), qApp, SLOT(quit()));
}

void MainTray::updateCache() {
    General::cleanXmlCache();

    QSettings set;
    set.beginGroup("Feeds");
    QStringList feeds = set.childKeys();
    set.endGroup();

    foreach (QString feed, feeds) {
        FeedSource *fs;
        fs = new FeedSource();
        fs->updateFeed(feed);
        connect(fs, SIGNAL(feedUpdated(QString)), this, SLOT(reloadFromCache()));
    }
}

void MainTray::refreshAll() {
    this->updateCache();
}

void MainTray::openClickedItem(QAction *act) {
    QDesktopServices::openUrl(QUrl(act->data().toString()));
}




