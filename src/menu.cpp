#include "menu.h"
#include <QDebug>

Menu::Menu(QWidget *parent) :
    QMenu(parent)
{}

bool Menu::event(QEvent * e) {
    //FIXME: show tooltips
    /*
    const QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);

    if (helpEvent->type() == QEvent::ToolTip ||
        helpEvent->type() == QEvent::StatusTip &&
        this->activeAction() != 0) {
        qDebug() << "asdasd";
        QToolTip::showText(helpEvent->globalPos(), this->activeAction()->toolTip());
        return true;
    }
    else
        QToolTip::hideText();
    */
    return QMenu::event(e);
}
