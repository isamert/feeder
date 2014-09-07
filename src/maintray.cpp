#include "maintray.h"

MainTray::MainTray(QWidget *parent)
    : QSystemTrayIcon(parent)
{
    this->sd = new SettingsDialog();
    connect(this->sd, SIGNAL(reloadRequested()), this, SLOT(reloadFromCache()));
    //this->sd->show();

    this->menu = new Menu();

    this->setIcon(QIcon::fromTheme("rss"));
    this->setContextMenu(this->menu);

    this->loadItems();
    this->addDefaultItems();
}

MainTray::~MainTray() {
    delete this->menu;
}


void MainTray::loadItems() {
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

        QString type = set.value("type").toString();
        QString title = set.value("title").toString();
        QString cache = set.value("cache").toString();
        QString icon = set.value("icon").toString();
        int limit = set.value("limit").toInt();

        QAction *mainAct = this->menu->addAction(QIcon(icon), title, this, SLOT(openClickedItem()));

        if(type == "atom") {
            AtomReader ar;
            ar.setDocument(cache);

            QList<AtomReader::AtomItem> entries = ar.getAtomEntries();
            for(int i = 0; i < limit; ++i) {
                if(entries.count() > i) {
                    QString subTitle = entries[i].title;
                    if(subTitle.count() > maxLength)
                        subTitle.remove(maxLength, subTitle.count() - maxLength).append("...");
                    QAction *act = this->menu->addAction(QIcon(), "\t" + subTitle, this, SLOT(openClickedItem()));
                    act->setToolTip(entries[i].content);
                    mainAct->setText(QString("(%1) " + title).arg(ar.totalCount()));
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
                        subTitle.remove(maxLength, subTitle.count() - maxLength).append("...");;
                    QAction *act = this->menu->addAction(QIcon(), "\t" + subTitle, this, SLOT(openClickedItem()));
                    act->setToolTip(items[i].description);
                    mainAct->setText(QString("(%1) " + title).arg(rr.totalCount()));
                }
            }
        }
        this->menu->addSeparator();
        set.endGroup();
    }
}

void MainTray::addDefaultItems() {
    this->menu->addAction(trUtf8("Refresh All"), this->sd, SLOT(show()));
    this->menu->addAction(trUtf8("Settings"), this->sd, SLOT(show()));
    this->menu->addAction(trUtf8("About"), this, SLOT(about()));
    this->menu->addAction(trUtf8("Quit"), qApp, SLOT(quit()));
}

void MainTray::reloadFromCache() {
    this->loadItems();
}

void MainTray::reloadAll() {

}

void MainTray::openClickedItem() {

}
