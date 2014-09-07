#include "feedsource.h"

FeedSource::FeedSource(QObject *parent, const QUrl &feedUrl) :
    QObject(parent)
{
    this->manager = new QNetworkAccessManager();
    this->iconmanager = new QNetworkAccessManager();
    connect(this->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(feedDownloaded(QNetworkReply*)));
    connect(this->iconmanager, SIGNAL(finished(QNetworkReply*)), this, SLOT(iconDownloaded(QNetworkReply*)));

    this->feedUrl = feedUrl;
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
    qDebug() << url;
    this->iconmanager->get(QNetworkRequest(QUrl(url)));
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
