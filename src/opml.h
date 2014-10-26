#ifndef OPML_H
#define OPML_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDomDocument>

#include "feedsource.h"

class Opml : public QObject
{
    Q_OBJECT
public:
    explicit Opml(QObject *parent = 0);

    FeedSource *fs;
    bool importOpml(const QString &filePath);
    void exportOpml(const QString &filePath);

signals:

public slots:

};

#endif // OPML_H
