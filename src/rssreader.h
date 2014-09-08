#ifndef RSSREADER_H
#define RSSREADER_H

#include <QObject>
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QDir>

class RssReader : public QObject
{
    Q_OBJECT

private:
    QDomDocument *document;

public:
    struct RssItem {
        QString title;
        QString link;
        QString description;
        QString pubDate;
    };

    struct RssImage {
        QString url;   /* image adress */
        QString link;  /* image href */
        QString title;
        QString description;
    };

    struct ChannelInfo {
        QString title;
        QString link;
        QString description;
        QString category;
        QString copyright;
        QString language;
    };

    explicit RssReader(QObject *parent = 0);
    ~RssReader();

    bool setDocument(const QString &xmlFilePath);
    bool setDocumentString(const QString &strDoc);
    ChannelInfo getChannelInfo();
    QList<RssItem> getRssItems();
    QList<RssImage> getRssImages();
    int totalCount();

signals:

public slots:
};

#endif // RSSREADER_H
