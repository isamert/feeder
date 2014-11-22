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
    QString currentUpdateFeed;
    QNetworkAccessManager *manager;
    QNetworkAccessManager *iconmanager;
    QNetworkAccessManager *updatemanager;

public:
    enum FileType {
        AtomFile = 0,
        RssFile = 1,
        NoType = 2,
    };

    explicit FeedSource(QObject *parent = 0, const QUrl &feedUrl = QUrl());
    ~FeedSource();

    QString title;
    QString category;
    QString htmlUrl;
    QString xmlUrl;
    QString url;
    QUrl feedUrl;

    bool downloadFeed();
    bool downloadFeed(const QString &url);
    bool downloadFeed(const QUrl &feedUrl);
    bool downloadIcon();
    bool updateFeed(const QString &feed);
    static FileType fileType(const QString &filePath);

signals:
    void feedDownloadCompleted(const QString &filePath);
    void iconDownloadCompleted(const QString &iconPath);
    void feedUpdated(const QString &feed);

private slots:
    void feedDownloaded(QNetworkReply *reply);
    void iconDownloaded(QNetworkReply *reply);
    void doUpdate(QNetworkReply *reply);

public slots:

};

#endif // FEEDSOURCE_H
