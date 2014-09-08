#include "feedsource.h"

FeedSource::FeedSource(QObject *parent, const QUrl &feedUrl) :
    QObject(parent)
{
    this->manager = new QNetworkAccessManager();
    this->iconmanager = new QNetworkAccessManager();
    this->updatemanager = new QNetworkAccessManager();

    connect(this->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(feedDownloaded(QNetworkReply*)));
    connect(this->iconmanager, SIGNAL(finished(QNetworkReply*)), this, SLOT(iconDownloaded(QNetworkReply*)));
    connect(this->updatemanager, SIGNAL(finished(QNetworkReply*)), this, SLOT(doUpdate(QNetworkReply*)));

    this->feedUrl = feedUrl;
}
FeedSource::~FeedSource() {
    delete this->manager;
    delete this->iconmanager;
    delete this->updatemanager;
}


bool FeedSource::downloadFeed() {
    return this->downloadFeed(this->feedUrl);
}

bool FeedSource::downloadFeed(const QString &url) {
    this->feedUrl = QUrl(url);
    return this->downloadFeed(QUrl(url));
}

bool FeedSource::downloadFeed(const QUrl &feedUrl) {
    this->feedUrl = feedUrl;
    this->manager->get(QNetworkRequest(feedUrl));
    return true;
}

bool FeedSource::downloadIcon() {
    QString url = "http://www.google.com/s2/favicons?domain=" + this->feedUrl.toString();
    this->iconmanager->get(QNetworkRequest(QUrl(url)));
    return true;
}

bool FeedSource::updateFeed(const QString &feed) {
    this->currentUpdateFeed = feed;

    QSettings set;
    set.beginGroup("Feed_" + this->currentUpdateFeed);
    QUrl url = QUrl(set.value("url").toString());
    set.endGroup();

    this->updatemanager->get(QNetworkRequest(url));
    return true;
}


void FeedSource::feedDownloaded(QNetworkReply *reply) {
    QString filePath = General::combine(General::xmlCachePath(), General::randomString(25));
    QFile file(filePath);

    if(!file.open(QIODevice::WriteOnly))
        return;

    file.write(reply->readAll());
    file.close();

    reply->deleteLater();
    emit this->feedDownloadCompleted(filePath);
}

void FeedSource::iconDownloaded(QNetworkReply *reply) {
    QString filePath = General::combine(General::iconCachePath(), General::randomString(25));
    QFile file(filePath);

    if(!file.open(QIODevice::WriteOnly))
        return;

    file.write(reply->readAll());
    file.close();

    reply->deleteLater();
    emit this->iconDownloadCompleted(filePath);
}

void FeedSource::doUpdate(QNetworkReply *reply) {
    QString filePath = General::combine(General::xmlCachePath(), General::randomString(25));
    QFile file(filePath);

    if(!file.open(QIODevice::WriteOnly))
        return;

    file.write(reply->readAll());
    file.close();

    QSettings set;
    set.beginGroup("Feed_" + this->currentUpdateFeed);
    set.setValue("cache", filePath);
    set.endGroup();

    emit this->feedUpdated(this->currentUpdateFeed);
    reply->deleteLater();
    this->deleteLater();
}

FeedSource::FileType FeedSource::fileType(const QString &filePath) {
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return NoType;

    QTextStream ts(&file);
    while (!ts.atEnd()) {
        QString line = ts.readLine();
        if(line.contains("<channel")) {
            file.close();
            return RssFile;
        }
        else if(line.contains("<feed")) {
            file.close();
            return AtomFile;
        }
    }

    file.close();
    return NoType;
}
