#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include <QMenu>
#include <QHelpEvent>
#include <QToolTip>

class Menu : public QMenu
{
    Q_OBJECT
public:
    explicit Menu(QWidget *parent = 0);

signals:

public slots:

protected:
    bool event(QEvent *e);
};

#endif // MENU_H
