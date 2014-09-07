#ifndef FEEDSOURCE_H
#define FEEDSOURCE_H

#include <QObject>
#include <QDebug>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>

#include "general.h"

class FeedSource : public QObject
{
    Q_OBJECT

private:
    QUrl feedUrl;
    QNetworkAccessManager *manager;
    QNetworkAccessManager *iconmanager;

public:
    enum FileType {
        AtomFile = 0,
        RssFile = 1,
        NoType = 2,
    };

    explicit FeedSource(QObject *parent = 0, const QUrl &feedUrl = QUrl());
    bool downloadFeed();
    bool downloadFeed(const QString &url);
    bool downloadFeed(const QUrl &feedUrl);
    bool downloadIcon();
    static FileType fileType(const QString &filePath);

signals:
    void feedDownloadCompleted(const QString &filePath);
    void iconDownloadCompleted(const QString &iconPath);

private slots:
    void feedDownloaded(QNetworkReply *reply);
    void iconDownloaded(QNetworkReply *reply);

public slots:
};

#endif // FEEDSOURCE_H
