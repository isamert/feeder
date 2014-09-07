#ifndef ATOMREADER_H
#define ATOMREADER_H

#include <QObject>
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QDir>

class AtomReader : public QObject
{
    Q_OBJECT

private:
    QDomDocument *document;

public:    
    struct AtomItem {
        QString title;
        QString link;
        QString id;
        QString published;
        QString updated;
        QHash<QString, QString> authors;
        QString content;
        QString summary;
    };

    struct ChannelInfo {
        QString title;
        QString subtitle;
        QString updated;
        QString rights;
        QString link;
    };

    explicit AtomReader(QObject *parent = 0);
    bool setDocument(const QString &xmlFilePath);
    bool setDocumentString(const QString &strDoc);
    ChannelInfo getChannelInfo();
    QList<AtomItem> getAtomEntries();
    int totalCount();

signals:

public slots:

};

#endif // ATOMREADER_H
